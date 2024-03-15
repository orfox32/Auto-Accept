#include "utility.h"
#include "base64.h"
#include "league_client.h"
#include "http_request.h"
#include <iostream>
#include <string>
#include <regex>
#include <chrono>
#include <thread>
#include <fstream>
#include <nlohmann/json.hpp>
#include <iomanip>
#include <windows.h>
#include "artwindow.h"
#include <conio.h>
#include <unordered_map>

using namespace std;
using json = nlohmann::json;
const string CONFIG_FILE = "config.json";

const string ArtName = R"(
     ___      __    __  .___________.  ______           ___       ______   ______  _______ .______   .___________.
    /   \    |  |  |  | |           | /  __  \         /   \     /      | /      ||   ____||   _  \  |           |
   /  ^  \   |  |  |  | `---|  |----`|  |  |  |       /  ^  \   |  ,----'|  ,----'|  |__   |  |_)  | `---|  |----`
  /  /_\  \  |  |  |  |     |  |     |  |  |  |      /  /_\  \  |  |     |  |     |   __|  |   ___/      |  |     
 /  _____  \ |  `--'  |     |  |     |  `--'  |     /  _____  \ |  `----.|  `----.|  |____ |  |          |  |     
/__/     \__\ \______/      |__|      \______/     /__/     \__\ \______| \______||_______|| _|          |__|     
 )";



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


void saveConfig(bool autoAcceptEnabled) {
    json config;
    config["autoAcceptEnabled"] = autoAcceptEnabled;

    ofstream file(CONFIG_FILE);
    if (file.is_open()) {
        file << config;
        file.close();
    } else {
        cerr << "Unable to open config file for writing." << endl;
    }
}

bool loadConfig(bool &autoAcceptEnabled) {
    ifstream file(CONFIG_FILE);
    if (file.is_open()) {
        json config;
        file >> config;
        file.close();
        if (config.contains("autoAcceptEnabled")) {
            autoAcceptEnabled = config["autoAcceptEnabled"];
            return true;
        } else {
            cerr << "Config file is missing 'autoAcceptEnabled' field." << endl;
        }
    } else {
        cerr << "\nConfig file does not exist. Creating default config." << endl;
        saveConfig(autoAcceptEnabled);
        std::this_thread::sleep_for(std::chrono::seconds(3));
    }
    return false;
}

void showMenu(int selectedIndex, const string &gameName, const string &tagLine, bool &autoAcceptEnabled) {
    system("cls");
    setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);

    printCenteredArt(ArtName, 1, 1);

    setConsoleColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);
    centerText("Log in as: [" + gameName + "#" + tagLine + "]");
    cout << "\n";
    cout << "Menu Selection:\n";
    cout << (selectedIndex == 0 ? "> " : "  ") << "1. Auto Accept: " << (autoAcceptEnabled ? "Enabled" : "Disabled") << endl;;
    cout << (selectedIndex == 1 ? "> " : "  ") << "2. Show logs" << endl;
    cout << (selectedIndex == 2 ? "> " : "  ") << "3. Exit" << endl;
}

void handleArrowKeyPress(int &selectedIndex, int maxIndex, bool isUpArrow) {
    if (isUpArrow) {
        selectedIndex--;
        if (selectedIndex < 0)
            selectedIndex = maxIndex;
    } else {
        selectedIndex++;
        if (selectedIndex > maxIndex)
            selectedIndex = 0;
    }
}


enum class GamePhase {
    Lobby,
    Matchmaking,
    ReadyCheck,
    ChampSelect,
    InProgress,
    WaitingForStats,
    PreEndOfGame,
    EndOfGame
};

template<typename T>
class PhaseOption {
private:
    bool hasValue;
    T value;

public:
    PhaseOption() : hasValue(false) {}

    PhaseOption(const T& value) : hasValue(true), value(value) {}

    bool has_value() const {
        return hasValue;
    }

    const T& value_or(const T& defaultValue) const {
        return hasValue ? value : defaultValue;
    }
};

std::unordered_map<std::string, GamePhase> phaseMap = {
    {"Lobby", GamePhase::Lobby},
    {"Matchmaking", GamePhase::Matchmaking},
    {"ReadyCheck", GamePhase::ReadyCheck},
    {"ChampSelect", GamePhase::ChampSelect},
    {"InProgress", GamePhase::InProgress},
    {"WaitingForStats", GamePhase::WaitingForStats},
    {"PreEndOfGame", GamePhase::PreEndOfGame},
    {"EndOfGame", GamePhase::EndOfGame}
};

PhaseOption<GamePhase> getPhaseFromString(const std::string& phaseStr) {
    auto it = phaseMap.find(phaseStr);
    if (it != phaseMap.end()) {
        return PhaseOption<GamePhase>(it->second);
    }
    return PhaseOption<GamePhase>();
}

