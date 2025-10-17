#pragma once
#include <string>
#include "../storage/FileUtils.h"
#include "../core/Logger.h"
#include "../core/Config.h"
#include "LockfileParser.h"

class LeagueClient {
public:
    static std::string findLockfile(const std::string& gameDirectory);
};
