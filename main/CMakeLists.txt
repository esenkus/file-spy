file(GLOB_RECURSE MAIN_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )

add_executable(main ${MAIN_SOURCE})

target_link_libraries(main file-spy-lib)