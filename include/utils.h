#pragma once
#include <string>

std::string findLockFile(const std::string& gamedir);
bool isLockFileExists(const std::string& lockfilePath);
