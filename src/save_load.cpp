#include "save_load.h"
#include <stdexcept>
#include <cstddef>
#include <cstdint>
#include <fstream>
#include <limits>
#include <utility>

#include <nlohmann/json.hpp>

using json = nlohmann::json;

namespace {
    json stoneToJson(Stone stone) {
        switch (stone) {
            case Stone::Empty:
                return "Empty";
            case Stone::Black:
                return "Black";
            case Stone::White:
                return "White";
        }
        throw std::invalid_argument("Invalid stone value");
    }

    Stone stoneFromJson(const json& value) {
        if (!value.is_string()) {
            throw std::invalid_argument("Expected a string for stone value in JSON");
        }
        std::string stoneStr = value.get<std::string>();
        if (stoneStr == "Empty") {
            return Stone::Empty;
        } else if (stoneStr == "Black") {
            return Stone::Black;
        } else if (stoneStr == "White") {
            return Stone::White;
        } else {
            throw std::invalid_argument("Invalid stone value in JSON");
        }
    }

    json gamePhaseToJson(GamePhase phase) {
        switch (phase) {
            case GamePhase::Playing:
                return "Playing";
            case GamePhase::Scoring:
                return "Scoring";
            case GamePhase::Finished:
                return "Finished";
        }
        throw std::invalid_argument("Invalid game phase value");
    }

    GamePhase gamePhaseFromJson(const json& value) {
        if (!value.is_string()) {
            throw std::invalid_argument("Expected a string for game phase value in JSON");
        }
        std::string phaseStr = value.get<std::string>();
        if (phaseStr == "Playing") {
            return GamePhase::Playing;
        } else if (phaseStr == "Scoring") {
            return GamePhase::Scoring;
        } else if (phaseStr == "Finished") {
            return GamePhase::Finished;
        } else {
            throw std::invalid_argument("Invalid game phase value in JSON");
        }
    }

    json territoryOwnerToJson(TerritoryOwner owner) {
        switch (owner) {
            case TerritoryOwner::Unresolved:
                return "Unresolved";
            case TerritoryOwner::Neutral:
                return "Neutral";
            case TerritoryOwner::Black:
                return "Black";
            case TerritoryOwner::White:
                return "White";
        }
        throw std::invalid_argument("Invalid territory owner value");
    }

    TerritoryOwner territoryOwnerFromJson(const json& value) {
        if (!value.is_string()) {
            throw std::invalid_argument("Expected a string for territory owner value in JSON");
        }
        std::string ownerStr = value.get<std::string>();
        if (ownerStr == "Unresolved") {
            return TerritoryOwner::Unresolved;
        } else if (ownerStr == "Neutral") {
            return TerritoryOwner::Neutral;
        } else if (ownerStr == "Black") {
            return TerritoryOwner::Black;
        } else if (ownerStr == "White") {
            return TerritoryOwner::White;
        } else {
            throw std::invalid_argument("Invalid territory owner value in JSON");
        }
    }

    json gameModeToJson(GameMode mode) {
        switch (mode) {
            case GameMode::PvP:
                return "PvP";
            case GameMode::PvE:
                return "PvE";
        }
        throw std::invalid_argument("Invalid game mode value");
    }

    GameMode gameModeFromJson(const json& value) {
        if (!value.is_string()) {
            throw std::invalid_argument("Expected a string for game mode value in JSON");
        }
        std::string modeStr = value.get<std::string>();
        if (modeStr == "PvP") {
            return GameMode::PvP;
        } else if (modeStr == "PvE") {
            return GameMode::PvE;
        } else {
            throw std::invalid_argument("Invalid game mode value in JSON");
        }
    }

    json deadStoneMapToJson(const DeadStoneMap& deadStones) {
        if (deadStones.empty()) {
            throw std::invalid_argument("Dead stone map is empty");
        }

        json jMap = json::array();

        for (const auto& col : deadStones) {
            if (col.size() != deadStones.size()) {
                throw std::invalid_argument("Dead stone map is not square");
            }
            json jCol = json::array();
            for (const auto& isDead : col) {
                jCol.push_back(isDead);
            }
            jMap.push_back(jCol);
        }
        return jMap;
    }

