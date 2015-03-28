#pragma once

#include <string>
#include "Page.h"

namespace storage{

class DataStorage{
    const int pageSizeMb=1;
public:
    static DataStorage* Create(const std::string& ds_path);
    static DataStorage* Open(const std::string& ds_path);
    ~DataStorage();
private:
    DataStorage();
    void createNewPage();
protected:
    std::string m_path;
    Page::PPage m_curpage;
};

};
