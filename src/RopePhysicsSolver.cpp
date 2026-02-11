#include "RopePhysicsSolver.h"

//adds acceleration that resets every frame to a node
void RopePhysicsSolver::Accelerate(RopeNode& ropenode, const Vector2 acceleration) {

	if (ropenode.IsAnchored) {
		ropenode.Acceleration = { 0,0 };
	}
	else {
		ropenode.Acceleration = ropenode.Acceleration + acceleration;
	}
}


void RopePhysicsSolver::UpdateRopeNodePosition(RopeNode& node, Rope& rope, const double deltaTime) {	// Physics for the nodes using Verlet integration

	//physically based damping
	auto dampVelocity = [this](Vector2& velocity, Rope& rope, float deltaTime) {

		float crossSection = rope.RopeLengthForEach;
		float speed = Vector2Length(velocity) / deltaTime;

		if (speed > 0.01f) {
			float dragForceMagnitude = 0.5 * config.physics.airDensity * speed * speed * config.physics.dragCoef * crossSection;

			float dragFactor = 1 - (dragForceMagnitude * deltaTime / speed);

			if (dragFactor < 0) { dragFactor = 0; }

			velocity *= dragFactor;
		}
	};

	// if the node is anchored, it doesnt move
	if (node.IsAnchored) {

		node.Acceleration = { 0,0 };
		return;
	}
	else {

		Vector2 velocity = (node.Position - node.OldPosition);

		dampVelocity(velocity, rope, deltaTime);

		node.OldPosition = node.Position;	//update old position
		Vector2 nextPosition = node.OldPosition + velocity + node.Acceleration * deltaTime * deltaTime; // update node's position



		node.Position = nextPosition;		// set node's position
		node.Acceleration = { 0,0 };	// reset node's acceleration
	}

}

void RopePhysicsSolver::ApplyForces(RopeNode& node) {

	Accelerate(node, config.physics.g); // apply gravity for all nodes
}

void RopePhysicsSolver::UpdateRopes(Camera2D& camera, const int substeps, const int iterations, const double deltaTime) {

	float subDT = deltaTime / substeps;


	Vector2 dragStartFramePos = {};

	//rope interaction
	// Only search for a new node if we aren't already dragging one
	if (config.interaction.draggedRope == nullptr) {
		for (Rope& rope : AllRopes) {
			dragStartFramePos = FindNodeToMove(rope, camera);
			// If we found a node in this rope, stop checking other ropes
			if (config.interaction.draggedRope != nullptr) break;
		}
	}
	else {
		// If we are already dragging, just get the position from the active rope
		if (config.interaction.draggedNodeID != -1) {
			dragStartFramePos = AllNodes[config.interaction.draggedNodeID].Position;
		}
	}

	// UpdateRope is rope's full life cycle. use after creating the ropes to update and render them
	for (int i = 1; i <= substeps; i++) {

		//update positions
		threadpool.ParralelFor(0, AllNodes.size(), [&](int i) {

			Rope& thisRope = AllRopes[AllNodes[i].RopeID];

			ApplyForces(AllNodes[i]);
			UpdateRopeNodePosition(AllNodes[i], thisRope, subDT);

		});

		//move the node
		for (Rope& rope : AllRopes) {
			MoveRopeNode(rope, camera, substeps, i, dragStartFramePos);
		}

		//calculate constraints
		threadpool.ParralelFor(0, AllRopes.size(), [&](int i) {

				Rope& thisRope = AllRopes[i];
				ApplyConstraints(AllNodes, thisRope, iterations);
			
		});

	}
	//toggle if we want the node to be ahnchored
	for (Rope& rope : AllRopes) {
		ToggleAnchor(rope);
	}
}


Rope& RopePhysicsSolver::SetupRope(const Vector2 firstNodePos, bool isFirstNodeAnchored, int nodeAmount, float RopeLengthForEach, float nodeRadiusForEach) {

	// create a helper rope
	AllRopes.emplace_back(nodeAmount, nodeRadiusForEach, RopeLengthForEach); // Add this rope to a list of all existing ropes

	int currentRopeID = AllRopes.size() - 1;
	// create the first node
	AllNodes.emplace_back(firstNodePos, Vector2{ 0,0 }, nodeRadiusForEach, RopeLengthForEach, isFirstNodeAnchored, currentRopeID);

	// set the starting index for thr newly created rope
	AllRopes.back().startNodeIndex = AllNodes.size() - 1;

	// create the rest and offset them by ropelength to the right
	for (int i = 1; i < nodeAmount; i++) {

		Vector2 offset = Vector2{ RopeLengthForEach * i, 0 };
		AllNodes.emplace_back(firstNodePos + offset, Vector2{ 0,0 }, nodeRadiusForEach, RopeLengthForEach, false, currentRopeID);
	}


	return AllRopes.back();
}


