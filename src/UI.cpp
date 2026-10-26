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

void Button::LoadAssets(const char *path, const char *path_hovered) {
    texture = LoadTexture(path);
    texture_hovered = LoadTexture(path_hovered);
}

void Button::LoadFonts(const char *path) {
    font = LoadFontEx(path, 64, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

void Button::UnloadAssets() {
    UnloadTexture(texture);
    UnloadTexture(texture_hovered);
    UnloadFont(font);
}

void Button::SetPosition(float x, float y) {
    bounds = {x, y, (float)texture.width, (float)texture.height};
}

void Button::SetText(const char *s) {
    text = s;
}

std::pair<float, float> Button::GetSize() {
    return {texture.width, texture.height};
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
    if (IsHovered(GetMousePosition())) {
        DrawTexture(texture_hovered, bounds.x, bounds.y, WHITE);
    }
    else DrawTexture(texture, bounds.x, bounds.y, WHITE);
    Vector2 texSize = MeasureTextEx(font, text, 50.0f, 3.0f);
    DrawTextEx(font, text, {bounds.x + bounds.width / 2.0f - texSize.x / 2.0f, bounds.y + bounds.height / 2.0f - texSize.y / 2.0f}, 50.0f, 3.0f, BLACK);
}

