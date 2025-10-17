#pragma once
#include <string>
#include <fstream>
#include <nlohmann/json.hpp>
#include "../core/Logger.h"
#include "../core/Config.h"

class ConfigManager {
public:
    static bool saveDirectory(const std::string& directory);
    static std::string loadDirectory();
};
