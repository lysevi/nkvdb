#include "page.h"
#include <utils/exception.h>
#include <utils/search.h>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <boost/filesystem.hpp>

#include <boost/interprocess/file_mapping.hpp>
#include <boost/interprocess/mapped_region.hpp>

const uint8_t page_version = 1;
uint64_t storage::PageReader::ReadSize=storage::PageReader::defaultReadSize;

using namespace storage;

const size_t oneMb = sizeof(char) * 1024 * 1024;

Page::Page(std::string fname)
    : m_filename(new std::string(fname)),
      m_file(nullptr),
      m_region(nullptr)
{
	
	this->m_index.setFileName(this->index_fileName());
}

Page::~Page() {
  this->close();
}

void Page::close() {
    if ((this->m_file!=nullptr) && (m_region!=nullptr)) {
        this->m_header->isOpen = false;
        this->m_header->ReadersCount = 0;
        delete m_region;
        delete m_file;
        m_region=nullptr;
        m_file=nullptr;
    }
}

size_t Page::size() const { return m_region->get_size(); }

std::string Page::fileName() const { return std::string(*m_filename); }

std::string Page::index_fileName() const {
  return std::string(*m_filename) + "i";
}

Time Page::minTime() const { return m_header->minTime; }

Time Page::maxTime() const { return m_header->maxTime; }

Page::Page_ptr Page::Open(std::string filename, bool readOnly) {
    if(!readOnly){
        storage::Page::Header hdr = Page::ReadHeader(filename);
        if (hdr.isOpen) {
            throw MAKE_EXCEPTION("page is already openned. ");
        }
    }
    Page_ptr result(new Page(filename));

    try {
           result->m_file=new bi::file_mapping(filename.c_str(),bi::read_write);
           result->m_region=new bi::mapped_region(*result->m_file, bi::read_write);
    } catch (std::runtime_error &ex) {
        std::string what = ex.what();
        throw MAKE_EXCEPTION(ex.what());
    }

    char *data = static_cast<char*>(result->m_region->get_address());
    result->m_header = (Page::Header *)data;
    result->m_data_begin = (Meas *)(data + sizeof(Page::Header));

    result->m_header->isOpen = true;
    if(readOnly){
        result->m_header->ReadersCount+=1;
    }
    return result;
}

Page::Page_ptr Page::Create(std::string filename, uint64_t fsize) {
  Page_ptr result(new Page(filename));

  try {
      {
          bi::file_mapping::remove(filename.c_str());
          std::filebuf fbuf;
          fbuf.open(filename,
                    std::ios_base::in | std::ios_base::out|std::ios_base::trunc | std::ios_base::binary);
                   //Set the size
          fbuf.pubseekoff(fsize-1, std::ios_base::beg);
          fbuf.sputc(0);
      }
      result->m_file=new bi::file_mapping(filename.c_str(),bi::read_write);
      result->m_region=new bi::mapped_region(*result->m_file, bi::read_write);
  } catch (std::runtime_error &ex) {
	  std::string what = ex.what();
	  throw MAKE_EXCEPTION(ex.what());
  }

  char *data = static_cast<char*>(result->m_region->get_address());

  result->initHeader(data);
  result->m_data_begin = (Meas *)(data + sizeof(Page::Header));
  result->m_header->isOpen = true;
  return result;
}

Page::Header Page::ReadHeader(std::string filename) {
  std::ifstream istream;
  istream.open(filename, std::fstream::in);
  if (!istream.is_open())
	  throw MAKE_EXCEPTION("can open file.");

  Header result;
  istream.read((char *)&result, sizeof(Page::Header));
  istream.close();
  return result;
}

void Page::initHeader(char *data) {
  m_header = (Page::Header *)data;
  memset(m_header, 0, sizeof(Page::Header));
  m_header->version = page_version;
  m_header->size = this->size();
  m_header->minMaxInit = false;
}

