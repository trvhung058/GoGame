#include "ai.h"
#include "game_engine.h"
#include <random>
#include <stdexcept>
#include <cstddef>
#include <algorithm>
#include <utility>
#include <limits>

Move chooseEasyMove(const SessionSnapshot& state) {
    if (state.phase != GamePhase::Playing) {
        throw std::invalid_argument("Cannot choose a move when the game is not in the Playing phase.");
    }

    std::vector<Move> legalMoves;
    for (std::size_t col = 0; col < state.board.size(); ++col) {
        for (std::size_t row = 0; row < state.board[col].size(); ++row) {
            Point position{static_cast<int>(col), static_cast<int>(row)};
            if (state.board[col][row] == Stone::Empty) {
                SessionSnapshot tempState = state;
                if (tryPlayStone(tempState, position)) {
                    legalMoves.push_back({false, position});
                }
            }
        }
    }

    if (legalMoves.empty()) {
        return {true, {0, 0}}; // Pass move
    }

    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<std::size_t> dist(0, legalMoves.size() - 1);
    return legalMoves[dist(rng)];
}

int evaluatePosition(const SessionSnapshot& state, Stone aiColor) {
    if (aiColor != Stone::Black && aiColor != Stone::White) {
        throw std::invalid_argument("AI color must be either Black or White.");
    }

    const Stone opponentColor = aiColor == Stone::Black ? Stone::White : Stone::Black;
    const int captureDifference = aiColor == Stone::Black
        ? state.capturedByBlack - state.capturedByWhite
        : state.capturedByWhite - state.capturedByBlack;

    std::vector<std::vector<bool>> visited;
    visited.reserve(state.board.size());
    for (const auto& column : state.board) {
        visited.emplace_back(column.size(), false);
    }

    int aiAtariGroups = 0;
    int opponentAtariGroups = 0;
    for (std::size_t col = 0; col < state.board.size(); ++col) {
        for (std::size_t row = 0; row < state.board[col].size(); ++row) {
            const Stone stone = state.board[col][row];
            if (stone == Stone::Empty || visited[col][row]) {
                continue;
            }

            const std::vector<Point> group = findGroup(state, {
                static_cast<int>(col),
                static_cast<int>(row)
            });
            for (const Point& point : group) {
                visited[point.col][point.row] = true;
            }

            if (countLiberties(state, group) == 1) {
                if (stone == aiColor) {
                    ++aiAtariGroups;
                } else if (stone == opponentColor) {
                    ++opponentAtariGroups;
                }
            }
        }
    }

    return 10 * captureDifference + 3 * (opponentAtariGroups - aiAtariGroups);
}

namespace {
    std::vector<Move> generateCandidateMoves(const SessionSnapshot& state, Stone aiColor, bool maximizingPlayer) {
        if (aiColor != Stone::Black && aiColor != Stone::White) {
            throw std::invalid_argument("AI color must be either Black or White.");
        }

        if (state.phase != GamePhase::Playing) {
            throw std::invalid_argument("Cannot generate moves when the game is not in the Playing phase.");
        }
        
        std::vector<std::pair<Point, int>> moveScores;
        for (std::size_t col = 0; col < state.board.size(); ++col) {
            for (std::size_t row = 0; row < state.board[col].size(); ++row) {
                if (state.board[col][row] == Stone::Empty) {
                    SessionSnapshot tempState = state;
                    Point position{static_cast<int>(col), static_cast<int>(row)};
                    if (tryPlayStone(tempState, position)) {
                        moveScores.push_back({position, evaluatePosition(tempState, aiColor)});
                    }
                }
            }
        }

        if (moveScores.empty()) {
            SessionSnapshot passState = state;
            if (tryPass(passState)) {
                return {{true, {0, 0}}}; // Pass move
            } else {
                throw std::runtime_error("No legal moves available and cannot pass.");
            }
        }

        // Shuffle moveScores to ensure randomness among moves with the same score
        static std::mt19937 rng(std::random_device{}());
        std::shuffle(moveScores.begin(), moveScores.end(), rng);

        std::stable_sort(moveScores.begin(), moveScores.end(), [maximizingPlayer](const auto& a, const auto& b) {
            return maximizingPlayer ? a.second > b.second : a.second < b.second;
        });

        std::vector<Move> candidates;
        for (std::size_t i = 0; i < std::min(moveScores.size(), static_cast<std::size_t>(12)); ++i) {
            candidates.push_back({false, moveScores[i].first});
        }

        return candidates;
    }

    int minimaxScore(const SessionSnapshot& state, int depth, bool maximizingPlayer, Stone aiColor) {
        if (depth <= 0 || state.phase != GamePhase::Playing) {
            return evaluatePosition(state, aiColor);
        }
        std::vector<Move> candidateMoves = generateCandidateMoves(state, aiColor, maximizingPlayer);
        if (candidateMoves.empty()) {
            return evaluatePosition(state, aiColor);
        }
        int bestScore = maximizingPlayer ? std::numeric_limits<int>::lowest() : std::numeric_limits<int>::max();
        bool foundChild = false;
        for (const Move& move : candidateMoves) {
            SessionSnapshot nextState = state;
            if (move.isPass) {
                if (tryPass(nextState)) {
                    foundChild = true;
                } else {
                    continue; // Skip this move if passing is not allowed
                }
            } else {
                if (tryPlayStone(nextState, move.position)) {
                    foundChild = true;
                } else {
                    continue; // Skip this move if it's not legal
                }
            }
            int score = minimaxScore(nextState, depth - 1, !maximizingPlayer, aiColor);
            if (maximizingPlayer) {
                bestScore = std::max(score, bestScore);
            } else {
                bestScore = std::min(score, bestScore);
            }
        }
        return foundChild ? bestScore : evaluatePosition(state, aiColor);
    }
}

Move chooseMediumMove(const SessionSnapshot& state, Stone aiColor) {
    std::vector<Move> candidateMoves = generateCandidateMoves(state, aiColor, true);
    bool foundChild = false;
    int bestScore = std::numeric_limits<int>::lowest();
    Move bestMove = {true, {0, 0}}; // Default to pass if no legal moves are found
    for (const Move& move : candidateMoves) {
        SessionSnapshot nextState = state;
        if (move.isPass) {
            if (!tryPass(nextState)) {
                continue; // Skip this move if passing is not allowed
            }
        } else {
            if (!tryPlayStone(nextState, move.position)) {
                continue; // Skip this move if it's not legal
            }
        }
        int score = minimaxScore(nextState, 1, false, aiColor);
        if (!foundChild || score > bestScore) {
            bestScore = score;
            bestMove = move;
        }
        foundChild = true;
    }
    if (!foundChild) {
        throw std::runtime_error("No legal moves available for the AI.");
    }
    return bestMove;
}