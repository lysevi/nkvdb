#include "LinearCache.h"
#include <cassert>

using namespace utils;

LinearCache::LinearCache(const common::IdList&ids){
	m_meases.resize(ids.size());
}

LinearCache::~LinearCache()
{
	m_meases.clear();
}

size_t LinearCache::size()const {
	return m_meases.size();
}


void LinearCache::writeValue(const common::Meas &meas) {
	m_meases[meas.id] = meas;
}

common::Meas LinearCache::readValue(const common::Id id){
	assert(id < m_meases.size());

	return m_meases.at(id);
}