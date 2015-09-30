#pragma once

#include "meas.h"
#include <chrono>

namespace mdb {
namespace TimeWork {

    mdb::Time CurrentUtcTime();

    template <class Duration>
    mdb::Time fromDuration(Duration d){
        return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
    }
}
}
