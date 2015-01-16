#pragma once

#include <cstdint>
#include <memory>
#include <boost\noncopyable.hpp>

namespace common
{
	class Meas: boost::noncopyable
	{
		Meas() = delete;
		~Meas() = delete;
	public:
		uint64_t id;
		uint64_t time;
		uint64_t flag;
		uint64_t size;
		std::shared_ptr<void*> data;
	};
}