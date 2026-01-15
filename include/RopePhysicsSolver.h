#pragma once
#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "RopeNode.h"
#include "Rope.h"

class RopePhysicsSolver
{
	public:
	static Vector2 g;

	static float airDensity;
	static float dragCoef;
	static bool canDrag;


	static void RopePhysicsSolver::Accelerate(RopeNode& ropenode, Vector2 acceleration);

	static Rope SetupRope(Vector2 firstNodePos, bool isFirstNodeStatic, int nodeAmount, float RopeLengthForEachNode, float nodeRadious);
	static void UpdateRope(Rope& ropenodes, float deltatime);

	static std::vector<Rope> ExistingRopes;
	static void HandleRopes(std::vector<Rope>& ExistingRopes, Camera2D& mainCamera);
	static void MoveRopeNode(Rope& ropenodes, const Camera2D& mainCamera);


	private:
		RopePhysicsSolver() = default;
		~RopePhysicsSolver() = default;

		static void UpdateRopeNodesPositions(Rope& ropenodes, float deltaTime);
		static void ApplyForces(Rope& ropenodes);
		static void ApplyConstraints(Rope& ropenodes, float deltatime);
};

class RopeRenderer
{
public:
	RopeRenderer() = default;
	~RopeRenderer() = default;

	static void RenderRope(Rope& rope);

};

