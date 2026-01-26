#include "CameraController.h"
#include "raylib.h"

void CameraMove(Camera2D& mainCamera)
{
	mainCamera.offset = Vector2{ (float)GetScreenWidth() / 2,(float)GetScreenWidth() / 2 };

	if (GetMouseWheelMove()) {
		mainCamera.zoom += 0.05 * GetMouseWheelMove(); // change zoom based on the mouse wheel movement
	}

	if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {		//move the camera's position by mouse delta when RMB is held
		Vector2 mouseDelta = GetMouseDelta();

		mainCamera.target.x -= mouseDelta.x / mainCamera.zoom;	//moving the camera
		mainCamera.target.y -= mouseDelta.y / mainCamera.zoom;
	}

}