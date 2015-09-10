include_directories(..)
FILE(GLOB SRC "*.cpp"  "*.h")


if(MSVC)
	add_library(utils STATIC ${SRC})
else(MSVC)
	add_library(utils SHARED ${SRC})
	TARGET_LINK_LIBRARIES(utils  ${Boost_LIBRARIES})
endif(MSVC)
