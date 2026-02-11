#pragma once
#include "raylib.h"

struct RopeNode
{
	public:

		Vector2 Position;
		Vector2 OldPosition;
		Vector2 Acceleration;
		int RopeID;
		bool IsAnchored;

		RopeNode() = default;

		RopeNode(Vector2 position, Vector2 initialAcceleration, float radius, float ropeLength, bool isAnchored, int ropeid = 0) {
			Position = position;//world coordinates of a rope node
			OldPosition = position;
			Acceleration = initialAcceleration; // velocity vector of a rope node
			IsAnchored = isAnchored; // can this rope node be moved or not
			RopeID = ropeid; // keep track of the rope this node belongs to

			if (isAnchored) {			//if a node is anchored, it never moves
				Acceleration = { 0,0 };
			}

		};

		~RopeNode() = default;

};


