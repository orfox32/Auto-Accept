#include "Application.h"

GLFWwindow* Application::window = nullptr;

bool Application::initialize() {
    if (!initializeGLFW()) {
        return false;
    }

    window = createWindow();
    if (!window) {
        glfwTerminate();
        return false;
    }

    if (!initializeOpenGL()) {
        return false;
    }

    initializeImGui();
    loadConfiguration();

    Logger::log("Application started. Welcome to League of Legends Auto-Accept!");
    appState.lastCheckTime = std::chrono::steady_clock::now();

    return true;
}

void Application::run() {
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();

        if (appState.isMonitoring) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(
                now - appState.lastCheckTime).count();

            if (elapsed >= Config::CHECK_INTERVAL_SECONDS) {
                GameMonitor::checkGamePhase();
                appState.lastCheckTime = now;
            }
        }

        renderFrame();
    }
}

void Application::shutdown() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();
}

bool Application::initializeGLFW() {
    if (!glfwInit()) {
        std::cerr << "[ERROR] Failed to initialize GLFW" << std::endl;
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);

    return true;
}

GLFWwindow* Application::createWindow() {
    GLFWwindow* win = glfwCreateWindow(
        Config::DEFAULT_WINDOW_WIDTH,
        Config::DEFAULT_WINDOW_HEIGHT,
        Config::WINDOW_TITLE,
        NULL, NULL
    );

    if (win) {
        glfwSetWindowAttrib(win, GLFW_RESIZABLE, GLFW_TRUE);
        TextureLoader::setWindowIcon(win, Config::APP_ICON_PATH);
        glfwMakeContextCurrent(win);
        glfwSwapInterval(1);
    }

    return win;
}

bool Application::initializeOpenGL() {
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "[ERROR] Failed to initialize OpenGL loader" << std::endl;
        return false;
    }
    return true;
}

void Application::initializeImGui() {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    UIStyler::setupModernTheme();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImFont* customFont = io.Fonts->AddFontFromFileTTF(
        "C:/Windows/Fonts/segoeui.ttf", 18.0f,
        nullptr, io.Fonts->GetGlyphRangesDefault()
    );

    if (customFont) {
        io.FontDefault = customFont;
    }

    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 130");
}

void Application::loadConfiguration() {
    std::string savedDirectory = ConfigManager::loadDirectory();
    if (!savedDirectory.empty()) {
        strncpy(appState.directoryBuffer, savedDirectory.c_str(),
               sizeof(appState.directoryBuffer) - 1);
        Logger::log("Loaded saved directory: " + savedDirectory);
    }
}

void Application::renderFrame() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    MainWindow::render(window);

    ImGui::Render();
    int displayWidth, displayHeight;
    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
    glViewport(0, 0, displayWidth, displayHeight);
    glClearColor(0.1f, 0.11f, 0.13f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

    glfwSwapBuffers(window);
}