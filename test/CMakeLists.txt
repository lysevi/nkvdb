include_directories(..)

add_executable(empty_test empty.cpp)

TARGET_LINK_LIBRARIES(empty_test utils ${ALL_LIBRARIES} ${Boost_LIBRARIES})

add_test(empty_test empty_test)