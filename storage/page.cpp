#include "page.h"
#include <utils/exception.h>
#include <utils/search.h>
#include <algorithm>
#include <sstream>
#include <cstdlib>
#include <cmath>
#include <fstream>
#include <boost/filesystem.hpp>

const uint8_t page_version = 1;

namespace ios = boost::iostreams;

using namespace storage;

const size_t oneMb = sizeof(char) * 1024 * 1024;

int cmp_pred(const storage::Meas &r, const storage::Meas &l) {
  if (r.time < l.time)
    return -1;
  if (r.time == l.time)
    return 0;

  return 1;
}

int delta_pred(const Meas &r, const Meas &l) { return r.time - l.time; }

Page::Page(std::string fname)
    : m_filename(new std::string(fname)),
      m_file(new boost::iostreams::mapped_file) {
	
	this->m_index.setFileName(this->index_fileName());
}

Page::~Page() {
  this->close();
  delete m_file;
  delete m_filename;
}

void Page::close() {
  if (this->m_file->is_open()) {
    this->m_header->isOpen = false;
    m_file->close();
  }

  this->m_file->close();
}

size_t Page::size() const { return m_file->size(); }

std::string Page::fileName() const { return std::string(*m_filename); }

std::string Page::index_fileName() const {
  return std::string(*m_filename) + "i";
}

Time Page::minTime() const { return m_header->minTime; }

Time Page::maxTime() const { return m_header->maxTime; }

Page::PPage Page::Open(std::string filename) {
#ifdef CHECK_PAGE_OPEN
  storage::Page::Header hdr = Page::ReadHeader(filename);
  if (hdr.isOpen) {
	  throw MAKE_EXCEPTION("page is already openned. ");
  }
#endif
  PPage result(new Page(filename));

  try {
	  boost::iostreams::mapped_file_params params;
	  params.path = filename;
	  params.flags = result->m_file->readwrite;
	  result->m_file->open(params);
  } catch (std::runtime_error &ex) {
	  std::string what = ex.what();
	  throw MAKE_EXCEPTION(ex.what());
  }
  if (!result->m_file->is_open())
	  throw MAKE_EXCEPTION("can`t create file ");

  char *data = result->m_file->data();
  result->m_header = (Page::Header *)data;
  result->m_data_begin = (Meas *)(data + sizeof(Page::Header));
  result->m_header->isOpen = true;
  return result;
}

Page::PPage Page::Create(std::string filename, uint64_t fsize) {
  PPage result(new Page(filename));

  try {
	  boost::iostreams::mapped_file_params params;
	  params.new_file_size = fsize;
	  params.path = filename;
	  params.flags = result->m_file->readwrite;
	  result->m_file->open(params);
  } catch (std::runtime_error &ex) {
	  std::string what = ex.what();
	  throw MAKE_EXCEPTION(ex.what());
  }

  if (!result->m_file->is_open())
	  throw MAKE_EXCEPTION("can`t create file ");

  char *data = result->m_file->data();

  result->initHeader(data);
  result->m_data_begin = (Meas *)(data + sizeof(Page::Header));
  result->m_header->isOpen = true;
  return result;
}

Page::Header Page::ReadHeader(std::string filename) {
  std::ifstream istream;
  istream.open(filename, std::fstream::in);
  if (!istream.is_open())
	  throw MAKE_EXCEPTION("can open file.");

  Header result;
  istream.read((char *)&result, sizeof(Page::Header));
  istream.close();
  return result;
}

void Page::initHeader(char *data) {
  m_header = (Page::Header *)data;
  memset(m_header, 0, sizeof(Page::Header));
  m_header->version = page_version;
  m_header->size = this->m_file->size();
  m_header->minMaxInit = false;
}

void Page::updateMinMax(const Meas& value) {
  if (m_header->minMaxInit) {
    m_header->minTime = std::min(value.time, m_header->minTime);
    m_header->maxTime = std::max(value.time, m_header->maxTime);

    m_header->minId = std::min(value.id, m_header->minId);
    m_header->maxId = std::max(value.id, m_header->maxId);
  } else {
    m_header->minMaxInit = true;
    m_header->minTime = value.time;
    m_header->maxTime = value.time;

    m_header->minId = value.id;
    m_header->maxId = value.id;
  }
}

