#include "cache.h"
#include "time.h"
#include <utils/utils.h>
#include <iostream>

using namespace storage;

Cache::Cache(size_t size): m_max_size(size), m_size(0), m_index(0), m_sync(false) {
  m_meases = new Meas[size];
}

Cache::~Cache() { delete[] m_meases; }

bool Cache::isFull() const { return m_size == m_max_size; }

void Cache::clear() {
  //	m_data.clear();
  m_size = 0;
  m_index = 0;
}

bool Cache::is_sync() const { return m_sync; }

void Cache::sync_begin() { m_sync = true; }

void Cache::sync_complete() { m_sync = false; }

append_result Cache::append(const Meas &value, const Time past_time) {
  append_result res{};

  if (!checkPastTime(value.time, past_time)) {
    res.writed = 1;
    res.ignored = 1;
    return res;
  }
  // std::lock_guard<std::mutex> lock(this->m_rw_lock);
  if (!isFull()) {
    m_meases[m_index] = value;
    // this->m_data[value.time].push_back(m_index);
    m_index++;
    m_size++;

    res.writed = 1;
    res.ignored = 0;
    return res;
  } else {
    return res;
  }
}

append_result Cache::append(const Meas::PMeas begin, const size_t size,
                            const Time past_time) {
  // std::lock_guard<std::mutex> lock(this->m_rw_lock);
  size_t cap = this->m_max_size;
  size_t to_write = 0;
  if (cap > size) {
    to_write = size;
  } else if (cap == size) {
    to_write = size;
  } else if (cap < size) {
    to_write = cap;
  }

  append_result res{};
  res.writed = to_write;

  for (size_t i = 0; i < to_write; ++i) {
    if (!checkPastTime(begin[i].time, past_time)) {
      res.ignored++;
      continue;
    }
    m_meases[m_index] = Meas{begin[i]};
    m_size++;
    m_index++;
  }

  return res;
}

Meas::MeasList Cache::readInterval(Time from, Time to) const {
  // std::lock_guard<std::mutex> lock(this->m_rw_lock);
  Meas::MeasList result;
  for (size_t i = 0; i < m_index; ++i) {
    if (utils::inInterval(from, to, m_meases[i].time)) {
      result.push_back(m_meases[i]);
    }
  }
  return result;
}

Meas::PMeas Cache::asArray() const { return m_meases; }

void Cache::setSize(const size_t sz) {
  if (sz != m_max_size) {
    Meas *new_cache = new Meas[sz];
    delete[] m_meases;
    m_meases = new_cache;
    m_max_size = sz;
    m_size = sz;
  }
}

CachePool::CachePool(const size_t pool_size, const size_t cache_size)
    : m_pool_size(pool_size), m_cache_size(cache_size),
      m_default_pool_size(pool_size), /*m_default_cache_size(cache_size),*/
      m_dynamic_size(false) {
  m_recalc_period = (int)(pool_size / 3);
  init_pool();
}

void CachePool::init_pool() {
  this->clear();
  this->resize(m_pool_size);
  for (size_t i = 0; i < m_pool_size; ++i) {
    Cache::PCache c(new Cache(m_cache_size));
    (*this)[i] = c;
  }
}

void CachePool::enableDynamicSize(bool flg) { m_dynamic_size = flg; }

bool CachePool::dynamicSize() const { return m_dynamic_size; }

bool CachePool::haveCache() {
  auto c = this->getCache();
  return c != nullptr;
}

Cache::PCache CachePool::getCache() {
  m_recalc_period--;
  Cache::PCache result = nullptr;
  int count_of_free = 0;
  for (size_t i = 0; i < this->size(); i++) {
    if (!this->at(i)->is_sync()) {
      result = this->at(i);
      count_of_free++;
    }
  }

  if (m_dynamic_size) {
    if (result == nullptr) {
      this->setPoolSize((int)(m_pool_size * 1.5));
      this->setCacheSize((int)(m_cache_size * 1.5));
      m_recalc_period = m_pool_size;
      return this->getCache();
    }

    // FIX more smartable method.
    if (m_recalc_period <= 0) {
      if ((count_of_free >= (int)(m_pool_size / 2) &&
           (m_pool_size > m_default_pool_size))) { // free >= 50%
        size_t new_size = (size_t)m_pool_size / 2;
        if (new_size < m_default_pool_size) {
          new_size = m_default_pool_size;
        }
        //                logger<<"cache pool down to "<<new_size;
        this->setPoolSize(new_size);
        //                new_size=m_cache_size*0.75;
        //                if (new_size<m_default_cache_size){
        //                    new_size=m_default_cache_size;
        //                }
        //                this->setCacheSize(new_size);
        //                logger<<"cache up to "<<new_size;
        m_recalc_period = (int)m_pool_size / 2;
      }

      if (count_of_free <= m_pool_size * 0.85) { // free<=85%
        size_t new_size = m_pool_size * 2;
        //                logger<<"cache pool up to "<<new_size;
        this->setPoolSize(new_size);
        // logger<<"cache up to "<<(int)m_cache_size*1.5;
        // this->setCacheSize((int)m_cache_size*1.5);
        m_recalc_period = (int)m_pool_size / 2;
      }
    }
  }
  return result;
}

void CachePool::setCacheSize(const size_t sz) {
  m_cache_size = sz;
  for (size_t i = 0; i < this->size(); i++) {
    if (!this->at(i)->is_sync()) {
      this->at(i)->setSize(sz);
    }
  }
}

size_t CachePool::getCacheSize() const{
    return m_cache_size;
}

void CachePool::setPoolSize(const size_t sz) {
  m_pool_size = sz;
  this->init_pool();
}

size_t CachePool::getPoolSize()const{
    return m_pool_size;
}
