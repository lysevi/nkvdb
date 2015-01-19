include_directories(..)

add_library(mdb_test STATIC test_common.cpp test_common.h)

MESSAGE(STATUS "Test: " ${name})

macro(TEST_CASE name src)
MESSAGE(STATUS " +" ${name})
add_executable(${name} ${src})
TARGET_LINK_LIBRARIES(${name} mdb_test utils common ${ALL_LIBRARIES} ${Boost_LIBRARIES})
add_test(${name} ${name})
endmacro(TEST_CASE)

TEST_CASE(empty_test empty.cpp)
TEST_CASE(common_meas_test common_meas_test.cpp)
TEST_CASE(utils_linearcache_test utils_linearcache_test.cpp)
TEST_CASE(utils_externalcachememory_test utils_externalcachememory_test.cpp)
TEST_CASE(utils_datastorage_test utils_datastorage_test.cpp)


