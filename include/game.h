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

        enum Scene {
            MENU,
            PVP,
            PVE,
            EXIT
        } currentScreen;

        void UpdateMenu();
        void DrawMenu();

        void UpdateGameplay();
        void DrawGameplay();

        void HandleInput();

    public:
        Game();

        void Update(); 
        void Draw();  

        bool ShouldClose() const { return currentScreen == EXIT; }
};