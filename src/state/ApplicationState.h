#pragma once
#include <string>
#include <chrono>
#include "imgui.h"
#include <glad/glad.h>
#include "../core/Config.h"

struct ApplicationState {
    // Window state
    bool isMaximized = false;
    bool isDragging = false;
    ImVec2 dragOffset = ImVec2(0, 0);
    int storedWindowPos[2] = {100, 100};
    int storedWindowSize[2] = {Config::DEFAULT_WINDOW_WIDTH, Config::DEFAULT_WINDOW_HEIGHT};

    // League client state
    std::string leagueDirectory;
    std::string lockfilePath;
    std::string port;
    std::string authorizationHeader;
    bool isClientConnected = false;

    // Monitoring state
    bool isMonitoring = false;
    std::string currentPhase = "Not Connected";
    std::string connectionInfo = "";

    // UI state
    char directoryBuffer[512] = "";
    std::string logMessages = "";
    std::chrono::steady_clock::time_point lastCheckTime;

    // Icon texture
    GLuint appIconTexture = 0;
    int appIconWidth = 0;
    int appIconHeight = 0;
};

extern ApplicationState appState;