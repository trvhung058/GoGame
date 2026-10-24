#pragma once

#include <vector>

#include "types.h"

bool isOnBoard(int col, int row, int size);
std::vector<Point> findGroup(const SessionSnapshot& state, Point start);
int countLiberties(const SessionSnapshot& state, const std::vector<Point>& group);
std::vector<Point> findCapturedGroup(const SessionSnapshot& state, Point adjacentEnemyStone);
bool tryPlayStone(SessionSnapshot& state, Point position);