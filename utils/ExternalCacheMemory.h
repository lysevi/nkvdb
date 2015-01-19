
#pragma once

#include "IExternalCache.h"
#include <vector>

namespace utils {

    class ExternalCacheMemory:public IExternalCache {
		typedef std::vector<common::Meas::MeasArray> MeasMatrix;
    public:
		ExternalCacheMemory(size_t cacheSize,const common::IdArray& ids);
        void writeValues(const common::Meas::MeasArray &meases);

        common::Meas::MeasArray readValues(const common::IdArray& ids);
        common::Meas::MeasArray readValuesInterval(const common::IdArray& ids, const common::Time from,const common::Time to);
        common::Meas::MeasArray readValuesFltr(const common::IdArray& ids, const common::Time from,const common::Flag flg);
        common::Meas::MeasArray readValuesByDate(const common::IdArray& ids, const common::Time date);

        virtual ~ExternalCacheMemory();
    private:
		size_t          m_cacheSize;
		common::IdArray m_ids;
		MeasMatrix      m_mmatrix;
    };

}