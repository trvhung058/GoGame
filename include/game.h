#pragma once
#include <vector>
#include "raylib.h"
#include "types.h"     
#include "game_engine.h"
#include "board.h" 
#include "UI.h"

class Game {
    private:
        Board board;
        GameSession session;

        UI background;
        UI logo;
        Font titleFont;

        // Menu buttons
        Button ContinueButton;
        Button NewGameButton;
        Button SkinsButton;
        Button ExitButton;

        // Size selection buttons
        Button Size9Button;
        Button Size13Button;
        Button Size19Button;
        Button BackFromSizeButton;

        // Mode selection buttons
        Button PvPButton;
        Button PvEButton;
        Button BackFromModeButton;

        int selectedBoardSize = 19;

        enum Scene {
            MENU,
            SKIN,
            MODE,
            SIZE,
            PVP,
            PVE,
            EXIT
        } currentScreen;

        void UpdateMenu();
        void DrawMenu();

        void UpdateSelectSize();
        void DrawSelectSize();

        void UpdateSelectMode();
        void DrawSelectMode();

        void UpdateGameplay();
        void DrawGameplay();

        void HandleInput();

        void DrawPageTitle(const char* title);

    public:
        Game();
        ~Game();

        void Update(); 
        void Draw();  

        bool ShouldClose() const { return currentScreen == EXIT; }
};
