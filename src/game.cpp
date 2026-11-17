#include "game.h"
#include <cmath>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>
#include "save_load.h"
#include "ai.h"

Game::Game(){
    createInitialState(session.currentState, 19);
    initializePositionHistory(session);

    board.LoadAssetsDefault("assets/board/Board_800.png", "assets/b_40.png", "assets/w_40.png");
    background.LoadAssets("assets/go-jeu_blur.jpg");
    background.SetPosition(0, 0);

    int screenWidth = GetScreenWidth();

    logo.LoadAssets("assets/UI/icon258.png");
    std::pair<int, int> logoSize = logo.GetSize();
    logo.SetPosition((screenWidth - logoSize.first) / 2, 70);

    const char *btnUnhovered300 = "assets/button/button_unhovered_300.png";
    const char *btnHovered300 = "assets/button/button_hovered_300.png";
    const char *btnUnhovered200 = "assets/button/button_unhovered_200.png";
    const char *btnHovered200 = "assets/button/button_hovered_200.png";
    const char *fontPath = "assets/Font/NotoSerif-Bold.ttf";

    titleFont = LoadFontEx(fontPath, 128, 0, 0);
    SetTextureFilter(titleFont.texture, TEXTURE_FILTER_BILINEAR);

    textFont = LoadFontEx(fontPath, 64, 0, 0);
    SetTextureFilter(textFont.texture, TEXTURE_FILTER_BILINEAR);

    // ----------------- Menu Buttons (300px) -----------------
    ContinueButton.LoadAssets(btnUnhovered300, btnHovered300);
    ContinueButton.LoadFonts(fontPath);
    ContinueButton.SetText("Continue");

    NewGameButton.LoadAssets(btnUnhovered300, btnHovered300);
    NewGameButton.LoadFonts(fontPath);
    NewGameButton.SetText("New Game");

    SkinsButton.LoadAssets(btnUnhovered300, btnHovered300);
    SkinsButton.LoadFonts(fontPath);
    SkinsButton.SetText("Skins");

    ExitButton.LoadAssets(btnUnhovered300, btnHovered300);
    ExitButton.LoadFonts(fontPath);
    ExitButton.SetText("Exit");

    std::pair<float, float> texSize = NewGameButton.GetSize();
    float startX = (screenWidth - texSize.first) / 2.0f;
    float startY = 360.0f;
    float spacing = texSize.second + 15.0f;

    ContinueButton.SetPosition(startX, startY);
    NewGameButton.SetPosition(startX, startY + spacing);
    SkinsButton.SetPosition(startX, startY + spacing * 2.0f);
    ExitButton.SetPosition(startX, startY + spacing * 3.0f);

    // ----------------- Size Selection Buttons -----------------
    Size9Button.LoadAssets(btnUnhovered300, btnHovered300);
    Size9Button.LoadFonts(fontPath);
    Size9Button.SetText("9x9");

    Size13Button.LoadAssets(btnUnhovered300, btnHovered300);
    Size13Button.LoadFonts(fontPath);
    Size13Button.SetText("13x13");

    Size19Button.LoadAssets(btnUnhovered300, btnHovered300);
    Size19Button.LoadFonts(fontPath);
    Size19Button.SetText("19x19");

    BackFromSizeButton.LoadAssets(btnUnhovered300, btnHovered300);
    BackFromSizeButton.LoadFonts(fontPath);
    BackFromSizeButton.SetText("Back");

    Size9Button.SetPosition(startX, startY);
    Size13Button.SetPosition(startX, startY + spacing);
    Size19Button.SetPosition(startX, startY + spacing * 2.0f);
    BackFromSizeButton.SetPosition(startX, startY + spacing * 3.0f);

    // ----------------- Mode Selection Buttons -----------------
    PvPButton.LoadAssets(btnUnhovered300, btnHovered300);
    PvPButton.LoadFonts(fontPath);
    PvPButton.SetText("Player vs Player");

    PvEButton.LoadAssets(btnUnhovered300, btnHovered300);
    PvEButton.LoadFonts(fontPath);
    PvEButton.SetText("Player vs AI");

    BackFromModeButton.LoadAssets(btnUnhovered300, btnHovered300);
    BackFromModeButton.LoadFonts(fontPath);
    BackFromModeButton.SetText("Back");

    float modeStartY = 400.0f;
    float modeSpacing = texSize.second + 20.0f;
    PvPButton.SetPosition(startX, modeStartY);
    PvEButton.SetPosition(startX, modeStartY + modeSpacing);
    BackFromModeButton.SetPosition(startX, modeStartY + modeSpacing * 2.0f);

    // ----------------- AI Difficulty Buttons -----------------
    EasyButton.LoadAssets(btnUnhovered300, btnHovered300);
    EasyButton.LoadFonts(fontPath);
    EasyButton.SetText("Easy");

    MediumButton.LoadAssets(btnUnhovered300, btnHovered300);
    MediumButton.LoadFonts(fontPath);
    MediumButton.SetText("Medium");

    HardButton.LoadAssets(btnUnhovered300, btnHovered300);
    HardButton.LoadFonts(fontPath);
    HardButton.SetText("Hard");

    BackFromDiffButton.LoadAssets(btnUnhovered300, btnHovered300);
    BackFromDiffButton.LoadFonts(fontPath);
    BackFromDiffButton.SetText("Back");

    EasyButton.SetPosition(startX, startY);
    MediumButton.SetPosition(startX, startY + spacing);
    HardButton.SetPosition(startX, startY + spacing * 2.0f);
    BackFromDiffButton.SetPosition(startX, startY + spacing * 3.0f);

    float col1X = 980.0f;
    float col2X = 1220.0f;
    float row1Y = 560.0f;
    float row2Y = 670.0f;
    float row3Y = 780.0f;

    PassButton.LoadAssets(btnUnhovered200, btnHovered200);
    PassButton.LoadFonts(fontPath);
    PassButton.SetText("Pass");
    PassButton.SetPosition(col1X, row1Y);

    ResignButton.LoadAssets(btnUnhovered200, btnHovered200);
    ResignButton.LoadFonts(fontPath);
    ResignButton.SetText("Resign");
    ResignButton.SetPosition(col2X, row1Y);

    UndoButton.LoadAssets(btnUnhovered200, btnHovered200);
    UndoButton.LoadFonts(fontPath);
    UndoButton.SetText("Undo");
    UndoButton.SetPosition(col1X, row2Y);

    RedoButton.LoadAssets(btnUnhovered200, btnHovered200);
    RedoButton.LoadFonts(fontPath);
    RedoButton.SetText("Redo");
    RedoButton.SetPosition(col2X, row2Y);

    SaveButton.LoadAssets(btnUnhovered200, btnHovered200);
    SaveButton.LoadFonts(fontPath);
    SaveButton.SetText("Save");
    SaveButton.SetPosition(col1X, row3Y);

    MainMenuButton.LoadAssets(btnUnhovered200, btnHovered200);
    MainMenuButton.LoadFonts(fontPath);
    MainMenuButton.SetText("Menu");
    MainMenuButton.SetPosition(col2X, row3Y);

    currentScreen = MENU;
}

