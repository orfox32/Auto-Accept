#include "DashboardTab.h"

void DashboardTab::render() {
    if (!ImGui::BeginTabItem("Dashboard")) {
        return;
    }

    ImGui::Spacing();

    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
    ImGui::BeginChild("Welcome", ImVec2(0, 80), true, ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("Welcome to LoL Auto-Accept");
    ImGui::TextColored(Theme::TextDimmed,
                      "Automatically accept matches and never miss a game!");
    ImGui::EndChild();
    ImGui::PopStyleVar();

    ImGui::Spacing();

    ImGui::Columns(2, "StatusColumns", false);

    const char* connStatus = appState.isClientConnected ? "Connected" : "Disconnected";
    const char* connInfo = appState.isClientConnected ?
                          appState.connectionInfo.c_str() :
                          "Start League and click Connect";
    UIComponents::drawStatusCard("Connection Status", connStatus,
                                appState.isClientConnected, connInfo);

    ImGui::NextColumn();

    const char* phaseDisplay = appState.currentPhase.c_str();
    bool phaseActive = appState.currentPhase != "Not Connected" &&
                      appState.currentPhase != "Stopped" &&
                      appState.currentPhase != "Client Disconnected";
    UIComponents::drawStatusCard("Game Phase", phaseDisplay, phaseActive);

    ImGui::Columns(1);
    ImGui::Spacing();

    drawControlButtons();

    ImGui::EndTabItem();
}

void DashboardTab::drawControlButtons() {
    ImGui::BeginChild("Controls", ImVec2(0, 120), true, ImGuiWindowFlags_NoScrollbar);
    ImGui::Text("Quick Actions");
    ImGui::Separator();
    ImGui::Spacing();

    if (!appState.isClientConnected) {
        bool hasDirectory = strlen(appState.directoryBuffer) > 0;
        if (UIComponents::drawButton("Connect to League", ImVec2(180, 40), hasDirectory)) {
            if (GameMonitor::connectToClient()) {
                Logger::log("Successfully connected to League of Legends!");
            }
        }
    }
    else {
        if (!appState.isMonitoring) {
            if (UIComponents::drawButton("Start Monitoring", ImVec2(160, 40),
                                       true, Theme::Success)) {
                GameMonitor::startMonitoring();
            }
        }
        else {
            if (UIComponents::drawButton("Stop Monitoring", ImVec2(160, 40),
                                       true, Theme::Warning)) {
                GameMonitor::stopMonitoring();
            }
        }
    }

    ImGui::EndChild();
}