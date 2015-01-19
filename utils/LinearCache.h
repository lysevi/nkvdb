#pragma once

#include <list>
#include <vector>
#include <common/Meas.h>
#include "ICache.h"

namespace utils
{

	class LinearCache: public ICache
	{
	public:
		LinearCache(const common::IdArray&ids);
		~LinearCache();

                void writeValue(const common::Meas::PMeas &meas);
                void writeValues(const common::Meas::MeasArray &meases);

				common::Meas::PMeas readValue(const common::Id id);
                common::Meas::MeasArray readValues(const common::IdArray &ids);


                common::Meas::MeasArray readValuesInterval(const common::IdArray& ids, const common::Time from,const common::Time to);
                common::Meas::MeasArray readValuesFltr(const common::IdArray &ids, const common::Time from,const common::Flag flg);
                common::Meas::MeasArray readValuesByDate(const common::IdArray &ids, const common::Time date);
                
		size_t size()const;
	private:
		std::vector<common::Meas::PMeas> m_meases;
	};
};