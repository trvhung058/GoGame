#include "game_engine.h"

#include <vector>
#include <stdexcept>
#include <utility>

void createInitialState(SessionSnapshot& state, int boardSize) {
    if (boardSize <= 0) {
        throw std::invalid_argument("Board size must be positive.");
    }
    state.board = std::vector<std::vector<Stone>>(boardSize, std::vector<Stone>(boardSize, Stone::Empty));
    state.previousBoard = std::vector<std::vector<Stone>>(boardSize, std::vector<Stone>(boardSize, Stone::Empty));
    state.sideToMove = Stone::Black;
    state.capturedByBlack = 0;
    state.capturedByWhite = 0;
    state.consecutivePasses = 0;
    state.phase = GamePhase::Playing;
    state.blackAcceptedScore = false;
    state.whiteAcceptedScore = false;
    state.territoryProposal = std::vector<std::vector<TerritoryOwner>>(boardSize, std::vector<TerritoryOwner>(boardSize, TerritoryOwner::Unresolved));
    state.deadStones = std::vector<std::vector<bool>>(boardSize, std::vector<bool>(boardSize, false));
}

void initializePositionHistory(GameSession& session) {
    session.positionHistory.clear();
    session.positionHistory.push_back(session.currentState.board);
    session.currentState.positionHistorySize = 1;
}

bool isOnBoard(std::size_t col, std::size_t row, std::size_t size) {
    return col < size && row < size;
}

std::vector<Point> findGroup(const SessionSnapshot& state, Point start) {
    if (!isOnBoard(start.col, start.row, state.board.size())) return {};
    Stone targetStone = state.board[start.col][start.row];
    if (targetStone == Stone::Empty) return {};

    std::vector<std::vector<bool>> visited(state.board.size(), std::vector<bool>(state.board.size(), false));
    std::vector<Point> stack = { start };
    std::vector<Point> group = { start };

    const int dx[] = { -1, 1, 0, 0 };
    const int dy[] = { 0, 0, -1, 1 };

    visited[start.col][start.row] = true;

    while (!stack.empty()) {
        Point current = stack.back();
        stack.pop_back();

        for (int i = 0; i < 4; ++i) {
            size_t newCol = current.col + dx[i];
            size_t newRow = current.row + dy[i];

            if (isOnBoard(newCol, newRow, state.board.size()) && !visited[newCol][newRow] && state.board[newCol][newRow] == targetStone) {
                stack.push_back({ newCol, newRow });
                group.push_back({ newCol, newRow });
                visited[newCol][newRow] = true;
            }
        }
    }

    return group;
}

int countLiberties(const SessionSnapshot& state, const std::vector<Point>& group) {
    if (group.empty()) return 0;
    std::vector<std::vector<bool>> visited(state.board.size(), std::vector<bool>(state.board.size(), false));
    int liberties = 0;

    const int dx[] = { -1, 1, 0, 0 };
    const int dy[] = { 0, 0, -1, 1 };

    for (const Point& stone : group) {
        for (int i = 0; i < 4; ++i) {
            size_t newCol = stone.col + dx[i];
            size_t newRow = stone.row + dy[i];

            if (isOnBoard(newCol, newRow, state.board.size()) && !visited[newCol][newRow] && state.board[newCol][newRow] == Stone::Empty) {
                liberties++;
                visited[newCol][newRow] = true;
            }
        }
    }

    return liberties;
}

std::vector<Point> findCapturedGroup(const SessionSnapshot& state, Point adjacentEnemyStone) {
    if (!isOnBoard(adjacentEnemyStone.col, adjacentEnemyStone.row, state.board.size())) return {};
    Stone targetStone = state.board[adjacentEnemyStone.col][adjacentEnemyStone.row];
    if (targetStone == Stone::Empty) return {};

    std::vector<Point> group = findGroup(state, adjacentEnemyStone);
    int liberties = countLiberties(state, group);

    if (liberties == 0) {
        return group;
    } else {
        return {};
    }
}

