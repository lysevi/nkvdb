#pragma once

#include <cstdint>
#include <list>
#include <memory>
#include <stdexcept>

#define NOT_IMPLEMENTED throw std::exception("Not implemented");

namespace common
{
	typedef uint64_t              Time;
	typedef uint64_t              Id;
	typedef uint64_t              Flag;
	typedef uint64_t              Value;
	typedef std::list<Id> IdList;
}