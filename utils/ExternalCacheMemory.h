
#pragma once

#include "IExternalCache.h"

namespace utils {

    class ExternalCacheMemory:public IExternalCache {
    public:
        void writeValues(const common::Meas::MeasArray &meases);

        common::Meas::MeasArray readValues(const common::IdArray ids);
        common::Meas::MeasArray readValuesInterval(const common::IdArray ids, const common::Time from,const common::Time to);
        common::Meas::MeasArray readValuesFltr(const common::IdArray ids, const common::Time from,const common::Flag flg);
        common::Meas::MeasArray readValuesByDate(const common::IdArray ids, const common::Time date);

        virtual ~ExternalCacheMemory();
    private:

    };

}