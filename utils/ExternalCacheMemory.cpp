#include "ExternalCacheMemory.h"

using namespace utils;

ExternalCacheMemory::ExternalCacheMemory(size_t cacheSize, const common::IdArray& ids) :m_cacheSize(cacheSize), m_ids(ids) {
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
}