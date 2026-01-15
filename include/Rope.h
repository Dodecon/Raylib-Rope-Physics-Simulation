#pragma once
#include <vector>
#include "RopeNode.h"
struct Rope
{
	public:
		Rope() = default;
		Rope(const std::vector<RopeNode>& _rope) : nodes(_rope) {}
		~Rope() = default;

		std::vector<RopeNode> nodes;
};