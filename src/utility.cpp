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

using namespace std;
using json = nlohmann::json;

bool extractAndEncodeRiotPassword(DWORD processId, string& port, string& encodedPassword) {
    
    string command = "wmic process where 'ProcessId=" + to_string(processId) + "' get Commandline";


    string result;
    FILE* pipe = _popen(command.c_str(), "r");
    if (!pipe) {
        cerr << "Error: Could not execute command." << endl;
        return false;
    }
    char buffer[128];
    while (!feof(pipe)) {
        if (fgets(buffer, 128, pipe) != nullptr)
            result += buffer;
    }
    _pclose(pipe);

    regex portRegex("--app-port=\"?(\\d+)\"?");
    regex authTokenRegex("--remoting-auth-token=([a-zA-Z0-9_-]+)");
    smatch portMatch, authTokenMatch;

    if (regex_search(result, portMatch, portRegex) && regex_search(result, authTokenMatch, authTokenRegex)) {
        if (portMatch.size() > 1 && authTokenMatch.size() > 1) {
            port = portMatch.str(1);
            string authToken = authTokenMatch.str(1);

            string auth = "riot:" + authToken;
            encodedPassword = base64_encode(auth);
            return true;
        }
    }
    return false;
}

bool authenticateAndAutoAccept(const string& port, const string& authorization) {

    bool loggedIn = false;
    
    while (!loggedIn) {
        cout << "Checking login status..." << endl;
        
        try {
            if (!checkLoggedIn(port, authorization)) {
                cout << "Waiting for login..." << endl;
                std::this_thread::sleep_for(std::chrono::seconds(1));
                continue;
            }

            cout << "Login successful!" << endl;
            loggedIn = true;

            string phase;
            while (true) {
                 if (!LeagueClientIsOpen()) {
                    cerr << "League client has been closed. exiting..." << endl;
                    std::this_thread::sleep_for(std::chrono::seconds(5));
                    return false;
                }
                string phaseUrl = "https://127.0.0.1:" + port + "/lol-gameflow/v1/gameflow-phase";
                string phaseResponse;
                if (sendHttpRequest(phaseUrl, authorization, phaseResponse, "GET")) {
                    try {
                        json phaseJson = json::parse(phaseResponse);
                        phase = phaseJson.get<string>();
                        cout << "Current phase: " << phase << "\r" << std::flush;

                        if (phase == "ReadyCheck") {
                            string acceptUrl = "https://127.0.0.1:" + port + "/lol-matchmaking/v1/ready-check/accept";
                            string acceptResponse;
                            if (!sendHttpRequest(acceptUrl, authorization, acceptResponse, "POST")) {
                                cerr << "Failed to auto-accept match." << endl;
                            }
                        }
                        else if (phase == "ChampSelect") {
                            cout << "ChampSelect phase detected. Waiting..." << "\r" << std::flush;
                        }

                        else if (phase == "InProgress") {
                            cout << "Game is in progress. Exiting." << endl;
                            return true;
                        }
                    }
                    catch (json::exception &e) {
                        cerr << "Error parsing JSON response: " << e.what() << endl;
                    }
                }
                else {
                    cerr << "Failed to retrieve gameflow phase." << endl;
                }

                std::this_thread::sleep_for(std::chrono::seconds(1));
            }
        }
        catch (const std::exception &e) {
            cerr << "An error occurred: " << e.what() << endl;
        }
    }

    return false;
}