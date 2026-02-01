#include "RopePhysicsSolver.h"

#include <chrono> // to measure performance
#include <iostream>


//adds acceleration as a force to a rope node. for things like gravity, bounces, wind, etc.
void RopePhysicsSolver::Accelerate(Rope& rope, int NodeIndex, const Vector2 acceleration) {

	if (rope.IsAnchored[NodeIndex]) {
		rope.Accelerations[NodeIndex] = {0,0};
	}
	else {
		rope.Accelerations[NodeIndex] = rope.Accelerations[NodeIndex] + acceleration;
	}
}


void RopePhysicsSolver::UpdateRopeNodesPositions(Rope& rope, const double deltaTime) {	// Physics for the nodes using Verlet integration


	//physically based damping
	auto dampVelocity = [this](Vector2& velocity, Rope& rope, float deltaTime) {

		float crossSection = rope.TargetLength;
		float speed = Vector2Length(velocity) / deltaTime;

		if (speed > 0.01f) {
			float dragForceMagnitude = 0.5 * config.physics.airDensity * speed * speed * config.physics.dragCoef * crossSection;

			float dragFactor = 1 - (dragForceMagnitude * deltaTime / speed);

			if (dragFactor < 0) { dragFactor = 0; }

			velocity *= dragFactor;
		}
	};

	for (int i = 0; i < rope.NodeAmount; i++)
	{
		// if the node is anchored, it doesnt move
		if (rope.IsAnchored[i]) {
			rope.Accelerations[i] = {0,0};
			continue;
		}
		else {

			Vector2 velocity = (rope.Positions[i] - rope.OldPositions[i]);

			dampVelocity(velocity, rope, deltaTime);

			rope.OldPositions[i] = rope.Positions[i];	//update old position
			Vector2 nextPosition = rope.OldPositions[i] + velocity + rope.Accelerations[i] * deltaTime * deltaTime; // update node's position



			rope.Positions[i] = nextPosition;		// set node's position
			rope.Accelerations[i] = {0,0};	// reset node's acceleration
		}
	}
}

void RopePhysicsSolver::ApplyForces(Rope& rope) {

	for (int i = 0; i < rope.NodeAmount; i++)  // apply gravity for all nodes
	{
		Accelerate(rope, i, config.physics.g);
	}
}

void RopePhysicsSolver::UpdateRope (Rope& rope, Camera2D& camera, const int substeps, const int iterations, const double deltaTime) {

	Vector2 dragStartFramePos = FindNodeToMove(rope, camera);

	// UpdateRope is rope's full life cycle. use after creating the rope to update and render it
	for (int i = 1; i <= substeps; i++) {
		ApplyForces(rope);
		UpdateRopeNodesPositions(rope, deltaTime / (float)substeps);
		MoveRopeNode(rope, camera, substeps, i, dragStartFramePos);
		ApplyConstraints(rope, iterations);

	}

	ToggleAnchor(rope);
}


Rope& RopePhysicsSolver::SetupRope(const Vector2 firstNodePos, const Vector2 initialAcceleration, bool isFirstNodeAnchored, int nodeAmount, float LengthForEach, float nodeRadiusForEach) {

	// create the rope
	Rope newRope(nodeAmount, nodeRadiusForEach, LengthForEach);

	// fill the nodes with data
	newRope.FillRope(firstNodePos, initialAcceleration);

	// set IsAnchored the users value for the first node
	newRope.IsAnchored[0] = (float)isFirstNodeAnchored;

	// Add this rope to a list of all existing ropes within a config
	config.physics.ExistingRopes.push_back(std::move(newRope));

	return config.physics.ExistingRopes.back();
}


