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
#include <fstream>
#include <windows.h>

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

void centerText(const string& text) {
    int consoleWidth = getConsoleWidth();
    int textLength = text.length();
    int leftPadding = (consoleWidth - textLength) / 2;
    cout << setw(leftPadding + textLength) << right << text << "\r" << std::flush;
}

void toggleAutoAccept(bool &autoAcceptOn, json &config)
{
    HANDLE hConsoleInput;
    INPUT_RECORD irInput;
    DWORD dwEvents;
    INPUT_RECORD irKey;

    hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);

    HANDLE hConsoleOutput;
    DWORD dwWritten;
    COORD position; 

    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    position.X = 1;
    position.Y = 13; 

    string statusMessage;
    if (autoAcceptOn) {
        statusMessage = "On";
    } else {
        statusMessage = "Off";
    }

    string message = "Auto Accept: " + statusMessage + " (Press Up arrow key to On and Down arrow key to Off. Then press Enter Key. R to edit choice)";

    do
    {
        SetConsoleCursorPosition(hConsoleOutput, position);

        int consoleWidth = getConsoleWidth();

        int messageWidth = message.length();

        int leftPadding = (consoleWidth - messageWidth) / 2;

        cout << setw(leftPadding + messageWidth) << right << message << "\r" << std::flush;


        ReadConsoleInput(hConsoleInput, &irKey, 1, &dwEvents);
        if (irKey.EventType == KEY_EVENT && irKey.Event.KeyEvent.bKeyDown)
        {
            if (irKey.Event.KeyEvent.wVirtualKeyCode == VK_UP)
            {
                autoAcceptOn = true;
            }
            else if (irKey.Event.KeyEvent.wVirtualKeyCode == VK_DOWN)
            {
                autoAcceptOn = false;
            }

            string newStatusMessage = autoAcceptOn ? "On" : "Off";
            message = "Auto Accept: " + newStatusMessage + " (Press Up arrow key to On and Down arrow key to Off. Then press Enter Key. R to edit choice)";
    
        }
    } while (irKey.Event.KeyEvent.wVirtualKeyCode != VK_RETURN);

    config["autoAccept"] = autoAcceptOn;

    ofstream configFile("config.json");
    configFile << setw(4) << config << endl;
}


json loadConfig() {
    json config;
    ifstream configFile("config.json");
    if (configFile.is_open()) {
        configFile >> config;
        configFile.close();
    } else {
        config["autoAccept"] = true;
        ofstream newConfigFile("config.json");
        newConfigFile << setw(4) << config << endl;
        newConfigFile.close();
        cout << "Created new config file with default settings." << endl;
    }
    return config;
}


bool handleGamePhases(const string& port, const string& authorization, bool autoAcceptOn, int retryDelaySeconds, bool loggedIn, json &config) {

    HANDLE hConsoleInput;
    INPUT_RECORD irKey;
    DWORD dwEvents;

    hConsoleInput = GetStdHandle(STD_INPUT_HANDLE);

    HANDLE hConsoleOutput;
    DWORD dwWritten;
    COORD position; 

    hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    position.X = 0;
    position.Y = 14; 

    while (loggedIn) {
        if (!LeagueClientIsOpen()) {
            cerr << "League client has been closed. Exiting..." << endl;
            return false;
        }

        ReadConsoleInput(hConsoleInput, &irKey, 1, &dwEvents);
        if (irKey.EventType == KEY_EVENT && irKey.Event.KeyEvent.bKeyDown && irKey.Event.KeyEvent.wVirtualKeyCode == 'R') {
            toggleAutoAccept(autoAcceptOn, config);
        }


        string phaseUrl = "https://127.0.0.1:" + port + "/lol-gameflow/v1/gameflow-phase";
        string phaseResponse;
        if (sendHttpRequest(phaseUrl, authorization, phaseResponse, "GET", retryDelaySeconds)) {
            try {
                json phaseJson = json::parse(phaseResponse);
                string phase = phaseJson.get<string>();

                if (phase == "ChampSelect") {
                    cout << setw(100) << left << " " << "\r" << std::flush;
                    cout << "ChampSelect phase detected. Waiting..." << "\r" << std::flush;
                    while (phase == "ChampSelect") {
                        std::this_thread::sleep_for(std::chrono::seconds(1));
                        if (!LeagueClientIsOpen()) {
                            cerr << "League client has been closed. Exiting..." << endl;
                            return false;
                        }
                        string phaseResponse;
                        if (sendHttpRequest(phaseUrl, authorization, phaseResponse, "GET", retryDelaySeconds)) {
                            json phaseJson = json::parse(phaseResponse);
                            phase = phaseJson.get<string>();
                        } else {
                            cerr << "Failed to retrieve gameflow phase." << endl;
                            break;
                        }
                    }
                } else {
                    SetConsoleCursorPosition(hConsoleOutput, position);
                    cout << setw(100) << left << " " << "\r" << std::flush;
                    cout << "Current phase: " << phase << "\r" << std::flush;
                }

                if (phase == "ReadyCheck") {
                    if (autoAcceptOn) {
                        string acceptUrl = "https://127.0.0.1:" + port + "/lol-matchmaking/v1/ready-check/accept";
                        string acceptResponse;
                        if (!sendHttpRequest(acceptUrl, authorization, acceptResponse, "POST", retryDelaySeconds)) {
                            cerr << "Failed to auto-accept match." << endl;
                        }
                    } else {
                        cout << setw(100) << left << " " << "\r" << std::flush;
                        cout << "Auto accept is turned off. Skipping ready-check." << "\r" << std::flush;
                    }
                } 
            } catch (json::exception& e) {
                cerr << "Error parsing JSON response: " << e.what() << endl;
            }
        } else {
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
        try
        {
            if (!checkLoggedIn(port, authorization, retryDelaySeconds))
            {
                cout << "Waiting for login..." << endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }

            loggedIn = true;
            
            centerText("Login successful!"); 
            std::this_thread::sleep_for(std::chrono::seconds(5));

            
            string summonerData;
            if (getSummonerData(port, authorization, summonerData, retryDelaySeconds))
            {   
                try
                {
                    auto summonerJson = json::parse(summonerData);
                    string SummonerId = to_string(summonerJson["summonerId"]);
                    centerText("Summoner ID loaded: " + SummonerId);
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
            
            json config = loadConfig();
            bool autoAcceptOn = config.value("autoAccept", true);
            toggleAutoAccept(autoAcceptOn, config);

            handleGamePhases(port, authorization, retryDelaySeconds, autoAcceptOn, loggedIn, config);
        }
        catch (const std::exception &e)
        {
            cerr << "An error occurred: " << e.what() << endl;
            return false;
        }
    }
    return true;
}