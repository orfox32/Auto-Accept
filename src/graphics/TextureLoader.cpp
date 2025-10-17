#include "TextureLoader.h"
#include "../state/ApplicationState.h"
#include "../core/Config.h"

GLuint TextureLoader::loadFromFile(const char* filename, int& width, int& height) {
    int channels;
    unsigned char* data = stbi_load(filename, &width, &height, &channels, 4);

    if (!data) {
        return 0;
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    return texture;
}

void TextureLoader::loadAppIcon() {
    if (appState.appIconTexture == 0) {
        appState.appIconTexture = loadFromFile(Config::APP_ICON_PATH,
                                              appState.appIconWidth,
                                              appState.appIconHeight);
        if (appState.appIconTexture) {
            std::cout << "[INFO] App icon loaded successfully." << std::endl;
        }
        else {
            std::cerr << "[ERROR] Failed to load app icon." << std::endl;
        }
    }
}

void TextureLoader::setWindowIcon(GLFWwindow* window, const std::string& iconPath) {
    GLFWimage images[1];
    images[0].pixels = stbi_load(iconPath.c_str(), &images[0].width,
                                &images[0].height, nullptr, 4);

    if (images[0].pixels) {
        glfwSetWindowIcon(window, 1, images);
        stbi_image_free(images[0].pixels);
        std::cout << "[INFO] Window icon loaded: " << iconPath << std::endl;
    }
    else {
        std::cerr << "[ERROR] Failed to load window icon: " << iconPath << std::endl;
    }
}
