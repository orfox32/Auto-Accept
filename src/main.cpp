#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
#include <thread>
#include "config.h"
#include "http.h"
#include "utils.h"
#include "nlohmann/json.hpp"

using json = nlohmann::json;

int main()
{
    std::string userDirectory = readUserDirectory();

    std::string choice;
    if (!userDirectory.empty())
    {
        std::cout << "League of Legends directory found: " << userDirectory << std::endl;
        std::cout << "Do you want to use this directory? (yes/no): ";
        std::getline(std::cin, choice);
    }

    if (userDirectory.empty() || choice == "no")
    {
        userDirectory = getUserDirectory();
        saveUserDirectory(userDirectory);
    }

    std::string lockfilePath;
    int attempts = 0;
    while (lockfilePath.empty() && attempts < 10)
    {
        lockfilePath = findLockFile(userDirectory);
        if (lockfilePath.empty())
        {
            attempts++;
            std::cout << "Unable to find Lockfile. Attempt: " << attempts << "\r" << std::flush;
            std::this_thread::sleep_for(std::chrono::seconds(10));
        }
    }

    if (lockfilePath.empty())
    {
        std::cerr << "Error: Unable to find Lockfile." << std::endl;
        return 1;
    }

    std::ifstream lockFileStream(lockfilePath);
    if (!lockFileStream.is_open())
    {
        std::cerr << "Error: Unable to open lock file." << std::endl;
        return 1;
    }

    std::string lockdata;
    std::getline(lockFileStream, lockdata);
    lockFileStream.close();

    std::vector<std::string> lockDataParts;
    size_t pos = 0;
    while ((pos = lockdata.find(':')) != std::string::npos)
    {
        lockDataParts.push_back(lockdata.substr(0, pos));
        lockdata.erase(0, pos + 1);
    }
    lockDataParts.push_back(lockdata);

    if (lockDataParts.size() != 5)
    {
        std::cerr << "Error: Invalid lock data format." << std::endl;
        return 1;
    }

    std::string procname = lockDataParts[0];
    std::string pid = lockDataParts[1];
    std::string port = lockDataParts[2];
    std::string password = lockDataParts[3];
    std::string protocol = lockDataParts[4];

    std::string username = "riot";
    std::string userpass = username + ":" + password;
    std::string encodedUserpass = base64_encode(userpass);
    std::string authorization = "Authorization: Basic " + encodedUserpass;

    std::string summonerId;
    bool loggedIn = false;
    while (!loggedIn)
    {
        std::cout << "Checking login status..." << std::endl;
        try
        {

            if (!isLockFileExists(lockfilePath))
            {
                std::cerr << "Error: League of Legends client is not running." << std::endl;
                return 1;
            }

            if (!checkLoggedIn(port, authorization))
            {
                std::cout << "Waiting for login..." << std::endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }

            std::cout << "Login successful!" << std::endl;
            loggedIn = true;

            std::string phase;
            while (true)
            {
                std::string phaseUrl = "https://127.0.0.1:" + port + "/lol-gameflow/v1/gameflow-phase";
                std::string phaseResponse;
                if (sendHttpRequest(phaseUrl, authorization, phaseResponse, "GET"))
                {
                    try
                    {
                        json phaseJson = json::parse(phaseResponse);
                        phase = phaseJson.get<std::string>();
                        std::cout << "Current phase: " << phase << "\r" << std::flush;

                        if (phase == "ReadyCheck")
                        {
                            std::string acceptUrl = "https://127.0.0.1:" + port + "/lol-matchmaking/v1/ready-check/accept";
                            std::string acceptResponse;
                            if (!sendHttpRequest(acceptUrl, authorization, acceptResponse, "POST"))
                            {
                                std::cerr << "Failed to auto-accept match." << std::endl;
                            }
                        }
                        else if (phase == "ChampSelect")
                        {
                            std::cout << "ChampSelect phase detected. Waiting..." << "\r" << std::flush;
                        }
                        else if (phase == "InProgress")
                        {
                            std::cout << "Game is in progress. Exiting." << std::endl;
                            std::this_thread::sleep_for(std::chrono::seconds(10));
                            return 0;
                        }
                    }
                    catch (json::exception& e)
                    {
                        std::cerr << "Error parsing JSON response: " << e.what() << std::endl;
                    }
                }
                else
                {
                    std::cerr << "Failed to retrieve gameflow phase." << std::endl;
                }

                std::this_thread::sleep_for(std::chrono::seconds(2));

                if (!isLockFileExists(lockfilePath))
                {
                    std::cerr << "League of Legends client has been closed." << std::endl;
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    return 1;
                }
            }
        }
        catch (const std::exception& e)
        {
            std::cerr << "An error occurred: " << e.what() << std::endl;
        }
    }

    return 0;
}