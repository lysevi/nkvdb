include_directories(..)
FILE(GLOB SRC "*.cpp"  "*.h")
add_library(storage SHARED ${SRC})
TARGET_LINK_LIBRARIES(storage utils  ${Boost_LIBRARIES})
