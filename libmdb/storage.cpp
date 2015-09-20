#include "storage.h"
#include "page_manager.h"
#include "utils/exception.h"

#include <ctime>
#include <cmath>
#include <sstream>
#include <iterator>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

using namespace mdb;


Storage::Storage()
    : m_cache_pool(defaultcachePoolSize, defaultcacheSize) {
  m_cache = m_cache_pool.getCache();
  m_cache->setStorage(this);
  m_cache_writer.setStorage(this);
  m_cache_writer.start();
  m_past_time = 0;
  m_closed = false;
}

Storage::~Storage() { 
	if (!m_closed) { 
		this->Close(); 
    }
}

void Storage::Close() {
  if (!m_cache_writer.stoped()) {
    this->writeCache();
    m_cache_writer.stop();
  }

  PageManager::get()->closeCurrentPage();
  PageManager::stop();
  m_closed = true;
}

Storage::Storage_ptr Storage::Create(const std::string &ds_path,
                                              uint64_t page_size) {
  Storage::Storage_ptr result(new Storage);
  

  if (fs::exists(ds_path)) {
    if (!utils::rm(ds_path)) {
      throw utils::Exception::CreateAndLog(
          POSITION, "can`t create. remove error: " + ds_path);
    }
  }

  fs::create_directory(ds_path);
  result->m_path = std::string(ds_path);
  PageManager::start(result->m_path);
  PageManager::get()->default_page_size = page_size;
  PageManager::get()->createNewPage();
  return result;
}

Storage::Storage_ptr Storage::Open(const std::string &ds_path) {

  Storage::Storage_ptr result(new Storage);

  if (!fs::exists(ds_path)) {
    throw utils::Exception::CreateAndLog(POSITION, ds_path + " not exists.");
  }

  auto pages = utils::ls(ds_path, ".page");
  result->m_path = std::string(ds_path);

  PageManager::start(result->m_path);
  std::string maxTimePage = PageManager::get()->getOldesPage();
  PageManager::get()->open(maxTimePage);

  return result;
}

bool Storage::havePage2Write() const {
    return (PageManager::get()->getCurPage() != nullptr) && (!PageManager::get()->getCurPage()->isFull());
}

append_result Storage::append(const Meas& m) {
  std::lock_guard<std::mutex> guard(m_write_mutex);
  append_result res{};
  while (res.writed == 0) {
    res = m_cache->append(m, m_past_time);
    if (res.writed == 0) {
      this->writeCache();
    }
	if (res.ignored != 0) {
		logger_info("DataStorage: ignored on write:" << res.ignored);
	}
  }
  return res;
}

append_result Storage::append(const Meas::PMeas begin, const size_t meas_count) {
  std::lock_guard<std::mutex> guard(m_write_mutex);
  if (m_cache->isFull()) {
    this->writeCache();
  }
  size_t to_write = meas_count;
  append_result result{};
  while (to_write > 0) {
    auto wrt_res =
        m_cache->append(begin + (meas_count - to_write), to_write, m_past_time);

    if (wrt_res.writed != to_write) {
      this->writeCache();
    }
    to_write -= wrt_res.writed;
    result = result + wrt_res;
  }
  if (result.ignored != 0) {
	  logger_info("DataStorage: ignored on write:" << result.ignored);
  }
  return result;
}

void Storage::writeCache() {
  if (m_cache->size() == 0) {
    return;
  }
  
  m_cache->sync_begin();

  m_cache_writer.add(m_cache);

  // FIX must use more smart method.
  m_cache = nullptr;
  while (true) {
    m_cache = m_cache_pool.getCache();
    if (m_cache != nullptr) {
      break;
    }
  }
  m_cache->setStorage(this);
}

StorageReader_ptr Storage::readInterval(Time from, Time to) {
	static IdArray empty{};
	return this->readInterval(empty, 0, 0, from, to);
}

