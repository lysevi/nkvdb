#pragma once

#include <string>
#include <memory>
#include <thread>
#include <mutex>
#include "page.h"
#include "cache.h"
#include "asyncwriter.h"

namespace storage {

const uint64_t defaultPageSize = 10 * 1024 * 1024; // 10Mb
const size_t defaultcacheSize = 10000;
const size_t defaultcachePoolSize = 100;

class StorageReader;
typedef std::shared_ptr<StorageReader> PStorageReader;

/**
* Main class of mdb storage.
*/
class Storage {
public:
  typedef std::shared_ptr<Storage> Storage_ptr;

public:
  static Storage_ptr Create(const std::string &ds_path, uint64_t page_size = defaultPageSize);
  static Storage_ptr Open(const std::string &ds_path);
  ~Storage();
  void Close();

  bool havePage2Write() const;
  append_result append(const Meas& m);
  append_result append(const Meas::PMeas begin, const size_t meas_count);

  PStorageReader readInterval(Time from, Time to);
  PStorageReader readInterval(const IdArray &ids, storage::Flag source, storage::Flag flag, Time from, Time to);
  Meas::MeasList curValues(const IdArray&ids);

  /// get max time in past to write
  Time pastTime() const;
  /// set max time in past to write
  void setPastTime(const Time &t);

  void enableCacheDynamicSize(bool flg);
  bool cacheDynamicSize() const;

  size_t getPoolSize()const;
  void setPoolSize(size_t sz);

  size_t getCacheSize()const;
  void setCacheSize(size_t sz);

  /// load current values of ids. return array of not founded measurements.
  IdArray loadCurValues(const IdArray&ids);
private:
  Storage();
  void writeCache();
protected:
  std::string m_path;

  std::mutex m_write_mutex;
  storage::Cache::PCache m_cache;
  AsyncWriter m_cache_writer;
  CachePool m_cache_pool;
  CurValuesCache m_cur_values;
  Time m_past_time;
  bool m_closed;
  friend class storage::Cache;
};

class StorageReader: public utils::NonCopy{
public:
    StorageReader();
    bool isEnd();
    void readNext(Meas::MeasList*output);
    void addReader(PageReader_ptr reader);
private:
    std::vector<PageReader_ptr> m_readers;
    PageReader_ptr m_reader;
};
}
