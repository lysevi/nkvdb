#include "storage.h"
#include <boost/filesystem.hpp>
#include <utils/Exception.h>
#include <ctime>
#include <sstream>

using namespace storage;
namespace fs=boost::filesystem;

DataStorage::DataStorage(){

}

DataStorage::~DataStorage(){

}

DataStorage* DataStorage::Create(const std::string& ds_path){
    DataStorage*result=new DataStorage;

    if(fs::exists(ds_path)){
        fs::path path_to_remove(ds_path);
        for (fs::directory_iterator end_dir_it, it(path_to_remove); it!=end_dir_it; ++it) {
            if(!remove_all(it->path())){
                throw utils::Exception::CreateAndLog(POSITION, "can`t create. remove error: "+it->path().string());
            }
        }
    }

    fs::create_directory(ds_path);
    result->m_path=std::string(ds_path);
    result->createNewPage();
    return result;
}

DataStorage* DataStorage::Open(const std::string& ds_path){
    DataStorage*result=new DataStorage;

    if(!fs::exists(ds_path)){
        throw utils::Exception::CreateAndLog(POSITION, ds_path+" not exists.");
    }

    result->m_path=std::string(ds_path);
    return result;
}

void DataStorage::createNewPage(){
    if(m_curpage!=nullptr){
        m_curpage=nullptr;
    }

    std::stringstream ss;
    ss<<std::time(nullptr);

    fs::path page_path;
    page_path.append(m_path);
    page_path.append(ss.str()+".page");

    m_curpage=Page::Create(page_path.string(),DataStorage::pageSizeMb);
}
