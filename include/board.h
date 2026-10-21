#pragma once
#include <utility> 
#include "raylib.h"

class Board {
    private:
        int x;
        int y;
        int board[19][19]; 
        float margin;
        float cell_size;
        
        Texture2D texBoard;
        Texture2D texBlackStone;
        Texture2D texWhiteStone;
        bool isLoaded = false; 
        
    public:
        Board(); 
        ~Board(); 

        void LoadAssets(const char *boardPath, const char *blackPath, const char *whitePath);
        void UnloadAssets();
        void Draw(); 
        void DrawGhostStone(int row, int col, int currentPlayer);
        void DrawStone(int row, int col, int currentPlayer);
        
        std::pair<int, int> GetCellIndex(Vector2 MousePos);
};