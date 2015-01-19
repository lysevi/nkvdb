#include "LinearCache.h"
#include <cassert>
#include <list>

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


void LinearCache::writeValue(const common::Meas::PMeas &meas) {
	m_meases[meas->id] = meas;
}

void LinearCache::writeValues(const common::Meas::MeasArray &meases){
    for(auto &m:meases){
        this->writeValue(m);
    }
}

common::Meas::PMeas LinearCache::readValue(const common::Id id){
	assert(id < m_meases.size());

	return m_meases.at(id);
}

common::Meas::MeasArray LinearCache::readValues(const common::IdArray& ids){
    common::Meas::MeasArray result;
    result.resize(ids.size());
    int i=0;
    for(auto id:ids){
        result[i]=(this->readValue(id));

        i++;
    }
    return result;
}

common::Meas::MeasArray LinearCache::readValuesInterval(const common::IdArray& ids, const common::Time from,const common::Time to){
    NOT_IMPLEMENTED;
}

common::Meas::MeasArray LinearCache::readValuesFltr(const common::IdArray& ids, const common::Time from,const common::Flag flg){
    std::list<common::Meas::PMeas> subRes;
    for(auto id:ids){
        auto m=(this->readValue(id));
        if(m->flag==flg)
            subRes.push_back(m);
    }

    int i=0;
    common::Meas::MeasArray res;
    res.resize(subRes.size());
    for(auto m:subRes){
        res[i]=m;
        i++;
    }
    return res;
}

common::Meas::MeasArray LinearCache::readValuesByDate(const common::IdArray& ids, const common::Time date){
    NOT_IMPLEMENTED;
}