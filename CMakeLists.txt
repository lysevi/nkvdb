cmake_minimum_required (VERSION 2.8)
project (mdb)

IF(WIN32)
        MESSAGE(STATUS "WIN32:")
	set(BOOST_ROOT "d:\\soft\\boost_1_57_0\\boost_1_57_0")
        MESSAGE(STATUS "+ boost root: " ${BOOST_ROOT})
	LINK_DIRECTORIES(${LINK_DIRECTORIES} "d:\\soft\\boost_1_57_0\\build")
	include (GenerateExportHeader)

macro(DEFINE_EXPORT libname)
GENERATE_EXPORT_HEADER(${libname}
             BASE_NAME ${libname}
             EXPORT_MACRO_NAME _EXPORT
             EXPORT_FILE_NAME ${libname}_export.h
             STATIC_DEFINE ${libname}_BUILT_AS_STATIC
)
SET_TARGET_PROPERTIES (${libname} PROPERTIES DEFINE_SYMBOL  "COMPILING_DLL" )
endmacro(DEFINE_EXPORT)

else(WIN32)
macro(DEFINE_EXPORT libname)
endmacro(DEFINE_EXPORT)
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_CXX_FLAGS} -Wwrite-strings -g -ftest-coverage -fprofile-arcs -Wsign-compare")
set(BOOST_ROOT "/home/lsv/soft/cpp/boost_1_57_0")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -Wall -Werror")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Werror")
MESSAGE(STATUS "+ boost_root: " ${BOOST_ROOT})
ENDIF(WIN32)

find_package(Boost 1.57.0
    REQUIRED
    unit_test_framework
    system
    iostreams
)

IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
ENDIF(Boost_FOUND)

enable_testing()

add_subdirectory (utils)
add_subdirectory (storage)
add_subdirectory (test)
add_executable (mdb main.cpp)
TARGET_LINK_LIBRARIES(mdb utils storage)