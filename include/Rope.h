#pragma once
#include <vector>
#include "RopeNode.h"

// simply stores all nodes and other data of a rope
struct Rope
{
	public:
		int startNodeIndex;
		int nodeAmount;

		float Radius;
		float RopeLengthForEach;

		Rope(int _nodeAmount, float _radius, float _ropeLength) :

			nodeAmount(_nodeAmount),
			Radius(_radius),
			RopeLengthForEach(_ropeLength)
		{
			startNodeIndex = 0;
		};

		Rope(const std::vector<RopeNode>& _rope, float _radius, float _ropeLength) :

			nodeAmount(_rope.size()),
			Radius(_radius),
			RopeLengthForEach(_ropeLength)
		{
			startNodeIndex = 0;
		};

		~Rope() = default;
};