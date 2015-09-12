#pragma once

#include <utils/utils.h>

#include <cstdint>
#include <memory>
#include <string>
#include <map>
#include <boost/iostreams/device/mapped_file.hpp>

#include "meas.h"
#include "config.h"
#include "index.h"

namespace storage {

/**
* Page class.
* Header + [meas_0...meas_i]
*/
class Page : public utils::NonCopy {
public:
  struct Header {
    /// format version
    uint8_t version;
    /// is page already openned.
    bool isOpen;
    /// fields min* max* is init or empty.
    bool minMaxInit;
    /// min-max of time
    Time minTime;
    Time maxTime;
    /// min-max of id
    Id minId;
    Id maxId;
    /// current write position.
    uint64_t write_pos;
    /// size in bytes
    uint64_t size;
  };

  typedef std::shared_ptr<Page> PPage;

public:
  static PPage Open(std::string filename);
  static PPage Create(std::string filename, uint64_t fsize);
  /// read only header from page file.
  static Page::Header ReadHeader(std::string filename);
  ~Page();

  /// mapped file size.
  size_t size() const;
  std::string fileName() const;
  std::string index_fileName() const;
  /// min time of writed meas
  Time minTime() const;
  /// max time of writed meas
  Time maxTime() const;
  bool isFull() const;
  /// free space in page
  size_t capacity() const;
  void close();
  Header getHeader() const;

  bool append(const Meas& value);
  size_t append(const Meas::PMeas begin, const size_t size);
  bool read(Meas::PMeas result, uint64_t position);
  void readInterval(Time from, Time to, storage::Meas::MeasList&result);
  void readInterval(const IdArray &ids, storage::Flag source, storage::Flag flag, Time from, Time to, storage::Meas::MeasList&result);
  
  Meas::MeasList readCurValues(IdSet&id_set);
private:
  void readAll(storage::Meas::MeasList *dest);
  void readFromToPos(const IdArray &ids,
                                        storage::Flag source,
                                        storage::Flag flag, Time from,
										Time to, size_t begin, size_t end, 
										storage::Meas::MeasList *dest);
  Page(std::string fname);
  /// write empty header.
  void initHeader(char *data);
  void updateMinMax(const Meas& value);
  
  
protected:
  std::string *m_filename;

  boost::iostreams::mapped_file *m_file;

  Meas *m_data_begin;

  Header *m_header;
  Index  m_index;
};

class PageReader: public utils::NonCopy{
    PageReader();
public:
    typedef std::shared_ptr<PageReader> PPageReader;
    ~PageReader();
    bool isEnd() const;
    void readNext(Meas::MeasList*output);
private:
    Page::PPage m_page;
};


}
