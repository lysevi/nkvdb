#pragma once

#include <list>
#include <vector>
#include <common/Meas.h>

namespace utils
{
/*
Кеш для хранения текущего среза данных
*/
	class LinearCache
	{
	public:
		LinearCache(const common::IdList&ids);
		~LinearCache();
		void writeValue(const common::Meas &meas);
		common::Meas readValue(const common::Id id);
		size_t size()const;
	private:
		std::vector<common::Meas> m_meases;
	};
};