file(GLOB_RECURSE LIB_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )

set(LIB_INCLUDES
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_SOURCE_DIR}/util/
        ${CMAKE_CURRENT_SOURCE_DIR}/domain/
        )

add_library(file-spy-lib STATIC ${LIB_SOURCE})

# Specify here the include directories exported by this library
target_include_directories(file-spy-lib PUBLIC ${LIB_INCLUDES})