#include "page_manager.h"
#include "common.h"

#include "utils/exception.h"

#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
using namespace storage;

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

Page::Page_ptr PageManager::getCurPage() {
	return m_curpage;
}

void PageManager::closeCurrentPage() {
	if (m_curpage != nullptr) {
		m_curpage->close();
		m_curpage = nullptr;
	}
}

void PageManager::createNewPage() {
	if (m_curpage != nullptr) {
		m_curpage->close();
		m_curpage = nullptr;
	}

	std::string page_path = getNewPageUniqueName();

	m_curpage = Page::Create(page_path, this->default_page_size);
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
		storage::Page::Header hdr = storage::Page::ReadHeader(p);
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
		ss << std::time(nullptr) << '_' << suffix;
		++suffix;
		page_path.append(m_path);
		page_path.append(ss.str() + ".page");
	}

	return page_path.string();
}

std::list<std::string> PageManager::pageList() const {
	auto page_list = utils::ls(m_path, ".page");

	std::list<std::string> result;
	for (auto it = page_list.begin(); it != page_list.end(); ++it) {
		result.push_back(it->string());
	}

	return result;
}

Page::Page_ptr PageManager::open(std::string path) {
	m_curpage = Page::Open(path);
    return m_curpage;
}

std::vector<PageManager::PageInfo> PageManager::pagesByTime()const {
	std::vector<PageManager::PageInfo> page_time_vector{};

	// read page list and sort them by time;
	auto page_list = PageManager::get()->pageList();
	for (auto page : page_list) {
		storage::Page::Header hdr = storage::Page::ReadHeader(page);
		page_time_vector.push_back(PageManager::PageInfo{ hdr, page });
	}

	std::sort(page_time_vector.begin(),
			  page_time_vector.end(),
			  [](const PageManager::PageInfo&a, const PageManager::PageInfo&b){return a.header.maxTime > b.header.maxTime; });
	
	return page_time_vector;
}