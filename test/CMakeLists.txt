include_directories(..)

add_library(mdb_test STATIC test_common.cpp test_common.h)

MESSAGE(STATUS "Test: " ${name})

macro(TEST_CASE name src)
MESSAGE(STATUS " +" ${name})
add_executable(${name} ${src})
TARGET_LINK_LIBRARIES(${name} mdb_test storage utils ${ALL_LIBRARIES} ${Boost_LIBRARIES})
add_test(${name} ${name})
endmacro(TEST_CASE)



#TEST_CASE(empty_test empty.cpp)
TEST_CASE(utils_test utils_test.cpp)
TEST_CASE(storage_test storage_test.cpp)


