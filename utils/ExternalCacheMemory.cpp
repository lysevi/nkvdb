#include "ExternalCacheMemory.h"

using namespace utils;

void ExternalCacheMemory::writeValues(const common::Meas::MeasArray &meases){
    NOT_IMPLEMENTED;
}

common::Meas::MeasArray ExternalCacheMemory::readValues(const common::IdArray ids){
    NOT_IMPLEMENTED;
}

common::Meas::MeasArray ExternalCacheMemory::readValuesInterval(const common::IdArray ids, const common::Time from,const common::Time to){
    NOT_IMPLEMENTED;
}

common::Meas::MeasArray ExternalCacheMemory::readValuesFltr(const common::IdArray ids, const common::Time from,const common::Flag flg){
    NOT_IMPLEMENTED;
}

common::Meas::MeasArray ExternalCacheMemory::readValuesByDate(const common::IdArray ids, const common::Time date){
    NOT_IMPLEMENTED;
}

ExternalCacheMemory::~ExternalCacheMemory(){
}