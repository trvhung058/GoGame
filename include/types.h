#pragma once
#include <vector>
#include <string>

// Pure data structures with NO Raylib dependencies
enum class Stone { Empty, Black, White };

struct Point { int col; int row; };

struct Move {
    bool isPass;
    Point position;
};

struct SessionSnapshot {
    std::vector<std::vector<Stone>> board;
    Stone sideToMove;
    int capturedByBlack;
    int capturedByWhite;
};