Game::~Game() {
    if (titleFont.texture.id != 0) {
        UnloadFont(titleFont);
    }
    if (textFont.texture.id != 0) {
        UnloadFont(textFont);
    }
}

void Game::DrawPageTitle(const char* title) {
    if (title == nullptr || title[0] == '\0') return;

    float fontSize = 96.0f; 
    float spacing = 3.0f;
    Font currentFont = (titleFont.texture.id != 0) ? titleFont : GetFontDefault();
    Vector2 textSize = MeasureTextEx(currentFont, title, fontSize, spacing);
    float titleX = std::round((GetScreenWidth() - textSize.x) / 2.0f);
    float titleY = 120.0f; 

    DrawTextEx(currentFont, title, {titleX + 3.0f, titleY + 3.0f}, fontSize, spacing, Color{0, 0, 0, 200});
    DrawTextEx(currentFont, title, {titleX, titleY}, fontSize, spacing, WHITE);
}

// ======================= MENU =======================
void Game::UpdateMenu() {
    Vector2 mousePos = GetMousePosition();
    if (ContinueButton.IsClicked(mousePos)) {
        currentScreen = (session.mode == GameMode::PvE) ? PVE : PVP;
    }
    else if (NewGameButton.IsClicked(mousePos)) {
        currentScreen = SIZE;
    }
    else if (SkinsButton.IsClicked(mousePos)) {
        currentScreen = SKIN;
    }
    else if (ExitButton.IsClicked(mousePos)) {
        currentScreen = EXIT;
    }
}

void Game::DrawMenu() {
    background.Draw(true);
    logo.Draw(false);
    ContinueButton.Draw();
    NewGameButton.Draw();
    SkinsButton.Draw();
    ExitButton.Draw();
}

