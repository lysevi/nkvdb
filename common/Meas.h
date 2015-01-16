#pragma once

#include <boost\noncopyable.hpp>

#include "common.h"

namespace common
{
	class Meas
	{
	public:
		/*typedef std::shared_ptr<Meas> PMeas;*/
	public:
		Meas();
		~Meas();
	public:
		Id   id;                     // идентификатор
		Time time;                   // время генерации
		Flag source;                 // битовые флаги для кодирования источник
		Flag flag;                   // флаги со всякой мишурой
		Value data;                  // сами данные
	};
}