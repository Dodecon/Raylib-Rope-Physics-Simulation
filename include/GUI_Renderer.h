#pragma once
#include "raylib.h"
#include "raygui.h"
#include "raymath.h"
#include "RopePhysicsSolver.h"
#include "PhysicsConfig.h"

class GUI_Renderer
{
public:
/// <summary>
/// render all elements
/// </summary>
    void Render_GUI();
    void DrawFixedBackground(Texture2D background, int screenWidth, int screenHeight);

    Config& config;
    RopePhysicsSolver& Solver;
    bool isMinimized = false;

    GUI_Renderer(RopePhysicsSolver& SLV, Config& CFG) : Solver(SLV), config(CFG) {};   //constructor
private:

/// <summary>
/// convert relative vec2 (range 0-1) to a pixel position vec2
/// </summary>
    Vector2 RelativeToScreen(Vector2 RelativePos);

/// <summary>
/// pass relative bounds (range 0-1) and a Parent (pixel coords) and get pixel coord bounds
/// </summary>
    Rectangle SetBoundsRelative(float xPos, float yPos, float length, float height, Rectangle Parent = { 0,0, static_cast<float>(GetScreenWidth()), static_cast<float>(GetScreenHeight()) });

/// <summary>
/// render the ToolBox Panel
/// </summary>
    void RenderPanel(float xPos, float yPos, float length, float height);
};