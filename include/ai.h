#pragma once

#include "types.h"

Move chooseEasyMove(const SessionSnapshot& state, const PositionHistory& positionHistory);
int evaluatePosition(const SessionSnapshot& state, Stone aiColor);
Move chooseMediumMove(const SessionSnapshot& state, Stone aiColor, const PositionHistory& positionHistory);
Move chooseHardMove(const SessionSnapshot& state, Stone aiColor, const PositionHistory& positionHistory);