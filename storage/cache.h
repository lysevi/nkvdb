#pragma once

#include <map>
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
		void asArray(Meas::PMeas output)const;
		size_t size()const { return m_size; }
		void clear();
	private:
		//typedef std::map<storage::Time, std::list<size_t>> time2meas;
		
		size_t m_max_size;
		Meas  *m_meases;
		//time2meas m_data;
		size_t  m_size;
		size_t  m_index;
	};

}