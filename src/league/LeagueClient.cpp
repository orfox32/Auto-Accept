#include "LeagueClient.h"

std::string LeagueClient::findLockfile(const std::string& gameDirectory) {
    std::string lockfilePath = gameDirectory + Config::LOCKFILE_NAME;

    if (FileUtils::fileExists(lockfilePath)) {
        Logger::log("Found League client in: " + gameDirectory);
        return lockfilePath;
    }

    return "";
}
