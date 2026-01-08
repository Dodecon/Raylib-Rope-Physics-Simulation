
#include "raylib.h"
#include <vector>
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "main.h"
#include "Vector2_Operators.h"
#include "RopeNode.h"
#include "RopePhysicsSolver.h"




int main()
{
	int ResolutionX = 1200;
	int ResolutionY = 800;


	Camera2D mainCamera = { 0 }; // Camera setup
	mainCamera.zoom = 1;

	std::vector<RopeNode> Rope1 = RopePhysicsSolver::SetupRope(Vector2{200,100}, true, 3, 100, 15);		//creating 3 example ropes
	std::vector<RopeNode> Rope2 = RopePhysicsSolver::SetupRope(Vector2{400,100}, true, 9, 40, 10);
	std::vector<RopeNode> Rope3 = RopePhysicsSolver::SetupRope(Vector2{600,100}, true, 50, 8, 5);


	// Tell the window to use vsync and work on high DPI displays
	// SetConfigFlags(FLAG_VSYNC_HINT);
	//SetConfigFlags(FLAG_WINDOW_HIGHDPI);
	SetTargetFPS(120);

	// Enable 4x MSAA anti-aliasing
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	// Create the window and OpenGL context
	InitWindow(ResolutionX, ResolutionY, "Rope Simulation");
	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	//load the background image
	Texture2D background = LoadTexture("Background_for_RopeSim.png");



	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{

		CameraMove(mainCamera);



		// drawing
		BeginDrawing();
		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(RAYWHITE);
		//draw the static background
		DrawFixedBackground(background, ResolutionX, ResolutionY);

		// draw fps at the top left corner of the screen
		DrawFPS(20, 30);

		BeginMode2D(mainCamera); // start world space drawing

		HandleRopes(mainCamera, RopePhysicsSolver::ExistingRopes); //render all ropes and calculate physics


		EndMode2D(); // end world space drawing

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}


	// destroy the window and cleanup the OpenGL context
	CloseWindow();

	return 0;
}

void HandleRopes(Camera2D& mainCamera, std::vector<std::vector<RopeNode>>& ExistingRopes) {

	for (std::vector<RopeNode>& rope : ExistingRopes) {
		MoveRopeNode(rope, mainCamera);
		RopePhysicsSolver::UpdateRope(rope, 0.0083333);  //using fixed time step (1/120) for best simulation results
	}
}

void DrawFixedBackground(Texture2D background, int screenWidth, int screenHeight) {
	// Calculate scale to fill screen while maintaining aspect ratio
	float scaleX = (float)screenWidth / background.width;
	float scaleY = (float)screenHeight / background.height;
	float scale = (scaleX > scaleY) ? scaleX : scaleY; // Use larger scale to cover entire screen

	// Center the background on screen
	Vector2 position = {
		(screenWidth - (background.width * scale)) / 2.0f,
		(screenHeight - (background.height * scale)) / 2.0f
	};

	DrawTextureEx(background, position, 0.0f, scale, WHITE);
}
