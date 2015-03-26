#include "ProcessLogger.h"
#include <iostream>

using namespace common;

ProcessLogger* ProcessLogger::m_instance;
ProcessLogger* ProcessLogger::get() {
	if (ProcessLogger::m_instance == nullptr) {
		ProcessLogger::m_instance = new ProcessLogger();
		ProcessLogger::m_instance->setLevel(0);
	}
	return m_instance;
}
std::ostream& ProcessLogger::GetStream() {
	return std::cout;
}
void ProcessLogger::start() {
	get();
}
void ProcessLogger::stop() {
	delete get();
}