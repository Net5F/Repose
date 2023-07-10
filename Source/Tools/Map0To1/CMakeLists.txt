cmake_minimum_required(VERSION 3.16)

message(STATUS "Configuring Map0To1")

add_executable(Map0To1
    Private/Map0To1Main.cpp
)

target_include_directories(Map0To1
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(Map0To1
    PRIVATE
        Bitsery::bitsery
        AmalgamEngine::SharedLib
)

# Compile with C++20.
target_compile_features(Map0To1 PRIVATE cxx_std_20)
set_target_properties(Map0To1 PROPERTIES CXX_EXTENSIONS OFF)

# Enable compile warnings.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Map0To1 PUBLIC -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(Map0To1 PUBLIC /W3 /permissive-)
endif()
