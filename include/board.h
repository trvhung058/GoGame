#pragma once
#include <vector>
#include "raylib.h"
#include "types.h"

class Board {
    private:
        int x;
        int y;
        float margin;
        float cell_size;
        
        Texture2D texBoard;
        Texture2D texBlackStone;
        Texture2D texWhiteStone;
        bool isLoaded = false; 
        
    public:
        Board(); 
        ~Board(); 

        Board(const Board&) = delete;
        Board& operator=(const Board&) = delete;

        void LoadAssets(const char *boardPath, const char *blackPath, const char *whitePath);
        void UnloadAssets();

        void Draw(); 
        void Draw(const std::vector<std::vector<Stone>>& grid);
        void Draw(const SessionSnapshot& state);

        void DrawGhostStone(Point pos, Stone player);
        void DrawGhostStone(int col, int row, Stone player);

        void DrawStone(Point pos, Stone player);
        void DrawStone(int col, int row, Stone player);

        void DrawLastPlayed(Point pos, Stone player);
        void DrawLastPlayed(int col, int row, Stone player);
        
        Point GetCellIndex(Vector2 mousePos);
        Vector2 GetPixelPosition(Point pos);
        Vector2 GetPixelPosition(int col, int row);
};
