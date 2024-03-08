#include <iostream>
#include <Windows.h>
#include <string>
#include <chrono>
#include <thread>
#include "base64.h"
#include "http_request.h"
#include "league_client.h"
#include "utility.h"

using namespace std;

int main() {
    
   const int maxAttempts = 10;
    int attempts = 0;

    while (attempts < maxAttempts) {
        if (LeagueClientIsOpen()) {
            break;
        } else {
            cerr << "League Client is not running. Attempt " << attempts + 1 << " of " << maxAttempts << "." << endl;
            attempts++;
            std::this_thread::sleep_for(std::chrono::seconds(3));
        }
    }

    DWORD lcProcessId = GetLeagueClientProcessId();

    string port, encodedPassword;

    if (!extractAndEncodeRiotPassword(lcProcessId, port, encodedPassword)) {
        cerr << "Error: Failed to extract Riot password or encode it." << endl;
        return 1;
    }

    string authorization = "Authorization: Basic " + encodedPassword;

    if (!authenticateAndAutoAccept(port, authorization)) {
         if (LeagueClientIsOpen()) {
        cerr << "Error: Authentication failed." << endl;
        return 1;
         }
    }
  
    
    return 0;

}

