#include "board.h"
#include <cmath> 
#include <limits>

Board::Board() {
    x = 80;
    y = 100;
    margin = 0;
    cell_size = 0;
    currentBoardSize = 19;
}

Board::~Board() {
    UnloadAssets(); 
}

void Board::LoadAllAssets() {
    if (isLoaded) return;

    // Board skins
    boardTextures.push_back(LoadTexture("assets/board/Board_800.png"));           // ClassicWood = 0
    boardTextures.push_back(LoadTexture("assets/board/Hardwood_board_800.png"));  // Hardwood = 1
    boardTextures.push_back(LoadTexture("assets/board/wood_800.png"));            // DarkWood = 2

    std::vector<Texture2D> bClassic = {
        LoadTexture("assets/stone/b_40.png"),
        LoadTexture("assets/stone/b_50.png"),
        LoadTexture("assets/stone/b_70.png")
    };
    std::vector<Texture2D> wClassic = {
        LoadTexture("assets/stone/w_40.png"),
        LoadTexture("assets/stone/w_50.png"),
        LoadTexture("assets/stone/w_70.png")
    };
    blackStoneTextures.push_back(bClassic);
    whiteStoneTextures.push_back(wClassic);

    std::vector<Texture2D> bFlat = {
        LoadTexture("assets/stone/b_2_40.png"),
        LoadTexture("assets/stone/b_2_50.png"),
        LoadTexture("assets/stone/b_2_70.png")
    };
    std::vector<Texture2D> wFlat = {
        LoadTexture("assets/stone/w_2_40.png"),
        LoadTexture("assets/stone/w_2_50.png"),
        LoadTexture("assets/stone/w_2_70.png")
    };
    blackStoneTextures.push_back(bFlat);
    whiteStoneTextures.push_back(wFlat);

    isLoaded = true;
}

void Board::UnloadAssets() {
    if (!isLoaded) return;

    for (auto& tex : boardTextures) {
        if (tex.id != 0) UnloadTexture(tex);
    }
    boardTextures.clear();

    for (auto& skinVec : blackStoneTextures) {
        for (auto& tex : skinVec) {
            if (tex.id != 0) UnloadTexture(tex);
        }
    }
    blackStoneTextures.clear();

    for (auto& skinVec : whiteStoneTextures) {
        for (auto& tex : skinVec) {
            if (tex.id != 0) UnloadTexture(tex);
        }
    }
    whiteStoneTextures.clear();

    isLoaded = false;
}

int Board::GetSizeIndex(int boardSize) const {
    if (boardSize <= 0) boardSize = currentBoardSize;
    if (boardSize <= 9) return 2;  
    if (boardSize <= 13) return 1;
    return 0;                      
}

Texture2D Board::GetBoardTexture(BoardSkinType skin) const {
    size_t idx = static_cast<size_t>(skin);
    if (idx < boardTextures.size()) {
        return boardTextures[idx];
    }
    return { 0 };
}

Texture2D Board::GetBlackStoneTexture(StoneSkinType skin, int boardSize) const {
    size_t sIdx = static_cast<size_t>(skin);
    if (sIdx < blackStoneTextures.size()) {
        size_t szIdx = static_cast<size_t>(GetSizeIndex(boardSize));
        if (szIdx < blackStoneTextures[sIdx].size()) {
            return blackStoneTextures[sIdx][szIdx];
        }
    }
    return { 0 };
}

Texture2D Board::GetWhiteStoneTexture(StoneSkinType skin, int boardSize) const {
    size_t sIdx = static_cast<size_t>(skin);
    if (sIdx < whiteStoneTextures.size()) {
        size_t szIdx = static_cast<size_t>(GetSizeIndex(boardSize));
        if (szIdx < whiteStoneTextures[sIdx].size()) {
            return whiteStoneTextures[sIdx][szIdx];
        }
    }
    return { 0 };
}

void Board::Draw(int sz) {
    if (!isLoaded) return;

    currentBoardSize = sz;
    Texture2D texBoard = GetBoardTexture(currentBoardSkin);
    if (texBoard.id == 0) return;

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

    if (currentStoneSkin == StoneSkinType::PureSolid) {
        float radius = (cell_size > 0) ? (cell_size * 0.46f) : 18.0f;
        if (player == Stone::Black) {
            DrawCircle(static_cast<int>(pixelPos.x), static_cast<int>(pixelPos.y), radius, Color{15, 15, 15, 255});
            DrawCircleLines(static_cast<int>(pixelPos.x), static_cast<int>(pixelPos.y), radius, Color{70, 70, 70, 255});
        } else if (player == Stone::White) {
            DrawCircle(static_cast<int>(pixelPos.x), static_cast<int>(pixelPos.y), radius, Color{245, 245, 245, 255});
            DrawCircleLines(static_cast<int>(pixelPos.x), static_cast<int>(pixelPos.y), radius, Color{180, 180, 180, 255});
        }
        return;
    }

    Texture2D texBlackStone = GetBlackStoneTexture(currentStoneSkin, currentBoardSize);
    Texture2D texWhiteStone = GetWhiteStoneTexture(currentStoneSkin, currentBoardSize);

    if (player == Stone::Black && texBlackStone.id != 0) {
        float stoneHalfSize = texBlackStone.width / 2.0f;
        DrawTexture(
            texBlackStone,
            static_cast<int>(pixelPos.x - stoneHalfSize),
            static_cast<int>(pixelPos.y - stoneHalfSize),
            WHITE
        );
    } else if (player == Stone::White && texWhiteStone.id != 0) {
        float stoneHalfSize = texWhiteStone.width / 2.0f;
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

    float radius = (cell_size > 0) ? (cell_size * 0.46f) : 16.0f;
    DrawCircle(
        static_cast<int>(pixelPos.x),
        static_cast<int>(pixelPos.y),
        radius,
        Fade(ghostColor, 0.45f)
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
    float markerSize = (cell_size > 0) ? (cell_size * 0.35f) : 14.0f;

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
