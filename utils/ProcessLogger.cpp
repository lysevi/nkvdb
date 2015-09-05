#include "ProcessLogger.h"
#include <iostream>

namespace logging = boost::log;

bool init_logger() {
  logging::core::get()->set_filter(logging::trivial::severity >=
                                   logging::trivial::info);

  return true;
}

static bool isLogInit = init_logger();
