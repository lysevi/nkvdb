#include "writewindow.h"

using namespace storage;

WriteWindow::WriteWindow(const size_t &size): std::vector<storage::Meas>(size) {
}

WriteWindow::WriteWindow(const WriteWindow& other) : std::vector<storage::Meas>(other) {
}

WriteWindow::WriteWindow(WriteWindow::const_iterator begin, WriteWindow::const_iterator end) : std::vector<storage::Meas>(begin,end) {
}

Meas WriteWindow::operator[](const size_t pos)const {
	if (size() <= pos) {
		return Meas::empty();
	}
	return this->at(pos);
}

Meas&WriteWindow::operator[](const size_t pos) {
	if (size() <= pos) {
		this->resize(pos + 1);
	}
	return this->at(pos);
}