#include "raylib.h"
#include "board.h"
#include <iostream>

const int W = 1000;
const int H = 1000;

int main() {
    InitWindow(W, H, "Go Game");

    SetTargetFPS(60); 

    Board board;
    board.LoadAssets("assets/board/wood_768.jpg", "assets/b.png", "assets/w.png");


    while (!WindowShouldClose()) {

        // if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        //     std::pair<int,int> res = board.GetCellIndex(GetMousePosition());
        //     std::cout << res.first << " " << res.second << '\n';
        // }

        BeginDrawing();
        
        ClearBackground(RAYWHITE); 
        

        board.Draw();

        // std::pair<int,int> cell = board.GetCellIndex(GetMousePosition());

        // if (cell != std::pair<int, int>{-1, -1}){
        //     board.DrawGhostStone(cell.second, cell.first, 1);
        // }


        EndDrawing();
    }

    CloseWindow();
    return 0;
}