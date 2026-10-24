#pragma once
#include <vector>
#include <string>

// Pure data structures with NO Raylib dependencies
enum class Stone { Empty, Black, White };
enum class GamePhase { Playing, Scoring, Finished };
enum class TerritoryOwner {
    Unresolved,
    Neutral,
    Black,
    White
};

using TerritoryMap = std::vector<std::vector<TerritoryOwner>>;

struct Point { int col; int row; };

struct Move {
    bool isPass;
    Point position;
};

struct SessionSnapshot {
    std::vector<std::vector<Stone>> board;
    std::vector<std::vector<Stone>> previousBoard;
    TerritoryMap territoryProposal;
    Stone sideToMove;
    int capturedByBlack;
    int capturedByWhite;
    int consecutivePasses;
    GamePhase phase;
    bool blackAcceptedScore;
    bool whiteAcceptedScore;
};

struct Score {
    double black;
    double white;
};