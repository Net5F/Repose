cmake_minimum_required(VERSION 3.16)

message(STATUS "Configuring Shared")

# Note: We normally would build this as a static library target, but this 
#       library currently only has headers, thus we must make it an interface 
#       target. The lines for making it a static library are commented out.

# Add our static library target.
#add_library(Shared STATIC "")
add_library(Shared INTERFACE)

#target_link_libraries(Shared
#    PUBLIC
#        AmalgamEngine::SharedLib
#)

# Compile with C++23.
#target_compile_features(Shared PRIVATE cxx_std_23)
#set_target_properties(Shared PROPERTIES CXX_EXTENSIONS OFF)

# Enable compile warnings.
#if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
#    target_compile_options(Shared PUBLIC -Wall -Wextra)
#elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
#    target_compile_options(Shared PUBLIC /W3 /permissive-)
#endif()

# Build all of the subdirectories.
add_subdirectory(Messages)
add_subdirectory(Simulation)
