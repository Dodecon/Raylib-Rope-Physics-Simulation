#pragma once
#include "raylib.h"

struct RopeNode
{
	public:
		RopeNode(Vector2 position, float radius, float ropeLength, bool isAnchored, Vector2 initialAcceleration) {
			Position = position;//world coordinates of a rope node
			OldPosition = position;
			Acceleration = initialAcceleration; // velocity vector of a rope node
			Radius = radius; // for collisions(future) and visibility for debugging
			RopeLength = ropeLength; // maximum distance that an attached rope node can be from this one
			IsAnchored = isAnchored; // can this rope node be moved or not

			if (isAnchored) {			//if a node is anchored, it never moves
				Acceleration = { 0,0 };
			}

		};
		
		Vector2 Position;
		Vector2 OldPosition;
		Vector2 Acceleration;
		float Radius;
		float RopeLength;
		bool IsAnchored;

};


