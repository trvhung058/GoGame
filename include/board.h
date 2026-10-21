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
        bool isLoaded = false; 
        
    public:
        Board(); 
        ~Board(); 

        void LoadAssets(const char *path);
        void UnloadAssets();
        void Draw(); 
        
        std::pair<int, int> GetCellIndex(Vector2 MousePos);
};