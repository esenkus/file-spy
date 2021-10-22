cmake_minimum_required(VERSION 3.20)
project(file_spy)

set(CMAKE_CXX_STANDARD 14)

# Download conan.cmake automatically if it doesn't exist.
if (NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
    file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/v0.15/conan.cmake" "${CMAKE_BINARY_DIR}/conan.cmake" STATUS statusList)

    list(GET statusList 0 ERRCODE)

    if (${ERRCODE} GREATER 0)
        file(REMOVE "${CMAKE_BINARY_DIR}/conan.cmake")
        list(GET statusList 1 ERRMSG)
        message(SEND_ERROR "Failed to download conan.cmake with error: ${ERRMSG}, cmake error code ${ERRCODE}")
    endif ()
endif ()

include(${CMAKE_BINARY_DIR}/conan.cmake)

# Enable conan's cmake_multi generator
set(CONAN_CMAKE_MULTI OFF)
# To make sure conan.cmake doesn't built multi then specify a build type that isn't empty i.e. -DCMAKE_BUILD_TYPE=Debug

enable_testing()

add_subdirectory(main)
add_subdirectory(file-spy-test)
add_subdirectory(file-spy-lib)
