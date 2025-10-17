#pragma once
#include <string>
#include <sys/stat.h>

class FileUtils {
public:
    static inline bool fileExists(const std::string& filepath) {
        struct stat buffer;
        return (stat(filepath.c_str(), &buffer) == 0);
    }
};
