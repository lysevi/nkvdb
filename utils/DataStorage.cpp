#include "DataStorage.h"

using namespace utils;

DataStorage::DataStorage(LinearCache *lcache,IExternalCache*ecache)
:m_ecache(ecache),m_lcache(lcache) {
}


DataStorage::~DataStorage() {
}

void DataStorage::writeValues(const common::Meas::MeasArray &meases){
    m_lcache->writeValues(meases);
    m_ecache->writeValues(meases);
}

common::Meas::MeasArray DataStorage::readValues(const common::IdArray& ids){
    return m_lcache->readValues(ids);
}

common::Meas::MeasArray DataStorage::readValuesInterval(const common::IdArray& ids, const common::Time from,const common::Time to){
    return m_ecache->readValuesInterval(ids,from,to);
}

common::Meas::MeasArray DataStorage::readValuesFltr(const common::IdArray& ids, const common::Time from,const common::Flag flg){
    return m_ecache->readValuesFltr(ids,from,flg);
}

common::Meas::MeasArray DataStorage::readValuesByDate(const common::IdArray& ids, const common::Time date){
    return m_ecache->readValuesByDate(ids,date);
}