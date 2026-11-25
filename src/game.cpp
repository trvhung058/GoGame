#include "game.h"
#include <cmath>
#include <exception>
#include <limits>
#include <string>
#include <sstream>
#include <iomanip>
#include "save_load.h"
#include "ai.h"

Game::Game(){
    bool hasSavedGame = loadGame("savegame.json", session);
    if (!hasSavedGame) {
        createInitialState(session.currentState, 19);
        initializePositionHistory(session);
    }

    selectedBoardSize = static_cast<int>(session.currentState.board.size());
    board.SetBoardSize(selectedBoardSize);

    board.LoadAllAssets();

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

    BackFromSkinButton.LoadAssets(btnUnhovered300, btnHovered300);
    BackFromSkinButton.LoadFonts(fontPath);
    BackFromSkinButton.SetText("Back");
    BackFromSkinButton.SetPosition((screenWidth - texSize.first) / 2.0f, 800.0f);

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

    AcceptButton.LoadAssets(btnUnhovered200, btnHovered200);
    AcceptButton.LoadFonts(fontPath);
    AcceptButton.SetText("Accept");
    AcceptButton.SetPosition(col1X, row1Y);

    ResumeButton.LoadAssets(btnUnhovered200, btnHovered200);
    ResumeButton.LoadFonts(fontPath);
    ResumeButton.SetText("Resume");
    ResumeButton.SetPosition(col2X, row1Y);

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
    float titleY = 100.0f; 

    DrawTextEx(currentFont, title, {titleX + 3.0f, titleY + 3.0f}, fontSize, spacing, Color{0, 0, 0, 200});
    DrawTextEx(currentFont, title, {titleX, titleY}, fontSize, spacing, WHITE);
}

