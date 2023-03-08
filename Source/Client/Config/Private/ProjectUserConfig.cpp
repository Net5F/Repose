#include "ProjectUserConfig.h"
#include "Paths.h"
#include "Log.h"
#include "nlohmann/json.hpp"
#include <string>
#include <fstream>

namespace AM
{
namespace Client
{

ProjectUserConfig::ProjectUserConfig()
{
    // Open the file.
    std::string fullPath{Paths::BASE_PATH};
#ifdef __APPLE__
    fullPath += "../Resources/UserConfig.json";
#else
    fullPath += "UserConfig.json";
#endif
    std::ifstream workingFile(fullPath);
    if (!(workingFile.is_open())) {
        LOG_FATAL("Failed to open UserConfig.json");
    }

    // Parse the file into a json structure.
    nlohmann::json json;
    try {
        json = nlohmann::json::parse(workingFile, nullptr, true, true);
    } catch (nlohmann::json::exception& e) {
        LOG_FATAL("Failed to parse UserConfig.json: %s", e.what());
    }

    // Initialize our members.
    try {
        init(json);
    } catch (nlohmann::json::exception& e) {
        LOG_FATAL("%s", e.what());
    }
}

ProjectUserConfig& ProjectUserConfig::get()
{
    static ProjectUserConfig userConfig;
    return userConfig;
}

void ProjectUserConfig::init([[maybe_unused]] nlohmann::json& json) {}

} // End namespace Client
} // End namespace AM
