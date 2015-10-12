#include "page_manager.h"
#include "common.h"

#include "exception.h"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using namespace nkvdb;

PageManager *PageManager::m_instance=nullptr;

void PageManager::start(std::string path) {
	if (m_instance != nullptr) {
		throw MAKE_EXCEPTION("m_instance != nullptr");
	}
	PageManager::m_instance = new PageManager();
	m_instance->m_path = path;
}

void PageManager::stop() {
	delete m_instance;
	m_instance = nullptr;
}

PageManager* PageManager::get() {
	return m_instance;
}

CommonPage::Page_ptr PageManager::getCurPage() {
	return m_curpage;
}

void PageManager::closeCurrentPage() {
	if (m_curpage != nullptr) {
		m_curpage->close();
		m_curpage = nullptr;
	}
}

void PageManager::createNewPage() {
    WriteWindow wwindow;
    bool loaded=false;
	if (m_curpage != nullptr) {
        wwindow=m_curpage->getWriteWindow();
        loaded=true;
		m_curpage->close();
		m_curpage = nullptr;
	}

	std::string page_path = getNewPageUniqueName();

	m_curpage = Page::Create(page_path, this->default_page_size);
    if(loaded){
        m_curpage->setWriteWindow(wwindow);
    }
	m_page_list.push_back(page_path);
}

std::string PageManager::getOldesPage()const {
	return this->getOldesPage(this->pageList());
}
std::string PageManager::getOldesPage(const std::list<std::string> &pages)const {
	if (pages.size() == 1)
		return pages.front();
	fs::path maxTimePage;
	Time maxTime = 0;
	for (auto p : pages) {
		nkvdb::Page::Header hdr = nkvdb::Page::ReadHeader(p);
		Time cur_time = hdr.maxTime;
		if (maxTime < cur_time || cur_time == 0) {
			maxTime = cur_time;
			maxTimePage = p;
		}
	}
	if (maxTimePage.string().size() == 0) {
		throw utils::Exception::CreateAndLog(POSITION,
											 "open error. page not found.");
	}
	return maxTimePage.string();
}

std::string PageManager::getNewPageUniqueName()const {
	fs::path page_path;
	uint32_t suffix = 0;

	while (true) {
		if (page_path.string().length() != 0 && !fs::exists(page_path))
			break;

		page_path.clear();

		std::stringstream ss;
		ss << std::time(nullptr) << '_' << suffix << ".page";
		++suffix;
		page_path /= fs::path(m_path);
		page_path /= fs::path(ss.str());
	}

	return page_path.string();
}

std::list<std::string> PageManager::pageList() const {
	if (m_page_list.size() == 0) {
		auto page_list = utils::ls(m_path, ".page");

		for (auto it = page_list.begin(); it != page_list.end(); ++it) {
			m_page_list.push_back(it->string());
		}
	}
	return m_page_list;
}

CommonPage::Page_ptr PageManager::open(std::string path,bool readOnly) {
	m_curpage = Page::Open(path, readOnly);
    return m_curpage;
}

std::vector<PageManager::PageInfo> PageManager::pagesByTime()const {
	std::vector<PageManager::PageInfo> page_time_vector{};

	// read page list and sort them by time;
	auto page_list = PageManager::get()->pageList();
	
	page_time_vector.resize(page_list.size());
	int pos = 0;
	for (auto page : page_list) {
		nkvdb::Page::Header hdr = nkvdb::Page::ReadHeader(page);
		page_time_vector[pos]=PageManager::PageInfo{ hdr, page };
		pos++;
	}

	std::sort(page_time_vector.begin(),
			  page_time_vector.end(),
			  [](const PageManager::PageInfo&a, const PageManager::PageInfo&b){return a.header.maxTime > b.header.maxTime; });
	
	return page_time_vector;
}