void RopePhysicsSolver::ApplyConstraints(std::vector<RopeNode>& nodes, Rope& rope, const int iterations) {

	for (int j = 0; j < iterations; ++j) {

		for (int i = rope.startNodeIndex; i < rope.startNodeIndex + rope.nodeAmount - 1; ++i) {

			//get the nodes
			RopeNode& nodeA = nodes[i];
			RopeNode& nodeB = nodes[i + 1];

			Vector2 vec = nodeB.Position - nodeA.Position;  // Vector from A to B
			float currentDist = Vector2Length(vec);
			float targetDist = rope.RopeLengthForEach;


			// Only correct if distance is greater than target (rope is too long)
			if (config.physics.areRopesRigid || (currentDist > targetDist)) [[likely]] {

				// prevent division by zero
				if (currentDist == 0) { currentDist = 0.001f; }

				Vector2 dir = vec / currentDist;  // Normalized direction from A to B
				float error = currentDist - targetDist;
				Vector2 correction = dir * (error * 0.5f);

				// prevent acces momentum build up
				float correctionCoef = Clamp(targetDist / currentDist, 0, 0.25f);


				if (!nodeA.IsAnchored && !nodeB.IsAnchored) [[likely]] {
					// Case 1: Neither anchored - move both toward each other
					nodeA.Position += correction;
					nodeB.Position -= correction;

					// prevent acces momentum build up
					nodeA.OldPosition += correction * correctionCoef;
					nodeB.OldPosition -= correction * correctionCoef;


				}
				else if (nodeA.IsAnchored && !nodeB.IsAnchored) {
					// Case 2: A anchored - move B the full error distance toward A
					nodeB.Position -= dir * error;

					nodeB.OldPosition -= dir * error * correctionCoef;
				}
				else if (!nodeA.IsAnchored && nodeB.IsAnchored) {
					// Case 3: B anchored - move A the full error distance toward B
					nodeA.Position += dir * error;

					nodeA.OldPosition += dir * error * correctionCoef;
				}


			}
		}
	}
}

//calculates physics and renders all the ropes in one command
void RopePhysicsSolver::HandleRopes(Camera2D& camera, const int substeps, const int iterations, const double deltaTime) {


		UpdateRopes(camera, substeps, iterations, deltaTime);

		//need to change to use multi-threading
		for (int i = 0; i < AllRopes.size(); i++) {

			Rope& thisRope = AllRopes[i];
			RopeRenderer::RenderRopes(camera, thisRope, AllNodes);
		}

}


//check overlap of a node with the mouse
Vector2 RopePhysicsSolver::FindNodeToMove(Rope& rope, Camera2D& Camera) {

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

		//get cursor world position
		Vector2 cursorWorldPos = GetScreenToWorld2D(GetMousePosition(), Camera);



		if (config.interaction.draggedRope == nullptr && config.interaction.canDrag) {

			for (int i = rope.startNodeIndex; i < rope.startNodeIndex + rope.nodeAmount; i++) { //bruteforce. for every node in a rope, check overlap between the cursor and a node
													// until we find the one that overlaps
				if (Vector2Distance(cursorWorldPos, AllNodes[i].Position) < rope.Radius) {
					config.interaction.draggedNodeID = i;	//found the node
					config.interaction.draggedRope = &rope;
					config.interaction.wasAnchored = AllNodes[config.interaction.draggedNodeID].IsAnchored;	//check if it was anchored to return to this state after LMB is no longer being held
					
					break;
				}

			}
		}
	}

	//get the position at which the dragging will start. must call every frame
	Vector2 dragStartFramePos = { 0,0 };
	// Only access the node if this rope is the one being dragged
	if (config.interaction.draggedRope == &rope && config.interaction.draggedNodeID != -1) {
		dragStartFramePos = AllNodes[config.interaction.draggedNodeID].Position;
	}

	return dragStartFramePos;
}

void RopePhysicsSolver::ToggleAnchor(Rope& rope)
{
	if (config.interaction.draggedRope != nullptr) {

		if (&rope == config.interaction.draggedRope && IsKeyPressed(KEY_LEFT_CONTROL)) {	//if control is pressed (and we are checking thr correct rope), change whether or not the node is anchored

			config.interaction.wasAnchored = !config.interaction.wasAnchored;
			AllNodes[config.interaction.draggedNodeID].IsAnchored = !AllNodes[config.interaction.draggedNodeID].IsAnchored;
		}
	}
}

//moves the node to the cursors position
void RopePhysicsSolver::MoveRopeNode(Rope& rope, const Camera2D& mainCamera, const int substeps, const int i, const Vector2 dragStartFramePos) {

	Vector2 cursorWorldPos = GetScreenToWorld2D(GetMousePosition(), mainCamera);
	int& draggedNodeID = config.interaction.draggedNodeID;

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

		//Only proceed if THIS specific rope is the one being dragged
		if (config.interaction.draggedRope != &rope) return;

		//anchore the node thats being dragged
		AllNodes[draggedNodeID].IsAnchored = true;

			Vector2 dir = cursorWorldPos - dragStartFramePos;
			float length = Vector2Length(dir);

			if (length > 0) {

				dir = dir / length;

				//interpolate the position over all sub-steps
				AllNodes[draggedNodeID].Position = dragStartFramePos + dir * (float)i / ((float)substeps) * length;
				AllNodes[draggedNodeID].OldPosition = dragStartFramePos + dir * (float)i / ((float)substeps) * length;

			}

	}

	//stop dragging the node
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {

		//check if we are in the correct rope
		if (config.interaction.draggedRope != &rope) return;

		if (config.interaction.draggedNodeID != -1) {	//return the node to its state before dragging

			if (config.interaction.wasAnchored) {

				AllNodes[draggedNodeID].IsAnchored = true;
			}
			else {

				AllNodes[draggedNodeID].IsAnchored = false;
			}
			//reset config
			draggedNodeID = -1;
			config.interaction.draggedRope = nullptr;
		}
	}
}