// ======================= SELECT SIZE =======================
void Game::UpdateSelectSize() {
    Vector2 mousePos = GetMousePosition();

    if (Size9Button.IsClicked(mousePos)) {
        selectedBoardSize = 9;
        currentScreen = MODE;
    }
    else if (Size13Button.IsClicked(mousePos)) {
        selectedBoardSize = 13;
        currentScreen = MODE;
    }
    else if (Size19Button.IsClicked(mousePos)) {
        selectedBoardSize = 19;
        currentScreen = MODE;
    }
    else if (BackFromSizeButton.IsClicked(mousePos)) {
        currentScreen = MENU;
    }
}

void Game::DrawSelectSize() {
    background.Draw(true);
    DrawPageTitle("Board Size");
    Size9Button.Draw();
    Size13Button.Draw();
    Size19Button.Draw();
    BackFromSizeButton.Draw();
}

// ======================= SELECT MODE =======================
void Game::UpdateSelectMode() {
    Vector2 mousePos = GetMousePosition();

    if (PvPButton.IsClicked(mousePos)) {
        createInitialState(session.currentState, selectedBoardSize);
        initializePositionHistory(session);
        session.undoHistory.clear();
        session.redoHistory.clear();
        session.mode = GameMode::PvP;
        currentScreen = PVP;
    }
    else if (PvEButton.IsClicked(mousePos)) {
        currentScreen = DIFFICULTY;
    }
    else if (BackFromModeButton.IsClicked(mousePos)) {
        currentScreen = SIZE;
    }
}

void Game::DrawSelectMode() {
    background.Draw(true);
    DrawPageTitle("Game Mode");
    PvPButton.Draw();
    PvEButton.Draw();
    BackFromModeButton.Draw();
}

// ======================= SELECT DIFFICULTY =======================
void Game::UpdateSelectDifficulty() {
    Vector2 mousePos = GetMousePosition();

    if (EasyButton.IsClicked(mousePos)) {
        aiDifficulty = AIDifficulty::Easy;
        createInitialState(session.currentState, selectedBoardSize);
        initializePositionHistory(session);
        session.undoHistory.clear();
        session.redoHistory.clear();
        session.mode = GameMode::PvE;
        aiThinkTimer = 0.0f;
        currentScreen = PVE;
    }
    else if (MediumButton.IsClicked(mousePos)) {
        aiDifficulty = AIDifficulty::Medium;
        createInitialState(session.currentState, selectedBoardSize);
        initializePositionHistory(session);
        session.undoHistory.clear();
        session.redoHistory.clear();
        session.mode = GameMode::PvE;
        aiThinkTimer = 0.0f;
        currentScreen = PVE;
    }
    else if (HardButton.IsClicked(mousePos)) {
        aiDifficulty = AIDifficulty::Hard;
        createInitialState(session.currentState, selectedBoardSize);
        initializePositionHistory(session);
        session.undoHistory.clear();
        session.redoHistory.clear();
        session.mode = GameMode::PvE;
        aiThinkTimer = 0.0f;
        currentScreen = PVE;
    }
    else if (BackFromDiffButton.IsClicked(mousePos)) {
        currentScreen = MODE;
    }
}

void Game::DrawSelectDifficulty() {
    background.Draw(true);
    DrawPageTitle("AI Difficulty");
    EasyButton.Draw();
    MediumButton.Draw();
    HardButton.Draw();
    BackFromDiffButton.Draw();
}

// ======================= AI TURN HANDLER =======================
void Game::HandleAITurn() {
    if (session.currentState.phase != GamePhase::Playing) return;
    if (session.currentState.sideToMove != Stone::White) return;

    aiThinkTimer += GetFrameTime();
    if (aiThinkTimer < 0.4f) {
        return;
    }

    aiThinkTimer = 0.0f;
    Move aiMove;

    try {
        if (aiDifficulty == AIDifficulty::Easy) {
            aiMove = chooseEasyMove(session.currentState, session.positionHistory);
        } else if (aiDifficulty == AIDifficulty::Medium) {
            aiMove = chooseMediumMove(session.currentState, Stone::White, session.positionHistory);
        } else if (aiDifficulty == AIDifficulty::Hard) {
            aiMove = chooseHardMove(session.currentState, Stone::White, session.positionHistory);
        }
    } catch (...) {
        aiMove = { true, {0, 0} };
    }

    tryPlayMove(session, aiMove);
}

