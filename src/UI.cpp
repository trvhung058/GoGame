#include "UI.h"

UI::UI() {

}

UI::~UI() {
    UnloadAssets();
}

void UI::LoadAssets(const char *path) {
    tex = LoadTexture(path);
}

void UI::UnloadAssets() {
    UnloadTexture(tex);
}

void UI::SetPosition(int pixelX, int pixelY) {
    x = pixelX;
    y = pixelY;
}

void UI::Draw(bool blur = false) {
    DrawTexture(tex, x, y, WHITE);
    if (blur) DrawRectangle(x, y, tex.width, tex.height, Fade(BLACK, 0.4f));
}

