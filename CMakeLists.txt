cmake_minimum_required (VERSION 2.8)
project (mdb)

IF(WIN32)
	set(BOOST_ROOT "d:\\ck11\\CK11\\Integration\\ThirdParty\\Boost")
ENDIF(WIN32)

find_package(Boost 1.53.0
    REQUIRED
)

add_subdirectory (Utils)
add_executable (mdb main.cpp)
TARGET_LINK_LIBRARIES(mdb utils)