#include "game_engine.h"

#include <vector>

bool isOnBoard(int col, int row, int size) {
    return col >= 0 && col < size && row >= 0 && row < size;
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
            int newCol = current.col + dx[i];
            int newRow = current.row + dy[i];

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
            int newCol = stone.col + dx[i];
            int newRow = stone.row + dy[i];

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

bool tryPlayStone(SessionSnapshot& state, Point position) {
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
        int newCol = position.col + dx[i];
        int newRow = position.row + dy[i];

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

    temp.sideToMove = (state.sideToMove == Stone::Black) ? Stone::White : Stone::Black;
    state = temp;
    return true;
}
