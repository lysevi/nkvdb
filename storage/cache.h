#pragma once

#include <map>
#include <mutex>
#include "Meas.h"

namespace storage
{
	class Cache
	{
	public:
		Cache(size_t size);
		~Cache();
		bool isFull()const;
		bool append(const Meas value);
		size_t append(const Meas::PMeas begin, const size_t size);
		storage::Meas::MeasList readInterval(Time from, Time to)const;
		storage::Meas::PMeas asArray()const;
		size_t size()const { return m_size; }
		void clear();
	private:
		typedef std::map<storage::Time, std::list<Meas>> time2meas;

		mutable std::mutex m_rw_lock;
		size_t m_max_size;
		time2meas m_data;
		size_t  m_size;
	};

}