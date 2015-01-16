#pragma once

#include <boost\noncopyable.hpp>

#include "common.h"

namespace common
{
	class Meas: public boost::noncopyable
	{
	public:
		typedef std::shared_ptr<Meas> PMeas;
	public:
		Meas();
		~Meas();

		static PMeas empty();
	public:
		Id   id;                     // идентификатор
		Time time;                   // время генерации
		Flag source;                 // битовые флаги для кодирования источник
		Flag flag;                   // флаги со всякой мишурой
		size_t size;                 // размер данных
		std::shared_ptr<void*> data; // сами данные
	};
}