    DeadStoneMap deadStoneMapFromJson(const json& value, std::size_t boardSize) {
        if (!value.is_array() || value.size() != boardSize) {
            throw std::invalid_argument("Invalid dimensions for dead stone map in JSON");
        }

        DeadStoneMap deadStones(boardSize, std::vector<bool>(boardSize, false));
        for (std::size_t col = 0; col < boardSize; ++col) {
            const auto& jCol = value.at(col);
            if (!jCol.is_array() || jCol.size() != boardSize) {
                throw std::invalid_argument("Invalid column size in dead stone map JSON");
            }
            for (std::size_t row = 0; row < boardSize; ++row) {
                if (!jCol.at(row).is_boolean()) {
                    throw std::invalid_argument("Invalid value in dead stone map JSON");
                }
                deadStones[col][row] = jCol.at(row).get<bool>();
            }
        }
        return deadStones;
    }

    json stoneGridToJson(const std::vector<std::vector<Stone>>& grid) {
        if (grid.empty()) {
            throw std::invalid_argument("Grid is empty");
        }

        json jGrid = json::array();

        for (const auto& col : grid) {
            if (col.size() != grid.size()) {
                throw std::invalid_argument("Grid is not square");
            }
            json jCol = json::array();
            for (const auto& stone : col) {
                jCol.push_back(stoneToJson(stone));
            }
            jGrid.push_back(jCol);
        }
        return jGrid;
    }

    std::vector<std::vector<Stone>> stoneGridFromJson(const json& value) {
        if (!value.is_array() || value.empty()) {
            throw std::invalid_argument("Expected a non-empty array for stone grid in JSON");
        }

        std::size_t boardSize = value.size();
        std::vector<std::vector<Stone>> grid(boardSize, std::vector<Stone>(boardSize, Stone::Empty));

        for (std::size_t col = 0; col < boardSize; ++col) {
            const auto& jCol = value.at(col);
            if (!jCol.is_array() || jCol.size() != boardSize) {
                throw std::invalid_argument("Invalid column size in stone grid JSON");
            }
            for (std::size_t row = 0; row < boardSize; ++row) {
                grid[col][row] = stoneFromJson(jCol.at(row));
            }
        }
        return grid;
    }

    json positionHistoryToJson(const PositionHistory& positionHistory) {
        if (positionHistory.empty()) {
            throw std::invalid_argument("Position history is empty");
        }

        json jHistory = json::array();
        for (const auto& board : positionHistory) {
            jHistory.push_back(stoneGridToJson(board));
        }
        return jHistory;
    }

    PositionHistory positionHistoryFromJson(const json& value, std::size_t boardSize) {
        if (!value.is_array() || value.empty()) {
            throw std::invalid_argument("Expected a non-empty array for position history in JSON");
        }

        PositionHistory positionHistory;
        positionHistory.reserve(value.size());
        for (const auto& jBoard : value) {
            BoardGrid board = stoneGridFromJson(jBoard);
            if (board.size() != boardSize) {
                throw std::invalid_argument("Inconsistent board size in position history JSON");
            }
            positionHistory.push_back(std::move(board));
        }
        return positionHistory;
    }

    std::size_t positionHistorySizeFromJson(const json& value) {
        std::uint64_t size = 0;
        if (value.is_number_unsigned()) {
            size = value.get<std::uint64_t>();
        } else if (value.is_number_integer()) {
            const auto signedSize = value.get<std::int64_t>();
            if (signedSize < 0) {
                throw std::invalid_argument("Invalid value for positionHistorySize in session snapshot JSON");
            }
            size = static_cast<std::uint64_t>(signedSize);
        } else {
            throw std::invalid_argument("Invalid value for positionHistorySize in session snapshot JSON");
        }
        if (size > std::numeric_limits<std::size_t>::max()) {
            throw std::invalid_argument("Invalid value for positionHistorySize in session snapshot JSON");
        }
        return static_cast<std::size_t>(size);
    }

    json territoryMapToJson(const TerritoryMap& territory) {
        if (territory.empty()) {
            throw std::invalid_argument("Territory map is empty");
        }

        json jMap = json::array();

        for (const auto& col : territory) {
            if (col.size() != territory.size()) {
                throw std::invalid_argument("Territory map is not square");
            }
            json jCol = json::array();
            for (const auto& owner : col) {
                jCol.push_back(territoryOwnerToJson(owner));
            }
            jMap.push_back(jCol);
        }
        return jMap;
    }

    TerritoryMap territoryMapFromJson(const json& value) {
        if (!value.is_array() || value.empty()) {
            throw std::invalid_argument("Expected a non-empty array for territory map in JSON");
        }

        std::size_t boardSize = value.size();
        TerritoryMap territory(boardSize, std::vector<TerritoryOwner>(boardSize, TerritoryOwner::Unresolved));

        for (std::size_t col = 0; col < boardSize; ++col) {
            const auto& jCol = value.at(col);
            if (!jCol.is_array() || jCol.size() != boardSize) {
                throw std::invalid_argument("Invalid column size in territory map JSON");
            }
            for (std::size_t row = 0; row < boardSize; ++row) {
                territory[col][row] = territoryOwnerFromJson(jCol.at(row));
            }
        }
        return territory;
    }

