#pragma once
#include <iostream>
#include <chrono>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include "Config.h"
#include "Logger.h"
#include "../state/ApplicationState.h"
#include "../ui/MainWindow.h"
#include "../ui/UIStyler.h"
#include "../graphics/TextureLoader.h"
#include "../storage/ConfigManager.h"
#include "../league/GameMonitor.h"

class Application {
public:
    static bool initialize();
    static void run();
    static void shutdown();

private:
    static GLFWwindow* window;

    static bool initializeGLFW();
    static GLFWwindow* createWindow();
    static bool initializeOpenGL();
    static void initializeImGui();
    static void loadConfiguration();
    static void renderFrame();
};
