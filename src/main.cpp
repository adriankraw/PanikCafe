#include <cstddef>
#include <string>
#include "window.cpp"

int main(int argc, char *argv[]) {
    window mainWindow{};
    mainWindow.Init(1280, 720, "CodingProject");

    while (!WindowShouldClose()) {
        mainWindow.Loop();
    }
    mainWindow.Close();
    return 0;
}
