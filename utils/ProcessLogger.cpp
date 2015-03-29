#include "ProcessLogger.h"
#include <iostream>

ProcessLogger* ProcessLogger::m_instance;

ProcessLogger* ProcessLogger::get() {
    if (ProcessLogger::m_instance == nullptr) {
        ProcessLogger::m_instance = new ProcessLogger();
    }
    return m_instance;
}

std::ostream& ProcessLogger::GetStream() {
    return std::cerr;
}
