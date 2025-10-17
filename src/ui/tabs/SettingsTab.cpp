#include "SettingsTab.h"

void SettingsTab::render() {
    if (!ImGui::BeginTabItem("Settings")) {
        return;
    }

    ImGui::Spacing();

    ImGui::BeginChild("SettingsContent", ImVec2(0, 0), true);

    ImGui::Text("Configuration");
    ImGui::Separator();
    ImGui::Spacing();

    ImGui::Text("League of Legends Installation Directory");
    ImGui::TextColored(Theme::TextDimmed,
                      "Select your League installation folder (usually in Riot Games/League of Legends)");
    ImGui::Spacing();

    ImGui::SetNextItemWidth(-120);
    ImGui::InputTextWithHint("##directory", "C:\\Riot Games\\League of Legends",
                            appState.directoryBuffer, sizeof(appState.directoryBuffer));
    ImGui::SameLine();

    bool hasDirectory = strlen(appState.directoryBuffer) > 0;
    if (UIComponents::drawButton("Save", ImVec2(100, 0), hasDirectory)) {
        ConfigManager::saveDirectory(std::string(appState.directoryBuffer));
    }

    ImGui::Spacing();
    ImGui::Separator();
    ImGui::Spacing();

    drawInstructions();

    ImGui::EndChild();

    ImGui::EndTabItem();
}

void SettingsTab::drawInstructions() {
    ImGui::Text("Setup Instructions");
    ImGui::Spacing();

    ImGui::PushStyleColor(ImGuiCol_ChildBg, Theme::Surface);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(12, 12));
    ImGui::BeginChild("InstructionsBox", ImVec2(0, 120), true);

    ImGui::TextWrapped(
        "1. Set your League of Legends installation directory above\n"
        "2. Launch League of Legends and log into your account\n"
        "3. Go to the Dashboard tab and click 'Connect to League'\n"
        "4. Click 'Start Monitoring' to enable auto-accept\n"
        "5. Queue for a game and the tool will automatically accept matches!");

    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}