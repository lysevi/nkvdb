#include "cache.h"

using namespace storage;

Cache::Cache(size_t size) :m_max_size(size), m_size(0) {
}


Cache::~Cache() {
}

bool Cache::isFull()const {
	return m_size == m_max_size;
}

void Cache::clear() {
	//std::lock_guard<std::mutex> lock(this->m_rw_lock);
	m_data.clear();
	m_size=0;
}

bool Cache::append(const Meas value) {
	//std::lock_guard<std::mutex> lock(this->m_rw_lock);
	if (!isFull()) {
		this->m_data[value.time].push_back(value);
		m_size++;
		return true;
	} else {
		return false;
	}
}

size_t Cache::append(const Meas::PMeas begin, const size_t size) {
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
		m_data[begin[i].time].push_back(Meas{ begin[i] });
		m_size++;
	}

	return to_write;
}

storage::Meas::MeasList Cache::readInterval(Time from, Time to) const {
	std::lock_guard<std::mutex> lock(this->m_rw_lock);
	Meas::MeasList result;
	for (auto it = m_data.begin(); it != m_data.end(); ++it) {
		if (utils::inInterval(from, to, it->first)) {
			for (auto m : it->second) {
				result.push_back(m);
			}
		}
	}
	return result;
}

storage::Meas::PMeas Cache::asArray()const {
	std::lock_guard<std::mutex> lock(this->m_rw_lock);
	Meas::PMeas result = new Meas[m_size];
	int i = 0;
	for (auto it = m_data.begin(); it != m_data.end(); ++it) {
		for (auto m : it->second) {
			result[i]=m;
			++i;
		}
	}
	return result;
}