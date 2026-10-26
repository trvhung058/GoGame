#include "game.h"

Game::Game(){
    createInitialState(session.currentState, 19);

    board.LoadAssetsDefault("assets/board/wood1.jpg", "assets/b_40.png", "assets/w_40.png");
    background.LoadAssets("assets/go-jeu_1000_blur.jpg");
    background.SetPosition(0, 0);
    logo.LoadAssets("assets/UI/icon258.png");
    logo.SetPosition(371, 70);

    const char *btnUnhovered = "assets/button/button_unhovered_300.png";
    const char *btnHovered = "assets/button/button_hovered_300.png";
    const char *fontPath = "assets/Font/NotoSerif-Regular.ttf";

    //Continue Button
    ContinueButton.LoadAssets(btnUnhovered, btnHovered);
    ContinueButton.LoadFonts(fontPath);
    ContinueButton.SetText("Continue");

    //New Game Button
    NewGameButton.LoadAssets(btnUnhovered, btnHovered);
    NewGameButton.LoadFonts(fontPath);
    NewGameButton.SetText("New Game");

    //Skins Button
    SkinsButton.LoadAssets(btnUnhovered, btnHovered);
    SkinsButton.LoadFonts(fontPath);
    SkinsButton.SetText("Skins");

    //Exit Button
    ExitButton.LoadAssets(btnUnhovered, btnHovered);
    ExitButton.LoadFonts(fontPath);
    ExitButton.SetText("Exit");

    std::pair<float, float> texSize = NewGameButton.GetSize();
    float startX = 500.0f - texSize.first / 2.0f;
    float startY = 360.0f;
    float spacing = texSize.second + 15.0f;

    ContinueButton.SetPosition(startX, startY);
    NewGameButton.SetPosition(startX, startY + spacing);
    SkinsButton.SetPosition(startX, startY + spacing * 2.0f);
    ExitButton.SetPosition(startX, startY + spacing * 3.0f);

    currentScreen = MENU;
}

void Game::UpdateMenu() {
    Vector2 mousePos = GetMousePosition();
    if (ContinueButton.IsClicked(mousePos)) {
        currentScreen = PVP;
    }
    else if (NewGameButton.IsClicked(mousePos)) {
        createInitialState(session.currentState, 19);
        session.undoHistory.clear();
        session.redoHistory.clear();
        currentScreen = PVP;
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

void Game::Draw(){
    switch (currentScreen)
    {
    case MENU:
        DrawMenu();
        break;
    case PVP:
        board.Draw(session.currentState.board);
        break;
    case PVE:
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
    case PVP:
        // UpdateGameplay();
        break;
    case PVE:
        break;
    case SKIN:
        break;
    case EXIT:
        break;
    default:
        break;
    }
}