    json sessionSnapshotToJson(const SessionSnapshot& state) {
        if (state.board.size() != state.previousBoard.size() ||
            state.board.size() != state.territoryProposal.size() ||
            state.board.size() != state.deadStones.size()) {
            throw std::invalid_argument("Board, previousBoard, territoryProposal, and deadStones must have the same size in SessionSnapshot");
        }
        json jSnapshot = json::object();
        jSnapshot["board"] = stoneGridToJson(state.board);
        jSnapshot["previousBoard"] = stoneGridToJson(state.previousBoard);
        jSnapshot["territoryProposal"] = territoryMapToJson(state.territoryProposal);
        jSnapshot["deadStones"] = deadStoneMapToJson(state.deadStones);
        jSnapshot["sideToMove"] = stoneToJson(state.sideToMove);
        jSnapshot["capturedByBlack"] = state.capturedByBlack;
        jSnapshot["capturedByWhite"] = state.capturedByWhite;
        jSnapshot["consecutivePasses"] = state.consecutivePasses;
        jSnapshot["positionHistorySize"] = state.positionHistorySize;
        jSnapshot["phase"] = gamePhaseToJson(state.phase);
        jSnapshot["blackAcceptedScore"] = state.blackAcceptedScore;
        jSnapshot["whiteAcceptedScore"] = state.whiteAcceptedScore;
        return jSnapshot;
    }

    SessionSnapshot sessionSnapshotFromJson(const json& value, int formatVersion) {
        if (!value.is_object()) {
            throw std::invalid_argument("Expected an object for session snapshot in JSON");
        }

        if (!value.contains("board")) {
            throw std::invalid_argument("Missing 'board' field in session snapshot JSON");
        }
        if (!value.contains("previousBoard")) {
            throw std::invalid_argument("Missing 'previousBoard' field in session snapshot JSON");
        }
        if (!value.contains("territoryProposal")) {
            throw std::invalid_argument("Missing 'territoryProposal' field in session snapshot JSON");
        }
        if (formatVersion >= 3 && !value.contains("deadStones")) {
            throw std::invalid_argument("Missing 'deadStones' field in session snapshot JSON");
        }
        if (formatVersion >= 4 && !value.contains("positionHistorySize")) {
            throw std::invalid_argument("Missing 'positionHistorySize' field in session snapshot JSON");
        }
        if (!value.contains("sideToMove")) {
            throw std::invalid_argument("Missing 'sideToMove' field in session snapshot JSON");
        }
        if (!value.contains("phase")) {
            throw std::invalid_argument("Missing 'phase' field in session snapshot JSON");
        }
        if (!value.contains("capturedByBlack")) {
            throw std::invalid_argument("Missing 'capturedByBlack' field in session snapshot JSON");
        }
        if (!value.contains("capturedByWhite")) {
            throw std::invalid_argument("Missing 'capturedByWhite' field in session snapshot JSON");
        }
        if (!value.contains("consecutivePasses")) {
            throw std::invalid_argument("Missing 'consecutivePasses' field in session snapshot JSON");
        }
        if (!value.contains("blackAcceptedScore")) {
            throw std::invalid_argument("Missing 'blackAcceptedScore' field in session snapshot JSON");
        }
        if (!value.contains("whiteAcceptedScore")) {
            throw std::invalid_argument("Missing 'whiteAcceptedScore' field in session snapshot JSON");
        }

        SessionSnapshot state;
        state.board = stoneGridFromJson(value.at("board"));
        state.previousBoard = stoneGridFromJson(value.at("previousBoard"));
        state.territoryProposal = territoryMapFromJson(value.at("territoryProposal"));
        state.deadStones = formatVersion >= 3
            ? deadStoneMapFromJson(value.at("deadStones"), state.board.size())
            : DeadStoneMap(state.board.size(), std::vector<bool>(state.board.size(), false));
        state.sideToMove = stoneFromJson(value.at("sideToMove"));
        state.phase = gamePhaseFromJson(value.at("phase"));

        if (!value.at("capturedByBlack").is_number_integer()) {
            throw std::invalid_argument("Invalid value for capturedByBlack in session snapshot JSON");
        }
        state.capturedByBlack = value.at("capturedByBlack").get<int>();
        if (state.capturedByBlack < 0) {
            throw std::invalid_argument("Invalid value for capturedByBlack in session snapshot JSON");
        }
        if (!value.at("capturedByWhite").is_number_integer()) {
            throw std::invalid_argument("Invalid value for capturedByWhite in session snapshot JSON");
        }
        state.capturedByWhite = value.at("capturedByWhite").get<int>();
        if (state.capturedByWhite < 0) {
            throw std::invalid_argument("Invalid value for capturedByWhite in session snapshot JSON");
        }
        if (!value.at("consecutivePasses").is_number_integer()) {
            throw std::invalid_argument("Invalid value for consecutivePasses in session snapshot JSON");
        }
        state.consecutivePasses = value.at("consecutivePasses").get<int>();
        if (state.consecutivePasses < 0) {
            throw std::invalid_argument("Invalid value for consecutivePasses in session snapshot JSON");
        }
        state.positionHistorySize = formatVersion >= 4
            ? positionHistorySizeFromJson(value.at("positionHistorySize"))
            : 0;

        if (!value.at("blackAcceptedScore").is_boolean()) {
            throw std::invalid_argument("Invalid value for blackAcceptedScore in session snapshot JSON");
        }
        state.blackAcceptedScore = value.at("blackAcceptedScore").get<bool>();
        if (!value.at("whiteAcceptedScore").is_boolean()) {
            throw std::invalid_argument("Invalid value for whiteAcceptedScore in session snapshot JSON");
        }
        state.whiteAcceptedScore = value.at("whiteAcceptedScore").get<bool>();

        // After filling the snapshot, check that previousBoard and territoryProposal have the same board size as board
        std::size_t boardSize = state.board.size();
        if (state.previousBoard.size() != boardSize) {
            throw std::invalid_argument("previousBoard size does not match board size in session snapshot JSON");
        }
        if (state.territoryProposal.size() != boardSize) {
            throw std::invalid_argument("territoryProposal size does not match board size in session snapshot JSON");
        }

        if (state.sideToMove != Stone::Black && state.sideToMove != Stone::White) {
            throw std::invalid_argument("Invalid value for sideToMove in session snapshot JSON");
        }

        return state;
    }

