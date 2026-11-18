#pragma once
#include <vector>
#include "raylib.h"
#include "types.h"

enum class BoardSkinType {
    ClassicWood = 0,
    Hardwood = 1,
    DarkWood = 2
};

enum class StoneSkinType {
    Classic = 0,
    FlatMinimal = 1,
    PureSolid = 2
};

class Board {
    private:
        int x;
        int y;
        float margin;
        float cell_size;
        
        std::vector<Texture2D> boardTextures;
        std::vector<Texture2D> blackStoneTextures;
        std::vector<Texture2D> whiteStoneTextures;
        bool isLoaded = false; 

        BoardSkinType currentBoardSkin = BoardSkinType::ClassicWood;
        StoneSkinType currentStoneSkin = StoneSkinType::Classic;
        
    public:
        Board(); 
        ~Board(); 

        Board(const Board&) = delete;
        Board& operator=(const Board&) = delete;

        void LoadAllAssets();
        void UnloadAssets();
        
        void SetBoardSkin(BoardSkinType skin) { currentBoardSkin = skin; }
        void SetStoneSkin(StoneSkinType skin) { currentStoneSkin = skin; }

        BoardSkinType GetBoardSkin() const { return currentBoardSkin; }
        StoneSkinType GetStoneSkin() const { return currentStoneSkin; }

        Texture2D GetBoardTexture(BoardSkinType skin) const;
        Texture2D GetBlackStoneTexture(StoneSkinType skin) const;
        Texture2D GetWhiteStoneTexture(StoneSkinType skin) const;

        void Draw(int sz); 
        void Draw(const std::vector<std::vector<Stone>>& grid);
        void Draw(const SessionSnapshot& state);

        void DrawGhostStone(Point pos, Stone player);
        void DrawGhostStone(int col, int row, Stone player);

        void DrawStone(Point pos, Stone player);
        void DrawStone(int col, int row, Stone player);

        void DrawLastPlayed(Move lastMove, Stone player);
        void DrawLastPlayed(int col, int row, Stone player);
        
        Point GetCellIndex(Vector2 mousePos, int boardSize = 19);
        Vector2 GetPixelPosition(Point pos);
        Vector2 GetPixelPosition(int col, int row);
};
