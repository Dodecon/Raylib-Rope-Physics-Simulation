#pragma once
#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "RopeNode.h"

class RopePhysicsSolver
{
	public:
	static Vector2 g;

	static float airDensity;
	static float dragCoef;

	static std::vector<RopeNode> SetupRope(Vector2 firstNodePos, bool isFirstNodeStatic, int nodeAmount, float RopeLengthForEachNode, float nodeRadious);
	static void UpdateRope(std::vector<RopeNode>& ropenodes, float deltatime);

	static std::vector<std::vector<RopeNode>> ExistingRopes;
	static void HandleRopes(Camera2D& mainCamera, std::vector<std::vector<RopeNode>>& ExistingRopes);
	static void MoveRopeNode(std::vector<RopeNode>& ropenodes, const Camera2D& mainCamera);


	private:
		RopePhysicsSolver() = default;

		static void UpdateRopeNodesPositions(std::vector<RopeNode>& ropenodes, float deltaTime);
		static void ApplyForces(std::vector<RopeNode>& ropenodes);
		static void ApplyConstraints(std::vector<RopeNode>& ropenodes, float deltatime);
		static void RenderNodes(std::vector<RopeNode>& ropenodes);
};