void Page::updateMinMax(const Meas& value) {
  if (m_header->minMaxInit) {
    m_header->minTime = std::min(value.time, m_header->minTime);
    m_header->maxTime = std::max(value.time, m_header->maxTime);

    m_header->minId = std::min(value.id, m_header->minId);
    m_header->maxId = std::max(value.id, m_header->maxId);
	
  } else {
    m_header->minMaxInit = true;
    m_header->minTime = value.time;
    m_header->maxTime = value.time;

    m_header->minId = value.id;
    m_header->maxId = value.id;
  }
}

bool Page::append(const Meas& value) {
    assert(m_header->ReadersCount==0);
    if (this->isFull()) {
        return false;
    }

    updateMinMax(value);

    memcpy(&m_data_begin[m_header->write_pos], &value, sizeof(Meas));

    Index::IndexRecord rec;
    rec.minTime = value.time;
    rec.maxTime = value.time;
    rec.minId = value.id;
    rec.maxId = value.id;
    rec.count = 1;
    rec.pos = m_header->write_pos;

    this->m_index.writeIndexRec(rec);

    m_header->write_pos++;
    return true;
}

size_t Page::append(const Meas::PMeas begin, const size_t size) {
    assert(m_header->ReadersCount==0);
    size_t cap = this->capacity();
    size_t to_write = 0;
    if (cap == 0) {
        return 0;
    }
    if (cap > size) {
        to_write = size;
    } else if (cap == size) {
        to_write = size;
    } else if (cap < size) {
        to_write = cap;
    }
    memcpy(m_data_begin + m_header->write_pos, begin, to_write * sizeof(Meas));

    updateMinMax(begin[0]);
    updateMinMax(begin[to_write-1]);

    Index::IndexRecord rec;
    rec.minTime = begin[0].time;
    rec.maxTime = begin[to_write - 1].time;
    rec.minId = begin[0].id;
    rec.maxId = begin[to_write - 1].id;
    rec.count = to_write;
    rec.pos = m_header->write_pos;

    this->m_index.writeIndexRec(rec);

    m_header->write_pos += to_write;
    return to_write;
}



bool Page::read(Meas::PMeas result, uint64_t position) {
    if(this->m_header->write_pos==0){
        return false;
    }
    if (result == nullptr)
        return false;
    {
        if (m_header->write_pos <= position) {
            return false;
        }
    }

    Meas *m = &m_data_begin[position];
    result->readFrom(m);
    return true;
}

void Page::readComplete(){
    std::lock_guard<std::mutex> _lock(m_lock);
    this->m_header->ReadersCount--;

    if(m_header->ReadersCount==0){
        this->close();
    }

}
PageReader_ptr  Page::readAll() {
    if(this->m_header->write_pos==0){
        return nullptr;
    }
    auto ppage=this->shared_from_this();
    auto preader=new PageReader(ppage);
    auto result=PageReader_ptr(preader);
    result->addReadPos(0,m_header->write_pos);
    return result;
}


PageReader_ptr Page::readFromToPos(const IdArray &ids, storage::Flag source, storage::Flag flag, Time from, Time to,size_t begin,size_t end){
    if(this->m_header->write_pos==0){
        return nullptr;
    }

    auto ppage=this->shared_from_this();
    auto preader=new PageReader(ppage);
    auto result=PageReader_ptr(preader);
    result->addReadPos(begin,end);
    result->ids=ids;
    result->source=source;
    result->flag=flag;
    result->from=from;
    result->to=to;
    return result;
}

PageReader_ptr Page::readInterval(Time from, Time to) {
    if(this->m_header->write_pos==0){
        return nullptr;
    }
    static IdArray emptyArray;
    return this->readInterval(emptyArray, 0, 0, from, to);
}

