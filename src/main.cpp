
#include "raylib.h"
#include <vector>
#include "resource_dir.h"	// utility header for SearchAndSetResourceDir
#include "CameraController.h"
#include "RopeNode.h"
#include "RopePhysicsSolver.h"
#include "PhysicsConfig.h"

#include "GUI_Renderer.h"


int main()
{
	int DefaultResolutionX = 1200;
	int DefaultResolutionY = 700;


	Camera2D mainCamera = { 0 };	// Camera setup
	mainCamera.zoom = 1;
	//center the camera
	mainCamera.target = Vector2{ (float)DefaultResolutionX / 2 , (float)DefaultResolutionY / 2 };
	mainCamera.offset = Vector2{ (float)DefaultResolutionX / 2,(float)DefaultResolutionY / 2 };


	Config DefaultConfig;	//set up config, physics, GUI rrendering
	InteractionConfig DefaultInteractionCFG;
	Threadpool threadpool(std::thread::hardware_concurrency());

	RopePhysicsSolver DefaultSolver(DefaultConfig, threadpool);
	GUI_Renderer GUI(DefaultSolver, DefaultConfig);


	DefaultSolver.SetupRope(Vector2{200,100}, true, 9, 40, 10);		//creating 3 example ropes
	DefaultSolver.SetupRope(Vector2{400,100}, true, 27, 22, 7);
	DefaultSolver.SetupRope(Vector2{600,100}, true, 50, 8, 5);


	// Tell the window to use vsync and work on high DPI displays
	// SetConfigFlags(FLAG_VSYNC_HINT);
	//SetConfigFlags(FLAG_WINDOW_HIGHDPI);

	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	SetTargetFPS(DefaultConfig.TargetFPS);

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
	while (!WindowShouldClose())  // run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		CameraMove(mainCamera);

		// drawing
		BeginDrawing();
		// Setup the back buffer for drawing (clear color and depth buffers)
		ClearBackground(RAYWHITE);

		GUI.DrawFixedBackground(background, GetScreenWidth(), GetScreenHeight());

		BeginMode2D(mainCamera); // start world space drawing

		double frameTime = 1.0 / DefaultConfig.TargetFPS;

		DefaultSolver.HandleRopes(mainCamera, 6, 4, frameTime); //render all ropes and calculate physics


		EndMode2D(); // end world space drawing

		GUI.Render_GUI();

		// end the frame and get ready for the next one  (display frame, poll input, etc...)
		EndDrawing();
	}


	// destroy the window and cleanup the OpenGL context
	CloseWindow();

	return 0;
}
