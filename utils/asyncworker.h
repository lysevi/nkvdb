#pragma once

#include <thread>
#include <condition_variable>
#include <queue>
#include <mutex>
#include <assert.h>
#include <atomic>

namespace utils {

// look usage example in utils_test.cpp
template <class T> class AsyncWorker {
public:
  void add(T data) {
    std::unique_lock<std::mutex> lock(m_add_lock);
    m_data.push(data);
    m_have_data.notify_one();
  }

  virtual void call(const T data) { assert(false); }

  void start() {
    m_stop_flag = false;
    m_thread = std::thread(&AsyncWorker<T>::_thread_func, this);
    assert(m_thread.joinable());
  }

  void kill() {
    m_stop_flag = true;
    while (m_thread_work) {
      m_have_data.notify_one();
    }

    m_thread.join();
  }

  // whait, while all works done and stop thread.
  void stop() {
    if (!stoped()) {
      while (true) {
        if (!isBusy()) {
          break;
        }
      }
      kill();
    }
  }

  bool isBusy() const { return !m_data.empty(); }

  void pause_work() { m_thread_lock.lock(); }

  void continue_work() { m_thread_lock.unlock(); }

  bool stoped() const { return m_stop_flag; }

protected:
  void _thread_func() {
    std::unique_lock<std::mutex> lock(m_thread_lock);
    m_thread_work = true;
    while (true) {
      while (m_data.empty()) {
        m_have_data.wait(lock);

        if (m_stop_flag) {
          break;
        }
      }

      if (m_stop_flag) {
        break;
      }
      m_add_lock.lock();
      T d = m_data.front();
      m_data.pop();
      m_add_lock.unlock();
      this->call(d);
    }
    m_thread_work = false;
  }

private:
  mutable std::mutex m_add_lock, m_thread_lock;
  std::queue<T> m_data;
  std::condition_variable m_have_data;
  std::thread m_thread;
  std::atomic<bool> m_stop_flag, m_thread_work;
};
}
