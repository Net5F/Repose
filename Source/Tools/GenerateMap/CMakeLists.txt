cmake_minimum_required(VERSION 3.16)

message(STATUS "Configuring GenerateMap")

add_executable(GenerateMap
    Private/MapGenerator.cpp
    Private/MapGenerator.h
    Private/GenerateMapMain.cpp
)

target_include_directories(GenerateMap
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(GenerateMap
    PRIVATE
        Bitsery::bitsery
        AmalgamEngine::SharedLib
)

# Compile with C++23.
target_compile_features(GenerateMap PRIVATE cxx_std_23)
set_target_properties(GenerateMap PROPERTIES CXX_EXTENSIONS OFF)

# Enable compile warnings.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(GenerateMap PUBLIC -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(GenerateMap PUBLIC /W3 /permissive-)
endif()
