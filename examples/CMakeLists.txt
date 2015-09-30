include_directories(../include)

add_executable (read_write_example read_write_example.cpp)
TARGET_LINK_LIBRARIES(read_write_example mdb ${Boost_LIBRARIES})