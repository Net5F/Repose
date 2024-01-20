cmake_minimum_required(VERSION 3.13)

###############################################################################
# Modules
###############################################################################
# Configure EngineSupplement (directly adds to the engine's targets).
add_subdirectory(EngineSupplement)

# Configure Shared.
add_subdirectory(Shared)

# Configure Client.
add_subdirectory(Client)

# Configure Server.
add_subdirectory(Server)

# Configure tools.
if (BUILD_TOOLS)
    add_subdirectory(Tools)
endif()
