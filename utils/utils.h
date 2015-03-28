#pragma once

#include <string>
#include <list>

#include <boost/filesystem.hpp>

#define NOT_IMPLEMENTED throw std::logic_error("Not implemented");

namespace utils {

    class NonCopy {
    private:
        NonCopy(const NonCopy&) = delete;
        NonCopy& operator=(const NonCopy&) = delete;
    protected:
        NonCopy() = default;
    };

    std::list<boost::filesystem::path> ls(const std::string& path);
}
