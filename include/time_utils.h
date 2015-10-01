#pragma once

#include "meas.h"
#include <chrono>

namespace nkvdb {
namespace TimeWork {

    nkvdb::Time CurrentUtcTime();

    template <class Duration>
    nkvdb::Time fromDuration(Duration d){
        return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
    }
}
}
