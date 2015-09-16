#include "index.h"

#include "utils/exception.h"
#include "utils/utils.h"
#include "utils/search.h"

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

using namespace storage;

namespace bi = boost::interprocess;

Index::Index():m_fname("not_set") {
}


Index::~Index() {
}

void Index::setFileName(const std::string& fname) {
	m_fname = fname;
}

std::string Index::fileName()const {
	return m_fname;
}

void Index::writeIndexRec(const Index::IndexRecord &rec) {
	FILE *pFile = std::fopen(this->fileName().c_str(), "ab");

	try {
		fwrite(&rec, sizeof(rec), 1, pFile);
	} catch (std::exception &ex) {
		auto message = ex.what();
		fclose(pFile);
		throw MAKE_EXCEPTION(message);
	}
	fclose(pFile);
}

std::list<Index::IndexRecord> Index::findInIndex(const IdArray &ids, Time from, Time to) const {
	std::list<Index::IndexRecord> result;

	try {

		bi::file_mapping i_file(this->fileName().c_str(), bi::read_write);
		bi::mapped_region region(i_file, bi::read_write);

		
		IndexRecord *i_data = (IndexRecord *)region.get_address();
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

			if (utils::inInterval(from, to, rec.minTime) ||	utils::inInterval(from, to, rec.maxTime)) {
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