// ======================= MENU =======================
void Game::UpdateMenu() {
    Vector2 mousePos = GetMousePosition();
    if (ContinueButton.IsClicked(mousePos)) {
        if (loadGame("savegame.json", session)) {
            selectedBoardSize = static_cast<int>(session.currentState.board.size());
            board.SetBoardSize(selectedBoardSize);
            aiThinkTimer = 0.0f;
            hasPendingPointlessWarning = false;
            currentScreen = (session.mode == GameMode::PvE) ? PVE : PVP;
        }
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
        board.SetBoardSize(9);
        currentScreen = MODE;
    }
    else if (Size13Button.IsClicked(mousePos)) {
        selectedBoardSize = 13;
        board.SetBoardSize(13);
        currentScreen = MODE;
    }
    else if (Size19Button.IsClicked(mousePos)) {
        selectedBoardSize = 19;
        board.SetBoardSize(19);
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
        aiThinkTimer = 0.0f;
        hasPendingPointlessWarning = false;
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
        hasPendingPointlessWarning = false;
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
        hasPendingPointlessWarning = false;
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
        hasPendingPointlessWarning = false;
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

// ======================= SKINS CUSTOMIZATION =======================
struct BoardSkinOption {
    BoardSkinType type;
    const char* name;
    Rectangle rect;
};

struct StoneSkinOption {
    StoneSkinType type;
    const char* name;
    Vector2 center;
    float radius;
};

static constexpr BoardSkinOption kBoardSkinOptions[3] = {
    { BoardSkinType::ClassicWood, "Classic Wood", { 90.0f, 320.0f, 180.0f, 180.0f } },
    { BoardSkinType::Hardwood,    "Hardwood",     { 310.0f, 320.0f, 180.0f, 180.0f } },
    { BoardSkinType::DarkWood,    "Dark Wood",    { 530.0f, 320.0f, 180.0f, 180.0f } }
};

static constexpr StoneSkinOption kStoneSkinOptions[3] = {
    { StoneSkinType::Classic,     "Classic 3D",   { 900.0f, 410.0f }, 75.0f },
    { StoneSkinType::FlatMinimal, "Glossy Stone", { 1110.0f, 410.0f }, 75.0f },
    { StoneSkinType::PureSolid,   "Pure Solid",   { 1320.0f, 410.0f }, 75.0f }
};

void Game::UpdateSkin() {
    Vector2 mousePos = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        for (const auto& opt : kBoardSkinOptions) {
            if (CheckCollisionPointRec(mousePos, opt.rect)) {
                board.SetBoardSkin(opt.type);
                break;
            }
        }

        for (const auto& opt : kStoneSkinOptions) {
            if (CheckCollisionPointCircle(mousePos, opt.center, opt.radius)) {
                board.SetStoneSkin(opt.type);
                break;
            }
        }
    }

    if (BackFromSkinButton.IsClicked(mousePos)) {
        currentScreen = MENU;
    }
}

void Game::DrawSkin() {
    background.Draw(true);
    DrawPageTitle("Skins Customization");

    Vector2 tSize1 = MeasureTextEx(textFont, "Board Skins", 42.0f, 2.0f);
    DrawTextEx(textFont, "Board Skins", {400.0f - tSize1.x / 2.0f, 230.0f}, 42.0f, 2.0f, GOLD);

    Vector2 tSize2 = MeasureTextEx(textFont, "Stone Skins", 42.0f, 2.0f);
    DrawTextEx(textFont, "Stone Skins", {1110.0f - tSize2.x / 2.0f, 230.0f}, 42.0f, 2.0f, GOLD);

    DrawLine(750, 240, 750, 750, Fade(WHITE, 0.2f));

    Vector2 mousePos = GetMousePosition();

    for (const auto& opt : kBoardSkinOptions) {
        Rectangle r = opt.rect;
        bool isHovered = CheckCollisionPointRec(mousePos, r);
        bool isSelected = (board.GetBoardSkin() == opt.type);

        DrawRectangleRounded({ r.x + 3.0f, r.y + 3.0f, r.width, r.height }, 0.08f, 8, Fade(BLACK, 0.5f));

        Texture2D tex = board.GetBoardTexture(opt.type);
        if (tex.id != 0) {
            Rectangle src = { 0.0f, 0.0f, static_cast<float>(tex.width), static_cast<float>(tex.height) };
            DrawTexturePro(tex, src, r, {0, 0}, 0.0f, WHITE);
        }

        float step = r.width / 5.0f;
        for (int line = 1; line < 5; line++) {
            DrawLineV({ r.x + line * step, r.y + step }, { r.x + line * step, r.y + r.height - step }, Fade(BLACK, 0.5f));
            DrawLineV({ r.x + step, r.y + line * step }, { r.x + r.width - step, r.y + line * step }, Fade(BLACK, 0.5f));
        }

        if (isSelected) {
            DrawRectangleLinesEx({ r.x - 6.0f, r.y - 6.0f, r.width + 12.0f, r.height + 12.0f }, 4.0f, GREEN);
            DrawRectangleLinesEx({ r.x - 1.0f, r.y - 1.0f, r.width + 2.0f, r.height + 2.0f }, 2.0f, WHITE);
        } else if (isHovered) {
            DrawRectangleLinesEx({ r.x - 4.0f, r.y - 4.0f, r.width + 8.0f, r.height + 8.0f }, 3.0f, Fade(YELLOW, 0.7f));
        } else {
            DrawRectangleLinesEx(r, 2.0f, Fade(WHITE, 0.3f));
        }

        Vector2 nameSize = MeasureTextEx(textFont, opt.name, 26.0f, 2.0f);
        DrawTextEx(textFont, opt.name, { r.x + (r.width - nameSize.x) / 2.0f, r.y + r.height + 15.0f }, 26.0f, 2.0f, isSelected ? GREEN : RAYWHITE);
    }

    for (const auto& opt : kStoneSkinOptions) {
        Vector2 c = opt.center;
        float radius = opt.radius;
        bool isHovered = CheckCollisionPointCircle(mousePos, c, radius);
        bool isSelected = (board.GetStoneSkin() == opt.type);

        DrawCircleV(c, radius, Fade(BLACK, 0.4f));

        if (opt.type == StoneSkinType::PureSolid) {
            DrawCircleV({ c.x - 24.0f, c.y }, 34.0f, Color{15, 15, 15, 255});
            DrawCircleLinesV({ c.x - 24.0f, c.y }, 34.0f, Color{70, 70, 70, 255});

            DrawCircleV({ c.x + 24.0f, c.y }, 34.0f, Color{245, 245, 245, 255});
            DrawCircleLinesV({ c.x + 24.0f, c.y }, 34.0f, Color{180, 180, 180, 255});
        } else {
            Texture2D bTex = board.GetBlackStoneTexture(opt.type, 9);
            Texture2D wTex = board.GetWhiteStoneTexture(opt.type, 9);

            if (bTex.id != 0) {
                DrawTexturePro(bTex, { 0, 0, (float)bTex.width, (float)bTex.height }, { c.x - 56.0f, c.y - 34.0f, 68.0f, 68.0f }, {0, 0}, 0.0f, WHITE);
            }
            if (wTex.id != 0) {
                DrawTexturePro(wTex, { 0, 0, (float)wTex.width, (float)wTex.height }, { c.x - 12.0f, c.y - 34.0f, 68.0f, 68.0f }, {0, 0}, 0.0f, WHITE);
            }
        }

        if (isSelected) {
            DrawCircleLinesV(c, radius + 7.0f, GREEN);
            DrawCircleLinesV(c, radius + 8.0f, GREEN);
            DrawCircleLinesV(c, radius + 9.0f, GREEN);
            DrawCircleLinesV(c, radius, WHITE);
        } else if (isHovered) {
            DrawCircleLinesV(c, radius + 5.0f, Fade(YELLOW, 0.7f));
            DrawCircleLinesV(c, radius + 6.0f, Fade(YELLOW, 0.7f));
        } else {
            DrawCircleLinesV(c, radius, Fade(WHITE, 0.3f));
        }

        Vector2 nameSize = MeasureTextEx(textFont, opt.name, 26.0f, 2.0f);
        DrawTextEx(textFont, opt.name, { c.x - nameSize.x / 2.0f, c.y + radius + 25.0f }, 26.0f, 2.0f, isSelected ? GREEN : RAYWHITE);
    }

    BackFromSkinButton.Draw();
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
    } catch (const std::exception& exception) {
        TraceLog(LOG_ERROR, "AI move selection failed: %s", exception.what());
        aiMove = { true, {0, 0} };
    } catch (...) {
        TraceLog(LOG_ERROR, "AI move selection failed with an unknown exception.");
        aiMove = { true, {0, 0} };
    }

    tryPlayMove(session, aiMove);
}

