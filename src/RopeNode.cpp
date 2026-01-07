#include "RopeNode.h"
#include "Vector2_Operators.h"


RopeNode::RopeNode(Vector2 position, float radius, float ropeLength, bool isAnchored, Vector2 initialAcceleration) {
	Position = position ;//world coordinates of a rope node
	OldPosition = position;
	Acceleration = initialAcceleration; // velocity vector of a rope node
	Radius = radius; // for collisions(future) and visibility for debugging
	RopeLength = ropeLength; // maximum distance that an attached rope node can be from this one
	IsAnchored = isAnchored; // can this rope node be moved or not

	if (isAnchored) {			//if a node is anchored, it never moves
		Acceleration = {0,0};
	}

};


//helper method to set position of a rope node
void RopeNode::SetPosition(Vector2& newPos) {
	Position = newPos;
}
//set any rope nodes acceleration to anything
void RopeNode::SetAcceleration(Vector2& newAcc) {
	Acceleration = newAcc;
}




//adds acceleration as a force to a rope node. for things like gravity, bounces, wind, etc.
void RopeNode::Accelerate(Vector2 acceleration) {

	if (IsAnchored) {
		Acceleration = { 0,0 };
	}
	else {
		Acceleration = Acceleration + acceleration;
	}
}