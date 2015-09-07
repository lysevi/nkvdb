#pragma once
#include <string>
#include <cstdint>
#include <iostream>

#define logger(msg)       utils::Logger::get()->GetStream()<<"   "<<msg<<std::endl
#define logger_fatal(msg) utils::Logger::get()->GetStream()<<"   "<<msg<<std::endl

namespace utils
{
	class Logger
	{
	public:
		static Logger* get();
		virtual std::ostream& GetStream();

		virtual ~Logger() {
		};
	private:
		uint16_t m_level;
		static Logger *m_instance;

		Logger() {
		};
	};
}