include_directories(..)

add_executable (read_write_example read_write_example.cpp)
TARGET_LINK_LIBRARIES(read_write_example storage ${Boost_LIBRARIES})