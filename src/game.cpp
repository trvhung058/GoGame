#include "game.h"

Game::Game(){
    board.LoadAssets("assets/board/wood1.jpg", "assets/b_40.png", "assets/w_40.png");
    background.LoadAssets("assets/go-jeu_1000_blur.jpg");
    background.SetPosition(0, 0);
    logo.LoadAssets("assets/UI/icon258.png");
    logo.SetPosition(371, 200);

    currentPlayer = 1;
    CurrentScreen = MENU;
}

void Game::Draw(){
    board.Draw();

    if (!moves.empty()) {
        int col = moves.back().col;
        int row = moves.back().row;
        int player = moves.back().player;

        auto pixelPosition = board.GetPixelPosition(col, row);
        int pixelX = pixelPosition.first;
        int pixelY = pixelPosition.second;

        if (player == 1) {
            DrawRectangle(pixelX - 7.0f, pixelY - 7.0f, 14.0f, 14.0f, WHITE);
        }
        else if (player == 2) {
            DrawRectangle(pixelX - 7.0f, pixelY - 7.0f, 14.0f, 14.0f, BLACK);
        }
    }
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

    moves.push_back({col, row, currentPlayer});

    currentPlayer = 3 - currentPlayer;
}

std::pair<int, int> Game::GetCellIndex(Vector2 MousePos){
    return board.GetCellIndex(MousePos);
}

void Game::GameScreen() {
    switch (CurrentScreen){
        case MENU: 
        {
            background.Draw(true);
            logo.Draw(false);
        } break;
    }
}