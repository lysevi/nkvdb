#include "index.h"

#include "exception.h"
#include "utils.h"
#include "search.h"

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace nkvdb;

namespace bi = boost::interprocess;

Index::Index(const size_t cache_size) :m_fname("not_set") {
	m_cache.resize(cache_size);
	m_cache_pos = 0;
}


Index::~Index() {
	this->flush();
}

void Index::setFileName(const std::string& fname) {
	m_fname = fname;
	if (!boost::filesystem::exists(fname)) {
		IndexHeader ih;
		ih.format = index_file_format;

		FILE *pFile = std::fopen(this->fileName().c_str(), "ab");
		fwrite(&ih, sizeof(IndexHeader), 1, pFile);
		fclose(pFile);
	}
}

std::string Index::fileName()const {
	return m_fname;
}

void Index::flush()const {
	if (m_cache_pos == 0) {
		return;
	}
	FILE *pFile = std::fopen(this->fileName().c_str(), "ab");

	try {
		for (size_t i = 0; i < m_cache_pos; i++){
			auto rec = m_cache[i];
			fwrite(&rec, sizeof(IndexRecord), 1, pFile);
		}
	} catch (std::exception &ex) {
		auto message = ex.what();
		fclose(pFile);
		throw MAKE_EXCEPTION(message);
	}
	fclose(pFile);
	m_cache_pos = 0;
}

void Index::writeIndexRec(const Index::IndexRecord &rec) {
	if (m_cache_pos==m_cache.size()) {
		this->flush();
	}
	m_cache[m_cache_pos] = rec;
	m_cache_pos++;
}

std::list<Index::IndexRecord> Index::findInIndex(const IdArray &ids, Time from, Time to) const {
	this->flush();

	std::list<Index::IndexRecord> result;

	try {

		bi::file_mapping i_file(this->fileName().c_str(), bi::read_write);
		bi::mapped_region region(i_file, bi::read_write);

		
		IndexRecord *i_data = (IndexRecord *)((char*)region.get_address()+sizeof(Index::IndexHeader));
		auto fsize = region.get_size();

		bool index_filter = false;
		Id minId = 0;
		Id maxId = 0;
		if (ids.size() != 0) {
			index_filter = true;
			minId = *std::min_element(ids.cbegin(), ids.cend());
			maxId = *std::max_element(ids.cbegin(), ids.cend());
		}

       		
		Index::IndexRecord prev_value;
		bool first = true;
        for (size_t pos = 0; pos<fsize / sizeof(IndexRecord); pos++) {
			Index::IndexRecord rec;

			rec = i_data[pos];

			if (checkInterval(rec,from,to))
			{
				result.push_back(rec);
			}
		}
		if (!first) {
			result.push_back(prev_value);
		}
	} catch (std::exception &ex) {
		auto message = ex.what();
		throw MAKE_EXCEPTION(message);
	}

	return result;
}

bool  Index::checkInterval(const IndexRecord&rec, Time from, Time to)const {
	return utils::inInterval(from, to, rec.time);
}
