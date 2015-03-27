cmake_minimum_required (VERSION 2.8)
project(rbtree)
FILE(GLOB SRC "*.cpp"  "*.h")
add_executable (rbtree ${SRC})