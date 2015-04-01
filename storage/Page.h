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
public:
    struct Header {
		uint8_t version;
        Time   minTime;
        Time   maxTime;
		Id     minId;
		Id     maxId;
		Flag   minSource;
		Flag   maxSource;
		Flag   minFlag;
		Flag   maxFlag;
        uint64_t write_pos;
        uint64_t size; //in bytes
    };

    typedef std::shared_ptr<Page> PPage;
public:
    static PPage Open(std::string filename);
	static PPage Create(std::string filename, uint64_t fsize);
    static Page::Header ReadHeader(std::string filename);
    ~Page();

    size_t size()const;
    std::string fileName()const;
    Time minTime()const;
    Time maxTime()const;
    bool isFull()const;
	size_t capacity()const;
	void close();
    Header getHeader()const;

	bool append(const Meas::PMeas value);
	size_t append(const Meas::PMeas begin, const size_t size);
	bool read(Meas::PMeas result, uint64_t position);
	storage::Meas::MeasList readInterval(Time from, Time to);
private:
    Page(std::string fname);
    void initHeader(char * data);
    void updateMinMax(Meas::PMeas value);
protected:
    std::string *m_filename;

    mutable std::mutex m_writeMutex;

    boost::iostreams::mapped_file *m_file;

    Meas* m_data_begin;

    Header *m_header;
};
}
