cmake_minimum_required(VERSION 3.16)

message(STATUS "Configuring Shared")

add_library(Shared STATIC "")

target_link_libraries(Shared
    PRIVATE
        AmalgamEngine::SharedLib
)

# Compile with C++20.
target_compile_features(Shared PRIVATE cxx_std_20)
set_target_properties(Shared PROPERTIES CXX_EXTENSIONS OFF)

# Enable compile warnings.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Shared PUBLIC -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(Shared PUBLIC /W3 /permissive-)
endif()

# Build all of the subdirectories
add_subdirectory(Config)
