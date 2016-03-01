include_directories(../include)

add_library(nkvdb_test STATIC test_common.cpp test_common.h)

MESSAGE(STATUS "Test: " ${name})

macro(TEST_CASE name src)
MESSAGE(STATUS " +" ${name})
add_executable(${name} ${src})
TARGET_LINK_LIBRARIES(${name} nkvdb_test nkvdb ${Boost_LIBRARIES})
add_test(${name} ${name})
endmacro(TEST_CASE)

TEST_CASE(utils_test utils_test.cpp)
TEST_CASE(storage_common_test storage_common_test.cpp)
TEST_CASE(btree_test btree_test.cpp)
TEST_CASE(storage_page_test storage_page_test.cpp)
TEST_CASE(storage_cache_test storage_cache_test.cpp)
TEST_CASE(storage_test storage_test.cpp)
TEST_CASE(storage_readinterval_test storage_readinterval_test.cpp)
TEST_CASE(storage_thread_test storage_thread_test.cpp)


