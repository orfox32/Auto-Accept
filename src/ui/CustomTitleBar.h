#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "../state/ApplicationState.h"
#include "../core/Config.h"
#include "../graphics/TextureLoader.h"

class CustomTitleBar {
public:
    static bool draw(GLFWwindow* window);

private:
    static constexpr float TITLE_BAR_HEIGHT = 40.0f;
    static constexpr float BUTTON_WIDTH = 46.0f;
    static constexpr float BUTTON_HEIGHT = 30.0f;

    static void drawIconAndTitle();
    static bool drawControlButtons(GLFWwindow* window, ImVec2 windowSize);
    static void handleWindowInteraction(GLFWwindow* window, ImVec2 windowSize);
    static void toggleMaximize(GLFWwindow* window);
};
