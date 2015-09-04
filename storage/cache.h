#pragma once

#include <vector>
#include <memory>
#include "Meas.h"
#include "common.h"

namespace storage
{
	
    class Cache
    {
    public:
        typedef std::shared_ptr<Cache> PCache;
        Cache(size_t size);
        ~Cache();
        bool isFull()const;
		append_result append(const Meas& value, const Time past_time);
		append_result append(const Meas::PMeas begin, const size_t size, const Time past_time);
        storage::Meas::MeasList readInterval(Time from, Time to)const;
        Meas::PMeas asArray()const;
        size_t size()const { return m_size; }
		void setSize(const size_t sz);
        void clear();

        bool is_sync() const;
        void sync_begin();
        void sync_complete();
    private:
        //typedef std::map<storage::Time, std::list<size_t>> time2meas;

        size_t m_max_size;
        Meas  *m_meases;
        //time2meas m_data;
        size_t  m_size;
        size_t  m_index;
        bool    m_sync;
    };

    class CachePool: std::vector<Cache::PCache>{
        CachePool(){}
    public:
        CachePool(const size_t pool_size, const size_t cache_size);
        bool haveCache();
        Cache::PCache getCache();
		void setCacheSize(const size_t sz);
		void setPoolSize(const size_t sz);
	protected:
		void init_pool();
    private:
        size_t m_pool_size, m_cache_size;
		int m_recalc_period;
    };
}
