#include "config.h"
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

void saveUserDirectory(const std::string& directory)
{
    json config;
    config["directory"] = directory;

    system("mkdir config");

    std::ofstream configFile("config/config.json");
    if (configFile.is_open())
    {
        configFile << config.dump(4);
        configFile.close();
        std::cout << "User directory saved successfully." << std::endl;
    }
    else
    {
        std::cerr << "Unable to save user directory." << std::endl;
    }
}

std::string readUserDirectory()
{
    std::ifstream configFile("config/config.json");
    if (configFile.is_open())
    {
        json config;
        configFile >> config;
        configFile.close();
        if (config.contains("directory"))
            return config["directory"];
    }
    return "";
}

std::string getUserDirectory()
{
    std::string directory;
    std::cout << "Enter the directory where League of Legends is installed: ";
    std::getline(std::cin, directory);
    return directory;
}