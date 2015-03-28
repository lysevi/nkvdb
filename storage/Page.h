#pragma once

#include <utils/utils.h>

#include <cstdint>
#include <memory>
#include <string>
#include <mutex>
#include <boost/iostreams/device/mapped_file.hpp>

#include "Meas.h"

namespace storage {

class Page : public utils::NonCopy {

    struct Header {
        u_char version;
        Time   minTime;
        Time   maxTime;
        uint64_t write_pos;
    };
public:
    typedef std::shared_ptr<Page> PPage;
public:
    static PPage Open(std::string filename);
    static PPage Create(std::string filename, size_t sizeInMbytes);
    ~Page();

    size_t size()const;
    size_t sizeMb()const;
    std::string fileName()const;
    Time minTime()const;
    Time maxTime()const;
    bool append(const Meas::PMeas value);
    bool read(Meas::PMeas result, uint64_t position);

private:
    Page(std::string fname);
    void initHeader(char * data);
    void updateMinMax(Meas::PMeas value);
protected:
    std::string *m_filename;

    std::mutex m_writeMutex;

    boost::iostreams::mapped_file *m_file;

    Meas* m_data_begin;

    Header *m_header;
};
}
