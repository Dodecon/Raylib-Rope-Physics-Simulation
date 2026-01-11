#pragma once
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"

class GUI_Renderer
{
public:
    // Static method to render all of the UI
    static void Render_GUI();

private:
    GUI_Renderer() = default;

    static bool isMinimized;


    /// <summary>
    /// convert relative vec2 (range: 0-1) to a screen position vec2
    /// </summary>
    static Vector2 RelativeToScreen(Vector2 RelativePos);

    /// <summary>
    /// pass relative bounds (range: 0-1) and a Parent (pixel coords) and get screen position bounds
    /// </summary>
    static Rectangle SetBoundsRelative(float xPos, float yPos, float length, float height, Rectangle Parent = { 0,0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) });

    static bool RenderPanel(float xPos, float yPos, float length, float height);
};