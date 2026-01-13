
#include "raylib.h"
#include <vector>
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "main.h"
#include "RopeNode.h"
#include "RopePhysicsSolver.h"

#include "GUI_Renderer.h"


int main()
{
	int DefaultResolutionX = 1200;
	int DefaultResolutionY = 700;


	Camera2D mainCamera = { 0 }; // Camera setup
	mainCamera.zoom = 1;

	std::vector<RopeNode> Rope1 = RopePhysicsSolver::SetupRope(Vector2{200,100}, true, 3, 100, 15);		//creating 3 example ropes
	std::vector<RopeNode> Rope2 = RopePhysicsSolver::SetupRope(Vector2{400,100}, true, 9, 40, 10);
	std::vector<RopeNode> Rope3 = RopePhysicsSolver::SetupRope(Vector2{600,100}, true, 50, 8, 5);


	// Tell the window to use vsync and work on high DPI displays
	// SetConfigFlags(FLAG_VSYNC_HINT);
	//SetConfigFlags(FLAG_WINDOW_HIGHDPI);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(120);

	// Enable 4x MSAA anti-aliasing
	SetConfigFlags(FLAG_MSAA_4X_HINT);
	// Create the window and OpenGL context
	InitWindow(DefaultResolutionX, DefaultResolutionY, "Rope Simulation");
	// Utility function from resource_dir.h to find the resources folder and set it as the current working directory so we can load from it
	SearchAndSetResourceDir("resources");

	//load the background image
	Texture2D background = LoadTexture("Background_for_RopeSim.png");
	
	// Set Raygui style
	GuiLoadStyle("style_jungle.rgs");
	GuiSetFont(LoadFontEx("Jersey10-Regular.ttf", 128, 0, 0));


	// game loop
	while (!WindowShouldClose())		// run the loop until the user presses ESCAPE or presses the Close button on the window
	{

		CameraMove(mainCamera);

		// drawing
		BeginDrawing();
		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(RAYWHITE);


		//draw the static background
		DrawFixedBackground(background, GetScreenWidth(), GetScreenHeight());


		BeginMode2D(mainCamera); // start world space drawing

		RopePhysicsSolver::HandleRopes(mainCamera, RopePhysicsSolver::ExistingRopes); //render all ropes and calculate physics


		EndMode2D(); // end world space drawing

		// draw fps at the top left corner of the screen
		DrawFPS(45, 30);
		GUI_Renderer::Render_GUI();

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}


	// destroy the window and cleanup the OpenGL context
	CloseWindow();

	return 0;
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
