include_directories(..)
include_directories(../include)

FILE(GLOB SRC "*.cpp" "../include/*.h")
FILE(GLOB HDRS  "../include/*.h")

if(MSVC)
        add_library(mdb STATIC ${SRC})
else(MSVC)
	add_library(mdb SHARED ${SRC})
	TARGET_LINK_LIBRARIES(mdb ${Boost_LIBRARIES})
endif(MSVC)

export(PACKAGE mdb)

set_target_properties(mdb PROPERTIES PUBLIC_HEADER "${HDRS}")
 
install(TARGETS mdb
  EXPORT MDBTargets
  RUNTIME DESTINATION "${INSTALL_BIN_DIR}"
  LIBRARY DESTINATION "${INSTALL_LIB_DIR}"
  ARCHIVE DESTINATION "${INSTALL_LIB_DIR}/static"
  PUBLIC_HEADER DESTINATION "${INSTALL_INCLUDE_DIR}/mdb"
    COMPONENT dev)