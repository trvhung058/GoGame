#pragma once

#include <string>

#include "types.h"

bool saveGame(const std::string& filePath, const GameSession& session);
bool loadGame(const std::string& filePath, GameSession& session);