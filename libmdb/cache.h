#pragma once

#include <vector>
#include <memory>
#include <map>
#include "meas.h"
#include "common.h"

namespace mdb {
class Storage;

/**
  * Cache of values. after  fulled, cache write to page.
    */
class Cache {
public:
    typedef std::shared_ptr<Cache> PCache;
    Cache(size_t size);
    ~Cache();
    bool isFull() const;
    append_result append(const Meas &value, const Time past_time);
    append_result append(const Meas::PMeas begin, const size_t size,
                         const Time past_time);
    mdb::Meas::MeasList readInterval(Time from, Time to) const;
    Meas::PMeas asArray() const;
    size_t size() const { return m_size; }
    void setSize(const size_t sz);
    void clear();

    /// this cache is ander sync
    bool is_sync() const;
    void sync_begin();
    void sync_complete();

    void setStorage(Storage*ds);
private:
    // typedef std::map<storage::Time, std::list<size_t>> time2meas;

    size_t m_max_size;
    Meas *m_meases;
    // time2meas m_data;
    size_t m_size;
    size_t m_index;
    bool m_sync;
    Storage*m_ds;
};

/**
* Pool of caches.
*/
class CachePool : std::vector<Cache::PCache> {
    CachePool() {}

public:
    CachePool(const size_t pool_size, const size_t cache_size);
    /// have free cache for write
    bool haveCache();
    /// get cache to write
    Cache::PCache getCache();
    void setCacheSize(const size_t sz);
    size_t getCacheSize()const;
    void setPoolSize(const size_t sz);
    size_t getPoolSize()const;

    /// enable dynamic cache size
    void enableDynamicSize(bool flg);
    /// is dynamic size enabled
    bool dynamicSize() const;

protected:
    void init_pool();

private:
    size_t m_pool_size, m_cache_size;
    size_t m_default_pool_size;  // pool size on it init.
    //size_t m_default_cache_size; // pool size on it init.
    int m_recalc_period;
    bool m_dynamic_size;
};

/**
* Cache of cur values
*/
class CurValuesCache
{
public:
    CurValuesCache();
    void writeValue(const mdb::Meas&v);
    mdb::Meas::MeasList readValue(const mdb::IdArray&ids)const;
private:
    std::map<mdb::Id, mdb::Meas> m_values;
};

}
