#include "raylib.h"
#include "game.h"
#include <iostream>

const int W = 1000;
const int H = 1000;

int main() {
    InitWindow(W, H, "Go Game");

    SetTargetFPS(60); 

    Game game;


    while (!WindowShouldClose()) {

        // std::pair<int,int> res = game.GetCellIndex(GetMousePosition());
        // int col = res.first, row = res.second;

        // if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        //     game.PlaceStone(col, row);
        // }

        BeginDrawing();
        
        ClearBackground(RAYWHITE); 
        
        // game.Draw();
        // game.GhostStone(col, row);
        game.GameScreen();

        


        EndDrawing();
    }

    CloseWindow();
    return 0;
}