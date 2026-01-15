#include "RopePhysicsSolver.h"

Vector2 RopePhysicsSolver::g = {0, 9.81 * 100 };	// a gravity coef. multiplied by 100 since raylib uses pixel space for positions

float RopePhysicsSolver::airDensity = 0.00002; //default value, may be changed via UI
float RopePhysicsSolver::dragCoef = 0.47; //for circles

std::vector<Rope> RopePhysicsSolver::ExistingRopes; // A list to track all existing ropes


//adds acceleration as a force to a rope node. for things like gravity, bounces, wind, etc.
void RopePhysicsSolver::Accelerate(RopeNode& ropenode, Vector2 acceleration) {

	if (ropenode.IsAnchored) {
		ropenode.Acceleration = { 0,0 };
	}
	else {
		ropenode.Acceleration = ropenode.Acceleration + acceleration;
	}
}


void RopePhysicsSolver::UpdateRopeNodesPositions(Rope& rope, float deltaTime) {	// Physics for the nodes using Verlet integration

	//physically based damping
	auto dampVelocity = [](Vector2& velocity, RopeNode& ropenode, float deltaTime) {

		float crossSection = 2 * ropenode.Radius;
		float speed = Vector2Length(velocity) / deltaTime;

		if (speed > 0.000001f) {
			float dragForceMagnitude = 0.5 * airDensity * speed * speed * dragCoef * crossSection;

			float dragFactor = 1 - (dragForceMagnitude * deltaTime / speed);

			if (dragFactor < 0) { dragFactor = 0; }

			velocity *= dragFactor;
		}
	};

	for (RopeNode& ropenode : rope.nodes)
	{
		if (ropenode.IsAnchored) {				// if the node is anchored, it doesnt move
			ropenode.Acceleration = { 0,0 };

		}
		else {

			Vector2 velocity = (ropenode.Position - ropenode.OldPosition);
			dampVelocity(velocity, ropenode, deltaTime);

			ropenode.OldPosition = ropenode.Position;	//update old position
			Vector2 nextPosition = ropenode.Position + velocity + ropenode.Acceleration * deltaTime * deltaTime; // update node's position



			ropenode.Position = nextPosition;		// set node's position
			ropenode.Acceleration = { 0,0 };	// reset node's acceleration
		}
	}
}

void RopePhysicsSolver::ApplyForces(Rope& rope) {

	for (RopeNode& ropenode : rope.nodes)
	{
		Accelerate(ropenode,g); // apply gravity for all nodes
	}
}

void RopePhysicsSolver::UpdateRope(Rope& rope, float deltaTime) {

	// UpdateRope is rope's full life cycle. use after creating the rope to update and render it

	ApplyForces(rope);
	UpdateRopeNodesPositions(rope, deltaTime);
	ApplyConstraints(rope, deltaTime);
	RopeRenderer::RenderRope(rope);
}


Rope RopePhysicsSolver::SetupRope(Vector2 firstNodePos, bool isFirstNodeAnchored, int nodeAmount, float RopeLengthForEach, float nodeRadiousForEach) {

	Rope newRopeNodes;

	// create the first node
	newRopeNodes.nodes.emplace_back(firstNodePos, nodeRadiousForEach, RopeLengthForEach, isFirstNodeAnchored, Vector2{ 0,0 });

	// create the rest and offset them by ropelength to the right
	for (int i = 1; i < nodeAmount; i++) {
		newRopeNodes.nodes.emplace_back(firstNodePos + Vector2{ RopeLengthForEach * i, 0}, nodeRadiousForEach, RopeLengthForEach, false, Vector2{0,0});
	}

	ExistingRopes.emplace_back(newRopeNodes); // Add this rope to a list of all existing ropes

	return newRopeNodes;
}


