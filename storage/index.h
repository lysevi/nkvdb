#pragma once

#include "meas.h"
#include <list>
namespace storage
{

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
	public:
		Index();
		~Index();
		std::string fileName()const;
		void setFileName(const std::string& fname);
		void writeIndexRec(const IndexRecord &rec);
		std::list<Index::IndexRecord> findInIndex(const IdArray &ids, Time from, Time to) const;
	private:
		std::string m_fname;
	};

}