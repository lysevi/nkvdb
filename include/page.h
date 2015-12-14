#pragma once

#include "utils.h"

#include <cstdint>
#include <memory>
#include <string>
#include <map>
#include <mutex>
#include <boost/interprocess/file_mapping.hpp>

#include "meas.h"
#include "index.h"
#include "writewindow.h"

namespace nkvdb {

class PageReader;
typedef std::shared_ptr<PageReader> PageReader_ptr;

struct PageCommonHeader
{
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
	// bytes reserved to future options.
	
};

struct CommonPage :public MetaStorage
{
	typedef std::shared_ptr<CommonPage> Page_ptr;
	virtual size_t capacity() const=0;
	virtual void close() = 0;
	virtual size_t size() const = 0;
	virtual std::string fileName() const = 0;
	virtual bool isFull() const = 0;
	virtual void flushWriteWindow() = 0;
	virtual WriteWindow getWriteWindow() = 0;
	virtual void setWriteWindow(const WriteWindow&other) = 0;
};

/**
* Page class.
* Header + [meas_0...meas_i]
*/
class Page : public utils::NonCopy, public std::enable_shared_from_this<Page>, public CommonPage {
public:
    static const uint8_t page_version = 2;
	struct Header : public PageCommonHeader
	{
		uint64_t _1;
		uint64_t _2;
		uint64_t _3;
		uint64_t _4;
		uint64_t _5;
		uint64_t _6;
		uint64_t _7;
		uint64_t _8;
		uint64_t _9;
		uint64_t _10;
		uint64_t _11;
		uint64_t _12;
		uint64_t _13;
		uint64_t _14;
		uint64_t _15;
		uint64_t _16;
	};

  

public:
	typedef std::shared_ptr<Page> Page_ptr;
  template<int n>
  static uint64_t calc_size(){
      return sizeof(nkvdb::Page::Header)+sizeof(nkvdb::Meas)*n;
  }

  static Page_ptr Open(std::string filename, bool readOnly=false);
  static Page_ptr Create(std::string filename, uint64_t fsize);
  /// read only header from page file.
  static Page::Header ReadHeader(std::string filename);
  ~Page();

  /// mapped file size.
  size_t size() const override;
  std::string fileName() const override;
  std::string index_fileName() const;
  std::string writewindow_fileName() const;
  bool isFull() const override;
  /// free space in page
  size_t capacity() const override;
  void close() override;
  Header getHeader() const;

  virtual append_result append(const Meas& value)override;
  virtual append_result append(const Meas::PMeas begin, const size_t size)override;
  
  using MetaStorage::readInterval;
  using MetaStorage::readInTimePoint;

  virtual Reader_ptr readInterval(const IdArray &ids, nkvdb::Flag source, nkvdb::Flag flag, Time from, Time to) override;
  virtual Reader_ptr readInTimePoint(const IdArray &ids, nkvdb::Flag source, nkvdb::Flag flag, Time time_point) override;

  virtual Time minTime()override;
  virtual Time maxTime()override;

  bool read(Meas::PMeas result, uint64_t position);

  // read from end to start while not find all meases in ids;
  Meas::MeasList backwardRead(const IdArray &ids, nkvdb::Flag source, nkvdb::Flag flag, Time time_point);
  /// if page openned to read, after read must call this method.
  /// if count of reader is zero, page automaticaly closed;
  void readComplete();
  WriteWindow getWriteWindow()override;
  void        setWriteWindow(const WriteWindow&other)override;

  void flushWriteWindow()override;
private:
  PageReader_ptr readAll();
  PageReader_ptr readFromToPos(const IdArray &ids, nkvdb::Flag source, nkvdb::Flag flag, Time from, Time to, size_t begin, size_t end);
  Page(std::string fname);
  /// write empty header.
  void initHeader(char *data);
  void updateMinMax(const Meas& value);
  
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

bool HeaderIntervalCheck(Time from, Time to, PageCommonHeader hdr);
bool HeaderIdIntervalCheck(Id from, Id to, PageCommonHeader hdr);

class PageReader: public utils::NonCopy, public Reader{
public:
    static const  uint64_t defaultReadSize=1024;
    /// max count of measurements readed in on call of readNext
    static uint64_t ReadSize;

    PageReader(Page::Page_ptr page);
    ~PageReader();

    IdArray ids;
    nkvdb::Flag source;
    nkvdb::Flag flag;
    WriteWindow prev_ww;

protected:
    bool checkValueFlags(const Meas&m)const;
    void timePointRead(Time tp, Meas::MeasList*output);
protected:
     Page::Page_ptr m_page;
};

}
