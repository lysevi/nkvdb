cmake_minimum_required (VERSION 2.8)
project (Ladybird)

set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(BOOST_ALL_DYN_LINK           ON)

if(NOT MSVC)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
endif(NOT MSVC)

IF(WIN32)
  MESSAGE(STATUS "WIN32:")
  MESSAGE(STATUS "+ boost root: " ${BOOST_ROOT})

  if(MSVC)
	find_package(Boost 1.57.0 REQUIRED unit_test_framework system  filesystem log )
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  /EHsc")
  else(MSVC)
        find_package(Boost 1.57.0 REQUIRED unit_test_framework system  filesystem log program_options)
  endif(MSVC)

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

  set(BoostUSESTATIC_LIBS OFF)
  
  find_package(Boost 1.57.0 REQUIRED unit_test_framework system  filesystem log program_options)
ENDIF(WIN32)

IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
else(Boost_FOUND)
  MESSAGE(FATAL_ERROR "boost not found")
  EXIT()
ENDIF(Boost_FOUND)

if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ftemplate-backtrace-limit=0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -pedantic-errors")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wwrite-strings")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wdeprecated-declarations")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ftest-coverage")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fprofile-arcs")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wsign-compare")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK=1")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -march=native -mtune=native -DNDEBUG")
endif(CMAKE_COMPILER_IS_GNUCXX)

enable_testing()

add_subdirectory (storage)
add_subdirectory (test)
add_subdirectory (benchmarks)
add_executable (Ladybird main.cpp)
TARGET_LINK_LIBRARIES(Ladybird storage ${Boost_LIBRARIES})
