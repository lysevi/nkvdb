#include "time.h"

namespace storage {
    
namespace TimeWork {

    storage::Time CurrentUtcTime() {
        auto timestamp = std::chrono::system_clock::now();
        auto duration = timestamp.time_since_epoch();
        auto result = std::chrono::duration_cast<std::chrono::nanoseconds>(duration);
        return result.count();
    }
}
}
