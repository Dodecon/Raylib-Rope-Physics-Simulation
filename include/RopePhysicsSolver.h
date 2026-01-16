#pragma once
#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "RopeNode.h"
#include "Rope.h"
#include "PhysicsConfig.h"

class RopePhysicsSolver
{
	public:

		PhysicsConfig& config;

		RopePhysicsSolver(PhysicsConfig& CFG) : config(CFG) {}
		~RopePhysicsSolver() = default;

	 void Accelerate(RopeNode& ropenode, Vector2 acceleration);

	 Rope SetupRope(Vector2 firstNodePos, bool isFirstNodeStatic, int nodeAmount, float RopeLengthForEachNode, float nodeRadious);
	 void UpdateRope(Rope& ropenodes, float deltatime);

	 void HandleRopes(Camera2D& mainCamera);
	 void MoveRopeNode(Rope& ropenodes, const Camera2D& mainCamera);


	private:

		 void UpdateRopeNodesPositions(Rope& ropenodes, float deltaTime);
		 void ApplyForces(Rope& ropenodes);
		 void ApplyConstraints(Rope& ropenodes, float deltatime);
};

class RopeRenderer
{
public:
	RopeRenderer() = default;
	~RopeRenderer() = default;

	static void RenderRope(Rope& rope);

};

