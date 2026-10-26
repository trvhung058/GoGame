#pragma once
#include "raylib.h"
#include <string>

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

};

class Button {
    private:
        Rectangle bounds;
        std::string text;
        Texture2D texture;

    public:
        void LoadAssets(const char *path);
        void UnloadAssets();
        bool IsClicked(Vector2 mousePos);
        bool IsHovered(Vector2 mousePos);
        void Draw();
};