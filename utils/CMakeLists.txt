include_directories(..)
FILE(GLOB SRC "*.cpp"  "*.h")
add_library(utils STATIC ${SRC})