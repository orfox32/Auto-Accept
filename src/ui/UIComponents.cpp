#include "UIComponents.h"

void UIComponents::drawStatusCard(const char* title, const char* status,
                          bool isActive, const char* additionalInfo) {
    ImGui::BeginChild(title, ImVec2(0, 120), true, ImGuiWindowFlags_NoScrollbar);

    ImGui::Text("%s", title);
    ImGui::Separator();
    ImGui::Spacing();

    ImVec4 statusColor = isActive ? Theme::Success : Theme::Error;
    const char* indicator = "●";

    ImGui::TextColored(statusColor, "%s", indicator);
    ImGui::SameLine();
    ImGui::Text("%s", status);

    if (additionalInfo) {
        ImGui::TextColored(Theme::TextDimmed, "%s", additionalInfo);
    }

    ImGui::EndChild();
}

bool UIComponents::drawButton(const char* label, ImVec2 size,
                      bool enabled, ImVec4 color) {
    bool clicked = false;

    if (!enabled) {
        ImGui::BeginDisabled();
    }

    ImGui::PushStyleColor(ImGuiCol_Button, color);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered,
                        ImVec4(color.x + 0.1f, color.y + 0.1f, color.z + 0.1f, color.w));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,
                        ImVec4(color.x - 0.1f, color.y - 0.1f, color.z - 0.1f, color.w));

    clicked = ImGui::Button(label, size);

    ImGui::PopStyleColor(3);

    if (!enabled) {
        ImGui::EndDisabled();
    }

    return clicked;
}

void UIComponents::drawInfoBox(const char* text, ImVec4 bgColor) {
    ImGui::PushStyleColor(ImGuiCol_ChildBg, bgColor);
    ImGui::BeginChild("InfoBox", ImVec2(0, 100), true, ImGuiWindowFlags_NoScrollbar);
    ImGui::TextWrapped("%s", text);
    ImGui::EndChild();
    ImGui::PopStyleColor();
}