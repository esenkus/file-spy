cmake_minimum_required(VERSION 3.10)

set(conan_packages
        gtest/1.11.0
        )
set(conan_options
        gtest:shared=False
        )

conan_cmake_run(REQUIRES ${conan_packages}
        BASIC_SETUP CMAKE_TARGETS
        BUILD missing
        OPTIONS ${conan_options}
        )

file(GLOB_RECURSE TEST_SOURCE
        ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
        )

add_executable(file-spy-test ${TEST_SOURCE})
target_link_libraries(file-spy-test file-spy-lib CONAN_PKG::gtest)