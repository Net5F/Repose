cmake_minimum_required(VERSION 3.16)

message(STATUS "Configuring Client")

###############################################################################
# Configure Step
###############################################################################
add_executable(Client "")

target_link_libraries(Client
    PRIVATE
        AmalgamEngine::ClientLib
        Shared
)

# Compile with C++20.
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

# Copy UserConfig.json to the build directory.
file(COPY ${PROJECT_SOURCE_DIR}/Resources/Client/Common/UserConfig.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# Copy SpriteData.json to the build directory.
file(COPY ${PROJECT_SOURCE_DIR}/Resources/Shared/Common/SpriteData.json
     DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)

# On Windows, copy the SDL2 DLLs into the build folder so we can run our executable.
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    list(APPEND SDL2_DLL_PATHS 
         ${SDL2_DIR}/lib/x64/
         ${SDL2_IMAGE_INCLUDE_DIR}/../lib/x64
         ${SDL2_MIXER_INCLUDE_DIR}/../lib/x64
         ${SDL2_TTF_INCLUDE_DIR}/../lib/x64
         ${SDL2_GFX_INCLUDE_DIR}/../lib/x64
    )

    foreach(DLL_PATH ${SDL2_DLL_PATHS})
        file(GLOB SDL2_DLLS ${DLL_PATH}/*.dll)
        file(COPY ${SDL2_DLLS} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
    endforeach()
endif()

# Build all of the subdirectories.
add_subdirectory(Config)
add_subdirectory(Launch)
add_subdirectory(Network)
add_subdirectory(Renderer)
add_subdirectory(Simulation)
add_subdirectory(UserInterface)

###############################################################################
# Install Step (Packaging the application)
###############################################################################
# Define an install step that copies the executable and all resources.
install(TARGETS Client RUNTIME DESTINATION ${CMAKE_PROJECT_NAME}/Client)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Client/Common/
        DESTINATION ${CMAKE_PROJECT_NAME}/Client)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Shared/Common/
        DESTINATION ${CMAKE_PROJECT_NAME}/Client)
install(FILES ${PROJECT_SOURCE_DIR}/Resources/Client/Common/UserConfig.json
        DESTINATION ${CMAKE_PROJECT_NAME}/Client)
install(FILES ${PROJECT_SOURCE_DIR}/Resources/Shared/Common/SpriteData.json
        DESTINATION ${CMAKE_PROJECT_NAME}/Client)

# Add platform-specific files to the install step.
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Client/Windows/
            DESTINATION ${CMAKE_PROJECT_NAME}/Client)

    # On Windows, we provide the SDL2 DLLs.
    foreach(DLL_PATH ${SDL2_DLL_PATHS})
        install(DIRECTORY ${DLL_PATH}/
                DESTINATION ${CMAKE_PROJECT_NAME}/Client
                FILES_MATCHING PATTERN "*.dll")
    endforeach()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Client/Linux/
            DESTINATION ${CMAKE_PROJECT_NAME}/Client)
endif()
