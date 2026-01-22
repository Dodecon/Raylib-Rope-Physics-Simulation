#pragma once
#include "raylib.h"
#include "Rope.h"

struct PhysicsConfig
{
    // physics variables
    Vector2 g;
    float airDensity;
    float dragCoef;

    // variables to keep state between frames for rope interaction
    RopeNode* draggedNode = nullptr;
    bool wasAnchored = false;
    bool canDrag;

    std::vector<Rope> ExistingRopes;

    // 2. Default Constructor
    // It calls the other constructor below, passing in the default values you want.
    PhysicsConfig()
        : PhysicsConfig(Vector2{ 0, 9.81 * 100 }, 0.00002f, 0.47f, true)
    {
    }

    // 3. Parameterized Constructor
    PhysicsConfig(Vector2 G, float AirDensity, float DragCoef, bool CanDrag)
        : g(G),
        airDensity(AirDensity),
        dragCoef(DragCoef),
        canDrag(CanDrag)
    {
    }
};