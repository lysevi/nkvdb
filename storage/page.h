#pragma once

#include "utils/utils.h"

#include <cstdint>
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include <boost/interprocess/file_mapping.hpp>

#include "meas.h"
#include "config.h"
#include "index.h"

namespace storage {

class PageReader;
typedef std::shared_ptr<PageReader> PageReader_ptr;

/**
* Page class.
* Header + [meas_0...meas_i]
*/
class Page : public utils::NonCopy, public std::enable_shared_from_this<Page> {
public:
    static const uint8_t page_version = 1;
  struct Header {
    /// format version
    uint8_t version;
    /// is page already openned to read/write.
    bool isOpen;
    /// is page already openned to read.
    size_t ReadersCount;

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
    uint64_t WriteWindowSize;
  };

  typedef std::shared_ptr<Page> Page_ptr;
  typedef std::map<storage::Id,Meas> WriteWindow;

public:
  static Page_ptr Open(std::string filename, bool readOnly=false);
  static Page_ptr Create(std::string filename, uint64_t fsize);
  /// read only header from page file.
  static Page::Header ReadHeader(std::string filename);
  ~Page();

  /// mapped file size.
  size_t size() const;
  std::string fileName() const;
  std::string index_fileName() const;
  std::string writewindow_fileName() const;
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
  PageReader_ptr readInterval(Time from, Time to);
  PageReader_ptr readInterval(const IdArray &ids, storage::Flag source, storage::Flag flag, Time from, Time to);

  /// if page openned to read, after read must call this method.
  /// if count of reader is zero, page automaticaly closed;
  void readComplete();
  WriteWindow getWriteWindow();
  void        setWriteWindow(const Page::WriteWindow&other);
private:
  PageReader_ptr readAll();
  PageReader_ptr readFromToPos(const IdArray &ids, storage::Flag source, storage::Flag flag, Time from, Time to, size_t begin, size_t end);
  Page(std::string fname);
  /// write empty header.
  void initHeader(char *data);
  void updateMinMax(const Meas& value);
  void flushWriteWindow();
  void loadWriteWindow();
  void updateWriteWindow(const Meas&m);
protected:
  std::string *m_filename;

  boost::interprocess::file_mapping *m_file;
  boost::interprocess::mapped_region*m_region;
  Meas *m_data_begin;

  Header *m_header;
  Index  m_index;

  std::mutex m_lock;
  WriteWindow m_writewindow;
};


class PageReader: public utils::NonCopy
{
    typedef std::pair<uint64_t,uint64_t> from_to_pos;
public:
    static const  uint64_t defaultReadSize=1024;
    /// max count of measurements readed in on call of readNext
    static uint64_t ReadSize;

    PageReader(Page::Page_ptr page);
    ~PageReader();
    bool isEnd() const;
    void readNext(Meas::MeasList*output);
    /// add {from,to} position to read.
    void addReadPos(uint64_t begin,uint64_t end);
public:
    IdArray ids;
    storage::Flag source;
    storage::Flag flag;
    Time from;
    Time to;
	bool isWindowReader;
private:
	bool checkValueInterval(const Meas&m)const;
	bool checkValueFlags(const Meas&m)const;
private:
    Page::Page_ptr m_page;
    std::list<from_to_pos> m_read_pos_list;
    uint64_t m_cur_pos_begin;
    uint64_t m_cur_pos_end;
	
	bool m_wwWindowReader_read_end;
};

bool HeaderIntervalCheck(Time from, Time to, Page::Header hdr);
bool HeaderIdIntervalCheck(Id from, Id to, Page::Header hdr);
}
