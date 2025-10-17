#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "CustomTitleBar.h"
#include "tabs/DashboardTab.h"
#include "tabs/SettingsTab.h"
#include "tabs/ActivityLogTab.h"

class MainWindow {
public:
    static void render(GLFWwindow* window);

private:
    static void drawMainContent();
};
