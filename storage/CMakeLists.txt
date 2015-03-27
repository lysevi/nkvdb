include_directories(..)
FILE(GLOB SRC "*.cpp"  "*.h")
add_library(storage STATIC ${SRC})