static bool boardsEqual(const std::vector<std::vector<Stone>>& a, const std::vector<std::vector<Stone>>& b) {
    if (a.size() != b.size()) {
        return false;
    }

    for (size_t row = 0; row < a.size(); ++row) {
        if (a[row].size() != b[row].size()) {
            return false;
        }

        for (size_t col = 0; col < a[row].size(); ++col) {
            if (a[row][col] != b[row][col]) {
                return false;
            }
        }
    }

    return true;
}

bool tryPlayStone(SessionSnapshot& state, Point position) {
    if (state.phase != GamePhase::Playing) {
        return false;
    }

    if (!isOnBoard(position.col, position.row, state.board.size()) ||
        state.board[position.col][position.row] != Stone::Empty) {
        return false;
    }

    SessionSnapshot temp = state;
    temp.board[position.col][position.row] = state.sideToMove;

    const int dx[] = { -1, 1, 0, 0 };
    const int dy[] = { 0, 0, -1, 1 };

    int capturedCount = 0;

    for (int i = 0; i < 4; ++i) {
        size_t newCol = position.col + dx[i];
        size_t newRow = position.row + dy[i];

        if (!isOnBoard(newCol, newRow, temp.board.size())) {
            continue;
        }

        Stone adjacentStone = temp.board[newCol][newRow];
        if (adjacentStone == Stone::Empty || adjacentStone == state.sideToMove) {
            continue;
        }

        std::vector<Point> enemyGroup = findGroup(temp, { newCol, newRow });
        if (enemyGroup.empty()) {
            continue;
        }

        if (countLiberties(temp, enemyGroup) == 0) {
            for (const Point& stone : enemyGroup) {
                temp.board[stone.col][stone.row] = Stone::Empty;
            }
            capturedCount += static_cast<int>(enemyGroup.size());
        }
    }

    if (state.sideToMove == Stone::Black) {
        temp.capturedByBlack += capturedCount;
    } else if (state.sideToMove == Stone::White) {
        temp.capturedByWhite += capturedCount;
    }

    std::vector<Point> placedGroup = findGroup(temp, position);
    if (placedGroup.empty() || countLiberties(temp, placedGroup) == 0) {
        return false;
    }

    if (!state.previousBoard.empty() && boardsEqual(temp.board, state.previousBoard)) {
        return false;
    }

    temp.previousBoard = state.board;
    temp.sideToMove = (state.sideToMove == Stone::Black) ? Stone::White : Stone::Black;
    temp.consecutivePasses = 0;
    temp.phase = GamePhase::Playing;
    state = temp;
    return true;
}

bool tryPass(SessionSnapshot& state) {
    if (state.phase != GamePhase::Playing) {
        return false;
    }

    SessionSnapshot temp = state;
    temp.consecutivePasses++;
    temp.sideToMove = (state.sideToMove == Stone::Black) ? Stone::White : Stone::Black;
    temp.previousBoard = state.board;

    if (temp.consecutivePasses >= 2) {
        temp.phase = GamePhase::Scoring;
        temp.blackAcceptedScore = false;
        temp.whiteAcceptedScore = false;
        temp.territoryProposal = generateTerritoryProposal(temp);
    }

    state = temp;
    return true;
}

bool tryPlayMove(GameSession& session, const Move& move) {
    if (session.currentState.positionHistorySize == 0 ||
        session.currentState.positionHistorySize > session.positionHistory.size()) {
        return false;
    }
    SessionSnapshot nextState = session.currentState;
    if (move.isPass) {
        if (!tryPass(nextState)) {
            return false;
        }
    } else {
        if (!tryPlayStone(nextState, move.position)) {
            return false;
        }
        for (std::size_t i = 0; i < session.currentState.positionHistorySize; ++i) {
            if (nextState.board == session.positionHistory[i]) {
                return false;
            }
        }
    }

    session.positionHistory.resize(session.currentState.positionHistorySize);
    if (!move.isPass) {
        session.positionHistory.push_back(nextState.board);
        nextState.positionHistorySize = session.positionHistory.size();
    }

    session.undoHistory.push_back(session.currentState);
    session.redoHistory.clear();
    session.currentState = std::move(nextState);
    return true;
}

