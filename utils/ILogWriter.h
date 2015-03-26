#pragma once
#include <ostream>
namespace common
{
	class ILogWriter
	{
	public:
		virtual std::ostream& GetStream() = 0;
	};
}