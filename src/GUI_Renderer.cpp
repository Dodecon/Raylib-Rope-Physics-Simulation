#include "GUI_Renderer.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h" 

bool GUI_Renderer::isMinimized = false;

void GUI_Renderer::Render_GUI() {

    RenderPanel(0.69, 0.05, 0.3, 0.9);
}


Vector2 GUI_Renderer::RelativeToScreen(Vector2 RelativePos) {
    // Clamp relative values to fit the screen
    RelativePos.x = Clamp(RelativePos.x, 0, 1);
    RelativePos.y = Clamp(RelativePos.y, 0, 1);


    return {RelativePos.x * GetScreenWidth(), RelativePos.y * GetScreenHeight()};
}

// Calculate relative position by linearly interpolating between Parent Bounds (the screen by default)
Rectangle GUI_Renderer::SetBoundsRelative(float xPos, float yPos, float length, float height, Rectangle Parent) {   //all params are relative!

    Rectangle childRect;

    // 1. Position: Start at Parent.x and move 'xPos' percent of Parent.width
    childRect.x = Parent.x + (xPos * Parent.width);
    childRect.y = Parent.y + (yPos * Parent.height);

    // 2. Size: Simply a percentage of the Parent's dimensions
    childRect.width = length * Parent.width;
    childRect.height = height * Parent.height;

    return childRect;
}


bool GUI_Renderer::RenderPanel(float xPos, float yPos, float length, float height) {
    GuiSetStyle(DEFAULT, TEXT_SIZE, RelativeToScreen(Vector2{ 0,0.025 }).y);

    Rectangle PanelBounds = SetBoundsRelative(xPos, yPos, length, height);
    GuiPanel(PanelBounds, "ToollBox");

    static float value = 5;

    Rectangle SliderBounds = SetBoundsRelative(0.1, 0.2, 0.5, 0.1, PanelBounds);
    GuiSlider(SliderBounds, "left", "right", &value, 0, 10);

    return 1;
}