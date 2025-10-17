#include "ActivityLogTab.h"

void ActivityLogTab::render() {
    if (!ImGui::BeginTabItem("Activity Log")) {
        return;
    }

    ImGui::Spacing();

    drawLogHeader();
    drawLogContent();

    ImGui::EndTabItem();
}

void ActivityLogTab::drawLogHeader() {
    ImGui::BeginChild("LogHeader", ImVec2(0, 50), true, ImGuiWindowFlags_NoScrollbar);

    float headerHeight = 50.0f;
    float buttonWidth = 80.0f;
    float buttonHeight = 40.0f;

    ImGui::SetCursorPosY((headerHeight - ImGui::GetTextLineHeight()) * 0.5f);
    ImGui::Text("Activity Log");

    ImGui::SameLine();
    ImGui::SetCursorPosX(ImGui::GetWindowContentRegionMax().x - buttonWidth - 10);
    ImGui::SetCursorPosY((headerHeight - buttonHeight) * 0.5f);

    if (UIComponents::drawButton("Clear", ImVec2(buttonWidth, buttonHeight),
                                true, Theme::Warning)) {
        Logger::clear();
    }

    ImGui::EndChild();
}

void ActivityLogTab::drawLogContent() {
    ImGui::BeginChild("LogContent", ImVec2(0, 0), true);

    if (!appState.logMessages.empty()) {
        ImGui::TextUnformatted(appState.logMessages.c_str());

        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 20) {
            ImGui::SetScrollHereY(1.0f);
        }
    }
    else {
        ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 50);

        float textWidth = 200.0f;
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - textWidth) * 0.5f);
        ImGui::TextColored(Theme::TextDimmed, "No activity to show yet...");

        textWidth = 300.0f;
        ImGui::SetCursorPosX((ImGui::GetContentRegionAvail().x - textWidth) * 0.5f);
        ImGui::TextColored(Theme::TextDimmed,
                         "Connect to League to start seeing logs!");
    }

    ImGui::EndChild();
}