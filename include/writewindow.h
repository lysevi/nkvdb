#pragma once
#include <vector>
#include "meas.h"

namespace mdb
{
	class WriteWindow:public std::vector<Meas>
	{
	public:
		WriteWindow()=default;
		WriteWindow(WriteWindow::const_iterator begin, WriteWindow::const_iterator end);
		explicit WriteWindow(const size_t &size);
		WriteWindow(const WriteWindow& other);
		void operator= (const WriteWindow& other);
		~WriteWindow()=default;

		Meas operator[](const size_t pos)const;
		Meas&operator[](const size_t pos);
	};

}