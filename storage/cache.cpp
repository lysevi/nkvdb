#include "cache.h"


using namespace storage;

Cache::Cache(size_t size) :m_max_size(size), m_size(0), m_index(0) {
	m_meases=new Meas[size];
}


Cache::~Cache() {
	delete[] m_meases;
}

bool Cache::isFull()const {
	return m_size == m_max_size;
}

void Cache::clear() {
//	m_data.clear();
	m_size=0;
	m_index = 0;
}

bool Cache::append(const Meas value) {
	//std::lock_guard<std::mutex> lock(this->m_rw_lock);
	if (!isFull()) {
		m_meases[m_index] = value;
		//this->m_data[value.time].push_back(m_index);
		m_index++;
		m_size++;
		return true;
	} else {
		return false;
	}
}

size_t Cache::append(const Meas::PMeas begin, const size_t size) {
	//std::lock_guard<std::mutex> lock(this->m_rw_lock);
	size_t cap = this->m_max_size;
	size_t to_write = 0;
	if (cap > size) {
		to_write = size;
	} else if (cap == size) {
		to_write = size;
	} else if (cap < size) {
		to_write = cap;
	}

	for (auto i = 0; i < to_write; ++i) {
		//m_data[begin[i].time].push_back(m_index);
		m_meases[m_index] = Meas{ begin[i] };
		m_size++;
		m_index++;
	}

	return to_write;
}

storage::Meas::MeasList Cache::readInterval(Time from, Time to) const {
	//std::lock_guard<std::mutex> lock(this->m_rw_lock);
	Meas::MeasList result;
	for (auto i = 0; i<m_index; ++i) {
		if (utils::inInterval(from, to, m_meases[i].time)) {
			result.push_back(m_meases[i]);
		}
	}
	return result;
}

void Cache::asArray(Meas::PMeas output)const {
	//std::lock_guard<std::mutex> lock(this->m_rw_lock);
	//int i = 0;
	/*for (auto it = m_data.begin(); it != m_data.end(); ++it) {
		for (auto m : it->second) {
			result[i]=m_meases[m];
			++i;
		}
	}*/

	for (auto i = 0; i<m_index; ++i) {
		output[i] = m_meases[i];
	}
}