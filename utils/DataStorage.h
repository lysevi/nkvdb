
#pragma once
#include "LinearCache.h"
#include "IExternalCache.h"
#include <boost/noncopyable.hpp>

namespace utils {

    class DataStorage:public IExternalCache {
    public:
        DataStorage(LinearCache *lcache,IExternalCache*ecache);
        virtual ~DataStorage();

        void writeValues(const common::Meas::MeasArray &meases);
        common::Meas::MeasArray readValues(const common::IdArray& ids);
        common::Meas::MeasArray readValuesInterval(const common::IdArray& ids, const common::Time from,const common::Time to);
        common::Meas::MeasArray readValuesFltr(const common::IdArray& ids, const common::Time from,const common::Flag flg);
        common::Meas::MeasArray readValuesByDate(const common::IdArray& ids, const common::Time date);
    private:
        LinearCache   *m_lcache;
        IExternalCache*m_ecache;
    };

}