StorageReader_ptr Storage::readInterval(const IdArray &ids,
                                     mdb::Flag source, mdb::Flag flag,
                                     Time from, Time to) {
	std::lock_guard<std::mutex> guard(m_write_mutex);
	this->flush_and_stop();

    auto sr=new StorageReader();
    StorageReader_ptr result(sr);

    this->m_cache_writer.pause_work();

    auto pages = PageManager::get()->pagesByTime();
	std::reverse(std::begin(pages), std::end(pages));

	std::list<std::string> pages_to_read{};

	
    for (size_t i=0;i<pages.size();i++) {
		auto pinfo = pages[i];
		auto page_name = pinfo.name;
		auto hdr = pinfo.header;

		// [min from to max]
		if ((hdr.minTime <= from) && (hdr.maxTime >= to)) {
			pages_to_read.push_back(page_name);
			if (i>0) {
				if ((result->prev_interval_page == "") && (pages_to_read.size()==1)) {
					result->prev_interval_page = pages[i - 1].name;
				}
			}
			continue;
		}

        // [min from max]
        if ((hdr.minTime <= from) && (hdr.maxTime> from)) {
				pages_to_read.push_back(page_name);
				if (i>0) {
					result->prev_interval_page = pages[i - 1].name;
				}
				continue;
		}

        // [...max] from [min...]
		if (hdr.minTime > from) {
			if ((i > 0) && (pages[i - 1].header.maxTime <= from)) {
				pages_to_read.push_back(pages[i - 1].name);
				pages_to_read.push_back(page_name);
				continue;
			}
		}

        // from  [min to max] 
		if ((hdr.minTime >= from) && (hdr.maxTime >= to) && (hdr.minTime <= to)) {
			pages_to_read.push_back(page_name);
			continue;
		}
		
		// from  [min  max] to
		if ((hdr.minTime >= from) && (hdr.maxTime <= to)) {
			pages_to_read.push_back(page_name);
			continue;
		}
    }
	
    result->ids=ids;
    result->from=from;
    result->to=to;
    result->source=source;
    result->flag=flag;
	for (auto page_name : pages_to_read) {
        result->addPage(page_name);
	}

    this->m_cache_writer.continue_work();

    return result;
}

StorageReader_ptr Storage::readInTimePoint(Time time_point) {
	return this->readInTimePoint(IdArray{}, 0, 0, time_point);
}

StorageReader_ptr Storage::readInTimePoint(const IdArray &ids, mdb::Flag source, mdb::Flag flag, Time time_point) {
	std::lock_guard<std::mutex> guard(m_write_mutex);
	this->flush_and_stop();
	auto sr = new StorageReader();
	StorageReader_ptr result(sr);

	this->m_cache_writer.pause_work();

	auto pages = PageManager::get()->pagesByTime();
	std::reverse(std::begin(pages), std::end(pages));

	std::list<std::string> pages_to_read{};


	for (size_t i = 0; i<pages.size(); i++) {
		auto pinfo = pages[i];
		auto page_name = pinfo.name;
		auto hdr = pinfo.header;

		// [min  max] tp, pages.size==1
		if ((hdr.minTime <= time_point) && (hdr.maxTime <= time_point)) {
			if (pages.size() == 1) {
				pages_to_read.push_back(page_name);
				break;
			}
		}

		// [min tp max]
		if ((hdr.minTime <= time_point) && (hdr.maxTime >= time_point)) {
			pages_to_read.push_back(page_name);
			if (i>0) {
				result->prev_interval_page = pages[i - 1].name;
			}
			break;
		}

		// [...max] from [min...]
		if (hdr.minTime > time_point) {
			if ((i > 0) && (pages[i - 1].header.maxTime <= time_point)) {
				pages_to_read.push_back(pages[i - 1].name);
				break;
			}
		}
	}

	result->ids = ids;
	result->time_point = time_point;
	result->source = source;
	result->flag = flag;
	for (auto page_name : pages_to_read) {
		result->addPage(page_name);
	}

	this->m_cache_writer.continue_work();

	return result;
}
IdArray Storage::loadCurValues(const IdArray&ids) {
	auto from = *std::min_element(ids.begin(),ids.end());
	auto to = *std::max_element(ids.begin(), ids.end());
	std::vector<PageManager::PageInfo> pages_vector = PageManager::get()->pagesByTime();
	std::vector<PageManager::PageInfo> page_time_vector{};
	
	for (auto p : pages_vector) {
		if (HeaderIdIntervalCheck(from, to, p.header)) {
			page_time_vector.push_back(p);
		}
	}

	IdSet id_set(ids.begin(), ids.end());

	mdb::Page::Page_ptr page2read = mdb::Page::Open(page_time_vector.front().name, true);
	WriteWindow ww = page2read->getWriteWindow();
	for (auto m : ww) {
		m_cur_values.writeValue(m);
		id_set.erase(m.id);
	}
	page2read->readComplete();
    if(id_set.size()!=0){
        for(auto id:id_set){
            logger("DataStorage::loadCurValues: not found  "<<id);
        }
    }
    IdArray result(id_set.begin(),id_set.end());
    return result;
}


