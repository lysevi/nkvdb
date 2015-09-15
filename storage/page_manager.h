#pragma once

#include "page.h"
#include "utils/utils.h"
#include <string>
#include <list>

namespace storage {
    /**
    * Manage of pages.
    */
	class PageManager : utils::NonCopy
	{
		static PageManager *m_instance;
		PageManager() = default;
	public:
		struct PageInfo
		{
			Page::Header header;
			std::string  name;
		};

	public:
		static void start(std::string path);
		static void stop();
		static PageManager* get();

        /// get current openned page
		Page::Page_ptr getCurPage();
        void closeCurrentPage();
		void createNewPage();

		std::string getOldesPage()const;
		std::string getNewPageUniqueName()const;

		std::list<std::string> pageList() const;
        Page::Page_ptr open(std::string path);

		std::vector<PageManager::PageInfo> pagesByTime()const;
    protected:
        std::string getOldesPage(const std::list<std::string> &pages)const;
	public:
		uint64_t default_page_size;
	protected:
		std::string m_path;
		Page::Page_ptr m_curpage;
		
	};
}
