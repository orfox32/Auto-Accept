#pragma once
#include "imgui.h"
#include "../../state/ApplicationState.h"
#include "../../state/Theme.h"
#include "../UIComponents.h"
#include "../../storage/ConfigManager.h"

class SettingsTab {
public:
    static void render();

private:
    static void drawInstructions();
};