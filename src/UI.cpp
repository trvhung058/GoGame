#include "UI.h"
#include <cmath>

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
    font = LoadFontEx(path, 96, 0, 0);
    SetTextureFilter(font.texture, TEXTURE_FILTER_BILINEAR);
}

void Button::UnloadAssets() {
    UnloadTexture(texture);
    UnloadTexture(texture_hovered);
    if (font.texture.id != 0) {
        UnloadFont(font);
    }
}

void Button::SetPosition(float x, float y) {
    bounds = {x, y, (float)texture.width, (float)texture.height};
}

void Button::SetText(const char *s) {
    text = s;
}

std::pair<float, float> Button::GetSize() {
    return {
        static_cast<float>(texture.width),
        static_cast<float>(texture.height)
    };
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
        DrawTexture(texture_hovered, (int)bounds.x, (int)bounds.y, WHITE);
    }
    else {
        DrawTexture(texture, (int)bounds.x, (int)bounds.y, WHITE);
    }

    if (text == nullptr || text[0] == '\0') return;

    Font currentFont = (font.texture.id != 0) ? font : GetFontDefault();
    float fontSize = 48.0f;
    float spacing = 2.0f;

    Vector2 texSize = MeasureTextEx(currentFont, text, fontSize, spacing);
    if (texSize.x > bounds.width - 30.0f) {
        fontSize = std::floor(fontSize * ((bounds.width - 30.0f) / texSize.x));
        spacing = 1.0f;
        texSize = MeasureTextEx(currentFont, text, fontSize, spacing);
    }

    float textX = std::round(bounds.x + (bounds.width - texSize.x) / 2.0f);
    float textY = std::round(bounds.y + (bounds.height - texSize.y) / 2.0f);

    DrawTextEx(currentFont, text, {textX, textY + 2.0f}, fontSize, spacing, Color{0, 0, 0, 140});

    Color mainColor = IsHovered(GetMousePosition()) ? Color{10, 10, 10, 255} : Color{30, 20, 15, 255};
    DrawTextEx(currentFont, text, {textX, textY}, fontSize, spacing, mainColor);
}
