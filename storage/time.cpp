#include "time.h"
#include <chrono>

using namespace storage;


storage::Time TimeWork::CurrentUtcTime(){
    auto timestamp=std::chrono::system_clock::now();
    auto duration = timestamp.time_since_epoch();
    auto result = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
    return result.count();
}
