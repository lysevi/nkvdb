#include "LinearCache.h"

using namespace utils;



LinearCache::LinearCache(const common::IdList&ids){
	m_meases.resize(ids.size());
}


LinearCache::~LinearCache()
{}


void LinearCache::writeValue(common::Meas::PMeas meas) {
	NOT_IMPLEMENTED;
}

void LinearCache::readValue(const common::Id id) {
	NOT_IMPLEMENTED;
}