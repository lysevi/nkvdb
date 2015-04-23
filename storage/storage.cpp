#include "storage.h"
#include <boost/filesystem.hpp>
#include <utils/Exception.h>
#include <ctime>
#include <sstream>
#include <iterator>

using namespace storage;
namespace fs=boost::filesystem;

fs::path getOldesPage(const std::list<fs::path> &pages){
    if(pages.size()==1)
        return pages.front();
    fs::path maxTimePage;
    Time maxTime=0;
    for(auto p:pages){
        storage::Page::Header hdr=storage::Page::ReadHeader(p.string());
        Time cur_time=hdr.maxTime;
        if(maxTime<cur_time || cur_time==0){
            maxTime=cur_time;
            maxTimePage=p;
        }
    }
    if(maxTimePage.string().size()==0){
        throw utils::Exception::CreateAndLog(POSITION, "open error. page not found.");
    }
    return maxTimePage;
}

std::string getNewPageUniqueName(const std::string &ds_path){
    fs::path page_path;
    uint32_t suffix=0;

    while(true){
        if(page_path.string().length()!=0 && !fs::exists(page_path))
            break;

        page_path.clear();

        std::stringstream ss;
        ss<<std::time(nullptr)<<'_'<<suffix;
        ++suffix;
        page_path.append(ds_path);
        page_path.append(ss.str()+".page");
    }

    return page_path.string();
}

DataStorage::DataStorage(){

}

DataStorage::~DataStorage(){
	this->Close();
}

void DataStorage::Close() {
	if (this->m_curpage != nullptr) {
		this->m_curpage->close();
		this->m_curpage = nullptr;
	}
}

DataStorage::PDataStorage DataStorage::Create(const std::string& ds_path, uint64_t page_size){
    DataStorage::PDataStorage result(new DataStorage);
    result->m_default_page_size=page_size;

    if(fs::exists(ds_path)){
        if(!utils::rm(ds_path)){
            throw utils::Exception::CreateAndLog(POSITION, "can`t create. remove error: "+ds_path);
        }
    }

    fs::create_directory(ds_path);
    result->m_path=std::string(ds_path);
    result->createNewPage();
    return result;
}

DataStorage::PDataStorage DataStorage::Open(const std::string& ds_path){
    DataStorage::PDataStorage result(new DataStorage);

    if(!fs::exists(ds_path)){
        throw utils::Exception::CreateAndLog(POSITION, ds_path+" not exists.");
    }

    auto pages=utils::ls(ds_path,".page");
    fs::path maxTimePage=getOldesPage(pages);


    result->m_curpage=storage::Page::Open(maxTimePage.string());
    result->m_path=std::string(ds_path);
    return result;
}

void DataStorage::createNewPage(){
    if(m_curpage!=nullptr){
        m_curpage->close();
        m_curpage=nullptr;
    }

    std::string page_path=getNewPageUniqueName(m_path);

    m_curpage=Page::Create(page_path, this->m_default_page_size);
}

bool DataStorage::havePage2Write()const{
    return this->m_curpage!=nullptr && !this->m_curpage->isFull();
}

bool DataStorage::append(const Meas::PMeas m){
    std::lock_guard<std::mutex> guard(m_write_mutex);
    if(this->m_curpage->isFull()){
        this->createNewPage();
    }
    return this->m_curpage->append(m);
}

void DataStorage::append(const Meas::PMeas begin, const size_t meas_count) {
	std::lock_guard<std::mutex> guard(m_write_mutex);
	if (m_curpage->isFull()) {
		this->createNewPage();
	}
	
	size_t to_write = meas_count;
	while (to_write > 0) {
		size_t writed = m_curpage->append(begin, to_write);
		if (writed != to_write) {
			this->createNewPage();
		}
		to_write -= writed;
	}
}

bool HeaderIntervalCheck(Time from, Time to, Page::Header hdr) {
	if (hdr.minTime <= from || hdr.maxTime >= to){
		return true;
	} else {
		return false;
	}
}

Meas::MeasArray DataStorage::readInterval(Time from, Time to) {
	
	Meas::MeasList  list_result;
	auto page_list = pageList();
	for (auto page : page_list) {
		storage::Page::PPage page2read;
		if (page == m_curpage->fileName()) {
			if (HeaderIntervalCheck(from, to, m_curpage->getHeader())) {
				page2read = m_curpage;
			}
		} else {
			storage::Page::Header hdr = storage::Page::ReadHeader(page);
			if (HeaderIntervalCheck(from,to,hdr)){
				page2read = storage::Page::Open(page);
			} else {
				continue;
			}
		}
		if (page2read == nullptr) {
			continue;
		}
		
		Meas::MeasList subResult=page2read->readInterval(from, to);
		std::copy(subResult.begin(), subResult.end(), std::back_inserter(list_result));
	}

	if (list_result.size() == 0) {
		return Meas::MeasArray{};
	}
	Meas::MeasArray result{ list_result.begin(), list_result.end() };
	return result;
}

std::list<std::string> DataStorage::pageList()const {
	auto page_list = utils::ls(m_path, ".page");
	
	std::list<std::string> result;
	for (auto it = page_list.begin(); it != page_list.end(); ++it) {
		result.push_back(it->string());
	}
	
	return result;
}