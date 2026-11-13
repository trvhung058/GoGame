#include "game.h"
#include <cmath>
#include <string>
#include "save_load.h"

Game::Game(){
    createInitialState(session.currentState, 19);

    board.LoadAssetsDefault("assets/board/Board_800.png", "assets/b_40.png", "assets/w_40.png");
    background.LoadAssets("assets/go-jeu_blur.jpg");
    background.SetPosition(0, 0);

    float screenWidth = static_cast<float>(GetScreenWidth());

    logo.LoadAssets("assets/UI/icon258.png");
    std::pair<int, int> logoSize = logo.GetSize();
    logo.SetPosition((screenWidth - logoSize.first) / 2.0f, 70);

    const char *btnUnhovered300 = "assets/button/button_unhovered_300.png";
    const char *btnHovered300 = "assets/button/button_hovered_300.png";
    const char *btnUnhovered200 = "assets/button/button_unhovered_200.png";
    const char *btnHovered200 = "assets/button/button_hovered_200.png";
    const char *fontPath = "assets/Font/NotoSerif-Bold.ttf";

    titleFont = LoadFontEx(fontPath, 128, 0, 0);
    SetTextureFilter(titleFont.texture, TEXTURE_FILTER_BILINEAR);

    textFont = LoadFontEx(fontPath, 64, 0, 0);
    SetTextureFilter(textFont.texture, TEXTURE_FILTER_BILINEAR);

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
        session.undoHistory.clear();
        session.redoHistory.clear();
        session.mode = GameMode::PvP;
        currentScreen = PVP;
    }
    else if (PvEButton.IsClicked(mousePos)) {
        createInitialState(session.currentState, selectedBoardSize);
        session.undoHistory.clear();
        session.redoHistory.clear();
        session.mode = GameMode::PvE;
        currentScreen = PVE;
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

// ======================= GAMEPLAY =======================
void Game::UpdateGameplay() {
    Vector2 mousePos = GetMousePosition();

    if (PassButton.IsClicked(mousePos)) {
        Move passMove;
        passMove.isPass = true;
        tryPlayMove(session, passMove);
        return;
    }
    else if (UndoButton.IsClicked(mousePos)) {
        size_t count = (session.mode == GameMode::PvE) ? 2 : 1;
        undoMoves(session, count);
        return;
    }
    else if (RedoButton.IsClicked(mousePos)) {
        size_t count = (session.mode == GameMode::PvE) ? 2 : 1;
        redoMoves(session, count);
        return;
    }
    else if (ResignButton.IsClicked(mousePos)) {
        session.currentState.phase = GamePhase::Finished;
        return;
    }
    else if (SaveButton.IsClicked(mousePos)) {
        saveGame("savegame.json", session);
        return;
    }
    else if (MainMenuButton.IsClicked(mousePos)) {
        currentScreen = MENU;
        return;
    }

    int boardSize = static_cast<int>(session.currentState.board.size());
    if (boardSize <= 0) return;

    Point cell = board.GetCellIndex(mousePos, boardSize);

    if (cell.col != -1 && cell.row != -1 && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Move currentMove{ false, cell };
        tryPlayMove(session, currentMove);
    }
}

void Game::DrawGameplay() {
    background.Draw(true);

    board.Draw(session.currentState.board);

    int boardSize = static_cast<int>(session.currentState.board.size());
    if (boardSize > 0) {
        Point cell = board.GetCellIndex(GetMousePosition(), boardSize);
        if (cell.col != -1 && cell.row != -1 && session.currentState.board[cell.col][cell.row] == Stone::Empty) {
            board.DrawGhostStone(cell.col, cell.row, session.currentState.sideToMove);
        }
    }

    DrawRectangleRounded({950.0f, 100.0f, 490.0f, 420.0f}, 0.08f, 16, Fade(BLACK, 0.45f));
    DrawRectangleRoundedLinesEx({950.0f, 100.0f, 490.0f, 420.0f}, 0.08f, 16, 2.0f, Fade(WHITE, 0.3f));

    const char* modeTitle = (session.mode == GameMode::PvE) ? "Player vs AI" : "Player vs Player";
    Vector2 mSize = MeasureTextEx(textFont, modeTitle, 36.0f, 2.0f);
    DrawTextEx(textFont, modeTitle, {std::round(950.0f + (490.0f - mSize.x) / 2.0f), 130.0f}, 36.0f, 2.0f, GOLD);

    const char* turnText = (session.currentState.sideToMove == Stone::Black) ? "Turn: Black" : "Turn: White";
    DrawTextEx(textFont, turnText, {990.0f, 200.0f}, 32.0f, 2.0f, WHITE);

    Color stoneColor = (session.currentState.sideToMove == Stone::Black) ? BLACK : WHITE;
    DrawCircle(1360, 215, 18, stoneColor);
    DrawCircleLines(1360, 215, 18, (session.currentState.sideToMove == Stone::Black) ? WHITE : BLACK);

    std::string capturedBlack = "Black captured: " + std::to_string(session.currentState.capturedByBlack);
    std::string capturedWhite = "White captured: " + std::to_string(session.currentState.capturedByWhite);
    DrawTextEx(textFont, capturedBlack.c_str(), {990.0f, 270.0f}, 26.0f, 2.0f, RAYWHITE);
    DrawTextEx(textFont, capturedWhite.c_str(), {990.0f, 320.0f}, 26.0f, 2.0f, RAYWHITE);

    std::string passInfo = "Consecutive passes: " + std::to_string(session.currentState.consecutivePasses) + " / 2";
    DrawTextEx(textFont, passInfo.c_str(), {990.0f, 370.0f}, 24.0f, 2.0f, LIGHTGRAY);

    if (session.currentState.phase == GamePhase::Finished) {
        DrawTextEx(textFont, "GAME OVER", {1100.0f, 430.0f}, 32.0f, 2.0f, RED);
    } else if (session.currentState.phase == GamePhase::Scoring) {
        DrawTextEx(textFont, "SCORING PHASE", {1080.0f, 430.0f}, 30.0f, 2.0f, YELLOW);
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
