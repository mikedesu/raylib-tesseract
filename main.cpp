#include "raylib.h"
#include "rlgl.h"  // For rlSetLineWidth

int main(void)
{
    // Initialization
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "3D Cube Example");

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 0.0f, 10.0f, 10.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };    // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };        // Camera up vector
    camera.fovy = 45.0f;                              // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;           // Camera projection type

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        // TODO: Update your variables here

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);

            BeginMode3D(camera);
                // Set line width and draw cube
                static float rotation = 0.0f;
                rotation += 0.5f;
                
                rlDisableBackfaceCulling(); // Needed for thicker lines
                rlSetLineWidth(10.0f); // Much thicker lines
                
                rlPushMatrix();
                    rlRotatef(rotation, 1.0f, 1.0f, 1.0f); // Rotate the cube
                    DrawCubeWires((Vector3){ 0.0f, 0.0f, 0.0f }, 2.0f, 2.0f, 2.0f, RED);
                rlPopMatrix();
                
                rlSetLineWidth(1.0f); // Reset to default
                rlEnableBackfaceCulling();
            EndMode3D();

            DrawFPS(10, 10);
        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context

    return 0;
}
