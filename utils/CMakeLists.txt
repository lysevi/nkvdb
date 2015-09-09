include_directories(..)
FILE(GLOB SRC "*.cpp"  "*.h")
add_library(utils SHARED ${SRC})
TARGET_LINK_LIBRARIES(utils  ${Boost_LIBRARIES})
