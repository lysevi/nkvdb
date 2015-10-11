#pragma once

#include <string>
#include <thread>
#include <mutex>
#include "meas.h"
#include "page.h"
#include "cache.h"
#include "asyncworker.h"

namespace nkvdb {

const uint64_t defaultPageSize = sizeof(Page::Header) + sizeof(Meas) * 1000000;
const size_t defaultcacheSize = 10000;
const size_t defaultcachePoolSize = 100;

class StorageReader;
typedef std::shared_ptr<StorageReader> StorageReader_ptr;

class Storage;

class AsyncWriter : public utils::AsyncWorker<Cache::PCache> {
public:
  AsyncWriter();
  void setStorage(Storage *nkvdb);
  void call(const Cache::PCache data) override;

private:
  Storage *m_storage;
};

/**
* Main class of nkvdb storage.
*/
class Storage: public MetaStorage  {
public:
    typedef std::shared_ptr<Storage> Storage_ptr;

public:
    static Storage_ptr Create(const std::string &ds_path, uint64_t page_size = defaultPageSize);
    static Storage_ptr Open(const std::string &ds_path);
    ~Storage();

    bool havePage2Write() const;

    virtual Time minTime() override;
    virtual Time maxTime() override;

    append_result append(const Meas& m);
    append_result append(const Meas::PMeas begin, const size_t meas_count);

    Reader_ptr readIntervalFltr(const IdArray &ids, nkvdb::Flag source, nkvdb::Flag flag, Time from, Time to) override;
    Reader_ptr readInTimePointFltr(const IdArray &ids, nkvdb::Flag source, nkvdb::Flag flag, Time time_point) override;

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
    void Close();
    Storage();
    void writeCache();
    void flush_and_stop();
protected:
    std::string m_path;

    mutable std::mutex m_write_mutex;
    nkvdb::Cache::PCache m_cache;
    AsyncWriter m_cache_writer;
    CachePool m_cache_pool;
    CurValuesCache m_cur_values;
    Time m_past_time;
    bool m_closed;
    friend class nkvdb::Cache;
};

class StorageReader: public utils::NonCopy, public Reader{
public:
    StorageReader();
    virtual bool isEnd()const override;
    virtual void readNext(Meas::MeasList*output) override;
    void addPage(std::string page_name);

    IdArray ids;
    nkvdb::Flag source;
    nkvdb::Flag flag;
    nkvdb::Time from;
    nkvdb::Time to;
    nkvdb::Time time_point;
    std::string prev_interval_page;
private:
    std::deque<std::string> m_pages;
    Reader_ptr m_current_reader;
};
}
