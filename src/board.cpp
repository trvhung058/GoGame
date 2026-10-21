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

void Board::LoadAssets(const char *boardPath, const char *blackPath, const char *whitePath) {
    texBoard = LoadTexture(boardPath);
    texBlackStone = LoadTexture(blackPath);
    texWhiteStone = LoadTexture(whitePath);
    isLoaded = true;                                                                                                                                                                                            
}

void Board::UnloadAssets() {
    if (isLoaded) {
        UnloadTexture(texBoard);
        UnloadTexture(texBlackStone);
        UnloadTexture(texWhiteStone);
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
            DrawLine(x + margin, y +  margin + i * cell_size, x + texBoard.height - margin, y + margin + i * cell_size, BLACK);
        }
        
        for (int i = 0; i < 19; i++) {
            for (int j = 0; j < 19; j++) {
                if (board[i][j] == 0) continue;

                float pixelX = x + margin + i * cell_size - texBlackStone.height / 2.0;
                float pixelY = y + margin + j * cell_size - texBlackStone.height / 2.0;

                if (board[i][j] == 1){
                    DrawTexture(texBlackStone, pixelX, pixelY, WHITE);
                }
                else if (board[i][j] == 2){
                    DrawTexture(texWhiteStone, pixelX, pixelY, WHITE);
                }
            }
        }
    }
}

void Board::DrawGhostStone(int col, int row, int currentPlayer) {
    if (board[col][row] != 0) return;

    int pixelX = x + margin + col * cell_size;
    int pixelY = y + margin + row * cell_size;

    Color ghostColor = (currentPlayer == 1) ? BLACK : WHITE;

    DrawCircle(pixelX, pixelY, 15.0f, Fade(ghostColor, 0.5f));
    
}

int Board::GetStone(int col, int row) {
    return board[col][row];
}

void Board::SetStone(int col, int row, int player) {
    board[col][row] = player;
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