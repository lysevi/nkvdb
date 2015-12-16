#pragma once

#include "meas.h"
#include <list>
#include <string>

namespace nkvdb
{
	const uint16_t index_file_format=1;
    /**
    * Implement index for page.
    */
	class Index
	{
	public:
		struct IndexRecord
		{
			uint64_t pos;
			Id id;
            Time time;
		};
		struct IndexHeader
		{
			uint16_t format;
		};
	public:
		Index(const size_t cache_size);
		~Index();
		std::string fileName()const;
		void setFileName(const std::string& fname);
		void writeIndexRec(const IndexRecord &rec);
		std::list<Index::IndexRecord> findInIndex(const IdArray &ids, Time from, Time to) const;
		void flush()const;
	protected:
		bool checkInterval(const IndexRecord&rec, Time from, Time to)const;
		
	private:
		std::string m_fname;
		std::vector<IndexRecord> m_cache;
		mutable size_t m_cache_pos;
	};

}