PageReader_ptr Page::readInterval(const IdArray &ids, storage::Flag source, storage::Flag flag, Time from, Time to) {
    // [from...minTime,maxTime...to]
    if(this->m_header->write_pos==0){
        return nullptr;
    }
    if ((from <= m_header->minTime) && (to >= m_header->maxTime)) {
        if ((ids.size() == 0) && (source == 0) && (flag == 0)) {
            auto result=this->readAll();
            result->from=from;
            result->to=to;
            return result;
        } else {
            return this->readFromToPos(ids, source, flag, from, to, 0, m_header->write_pos);
        }
    }
    
    auto ppage=this->shared_from_this();
    auto preader=new PageReader(ppage);
    auto result=PageReader_ptr(preader);
    result->ids = ids;
    result->source = source;
    result->flag = flag;
    result->from = from;
    result->to = to;

    auto irecords = m_index.findInIndex(ids, from, to);
    for (Index::IndexRecord &rec : irecords) {
        auto max_pos = rec.pos + rec.count;
        result->addReadPos(rec.pos,max_pos);
    }
  return result;
}

bool Page::isFull() const {
  return (sizeof(Page::Header) + sizeof(storage::Meas) * m_header->write_pos) >=
         m_header->size;
}

size_t Page::capacity() const {
  size_t bytes_left = m_header->size -(sizeof(Page::Header) + sizeof(storage::Meas) * m_header->write_pos);
  return bytes_left / sizeof(Meas);
}

Page::Header Page::getHeader() const { return *m_header; }


Meas::MeasList Page::readCurValues(IdSet&id_set) {
	Meas::MeasList result;
    if(this->m_header->write_pos==0){
        return result;
    }
    for (size_t pos = this->m_header->write_pos - 1; ; --pos) {
		Meas curValue = m_data_begin[pos];

		if (id_set.find(curValue.id) != id_set.end()) {
			result.push_back(curValue);
			id_set.erase(curValue.id);
		}
		if (id_set.size() == 0) {
			break;
		}
        if(pos==0){
            break;
        }
	}
	return result;
}

PageReader::PageReader(Page::Page_ptr page):
        ids(),
        source(0),
        flag(0),
        from(0),
        to(0),
        m_read_pos_list()        
{
    m_cur_pos_end=m_cur_pos_begin=0;
    m_page=page;
    shouldClose=false;
}

PageReader::~PageReader(){
    if((shouldClose) && (m_page!=nullptr)){
        m_page->readComplete();
        m_page=nullptr;
    }
}

void PageReader::addReadPos(uint64_t begin,uint64_t end){
    m_read_pos_list.push_back(std::make_pair(begin,end));
}

bool PageReader::isEnd() const{
    if(m_read_pos_list.size()==0){
        return true;
    }else{
        return false;
    }
}

void PageReader::readNext(Meas::MeasList*output){
    if(isEnd()){
        return;
    }
    // FIX read more small pieces
    auto pos=m_read_pos_list.front();
    m_read_pos_list.pop_front();
    
    m_cur_pos_begin=pos.first;
    m_cur_pos_end=pos.second;
    for (uint64_t i = m_cur_pos_begin; i < m_cur_pos_end; ++i) {
        storage::Meas readedValue;
        if (!m_page->read(&readedValue, i)) {
            std::stringstream ss;
            ss << "PageReader::readNext: "
                    << " file name: " << m_page->fileName()
                    << " readPos: " << i
                    << " size: " << m_page->getHeader().size;

            throw MAKE_EXCEPTION(ss.str());
        }

        if (utils::inInterval(from, to, readedValue.time)) {
            if (flag != 0) {
                if (readedValue.flag != flag) {
                    continue;
                }
            }
            if (source != 0) {
                if (readedValue.source != source) {
                    continue;
                }
            }
            if (ids.size() != 0) {
                if (std::find(ids.cbegin(), ids.cend(), readedValue.id) == ids.end()) {
                    continue;
                }
            }

            output->push_back(readedValue);
        }
        
    }
}
