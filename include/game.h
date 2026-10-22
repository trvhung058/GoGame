#pragma once
#include "board.h" 
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
        enum GameScreen {
            MENU,
            PVP,
            PVE,
            EXIT
        } GameScreen;

    public:
        Game();

        std::pair<int, int> GetCellIndex(Vector2 MousePos);
        void Draw();
        void GhostStone(int col, int row);
        void PlaceStone(int col, int row);

};