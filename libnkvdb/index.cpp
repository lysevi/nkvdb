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

void Index::writeIndexRec(const Index::IndexRecord &rec) {
    FILE *pFile=nullptr;
    try {
        FILE *pFile = std::fopen(this->fileName().c_str(), "ab");
        fwrite(&rec, sizeof(IndexRecord), 1, pFile);
        fclose(pFile);
    } catch (std::exception &ex) {
        if(pFile!=nullptr){
            fclose(pFile);
        }

        auto message = ex.what();
        throw MAKE_EXCEPTION(message);
    }
}

std::list<Index::IndexRecord> Index::findInIndex(const IdArray &ids, Time from, Time to) const {
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
                if ((!index_filter) || (utils::inInterval(minId, maxId, rec.minId) || utils::inInterval(minId, maxId, rec.maxId))) {
                    if (!first) {
                        if ((prev_value.pos + prev_value.count) == rec.pos) {
                            prev_value.count += rec.count;
                        } else {
                            result.push_back(prev_value);
                            prev_value = rec;
                        }
                    } else {
                        first = false;
                        prev_value = rec;
                    }
                }
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
    return utils::inInterval(from, to, rec.minTime)
            || utils::inInterval(from, to, rec.maxTime)
            || utils::inInterval(rec.minTime, rec.maxTime, from)
            || utils::inInterval(rec.minTime, rec.maxTime, from);
}
