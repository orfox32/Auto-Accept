#include "MainWindow.h"

void MainWindow::render(GLFWwindow* window) {
    int displayWidth, displayHeight;
    glfwGetFramebufferSize(window, &displayWidth, &displayHeight);
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)displayWidth, (float)displayHeight));

    ImGui::Begin("Main Window", nullptr,
                ImGuiWindowFlags_NoTitleBar |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoResize |
                ImGuiWindowFlags_NoMove);

    bool shouldClose = CustomTitleBar::draw(window);
    if (shouldClose) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        ImGui::End();
        return;
    }

    drawMainContent();

    ImGui::End();
}

void MainWindow::drawMainContent() {
    if (ImGui::BeginTabBar("MainTabs", ImGuiTabBarFlags_None)) {
        DashboardTab::render();
        SettingsTab::render();
        ActivityLogTab::render();
        ImGui::EndTabBar();
    }
}