bool Page::append(const Meas& value) {
  if (this->isFull()) {
    return false;
  }

  updateMinMax(value);

  memcpy(&m_data_begin[m_header->write_pos], &value, sizeof(Meas));

  Index::IndexRecord rec;
  rec.minTime = value.time;
  rec.maxTime = value.time;
  rec.minId = value.id;
  rec.maxId = value.id;
  rec.count = 1;
  rec.pos = m_header->write_pos;

  this->m_index.writeIndexRec(rec);

  m_header->write_pos++;
  return true;
}

size_t Page::append(const Meas::PMeas begin, const size_t size) {
  size_t cap = this->capacity();
  size_t to_write = 0;
  if (cap > size) {
    to_write = size;
  } else if (cap == size) {
    to_write = size;
  } else if (cap < size) {
    to_write = cap;
  }
  memcpy(m_data_begin + m_header->write_pos, begin, to_write * sizeof(Meas));

  updateMinMax(begin[0]);
  updateMinMax(begin[size - 1]);

  Index::IndexRecord rec;
  rec.minTime = begin[0].time;
  rec.maxTime = begin[size - 1].time;
  rec.minId = begin[0].id;
  rec.maxId = begin[size - 1].id;
  rec.count = to_write;
  rec.pos = m_header->write_pos;

  this->m_index.writeIndexRec(rec);

  m_header->write_pos += to_write;
  return to_write;
}



bool Page::read(Meas::PMeas result, uint64_t position) {
  if (result == nullptr)
    return false;
  {
    if (m_header->write_pos <= position) {
      return false;
    }
  }

  Meas *m = &m_data_begin[position];
  result->readFrom(m);
  return true;
}



storage::Meas::MeasList Page::readInterval(Time from, Time to) {
  return this->readInterval(IdArray{}, 0, 0, from, to);
}

storage::Meas::MeasList Page::readInterval(const IdArray &ids,
                                           storage::Flag source,
                                           storage::Flag flag, Time from,
                                           Time to) {
  storage::Meas::MeasList result;
  storage::Meas readedValue;

  auto irecords = m_index.findInIndex(ids, from, to);
  for (Index::IndexRecord &rec : irecords) {
    auto max_pos = rec.pos + rec.count;

    /*storage::Meas key;
    key.time = from;

    auto begin = utils::find_begin(this->m_data_begin + rec.pos,
    this->m_data_begin + max_pos, key, cmp_pred, delta_pred);*/
    for (size_t i = rec.pos; i < max_pos; ++i) {
      // for (size_t i = std::distance(m_data_begin, begin); i < max_pos; ++i) {
      if (!read(&readedValue, i)) {
        std::stringstream ss;
        ss << "ReadIntervalError: "
           << " file name: " << m_filename
           << " writePos: " << m_header->write_pos
           << " size: " << m_header->size;

		throw MAKE_EXCEPTION(ss.str());
      }
      if (utils::inInterval(from, to, readedValue.time)) {
        if (flag != 0) {
          if (readedValue.flag != flag) {
            continue;
          }
        }
        if (source != 0) {
          if (readedValue.source != source) {
            continue;
          }
        }
        if (ids.size() != 0) {
          if (std::find(ids.cbegin(), ids.cend(), readedValue.id) ==
              ids.end()) {
            continue;
          }
        }
        result.push_back(readedValue);
      }
    }
  }
  return result;
}

bool Page::isFull() const {
  return (sizeof(Page::Header) + sizeof(storage::Meas) * m_header->write_pos) >=
         m_header->size;
}

size_t Page::capacity() const {
  size_t bytes_left =
      m_header->size -
      (sizeof(Page::Header) + sizeof(storage::Meas) * m_header->write_pos);
  return bytes_left / sizeof(Meas);
}

Page::Header Page::getHeader() const { return *m_header; }
