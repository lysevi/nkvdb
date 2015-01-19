#pragma once

#include "common.h"
#include <list>

namespace common
{
	class Meas
	{
	public:
		typedef std::shared_ptr<Meas> PMeas;
        typedef std::vector<PMeas> MeasArray;
	public:
		Meas();
		~Meas();

		static Meas::PMeas empty();
	public:
		Id   id;
		Time time;
		Flag source;
		Flag flag;  
		Value data; 
	};
}