    void appendReconstructedPosition(GameSession& session, SessionSnapshot& snapshot) {
        if (session.positionHistory.empty() || session.positionHistory.back() != snapshot.board) {
            session.positionHistory.push_back(snapshot.board);
        }
        snapshot.positionHistorySize = session.positionHistory.size();
    }

    void reconstructPositionHistory(GameSession& session) {
        session.positionHistory.clear();
        SessionSnapshot* firstSnapshot = !session.undoHistory.empty()
            ? &session.undoHistory.front()
            : &session.currentState;
        if (firstSnapshot->previousBoard != firstSnapshot->board) {
            session.positionHistory.push_back(firstSnapshot->previousBoard);
        }

        for (auto& snapshot : session.undoHistory) {
            appendReconstructedPosition(session, snapshot);
        }
        appendReconstructedPosition(session, session.currentState);
        for (auto it = session.redoHistory.rbegin(); it != session.redoHistory.rend(); ++it) {
            appendReconstructedPosition(session, *it);
        }
    }

    void validateSnapshotPositionHistory(const SessionSnapshot& snapshot, const PositionHistory& positionHistory) {
        if (snapshot.positionHistorySize == 0 ||
            snapshot.positionHistorySize > positionHistory.size() ||
            positionHistory[snapshot.positionHistorySize - 1] != snapshot.board) {
            throw std::invalid_argument("Inconsistent position history size in session snapshot");
        }
    }

    json gameSessionToJson(const GameSession& session) {
        if (session.currentState.phase != GamePhase::Playing && session.currentState.phase != GamePhase::Finished && session.currentState.phase != GamePhase::Scoring) {
            throw std::invalid_argument("Invalid game phase in currentState of GameSession");
        }
        validateSnapshotPositionHistory(session.currentState, session.positionHistory);
        for (const auto& board : session.positionHistory) {
            if (board.size() != session.currentState.board.size()) {
                throw std::invalid_argument("Inconsistent board size in position history");
            }
        }
        json jSession = json::object();
        jSession["formatVersion"] = 4;
        jSession["gameMode"] = gameModeToJson(session.mode);
        jSession["currentState"] = sessionSnapshotToJson(session.currentState);
        jSession["positionHistory"] = positionHistoryToJson(session.positionHistory);
        jSession["undoHistory"] = json::array();
        for (const auto& snapshot : session.undoHistory) {
            if (snapshot.board.size() != session.currentState.board.size()) {
                throw std::invalid_argument("Inconsistent board size in undoHistory entry");
            }
            validateSnapshotPositionHistory(snapshot, session.positionHistory);
            jSession["undoHistory"].push_back(sessionSnapshotToJson(snapshot));
        }
        jSession["redoHistory"] = json::array();
        for (const auto& snapshot : session.redoHistory) {
            if (snapshot.board.size() != session.currentState.board.size()) {
                throw std::invalid_argument("Inconsistent board size in redoHistory entry");
            }
            validateSnapshotPositionHistory(snapshot, session.positionHistory);
            jSession["redoHistory"].push_back(sessionSnapshotToJson(snapshot));
        }
        return jSession;
    }

