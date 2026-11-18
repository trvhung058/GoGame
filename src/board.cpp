#include "board.h"
#include <cmath> 
#include <limits>

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

void Board::LoadAssetsBlackStone(const char *path) {
    texBlackStone = LoadTexture(path);
}

void Board::LoadAssetsWhiteStone(const char *path) {
    texWhiteStone = LoadTexture(path);
}

void Board::Draw(int sz) {
    if (!isLoaded) return;

    DrawTexture(texBoard, x, y, WHITE);
    cell_size = texBoard.height / static_cast<float>(sz + 1);
    margin = cell_size;

    for (int i = 0; i < sz; i++) {
        DrawLine(
            static_cast<int>(x + margin + i * cell_size),
            static_cast<int>(y + margin),
            static_cast<int>(x + margin + i * cell_size),
            static_cast<int>(y + texBoard.height - margin),
            BLACK
        );
        DrawLine(
            static_cast<int>(x + margin),
            static_cast<int>(y + margin + i * cell_size),
            static_cast<int>(x + texBoard.height - margin),
            static_cast<int>(y + margin + i * cell_size),
            BLACK
        );
    }
}

void Board::Draw(const std::vector<std::vector<Stone>>& grid) {
    Draw(static_cast<int>(grid.size()));

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
    DrawStone(static_cast<int>(pos.col), static_cast<int>(pos.row), player);
}

void Board::DrawStone(int col, int row, Stone player) {
    if (player == Stone::Empty || !isLoaded) return;

    Vector2 pixelPos = GetPixelPosition(col, row);
    float stoneHalfSize = texBlackStone.height / 2.0f;

    if (player == Stone::Black) {
        DrawTexture(
            texBlackStone,
            static_cast<int>(pixelPos.x - stoneHalfSize),
            static_cast<int>(pixelPos.y - stoneHalfSize),
            WHITE
        );
    } else if (player == Stone::White) {
        DrawTexture(
            texWhiteStone,
            static_cast<int>(pixelPos.x - stoneHalfSize),
            static_cast<int>(pixelPos.y - stoneHalfSize),
            WHITE
        );
    }
}

void Board::DrawGhostStone(Point pos, Stone player) {
    DrawGhostStone(static_cast<int>(pos.col), static_cast<int>(pos.row), player);
}

void Board::DrawGhostStone(int col, int row, Stone player) {
    if (player == Stone::Empty || !isLoaded) return;

    Vector2 pixelPos = GetPixelPosition(col, row);
    Color ghostColor = (player == Stone::Black) ? BLACK : WHITE;

    DrawCircle(
        static_cast<int>(pixelPos.x),
        static_cast<int>(pixelPos.y),
        15.0f,
        Fade(ghostColor, 0.5f)
    );
}

void Board::DrawLastPlayed(Move lastMove, Stone player) {
    if (lastMove.isPass) return;
    Point pos = lastMove.position;
    DrawLastPlayed(static_cast<int>(pos.col), static_cast<int>(pos.row), player);
}

void Board::DrawLastPlayed(int col, int row, Stone player) {
    if (player == Stone::Empty || !isLoaded) return;

    Vector2 pixelPos = GetPixelPosition(col, row);
    Color markerColor = (player == Stone::Black) ? WHITE : BLACK;
    float markerSize = texBlackStone.width / 3.0f;

    DrawRectangle(
        static_cast<int>(pixelPos.x - markerSize / 2.0f),
        static_cast<int>(pixelPos.y - markerSize / 2.0f),
        static_cast<int>(markerSize),
        static_cast<int>(markerSize),
        markerColor
    );
}

Point Board::GetCellIndex(Vector2 mousePos, int boardSize) {
    const std::size_t invalidCoordinate = std::numeric_limits<std::size_t>::max();
    if (cell_size <= 0) return {invalidCoordinate, invalidCoordinate};

    int col = static_cast<int>(std::round((mousePos.x - margin - x) / cell_size));
    int row = static_cast<int>(std::round((mousePos.y - margin - y) / cell_size));

    if (col < 0 || col >= boardSize || row < 0 || row >= boardSize) {
        return {invalidCoordinate, invalidCoordinate};
    }

    return {static_cast<std::size_t>(col), static_cast<std::size_t>(row)};
}

Vector2 Board::GetPixelPosition(Point pos) {
    return GetPixelPosition(static_cast<int>(pos.col), static_cast<int>(pos.row));
}

Vector2 Board::GetPixelPosition(int col, int row) {
    float pixelX = x + margin + col * cell_size;
    float pixelY = y + margin + row * cell_size;

    return {pixelX, pixelY};
}
