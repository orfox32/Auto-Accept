#include "LockfileParser.h"

bool LockfileParser::parse(const std::string& lockfilePath,
                          std::string& outPort,
                          std::string& outAuthHeader) {
    std::ifstream lockFileStream(lockfilePath);
    if (!lockFileStream.is_open()) {
        Logger::log("Error: Unable to open lockfile.");
        return false;
    }

    std::string lockData;
    std::getline(lockFileStream, lockData);
    lockFileStream.close();

    std::vector<std::string> parts;
    size_t pos = 0;
    while ((pos = lockData.find(':')) != std::string::npos) {
        parts.push_back(lockData.substr(0, pos));
        lockData.erase(0, pos + 1);
    }
    parts.push_back(lockData);

    if (parts.size() != 5) {
        Logger::log("Error: Invalid lockfile format.");
        return false;
    }

    outPort = parts[2];
    std::string password = parts[3];

    std::string credentials = "riot:" + password;
    std::string encodedCredentials = Base64Encoder::encode(credentials);
    outAuthHeader = "Authorization: Basic " + encodedCredentials;

    return true;
}