#pragma once

#include "types.h"

Move chooseEasyMove(const SessionSnapshot& state);
int evaluatePosition(const SessionSnapshot& state, Stone aiColor);
Move chooseMediumMove(const SessionSnapshot& state, Stone aiColor);