include_directories(..)
include_directories(../include)
FILE(GLOB SRC "*.cpp"  "../include/*.h")

if(MSVC)
	add_library(libmdb STATIC ${SRC})
else(MSVC)
	add_library(libmdb SHARED ${SRC})
	TARGET_LINK_LIBRARIES(libmdb ${Boost_LIBRARIES})
endif(MSVC)

