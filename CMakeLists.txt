cmake_minimum_required(VERSION 3.13)

project(Repose VERSION 1.0.2)

# Tell CMake where the CMake folder is.
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")

###############################################################################
# Options
###############################################################################
option(BUILD_TOOLS "Build tools, such as the Map Generator." OFF)

option(AM_BUILD_RESOURCE_IMPORTER "Build Amalgam Engine Resource Importer." OFF)
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
add_subdirectory("${PROJECT_SOURCE_DIR}/Libraries/AmalgamEngine/"
                 "${PROJECT_BINARY_DIR}/Libraries/AmalgamEngine/")

###############################################################################
# Source
###############################################################################
# Configure our project source.
add_subdirectory(Source)
