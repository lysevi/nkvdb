cmake_minimum_required (VERSION 2.8)
project (Ladybird)

IF(WIN32)
  MESSAGE(STATUS "WIN32:")
  set(BOOST_ROOT "d:\\soft\\boost_1_57_0\\boost_1_57_0")
  MESSAGE(STATUS "+ boost root: " ${BOOST_ROOT})

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

  set(CMAKE_CXX_FLAGS "-std=c++11 ${CMAKE_CXX_FLAGS} -Wwrite-strings -Wdeprecated-declarations -g -ftest-coverage -fprofile-arcs -Wsign-compare -fpermissive -Wall -pthread")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DDEBUG -Wall -Werror")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Werror  -pthread")

  #set(BOOST_ROOT "/home/lsv/soft/cpp/boost_1_57_0")

  #LINK_DIRECTORIES(${LINK_DIRECTORIES} "/mnt/data/soft/cpp/boost_1_57_0/stage/lib")
  set(BoostUSESTATIC_LIBS OFF)
ENDIF(WIN32)


set(Boost_USE_STATIC_LIBS        OFF)
set(Boost_USE_MULTITHREADED      ON)
set(Boost_USE_STATIC_RUNTIME     OFF)
set(BOOST_ALL_DYN_LINK           ON)

find_package(Boost 1.57.0 REQUIRED unit_test_framework system thread iostreams filesystem log program_options)

IF(Boost_FOUND)
  INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIRS})
  LINK_DIRECTORIES(${Boost_LIBRARY_DIRS})
else(Boost_FOUND)
  MESSAGE(FATAL_ERROR "boost not found")
  EXIT()
ENDIF(Boost_FOUND)

if("${CMAKE_CXX_COMPILER_ID}" MATCHES "Clang")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ftemplate-backtrace-limit=0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Weverything -Werror -pedantic-errors")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=deprecated-declarations -Wno-error=deprecated")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-c++98-compat -Wno-c++98-compat-pedantic")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-return-stack-address")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-variable -Wno-unused-parameter")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-weak-vtables -Wno-padded")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-global-constructors -Wno-exit-time-destructors")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shorten-64-to-32 -Wno-sign-conversion")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-prototypes -Wno-missing-variable-declarations")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-shadow -Wno-old-style-cast")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-documentation -Wno-documentation-unknown-command")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK=1")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -march=native -mtune=native -DNDEBUG")
elseif(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -ftemplate-backtrace-limit=0")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic -Werror -pedantic-errors")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error=deprecated-declarations")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-missing-field-initializers")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-unused-parameter")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DBOOST_LOG_DYN_LINK=1")
  set(CMAKE_CXX_FLAGS_DEBUG "-O0 -fno-inline -g3 -fstack-protector-all")
  set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -g0 -march=native -mtune=native -DNDEBUG")
elseif (MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  /EHsc")
endif()

enable_testing()

add_subdirectory (utils)
add_subdirectory (storage)
add_subdirectory (test)
add_subdirectory (benchmarks)
add_executable (Ladybird main.cpp)
TARGET_LINK_LIBRARIES(Ladybird utils storage ${Boost_LIBRARIES})
