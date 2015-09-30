include_directories(..)
include_directories(../include)

FILE(GLOB SRC "*.cpp" "../include/*.h")

if(MSVC)
        add_library(mdb STATIC ${SRC})
else(MSVC)
	add_library(mdb SHARED ${SRC})
	TARGET_LINK_LIBRARIES(mdb ${Boost_LIBRARIES})
endif(MSVC)

#install(TARGETS mdb LIBRARY DESTINATION ${CMAKE_INSTALL_PREFIX}/lib)
#install(FILES ${HDRS} DESTINATION ${CMAKE_INSTALL_PREFIX}/include)
