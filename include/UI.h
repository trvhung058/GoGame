#pragma once
#include "raylib.h"
#include <string>
#include <utility>

class UI {
    private:
        int x;
        int y;
        Texture2D tex;

    public:
        UI();
        ~UI();

        void LoadAssets(const char *path);
        void UnloadAssets();
        void SetPosition(int pixelX, int pixelY);
        void Draw(bool blur);
        std::pair<int, int> GetSize();
};

class Button {
    private:
        Rectangle bounds;
        const char *text;
        Font font;
        Texture2D texture;
        Texture2D texture_hovered;

    public:
        void LoadAssets(const char *path, const char *path_hovered);
        void LoadFonts(const char *path);
        void UnloadAssets();
        void SetPosition(float x, float y);
        void SetText(const char *s);
        std::pair<float, float> GetSize();
        bool IsClicked(Vector2 mousePos);
        bool IsHovered(Vector2 mousePos);
        void Draw();
};
