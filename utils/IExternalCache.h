
#pragma once

#include <common/Meas.h>
#include <common/common.h>

namespace utils {

    class IExternalCache: common::NonCopy {
	public:
		typedef std::shared_ptr<IExternalCache> PExternalCache;
    public:
        virtual void writeValues(const common::Meas::MeasArray &meases) = 0;
        virtual common::Meas::MeasArray readValues(const common::IdArray& ids) = 0;
        virtual common::Meas::MeasArray readValuesInterval(const common::IdArray &ids, const common::Time from,const common::Time to) = 0;
        virtual common::Meas::MeasArray readValuesFltr(const common::IdArray& ids, const common::Time from,const common::Flag flg) = 0;
        virtual common::Meas::MeasArray readValuesByDate(const common::IdArray& ids, const common::Time date) = 0;
    };

}