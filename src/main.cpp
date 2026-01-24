
#include "raylib.h"
#include <vector>
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "main.h"
#include "RopeNode.h"
#include "RopePhysicsSolver.h"
#include "PhysicsConfig.h"

#include "GUI_Renderer.h"


int main()
{
	int TargetFPS = 60;

	int DefaultResolutionX = 1200;
	int DefaultResolutionY = 700;


	Camera2D mainCamera = { 0 };	// Camera setup
	mainCamera.zoom = 1;

	Config DefaultConfig;	//set up config, physics, GUI rrendering
	InteractionConfig DefaultInteractionCFG;
	RopePhysicsSolver DefaultSolver(DefaultConfig);
	GUI_Renderer GUI(DefaultConfig);


	Rope Rope1 = DefaultSolver.SetupRope(Vector2{200,100}, true, 9, 40, 10);		//creating 3 example ropes
	Rope Rope2 = DefaultSolver.SetupRope(Vector2{400,100}, true, 27, 22, 7);
	Rope Rope3 = DefaultSolver.SetupRope(Vector2{600,100}, true, 70, 8, 5);


	// Tell the window to use vsync and work on high DPI displays
	// SetConfigFlags(FLAG_VSYNC_HINT);
	//SetConfigFlags(FLAG_WINDOW_HIGHDPI);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(TargetFPS);

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

		double frameTime = 1.0 / TargetFPS;

		DefaultSolver.HandleRopes(mainCamera,6, 4, frameTime); //render all ropes and calculate physics


		EndMode2D(); // end world space drawing

		GUI.Render_GUI();

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
