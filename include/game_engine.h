#pragma once

#include <vector>
#include <cstddef>
#include "types.h"

bool isOnBoard(int col, int row, int size);
std::vector<Point> findGroup(const SessionSnapshot& state, Point start);
int countLiberties(const SessionSnapshot& state, const std::vector<Point>& group);
std::vector<Point> findCapturedGroup(const SessionSnapshot& state, Point adjacentEnemyStone);
bool tryPlayStone(SessionSnapshot& state, Point position);
void createInitialState(SessionSnapshot& state, int boardSize);
bool tryPass(SessionSnapshot& state);
Score calculateAreaScore(const SessionSnapshot& state, double komi);
TerritoryMap generateTerritoryProposal(const SessionSnapshot& state);
bool reviewScore(SessionSnapshot& state, Stone player, bool accepts);
bool tryPlayMove(GameSession& session, const Move& move);
bool undoMoves(GameSession& session, std::size_t count);
bool redoMoves(GameSession& session, std::size_t count);