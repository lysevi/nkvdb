#include "Page.h"
#include <utils/Exception.h>
#include <fstream>

namespace ios = boost::iostreams;

using namespace storage;

const size_t oneMb = sizeof (char) * 1024 * 1024;

Page::Page(std::string fname):m_filename(new std::string(fname)),
m_file(new boost::iostreams::mapped_file), m_write_pos(0) {
}

Page::~Page() {
    m_file->close();
    delete m_file;
    delete m_filename;
}

size_t Page::size() const{
    return m_file->size();
}

size_t Page::sizeMb() const{
    return (size_t) (this->size() / oneMb);
}

std::string Page::fileName()const {
    return std::string(*m_filename);
}
Page::PPage Page::Open(std::string filename) {
    logger << "openning new file: " + filename << endl;
    PPage result(new Page(filename));

    try {
        boost::iostreams::mapped_file_params params;
        params.path = filename;
        params.flags = result->m_file->readwrite;
        result->m_file->open(params);
    } catch (std::runtime_error &ex) {
        std::string what = ex.what();
        throw utils::Exception::CreateAndLog(POSITION, ex.what());
    }
    if (!result->m_file->is_open())
        throw utils::Exception::CreateAndLog(POSITION, "can`t create file ");

    logger << "size: " << result->m_file->size() << endl;

    return result;
}

Page::PPage Page::Create(std::string filename, size_t sizeInMbytes) {
    logger << "creating new file: " + filename << endl;
    PPage result(new Page(filename));

    try {
        boost::iostreams::mapped_file_params params;
        params.new_file_size = sizeInMbytes*oneMb;
        params.path = filename;
        params.flags = result->m_file->readwrite;
        result->m_file->open(params);
    } catch (std::runtime_error &ex) {
        std::string what = ex.what();
        throw utils::Exception::CreateAndLog(POSITION, ex.what());
    }

    if (!result->m_file->is_open())
        throw utils::Exception::CreateAndLog(POSITION, "can`t create file ");

    logger << "file size: " << result->m_file->size() << endl;

    char * data = (char *) result->m_file->data();
    for (size_t i = 0; i < result->m_file->size(); i++) {
        data[i] = '*';
    }

    result->m_data_begin = (Meas*) data;
    result->m_write_pos = 0;

    return result;
}

void Page::append(const Meas::PMeas value) {
    std::lock_guard<std::mutex> guard(m_writeMutex);
    
    memcpy(&m_data_begin[m_write_pos],value.get(),sizeof(Meas));
    m_write_pos++;
}

bool Page::read(Meas::PMeas result, uint64_t position){
    if(result==nullptr)
        return false;
    {
        std::lock_guard<std::mutex> guard(m_writeMutex);
        if(m_write_pos<=position){
            return false;
        }
    }

    Meas* m = &m_data_begin[position];
    result->readFrom(m);
    return true;
    
}