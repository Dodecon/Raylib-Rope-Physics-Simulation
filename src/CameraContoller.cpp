#include "main.h"
#include "raymath.h"

void CameraMove(Camera2D& mainCamera)
{

	if (GetMouseWheelMove()) {
		mainCamera.zoom += 0.05 * GetMouseWheelMove(); // change zoom based on the mouse wheel movement
	}

	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {		//move the camera's position by mouse delta when RMB is held
		Vector2 mouseDelta = GetMouseDelta();

		mainCamera.target.x -= mouseDelta.x / mainCamera.zoom;	//moving the camera
		mainCamera.target.y -= mouseDelta.y / mainCamera.zoom;
	}

}

// Static variables to keep state between frames
static RopeNode* draggedNode = nullptr;
static bool wasAnchored = false;


//detect overlap of a ropenode with the cursor while the LMB is held, then moves the node to the cursors position
void MoveRopeNode(std::vector<RopeNode>& ropenodes, const Camera2D& mainCamera) {

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