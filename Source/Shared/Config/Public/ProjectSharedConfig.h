#pragma once

#include "SharedConfig.h"

namespace AM
{

/**
 * Provides a way to modify the default SharedConfig settings.
 */
class ProjectSharedConfig
{
public:
    /**
     * Returns a SharedConfig, set to the project's desired settings.
     *
     * Note: The values that are set in this function will be available to 
     *       both the engine code and this project's code through the 
     *       SharedConfig class.
     *       Shared config is used in the client, server, and sprite editor.
     */
    static SharedConfig getEngineSettings();
};

} // End namespace AM
