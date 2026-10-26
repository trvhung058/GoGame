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

void Button::LoadAssets(const char *path) {
    texture = LoadTexture(path);
    bounds = {600.0f, 600.0f, (float)texture.width, (float)texture.height};
}

void Button::UnloadAssets(){
    UnloadTexture(texture);
}

bool Button::IsClicked(Vector2 Mouse) {
    if (IsHovered(Mouse) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        return true;
    } 
    return false;
}

bool Button::IsHovered(Vector2 Mouse) {
    if (CheckCollisionPointRec(Mouse, bounds)) {
        return true;
    }
    return false;
}

void Button::Draw() {
    DrawTexture(texture, bounds.x, bounds.y, WHITE);
}

