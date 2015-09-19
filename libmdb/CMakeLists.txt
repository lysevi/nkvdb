include_directories(..)
FILE(GLOB SRC "*.cpp"  "*.h" "utils/*.cpp" "utils/*.h")

if(MSVC)
	add_library(libmdb STATIC ${SRC})
else(MSVC)
	add_library(libmdb SHARED ${SRC})
	TARGET_LINK_LIBRARIES(libmdb ${Boost_LIBRARIES})
endif(MSVC)

