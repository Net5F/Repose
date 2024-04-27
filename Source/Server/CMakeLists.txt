cmake_minimum_required(VERSION 3.16)

message(STATUS "Configuring Server")

###############################################################################
# Configure Step
###############################################################################
add_executable(Server
    "${PROJECT_SOURCE_DIR}/Resources/Shared/AppIcons/appicon.rc"
)

target_link_libraries(Server
    PRIVATE
        AmalgamEngine::ServerLib
        Shared
)

# Compile with C++20.
target_compile_features(Server PRIVATE cxx_std_23)
set_target_properties(Server PROPERTIES CXX_EXTENSIONS OFF)

# Enable compile warnings.
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(Server PUBLIC -Wall -Wextra)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(Server PUBLIC /W3 /permissive-)
endif()

# Copy UserConfig.json to the build directory.
file(COPY ${PROJECT_SOURCE_DIR}/Resources/Server/Common/UserConfig.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# Copy ResourceData.json to the build directory.
file(COPY ${PROJECT_SOURCE_DIR}/Resources/Shared/Common/ResourceData.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# Copy TileMap.bin to the build directory.
file(COPY ${PROJECT_SOURCE_DIR}/Resources/Server/Common/TileMap.bin
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# On Windows, copy the SDL2 DLL into the build folder so we can run our executable.
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    file(COPY ${SDL2_DIR}/lib/x64/SDL2.dll DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
endif()

# Build all of the subdirectories.
add_subdirectory(Config)
add_subdirectory(Launch)
add_subdirectory(Network)
add_subdirectory(Simulation)

###############################################################################
# Install Step (Packaging the application)
###############################################################################
# Define an install step that copies the executable and all resources.
install(TARGETS Server RUNTIME DESTINATION ${CMAKE_PROJECT_NAME}/Server)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Server/Common/
        DESTINATION ${CMAKE_PROJECT_NAME}/Server)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Shared/Common/
        DESTINATION ${CMAKE_PROJECT_NAME}/Server)
install(FILES ${PROJECT_SOURCE_DIR}/Resources/Server/Common/UserConfig.json
        DESTINATION ${CMAKE_PROJECT_NAME}/Server)
install(FILES ${PROJECT_SOURCE_DIR}/Resources/Shared/Common/ResourceData.json
        DESTINATION ${CMAKE_PROJECT_NAME}/Server)

# Add platform-specific files to the install step.
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Server/Windows/
            DESTINATION ${CMAKE_PROJECT_NAME}/Server)

    # On Windows, we provide the SDL2 DLL.
    install(FILES ${SDL2_DIR}/lib/x64/SDL2.dll
            DESTINATION ${CMAKE_PROJECT_NAME}/Server)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Server/Linux/
            DESTINATION ${CMAKE_PROJECT_NAME}/Server)
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Server/macOS/
            DESTINATION ${CMAKE_PROJECT_NAME}/Server)
endif()
