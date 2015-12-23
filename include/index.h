#pragma once

#include "meas.h"
#include <btree.h>

#include <list>
#include <string>
#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>


namespace bi = boost::interprocess;

namespace nkvdb
{
	const uint16_t index_file_format=2;
    /**
    * Implement index for page.
    */
	class Index
	{
	public:
		

        struct IndexRecord
        {
            uint64_t pos;
            uint64_t count;
            Time minTime;
            Time maxTime;
            Id minId;
            Id maxId;
        };
		struct IndexHeader
		{
			uint16_t format;
			size_t   cache_pos;
			size_t   root_pos;
			size_t   cache_size;
		};

		typedef trees::BTree<Time, Index::IndexRecord, 10> IndexTree;
	public:
		Index(const size_t cache_size);
		~Index();
		std::string fileName()const;
		void setFileName(const std::string& fname, uint64_t fsize);
		void writeIndexRec(const IndexRecord &rec);
		std::list<Index::IndexRecord> findInIndex(const IdArray &ids, Time from, Time to) const;
        void flush()const;
		void close();
	protected:
		bool checkInterval(const IndexRecord&rec, Time from, Time to)const;
	private:
		std::string m_fname;
		std::vector<IndexRecord> m_cache;
		mutable size_t m_cache_pos;
		
		
		bi::file_mapping *mfile;
		bi::mapped_region *mregion;
		Index::IndexHeader* header;
		IndexTree::Node*data;
	};

}
