#pragma once
#include "imgui.h"
#include "../../state/ApplicationState.h"
#include "../../state/Theme.h"
#include "../UIComponents.h"
#include "../../league/GameMonitor.h"
#include "../../core/Logger.h"

class DashboardTab {
public:
    static void render();

private:
    static void drawControlButtons();
};