void Game::UpdateTerritoty() {
    if (session.currentState.phase != GamePhase::Scoring) return;

    int bSize = static_cast<int>(session.currentState.board.size());
    if (session.currentState.territoryProposal.size() != static_cast<size_t>(bSize)) {
        if (session.currentState.deadStones.size() != session.currentState.board.size()) {
            session.currentState.deadStones = generateDeadStoneProposal(session.currentState);
        }
        session.currentState.territoryProposal = generateTerritoryProposal(session.currentState);
    }
}

void Game::DrawTerritoty() {
    bool isAcceptedFinished = (session.currentState.phase == GamePhase::Finished &&
                               session.currentState.blackAcceptedScore &&
                               session.currentState.whiteAcceptedScore);
    if (session.currentState.phase != GamePhase::Scoring && !isAcceptedFinished) return;
    int bSize = static_cast<int>(session.currentState.board.size());
    if (session.currentState.territoryProposal.size() != static_cast<size_t>(bSize)) return;

    float cell_size = board.GetCellSize();
    float markerSize = (cell_size > 0) ? (cell_size * 0.35f) : 14.0f;
    float halfSize = markerSize / 2.0f;

    for (int col = 0; col < bSize; col++) {
        for (int row = 0; row < bSize; row++) {
            TerritoryOwner owner = session.currentState.territoryProposal[col][row];
            if (owner != TerritoryOwner::White && owner != TerritoryOwner::Black) continue;

            Vector2 pixelPos = board.GetPixelPosition(col, row);
            int px = static_cast<int>(pixelPos.x - halfSize);
            int py = static_cast<int>(pixelPos.y - halfSize);
            int ps = static_cast<int>(markerSize);

            if (owner == TerritoryOwner::Black) {
                DrawRectangle(px, py, ps, ps, Color{ 15, 15, 15, 230 });
                DrawRectangleLines(px, py, ps, ps, Color{ 160, 160, 160, 200 });
            } else {
                DrawRectangle(px, py, ps, ps, Color{ 245, 245, 245, 230 });
                DrawRectangleLines(px, py, ps, ps, Color{ 50, 50, 50, 200 });
            }
        }
    }
}

void Game::UpdateDeadGroup() {
    if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        Vector2 MousePos = GetMousePosition();
        int bSize = static_cast<int>(session.currentState.board.size());
        Point position = board.GetCellIndex(MousePos, bSize);
        toggleDeadGroups(session.currentState, position);
    }
}

void Game::DrawDeadGroup() {

}