// ======================= GAMEPLAY =======================
void Game::UpdateGameplay() {
    Vector2 mousePos = GetMousePosition();

    if (MainMenuButton.IsClicked(mousePos)) {
        currentScreen = MENU;
        return;
    }
    else if (SaveButton.IsClicked(mousePos)) {
        saveGame("savegame.json", session);
        return;
    }

    if (session.currentState.phase != GamePhase::Playing) {
        return;
    }

    if (currentScreen == PVE && session.currentState.sideToMove == Stone::White) {
        HandleAITurn();
        return;
    }

    if (PassButton.IsClicked(mousePos)) {
        Move passMove;
        passMove.isPass = true;
        tryPlayMove(session, passMove);
        aiThinkTimer = 0.0f;
        return;
    }
    else if (UndoButton.IsClicked(mousePos)) {
        size_t count = (session.mode == GameMode::PvE) ? 2 : 1;
        undoMoves(session, count);
        aiThinkTimer = 0.0f;
        return;
    }
    else if (RedoButton.IsClicked(mousePos)) {
        size_t count = (session.mode == GameMode::PvE) ? 2 : 1;
        redoMoves(session, count);
        aiThinkTimer = 0.0f;
        return;
    }
    else if (ResignButton.IsClicked(mousePos)) {
        session.currentState.phase = GamePhase::Finished;
        return;
    }

    int boardSize = static_cast<int>(session.currentState.board.size());
    if (boardSize <= 0) return;

    Point cell = board.GetCellIndex(mousePos, boardSize);

    if (cell.col != std::numeric_limits<std::size_t>::max() &&
        cell.row != std::numeric_limits<std::size_t>::max() &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Move currentMove{ false, cell };
        if (tryPlayMove(session, currentMove)) {
            aiThinkTimer = 0.0f;
        }
    }
}

