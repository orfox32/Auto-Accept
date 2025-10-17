#include "core/Application.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

int main() {
    if (!Application::initialize()) {
        return -1;
    }

    Application::run();
    Application::shutdown();

    return 0;
}

#ifdef _WIN32
#include <windows.h>

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine, int nCmdShow) {
    return main();
}
#endif
