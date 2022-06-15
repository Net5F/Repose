cmake_minimum_required(VERSION 3.16)

message(STATUS "Configuring Client")

add_executable(Client "")

target_link_libraries(Client
    PRIVATE
        Amalgam::ClientLib
)

# Compile with C++20
target_compile_features(Client PRIVATE cxx_std_20)
set_target_properties(Client PROPERTIES CXX_EXTENSIONS OFF)

# Enable compile warnings.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Client PUBLIC -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(Client PUBLIC /W3 /permissive-)
endif()

# Copy assets to the build directory.
file(COPY ${PROJECT_SOURCE_DIR}/Resources/Client/Common/Assets/
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/Assets/)

# Copy SpriteData.json to the build directory.
file(COPY ${PROJECT_SOURCE_DIR}/Resources/Shared/Common/SpriteData.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# Build all of the subdirectories
add_subdirectory(Launch)
