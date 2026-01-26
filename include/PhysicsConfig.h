#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include "Rope.h"

struct PhysicsConfig
{
    // physics variables
    Vector2 g;
    float airDensity;
    float dragCoef;
    bool areRopesRigid; //control of ropes can fold onto themselves


    std::vector<Rope> ExistingRopes;

    // 2. Default Constructor
    // It calls the other constructor below, passing in the default values you want.
    PhysicsConfig()
        : PhysicsConfig(Vector2{ 0, 9.81 * 100 }, 0.00002f, 0.47f, false)
    {}

    // 3. Parameterized Constructor
    PhysicsConfig(Vector2 G, float AirDensity, float DragCoef, bool AreRopesRigid)
        : g(G),
        airDensity(AirDensity),
        dragCoef(DragCoef),
        areRopesRigid(AreRopesRigid)
    {}
};


struct InteractionConfig
{
    // Variables to keep state between frames for rope interaction
    int draggedNodeID = -1;
    Rope* draggedRope = nullptr;
    bool wasAnchored = false;
    bool canDrag = true;

    // 1. Default Constructor
    // Note: Removed the semicolon and used lowercase 'true' 
    InteractionConfig()
        : InteractionConfig(true)
    {
    }

    // 2. Parameterized Constructor
    // Note: Parameter names (CanDrag) should match what you pass in
    InteractionConfig(bool CanDrag)
        : canDrag(CanDrag)
    {}
};

struct Config
{
    //other
    int TargetFPS;

    PhysicsConfig physics;
    InteractionConfig interaction;

    // Default Constructor
    Config()
        : Config(60, Vector2{0, 9.81f * 100 }, 0.00002f, 0.47f, false, false)
    {
    }

    // Parameterized Constructor
    // Matches the arguments of the sub-constructors
    Config(int targetFPS, Vector2 G, float AirDensity, float DragCoef, bool CanDrag, bool AreRopesRigid)

        : TargetFPS(targetFPS < 1? 1 : targetFPS), // make sure that target fps isnt less than 1
        physics(G, AirDensity, DragCoef, AreRopesRigid), // Call PhysicsConfig constructor
          interaction(CanDrag)// Call InteractionConfig constructor
    {
    }
};
