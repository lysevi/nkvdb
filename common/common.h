#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

#define NOT_IMPLEMENTED throw std::logic_error("Not implemented");

namespace common {
    typedef uint64_t Time;
    typedef uint64_t Id;
    typedef uint64_t Flag;
    typedef uint64_t Value;
    typedef std::vector<Id> IdArray;

    class NonCopy {
    private:
        NonCopy(const NonCopy&) = delete;
        NonCopy& operator=(const NonCopy&) = delete;
    protected:
        NonCopy() = default;
    };

}