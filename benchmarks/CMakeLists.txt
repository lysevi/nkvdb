cmake_minimum_required (VERSION 2.8)
include_directories(..)
add_executable (read_write read_write.cpp)
TARGET_LINK_LIBRARIES(read_write storage utils  ${Boost_LIBRARIES})