Score calculateAreaScore(const SessionSnapshot& state, double komi) {
    const TerritoryMap& territoryOwner = state.territoryProposal;
    if (state.phase == GamePhase::Playing) {
        throw std::invalid_argument("Cannot calculate score while the game is in the Playing phase.");
    }
    if (territoryOwner.size() != state.board.size()) {
        throw std::invalid_argument("Territory map dimensions must match the board.");
    }
    if (state.deadStones.size() != state.board.size()) {
        throw std::invalid_argument("Dead stone map dimensions must match the board.");
    }

    for (size_t col = 0; col < state.board.size(); ++col) {
        if (territoryOwner[col].size() != state.board[col].size()) {
            throw std::invalid_argument("Territory map dimensions must match the board.");
        }
        if (state.deadStones[col].size() != state.board[col].size()) {
            throw std::invalid_argument("Dead stone map dimensions must match the board.");
        }
    }

    Score score{ 0.0, komi };

    for (size_t col = 0; col < state.board.size(); ++col) {
        for (size_t row = 0; row < state.board[col].size(); ++row) {
            Stone stone = state.board[col][row];
            if (stone == Stone::Black && !state.deadStones[col][row]) {
                score.black += 1.0;
            } else if (stone == Stone::White && !state.deadStones[col][row]) {
                score.white += 1.0;
            } else {
                TerritoryOwner owner = territoryOwner[col][row];
                if (owner == TerritoryOwner::Black) {
                    score.black += 1.0;
                } else if (owner == TerritoryOwner::White) {
                    score.white += 1.0;
                } else if (owner == TerritoryOwner::Unresolved) {
                    throw std::invalid_argument("Territory map contains unresolved points.");
                }
            }
        }
    }

    return score;
}

TerritoryMap generateTerritoryProposal(const SessionSnapshot& state) {
    if (state.board.size() != state.deadStones.size()) {
        throw std::invalid_argument("Board and deadStones must have the same number of columns.");
    }
    for (size_t col = 0; col < state.board.size(); ++col) {
        if (state.board[col].size() != state.deadStones[col].size()) {
            throw std::invalid_argument("Board and deadStones must have the same number of rows in each column.");
        }
    }
    TerritoryMap proposal;
    proposal.reserve(state.board.size());
    for (const auto& column : state.board) {
        proposal.emplace_back(column.size(), TerritoryOwner::Unresolved);
    }

    std::vector<std::vector<bool>> visited;
    visited.reserve(state.board.size());
    for (const auto& column : state.board) {
        visited.emplace_back(column.size(), false);
    }

    const int dx[] = { -1, 1, 0, 0 };
    const int dy[] = { 0, 0, -1, 1 };

    for (size_t col = 0; col < state.board.size(); ++col) {
        for (size_t row = 0; row < state.board[col].size(); ++row) {
            if ((state.board[col][row] != Stone::Empty && !state.deadStones[col][row]) || visited[col][row]) {
                continue;
            }

            bool touchesBlack = false;
            bool touchesWhite = false;
            std::vector<Point> region;
            std::vector<Point> stack = { { static_cast<size_t>(col), static_cast<size_t>(row) } };
            visited[col][row] = true;

            while (!stack.empty()) {
                Point current = stack.back();
                stack.pop_back();
                region.push_back(current);

                for (int i = 0; i < 4; ++i) {
                    size_t nextCol = current.col + dx[i];
                    size_t nextRow = current.row + dy[i];
                    if (nextCol >= state.board.size() ||
                        nextRow >= state.board[nextCol].size()) {
                        continue;
                    }

                    Stone neighbor = state.board[nextCol][nextRow];
                    if (neighbor == Stone::Black && !state.deadStones[nextCol][nextRow]) {
                        touchesBlack = true;
                    } else if (neighbor == Stone::White && !state.deadStones[nextCol][nextRow]) {
                        touchesWhite = true;
                    } else if (!visited[nextCol][nextRow]) {
                        visited[nextCol][nextRow] = true;
                        stack.push_back({ static_cast<size_t>(nextCol), static_cast<size_t>(nextRow) });
                    }
                }
            }

            TerritoryOwner owner = TerritoryOwner::Unresolved;
            if (touchesBlack && touchesWhite) {
                owner = TerritoryOwner::Neutral;
            } else if (touchesBlack) {
                owner = TerritoryOwner::Black;
            } else if (touchesWhite) {
                owner = TerritoryOwner::White;
            } else {
                owner = TerritoryOwner::Neutral;
            }

            for (const Point& point : region) {
                proposal[point.col][point.row] = owner;
            }
        }
    }

    return proposal;
}

