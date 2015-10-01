#include "writewindow.h"

using namespace nkvdb;

WriteWindow::WriteWindow(const size_t &size): std::vector<nkvdb::Meas>(size) {
}

WriteWindow::WriteWindow(const WriteWindow& other) : std::vector<nkvdb::Meas>(other) {
}

WriteWindow::WriteWindow(WriteWindow::const_iterator begin, WriteWindow::const_iterator end) : std::vector<nkvdb::Meas>(begin,end) {
}

void WriteWindow::operator= (const WriteWindow& other) {
	if (other.size() == 0){
		if (this->size() != 0) {
			this->clear();
		}
		return;
	}
	clear();
	resize(other.size());
    for (size_t i = 0; i < other.size(); i++) {
		this->at(i) = other[i];
	}
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
