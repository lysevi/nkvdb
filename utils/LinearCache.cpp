#include "LinearCache.h"
#include <cassert>

using namespace utils;

LinearCache::LinearCache(const common::IdArray&ids){
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

void LinearCache::writeValues(const common::Meas::MeasArray &meases){
    for(auto m:meases){
        this->writeValue(m);
    }
}

common::Meas LinearCache::readValue(const common::Id id){
	assert(id < m_meases.size());

	return m_meases.at(id);
}

common::Meas::MeasArray LinearCache::readValues(const common::IdArray ids){
    common::Meas::MeasArray result;
    result.resize(ids.size());
    int i=0;
    for(auto id:ids){
        result[i]=(this->readValue(id));
        i++;
    }
    return result;
}