TerritoryMap estimateTerritoryForWarning(const SessionSnapshot& state) {
    TerritoryMap estimate;
    estimate.reserve(state.board.size());
    for (const auto& column : state.board) {
        estimate.emplace_back(column.size(), TerritoryOwner::Unresolved);
    }

    std::vector<std::vector<bool>> visited;
    visited.reserve(state.board.size());
    for (const auto& column : state.board) {
        visited.emplace_back(column.size(), false);
    }

    for (size_t col = 0; col < state.board.size(); ++col) {
        for (size_t row = 0; row < state.board[col].size(); ++row) {
            if (state.board[col][row] != Stone::Empty || visited[col][row]) {
                continue;
            }

            bool touchesBlack = false;
            bool touchesWhite = false;
            bool touchesEdge = false;
            std::vector<Point> region;
            std::vector<Point> stack = { { col, row } };
            visited[col][row] = true;

            while (!stack.empty()) {
                Point current = stack.back();
                stack.pop_back();
                region.push_back(current);

                auto visitNeighbor = [&](size_t nextCol, size_t nextRow) {
                    if (nextCol >= state.board.size() ||
                        nextRow >= state.board[nextCol].size()) {
                        touchesEdge = true;
                        return;
                    }

                    Stone neighbor = state.board[nextCol][nextRow];
                    if (neighbor == Stone::Black) {
                        touchesBlack = true;
                    } else if (neighbor == Stone::White) {
                        touchesWhite = true;
                    } else if (!visited[nextCol][nextRow]) {
                        visited[nextCol][nextRow] = true;
                        stack.push_back({ nextCol, nextRow });
                    }
                };

                if (current.col == 0) {
                    touchesEdge = true;
                } else {
                    visitNeighbor(current.col - 1, current.row);
                }

                if (current.col + 1 >= state.board.size()) {
                    touchesEdge = true;
                } else {
                    visitNeighbor(current.col + 1, current.row);
                }

                if (current.row == 0) {
                    touchesEdge = true;
                } else {
                    visitNeighbor(current.col, current.row - 1);
                }

                if (current.row + 1 >= state.board[current.col].size()) {
                    touchesEdge = true;
                } else {
                    visitNeighbor(current.col, current.row + 1);
                }
            }

            TerritoryOwner owner = TerritoryOwner::Unresolved;
            if (touchesBlack && touchesWhite) {
                owner = TerritoryOwner::Neutral;
            } else if (!touchesEdge && touchesBlack) {
                owner = TerritoryOwner::Black;
            } else if (!touchesEdge && touchesWhite) {
                owner = TerritoryOwner::White;
            }

            for (const Point& point : region) {
                estimate[point.col][point.row] = owner;
            }
        }
    }

    return estimate;
}

