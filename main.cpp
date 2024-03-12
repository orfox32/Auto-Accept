#include <iostream>
#include <Windows.h>
#include <string>
#include <chrono>
#include <thread>
#include "base64.h"
#include "http_request.h"
#include "league_client.h"
#include "utility.h"
#include <nlohmann/json.hpp>

using namespace std;
using json = nlohmann::json;

void setConsoleColor(WORD color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}

const string ArtName = R"(
     ___      __    __  .___________.  ______           ___       ______   ______  _______ .______   .___________.
    /   \    |  |  |  | |           | /  __  \         /   \     /      | /      ||   ____||   _  \  |           |
   /  ^  \   |  |  |  | `---|  |----`|  |  |  |       /  ^  \   |  ,----'|  ,----'|  |__   |  |_)  | `---|  |----`
  /  /_\  \  |  |  |  |     |  |     |  |  |  |      /  /_\  \  |  |     |  |     |   __|  |   ___/      |  |     
 /  _____  \ |  `--'  |     |  |     |  `--'  |     /  _____  \ |  `----.|  `----.|  |____ |  |          |  |     
/__/     \__\ \______/      |__|      \______/     /__/     \__\ \______| \______||_______|| _|          |__|     
 )";

void printCenteredArt(const string& art, int paddingTop, int paddingBottom) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    int columns;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    columns = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    
    vector<string> lines;
    size_t pos = 0;
    size_t found;
    while ((found = art.find('\n', pos)) != string::npos) {
        lines.push_back(art.substr(pos, found - pos));
        pos = found + 1;
    }
    if (pos < art.size())
        lines.push_back(art.substr(pos));

    for (int i = 0; i < paddingTop; ++i) {
        for (int j = 0; j < columns; ++j)
            cout << " ";
        cout << endl;
    }

    for (const auto& l : lines) {
        int leftPadding = (columns - l.size()) / 2;
        for (int i = 0; i < leftPadding; ++i)
            cout << " ";
        cout << l << endl;
    }

    for (int i = 0; i < paddingBottom; ++i) {
        for (int j = 0; j < columns; ++j)
            cout << " ";
        cout << endl;
    }
}

int main() {

   setConsoleColor(FOREGROUND_RED | FOREGROUND_INTENSITY);

   printCenteredArt(ArtName, 1, 1);

   setConsoleColor(FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_GREEN);


   const int maxAttempts = 10;
    int attempts = 0;

    while (attempts < maxAttempts) {
        if (LeagueClientIsOpen()) {
            break;
        } else {
            cerr << "League Client is not running. Attempt " << attempts + 1 << " of " << maxAttempts << "." << "\r" << std::flush;
            attempts++;
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }

     if (LeagueClientIsOpen()) {
        DWORD lcProcessId = GetLeagueClientProcessId();
        string port, encodedPassword;

        if (!extractAndEncodeRiotPassword(lcProcessId, port, encodedPassword)) {
            cerr << "Error: Failed to extract Riot password or encode it." << endl;
            return 1;
        }
        
        string authorization = "Authorization: Basic " + encodedPassword;
        if (!authenticateAndAutoAccept(port, authorization)) {
            cerr << "Error: Authentication failed." << endl;
            return 1;
        }
    } else {
        cerr << "Error: League of Legends client is not running." << endl;
        return 1;
    }
    
    return 0;
}

