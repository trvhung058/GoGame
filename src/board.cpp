#include "board.h"
#include <cmath> 

Board::Board() {
    x = 80;
    y = 100;
    margin = 0;
    cell_size = 0;
}

Board::~Board() {
    UnloadAssets(); 
}

void Board::LoadAssetsDefault(const char *boardPath, const char *blackPath, const char *whitePath) {
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

void Board::Draw(int sz) {
    if (!isLoaded) return;

    DrawTexture(texBoard, x, y, WHITE);
    cell_size = texBoard.height / static_cast<float>(sz + 1);
    margin = cell_size;

    for (int i = 0; i < sz; i++) {
        DrawLine(x + margin + i * cell_size, y + margin, x + margin + i * cell_size, y + texBoard.height - margin, BLACK);
        DrawLine(x + margin, y + margin + i * cell_size, x + texBoard.height - margin, y + margin + i * cell_size, BLACK);
    }
}

void Board::Draw(const std::vector<std::vector<Stone>>& grid) {
    Draw(grid.size()); 

    if (!isLoaded) return;

    int cols = static_cast<int>(grid.size());
    for (int c = 0; c < cols; c++) {
        int rows = static_cast<int>(grid[c].size());
        for (int r = 0; r < rows; r++) {
            DrawStone(c, r, grid[c][r]);
        }
    }
}

void Board::Draw(const SessionSnapshot& state) {
    Draw(state.board);
}

void Board::DrawStone(Point pos, Stone player) {
    DrawStone(pos.col, pos.row, player);
}

void Board::DrawStone(int col, int row, Stone player) {
    if (player == Stone::Empty || !isLoaded) return;

    Vector2 pixelPos = GetPixelPosition(col, row);
    float stoneHalfSize = texBlackStone.height / 2.0f;

    if (player == Stone::Black) {
        DrawTexture(texBlackStone, pixelPos.x - stoneHalfSize, pixelPos.y - stoneHalfSize, WHITE);
    } else if (player == Stone::White) {
        DrawTexture(texWhiteStone, pixelPos.x - stoneHalfSize, pixelPos.y - stoneHalfSize, WHITE);
    }
}

void Board::DrawGhostStone(Point pos, Stone player) {
    DrawGhostStone(pos.col, pos.row, player);
}

void Board::DrawGhostStone(int col, int row, Stone player) {
    if (player == Stone::Empty || !isLoaded) return;

    Vector2 pixelPos = GetPixelPosition(col, row);
    Color ghostColor = (player == Stone::Black) ? BLACK : WHITE;

    DrawCircle(pixelPos.x, pixelPos.y, 15.0f, Fade(ghostColor, 0.5f));
}

void Board::DrawLastPlayed(Point pos, Stone player) {
    DrawLastPlayed(pos.col, pos.row, player);
}

void Board::DrawLastPlayed(int col, int row, Stone player) {
    if (player == Stone::Empty || !isLoaded) return;

    Vector2 pixelPos = GetPixelPosition(col, row);
    Color markerColor = (player == Stone::Black) ? WHITE : BLACK;
    float markerSize = 14.0f;

    DrawRectangle(pixelPos.x - markerSize / 2.0f, pixelPos.y - markerSize / 2.0f, markerSize, markerSize, markerColor);
}

Point Board::GetCellIndex(Vector2 mousePos) {
    if (cell_size <= 0) return {-1, -1};

    int col = static_cast<int>(std::round((mousePos.x - margin - x) / cell_size));
    int row = static_cast<int>(std::round((mousePos.y - margin - y) / cell_size));

    if (col < 0 || col >= 19 || row < 0 || row >= 19) {
        return {-1, -1};
    }

    return {col, row}; 
}

Vector2 Board::GetPixelPosition(Point pos) {
    return GetPixelPosition(pos.col, pos.row);
}

Vector2 Board::GetPixelPosition(int col, int row) {
    float pixelX = x + margin + col * cell_size;
    float pixelY = y + margin + row * cell_size;

    return {pixelX, pixelY};
}
