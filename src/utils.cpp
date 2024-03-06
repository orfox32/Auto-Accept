#include "utils.h"
#include <iostream>
#include <fstream>
#include <sys/stat.h>

std::string findLockFile(const std::string& gamedir)
{
    std::string lockfilePath = gamedir + "/lockfile";
    std::ifstream lockfile(lockfilePath);
    if (lockfile.good())
    {
        std::cout << "Found running League of Legends in directory: " << gamedir << std::endl;
        return lockfilePath;
    }
    return "";
}

bool isLockFileExists(const std::string& lockfilePath)
{
    struct stat buffer;
    return (stat(lockfilePath.c_str(), &buffer) == 0);
}