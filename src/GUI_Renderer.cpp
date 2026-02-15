#include "GUI_Renderer.h"
#include "RopePhysicsSolver.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h" 


void GUI_Renderer::Render_GUI() {
    // draw fps at the top left corner of the screen
    DrawFPS(RelativeToScreen({ 0.02f, 0 }).x, RelativeToScreen({ 0, 0.01 }).y);

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


void GUI_Renderer::DrawFixedBackground(Texture2D background, int screenWidth, int screenHeight) {
    // Calculate scale to fill screen while maintaining aspect ratio
    float scaleX = (float)screenWidth / background.width;
    float scaleY = (float)screenHeight / background.height;
    float scale = (scaleX > scaleY) ? scaleX : scaleY; // Use larger scale to cover entire screen

    // Center the background on screen
    Vector2 position = {
        (screenWidth - (background.width * scale)) / 2.0f,
        (screenHeight - (background.height * scale)) / 2.0f
    };

    DrawTextureEx(background, position, 0.0f, scale, WHITE);
}

// is the panel minimized or not

void GUI_Renderer::RenderPanel(float xPos, float yPos, float length, float height) {

    Vector2 mousePos = GetMousePosition();

    // change font size based on the window height
    GuiSetStyle(DEFAULT, TEXT_SIZE, std::min(RelativeToScreen(Vector2{ 0.0185, 0 }).x, RelativeToScreen(Vector2{ 0,0.035 }).y));

    //setup and render the panel
    Rectangle PanelBounds = SetBoundsRelative(xPos, yPos, length, (!isMinimized? height : height / 10));
    GuiPanel(PanelBounds, "ToollBox");
        

    // prevent nodes from being dragged while the mouse is inside the panel
    if (CheckCollisionPointRec(mousePos, PanelBounds)) {
        config.interaction.canDrag = false;
    }
    else {
        config.interaction.canDrag = true;
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
        GuiSetStyle(DEFAULT, TEXT_SIZE, std::min(RelativeToScreen(Vector2{ 0.022, 0 }).x, RelativeToScreen(Vector2{ 0,0.044 }).y));
        Rectangle labelIsAnchored = SetBoundsRelative(0.13, 0.07, 0.8, 0.02, PanelBounds);
        GuiLabel(labelIsAnchored, "Select the node with LMB");

        Rectangle labelIsAnchored2 = SetBoundsRelative(0.21, 0.1, 0.7, 0.02, PanelBounds);
        GuiLabel(labelIsAnchored2, "press CONTROL to");

        Rectangle labelIsAnchored3 = SetBoundsRelative(0.13, 0.13, 0.9, 0.02, PanelBounds);
        GuiLabel(labelIsAnchored3, "Anchor or Free the node");
        //--------------------------------------------------------------------------------------------------------

          //change the font size back to the rest of the panel
        GuiSetStyle(DEFAULT, TEXT_SIZE, std::min(RelativeToScreen(Vector2{ 0.0185, 0 }).x, RelativeToScreen(Vector2{ 0,0.035 }).y));

        //allow gravity X changes via sliders and buttons to reset values
        Rectangle labelBounds = SetBoundsRelative(0.3, 0.18, 0.5, 0.02, PanelBounds);
        GuiLabel(labelBounds, "Scale Gravity X");

        //get the gravity X from config
        static float* gravityX = &config.physics.g.x;
        //change gravity X via slider
        Rectangle SliderBoundsX = SetBoundsRelative(0.36, 0.22, 0.3, 0.03, PanelBounds);
        GuiSlider(SliderBoundsX, "-9.81 * 200", "9.81 * 200", gravityX, -9.81 * 100 * 2, 9.81 * 100 * 2);

        //checkbo to set the gravity to zero
        Rectangle checkBoxZeroX = SetBoundsRelative(0.4, 0.27, 0.16, 0.05, PanelBounds);

        if (GuiButton(checkBoxZeroX, "zero")) {
            *gravityX = 0;
        }


        ////allow gravity Y changes via sliders and buttons to reset values
        Rectangle labelBounds2 = SetBoundsRelative(0.3, 0.36, 0.5, 0.02, PanelBounds);
        GuiLabel(labelBounds2, "Scale Gravity Y");

        //get the gravity X from config
        static float* gravityY = &config.physics.g.y;
        const static float defaultGravityY = config.physics.g.y;
        //change gravity Y via slider
        Rectangle SliderBoundsY = SetBoundsRelative(0.36, 0.41, 0.3, 0.03, PanelBounds);
        GuiSlider(SliderBoundsY, "-9.81 * 200", "9.81 * 200", gravityY, -defaultGravityY * 2, defaultGravityY * 2);

        //checkbo to set the gravity to zero and the default value
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

        //get air density from config
        static float* airDesity = &config.physics.airDensity;
        const static float defaultAirDesity = config.physics.airDensity;

        //slider to change density
        Rectangle airDensitySliderBounds = SetBoundsRelative(0.12, 0.6, 0.7, 0.03, PanelBounds);
        GuiSlider(airDensitySliderBounds, "0", "0.0003", airDesity, 0, 0.0003);

        //checkBox to check the style
        Rectangle checkBoxDefaultAirDensity = SetBoundsRelative(0.4, 0.65, 0.22, 0.05, PanelBounds);

        if (GuiButton(checkBoxDefaultAirDensity, "default")) {
            *airDesity = defaultAirDesity;
        }


        //----------------------------------------------------------------------------------------------------------------

            // change FPS mid simulation and switch an ability for the rope to be able to compress onto itself

        //dont allow interactions with other buttons if one is currently pressed
        static bool wasButtonPressed = false;

        //create a value box
        static bool FPSeditMode = false;
        static int targetFPS = config.TargetFPS;
        Rectangle ChangeFPSBounds = SetBoundsRelative(0.3, 0.71, 0.15, 0.05, PanelBounds);

        if (GuiValueBox(ChangeFPSBounds, "Target FPS", &targetFPS, 1, 999, FPSeditMode) && wasButtonPressed == false) {

            FPSeditMode = true;
            wasButtonPressed = true;
            targetFPS < 10 ? targetFPS : 1;
        }
        if (IsKeyPressed(KEY_ENTER) && FPSeditMode == true) 
        {
            //pass the new target fps into config and change target fps
            FPSeditMode = false;
            wasButtonPressed = false;
            config.TargetFPS = targetFPS;
            SetTargetFPS(config.TargetFPS);
        }


        static bool SwitchCompressionEditMode = false;
        static int NewRigid = false;
        Rectangle NewRigidBounds = SetBoundsRelative(0.8, 0.71, 0.15, 0.05, PanelBounds);

        if (GuiValueBox(NewRigidBounds, "Rigid ropes ", &NewRigid, 0, 1, SwitchCompressionEditMode) && wasButtonPressed == false) {

            SwitchCompressionEditMode = true;
            wasButtonPressed = true;
        }
        // only change the value after enter is pressed
        if (IsKeyPressed(KEY_ENTER) && SwitchCompressionEditMode == true) {
            SwitchCompressionEditMode = false;
            wasButtonPressed = false;

            config.physics.areRopesRigid = (bool)NewRigid;
        }


        //-----------------------------------------------------------------------------------------------------------------

        // Create a new rope

        // X position for the new rope
        static bool PosXeditMode = false;
        static int NewPosX = 0;
        Rectangle NewRopeX = SetBoundsRelative(0.57, 0.77, 0.15, 0.05, PanelBounds);

        if (GuiValueBox(NewRopeX, "X ", &NewPosX, -10000, 10000, PosXeditMode) && wasButtonPressed == false) {

            PosXeditMode = true;
            wasButtonPressed = true;
        }
        // only change the value after enter is pressed
        if (IsKeyPressed(KEY_ENTER) && PosXeditMode == true) {
            PosXeditMode = false;
            wasButtonPressed = false;
        }


        // Y position for the new rope
        static bool PosYeditMode = false;
        static int NewPosY = 0;
        Rectangle NewRopeY = SetBoundsRelative(0.8, 0.77, 0.15, 0.05, PanelBounds);

        if (GuiValueBox(NewRopeY, "Y ", &NewPosY, -10000, 10000, PosYeditMode) && wasButtonPressed == false) {

            PosYeditMode = true;
            wasButtonPressed = true;
        }

        if (IsKeyPressed(KEY_ENTER) && PosYeditMode == true) {
            PosYeditMode = false;
            wasButtonPressed = false;
        }



        static bool NodesEditMode = false;
        static int NewNodesAmount = 1;
        Rectangle NodeAmount = SetBoundsRelative(0.34, 0.77, 0.15, 0.05, PanelBounds);

        if (GuiValueBox(NodeAmount, "Node amount ", &NewNodesAmount, 1, 100000, NodesEditMode) && wasButtonPressed == false) {

            NodesEditMode = true;
            wasButtonPressed = true;
        }

        if (IsKeyPressed(KEY_ENTER) && NodesEditMode == true) {
            NodesEditMode = false;
            wasButtonPressed = false;
        }



        static bool NodesLengthEditMode = false;
        static int NewNodesLength = 1;
        Rectangle NodeLength = SetBoundsRelative(0.46, 0.83, 0.15, 0.05, PanelBounds);

        if (GuiValueBox(NodeLength, "Connection Length ", &NewNodesLength, 1, 100000, NodesLengthEditMode) && wasButtonPressed == false) {

            NodesLengthEditMode = true;
            wasButtonPressed = true;
        }

        if (IsKeyPressed(KEY_ENTER) && NodesLengthEditMode == true) {

            NodesLengthEditMode = false;
            wasButtonPressed = false;
        }



        static bool NodesRadiusEditMode = false;
        static int NewNodesRadius = 1;
        Rectangle NodeRadius = SetBoundsRelative(0.31, 0.89, 0.15, 0.05, PanelBounds);

        if (GuiValueBox(NodeRadius, "Node radius ", &NewNodesRadius, 1, 100000, NodesRadiusEditMode) && wasButtonPressed == false) {

            NodesRadiusEditMode = true;
            wasButtonPressed = true;
        }

        if (IsKeyPressed(KEY_ENTER) && NodesRadiusEditMode == true) {

            NodesRadiusEditMode = false;
            wasButtonPressed = false;
        }


        Rectangle createNewRope = SetBoundsRelative(0.55, 0.9, 0.4, 0.05, PanelBounds);

        if (GuiButton(createNewRope, "create Rope")) {

            Solver.SetupRope({ (float)NewPosX, (float)NewPosY }, true, NewNodesAmount, NewNodesLength, NewNodesRadius);
        }

    }
}