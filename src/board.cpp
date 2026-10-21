#include "board.h"
#include <cmath> 

Board::Board() {
    for (int i = 0; i < 19; i++) {
        for (int j = 0; j < 19; j++) {
            board[i][j] = 0;
        }
    }
}

Board::~Board() {
    UnloadAssets(); 
}

void Board::LoadAssets(const char *path) {
    texBoard = LoadTexture(path);
    isLoaded = true;                                                                                                                                                                                            
}

void Board::UnloadAssets() {
    if (isLoaded) {
        UnloadTexture(texBoard);
        isLoaded = false;
    }
}

void Board::Draw() {
    if (isLoaded) {
        x = 50;
        y = 50;
        DrawTexture(texBoard, x, y, WHITE);
        cell_size = texBoard.height / 20.0f;
        margin = cell_size;
        for (int i = 0; i < 19; i++) {
            DrawLine(x + margin + i * cell_size, y + margin, x + margin + i * cell_size, y + texBoard.height - margin, BLACK);
        }
        for (int i = 0; i < 19; i++) {
            DrawLine(x + margin, y +  margin + i * cell_size, x + texBoard.height - margin, y + margin + i * cell_size, BLACK);
        }
    }
}

std::pair<int, int> Board::GetCellIndex(Vector2 MousePos) {
    float mouseX = MousePos.x;
    float mouseY = MousePos.y;
    int col = static_cast<int>(std::round((mouseX - margin - x) / cell_size));
    int row = static_cast<int>(std::round((mouseY - margin - y) / cell_size));

    if (col < 0 || col >= 19 || row < 0 || row >= 19) {
        return {-1, -1};
    }

    return {col, row}; 
}