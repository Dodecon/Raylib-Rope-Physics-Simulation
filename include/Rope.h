#pragma once
#include <vector>
#include "RopeNode.h"

// simply stores all nodes of a rope in one object (and metadata in the future)
struct Rope
{
	public:
		Rope() = default;
		Rope(const std::vector<RopeNode>& _rope) : nodes(_rope) {}
		~Rope() = default;

		std::vector<RopeNode> nodes;
};