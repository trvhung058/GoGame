#pragma once
#include "board.h" 


class Game {
    private:
        Board board;
        int currentPlayer;

    public:
        Game();

        std::pair<int, int> GetCellIndex(Vector2 MousePos);
        void Draw();
        void GhostStone(int col, int row);
        void PlaceStone(int col, int row);

};