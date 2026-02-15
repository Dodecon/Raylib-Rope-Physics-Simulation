#pragma once
#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "RopeNode.h"
#include "Rope.h"
#include "RopeRenderer.h"
#include "PhysicsConfig.h"
#include "Threadpool.h"

class RopePhysicsSolver
{
public:

	// vector to store all nodes of all ropes in one buffer
	// and a vector that stores an index where a specific ropes starts at AllNodes and other thats needed on a per-rope basis
	std::vector<Rope> AllRopes;
	std::vector<RopeNode> AllNodes;

	// config to get the physics and interaction data from
	Config& config;
	// a threadpool used for multithreading
	Threadpool& threadpool;

	RopePhysicsSolver(Config& CFG, Threadpool& tp) : config(CFG), threadpool(tp) {}
	~RopePhysicsSolver() = default;

	// add acceleration to nodes as a force
	void Accelerate(RopeNode& ropenode, const Vector2 acceleration);

	// set up the rope
	Rope& SetupRope(const Vector2 firstNodePos, bool isFirstNodeStatic, int nodeAmount, float RopeLengthForEachNode, float nodeRadius);
	//update a specific rope
	void UpdateRopes(Camera2D& camera, const int substeps, const int iterations, const double deltaTime);
	//update all ropes
	void HandleRopes(Camera2D& camera, const int substeps, const int iterations, const double deltaTime);

	//handle mouse interactions
	Vector2 FindNodeToMove(Rope& rope, Camera2D& mainCamera);
	void ToggleAnchor(Rope& rope);
	void MoveRopeNode(Rope& rope, const Camera2D& mainCamera, const int substeps, const int i, const Vector2 dragStartFramePos);


private:

	void UpdateRopeNodePosition(RopeNode& node, Rope& rope, const double deltaTime);
	void ApplyForces(RopeNode& node);
	void ApplyConstraints(std::vector<RopeNode>& nodes, Rope& rope, const int iterations);
};

