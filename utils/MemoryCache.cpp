#include "MemoryCache.h"
#include <common/Exception.h>
#include <sstream>

//#include <iostream>
using namespace utils;

const size_t pmeasSize = sizeof(common::Meas);

MemoryCache::MemoryCache(size_t cacheSize, const common::IdArray& ids) :m_cacheSize(cacheSize), m_ids(ids) {
	m_mmatrix.resize(ids.size());

	size_t sz = cacheSize / pmeasSize;
	sz=sz / ids.size();
	//std::cout << "cacheSize:" << cacheSize << " ids.size:" << ids.size() << "sz: " << sz << std::endl;
	for (size_t i = 0; i < ids.size(); ++i) {
		m_mmatrix[i].resize(sz);
		for (size_t j = 0; j < sz; ++j) {
			m_mmatrix[i][j] = common::Meas::empty();
		}
		//std::cout << "i:" << i << " sz:" << sz << std::endl;
	}
}

void MemoryCache::writeValues(const common::Meas::MeasArray &meases){
    for(auto&m:meases){
		if (m->id >= m_mmatrix.size()) {
			std::stringstream ss;
			ss << "meas id[" << m->id << "]>=" << m_mmatrix.size();
			throw common::Exception::CreateAndLog(POSITION, ss.str());
		}
        m_mmatrix[m->id].push_back(m);
    }
}

common::Meas::MeasArray MemoryCache::readValues(const common::IdArray& ids){
    NOT_IMPLEMENTED;
}

common::Meas::MeasArray MemoryCache::readValuesInterval(const common::IdArray& ids, const common::Time from,const common::Time to){
    NOT_IMPLEMENTED;
}

common::Meas::MeasArray MemoryCache::readValuesFltr(const common::IdArray& ids, const common::Time from,const common::Flag flg){
    NOT_IMPLEMENTED;
}

common::Meas::MeasArray MemoryCache::readValuesByDate(const common::IdArray &ids, const common::Time date){
    NOT_IMPLEMENTED;
}

MemoryCache::~MemoryCache(){
	m_mmatrix.clear();
}