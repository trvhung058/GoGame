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

std::pair<int, int> UI::GetSize() {
    return {tex.width, tex.height};
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
    else {
        DrawTexture(texture, bounds.x, bounds.y, WHITE);
    }

    if (text == nullptr || text[0] == '\0') return;

    Font currentFont = (font.texture.id != 0) ? font : GetFontDefault();
    float fontSize = 50.0f;
    float spacing = 3.0f;

    Vector2 texSize = MeasureTextEx(currentFont, text, fontSize, spacing);
    if (texSize.x > bounds.width - 30.0f) {
        fontSize = fontSize * ((bounds.width - 30.0f) / texSize.x);
        spacing = 2.0f;
        texSize = MeasureTextEx(currentFont, text, fontSize, spacing);
    }

    Vector2 textPos = {
        bounds.x + (bounds.width - texSize.x) / 2.0f,
        bounds.y + (bounds.height - texSize.y) / 2.0f
    };

    DrawTextEx(currentFont, text, {textPos.x + 2.0f, textPos.y + 3.0f}, fontSize, spacing, Fade(BLACK, 0.45f));
    DrawTextEx(currentFont, text, {textPos.x + 1.0f, textPos.y + 1.5f}, fontSize, spacing, Fade(BLACK, 0.25f));

    DrawTextEx(currentFont, text, {textPos.x, textPos.y - 1.0f}, fontSize, spacing, Fade(WHITE, 0.20f));

    Color mainColor = IsHovered(GetMousePosition()) ? Color{15, 10, 5, 255} : Color{35, 25, 20, 255};
    DrawTextEx(currentFont, text, textPos, fontSize, spacing, mainColor);
}
