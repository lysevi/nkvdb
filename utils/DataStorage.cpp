#include "DataStorage.h"

using namespace utils;

DataStorage::DataStorage(LinearCache *lcache,IExternalCache*ecache)
:m_ecache(ecache),m_lcache(lcache) {
}


DataStorage::~DataStorage() {
}

