#include "game.h"

Game::Game(){
    board.LoadAssets("assets/board/wood1.jpg", "assets/b_40.png", "assets/w_40.png");

    currentPlayer = 1;
}

void Game::Draw(){
    board.Draw();
}

void Game::GhostStone(int col, int row) {
    std::pair<int,int> cell = board.GetCellIndex(GetMousePosition());

    if (cell != std::pair<int, int>{-1, -1}){
        board.DrawGhostStone(cell.first, cell.second, currentPlayer);
    }
}

void Game::PlaceStone(int col, int row) {
    if (board.GetStone(col, row) != 0) return;

    board.SetStone(col, row, currentPlayer);

    currentPlayer = 3 - currentPlayer;
}

std::pair<int, int> Game::GetCellIndex(Vector2 MousePos){
    return board.GetCellIndex(MousePos);
}