cmake_minimum_required (VERSION 2.8)
project (mdb)

IF(WIN32)
        MESSAGE(STATUS "WIN32")
	set(BOOST_ROOT "d:\\soft\\boost_1_57_0\\boost_1_57_0")
        MESSAGE(STATUS "+ boost_root: " ${BOOST_ROOT})
	LINK_DIRECTORIES(${LINK_DIRECTORIES} "d:\\soft\\boost_1_57_0\\build")
        MESSAGE(STATUS "+ boost libs: " ${BOOST_ROOT})
else(WIN32)
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -Wwrite-strings -g -ftest-coverage -fprofile-arcs -Wsign-compare")
set(BOOST_ROOT "/home/lsv/soft/cpp/boost_1_57_0")
MESSAGE(STATUS "+ boost_root: " ${BOOST_ROOT})
ENDIF(WIN32)

#SET(Boost_USE_STATIC_LIBS OFF)
#SET(Boost_USE_MULTITHREAD ON)

find_package(Boost 1.57.0
    REQUIRED
    unit_test_framework
    system
)

IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

enable_testing()

add_subdirectory (common)
add_subdirectory (utils)
add_subdirectory (test)
add_executable (mdb main.cpp)
TARGET_LINK_LIBRARIES(mdb common)