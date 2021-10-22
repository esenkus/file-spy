# Declare the library
add_library(file-spy-lib domain/Observer.cpp domain/Observer.hpp util/Logger.cpp util/Logger.hpp)

# Specify here the include directories exported by this library
target_include_directories(file-spy-lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})