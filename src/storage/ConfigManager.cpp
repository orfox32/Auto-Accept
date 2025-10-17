#include "ConfigManager.h"

using json = nlohmann::json;

bool ConfigManager::saveDirectory(const std::string& directory) {
    json config;
    config["directory"] = directory;

    std::ofstream configFile(Config::CONFIG_FILE);
    if (configFile.is_open()) {
        configFile << config.dump(4);
        configFile.close();
        Logger::log("Directory saved successfully.");
        return true;
    }

    Logger::log("Failed to save directory.");
    return false;
}

std::string ConfigManager::loadDirectory() {
    std::ifstream configFile(Config::CONFIG_FILE);
    if (configFile.is_open()) {
        json config;
        configFile >> config;
        configFile.close();

        if (config.contains("directory")) {
            return config["directory"];
        }
    }
    return "";
}
