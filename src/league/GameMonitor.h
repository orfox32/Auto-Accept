#pragma once
#include <string>
#include <nlohmann/json.hpp>
#include "../state/ApplicationState.h"
#include "../core/Logger.h"
#include "../storage/FileUtils.h"
#include "../network/HttpClient.h"
#include "LeagueClient.h"

class GameMonitor {
public:
    static bool connectToClient();
    static void checkGamePhase();
    static void startMonitoring();
    static void stopMonitoring();

private:
    static void handleGamePhase(const std::string& phase);
    static void acceptMatch();
};