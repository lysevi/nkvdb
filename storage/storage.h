#pragma once

#include <string>
#include <memory>
#include "Page.h"

namespace storage{

    class DataStorage{
        const int pageSizeMb=1;
    public:
        typedef std::shared_ptr<DataStorage> PDataStorage;
    public:
        static PDataStorage Create(const std::string& ds_path);
        static PDataStorage Open(const std::string& ds_path);
        ~DataStorage();
        bool havePage2Write()const;
    private:
        DataStorage();
        void createNewPage();
    protected:
        std::string m_path;
        Page::PPage m_curpage;
    };

};
