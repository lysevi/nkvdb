#pragma once

#include <utils/utils.h>

#include <memory>
#include <string>
#include <mutex>
#include <boost/iostreams/device/mapped_file.hpp>

#include "Meas.h"

namespace storage {

    class Page : public utils::NonCopy {
    public:
        typedef std::shared_ptr<Page> PPage;
    public:
        static PPage Open(std::string filename);
        static PPage Create(std::string filename, size_t sizeInMbytes);
        ~Page();
        
        size_t size()const;
        size_t sizeMb()const;
        std::string fileName()const;
        
        void write(const Meas::PMeas value);
        bool read(Meas::PMeas result, uint64_t position);
        
    private:
        Page(std::string fname);
    protected:
        std::string *m_filename;

        std::mutex m_writeMutex;

        boost::iostreams::mapped_file *m_file;

        Meas* m_data_begin;
        uint64_t m_write_pos;
    };
}
