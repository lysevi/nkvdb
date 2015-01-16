enable_testing()

include_directories(..)

add_executable(empty_test empty.cpp)
add_executable(common_meas_test common_meas_test.cpp)
add_executable(utils_linearcache_test utils_linearcache_test.cpp)

TARGET_LINK_LIBRARIES(empty_test utils ${ALL_LIBRARIES} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(common_meas_test common ${ALL_LIBRARIES} ${Boost_LIBRARIES})
TARGET_LINK_LIBRARIES(utils_linearcache_test common utils ${ALL_LIBRARIES} ${Boost_LIBRARIES})

add_test(empty_test empty_test)
add_test(common_meas common_meas_test)
add_test(utils_linearcache utils_linearcache_test)