bool handleGamePhases(const std::string &port, const std::string &authorization, int retryDelaySeconds, bool &loggedIn, bool &autoAcceptEnabled) {
    std::string acceptUrl;
    std::string acceptResponse;

    while (loggedIn) {
        if (!LeagueClientIsOpen()) {
            std::cerr << "League client has been closed. Exiting..." << std::endl;
            return false;
        }

        if (autoAcceptEnabled) {
            std::string phaseUrl = "https://127.0.0.1:" + port + "/lol-gameflow/v1/session";
            std::string phaseResponse;

            if (sendHttpRequest(phaseUrl, authorization, phaseResponse, "GET", retryDelaySeconds)) {
                try {
                    json phaseJson = json::parse(phaseResponse);

                    if (phaseJson.contains("phase") && phaseJson["phase"].is_string()) {
                        std::string phaseStr = phaseJson["phase"].get<std::string>();
                        auto phaseOpt = getPhaseFromString(phaseStr);

                        if (phaseOpt.has_value()) {
                            GamePhase phase = phaseOpt.value_or(GamePhase::Lobby); 

                            switch (phase) {
                                case GamePhase::Lobby:
                                    std::this_thread::sleep_for(std::chrono::seconds(5));
                                    break;
                                case GamePhase::Matchmaking:
                                    std::this_thread::sleep_for(std::chrono::seconds(2));
                                    break;
                                case GamePhase::ReadyCheck:
                                    std::cout << "Current Phase: Ready Check" << std::endl;
                                    acceptUrl = "https://127.0.0.1:" + port + "/lol-matchmaking/v1/ready-check/accept";
                                    acceptResponse.clear();
                                    if (!sendHttpRequest(acceptUrl, authorization, acceptResponse, "POST", retryDelaySeconds)) {
                                        std::cerr << "Failed to auto-accept match." << std::endl;
                                    }
                                    break;
                                case GamePhase::ChampSelect:
                                    //todo: auto champ select
                                    std::this_thread::sleep_for(std::chrono::seconds(9));
                                    break;
                                case GamePhase::InProgress:
                                case GamePhase::WaitingForStats:
                                case GamePhase::PreEndOfGame:
                                    std::this_thread::sleep_for(std::chrono::seconds(9));
                                    break;
                                case GamePhase::EndOfGame:
                                    std::this_thread::sleep_for(std::chrono::seconds(5));
                                    break;
                                default:
                                    std::cerr << "Unknown phase: " << phaseStr << std::endl;
                                    std::this_thread::sleep_for(std::chrono::seconds(1));
                                    break;
                            }
                        } else {
                            std::cerr << "Error: Unknown phase: " << phaseStr << std::endl;
                        }
                    } else {
                        continue;
                    }
                } catch (json::exception &e) {
                    std::cerr << "Error parsing JSON response: " << e.what() << std::endl;
                }
            } else {
                std::cerr << "Failed to retrieve game phase." << std::endl;
            }
        } else {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }
    }
    return true;
}


bool authenticateAndAutoAccept(const string &port, const string &authorization, int retryDelaySeconds)
{
    bool loggedIn = false;
    string gameName;
    string tagLine;
    bool autoAcceptEnabled = true;

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
            cout << setw(110) << left << " " << "\r" << std::flush;
            centerText("Login successful!");
            std::this_thread::sleep_for(std::chrono::seconds(5));

            if (!loadConfig(autoAcceptEnabled)) {
            saveConfig(autoAcceptEnabled); 
            }

            string summonerData;
            if (getSummonerData(port, authorization, summonerData, retryDelaySeconds))
            {
                try
                {
                    auto summonerJson = json::parse(summonerData);
                    gameName = std::regex_replace(summonerJson["gameName"].dump(), std::regex("\""), "");
                    tagLine = std::regex_replace(summonerJson["tagLine"].dump(), std::regex("\""), "");
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

            int selectedIndex = 0;
            while (loggedIn) {
 
            
                showMenu(selectedIndex, gameName, tagLine, autoAcceptEnabled);
                DWORD fdwMode = ENABLE_WINDOW_INPUT | ENABLE_MOUSE_INPUT;
                HANDLE hStdin = GetStdHandle(STD_INPUT_HANDLE);
                SetConsoleMode(hStdin, fdwMode);

                INPUT_RECORD irInBuf;
                DWORD cNumRead;

                if (ReadConsoleInput(hStdin, &irInBuf, 1, &cNumRead)) {
                    if (irInBuf.EventType == KEY_EVENT && irInBuf.Event.KeyEvent.bKeyDown) {
                        switch (irInBuf.Event.KeyEvent.wVirtualKeyCode) {
                        case VK_UP:
                            handleArrowKeyPress(selectedIndex, 2, true); 
                            break;
                        case VK_DOWN:
                            handleArrowKeyPress(selectedIndex, 2, false); 
                            break;
                        case VK_RETURN:
                            switch (selectedIndex) {
                                case 0:
                                    autoAcceptEnabled = !autoAcceptEnabled; 
                                    saveConfig(autoAcceptEnabled);
                                    break;
                                case 1:
                                   
                                    break;
                                case 2:
                                    return 1;
                                    break;
                            }
                            break;
                        }
                    }
                }
            } 

            handleGamePhases(port, authorization, retryDelaySeconds, loggedIn, autoAcceptEnabled);
        }
        catch (const std::exception &e)
        {
            cerr << "An error occurred: " << e.what() << endl;
            return false;
        }
    }
 
    return true; 
}

