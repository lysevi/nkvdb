#pragma once
#include <string>
#include <cstdint>
#include "ILogWriter.h"

#define logger common::ProcessLogger::get()->GetStream()
using std::endl;

namespace common
{
	class ProcessLogger : public ILogWriter
	{
	public:
		static ProcessLogger* get();
		static void start();
		static void stop();
		virtual std::ostream& GetStream();
		void setLevel(uint16_t level) {
			m_level = level;
		}
		virtual ~ProcessLogger() {
		};
	private:
		uint16_t m_level;
		static ProcessLogger *m_instance;
		ProcessLogger() {
		};
	};
}
