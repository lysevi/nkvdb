include_directories(..)
include_directories(../include)

FILE(GLOB SRC "*.cpp" "../include/*.h" "../include/*.hpp")
FILE(GLOB HDRS  "../include/*.h" "../include/*.hpp")

if(MSVC)
        add_library(nkvdb STATIC ${SRC})
else(MSVC)
	add_library(nkvdb SHARED ${SRC})
	TARGET_LINK_LIBRARIES(nkvdb ${Boost_LIBRARIES})
endif(MSVC)

export(PACKAGE nkvdb)

set_target_properties(nkvdb PROPERTIES PUBLIC_HEADER "${HDRS}")
 
install(TARGETS nkvdb
  EXPORT nkvdbTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}/static"
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/nkvdb"
  COMPONENT dev)