// ======================= GAMEPLAY =======================
void Game::UpdateGameplay() {
    Vector2 mousePos = GetMousePosition();

    if (session.currentState.phase == GamePhase::Scoring) {
        if (AcceptButton.IsClicked(mousePos)) {
            reviewScore(session.currentState, session.currentState.sideToMove, true);
        }
        else if (ResumeButton.IsClicked(mousePos)) {
            reviewScore(session.currentState, session.currentState.sideToMove, false);
            aiThinkTimer = 0.0f;
            hasPendingPointlessWarning = false;
        }
        return;
    }

    if (MainMenuButton.IsClicked(mousePos)) {
        hasPendingPointlessWarning = false;
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
        hasPendingPointlessWarning = false;
        return;
    }
    else if (UndoButton.IsClicked(mousePos)) {
        size_t count = (session.mode == GameMode::PvE) ? 2 : 1;
        undoMoves(session, count);
        aiThinkTimer = 0.0f;
        hasPendingPointlessWarning = false;
        return;
    }
    else if (RedoButton.IsClicked(mousePos)) {
        size_t count = (session.mode == GameMode::PvE) ? 2 : 1;
        redoMoves(session, count);
        aiThinkTimer = 0.0f;
        hasPendingPointlessWarning = false;
        return;
    }
    else if (ResignButton.IsClicked(mousePos)) {
        session.currentState.phase = GamePhase::Finished;
        hasPendingPointlessWarning = false;
        return;
    }

    int boardSize = static_cast<int>(session.currentState.board.size());
    if (boardSize <= 0) return;

    Point cell = board.GetCellIndex(mousePos, boardSize);

    if (cell.col != std::numeric_limits<std::size_t>::max() &&
        cell.row != std::numeric_limits<std::size_t>::max() &&
        IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        bool confirmedSameCell = hasPendingPointlessWarning &&
                                 pendingWarningCell.col == cell.col &&
                                 pendingWarningCell.row == cell.row;

        if (!confirmedSameCell && shouldWarnAboutPointlessMove(session, cell)) {
            hasPendingPointlessWarning = true;
            pendingWarningCell = cell;
            return;
        }

        hasPendingPointlessWarning = false;
        Move currentMove{ false, cell };
        if (tryPlayMove(session, currentMove)) {
            aiThinkTimer = 0.0f;
        }
    }
}

