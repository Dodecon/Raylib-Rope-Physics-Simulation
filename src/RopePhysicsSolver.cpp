#include "RopePhysicsSolver.h"

const float RopePhysicsSolver::g = 9.81 * 100;	// a gravity constant. multiplied by 100 since raylib uses pixel space for positions
std::vector<std::vector<RopeNode>> RopePhysicsSolver::ExistingRopes; // A list to track all existing ropes


void RopePhysicsSolver::UpdateRopeNodesPositions(std::vector<RopeNode>& ropenodes, float deltaTime) {	// Physics for the nodes using Verlet integration
	for (RopeNode& ropenode : ropenodes)
	{
		if (ropenode.IsAnchored) {				// if the node is anchored, it doesnt move
			ropenode.Acceleration = { 0,0 };
		}
		else {
			Vector2 velocity = ropenode.Position - ropenode.OldPosition;
			velocity = velocity * 0.9995; //small damping facor for the rope


			ropenode.OldPosition = ropenode.Position;

			Vector2 nextPosition = ropenode.Position + velocity + ropenode.Acceleration * deltaTime * deltaTime; // update node's position


			ropenode.SetPosition(nextPosition);			// set node's position
			ropenode.SetAcceleration(Vector2{ 0,0 });	// reset node's acceleration
		}
	}
}

void RopePhysicsSolver::ApplyForces(std::vector<RopeNode>& ropenodes) {

	for (RopeNode& ropenode : ropenodes)
	{
		ropenode.Accelerate({0, g}); // apply gravity on Y axis for all nodes
	}
}

void RopePhysicsSolver::UpdateRope(std::vector<RopeNode>& ropenodes, float deltaTime) {

	RenderNodes(ropenodes);		// UpdateRope is rope's full life cycle. use after creating the rope to update and render it

	ApplyForces(ropenodes);
	ApplyConstraints(ropenodes, deltaTime);
	UpdateRopeNodesPositions(ropenodes, deltaTime);


}


std::vector<RopeNode> RopePhysicsSolver::SetupRope(Vector2 firstNodePos, bool isFirstNodeAnchored, int nodeAmount, float RopeLengthForEach, float nodeRadiousForEach) {

	std::vector<RopeNode> newRopeNodes;

	// create the first node
	newRopeNodes.emplace_back(firstNodePos, nodeRadiousForEach, RopeLengthForEach, isFirstNodeAnchored, Vector2{ 0,0 });

	// create the rest and offset them by ropelength to the right
	for (int i = 1; i < nodeAmount; i++) {
		newRopeNodes.emplace_back(firstNodePos + Vector2{ RopeLengthForEach * i, 0}, nodeRadiousForEach, RopeLengthForEach, false, Vector2{0,0});
	}


	ExistingRopes.emplace_back(newRopeNodes); // Add this rope to a list of all existing ropes

	return newRopeNodes;
}

void RopePhysicsSolver::RenderNodes(std::vector<RopeNode>& ropenodes) {

	// Draw connections between nodes
	for (int i = 0; i < ropenodes.size() - 1; i++) {
		DrawLineEx(ropenodes.at(i).Position,
			ropenodes.at(i + 1).Position, 7, RED);
	}

	// draw nodes themselves
	for (int i = 0; i < ropenodes.size(); i++) {
		DrawCircle(ropenodes.at(i).Position.x, ropenodes.at(i).Position.y, ropenodes.at(i).Radius, GREEN);
	}

}


void RopePhysicsSolver::ApplyConstraints(std::vector<RopeNode>& ropenodes, float deltaTime) {

		for (int iter = 0; iter < 10; iter++) {
			for (int i = 0; i < ropenodes.size() - 1; i++) {

				Vector2 nodeA = ropenodes[i].Position;
				Vector2 nodeB = ropenodes[i + 1].Position;

				Vector2 vec = nodeB - nodeA;  // Vector from A to B
				float current_dist = Vector2Length(vec);
				float target_dist = ropenodes[i].RopeLength;

				// Only correct if distance is greater than target (rope is too long)
				if (current_dist > target_dist) {
					Vector2 dir = vec / current_dist;  // Normalized direction from A to B
					float error = current_dist - target_dist;
					Vector2 correction_per_node = dir * (error * 0.5f);

					bool A_anchored = ropenodes[i].IsAnchored;
					bool B_anchored = ropenodes[i + 1].IsAnchored;

					if (!A_anchored && !B_anchored) {
						// Case 1: Neither anchored - move both toward each other
						ropenodes[i].SetPosition(nodeA + correction_per_node);
						ropenodes[i + 1].SetPosition(nodeB - correction_per_node);
					}
					else if (A_anchored && !B_anchored) {
						// Case 2: A anchored - move B the full error distance toward A
						ropenodes[i + 1].SetPosition(nodeB - dir * error);
					}
					else if (!A_anchored && B_anchored) {
						// Case 3: B anchored - move A the full error distance toward B
						ropenodes[i].SetPosition (nodeA + dir * error);
					}
				}
			}
		}
}


void RopePhysicsSolver::HandleRopes(Camera2D& mainCamera, std::vector<std::vector<RopeNode>>& ExistingRopes) {

	for (std::vector<RopeNode>& rope : ExistingRopes) {
		MoveRopeNode(rope, mainCamera);
		RopePhysicsSolver::UpdateRope(rope, 0.0083333);  //using fixed time step (1/120) for best simulation results
	}
}


// Static variables to keep state between frames
static RopeNode* draggedNode = nullptr;
static bool wasAnchored = false;
//detect overlap of a ropenode with the cursor while the LMB is held, then moves the node to the cursors position
void RopePhysicsSolver::MoveRopeNode(std::vector<RopeNode>& ropenodes, const Camera2D& mainCamera) {

	Vector2 cursorWorldPos = GetScreenToWorld2D(GetMousePosition(), mainCamera);

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

		if (draggedNode == nullptr) {
			for (RopeNode& ropenode : ropenodes) { //bruteforce. for every node in a rope, check overlap between the cursor and a node
				// until we find the one that overlaps

				if (Vector2Distance(cursorWorldPos, ropenode.Position) < ropenode.Radius) {
					draggedNode = &ropenode;	//found the node

					wasAnchored = draggedNode->IsAnchored;	//check if it was anchored to return to this state after LMB is no longer being held
				}

			}
		}

		if (draggedNode != nullptr) {	//if we've found the node, set it as anchored and change position to the cursor's position
			draggedNode->IsAnchored = true;
			draggedNode->SetPosition(cursorWorldPos);
			draggedNode->OldPosition = cursorWorldPos;
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