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
    class PageReader;
    typedef std::shared_ptr<PageReader> PPageReader;
/**
* Page class.
* Header + [meas_0...meas_i]
*/
class Page : public utils::NonCopy, public std::enable_shared_from_this<Page> {
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
  PPageReader readInterval(Time from, Time to);
  PPageReader readInterval(const IdArray &ids, storage::Flag source, storage::Flag flag, Time from, Time to);
  
  Meas::MeasList readCurValues(IdSet&id_set);
private:
  PPageReader readAll();
  PPageReader readFromToPos(const IdArray &ids, storage::Flag source, storage::Flag flag, Time from, Time to, size_t begin, size_t end);
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
    
public:
    typedef std::pair<uint64_t,uint64_t> from_to_pos;
    
    PageReader(Page::PPage page);
    ~PageReader();
    bool isEnd() const;
    void readNext(Meas::MeasList*output);
    void addReadPos(from_to_pos pos);
    IdArray ids;
    storage::Flag source;
    storage::Flag flag;
    Time from;
    Time to;
    bool shouldClose;
private:
    Page::PPage m_page;
    std::list<from_to_pos> m_read_pos_list;
    uint64_t m_cur_pos_begin;
    uint64_t m_cur_pos_end;
};
}
