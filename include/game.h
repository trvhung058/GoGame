#pragma once
#include <vector>
#include "raylib.h"
#include "types.h"     
#include "game_engine.h"
#include "board.h" 
#include "UI.h"

enum class AIDifficulty {
    Easy,
    Medium,
    Hard
};

class Game {
    private:
        Board board;
        GameSession session;

        UI background;
        UI logo;
        Font titleFont;
        Font textFont;

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

        // AI Difficulty buttons
        Button EasyButton;
        Button MediumButton;
        Button HardButton;
        Button BackFromDiffButton;
        
        Button BackFromSkinButton;

        // Gameplay buttons
        Button PassButton;
        Button UndoButton;
        Button RedoButton;
        Button ResignButton;
        Button SaveButton;
        Button MainMenuButton;

        int selectedBoardSize = 19;
        AIDifficulty aiDifficulty = AIDifficulty::Easy;
        float aiThinkTimer = 0.0f; 
        TerritoryMap territory;

        enum Scene {
            MENU,
            SKIN,
            MODE,
            DIFFICULTY,
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

        void UpdateSelectDifficulty();
        void DrawSelectDifficulty();

        void UpdateSkin();
        void DrawSkin();

        void UpdateGameplay();
        void DrawGameplay();

        void UpdateTerritoty();
        void DrawTerritoty();

        void HandleAITurn();

        void HandleInput();

        void DrawPageTitle(const char* title);

    public:
        Game();
        ~Game();

        void Update(); 
        void Draw();  

        bool ShouldClose() const { return currentScreen == EXIT; }
};
