#include "game.h"

Game::Game(){
    createInitialState(session.currentState, 19);

    board.LoadAssetsDefault("assets/board/Board_800.png", "assets/b_40.png", "assets/w_40.png");
    background.LoadAssets("assets/go-jeu_blur.jpg");
    background.SetPosition(0, 0);

    float screenWidth = static_cast<float>(GetScreenWidth());

    logo.LoadAssets("assets/UI/icon258.png");
    std::pair<int, int> logoSize = logo.GetSize();
    logo.SetPosition((screenWidth - logoSize.first) / 2.0f, 70);

    const char *btnUnhovered = "assets/button/button_unhovered_300.png";
    const char *btnHovered = "assets/button/button_hovered_300.png";
    const char *fontPath = "assets/Font/NotoSerif-Regular.ttf";

    titleFont = LoadFontEx(fontPath, 128, 0, 0);
    SetTextureFilter(titleFont.texture, TEXTURE_FILTER_BILINEAR);

    ContinueButton.LoadAssets(btnUnhovered, btnHovered);
    ContinueButton.LoadFonts(fontPath);
    ContinueButton.SetText("Continue");

    NewGameButton.LoadAssets(btnUnhovered, btnHovered);
    NewGameButton.LoadFonts(fontPath);
    NewGameButton.SetText("New Game");

    SkinsButton.LoadAssets(btnUnhovered, btnHovered);
    SkinsButton.LoadFonts(fontPath);
    SkinsButton.SetText("Skins");

    ExitButton.LoadAssets(btnUnhovered, btnHovered);
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

    Size9Button.LoadAssets(btnUnhovered, btnHovered);
    Size9Button.LoadFonts(fontPath);
    Size9Button.SetText("9x9");

    Size13Button.LoadAssets(btnUnhovered, btnHovered);
    Size13Button.LoadFonts(fontPath);
    Size13Button.SetText("13x13");

    Size19Button.LoadAssets(btnUnhovered, btnHovered);
    Size19Button.LoadFonts(fontPath);
    Size19Button.SetText("19x19");

    BackFromSizeButton.LoadAssets(btnUnhovered, btnHovered);
    BackFromSizeButton.LoadFonts(fontPath);
    BackFromSizeButton.SetText("Back");

    Size9Button.SetPosition(startX, startY);
    Size13Button.SetPosition(startX, startY + spacing);
    Size19Button.SetPosition(startX, startY + spacing * 2.0f);
    BackFromSizeButton.SetPosition(startX, startY + spacing * 3.0f);

    PvPButton.LoadAssets(btnUnhovered, btnHovered);
    PvPButton.LoadFonts(fontPath);
    PvPButton.SetText("Player vs Player");

    PvEButton.LoadAssets(btnUnhovered, btnHovered);
    PvEButton.LoadFonts(fontPath);
    PvEButton.SetText("Player vs AI");

    BackFromModeButton.LoadAssets(btnUnhovered, btnHovered);
    BackFromModeButton.LoadFonts(fontPath);
    BackFromModeButton.SetText("Back");

    float modeStartY = 400.0f;
    float modeSpacing = texSize.second + 20.0f;
    PvPButton.SetPosition(startX, modeStartY);
    PvEButton.SetPosition(startX, modeStartY + modeSpacing);
    BackFromModeButton.SetPosition(startX, modeStartY + modeSpacing * 2.0f);

    currentScreen = MENU;
}

Game::~Game() {
    if (titleFont.texture.id != 0) {
        UnloadFont(titleFont);
    }
}

void Game::DrawPageTitle(const char* title) {
    if (title == nullptr || title[0] == '\0') return;

    float fontSize = 95.0f; 
    float spacing = 4.0f;
    Font currentFont = (titleFont.texture.id != 0) ? titleFont : GetFontDefault();
    Vector2 textSize = MeasureTextEx(currentFont, title, fontSize, spacing);
    float titleX = (GetScreenWidth() - textSize.x) / 2.0f;
    float titleY = 120.0f; 

    DrawTextEx(currentFont, title, {titleX + 4.0f, titleY + 5.0f}, fontSize, spacing, Fade(BLACK, 0.75f));
    DrawTextEx(currentFont, title, {titleX + 2.0f, titleY + 2.5f}, fontSize, spacing, Fade(BLACK, 0.45f));
    
    DrawTextEx(currentFont, title, {titleX, titleY}, fontSize, spacing, RAYWHITE);
}

void Game::UpdateMenu() {
    Vector2 mousePos = GetMousePosition();
    if (ContinueButton.IsClicked(mousePos)) {
        currentScreen = PVP;
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

void Game::UpdateSelectMode() {
    Vector2 mousePos = GetMousePosition();

    if (PvPButton.IsClicked(mousePos)) {
        createInitialState(session.currentState, selectedBoardSize);
        session.undoHistory.clear();
        session.redoHistory.clear();
        currentScreen = PVP;
    }
    else if (PvEButton.IsClicked(mousePos)) {
        createInitialState(session.currentState, selectedBoardSize);
        session.undoHistory.clear();
        session.redoHistory.clear();
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
        board.Draw(session.currentState.board);
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
        // UpdateGameplay();
        break;
    case SKIN:
        break;
    case EXIT:
        break;
    default:
        break;
    }
}
