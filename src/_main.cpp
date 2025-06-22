#include "raylib.h"
#include <cmath>
#include <cstddef>
#include <filesystem>
#include <iostream>
#include <string>
#include <vector>

class Ring {
  public:
    bool running = false;
    float spaceCurrentSize = 0;
};

int main(int argc, char *argv[]) {
    std::vector<Ring> rings;
    for (size_t i = 0; i < 10; ++i) {
        rings.push_back(Ring());
    }
    const int screenWidth = 800;
    const int screenHeight = 480;
    const std::string windowName = "Game01";
    const int windowFPS = 30;
    const Color backgroundColor = DARKGRAY;
    const Color textColor = WHITE;
    Vector2 playerVelocity = Vector2{0, 0};

    InitWindow(screenWidth, screenHeight, windowName.c_str());
    SetTargetFPS(windowFPS);

    std::string currentPath = std::filesystem::current_path().c_str();
    Texture2D playerTexture = LoadTexture(currentPath.append("/texture/Logo.png").c_str());
    SetTextureFilter(playerTexture, TEXTURE_FILTER_BILINEAR);

    bool spacePressed = false;
    const int spaceMaxSize = 150;
    Vector2 playerInput = {0, 0};
    while (!WindowShouldClose()) {
        // Input
        if (IsKeyPressed(KEY_SPACE)) {
            for (auto &r : rings) {
                if (r.running == false) {
                    r.running = true;
                    break;
                }
            }
        }
        for (auto &r : rings) {
            if (!r.running)
                continue;
            if (r.spaceCurrentSize < spaceMaxSize) {
                r.spaceCurrentSize += (GetFrameTime() * 300);
            } else {
                r.spaceCurrentSize = 0;
                r.running = false;
            }
        }
        int movementSpeed = 100;

        if (IsKeyDown(KEY_W)) {
            playerInput.y -= GetFrameTime() * movementSpeed;
        } else if (IsKeyDown(KEY_S)) {
            playerInput.y += GetFrameTime() * movementSpeed;
        } else {
            // playerInput.y = 0;
        }
        if (IsKeyDown(KEY_A)) {
            playerInput.x -= GetFrameTime() * movementSpeed;
        } else if (IsKeyDown(KEY_D)) {
            playerInput.x += GetFrameTime() * movementSpeed;
        } else {
            // playerInput.x = 0;
        }
        // Drawing
        BeginDrawing();
        {
            ClearBackground(backgroundColor);

            DrawRectangle(0, 0, 100, 100, GRAY);

            // Boden
            Rectangle GroundRec = Rectangle{0, screenHeight - 100, screenWidth, 100};
            DrawRectangle(GroundRec.x, GroundRec.y, GroundRec.width, GroundRec.height, WHITE);

            if (IsTextureValid(playerTexture)) {
                Rectangle source = Rectangle{0, 0, 2048, 2048};
                Rectangle playerRect = Rectangle{screenWidth / 2.0f - playerVelocity.x + playerInput.x,
                                                 screenHeight / 2.0f - playerVelocity.y + playerInput.y, 64, 64};
                if (!CheckCollisionRecs(playerRect, GroundRec)) {
                    playerVelocity.y += -9.81f;
                }
                DrawTexturePro(playerTexture, source, playerRect,
                               Vector2{playerRect.width / 2.0f, playerRect.height / 2.0f}, 0, WHITE);

                for (auto &r : rings) {
                    if (r.running) {
                        DrawCircleLines(playerRect.x, playerRect.y, log(r.spaceCurrentSize) * 50, textColor);
                    }
                }
            } else {
                std::cout << "texture not working" << std::endl;
            }

            DrawText("Hello World!", 10, 10, 11, textColor);
        }
        EndDrawing();
    }
    UnloadTexture(playerTexture);
    CloseWindow();
    return 0;
}
