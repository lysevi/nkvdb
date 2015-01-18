#pragma once

#include <list>
#include <vector>
#include <common/Meas.h>

namespace utils
{
/*
��� ��� �������� �������� ����� ������
*/
	class LinearCache
	{
	public:
		LinearCache(const common::IdArray&ids);
		~LinearCache();

                void writeValue(const common::Meas &meas);
                void writeValues(const common::Meas::MeasArray &meases);

		common::Meas readValue(const common::Id id);
                common::Meas::MeasArray readValues(const common::IdArray ids);
		size_t size()const;
	private:
		std::vector<common::Meas> m_meases;
	};
};