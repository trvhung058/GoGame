#pragma once
#include "board.h" 
#include "UI.h"
#include <vector>
#include <array>
#include <iostream>

struct move {
    int col;
    int row;
    int player;
};

class Game {
    private:
        Board board;
        int currentPlayer;
        std::vector<move> moves;
        UI background;
        UI logo;
        enum Scene {
            MENU,
            PVP,
            PVE,
            EXIT
        } CurrentScreen;

    public:
        Game();

        std::pair<int, int> GetCellIndex(Vector2 MousePos);
        void Draw();
        void GhostStone(int col, int row);
        void PlaceStone(int col, int row);
        void GameScreen();
};