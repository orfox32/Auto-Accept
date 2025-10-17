#pragma once
#include "imgui.h"
#include "../state/Theme.h"

class UIComponents {
public:
    static void drawStatusCard(const char* title, const char* status,
                              bool isActive, const char* additionalInfo = nullptr);
    
    static bool drawButton(const char* label, ImVec2 size = ImVec2(0, 0),
                          bool enabled = true, ImVec4 color = Theme::Primary);
    
    static void drawInfoBox(const char* text, ImVec4 bgColor = Theme::Surface);
};