bool shouldWarnAboutPointlessMove(const GameSession& session, Point position) {
    if (session.currentState.phase != GamePhase::Playing) {
        return false;
    }

    if (!isOnBoard(position.col, position.row, session.currentState.board.size()) ||
        session.currentState.board[position.col][position.row] != Stone::Empty) {
        return false;
    }

    GameSession tempSession = session;
    if (!tryPlayMove(tempSession, { false, position })) {
        return false;
    }

    if (tempSession.currentState.capturedByBlack != session.currentState.capturedByBlack ||
        tempSession.currentState.capturedByWhite != session.currentState.capturedByWhite) {
        return false;
    }

    TerritoryMap originalEstimate = estimateTerritoryForWarning(session.currentState);
    if (originalEstimate[position.col][position.row] == TerritoryOwner::Neutral || originalEstimate[position.col][position.row] == TerritoryOwner::Unresolved) {
        return false;
    }
    if (originalEstimate[position.col][position.row] == TerritoryOwner::Black && session.currentState.sideToMove == Stone::Black ||
        originalEstimate[position.col][position.row] == TerritoryOwner::White && session.currentState.sideToMove == Stone::White) {
            const int dx[] = { -1, 1, 0, 0 };
            const int dy[] = { 0, 0, -1, 1 };
            for (int i = 0; i < 4; ++i) {
                size_t neighborCol = position.col + dx[i];
                size_t neighborRow = position.row + dy[i];
                if (isOnBoard(neighborCol, neighborRow, session.currentState.board.size())) {
                    Stone neighborStone = session.currentState.board[neighborCol][neighborRow];
                    if (neighborStone != session.currentState.sideToMove) {
                        continue;
                    }
                    std::vector<Point> neighborGroup = findGroup(session.currentState, { neighborCol, neighborRow });
                    int prevLiberties = countLiberties(session.currentState, neighborGroup);
                    if (prevLiberties != 1) {
                        continue;
                    }
                    std::vector<Point> newGroup = findGroup(tempSession.currentState, { neighborCol, neighborRow });
                    int newLiberties = countLiberties(tempSession.currentState, newGroup);
                    if (newLiberties > 1) {
                        return false;
                    }
                }
            }
            return true;
    }

    return false;
}

bool reviewScore(SessionSnapshot& state, Stone player, bool accepts) {
    if (state.phase != GamePhase::Scoring) {
        return false;
    }
    if ((player == Stone::Black || player == Stone::White) && !accepts) {
        state.phase = GamePhase::Playing;
        state.consecutivePasses = 0;
        state.blackAcceptedScore = false;
        state.whiteAcceptedScore = false;
        state.territoryProposal = std::vector<std::vector<TerritoryOwner>>(state.board.size(), std::vector<TerritoryOwner>(state.board.size(), TerritoryOwner::Unresolved));
        state.deadStones = std::vector<std::vector<bool>>(state.board.size(), std::vector<bool>(state.board.size(), false));
        return true;
    }

    if (player == Stone::Black) {
        state.blackAcceptedScore = accepts;
    } else if (player == Stone::White) {
        state.whiteAcceptedScore = accepts;
    } else {
        return false;
    }

    if (state.blackAcceptedScore && state.whiteAcceptedScore) {
        state.phase = GamePhase::Finished;
    }

    return true;
}

// Count = 1 for PvP, Count = 2 for PvE
bool undoMoves(GameSession& session, std::size_t count) {
    if (count == 0 || count > session.undoHistory.size()) {
        return false;
    }

    for (std::size_t i = 0; i < count; ++i) {
        session.redoHistory.push_back(session.currentState);
        session.currentState = std::move(session.undoHistory.back());
        session.undoHistory.pop_back();
    }

    return true;
}

bool redoMoves(GameSession& session, std::size_t count) {
    if (count == 0 || count > session.redoHistory.size()) {
        return false;
    }

    for (std::size_t i = 0; i < count; ++i) {
        session.undoHistory.push_back(session.currentState);
        session.currentState = std::move(session.redoHistory.back());
        session.redoHistory.pop_back();
    }

    return true;
}

bool toggleDeadGroups(SessionSnapshot& state, Point position) {
    if (state.phase != GamePhase::Scoring) {
        return false;
    }

    if (!isOnBoard(position.col, position.row, state.board.size())) {
        return false;
    }
    if (state.board[position.col][position.row] == Stone::Empty) {
        return false;
    }
    if (state.deadStones.size() != state.board.size()) {
        return false;
    }
    for (size_t c = 0; c < state.deadStones.size(); ++c) {
        if (state.deadStones[c].size() != state.board[c].size()) {
            return false;
        }
    }

    std::vector<Point> group = findGroup(state, position);
    if (group.empty()) {
        return false;
    }

    const bool markDead = !state.deadStones[position.col][position.row];
    bool proposalChanged = false;
    for (const Point& stone : group) {
        if (state.deadStones[stone.col][stone.row] != markDead) {
            state.deadStones[stone.col][stone.row] = markDead;
            proposalChanged = true;
        }
    }
    if (proposalChanged) {
        state.blackAcceptedScore = false;
        state.whiteAcceptedScore = false;
    }

    state.territoryProposal = generateTerritoryProposal(state);
    return true;
}