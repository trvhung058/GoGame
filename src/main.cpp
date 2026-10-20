#include "raylib.h"

int main() {
    const int screenWidth = 800;
    const int screenHeight = 600;
    InitWindow(screenWidth, screenHeight, "Go Game - C++ & Raylib");

    SetTargetFPS(60); 

    while (!WindowShouldClose()) {
        BeginDrawing();
        
        ClearBackground(RAYWHITE); 
        
        DrawText("Ban co Raylib da duoc khoi tao!", 190, 200, 20, LIGHTGRAY);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}