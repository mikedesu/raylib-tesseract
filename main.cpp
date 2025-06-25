#include "raylib.h"
#include "rlgl.h"
#include <vector>
#include <cmath>

int main(void)
{
    // Initialization
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "3D Cube Example");

    // Define the camera to look into our 3d world
    Camera3D camera = { 0 };
    camera.position = (Vector3){ 10.0f, 10.0f, 10.0f }; // Camera position
    camera.target = (Vector3){ 0.0f, 0.0f, 0.0f };    // Camera looking at point
    camera.up = (Vector3){ 0.0f, 1.0f, 0.0f };        // Camera up vector
    camera.fovy = 45.0f;                              // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;           // Camera projection type

    // Define 4D cube (tesseract) vertices
    std::vector<std::vector<float>> tesseractVertices = {
        {-1, -1, -1, -1}, {-1, -1, -1, 1}, {-1, -1, 1, -1}, {-1, -1, 1, 1},
        {-1, 1, -1, -1}, {-1, 1, -1, 1}, {-1, 1, 1, -1}, {-1, 1, 1, 1},
        {1, -1, -1, -1}, {1, -1, -1, 1}, {1, -1, 1, -1}, {1, -1, 1, 1},
        {1, 1, -1, -1}, {1, 1, -1, 1}, {1, 1, 1, -1}, {1, 1, 1, 1}
    };

    // Define edges between vertices
    std::vector<std::pair<int, int>> edges = {
        {0,1}, {0,2}, {0,4}, {1,3}, {1,5}, {2,3}, {2,6}, {3,7},
        {4,5}, {4,6}, {5,7}, {6,7}, {8,9}, {8,10}, {8,12}, {9,11},
        {9,13}, {10,11}, {10,14}, {11,15}, {12,13}, {12,14}, {13,15},
        {14,15}, {0,8}, {1,9}, {2,10}, {3,11}, {4,12}, {5,13}, {6,14}, {7,15}
    };

    // Scene management
    enum Scene { TESSERACT, PLACEHOLDER };
    Scene currentScene = TESSERACT;

    // Rotation angles for 4D
    float angleXY = 0.0f;
    float angleXZ = 0.0f;
    float angleXW = 0.0f;
    float angleYZ = 0.0f;
    float angleYW = 0.0f;
    float angleZW = 0.0f;

    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        if (IsKeyPressed(KEY_SPACE)) {
            currentScene = (currentScene == TESSERACT) ? PLACEHOLDER : TESSERACT;
        }

        if (currentScene == TESSERACT) {
            angleXY += 0.01f;
            angleXZ += 0.02f;
            angleXW += 0.03f;
            angleYZ += 0.015f;
            angleYW += 0.025f;
            angleZW += 0.035f;
        }

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);

            if (currentScene == TESSERACT) {
                BeginMode3D(camera);
                    // Project 4D to 3D and draw edges
                    std::vector<Vector3> projectedVertices;
                    for (const auto& vertex : tesseractVertices) {
                        // Apply 4D rotations
                        float x = vertex[0];
                        float y = vertex[1];
                        float z = vertex[2];
                        float w = vertex[3];
                        
                        // Rotate in 4D space
                        float x1 = x * cos(angleXY) - y * sin(angleXY);
                        float y1 = x * sin(angleXY) + y * cos(angleXY);
                        float z1 = z * cos(angleXZ) - x1 * sin(angleXZ);
                        float x2 = x1 * cos(angleXZ) + z * sin(angleXZ);
                        float w1 = w * cos(angleXW) - x2 * sin(angleXW);
                        float x3 = x2 * cos(angleXW) + w * sin(angleXW);
                        float y2 = y1 * cos(angleYZ) - z1 * sin(angleYZ);
                        float z2 = y1 * sin(angleYZ) + z1 * cos(angleYZ);
                        float w2 = w1 * cos(angleYW) - y2 * sin(angleYW);
                        float y3 = y2 * cos(angleYW) + w1 * sin(angleYW);
                        float z3 = z2 * cos(angleZW) - w2 * sin(angleZW);
                        float w3 = z2 * sin(angleZW) + w2 * cos(angleZW);

                        // Project to 3D (perspective projection)
                        float scale = 2.0f / (4.0f + w3);
                        Vector3 proj = {
                            x3 * scale,
                            y3 * scale,
                            z3 * scale
                        };
                        projectedVertices.push_back(proj);
                    }

                    // Draw edges
                    for (const auto& edge : edges) {
                        DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], RED);
                    }
                EndMode3D();
            } else {
                // Placeholder scene - tesseract with white lines
                ClearBackground(BLACK);
                BeginMode3D(camera);
                    // Project 4D to 3D and draw edges
                    std::vector<Vector3> projectedVertices;
                    for (const auto& vertex : tesseractVertices) {
                        // Apply 4D rotations
                        float x = vertex[0];
                        float y = vertex[1];
                        float z = vertex[2];
                        float w = vertex[3];
                        
                        // Rotate in 4D space
                        float x1 = x * cos(angleXY) - y * sin(angleXY);
                        float y1 = x * sin(angleXY) + y * cos(angleXY);
                        float z1 = z * cos(angleXZ) - x1 * sin(angleXZ);
                        float x2 = x1 * cos(angleXZ) + z * sin(angleXZ);
                        float w1 = w * cos(angleXW) - x2 * sin(angleXW);
                        float x3 = x2 * cos(angleXW) + w * sin(angleXW);
                        float y2 = y1 * cos(angleYZ) - z1 * sin(angleYZ);
                        float z2 = y1 * sin(angleYZ) + z1 * cos(angleYZ);
                        float w2 = w1 * cos(angleYW) - y2 * sin(angleYW);
                        float y3 = y2 * cos(angleYW) + w1 * sin(angleYW);
                        float z3 = z2 * cos(angleZW) - w2 * sin(angleZW);
                        float w3 = z2 * sin(angleZW) + w2 * cos(angleZW);

                        // Project to 3D (perspective projection)
                        float scale = 2.0f / (4.0f + w3);
                        Vector3 proj = {
                            x3 * scale,
                            y3 * scale,
                            z3 * scale
                        };
                        projectedVertices.push_back(proj);
                    }

                    // Draw edges in white
                    for (const auto& edge : edges) {
                        DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], WHITE);
                    }
                EndMode3D();
            }

            DrawFPS(10, 10);
        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context

    return 0;
}
