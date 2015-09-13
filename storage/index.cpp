#include "index.h"

#include "utils/exception.h"
#include "utils/utils.h"
#include "utils/search.h"

#include <boost/iostreams/device/mapped_file.hpp>

using namespace storage;

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
//int i_cmp_pred(const Index::IndexRecord &r, const Index::IndexRecord &l) {
//	if (r.minTime < l.minTime)
//		return -1;
//	if (r.minTime == l.minTime)
//		return 0;
//
//	return 1;
//}
//
//int i_delta_pred(const Index::IndexRecord &r, const Index::IndexRecord &l) { return r.minTime - l.minTime; }

std::list<Index::IndexRecord> Index::findInIndex(const IdArray &ids, Time from, Time to) const {
	std::list<Index::IndexRecord> result;

	boost::iostreams::mapped_file i_file;

	try {

		boost::iostreams::mapped_file_params params;
		params.path = this->fileName();
		params.flags = i_file.readwrite;

		i_file.open(params);

		if (!i_file.is_open()) {
			return result;
		}

		IndexRecord *i_data = (IndexRecord *)i_file.data();
		auto fsize = i_file.size();

		bool index_filter = false;
		Id minId = 0;
		Id maxId = 0;
		if (ids.size() != 0) {
			index_filter = true;
			minId = *std::min_element(ids.cbegin(), ids.cend());
			maxId = *std::max_element(ids.cbegin(), ids.cend());
		}

        /*IndexRecord val;
		val.minTime = from;
		val.maxTime = to;

        auto read_start = i_data;
        utils::find_begin(i_data, i_data + fsize / sizeof(IndexRecord), val, i_cmp_pred, i_delta_pred);*/
		/*IndexRecord *from_pos = std::lower_bound(i_data, i_data + fsize / sizeof(IndexRecord), val,
			[](IndexRecord a, IndexRecord b) { return a.minTime < b.minTime; });
		IndexRecord *to_pos = std::lower_bound(i_data, i_data + fsize / sizeof(IndexRecord), val,
			[](IndexRecord a, IndexRecord b) { return a.maxTime < b.maxTime; });*/
		
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
		i_file.close();
		throw MAKE_EXCEPTION(message);
	}
	i_file.close();

	return result;
}
