#include "logger.h"

#include <iostream>

namespace utils
{
	Logger* Logger::m_instance;

	Logger* Logger::get() {
		if (Logger::m_instance == nullptr) {
			Logger::m_instance = new Logger();
		}
		return m_instance;
	}

	std::ostream& Logger::GetStream() {
		return std::cerr;
	}
}