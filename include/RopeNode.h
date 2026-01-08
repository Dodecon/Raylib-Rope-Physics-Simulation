#pragma once
#include "raylib.h"

class RopeNode
{
	public:
		RopeNode(Vector2 position, float radius, float ropeLength, bool isAnchored, Vector2 initialAcceleration); // constructor
		
		Vector2 Position;
		Vector2 OldPosition;
		Vector2 Acceleration;
		float Radius;
		float RopeLength;
		bool IsAnchored;


		void SetAcceleration(Vector2& newAcc);
		void SetPosition(Vector2& newPos);


		void Accelerate(Vector2 acceleration);
};


