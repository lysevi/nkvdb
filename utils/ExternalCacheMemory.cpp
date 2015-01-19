#include "ExternalCacheMemory.h"
//#include <iostream>
using namespace utils;

const size_t pmeasSize = sizeof(common::Meas);

ExternalCacheMemory::ExternalCacheMemory(size_t cacheSize, const common::IdArray& ids) :m_cacheSize(cacheSize), m_ids(ids) {
	m_mmatrix.resize(ids.size());

	size_t sz = cacheSize / pmeasSize;
	sz=sz / ids.size();
	//std::cout << "cacheSize:" << cacheSize << " ids.size:" << ids.size() << "sz: " << sz << std::endl;
	for (int i = 0; i < ids.size(); ++i) {
		m_mmatrix[i].resize(sz);
		for (int j = 0; j < sz; ++j) {
			m_mmatrix[i][j] = common::Meas::empty();
		}
		//std::cout << "i:" << i << " sz:" << sz << std::endl;
	}
}

void ExternalCacheMemory::writeValues(const common::Meas::MeasArray &meases){
    NOT_IMPLEMENTED;
}

common::Meas::MeasArray ExternalCacheMemory::readValues(const common::IdArray& ids){
    NOT_IMPLEMENTED;
}

common::Meas::MeasArray ExternalCacheMemory::readValuesInterval(const common::IdArray& ids, const common::Time from,const common::Time to){
    NOT_IMPLEMENTED;
}

common::Meas::MeasArray ExternalCacheMemory::readValuesFltr(const common::IdArray& ids, const common::Time from,const common::Flag flg){
    NOT_IMPLEMENTED;
}

common::Meas::MeasArray ExternalCacheMemory::readValuesByDate(const common::IdArray &ids, const common::Time date){
    NOT_IMPLEMENTED;
}

ExternalCacheMemory::~ExternalCacheMemory(){
	m_mmatrix.clear();
}