#include "GUI_Renderer.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h" 

bool GUI_Renderer::isMinimized = false;

void GUI_Renderer::Render_GUI() {

    RenderPanel(0.79, 0.05, 0.2, 0.8);
}


Vector2 GUI_Renderer::RelativeToScreen(Vector2 RelativePos, Rectangle ParentBounds) {    //convert relative vec2 (range: 0-1) to a screen position vec2

    RelativePos.x = Clamp(RelativePos.x, 0, 1);
    RelativePos.y = Clamp(RelativePos.y, 0, 1);

    return {RelativePos.x * GetScreenWidth(), RelativePos.y * GetScreenHeight()};
}

Rectangle GUI_Renderer::SetBoundsRelative(float xPos, float yPos, float length, float height, Rectangle ParentBounds) {   // pass relative bounds (range: 0-1) and get screen position bounds

    Vector2 pos =  RelativeToScreen({xPos, yPos});
    Vector2 size = RelativeToScreen({length, height});

    return { pos.x, pos.y, size.x, size.y };
}


bool GUI_Renderer::RenderPanel(float xPos, float yPos, float length, float height) {
    GuiSetStyle(DEFAULT, TEXT_SIZE, RelativeToScreen(Vector2{ 0,0.025 }).y);

    Rectangle PanelBounds = SetBoundsRelative(xPos, yPos, length, height);

    GuiPanel(PanelBounds, "ToollBox");

    return 1;
}