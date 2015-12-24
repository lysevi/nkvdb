include_directories(../include)

add_executable (read_write read_write.cpp)
TARGET_LINK_LIBRARIES(read_write nkvdb ${Boost_LIBRARIES})

add_executable (page_read_write page_read_write.cpp)
TARGET_LINK_LIBRARIES(page_read_write nkvdb ${Boost_LIBRARIES})

add_executable (thread_read_write thread_read_write.cpp)
TARGET_LINK_LIBRARIES(thread_read_write nkvdb ${Boost_LIBRARIES})
