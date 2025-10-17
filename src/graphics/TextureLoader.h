#pragma once
#include <iostream>
#include <string>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

class TextureLoader {
public:
    static GLuint loadFromFile(const char* filename, int& width, int& height);
    static void loadAppIcon();
    static void setWindowIcon(GLFWwindow* window, const std::string& iconPath);
};
