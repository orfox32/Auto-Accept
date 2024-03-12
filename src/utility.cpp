#include "utility.h"
#include "base64.h"
#include "league_client.h"
#include "http_request.h"
#include <iostream>
#include <string>
#include <regex>
#include <chrono>
#include <thread>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <windows.h>
#include <conio.h>

using namespace std;
using json = nlohmann::json;

bool extractAndEncodeRiotPassword(DWORD processId, string &port, string &encodedPassword)
{

    string command = "wmic process where 'ProcessId=" + to_string(processId) + "' get Commandline";

    string result;
    FILE *pipe = _popen(command.c_str(), "r");
    if (!pipe)
    {
        cerr << "Error: Could not execute command." << endl;
        return false;
    }
    char buffer[128];
    while (!feof(pipe))
    {
        if (fgets(buffer, 128, pipe) != nullptr)
            result += buffer;
    }
    _pclose(pipe);

    regex portRegex("--app-port=\"?(\\d+)\"?");
    regex authTokenRegex("--remoting-auth-token=([a-zA-Z0-9_-]+)");
    smatch portMatch, authTokenMatch;

    if (regex_search(result, portMatch, portRegex) && regex_search(result, authTokenMatch, authTokenRegex))
    {
        if (portMatch.size() > 1 && authTokenMatch.size() > 1)
        {
            port = portMatch.str(1);
            string authToken = authTokenMatch.str(1);

            string auth = "riot:" + authToken;
            encodedPassword = base64_encode(auth);
            return true;
        }
    }
    return false;
}

int getConsoleWidth() {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    return csbi.srWindow.Right - csbi.srWindow.Left + 1;
}

void centerText(const string &text){
    int consoleWidth = getConsoleWidth();
    int textLength = text.length();
    int leftPadding = (consoleWidth - textLength) / 2;
    cout << setw(leftPadding + textLength) << right << text << "\r" << std::flush;  
}

bool handleGamePhases(const string &port, const string &authorization, int retryDelaySeconds, bool loggedIn)
{

    while (loggedIn)
    {
        if (!LeagueClientIsOpen())
        {
            cerr << "League client has been closed. Exiting..." << endl;
            return false;
        }

        string phaseUrl = "https://127.0.0.1:" + port + "/lol-gameflow/v1/gameflow-phase";
        string phaseResponse;
        if (sendHttpRequest(phaseUrl, authorization, phaseResponse, "GET", retryDelaySeconds))
        {
            try
            {
                json phaseJson = json::parse(phaseResponse);
                string phase = phaseJson.get<string>();

                if (phase == "ChampSelect")
                {
                    cout << "ChampSelect phase detected. Waiting..." << "\r" << std::flush;
                    while (phase == "ChampSelect")
                    {
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        if (!LeagueClientIsOpen())
                        {
                            cerr << "League client has been closed. Exiting..." << endl;
                            return false;
                        }
                        string phaseResponse;
                        if (sendHttpRequest(phaseUrl, authorization, phaseResponse, "GET", retryDelaySeconds))
                        {
                            json phaseJson = json::parse(phaseResponse);
                            phase = phaseJson.get<string>();
                        }
                        else
                        {
                            cerr << "Failed to retrieve gameflow phase." << endl;
                            break;
                        }
                    }
                }
                else
                {
                    cout << setw(100) << left << " " << "\r" << std::flush;
                    cout << "Current phase: " << phase << "\r" << std::flush;
                }

                if (phase == "ReadyCheck")
                {
                    string acceptUrl = "https://127.0.0.1:" + port + "/lol-matchmaking/v1/ready-check/accept";
                    string acceptResponse;
                    if (!sendHttpRequest(acceptUrl, authorization, acceptResponse, "POST", retryDelaySeconds))
                    {
                        cerr << "Failed to auto-accept match." << endl;
                    }
                }
            }
            catch (json::exception &e)
            {
                cerr << "Error parsing JSON response: " << e.what() << endl;
            }
        }
        else
        {
            cerr << "Failed to retrieve gameflow phase." << endl;
        }

        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    return false;
}

bool authenticateAndAutoAccept(const string &port, const string &authorization, int retryDelaySeconds)
{

    bool loggedIn = false;

    while (!loggedIn)
    {
        cout << setw(100) << left << " " << "\r" << std::flush;
        centerText("Checking login status...");
        std::this_thread::sleep_for(std::chrono::seconds(5));

        try
        {
            if (!checkLoggedIn(port, authorization, retryDelaySeconds))
            {
                cout << "Waiting for login..." << endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }
            
            loggedIn = true;
            cout << setw(100) << left << " " << "\r" << std::flush;
            centerText("Login successful!");
            std::this_thread::sleep_for(std::chrono::seconds(5));
            
            string summonerData;
            if (getSummonerData(port, authorization, summonerData, retryDelaySeconds))
            {
                try
                {

                    auto summonerJson = json::parse(summonerData);
                    string gameName = std::regex_replace(summonerJson["gameName"].dump(), std::regex("\""), "");
                    string tagLine = std::regex_replace(summonerJson["tagLine"].dump(), std::regex("\""), "");
                    centerText("Log in as: [" + gameName + ":" + tagLine + "]");
                    cout << "\n";
                }
                catch (const json::exception &e)
                {
                    cerr << "Error parsing summoner data JSON: " << e.what() << endl;
                }
            }
            else
            {
                cerr << "Failed to retrieve summoner data." << endl;
            }

            handleGamePhases(port, authorization, retryDelaySeconds, loggedIn);
        }
        catch (const std::exception &e)
        {
            cerr << "An error occurred: " << e.what() << endl;
            return false;
        }
    }
    return false;
}