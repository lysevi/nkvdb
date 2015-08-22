#pragma once

#include <vector>
#include <memory>
#include "Meas.h"

namespace storage
{
    class Cache
	{
	public:
        typedef std::shared_ptr<Cache> PCache;
		Cache(size_t size);
		~Cache();
		bool isFull()const;
		bool append(const Meas value);
		size_t append(const Meas::PMeas begin, const size_t size);
		storage::Meas::MeasList readInterval(Time from, Time to)const;
        Meas::PMeas asArray()const;
		size_t size()const { return m_size; }
		void clear();

        // FIX set normal names
        bool is_sync() const;
        void on_sync();
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
        bool haveCache()const;
        Cache::PCache getCache();
    private:
        size_t m_pool_size, m_cache_size;
    };
}
