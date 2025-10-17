#pragma once
#include <string>
#include <fstream>
#include <vector>
#include "../core/Logger.h"
#include "../network/Base64Encoder.h"

class LockfileParser {
public:
    static bool parse(const std::string& lockfilePath,
                     std::string& outPort,
                     std::string& outAuthHeader);
};
