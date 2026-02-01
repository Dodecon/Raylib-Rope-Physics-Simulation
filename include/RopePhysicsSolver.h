#pragma once
// Safety check for missing constants
#ifndef RL_TRIANGLE_STRIP
	#define RL_TRIANGLE_STRIP 0x0005
#endif
#include <vector>
#include <omp.h>
#include "raylib.h"
#include "raymath.h"
#include "RopeRenderer.h"
#include "Rope.h"
#include "PhysicsConfig.h"

class RopePhysicsSolver
{
public:

	Config& config;

	RopePhysicsSolver(Config& CFG) : config(CFG) {}
	~RopePhysicsSolver() = default;

	// add acceleration to nodes as a force
	void Accelerate(Rope& rope, int NodeIndex, const Vector2 acceleration);

	// set up the rope
	Rope& SetupRope(const Vector2 firstNodePos, const Vector2 initialAcceleration, bool isFirstNodeStatic, int nodeAmount, float RopeLengthForEachNode, float nodeRadius);
	//update a specific rope
	void UpdateRope(Rope& rope, Camera2D& camera, const int substeps, const int iterations, const double deltaTime);
	//update all ropes
	void HandleRopes(Camera2D& camera, const int substeps, const int iterations, const double deltaTime);

	//handle mouse interactions
	Vector2 FindNodeToMove(Rope& rope, Camera2D& mainCamera);
	void ToggleAnchor(Rope& rope);
	void MoveRopeNode(Rope& rope, const Camera2D& mainCamera, const int substeps, const int i, const Vector2 dragStartFramePos);


private:

	void UpdateRopeNodesPositions(Rope& rope, const double deltaTime);
	void ApplyForces(Rope& rope);
	void ApplyConstraints(Rope& rope, const int iterations);
};