void RopePhysicsSolver::ApplyConstraints(Rope& rope, const int iterations) {

	for (uint16_t iter = 0; iter < iterations; iter++) {

		for (uint16_t i = 0; i < rope.NodeAmount - 1; i++) {


			// nodeA is rope.Positions[i]
			// nodeB is rope.Positions[i + 1]

			Vector2 vec = rope.Positions[i + 1] - rope.Positions[i];  // Vector from A to B
			float currentDist = Vector2Length(vec);
			float targetDist = rope.TargetLength;


			// Only correct if distance is greater than target (rope is too long)
			if (config.physics.areRopesRigid || (currentDist > targetDist)) [[likely]] {

				// prevent division by zero
				if (currentDist == 0) { currentDist = 0.001f; }

				Vector2 dir = vec / currentDist;  // Normalized direction from A to B
				float error = currentDist - targetDist;
				Vector2 correction = dir * (error * 0.5f);

				// prevent acces momentum build up
				float correctionCoef = Clamp(targetDist / currentDist * 0.5f, 0, 0.5f);


				if (!rope.IsAnchored[i] && !rope.IsAnchored[i+1]) [[likely]] {
					// Case 1: Neither anchored - move both toward each other
					rope.Positions[i] += correction;
					rope.Positions[i + 1] -= correction;

					// prevent acces momentum build up
					rope.OldPositions[i] += correction * correctionCoef;
					rope.OldPositions[i + 1] -= correction * correctionCoef;


				}
				else if (rope.IsAnchored[i] && !rope.IsAnchored[i + 1]) {
					// Case 2: A anchored - move B the full error distance toward A
					rope.Positions[i + 1] -= dir * error;

					rope.OldPositions[i + 1] -= dir * error * correctionCoef;
				}
				else if (!rope.IsAnchored[i] && rope.IsAnchored[i + 1]) {
					// Case 3: B anchored - move A the full error distance toward B
					rope.Positions[i] += dir * error;

					rope.OldPositions[i] += dir * error * correctionCoef;
				}


			}
		}
	}
}

//calculates physics and renders all the ropes in one command
void RopePhysicsSolver::HandleRopes(Camera2D& camera, const int substeps, const int iterations, const double deltaTime) {


	for (int i = 0; i < config.physics.ExistingRopes.size(); i++) {
		UpdateRope(config.physics.ExistingRopes[i], camera, substeps, iterations, deltaTime);
		RopeRenderer::RenderRope(camera, config.physics.ExistingRopes[i]);
	}


}


//check overlap of a node with the mouse
Vector2 RopePhysicsSolver::FindNodeToMove(Rope& rope, Camera2D& Camera) {

	if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {

		//get cursor world position
		Vector2 cursorWorldPos = GetScreenToWorld2D(GetMousePosition(), Camera);



		if (config.interaction.draggedRope == nullptr && config.interaction.canDrag) {

			for (int i = 0; i < rope.NodeAmount; i++) { //bruteforce. for every node in a rope, check overlap between the cursor and a node
													// until we find the one that overlaps
				if (Vector2Distance(cursorWorldPos, rope.Positions[i]) < rope.Radius) {
					config.interaction.draggedNodeID = i;	//found the node
					config.interaction.draggedRope = &rope;
					config.interaction.wasAnchored = rope.IsAnchored[config.interaction.draggedNodeID];	//check if it was anchored to return to this state after LMB is no longer being held
					
					break;
				}

			}
		}
	}

	//get the position at which the dragging will start. must call every frame
	Vector2 dragStartFramePos = { 0,0 };
	// Only access the node if this rope is the one being dragged
	if (config.interaction.draggedRope == &rope && config.interaction.draggedNodeID != -1) {
		dragStartFramePos = rope.Positions[config.interaction.draggedNodeID];
	}

	return dragStartFramePos;
}

void RopePhysicsSolver::ToggleAnchor(Rope& rope)
{
	if (config.interaction.draggedRope != nullptr) {

		if (&rope == config.interaction.draggedRope && IsKeyPressed(KEY_LEFT_CONTROL)) {	//if control is pressed (and we are checking thr correct rope), change whether or not the node is anchored

			config.interaction.wasAnchored = !config.interaction.wasAnchored;
			rope.IsAnchored[config.interaction.draggedNodeID] = rope.IsAnchored[config.interaction.draggedNodeID] ? 0 : 1;
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
		rope.IsAnchored[draggedNodeID] = 1;

			Vector2 dir = cursorWorldPos - dragStartFramePos;
			float length = Vector2Length(dir);

			if (length > 0) {

				dir = dir / length;

				//interpolate the position over all sub-steps
				rope.Positions[draggedNodeID] = dragStartFramePos + dir * (float)i / ((float)substeps) * length;
				rope.OldPositions[draggedNodeID] = dragStartFramePos + dir * (float)i / ((float)substeps) * length;

			}

	}

	//stop dragging the node
	if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)) {

		//check if we are in the correct rope
		if (config.interaction.draggedRope != &rope) return;

		if (config.interaction.draggedNodeID != -1) {	//return the node to its state before dragging

			if (config.interaction.wasAnchored) {

				rope.IsAnchored[draggedNodeID] = 1;
			}
			else {

				rope.IsAnchored[draggedNodeID] = 0;
			}
			//reset config
			draggedNodeID = -1;
			config.interaction.draggedRope = nullptr;
		}
	}
}