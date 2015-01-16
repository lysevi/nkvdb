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
		void writeValue(common::Meas::PMeas meas);
		void readValue(const common::Id id);
	private:
		std::vector<common::Meas::PMeas> m_meases;
	};
};