void Game::DrawGameplay() {
    background.Draw(true);

    bool isAcceptedFinished = (session.currentState.phase == GamePhase::Finished &&
                               session.currentState.blackAcceptedScore &&
                               session.currentState.whiteAcceptedScore);
    if (session.currentState.phase == GamePhase::Scoring || isAcceptedFinished) {
        board.Draw(session.currentState.board, session.currentState.deadStones);
    } else {
        board.Draw(session.currentState.board);
    }
    Stone lastColor = Stone::Empty;
    if (session.currentState.sideToMove == Stone::Black) lastColor = Stone::White;
    else if (session.currentState.sideToMove == Stone::White) lastColor = Stone::Black;
    board.DrawLastPlayed(session.currentState.lastMove, lastColor);
    board.DrawCapturedMarkers(session.currentState.previousBoard, session.currentState.board);

    int boardSize = static_cast<int>(session.currentState.board.size());
    bool isPlayerTurn = (currentScreen == PVP) || (currentScreen == PVE && session.currentState.sideToMove == Stone::Black);
    if (boardSize > 0 && session.currentState.phase == GamePhase::Playing && isPlayerTurn) {
        Point cell = board.GetCellIndex(GetMousePosition(), boardSize);
        if (cell.col != std::numeric_limits<std::size_t>::max() &&
            cell.row != std::numeric_limits<std::size_t>::max() &&
            session.currentState.board[cell.col][cell.row] == Stone::Empty &&
            isLegalMove(session, cell)) {
            board.DrawGhostStone(cell, session.currentState.sideToMove);
        }
        if (hasPendingPointlessWarning) {
            Vector2 warnPos = board.GetPixelPosition(pendingWarningCell);
            float cellSize = board.GetCellSize();
            float ringRadius = (cellSize > 0.0f) ? (cellSize * 0.45f) : 16.0f;
            DrawCircleLines(static_cast<int>(warnPos.x), static_cast<int>(warnPos.y), ringRadius, ORANGE);
            DrawCircleLines(static_cast<int>(warnPos.x), static_cast<int>(warnPos.y), ringRadius + 1.5f, ORANGE);
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

        if (hasPendingPointlessWarning) {
            DrawTextEx(textFont, "Warning: Useless move!", {990.0f, 420.0f}, 24.0f, 1.5f, ORANGE);
            DrawTextEx(textFont, "Click again on the same spot to play.", {990.0f, 455.0f}, 20.0f, 1.5f, YELLOW);
        }
    } else if (session.currentState.phase == GamePhase::Scoring) {
        SessionSnapshot scoringState = session.currentState;
        if (scoringState.deadStones.size() != scoringState.board.size()) {
            scoringState.deadStones = generateDeadStoneProposal(scoringState);
        }
        if (scoringState.territoryProposal.empty()) {
            scoringState.territoryProposal = generateTerritoryProposal(scoringState);
        }

        Score score{0.0, kKomi};
        try {
            score = calculateAreaScore(scoringState, kKomi);
        } catch (...) {
        }

        std::ostringstream ssB, ssW;
        ssB << std::fixed << std::setprecision(1) << score.black;
        ssW << std::fixed << std::setprecision(1) << score.white;

        DrawTextEx(textFont, "DEAD STONE REVIEW", {1030.0f, 190.0f}, 28.0f, 2.0f, GOLD);

        std::string blackScoreStr = "Black: " + ssB.str() + " pts";
        std::string whiteScoreStr = "White: " + ssW.str() + " pts (Komi 7.5)";
        DrawTextEx(textFont, blackScoreStr.c_str(), {990.0f, 245.0f}, 26.0f, 2.0f, RAYWHITE);
        DrawTextEx(textFont, whiteScoreStr.c_str(), {990.0f, 290.0f}, 26.0f, 2.0f, RAYWHITE);
        DrawTextEx(textFont, "Click stone groups to toggle dead.", {990.0f, 355.0f}, 22.0f, 1.5f, LIGHTGRAY);
        DrawTextEx(textFont, "Accept to finish or Resume to play.", {990.0f, 395.0f}, 22.0f, 1.5f, LIGHTGRAY);
    } else {
        if (!isAcceptedFinished) {
            Stone resigner = session.currentState.sideToMove;
            std::string winStr = (resigner == Stone::Black) ? "White wins by Resignation!" : "Black wins by Resignation!";
            DrawTextEx(textFont, "RESIGNATION", {1080.0f, 220.0f}, 30.0f, 2.0f, GOLD);
            DrawTextEx(textFont, winStr.c_str(), {990.0f, 280.0f}, 26.0f, 2.0f, GREEN);
            DrawTextEx(textFont, "GAME OVER", {1100.0f, 400.0f}, 32.0f, 2.0f, RED);
        } else {
            SessionSnapshot scoringState = session.currentState;
            if (scoringState.deadStones.size() != scoringState.board.size()) {
                scoringState.deadStones = generateDeadStoneProposal(scoringState);
            }
            if (scoringState.territoryProposal.empty()) {
                scoringState.territoryProposal = generateTerritoryProposal(scoringState);
            }

            Score score{0.0, kKomi};
            try {
                score = calculateAreaScore(scoringState, kKomi);
            } catch (...) {
            }

            std::ostringstream ssB, ssW;
            ssB << std::fixed << std::setprecision(1) << score.black;
            ssW << std::fixed << std::setprecision(1) << score.white;

            DrawTextEx(textFont, "FINAL SCORE", {1080.0f, 190.0f}, 30.0f, 2.0f, GOLD);

            std::string blackScoreStr = "Black: " + ssB.str() + " pts";
            std::string whiteScoreStr = "White: " + ssW.str() + " pts (Komi 7.5)";
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
    }

    if (session.currentState.phase == GamePhase::Scoring) {
        AcceptButton.Draw();
        ResumeButton.Draw();
    } else {
        PassButton.Draw();
        ResignButton.Draw();
        UndoButton.Draw(); 
        RedoButton.Draw();
        SaveButton.Draw();
        MainMenuButton.Draw();
    }
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
    case SKIN:
        DrawSkin();
        break;
    case PVP:
    case PVE:
        DrawGameplay();
        DrawTerritoty();
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
    case SKIN:
        UpdateSkin();
        break;
    case PVP:
    case PVE:
        UpdateGameplay();
        UpdateTerritoty();
        UpdateDeadGroup();
        break;
    case EXIT:
        break;
    default:
        break;
    }
}