    GameSession gameSessionFromJson(const json& value) {
        if (!value.is_object()) {
            throw std::invalid_argument("Expected an object for game session in JSON");
        }

        if (!value.contains("formatVersion")) {
            throw std::invalid_argument("Missing 'formatVersion' field in game session JSON");
        }
        if (!value.at("formatVersion").is_number_integer()) {
            throw std::invalid_argument("Invalid value for formatVersion in game session JSON");
        }
        int formatVersion = value.at("formatVersion").get<int>();
        if (formatVersion != 1 && formatVersion != 2 && formatVersion != 3 && formatVersion != 4) {
            throw std::invalid_argument("Unsupported format version in game session JSON");
        }

        GameMode mode = GameMode::PvP;
        if (formatVersion >= 2) {
            if (!value.contains("gameMode")) {
                throw std::invalid_argument("Missing 'gameMode' field in game session JSON");
            }
            mode = gameModeFromJson(value.at("gameMode"));
        }

        if (!value.contains("currentState")) {
            throw std::invalid_argument("Missing 'currentState' field in game session JSON");
        }
        GameSession result{};
        result.mode = mode;
        result.currentState = sessionSnapshotFromJson(value.at("currentState"), formatVersion);
        if (result.currentState.phase != GamePhase::Playing &&
            result.currentState.phase != GamePhase::Scoring &&
            result.currentState.phase != GamePhase::Finished) {
            throw std::invalid_argument("Invalid game phase in currentState of GameSession");
        }

        if (!value.contains("undoHistory")) {
            throw std::invalid_argument("Missing 'undoHistory' field in game session JSON");
        }
        if (!value.at("undoHistory").is_array()) {
            throw std::invalid_argument("Invalid value for undoHistory in game session JSON");
        }
        for (const auto& jSnapshot : value.at("undoHistory")) {
            auto snapshot = sessionSnapshotFromJson(jSnapshot, formatVersion);
            if (snapshot.board.size() != result.currentState.board.size()) {
                throw std::invalid_argument("Inconsistent board size in undoHistory entry");
            }
            result.undoHistory.push_back(snapshot);
        }

        if (!value.contains("redoHistory")) {
            throw std::invalid_argument("Missing 'redoHistory' field in game session JSON");
        }
        if (!value.at("redoHistory").is_array()) {
            throw std::invalid_argument("Invalid value for redoHistory in game session JSON");
        }
        for (const auto& jSnapshot : value.at("redoHistory")) {
            auto snapshot = sessionSnapshotFromJson(jSnapshot, formatVersion);
            if (snapshot.board.size() != result.currentState.board.size()) {
                throw std::invalid_argument("Inconsistent board size in redoHistory entry");
            }
            result.redoHistory.push_back(snapshot);
        }

        if (formatVersion >= 4) {
            if (!value.contains("positionHistory")) {
                throw std::invalid_argument("Missing 'positionHistory' field in game session JSON");
            }
            result.positionHistory = positionHistoryFromJson(
                value.at("positionHistory"),
                result.currentState.board.size());
            validateSnapshotPositionHistory(result.currentState, result.positionHistory);
            for (const auto& snapshot : result.undoHistory) {
                validateSnapshotPositionHistory(snapshot, result.positionHistory);
            }
            for (const auto& snapshot : result.redoHistory) {
                validateSnapshotPositionHistory(snapshot, result.positionHistory);
            }
        } else {
            reconstructPositionHistory(result);
        }

        return result;
    }
}

bool saveGame(const std::string& filePath, const GameSession& session) {
    try {
        const json document = gameSessionToJson(session);
        std::ofstream file(filePath);
        if (!file.is_open()) {
            return false;
        }

        file << document.dump(4);
        file.flush();
        return file.good();
    } catch (const std::exception&) {
        return false;
    }
}

bool loadGame(const std::string& filePath, GameSession& session) {
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            return false;
        }
        json document = json::object();
        file >> document;
        GameSession loaded = gameSessionFromJson(document);
        session = std::move(loaded);
        return true;
    } catch (const std::exception&) {
        return false;
    }
}
