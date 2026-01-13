#include "GUI_Renderer.h"
#include "RopePhysicsSolver.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h" 

bool GUI_Renderer::isMinimized = false;

void GUI_Renderer::Render_GUI() {

    RenderPanel(0.73, 0.05, 0.25, 0.9);
}


Vector2 GUI_Renderer::RelativeToScreen(Vector2 RelativePos) {
    // Clamp relative values to fit the screen
    RelativePos.x = Clamp(RelativePos.x, 0, 1);
    RelativePos.y = Clamp(RelativePos.y, 0, 1);


    return {RelativePos.x * GetScreenWidth(), RelativePos.y * GetScreenHeight()};
}

// Calculate relative position by linearly interpolating between Parent Bounds (the screen by default)
Rectangle GUI_Renderer::SetBoundsRelative(float xPos, float yPos, float length, float height, Rectangle Parent) {   //all params are relative, except Parent

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
    GuiSetStyle(DEFAULT, TEXT_SIZE, RelativeToScreen(Vector2{ 0,0.035 }).y);

    Rectangle PanelBounds = SetBoundsRelative(xPos, yPos, length, (!isMinimized? height : height / 10));
    GuiPanel(PanelBounds, "ToollBox");
        

    Rectangle closeButtonBounds = SetBoundsRelative(0.92,0.001, 0.08,0, PanelBounds);
    closeButtonBounds.height = closeButtonBounds.width;
    if (GuiButton(closeButtonBounds, "-")) {
        isMinimized = !isMinimized;
    }

    if (!isMinimized) {
        Rectangle labelBounds = SetBoundsRelative(0.3, 0.05, 0.5, 0.02, PanelBounds);
        GuiLabel(labelBounds, "Scale Gravity X");


        static float* gravityX = &RopePhysicsSolver::g.x;
        Rectangle SliderBoundsX = SetBoundsRelative(0.36, 0.09, 0.3, 0.03, PanelBounds);
        GuiSlider(SliderBoundsX, "-9.81 * 100", "9.81 * 100", gravityX, -9.81 * 100 * 2, 9.81 * 100 * 2);

        Rectangle checkBoxZeroX = SetBoundsRelative(0.4, 0.14, 0.16, 0.05, PanelBounds);

        if (GuiButton(checkBoxZeroX, "zero")) {
            *gravityX = 0;
        }



        Rectangle labelBounds2 = SetBoundsRelative(0.3, 0.23, 0.5, 0.02, PanelBounds);
        GuiLabel(labelBounds2, "Scale Gravity Y");

        static float* gravityY = &RopePhysicsSolver::g.y;
        Rectangle SliderBoundsY = SetBoundsRelative(0.36, 0.28, 0.3, 0.03, PanelBounds);
        GuiSlider(SliderBoundsY, "-9.81 * 100", "9.81 * 200", gravityY, -9.81 * 100 * 2, 9.81 * 200);

        Rectangle checkBoxZeroY = SetBoundsRelative(0.2, 0.33, 0.16, 0.05, PanelBounds);
        Rectangle checkBoxDefault = SetBoundsRelative(0.6, 0.33, 0.25, 0.05, PanelBounds);

        if (GuiButton(checkBoxZeroY, "zero")) {
            *gravityY = 0;
        }

        if (GuiButton(checkBoxDefault, "default")) {
            *gravityY = 9.81 * 100;
        }
    }

    return 1;
}