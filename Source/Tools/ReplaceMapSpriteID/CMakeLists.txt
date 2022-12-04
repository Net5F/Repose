cmake_minimum_required(VERSION 3.16)

message(STATUS "Configuring ReplaceMapSpriteID")

add_executable(ReplaceMapSpriteID
    Private/ReplaceMapSpriteIDMain.cpp
)

target_include_directories(ReplaceMapSpriteID
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Private
)

target_link_libraries(ReplaceMapSpriteID
    PRIVATE
        Bitsery::bitsery
        AmalgamEngine::SharedLib
)

# Compile with C++20.
target_compile_features(ReplaceMapSpriteID PRIVATE cxx_std_20)
set_target_properties(ReplaceMapSpriteID PROPERTIES CXX_EXTENSIONS OFF)

# Enable compile warnings.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(ReplaceMapSpriteID PUBLIC -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(ReplaceMapSpriteID PUBLIC /W3 /permissive-)
endif()
