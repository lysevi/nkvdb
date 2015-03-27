#pragma once
#include <string>
#include <cstdint>
#include <iostream>
#include <boost/log/trivial.hpp>

#define logger ProcessLogger::get()->GetStream()

using std::endl;

class ProcessLogger{
public:
  static ProcessLogger* get();
  virtual std::ostream& GetStream();

  virtual ~ProcessLogger() {
  };
private:
  uint16_t m_level;
  static ProcessLogger *m_instance;

  ProcessLogger() {
  };
};

