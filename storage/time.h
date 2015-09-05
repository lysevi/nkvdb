#pragma once
#include "Meas.h"
#include <chrono>

namespace storage {
namespace TimeWork {

    storage::Time CurrentUtcTime();

    template <class Duration>
    storage::Time fromDuration(Duration d){
        return std::chrono::duration_cast<std::chrono::nanoseconds>(d).count();
    }
}
}
