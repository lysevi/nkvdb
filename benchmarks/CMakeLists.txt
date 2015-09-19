include_directories(..)

add_executable (read_write read_write.cpp)
TARGET_LINK_LIBRARIES(read_write libmdb ${Boost_LIBRARIES})

add_executable (thread_read_write thread_read_write.cpp)
TARGET_LINK_LIBRARIES(thread_read_write libmdb ${Boost_LIBRARIES})