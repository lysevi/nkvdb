#include "Page.h"
#include <utils/Exception.h>
#include <fstream>

namespace ios = boost::iostreams;

const size_t oneMb = sizeof(char) * 1024 * 1024;

Page::Page(std::string fname) : m_filename(new std::string(fname)),
m_file(new boost::iostreams::mapped_file) {
}


Page::~Page() {
	m_file->close();
	delete m_file;
	delete m_filename;
}

size_t Page::size() {
	return m_file->size();
}

size_t Page::sizeMb() {
	return (size_t)(this->size() / oneMb);
}

Page::PPage Page::Open(std::string filename) {
  logger << "openning new file: " + filename<<endl;
	PPage result(new Page(filename));

	try {
		boost::iostreams::mapped_file_params  params;
		params.path = filename;
		params.flags = result->m_file->readwrite;
		result->m_file->open(params);
	} catch (std::runtime_error &ex) {
		std::string what = ex.what();
		throw utils::Exception::CreateAndLog(POSITION, ex.what());
	}
	if (!result->m_file->is_open())
		throw utils::Exception::CreateAndLog(POSITION, "can`t create file ");

	logger << "size: " << result->m_file->size()<<endl;

	return result;
}

Page::PPage Page::Create(std::string filename, size_t sizeInMbytes) {
	logger << "creating new file: " + filename<<endl;
	PPage result(new Page(filename));

	try {
		boost::iostreams::mapped_file_params  params;
		params.new_file_size = sizeInMbytes*oneMb;
		params.path = filename;
		params.flags = result->m_file->readwrite;
		result->m_file->open(params);
	}
	catch (std::runtime_error &ex) {
		std::string what = ex.what();
		throw utils::Exception::CreateAndLog(POSITION, ex.what());
	}

	if (!result->m_file->is_open())
		throw utils::Exception::CreateAndLog(POSITION, "can`t create file ");

	logger << "file size: " << result->m_file->size()<<endl;
	
	char * data = (char *)result->m_file->data();
	for (size_t i = 0; i < result->m_file->size(); i++) {
		data[i] = '*';
	}

	return result;
}