void Game::DrawGameplay() {
    background.Draw(true);

    board.Draw(session.currentState.board);
    Stone lastColor = Stone::Empty;
    if (session.currentState.sideToMove == Stone::Black) lastColor = Stone::White;
    else if (session.currentState.sideToMove == Stone::White) lastColor = Stone::Black;
    board.DrawLastPlayed(session.currentState.lastMove, lastColor);

    int boardSize = static_cast<int>(session.currentState.board.size());
    bool isPlayerTurn = (currentScreen == PVP) || (currentScreen == PVE && session.currentState.sideToMove == Stone::Black);
    if (boardSize > 0 && session.currentState.phase == GamePhase::Playing && isPlayerTurn) {
        Point cell = board.GetCellIndex(GetMousePosition(), boardSize);
        if (cell.col != std::numeric_limits<std::size_t>::max() &&
            cell.row != std::numeric_limits<std::size_t>::max() &&
            session.currentState.board[cell.col][cell.row] == Stone::Empty) {
            board.DrawGhostStone(cell, session.currentState.sideToMove);
        }
    }

    DrawRectangleRounded({950.0f, 100.0f, 490.0f, 420.0f}, 0.08f, 16, Fade(BLACK, 0.45f));
    DrawRectangleRoundedLinesEx({950.0f, 100.0f, 490.0f, 420.0f}, 0.08f, 16, 2.0f, Fade(WHITE, 0.3f));

    std::string modeTitle = (session.mode == GameMode::PvE) ? "Player vs AI" : "Player vs Player";
    if (session.mode == GameMode::PvE) {
        if (aiDifficulty == AIDifficulty::Easy) modeTitle += " (Easy)";
        else if (aiDifficulty == AIDifficulty::Medium) modeTitle += " (Med)";
        else if (aiDifficulty == AIDifficulty::Hard) modeTitle += " (Hard)";
    }
    Vector2 mSize = MeasureTextEx(textFont, modeTitle.c_str(), 36.0f, 2.0f);
    DrawTextEx(textFont, modeTitle.c_str(), {std::round(950.0f + (490.0f - mSize.x) / 2.0f), 130.0f}, 36.0f, 2.0f, GOLD);

    if (session.currentState.phase == GamePhase::Playing) {
        std::string turnText;
        if (currentScreen == PVE && session.currentState.sideToMove == Stone::White) {
            turnText = "Turn: AI Thinking...";
        } else {
            turnText = (session.currentState.sideToMove == Stone::Black) ? "Turn: Black" : "Turn: White";
        }
        DrawTextEx(textFont, turnText.c_str(), {990.0f, 200.0f}, 30.0f, 2.0f, WHITE);

        Color stoneColor = (session.currentState.sideToMove == Stone::Black) ? BLACK : WHITE;
        DrawCircle(1380, 215, 18, stoneColor);
        DrawCircleLines(1380, 215, 18, (session.currentState.sideToMove == Stone::Black) ? WHITE : BLACK);

        std::string capturedBlack = "Black (Player): " + std::to_string(session.currentState.capturedByBlack);
        std::string capturedWhite = (currentScreen == PVE) 
            ? "White (AI): " + std::to_string(session.currentState.capturedByWhite)
            : "White captured: " + std::to_string(session.currentState.capturedByWhite);
        DrawTextEx(textFont, capturedBlack.c_str(), {990.0f, 270.0f}, 26.0f, 2.0f, RAYWHITE);
        DrawTextEx(textFont, capturedWhite.c_str(), {990.0f, 320.0f}, 26.0f, 2.0f, RAYWHITE);

        std::string passInfo = "Consecutive passes: " + std::to_string(session.currentState.consecutivePasses) + " / 2";
        DrawTextEx(textFont, passInfo.c_str(), {990.0f, 370.0f}, 24.0f, 2.0f, LIGHTGRAY);
    } else {
        SessionSnapshot scoringState = session.currentState;
        if (scoringState.phase == GamePhase::Playing) {
            scoringState.phase = GamePhase::Scoring;
        }
        if (scoringState.territoryProposal.empty()) {
            scoringState.territoryProposal = generateTerritoryProposal(scoringState);
        }

        Score score{0.0, 6.5};
        try {
            score = calculateAreaScore(scoringState, 6.5);
        } catch (...) {
        }

        std::ostringstream ssB, ssW;
        ssB << std::fixed << std::setprecision(1) << score.black;
        ssW << std::fixed << std::setprecision(1) << score.white;

        DrawTextEx(textFont, "FINAL SCORE", {1080.0f, 190.0f}, 30.0f, 2.0f, GOLD);

        std::string blackScoreStr = "Black: " + ssB.str() + " pts";
        std::string whiteScoreStr = "White: " + ssW.str() + " pts (Komi 6.5)";
        DrawTextEx(textFont, blackScoreStr.c_str(), {990.0f, 245.0f}, 26.0f, 2.0f, RAYWHITE);
        DrawTextEx(textFont, whiteScoreStr.c_str(), {990.0f, 290.0f}, 26.0f, 2.0f, RAYWHITE);

        if (score.black > score.white) {
            std::ostringstream diff;
            diff << std::fixed << std::setprecision(1) << (score.black - score.white);
            std::string winStr = "Winner: Black by " + diff.str() + " pts!";
            DrawTextEx(textFont, winStr.c_str(), {990.0f, 345.0f}, 26.0f, 2.0f, GREEN);
        } else {
            std::ostringstream diff;
            diff << std::fixed << std::setprecision(1) << (score.white - score.black);
            std::string winStr = "Winner: White by " + diff.str() + " pts!";
            DrawTextEx(textFont, winStr.c_str(), {990.0f, 345.0f}, 26.0f, 2.0f, GREEN);
        }

        DrawTextEx(textFont, "GAME OVER", {1100.0f, 430.0f}, 32.0f, 2.0f, RED);
    }

    PassButton.Draw();
    ResignButton.Draw();
    UndoButton.Draw();
    RedoButton.Draw();
    SaveButton.Draw();
    MainMenuButton.Draw();
}

// ======================= MAIN LOOP =======================
void Game::Draw(){
    switch (currentScreen)
    {
    case MENU:
        DrawMenu();
        break;
    case SIZE:
        DrawSelectSize();
        break;
    case MODE:
        DrawSelectMode();
        break;
    case DIFFICULTY:
        DrawSelectDifficulty();
        break;
    case PVP:
    case PVE:
        DrawGameplay();
        break;
    case SKIN:
        break;
    case EXIT:
        break;
    default:
        break;
    }
}

void Game::Update(){
    switch (currentScreen)
    {
    case MENU:
        UpdateMenu();
        break;
    case SIZE:
        UpdateSelectSize();
        break;
    case MODE:
        UpdateSelectMode();
        break;
    case DIFFICULTY:
        UpdateSelectDifficulty();
        break;
    case PVP:
    case PVE:
        UpdateGameplay();
        break;
    case SKIN:
        break;
    case EXIT:
        break;
    default:
        break;
    }
}