void RopePhysicsSolver::ApplyConstraints(Rope& rope, float deltaTime) {

		for (int iter = 0; iter < 5; iter++) {
			for (int i = 0; i < rope.nodes.size() - 1; i++) {

				Vector2 nodeA = rope.nodes[i].Position;
				Vector2 nodeB = rope.nodes[i + 1].Position;

				Vector2 vec = nodeB - nodeA;  // Vector from A to B
				float current_dist = Vector2Length(vec);
				float target_dist = rope.nodes[i].RopeLength;

				// Only correct if distance is greater than target (rope is too long)
				if (current_dist > target_dist) {
					Vector2 dir = vec / current_dist;  // Normalized direction from A to B
					float error = current_dist - target_dist;
					Vector2 correction_per_node = dir * (error * 0.5f);

					bool A_anchored = rope.nodes[i].IsAnchored;
					bool B_anchored = rope.nodes[i + 1].IsAnchored;

					if (!A_anchored && !B_anchored) {
						// Case 1: Neither anchored - move both toward each other
						rope.nodes[i].Position = nodeA + correction_per_node;
						rope.nodes[i + 1].Position = nodeB - correction_per_node;
					}
					else if (A_anchored && !B_anchored) {
						// Case 2: A anchored - move B the full error distance toward A
						rope.nodes[i + 1].Position = nodeB - dir * error;
					}
					else if (!A_anchored && B_anchored) {
						// Case 3: B anchored - move A the full error distance toward B
						rope.nodes[i].Position = nodeA + dir * error;
					}
				}
			}
		}
}


void RopePhysicsSolver::HandleRopes(std::vector<Rope>& ExistingRopes, Camera2D& mainCamera) {

	for (Rope& rope : ExistingRopes) {
		MoveRopeNode(rope, mainCamera);
		RopePhysicsSolver::UpdateRope(rope, 0.0083333);  //using fixed time step (1/120) for best simulation results
	}
}

void RopeRenderer::RenderRope(Rope& rope) {

	// Draw connections between nodes
	for (int i = 0; i < rope.nodes.size() - 1; i++) {
		DrawLineEx(rope.nodes.at(i).Position,
			rope.nodes.at(i + 1).Position, 7, RED);
	}

	// draw nodes themselves
	for (int i = 0; i < rope.nodes.size(); i++) {
		DrawCircle(rope.nodes.at(i).Position.x, rope.nodes.at(i).Position.y, rope.nodes.at(i).Radius, GREEN);
	}

}


// Static variables to keep state between frames
static RopeNode* draggedNode = nullptr;
bool RopePhysicsSolver::canDrag = true;
static bool wasAnchored = false;
//detect overlap of a ropenode with the cursor while the LMB is held, then moves the node to the cursors position
void RopePhysicsSolver::MoveRopeNode(Rope& rope, const Camera2D& mainCamera) {

	Vector2 cursorWorldPos = GetScreenToWorld2D(GetMousePosition(), mainCamera);

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

		if (draggedNode == nullptr && canDrag) {
			for (RopeNode& ropenode : rope.nodes) { //bruteforce. for every node in a rope, check overlap between the cursor and a node
				// until we find the one that overlaps

				if (Vector2Distance(cursorWorldPos, ropenode.Position) < ropenode.Radius) {
					draggedNode = &ropenode;	//found the node

					wasAnchored = draggedNode->IsAnchored;	//check if it was anchored to return to this state after LMB is no longer being held
				}

			}
		}

		if (draggedNode != nullptr) {	//if we've found the node, set it as anchored and change position to the cursor's position
			draggedNode->IsAnchored = true;
			draggedNode->Position = cursorWorldPos;
			draggedNode->OldPosition = cursorWorldPos;

			if (IsKeyPressed(KEY_LEFT_CONTROL)) {	//if control is pressed, change if the node is anchored or not
				wasAnchored = !wasAnchored;
				draggedNode->IsAnchored = !draggedNode->IsAnchored;
			}
		}
	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {

		if (draggedNode != nullptr) {	//dispose the pointer and release the node

			if (wasAnchored) {
				draggedNode->IsAnchored = true;
			}
			else {
				draggedNode->IsAnchored = false;
			}
			draggedNode = nullptr;
		}
	}
}