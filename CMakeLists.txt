cmake_minimum_required(VERSION 3.13)

project(Repose)

# Tell CMake where the CMake folder is.
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")

###############################################################################
# Options
###############################################################################
option(BUILD_TOOLS "Build tools, such as the Map Generator." OFF)

option(AM_BUILD_SPRITE_EDITOR "Build Amalgam Engine Sprite Editor." OFF)
option(AM_BUILD_TESTS "Build Amalgam Engine tests." OFF)

option(AM_THREADSANITIZER "Enable ThreadSanitizer data race detector." OFF)
if (AM_THREADSANITIZER)
    include("${PROJECT_SOURCE_DIR}/Libraries/AmalgamEngine/CMake/ThreadSanitizer.cmake")
endif()

option(AM_ADDRESSSANITIZER "Enable AddressSanitizer memory error detector." OFF)
if (AM_ADDRESSSANITIZER)
    include("${PROJECT_SOURCE_DIR}/Libraries/AmalgamEngine/CMake/AddressSanitizer.cmake")
endif()

###############################################################################
# Dependencies
###############################################################################
# Configure Amalgam::Client and Amalgam::Server.
set(AM_CONFIG_OVERRIDE_PATH
    "${PROJECT_SOURCE_DIR}/Source/ConfigOverrides" CACHE PATH 
    "Override the engine's default config files with the file at the provided path." FORCE)
add_subdirectory("${PROJECT_SOURCE_DIR}/Libraries/AmalgamEngine/"
                 "${PROJECT_BINARY_DIR}/Libraries/AmalgamEngine/")

###############################################################################
# Source
###############################################################################
# Configure our project source.
add_subdirectory(Source)
