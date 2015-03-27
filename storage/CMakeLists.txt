include_directories(..)
FILE(GLOB SRC "*.cpp"  "*.h")
add_library(storage SHARED ${SRC})

DEFINE_EXPORT(storage)

#if(WIN32)
#GENERATE_EXPORT_HEADER(storage
#             BASE_NAME storage
#             EXPORT_MACRO_NAME _EXPORT
#             EXPORT_FILE_NAME storage.h
#             STATIC_DEFINE storage_BUILT_AS_STATIC
#)
#SET_TARGET_PROPERTIES (storage PROPERTIES DEFINE_SYMBOL  "COMPILING_DLL" )
#endif(WIN32)