#include "index.h"

#include "exception.h"
#include "utils.h"
#include "search.h"
#include "bloom_filter.h"
#include <fstream>

using namespace nkvdb;

Index::Index(const size_t cache_size) :m_fname("not_set") {
	m_cache.resize(cache_size);
	m_cache_pos = 0;
}


Index::~Index() {
	this->close();
}

void Index::close() {
	if (mfile != nullptr) {
		this->flush();
		delete mregion;
		delete mfile;
		mfile = nullptr;
		mregion = nullptr;
	}
}

void Index::flush()const {
	// m_tree;
    if (m_cache_pos == 0) {
        return;
    }
	IndexTree tree(data, header->cache_size, header->root_pos, header->cache_pos);
	
    for (size_t i = 0; i < m_cache_pos; i++) {
		tree.insert(m_cache[i].minTime, m_cache[i]);
	}
    
	m_cache_pos = 0;
	header->root_pos = tree.m_root->id;
    header->cache_pos=tree.cache_pos;
	mregion->flush();
}

void Index::setFileName(const std::string& fname, uint64_t fsize) {
	m_fname = fname;
	if (!boost::filesystem::exists(fname)) {
		IndexHeader ih;
		ih.format = index_file_format;

		FILE *pFile = std::fopen(this->m_fname.c_str(), "ab");
		fwrite(&ih, sizeof(IndexHeader), 1, pFile);
		fclose(pFile);
		
		bi::file_mapping::remove(fname.c_str());
		std::filebuf fbuf;
		fbuf.open(fname,
				  std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
		//Set the size
		fbuf.pubseekoff(sizeof(Index::IndexHeader) + sizeof(IndexTree::Node)*fsize / (tree_N*tree_N), std::ios_base::beg);
		fbuf.sputc(0);
		fbuf.close();


		mfile = new bi::file_mapping(this->m_fname.c_str(), bi::read_write);
		mregion = new bi::mapped_region(*mfile, bi::read_write);
		
		char* raw_data = static_cast<char*>(mregion->get_address());

		header = (Index::IndexHeader*)(raw_data);
		header->format = index_file_format;
		header->cache_size = fsize / (tree_N * tree_N);
        header->root_pos = 1;
        header->cache_pos = 2;

		data = (IndexTree::Node*)(raw_data + sizeof(IndexHeader));
		IndexTree::init_cache(data, header->cache_size);
		mregion->flush();
		
	} else {
		mfile = new bi::file_mapping(this->m_fname.c_str(), bi::read_write);
		mregion = new bi::mapped_region(*mfile, bi::read_write);
		char* raw_data = static_cast<char*>(mregion->get_address());
		header = (Index::IndexHeader*)(raw_data);
		if (header->format != index_file_format) {
			std::stringstream ss;
			ss << "wrong index file format exist: " << header->format << " expected:" << index_file_format;
			MAKE_EXCEPTION(ss.str());
		}
		data = (IndexTree::Node*)(raw_data + sizeof(IndexHeader));

	}
}

std::string Index::fileName()const {
	return m_fname;
}

void Index::writeIndexRec(const Index::IndexRecord &rec) {
    if (m_cache_pos==m_cache.size()) {
            this->flush();
    }
    m_cache[m_cache_pos] = rec;
    m_cache_pos++;
}

std::list<Index::IndexRecord> Index::findInIndex(const IdArray &ids, Time from, Time to, Flag flag, Flag source) const {
    this->flush();
	std::list<Index::IndexRecord> result;

	try {
		IndexTree tree(data, header->cache_size, header->root_pos, header->cache_pos);


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

		auto start_node = tree.find_node(from);
        auto stop_node = tree.find_node(to);
      
        while ((start_node->id!=stop_node->id) || (first)){
			for (size_t i = 0; i < start_node->vals_size; i++) {
				Index::IndexRecord rec;
				auto kv = start_node->vals[i];
                rec = kv.second;

				if ((flag != 0) && (rec.flg_fltr != 0) && (!bloom_check(rec.flg_fltr, flag))) {
					continue;
				}

				if ((source != 0) && (rec.src_fltr != 0) && (!bloom_check(rec.src_fltr, source))) {
					continue;
				}

				if (checkInterval(rec, from, to)) {
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
			if (start_node->next == 0) {
				break;
			}
			start_node = tree.getNode(start_node->next);
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
