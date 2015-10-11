#include "storage.h"
#include "page_manager.h"
#include "exception.h"

#include <ctime>
#include <cmath>
#include <sstream>
#include <iterator>

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;

using namespace nkvdb;

struct MeasCmpByTime {
    bool operator()(nkvdb::Meas a, nkvdb::Meas b) { return a.time < b.time; }
};


AsyncWriter::AsyncWriter() {}

void AsyncWriter::setStorage(Storage *storage) { m_storage = storage; }

void AsyncWriter::call(const Cache::PCache data) {
    assert(m_storage != nullptr);

    auto output = data->asArray();

    MeasCmpByTime time_cmp;
    std::sort(output, output + data->size(), time_cmp);

    size_t meas_count = data->size();
    size_t to_write = data->size();

    while (to_write > 0) {
        auto ares=PageManager::get()->getCurPage()->append(output + (meas_count - to_write), to_write);
        size_t writed = ares.writed;
        if (writed != to_write) {
            PageManager::get()->createNewPage();
        }
        to_write -= writed;
    }
    PageManager::get()->getCurPage()->flushWriteWindow();
    data->clear();
    data->sync_complete();
}


Storage::Storage()
    : MetaStorage(),m_cache_pool(defaultcachePoolSize, defaultcacheSize)
{
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

Storage::Storage_ptr Storage::Create(const std::string &ds_path, uint64_t page_size) {
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

Time Storage::minTime() {
    std::lock_guard<std::mutex> guard(m_write_mutex);
    this->flush_and_stop();
    this->m_cache_writer.pause_work();

    auto pages = PageManager::get()->pagesByTime();

    this->m_cache_writer.continue_work();
    return pages.back().header.minTime;
}

Time Storage::maxTime() {
    std::lock_guard<std::mutex> guard(m_write_mutex);
    this->flush_and_stop();
    this->m_cache_writer.pause_work();

    auto pages = PageManager::get()->pagesByTime();

    this->m_cache_writer.continue_work();
    return pages.front().header.maxTime;
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


Reader_ptr  Storage::readIntervalFltr(const IdArray &ids,nkvdb::Flag source, nkvdb::Flag flag,Time from, Time to) {
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


Reader_ptr  Storage::readInTimePointFltr(const IdArray &ids, nkvdb::Flag source, nkvdb::Flag flag, Time time_point) {
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

        // [min  max] tp
        if ((hdr.minTime <= time_point) && (hdr.maxTime <= time_point)) {
            // pages.size==1 || last_page
            if ((pages.size() == 1) || (i==(pages.size()-1))) {
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

    nkvdb::Page::Page_ptr page2read = nkvdb::Page::Open(page_time_vector.front().name, true);
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

void Storage::flush_and_stop(){
    this->writeCache();
    while (true) {
        if (!m_cache_writer.isBusy()) {
            break;
        }
        std::this_thread::sleep_for(std::chrono::microseconds(100));
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

bool StorageReader::isEnd()const{
    if(this->m_pages.size()==0){
        return m_current_reader==nullptr?true:m_current_reader->isEnd();
    }else{
        return false;
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
        nkvdb::Page::Page_ptr page2read = nkvdb::Page::Open(page_name, true);

        WriteWindow prev_ww{};
        if (prev_interval_page != "") {
            nkvdb::Page::Page_ptr prev_page2read = nkvdb::Page::Open(prev_interval_page, true);
            prev_ww = prev_page2read->getWriteWindow();
            prev_page2read->readComplete();
        }

        if (this->time_point != 0) {
            m_current_reader = page2read->readInTimePointFltr(ids, source, flag, time_point);
            auto rdr=m_current_reader.get();
            dynamic_cast<PageReader*>(rdr)->prev_ww = prev_ww;

        }
        else {
            m_current_reader = page2read->readIntervalFltr(ids, source, flag, from, to);
            auto rdr=m_current_reader.get();
            dynamic_cast<PageReader*>(rdr)->prev_ww=prev_ww;

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
