#include "UIStyler.h"

void UIStyler::setupModernTheme() {
    ImGuiStyle& style = ImGui::GetStyle();

    style.Colors[ImGuiCol_Text] = Theme::Text;
    style.Colors[ImGuiCol_TextDisabled] = Theme::TextDimmed;
    style.Colors[ImGuiCol_WindowBg] = Theme::Background;
    style.Colors[ImGuiCol_ChildBg] = Theme::Surface;
    style.Colors[ImGuiCol_PopupBg] = Theme::Surface;
    style.Colors[ImGuiCol_Border] = Theme::Border;
    style.Colors[ImGuiCol_BorderShadow] = ImVec4(0, 0, 0, 0);

    style.Colors[ImGuiCol_FrameBg] = ImVec4(0.25f, 0.25f, 0.28f, 1.0f);
    style.Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.3f, 0.3f, 0.35f, 1.0f);
    style.Colors[ImGuiCol_FrameBgActive] = ImVec4(0.35f, 0.35f, 0.4f, 1.0f);

    style.Colors[ImGuiCol_TitleBg] = Theme::Surface;
    style.Colors[ImGuiCol_TitleBgActive] = Theme::Surface;
    style.Colors[ImGuiCol_TitleBgCollapsed] = Theme::Surface;

    style.Colors[ImGuiCol_Button] = Theme::Primary;
    style.Colors[ImGuiCol_ButtonHovered] = Theme::PrimaryHover;
    style.Colors[ImGuiCol_ButtonActive] = ImVec4(0.1f, 0.5f, 0.9f, 1.0f);

    style.WindowRounding = 0.0f;
    style.ChildRounding = 6.0f;
    style.FrameRounding = 6.0f;
    style.PopupRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 6.0f;

    style.WindowBorderSize = 0.0f;
    style.ChildBorderSize = 1.0f;
    style.PopupBorderSize = 1.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;

    style.WindowPadding = ImVec2(20.0f, 20.0f);
    style.FramePadding = ImVec2(12.0f, 8.0f);
    style.ItemSpacing = ImVec2(12.0f, 8.0f);
    style.ItemInnerSpacing = ImVec2(8.0f, 6.0f);
    style.IndentSpacing = 25.0f;
    style.ScrollbarSize = 12.0f;
    style.GrabMinSize = 12.0f;
}