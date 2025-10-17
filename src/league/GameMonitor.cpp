#include "GameMonitor.h"

bool GameMonitor::connectToClient() {
    appState.leagueDirectory = std::string(appState.directoryBuffer);

    appState.lockfilePath = LeagueClient::findLockfile(appState.leagueDirectory);
    if (appState.lockfilePath.empty()) {
        Logger::log("Unable to find lockfile. Make sure League is running.");
        return false;
    }

    if (!LockfileParser::parse(appState.lockfilePath,
                              appState.port,
                              appState.authorizationHeader)) {
        return false;
    }

    appState.connectionInfo = "Connected to port " + appState.port;
    Logger::log("Successfully connected to League client on port " + appState.port);
    appState.isClientConnected = true;

    return true;
}

void GameMonitor::checkGamePhase() {
    if (!appState.isClientConnected) {
        return;
    }

    if (!FileUtils::fileExists(appState.lockfilePath)) {
        Logger::log("League client is not running.");
        stopMonitoring();
        appState.isClientConnected = false;
        appState.currentPhase = "Client Disconnected";
        return;
    }

    std::string phaseUrl = HttpClient::buildUrl(appState.port, "/lol-gameflow/v1/gameflow-phase");
    std::string phaseResponse;

    if (HttpClient::sendRequest(phaseUrl, appState.authorizationHeader, phaseResponse)) {
        try {
            nlohmann::json phaseJson = nlohmann::json::parse(phaseResponse);
            std::string phase = phaseJson.get<std::string>();

            if (appState.currentPhase != phase) {
                appState.currentPhase = phase;
                Logger::log("Game phase changed to: " + phase);
            }

            handleGamePhase(phase);
        }
        catch (nlohmann::json::exception& e) {
            Logger::log("Error parsing JSON: " + std::string(e.what()));
        }
    }
    else {
        Logger::log("Failed to retrieve game phase.");
    }
}

void GameMonitor::startMonitoring() {
    appState.isMonitoring = true;
    Logger::log("Auto-accept monitoring started!");
}

void GameMonitor::stopMonitoring() {
    appState.isMonitoring = false;
    appState.currentPhase = "Stopped";
    Logger::log("Monitoring stopped.");
}

void GameMonitor::handleGamePhase(const std::string& phase) {
    if (phase == "ReadyCheck") {
        acceptMatch();
    }
    else if (phase == "InProgress") {
        Logger::log("Game is in progress. Monitoring continues...");
    }
}

void GameMonitor::acceptMatch() {
    std::string acceptUrl = HttpClient::buildUrl(appState.port,
                                           "/lol-matchmaking/v1/ready-check/accept");
    std::string acceptResponse;

    if (HttpClient::sendRequest(acceptUrl, appState.authorizationHeader,
                                acceptResponse, "POST")) {
        Logger::log("✓ Match accepted automatically!");
    }
    else {
        Logger::log("✗ Failed to auto-accept match.");
    }
}