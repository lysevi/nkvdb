#include "index.h"

#include "exception.h"
#include "utils.h"
#include "search.h"


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
	
	for (int i = 0; i < m_cache_pos; i++) {
		tree.insert(m_cache[i].minTime, m_cache[i]);
	}
    
	m_cache_pos = 0;
}

void Index::setFileName(const std::string& fname) {
	m_fname = fname;
	if (!boost::filesystem::exists(fname)) {
		IndexHeader ih;
		ih.format = index_file_format;

		FILE *pFile = std::fopen(this->fileName().c_str(), "ab");
		fwrite(&ih, sizeof(IndexHeader), 1, pFile);
		fclose(pFile);
		
		bi::file_mapping::remove(fname.c_str());
		std::filebuf fbuf;
		fbuf.open(fname,
				  std::ios_base::in | std::ios_base::out | std::ios_base::trunc | std::ios_base::binary);
		//Set the size
		const size_t cache_size = 1000000;
		fbuf.pubseekoff(sizeof(Index::IndexHeader) + sizeof(IndexTree::Node)*cache_size, std::ios_base::beg);
		fbuf.sputc(0);
		fbuf.close();


		mfile = new bi::file_mapping(this->m_fname.c_str(), bi::read_write);
		mregion = new bi::mapped_region(*mfile, bi::read_write);
		
		char* raw_data = static_cast<char*>(mregion->get_address());

		header = (Index::IndexHeader*)(raw_data);
		header->format = index_file_format;
		header->cache_size = cache_size;
		header->root_pos = 0;
		header->cache_pos = 1;

		data = (IndexTree::Node*)(raw_data + sizeof(IndexHeader));
		data[0].is_leaf = true;
		mregion->flush();
		
	} else {
		mfile = new bi::file_mapping(this->m_fname.c_str(), bi::read_write);
		mregion = new bi::mapped_region(*mfile, bi::read_write);
		char* raw_data = static_cast<char*>(mregion->get_address());
		header = (Index::IndexHeader*)(raw_data);
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

std::list<Index::IndexRecord> Index::findInIndex(const IdArray &ids, Time from, Time to) const {
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
		//auto stop_node = tree.find_node(to);
       		
      
		while (true) {
			for (size_t i = 0; i < start_node->vals_size; i++) {
				Index::IndexRecord rec;
				auto kv = start_node->vals[i];
				rec = start_node->vals[i].second;

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
