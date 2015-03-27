#pragma once

#include <memory>
#include <vector>

#include <utils/utils.h>
#include "storage_export.h"

using namespace utils;

namespace storage
{
	typedef uint64_t Time;
	typedef uint64_t Id;
	typedef uint64_t Flag;
	typedef uint64_t Value;
	typedef std::vector<Id> IdArray;

	class _EXPORT Meas
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
