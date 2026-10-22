#pragma once
#include "raylib.h"

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