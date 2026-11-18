#pragma once
#include <cstddef>
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
enum class GameMode { PvP, PvE };

using TerritoryMap = std::vector<std::vector<TerritoryOwner>>;
using DeadStoneMap = std::vector<std::vector<bool>>;
using BoardGrid = std::vector<std::vector<Stone>>;
using PositionHistory = std::vector<BoardGrid>;

struct Point { std::size_t col; std::size_t row; };

struct Move {
    bool isPass;
    Point position;
};

struct SessionSnapshot {
    BoardGrid board;
    BoardGrid previousBoard;
    TerritoryMap territoryProposal;
    DeadStoneMap deadStones;
    Stone sideToMove;
    int capturedByBlack;
    int capturedByWhite;
    int consecutivePasses;
    std::size_t positionHistorySize = 0;
    GamePhase phase;
    bool blackAcceptedScore;
    bool whiteAcceptedScore;
    Move lastMove{ true, {0, 0} };
};

struct GameSession {
    SessionSnapshot currentState{};
    std::vector<SessionSnapshot> undoHistory;
    std::vector<SessionSnapshot> redoHistory;
    PositionHistory positionHistory;
    GameMode mode = GameMode::PvP;
};

struct Score {
    double black;
    double white;
};