Time Storage::pastTime() const { return m_past_time; }

void Storage::setPastTime(const Time &t) { m_past_time = t; }

void Storage::enableCacheDynamicSize(bool flg) {
  m_cache_pool.enableDynamicSize(flg);
}

bool Storage::cacheDynamicSize() const {
  return m_cache_pool.dynamicSize();
}

size_t Storage::getPoolSize()const{
    return m_cache_pool.getPoolSize();
}

void Storage::setPoolSize(size_t sz){
    m_cache_pool.setPoolSize(sz);
}

size_t Storage::getCacheSize()const{
    return m_cache_pool.getCacheSize();
}

void Storage::setCacheSize(size_t sz){
    m_cache_pool.setCacheSize(sz);
}

void Storage::flush_and_stop() {
	this->writeCache();
	while (true) {
		if (!m_cache_writer.isBusy()) {
			break;
		}
	}
}

Meas::MeasList Storage::curValues(const IdArray&ids) {
	this->writeCache();
	while (true) {
		if (!m_cache_writer.isBusy()) {
			break;
		}
	}
	return m_cur_values.readValue(ids);
}


StorageReader::StorageReader():m_pages(){
    m_current_reader=nullptr;
    prev_interval_page="";
	time_point = 0;
}

bool StorageReader::isEnd(){
    if(this->m_pages.size()==0){
        return m_current_reader==nullptr?true:m_current_reader->isEnd();
    }else{
        return false;
    }
}

void StorageReader::readAll(Meas::MeasList*output) {
	while (!isEnd()) {
		this->readNext(output);
	}
}

void StorageReader::readNext(Meas::MeasList*output){
	assert(output != nullptr);
    if(isEnd()){
        return;
    }

    if(m_current_reader==nullptr){
        auto page_name=m_pages.front();
        m_pages.pop_front();
        mdb::Page::Page_ptr page2read = mdb::Page::Open(page_name, true);

        WriteWindow prev_ww{};
        if (prev_interval_page != "") {
            mdb::Page::Page_ptr prev_page2read = mdb::Page::Open(prev_interval_page, true);
            prev_ww = prev_page2read->getWriteWindow();
            prev_page2read->readComplete();
        }

		if (this->time_point != 0) {
			m_current_reader = page2read->readInTimePoint(ids, source, flag, time_point);
			m_current_reader->prev_ww = prev_ww;

		}
		else {
			m_current_reader = page2read->readInterval(ids, source, flag, from, to);
			m_current_reader->prev_ww=prev_ww;
		
		}
    }

	m_current_reader->readNext(output);

    if(m_current_reader->isEnd()){
        m_current_reader=nullptr;
    }
	
}

void StorageReader::addPage(std::string page_name){
    this->m_pages.push_back(page_name);
}
