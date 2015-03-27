#pragma once

#include <utils/utils.h>

#include <memory>
#include <string>

#include <boost/iostreams/device/mapped_file.hpp>


class Page : public utils::NonCopy
{
	std::string *m_filename;
	boost::iostreams::mapped_file *m_file;
public:
	typedef std::shared_ptr<Page> PPage;
public:
	static PPage Open(std::string filename);
	static PPage Create(std::string filename, size_t sizeInMbytes);
	
	size_t size();
	size_t sizeMb();
	~Page();
private:
	Page(std::string fname);
};

