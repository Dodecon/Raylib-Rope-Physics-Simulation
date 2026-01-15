#include "GUI_Renderer.h"
#include "RopePhysicsSolver.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h" 


void GUI_Renderer::Render_GUI() {

    RenderPanel(0.73, 0.05, 0.25, 0.9); //create he Toolbox panel at relative to the screen position
}


Vector2 GUI_Renderer::RelativeToScreen(Vector2 RelativePos) {
    // Clamp relative values to fit the screen
    RelativePos.x = Clamp(RelativePos.x, 0, 1);
    RelativePos.y = Clamp(RelativePos.y, 0, 1);


    return {RelativePos.x * GetScreenWidth(), RelativePos.y * GetScreenHeight()};
}

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

// is the panel minimized or not
static bool isMinimized = false;

bool GUI_Renderer::RenderPanel(float xPos, float yPos, float length, float height) {

    Vector2 mousePos = GetMousePosition();

    // change font size based on the window height
    GuiSetStyle(DEFAULT, TEXT_SIZE, RelativeToScreen(Vector2{ 0,0.035 }).y);

    //setup and render the panel
    Rectangle PanelBounds = SetBoundsRelative(xPos, yPos, length, (!isMinimized? height : height / 10));
    GuiPanel(PanelBounds, "ToollBox");
        

    // prevent nodes from being dragged while the mouse is inside the panel
    if (CheckCollisionPointRec(mousePos, PanelBounds)) {
        RopePhysicsSolver::canDrag = false;
    }
    else {
        RopePhysicsSolver::canDrag = true;
    }

    // setup and render Close button
    Rectangle minimizeButtonBounds = SetBoundsRelative(0.92,0.001, 0.08,0, PanelBounds);
    //force the button to be square
    minimizeButtonBounds.height = minimizeButtonBounds.width;

    //minimize if the button was pressed
    if (GuiButton(minimizeButtonBounds, "-")) {
        isMinimized = !isMinimized;
    }

    if (!isMinimized) {

        //describing what the user can do
        GuiSetStyle(DEFAULT, TEXT_SIZE, RelativeToScreen(Vector2{ 0,0.04 }).y);
        Rectangle labelIsAnchored = SetBoundsRelative(0.13, 0.05, 0.8, 0.02, PanelBounds);
        GuiLabel(labelIsAnchored, "Select the node with LMB");

        Rectangle labelIsAnchored2 = SetBoundsRelative(0.21, 0.08, 0.7, 0.02, PanelBounds);
        GuiLabel(labelIsAnchored2, "press CONTROL to");

        Rectangle labelIsAnchored3 = SetBoundsRelative(0.13, 0.11, 0.9, 0.02, PanelBounds);
        GuiLabel(labelIsAnchored3, "Anchor or Free the node");
      //--------------------------------------------------------------------------------------------------------

        //change the font size to the rest of the panel
        GuiSetStyle(DEFAULT, TEXT_SIZE, RelativeToScreen(Vector2{ 0,0.035 }).y);

        //allow gravity changes via sliders and buttons to reset values
        Rectangle labelBounds = SetBoundsRelative(0.3, 0.18, 0.5, 0.02, PanelBounds);
        GuiLabel(labelBounds, "Scale Gravity X");


        static float* gravityX = &RopePhysicsSolver::g.x;
        Rectangle SliderBoundsX = SetBoundsRelative(0.36, 0.22, 0.3, 0.03, PanelBounds);
        GuiSlider(SliderBoundsX, "-9.81 * 100", "9.81 * 100", gravityX, -9.81 * 100 * 2, 9.81 * 100 * 2);

        Rectangle checkBoxZeroX = SetBoundsRelative(0.4, 0.27, 0.16, 0.05, PanelBounds);

        if (GuiButton(checkBoxZeroX, "zero")) {
            *gravityX = 0;
        }



        Rectangle labelBounds2 = SetBoundsRelative(0.3, 0.36, 0.5, 0.02, PanelBounds);
        GuiLabel(labelBounds2, "Scale Gravity Y");

        static float* gravityY = &RopePhysicsSolver::g.y;
        const float defaultGravityY = RopePhysicsSolver::g.y;
        Rectangle SliderBoundsY = SetBoundsRelative(0.36, 0.41, 0.3, 0.03, PanelBounds);
        GuiSlider(SliderBoundsY, "-9.81 * 100", "9.81 * 200", gravityY, -9.81 * 100 * 2, 9.81 * 200);

        Rectangle checkBoxZeroY = SetBoundsRelative(0.2, 0.46, 0.16, 0.05, PanelBounds);
        Rectangle checkBoxDefault = SetBoundsRelative(0.6, 0.46, 0.22, 0.05, PanelBounds);

        if (GuiButton(checkBoxZeroY, "zero")) {
            *gravityY = 0;
        }

        if (GuiButton(checkBoxDefault, "default")) {
            *gravityY = defaultGravityY;
        }
//-------------------------------------------------------------------------------------------------------------

        //allow air density changes
        Rectangle labelBounds3 = SetBoundsRelative(0.25, 0.56, 0.5, 0.02, PanelBounds);
        GuiLabel(labelBounds3, "Change Air Density");

        static float* airDesity = &RopePhysicsSolver::airDensity;
        const float defaultAirDesity = RopePhysicsSolver::airDensity;
        Rectangle airDensitySliderBounds = SetBoundsRelative(0.12, 0.6, 0.7, 0.03, PanelBounds);
        GuiSlider(airDensitySliderBounds, "0", "0.0001", airDesity, 0, 0.0001);

        Rectangle checkBoxDefaultAirDensity = SetBoundsRelative(0.4, 0.65, 0.22, 0.05, PanelBounds);

        if (GuiButton(checkBoxDefaultAirDensity, "default")) {
            *airDesity = defaultAirDesity;
        }
    }

    return 1;
}