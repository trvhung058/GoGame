#pragma once
#include <vector>
#include <string>

// Pure data structures with NO Raylib dependencies
enum class Stone { Empty, Black, White };
enum class GamePhase { Playing, Scoring, Finished };

struct Point { int col; int row; };

struct Move {
    bool isPass;
    Point position;
};

struct SessionSnapshot {
    std::vector<std::vector<Stone>> board;
    std::vector<std::vector<Stone>> previousBoard;
    Stone sideToMove;
    int capturedByBlack;
    int capturedByWhite;
    int consecutivePasses;
    GamePhase phase;
};