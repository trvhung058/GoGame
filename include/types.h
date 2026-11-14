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

struct Point { std::size_t col; std::size_t row; };

struct Move {
    bool isPass;
    Point position;
};

struct SessionSnapshot {
    std::vector<std::vector<Stone>> board;
    std::vector<std::vector<Stone>> previousBoard;
    TerritoryMap territoryProposal;
    DeadStoneMap deadStones;
    Stone sideToMove;
    int capturedByBlack;
    int capturedByWhite;
    int consecutivePasses;
    GamePhase phase;
    bool blackAcceptedScore;
    bool whiteAcceptedScore;
};

struct GameSession {
    SessionSnapshot currentState{};
    std::vector<SessionSnapshot> undoHistory;
    std::vector<SessionSnapshot> redoHistory;
    GameMode mode = GameMode::PvP;
};

struct Score {
    double black;
    double white;
};