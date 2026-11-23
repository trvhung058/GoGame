#include "ai.h"
#include "game_engine.h"
#include <random>
#include <stdexcept>
#include <cstddef>
#include <algorithm>
#include <utility>
#include <limits>
#include <chrono>
#include <tuple>

Move chooseEasyMove(const SessionSnapshot& state, const PositionHistory& positionHistory) {
    if (state.positionHistorySize == 0 ||
        state.positionHistorySize > positionHistory.size()) {
        throw std::invalid_argument("Invalid position history.");
    }
    if (state.phase != GamePhase::Playing) {
        throw std::invalid_argument("Cannot choose a move when the game is not in the Playing phase.");
    }

    if (state.consecutivePasses == 1) {
        SessionSnapshot tempState = state;
        if (tryPass(tempState)) {
            if (evaluatePosition(tempState, state.sideToMove) >= 0) {
                return {true, {0, 0}}; // Pass move
            }
        }
    }

    std::vector<Move> allLegalMoves;
    std::vector<Move> usefulMoves;
    GameSession tempSession;
    tempSession.currentState = state;
    tempSession.positionHistory = positionHistory;
    for (std::size_t col = 0; col < state.board.size(); ++col) {
        for (std::size_t row = 0; row < state.board[col].size(); ++row) {
            Point position{col, row};
            if (state.board[col][row] == Stone::Empty) {
                SessionSnapshot tempState = state;
                if (tryPlayStone(tempState, position)) {
                    bool isRepeatedPosition = false;
                    for (std::size_t i = 0; i < state.positionHistorySize; ++i) {
                        if (tempState.board == positionHistory[i]) {
                            isRepeatedPosition = true;
                            break;
                        }
                    }
                    if (!isRepeatedPosition) {
                        allLegalMoves.push_back({false, position});
                        if (!shouldWarnAboutPointlessMove(tempSession, position)) {
                            usefulMoves.push_back({false, position});
                        }
                    }
                }
            }
        }
    }

    if (allLegalMoves.empty()) {
        return {true, {0, 0}}; // Pass move
    }

    if (!usefulMoves.empty()) {
        static std::mt19937 rng(std::random_device{}());
        std::uniform_int_distribution<std::size_t> dist(0, usefulMoves.size() - 1);
        return usefulMoves[dist(rng)];
    }

    static std::mt19937 rng(std::random_device{}());
    std::uniform_int_distribution<std::size_t> dist(0, allLegalMoves.size() - 1);
    return allLegalMoves[dist(rng)];
}

int evaluatePosition(const SessionSnapshot& state, Stone aiColor) {
    if (aiColor != Stone::Black && aiColor != Stone::White) {
        throw std::invalid_argument("AI color must be either Black or White.");
    }
    if (state.phase == GamePhase::Scoring || state.phase == GamePhase::Finished) {
        Score areaScore = calculateAreaScore(state, kKomi);
        double aiScore = (aiColor == Stone::Black ? areaScore.black : areaScore.white);
        double opponentScore = (aiColor == Stone::Black ? areaScore.white : areaScore.black);
        return static_cast<int>((aiScore - opponentScore) * 2.0); // Multiply by 2 to convert to integer
    }

    const Stone opponentColor = aiColor == Stone::Black ? Stone::White : Stone::Black;
    SessionSnapshot scoringState = state;
    scoringState.phase = GamePhase::Scoring;
    scoringState.territoryProposal = generateTerritoryProposal(scoringState);
    Score areaScore = calculateAreaScore(scoringState, kKomi);
    double aiScore = (aiColor == Stone::Black ? areaScore.black : areaScore.white);
    double opponentScore = (aiColor == Stone::Black ? areaScore.white : areaScore.black);
    const int areaScoreDifference = static_cast<int>((aiScore - opponentScore) * 2.0);

    std::vector<std::vector<bool>> visited;
    visited.reserve(state.board.size());
    for (const auto& column : state.board) {
        visited.emplace_back(column.size(), false);
    }

    int aiAtariStones = 0;
    int opponentAtariStones = 0;
    for (std::size_t col = 0; col < state.board.size(); ++col) {
        for (std::size_t row = 0; row < state.board[col].size(); ++row) {
            const Stone stone = state.board[col][row];
            if (stone == Stone::Empty || visited[col][row]) {
                continue;
            }

            const std::vector<Point> group = findGroup(state, {
                col,
                row
            });
            for (const Point& point : group) {
                visited[point.col][point.row] = true;
            }

            if (countLiberties(state, group) == 1) {
                if (stone == aiColor) {
                    aiAtariStones += static_cast<int>(group.size());
                } else if (stone == opponentColor) {
                    opponentAtariStones += static_cast<int>(group.size());
                }
            }
        }
    }

    return areaScoreDifference + 3 * (opponentAtariStones - aiAtariStones);
}

