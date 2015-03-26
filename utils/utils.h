#pragma once

#include <cstdint>
#include <vector>
#include <memory>
#include <string>
#include <stdexcept>

#define NOT_IMPLEMENTED throw std::logic_error("Not implemented");

namespace utils {

    class NonCopy {
    private:
        NonCopy(const NonCopy&) = delete;
        NonCopy& operator=(const NonCopy&) = delete;
    protected:
        NonCopy() = default;
    };

}