#pragma once
#include "imgui.h"
#include "../../state/ApplicationState.h"
#include "../../state/Theme.h"
#include "../UIComponents.h"
#include "../../core/Logger.h"

class ActivityLogTab {
public:
    static void render();

private:
    static void drawLogHeader();
    static void drawLogContent();
};
