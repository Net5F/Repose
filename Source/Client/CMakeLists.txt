cmake_minimum_required(VERSION 3.16)

message(STATUS "Configuring Client")

###############################################################################
# Configure Step
###############################################################################
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    add_executable(Client
        WIN32
        "${PROJECT_SOURCE_DIR}/Resources/Shared/Icons/appicon.rc"
    )
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    # Add the app icon to the app bundle
    set(CLIENT_ICON "${PROJECT_SOURCE_DIR}/Resources/Shared/Icons/icon.icns")
    set_source_files_properties(${CLIENT_ICON}
                                PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")

    # Add the PkgInfo file to the app bundle
    set(CLIENT_PKGINFO "${PROJECT_SOURCE_DIR}/Resources/Client/macOS/PkgInfo")
    set_source_files_properties(${CLIENT_PKGINFO}
                                PROPERTIES MACOSX_PACKAGE_LOCATION .)

    add_executable(Client
        MACOSX_BUNDLE
        ${CLIENT_ICON}
        ${CLIENT_PKGINFO}
    )

    # Reduce the amount of libraries bundled by telling the linker
    # to remove references to libraries which aren't used.
    target_link_options(
        Client
        PRIVATE
        "LINKER:-dead_strip,-dead_strip_dylibs"
    )

    # Configure the Info.plist file
    string(TIMESTAMP CurrentYear "%Y")
    set(MACOSX_BUNDLE_CATEGORY "public.app-category.games")
    set(MACOSX_BUNDLE_EXECUTABLE_NAME "Repose")
    set_target_properties(
      Client
      PROPERTIES OUTPUT_NAME "${CMAKE_PROJECT_NAME}"
                 MACOSX_BUNDLE TRUE
                 MACOSX_BUNDLE_BUNDLE_NAME "${CMAKE_PROJECT_NAME}"
                 MACOSX_BUNDLE_GUI_IDENTIFIER
                 "com.github.Net5F.${CMAKE_PROJECT_NAME}"
                 MACOSX_BUNDLE_COPYRIGHT
                 "Copyright © ${CurrentYear} Net5F. All rights reserved."
                 MACOSX_BUNDLE_INFO_STRING
                 "0.0.0, Copyright © ${CurrentYear} Net5F."
                 MACOSX_BUNDLE_BUNDLE_VERSION "0.0.0"
                 MACOSX_BUNDLE_SHORT_VERSION_STRING "0.0.0"
                 MACOSX_BUNDLE_ICON_FILE "icon.icns"
                 MACOSX_BUNDLE_INFO_PLIST
                 "${PROJECT_SOURCE_DIR}/CMake/MacOSXBundleInfo.plist.in"
    )
else()
    add_executable(Client)
endif()

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
if (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    install(TARGETS Client BUNDLE DESTINATION ${CMAKE_PROJECT_NAME}/Client)
    set(CLIENT_RESOURCES_FOLDER "${CMAKE_PROJECT_NAME}/Client/${CMAKE_PROJECT_NAME}.app/Contents/Resources")
else()
    install(TARGETS Client RUNTIME DESTINATION ${CMAKE_PROJECT_NAME}/Client)
    set(CLIENT_RESOURCES_FOLDER "${CMAKE_PROJECT_NAME}/Client")
endif()
install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Client/Common/
        DESTINATION ${CLIENT_RESOURCES_FOLDER})
install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Shared/Common/
        DESTINATION ${CLIENT_RESOURCES_FOLDER})
install(FILES ${PROJECT_SOURCE_DIR}/Resources/Client/Common/UserConfig.json
        DESTINATION ${CLIENT_RESOURCES_FOLDER})
install(FILES ${PROJECT_SOURCE_DIR}/Resources/Shared/Common/SpriteData.json
        DESTINATION ${CLIENT_RESOURCES_FOLDER})

# Add platform-specific files to the install step.
if (CMAKE_SYSTEM_NAME STREQUAL "Windows")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Client/Windows/
            DESTINATION ${CLIENT_RESOURCES_FOLDER})

    # On Windows, we provide the SDL2 DLLs.
    foreach(DLL_PATH ${SDL2_DLL_PATHS})
        install(DIRECTORY ${DLL_PATH}/
                DESTINATION ${CLIENT_RESOURCES_FOLDER}
                FILES_MATCHING PATTERN "*.dll")
    endforeach()
elseif (CMAKE_SYSTEM_NAME STREQUAL "Linux")
    install(DIRECTORY ${PROJECT_SOURCE_DIR}/Resources/Client/Linux/
            DESTINATION ${CLIENT_RESOURCES_FOLDER})
elseif (CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    install(FILES ${PROJECT_SOURCE_DIR}/Resources/Client/macOS/README.TXT
            DESTINATION "${CMAKE_PROJECT_NAME}/Client")
    # Bundle any libraries needed with fixup_bundle.
    install(CODE "
            include(BundleUtilities)
            fixup_bundle(\"\$\{CMAKE_INSTALL_PREFIX\}/${CMAKE_PROJECT_NAME}/Client/${CMAKE_PROJECT_NAME}.app\"\"\" \"\")
            ")
endif()
