#include "CustomTitleBar.h"

bool CustomTitleBar::draw(GLFWwindow* window) {
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    ImVec2 windowPos = ImGui::GetWindowPos();
    ImVec2 windowSize = ImGui::GetWindowSize();

    ImVec2 titleBarMin = windowPos;
    ImVec2 titleBarMax = ImVec2(windowPos.x + windowSize.x,
                                windowPos.y + TITLE_BAR_HEIGHT);
    drawList->AddRectFilled(titleBarMin, titleBarMax,
                           IM_COL32(22, 24, 31, 255),
                           8.0f, ImDrawFlags_RoundCornersTop);

    drawIconAndTitle();
    bool shouldClose = drawControlButtons(window, windowSize);
    handleWindowInteraction(window, windowSize);

    ImGui::SetCursorPosY(TITLE_BAR_HEIGHT + 5);
    ImGui::Dummy(ImVec2(0.0f, 0.0f));

    return shouldClose;
}

void CustomTitleBar::drawIconAndTitle() {
    TextureLoader::loadAppIcon();

    if (appState.appIconTexture) {
        ImGui::SetCursorPosY((35 - 20) * 0.5f);
        ImGui::Image((void*)(intptr_t)appState.appIconTexture, ImVec2(20, 20));
        ImGui::SameLine();
    }

    ImGui::SetCursorPosY((35 - ImGui::GetTextLineHeight()) * 0.5f);
    ImGui::Text("%s", Config::WINDOW_TITLE);
}

bool CustomTitleBar::drawControlButtons(GLFWwindow* window, ImVec2 windowSize) {
    float buttonsStartX = windowSize.x - (BUTTON_WIDTH * 3) - 10;
    ImGui::SetCursorPos(ImVec2(buttonsStartX, 5));

    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 0.5f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.2f, 0.2f, 0.2f, 0.7f));
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);

    if (ImGui::Button("−", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT))) {
        glfwIconifyWindow(window);
    }

    ImGui::SameLine(0, 0);
    const char* maximizeIcon = appState.isMaximized ? "❐" : "▢";
    if (ImGui::Button(maximizeIcon, ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT))) {
        toggleMaximize(window);
    }

    ImGui::SameLine(0, 0);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 0.8f));
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.7f, 0.1f, 0.1f, 1.0f));
    bool shouldClose = ImGui::Button("×", ImVec2(BUTTON_WIDTH, BUTTON_HEIGHT));
    ImGui::PopStyleColor(2);

    ImGui::PopStyleColor(3);
    ImGui::PopStyleVar(1);

    return shouldClose;
}

void CustomTitleBar::handleWindowInteraction(GLFWwindow* window, ImVec2 windowSize) {
    double mouseX, mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    bool mouseInTitleBar = mouseX >= 0 &&
                          mouseX <= windowSize.x - (BUTTON_WIDTH * 3) &&
                          mouseY >= 0 &&
                          mouseY <= TITLE_BAR_HEIGHT;

    if (mouseInTitleBar && ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
        appState.isDragging = true;
        appState.dragOffset = ImVec2((float)mouseX, (float)mouseY);
    }

    if (appState.isDragging && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
        int winX, winY;
        glfwGetWindowPos(window, &winX, &winY);
        int newX = (int)(mouseX - appState.dragOffset.x + winX);
        int newY = (int)(mouseY - appState.dragOffset.y + winY);
        glfwSetWindowPos(window, newX, newY);
    }

    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left)) {
        appState.isDragging = false;
    }

    if (mouseInTitleBar && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
        toggleMaximize(window);
    }
}

void CustomTitleBar::toggleMaximize(GLFWwindow* window) {
    if (appState.isMaximized) {
        glfwRestoreWindow(window);
        appState.isMaximized = false;
    }
    else {
        glfwMaximizeWindow(window);
        appState.isMaximized = true;
    }
}