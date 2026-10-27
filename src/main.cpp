#include "raylib.h"
#include "game.h"
#include <iostream>

const int W = 1500;
const int H = 1000;

int main() {
    InitWindow(W, H, "Go Game");

    SetTargetFPS(60); 

    Game game;

    while (!WindowShouldClose() && !game.ShouldClose()) {
        game.Update();   
        
        BeginDrawing();

        ClearBackground(RAYWHITE);

        game.Draw();  
        
        EndDrawing();
    }

    CloseWindow();
    return 0;
}
