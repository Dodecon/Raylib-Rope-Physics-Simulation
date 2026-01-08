#pragma once
#include <vector>
#include "raylib.h"
#include "raymath.h"
#include "RopeNode.h"
#include "Vector2_Operators.h"
 class RopePhysicsSolver
{
	public:

	static std::vector<RopeNode> SetupRope(Vector2 firstNodePos, bool isFirstNodeStatic, int nodeAmount, float RopeLengthForEachNode, float nodeRadious);
	static void UpdateRope(std::vector<RopeNode>& ropenodes, float deltatime);
	static std::vector<std::vector<RopeNode>> ExistingRopes;


	private:
		static const float g;

		static void UpdateRopeNodesPositions(std::vector<RopeNode>& ropenodes, float deltaTime);
		static void ApplyForces(std::vector<RopeNode>& ropenodes);
		static void ApplyConstraints(std::vector<RopeNode>& ropenodes, float deltatime);
		static void RenderNodes(std::vector<RopeNode>& ropenodes);
};

