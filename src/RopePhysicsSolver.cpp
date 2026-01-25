#include "RopePhysicsSolver.h"


//adds acceleration as a force to a rope node. for things like gravity, bounces, wind, etc.
void RopePhysicsSolver::Accelerate(RopeNode& ropenode, Vector2 acceleration) {

	if (ropenode.IsAnchored) {
		ropenode.Acceleration = { 0,0 };
	}
	else {
		ropenode.Acceleration = ropenode.Acceleration + acceleration;
	}
}


void RopePhysicsSolver::UpdateRopeNodesPositions(Rope& rope, double deltaTime) {	// Physics for the nodes using Verlet integration

	//physically based damping
	auto dampVelocity = [this](Vector2& velocity, RopeNode& ropenode, float deltaTime) {

		float crossSection = 2 * ropenode.Radius;
		float speed = Vector2Length(velocity) / deltaTime;

		if (speed > 0.01f) {
			float dragForceMagnitude = 0.5 * config.physics.airDensity * speed * speed * config.physics.dragCoef * crossSection;

			float dragFactor = 1 - (dragForceMagnitude * deltaTime / speed);

			if (dragFactor < 0) { dragFactor = 0; }

			velocity *= dragFactor;
		}
		};

	for (RopeNode& ropenode : rope.nodes)
	{
		//skip the node thats being dragged
		if (&ropenode == config.interaction.draggedNode) { continue; }

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
		Accelerate(ropenode, config.physics.g); // apply gravity for all nodes
	}
}

void RopePhysicsSolver::UpdateRope(Rope& rope, Camera2D& camera, int substeps, int iterations, double deltaTime) {

	Vector2 dragStartFramePos = FindNodeToMove(rope, camera);

	// UpdateRope is rope's full life cycle. use after creating the rope to update and render it
	for (int i = 1; i <= substeps; i++) {
		ApplyForces(rope);
		UpdateRopeNodesPositions(rope, deltaTime / (float)substeps);
		MoveRopeNode(rope, camera, substeps, i, dragStartFramePos);
		ApplyConstraints(rope, iterations);

	}

	ToggleAnchor();
}


Rope RopePhysicsSolver::SetupRope(Vector2 firstNodePos, bool isFirstNodeAnchored, int nodeAmount, float RopeLengthForEach, float nodeRadiusForEach) {

	Rope newRopeNodes;

	// create the first node
	newRopeNodes.nodes.emplace_back(firstNodePos, nodeRadiusForEach, RopeLengthForEach, isFirstNodeAnchored, Vector2{ 0,0 });

	// create the rest and offset them by ropelength to the right
	for (int i = 1; i < nodeAmount; i++) {
		Vector2 offset = Vector2{ RopeLengthForEach * i, 0 };
		newRopeNodes.nodes.emplace_back(firstNodePos + offset, nodeRadiusForEach, RopeLengthForEach, false, Vector2{ 0,0 });
	}

	config.physics.ExistingRopes.emplace_back(newRopeNodes); // Add this rope to a list of all existing ropes within this config

	return newRopeNodes;
}


void RopePhysicsSolver::ApplyConstraints(Rope& rope, int iterations) {

	for (uint16_t iter = 0; iter < iterations; iter++) {

		for (uint16_t i = 0; i < rope.nodes.size() - 1; i++) {

			//get the nodes
			RopeNode& nodeA = rope.nodes[i];
			RopeNode& nodeB = rope.nodes[i + 1];


			Vector2 vec = nodeB.Position - nodeA.Position;  // Vector from A to B
			float currentDist = Vector2Length(vec);
			float targetDist = nodeA.RopeLength;



			// Only correct if distance is greater than target (rope is too long)
			if (currentDist > targetDist) [[likely]] {

				// prevent division by zero
				if (currentDist == 0) { currentDist = 0.001f; }

				Vector2 dir = vec / currentDist;  // Normalized direction from A to B
				float error = currentDist - targetDist;
				Vector2 correction = dir * (error * 0.5f);

				// prevent acces momentum build up
				float correctionCoef = Clamp(targetDist / currentDist * 0.5f, 0, 0.5f);


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
void RopePhysicsSolver::HandleRopes(Camera2D& camera, int substeps, int iterations, double deltaTime) {

	for (int i = 0; i < config.physics.ExistingRopes.size(); i++) {

		UpdateRope(config.physics.ExistingRopes[i], camera, substeps, iterations, deltaTime);
		RopeRenderer::RenderRope(config.physics.ExistingRopes[i]);
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


//check overlap of a node with the mouse
Vector2 RopePhysicsSolver::FindNodeToMove(Rope& rope, const Camera2D& Camera) {

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

		Vector2 cursorWorldPos = GetScreenToWorld2D(GetMousePosition(), Camera);

		if (config.interaction.draggedNode == nullptr && config.interaction.canDrag) {

			for (RopeNode& ropenode : rope.nodes) { //bruteforce. for every node in a rope, check overlap between the cursor and a node
													// until we find the one that overlaps
				if (Vector2Distance(cursorWorldPos, ropenode.Position) < ropenode.Radius) {
					config.interaction.draggedNode = &ropenode;	//found the node
					config.interaction.wasAnchored = config.interaction.draggedNode->IsAnchored;	//check if it was anchored to return to this state after LMB is no longer being held
					break;
				}

			}
		}
	}

	//get the position at which the dragging will start. must call every frame
	Vector2 dragStartFramePos = { 0,0 };
	if (config.interaction.draggedNode != nullptr) {
		dragStartFramePos = config.interaction.draggedNode->Position;
	}

	return dragStartFramePos;
}

void RopePhysicsSolver::ToggleAnchor()
{
	if (config.interaction.draggedNode != nullptr) {

		if (IsKeyPressed(KEY_LEFT_CONTROL)) {	//if control is pressed, change if the node is anchored or not

			config.interaction.wasAnchored = !config.interaction.wasAnchored;
			config.interaction.draggedNode->IsAnchored = !config.interaction.draggedNode->IsAnchored;
		}
	}
}

//moves the node to the cursors position
void RopePhysicsSolver::MoveRopeNode(Rope& rope, const Camera2D& mainCamera, int substeps, int i, Vector2 dragStartFramePos) {

	Vector2 cursorWorldPos = GetScreenToWorld2D(GetMousePosition(), mainCamera);

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

		if (config.interaction.draggedNode == nullptr) return;

		// POINTER RANGE CHECK:
		// Check if the address of draggedNode is within the memory bounds of this rope's vector
		const RopeNode* firstNode = &rope.nodes.front();
		const RopeNode* lastNode = &rope.nodes.back();

		if (config.interaction.draggedNode < firstNode || config.interaction.draggedNode > lastNode) {
			return; // This node belongs to a different rope; exit this function
		}
		config.interaction.draggedNode->IsAnchored = true;

			Vector2 dir = cursorWorldPos - dragStartFramePos;
			float length = Vector2Length(dir);

			if (length > 0) {

				dir = dir / length;

				//interpolate the position over all sub-steps
				config.interaction.draggedNode->Position = dragStartFramePos + dir * (float)i / ((float)substeps) * length;
				config.interaction.draggedNode->OldPosition = dragStartFramePos + dir * (float)i / ((float)substeps) * length;

			}

	}

	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {

		if (config.interaction.draggedNode != nullptr) {	//dispose the pointer and release the node

			if (config.interaction.wasAnchored) {

				config.interaction.draggedNode->IsAnchored = true;
			}
			else {

				config.interaction.draggedNode->IsAnchored = false;
			}
			config.interaction.draggedNode = nullptr;
		}
	}
}