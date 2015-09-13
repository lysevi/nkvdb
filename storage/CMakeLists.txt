include_directories(..)
FILE(GLOB SRC "*.cpp"  "*.h" "utils/*.cpp" "utils/*.h")

if(MSVC)
	add_library(storage STATIC ${SRC})
else(MSVC)
	add_library(storage SHARED ${SRC})
	TARGET_LINK_LIBRARIES(storage ${Boost_LIBRARIES})
endif(MSVC)

