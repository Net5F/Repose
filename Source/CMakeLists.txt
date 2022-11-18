cmake_minimum_required(VERSION 3.13)

###############################################################################
# Modules
###############################################################################
# Configure shared.
add_subdirectory(Shared)

# Configure client.
add_subdirectory(Client)

# Configure server.
add_subdirectory(Server)

# Configure tools.
if (BUILD_TOOLS)
    add_subdirectory(Tools)
endif()