namespace {
    bool containsPosition(const PositionHistory& positionHistory, const BoardGrid& board) {
        return std::find(positionHistory.begin(), positionHistory.end(), board) != positionHistory.end();
    }

    std::vector<Move> generateCandidateMoves(const SessionSnapshot& state, Stone aiColor, bool maximizingPlayer, const PositionHistory& positionHistory, const std::chrono::steady_clock::time_point* deadline = nullptr, bool* timeOut = nullptr) {
        if (aiColor != Stone::Black && aiColor != Stone::White) {
            throw std::invalid_argument("AI color must be either Black or White.");
        }

        if (state.phase != GamePhase::Playing) {
            throw std::invalid_argument("Cannot generate moves when the game is not in the Playing phase.");
        }

        GameSession tempSession;
        tempSession.currentState = state;
        tempSession.positionHistory = positionHistory;
        
        std::vector<std::tuple<Point, int, bool>> moveScores;
        std::vector<std::pair<Point, int>> captureMoves;
        std::vector<std::pair<Point, int>> savingMoves;
        const int neighborColOffsets[] = {0, 0, -1, 1};
        const int neighborRowOffsets[] = {-1, 1, 0, 0};
        bool scanTimedOut = false;
        for (std::size_t col = 0; col < state.board.size(); ++col) {
            for (std::size_t row = 0; row < state.board[col].size(); ++row) {
                if (state.board[col][row] == Stone::Empty) {
                    SessionSnapshot tempState = state;
                    Point position{col, row};
                    if (tryPlayStone(tempState, position)) {
                        if (!containsPosition(positionHistory, tempState.board)) {
                            bool warn = shouldWarnAboutPointlessMove(tempSession, position);
                            const int score = evaluatePosition(tempState, aiColor);
                            moveScores.push_back({position, score, warn});
                            const bool capturedStone = state.sideToMove == Stone::Black
                                ? tempState.capturedByBlack > state.capturedByBlack
                                : tempState.capturedByWhite > state.capturedByWhite;
                            if (capturedStone) {
                                captureMoves.push_back({position, score});
                            }

                            for (std::size_t neighborIndex = 0; neighborIndex < 4; ++neighborIndex) {
                                const std::size_t neighborCol = position.col + neighborColOffsets[neighborIndex];
                                const std::size_t neighborRow = position.row + neighborRowOffsets[neighborIndex];
                                if (!isOnBoard(neighborCol, neighborRow, state.board.size()) ||
                                    state.board[neighborCol][neighborRow] != state.sideToMove) {
                                    continue;
                                }

                                const std::vector<Point> previousGroup = findGroup(state, {neighborCol, neighborRow});
                                if (countLiberties(state, previousGroup) != 1) {
                                    continue;
                                }
                                const std::vector<Point> updatedGroup = findGroup(tempState, {neighborCol, neighborRow});
                                if (countLiberties(tempState, updatedGroup) > 1) {
                                    savingMoves.push_back({position, score});
                                    break;
                                }
                            }
                        }
                    }
                }
                if (deadline && std::chrono::steady_clock::now() >= *deadline) {
                    if (timeOut) {
                        *timeOut = true;
                    }
                    scanTimedOut = true;
                    break;
                }
            }
            if (scanTimedOut) {
                break;
            }
        }

        if (scanTimedOut && moveScores.empty()) {
            for (std::size_t col = 0; col < state.board.size(); ++col) {
                for (std::size_t row = 0; row < state.board[col].size(); ++row) {
                    if (state.board[col][row] != Stone::Empty) {
                        continue;
                    }
                    SessionSnapshot tempState = state;
                    Point position{col, row};
                    if (tryPlayStone(tempState, position) &&
                        !containsPosition(positionHistory, tempState.board)) {
                        return {{false, position}};
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

        static std::mt19937 rng(std::random_device{}());
        std::vector<std::tuple<Point, int, bool>> unflaggedMoves;
        unflaggedMoves.reserve(moveScores.size());
        for (const auto& moveScore : moveScores) {
            if (!std::get<2>(moveScore)) {
                unflaggedMoves.push_back(moveScore);
            }
        }

        std::shuffle(unflaggedMoves.begin(), unflaggedMoves.end(), rng);
        std::stable_sort(unflaggedMoves.begin(), unflaggedMoves.end(), [maximizingPlayer](const auto& a, const auto& b) {
            return maximizingPlayer
                ? std::get<1>(a) > std::get<1>(b)
                : std::get<1>(a) < std::get<1>(b);
        });
        std::stable_sort(captureMoves.begin(), captureMoves.end(), [maximizingPlayer](const auto& a, const auto& b) {
            return maximizingPlayer ? a.second > b.second : a.second < b.second;
        });
        std::stable_sort(savingMoves.begin(), savingMoves.end(), [maximizingPlayer](const auto& a, const auto& b) {
            return maximizingPlayer ? a.second > b.second : a.second < b.second;
        });

        std::vector<Move> candidates;
        for (std::size_t i = 0; i < std::min(unflaggedMoves.size(), static_cast<std::size_t>(12)); ++i) {
            candidates.push_back({false, std::get<0>(unflaggedMoves[i])});
        }

        std::size_t additionalCaptureCount = 0;
        for (const auto& captureMove : captureMoves) {
            const Point& position = captureMove.first;
            const auto existingMove = std::find_if(candidates.begin(), candidates.end(), [&position](const Move& move) {
                return !move.isPass &&
                    move.position.col == position.col &&
                    move.position.row == position.row;
            });
            if (existingMove == candidates.end()) {
                candidates.push_back({false, position});
                ++additionalCaptureCount;
                if (additionalCaptureCount == 4) {
                    break;
                }
            }
        }

        std::size_t additionalSavingCount = 0;
        for (const auto& savingMove : savingMoves) {
            const Point& position = savingMove.first;
            const auto existingMove = std::find_if(candidates.begin(), candidates.end(), [&position](const Move& move) {
                return !move.isPass &&
                    move.position.col == position.col &&
                    move.position.row == position.row;
            });
            if (existingMove == candidates.end()) {
                candidates.push_back({false, position});
                ++additionalSavingCount;
                if (additionalSavingCount == 4) {
                    break;
                }
            }
        }

        if (scanTimedOut && candidates.empty() && !moveScores.empty()) {
            const auto bestMove = std::max_element(
                moveScores.begin(),
                moveScores.end(),
                [maximizingPlayer](const auto& a, const auto& b) {
                    return maximizingPlayer
                        ? std::get<1>(a) < std::get<1>(b)
                        : std::get<1>(a) > std::get<1>(b);
                });
            candidates.push_back({false, std::get<0>(*bestMove)});
        }

        const bool shouldOfferPass = !scanTimedOut &&
            (state.consecutivePasses > 0 || unflaggedMoves.empty());

        if (shouldOfferPass) {
            SessionSnapshot passState = state;
            if (tryPass(passState)) {
                candidates.push_back({true, {0, 0}});
            }
        }

        return candidates;
    }

    int minimaxScore(const SessionSnapshot& state, int depth, bool maximizingPlayer, Stone aiColor, const PositionHistory& positionHistory) {
        if (depth <= 0 || state.phase != GamePhase::Playing) {
            return evaluatePosition(state, aiColor);
        }
        std::vector<Move> candidateMoves = generateCandidateMoves(state, aiColor, maximizingPlayer, positionHistory);
        if (candidateMoves.empty()) {
            return evaluatePosition(state, aiColor);
        }
        int bestScore = maximizingPlayer ? std::numeric_limits<int>::lowest() : std::numeric_limits<int>::max();
        bool foundChild = false;
        for (const Move& move : candidateMoves) {
            SessionSnapshot nextState = state;
            PositionHistory branchHistory = positionHistory;
            if (move.isPass) {
                if (tryPass(nextState)) {
                    foundChild = true;
                } else {
                    continue; // Skip this move if passing is not allowed
                }
            } else {
                if (!tryPlayStone(nextState, move.position)) {
                    continue; // Skip this move if it's not legal
                }
                if (containsPosition(positionHistory, nextState.board)) {
                    continue;
                }
                branchHistory.push_back(nextState.board);
                foundChild = true;
            }
            int score = minimaxScore(nextState, depth - 1, !maximizingPlayer, aiColor, branchHistory);
            if (maximizingPlayer) {
                bestScore = std::max(score, bestScore);
            } else {
                bestScore = std::min(score, bestScore);
            }
        }
        return foundChild ? bestScore : evaluatePosition(state, aiColor);
    }

    int alphaBetaScore(const SessionSnapshot& state, int depth, bool maximizingPlayer, Stone aiColor, int alpha, int beta, std::chrono::steady_clock::time_point deadline, bool& timedOut, const PositionHistory& positionHistory) {
        if (std::chrono::steady_clock::now() >= deadline) {
            timedOut = true;
            return evaluatePosition(state, aiColor);
        }
        if (depth <= 0 || state.phase != GamePhase::Playing) {
            return evaluatePosition(state, aiColor);
        }
        std::vector<Move> candidateMoves = generateCandidateMoves(state, aiColor, maximizingPlayer, positionHistory, &deadline, &timedOut);
        if (timedOut || candidateMoves.empty()) {
            return evaluatePosition(state, aiColor);
        }
        bool foundChild = false;
        int bestScore = maximizingPlayer ? std::numeric_limits<int>::lowest() : std::numeric_limits<int>::max();
        for (const Move& move : candidateMoves) {
            SessionSnapshot nextState = state;
            PositionHistory branchHistory = positionHistory;
            if (move.isPass) {
                if (tryPass(nextState)) {
                    foundChild = true;
                } else {
                    continue; // Skip this move if passing is not allowed
                }
            } else {
                if (!tryPlayStone(nextState, move.position)) {
                    continue; // Skip this move if it's not legal
                }
                if (containsPosition(positionHistory, nextState.board)) {
                    continue;
                }
                branchHistory.push_back(nextState.board);
                foundChild = true;
            }
            int score = alphaBetaScore(nextState, depth - 1, !maximizingPlayer, aiColor, alpha, beta, deadline, timedOut, branchHistory);
            if (timedOut) {
                return evaluatePosition(state, aiColor);
            }
            if (maximizingPlayer) {
                bestScore = std::max(score, bestScore);
                alpha = std::max(alpha, score);
                if (beta <= alpha) {
                    break; // Beta cut-off
                }
            } else {
                bestScore = std::min(score, bestScore);
                beta = std::min(beta, score);
                if (beta <= alpha) {
                    break; // Alpha cut-off
                }
            }
        }
        return foundChild ? bestScore : evaluatePosition(state, aiColor);
    }
}

Move chooseMediumMove(const SessionSnapshot& state, Stone aiColor, const PositionHistory& positionHistory) {
    if (state.positionHistorySize == 0 ||
        state.positionHistorySize > positionHistory.size()) {
        throw std::invalid_argument("Invalid position history.");
    }
    PositionHistory truncatedPositionHistory(positionHistory.begin(), positionHistory.begin() + state.positionHistorySize);
    std::vector<Move> candidateMoves = generateCandidateMoves(state, aiColor, true, truncatedPositionHistory);
    bool foundChild = false;
    int bestScore = std::numeric_limits<int>::lowest();
    Move bestMove = {true, {0, 0}}; // Default to pass if no legal moves are found
    for (const Move& move : candidateMoves) {
        SessionSnapshot nextState = state;
        PositionHistory branchHistory = truncatedPositionHistory;
        if (move.isPass) {
            if (!tryPass(nextState)) {
                continue; // Skip this move if passing is not allowed
            }
        } else {
            if (!tryPlayStone(nextState, move.position)) {
                continue; // Skip this move if it's not legal
            }
            branchHistory.push_back(nextState.board);
        }
        int score = minimaxScore(nextState, 1, false, aiColor, branchHistory);
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

Move chooseHardMove(const SessionSnapshot& state, Stone aiColor, const PositionHistory& positionHistory) {
    if (state.positionHistorySize == 0 ||
        state.positionHistorySize > positionHistory.size()) {
        throw std::invalid_argument("Invalid position history.");
    }
    PositionHistory truncatedPositionHistory(positionHistory.begin(), positionHistory.begin() + state.positionHistorySize);
    auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(1000); // 1 second time limit
    bool timedOut = false;
    std::vector<Move> candidateMoves = generateCandidateMoves(state, aiColor, true, truncatedPositionHistory, &deadline, &timedOut);
    if (candidateMoves.empty()) {
        throw std::runtime_error("No legal moves available for the AI.");
    }

    Move bestMove = candidateMoves[0];
    for (int depth = 1; std::chrono::steady_clock::now() < deadline; ++depth) {
        timedOut = false;
        bool foundChild = false;
        int bestScore = std::numeric_limits<int>::lowest();
        int alpha = std::numeric_limits<int>::lowest();
        int beta = std::numeric_limits<int>::max();
        Move iterationBestMove = bestMove;

        for (const Move& move : candidateMoves) {
            SessionSnapshot nextState = state;
            PositionHistory branchHistory = truncatedPositionHistory;
            if (move.isPass) {
                if (!tryPass(nextState)) {
                    continue; // Skip this move if passing is not allowed
                }
            } else {
                if (!tryPlayStone(nextState, move.position)) {
                    continue; // Skip this move if it's not legal
                }
                branchHistory.push_back(nextState.board);
            }
            int score = alphaBetaScore(nextState, depth - 1, false, aiColor, alpha, beta, deadline, timedOut, branchHistory);
            if (timedOut || std::chrono::steady_clock::now() >= deadline) {
                timedOut = true;
                break;
            }
            if (!foundChild || score > bestScore) {
                bestScore = score;
                iterationBestMove = move;
            }
            alpha = std::max(alpha, score);
            foundChild = true;
        }

        if (timedOut || std::chrono::steady_clock::now() >= deadline) {
            break;
        }
        if (!foundChild) {
            throw std::runtime_error("No legal moves available for the AI.");
        }
        bestMove = iterationBestMove;
        auto foundMove = std::find_if(candidateMoves.begin(), candidateMoves.end(), [&bestMove](const Move& move) {
            return move.isPass == bestMove.isPass &&
                (bestMove.isPass ||
                    (move.position.col == bestMove.position.col &&
                        move.position.row == bestMove.position.row));
        });
        if (foundMove != candidateMoves.end()) {
            std::iter_swap(candidateMoves.begin(), foundMove);
        }
    }

    return bestMove;
}