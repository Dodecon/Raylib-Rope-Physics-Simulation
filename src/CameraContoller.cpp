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