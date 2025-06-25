#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <vector>
#include <cmath>

// Function to project 4D vertices to 3D
std::vector<Vector3> projectTesseract(const std::vector<std::vector<float>>& vertices, 
                                    float xy, float xz, float xw, 
                                    float yz, float yw, float zw) {
    std::vector<Vector3> projected;
    for (const auto& vertex : vertices) {
        // Apply 4D rotations
        float x = vertex[0];
        float y = vertex[1];
        float z = vertex[2];
        float w = vertex[3];
        
        // Rotate in 4D space
        float x1 = x * cos(xy) - y * sin(xy);
        float y1 = x * sin(xy) + y * cos(xy);
        float z1 = z * cos(xz) - x1 * sin(xz);
        float x2 = x1 * cos(xz) + z * sin(xz);
        float w1 = w * cos(xw) - x2 * sin(xw);
        float x3 = x2 * cos(xw) + w * sin(xw);
        float y2 = y1 * cos(yz) - z1 * sin(yz);
        float z2 = y1 * sin(yz) + z1 * cos(yz);
        float w2 = w1 * cos(yw) - y2 * sin(yw);
        float y3 = y2 * cos(yw) + w1 * sin(yw);
        float z3 = z2 * cos(zw) - w2 * sin(zw);
        float w3 = z2 * sin(zw) + w2 * cos(zw);

        // Project to 3D (perspective projection)
        float scale = 2.0f / (4.0f + w3);
        Vector3 proj = {
            x3 * scale,
            y3 * scale,
            z3 * scale
        };
        projected.push_back(proj);
    }
    return projected;
}

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
    enum Scene { TESSERACT, PLACEHOLDER, COLORED_FACES };
    Scene currentScene = TESSERACT;

    // Colors for tesseract faces (24 unique colors)
    const Color faceColors[24] = {
        (Color){255, 0, 0, 255},       // Red
        (Color){0, 255, 0, 255},      // Green
        (Color){0, 0, 255, 255},      // Blue
        (Color){255, 255, 0, 255},     // Yellow
        (Color){255, 165, 0, 255},     // Orange
        (Color){128, 0, 128, 255},     // Purple
        (Color){0, 191, 255, 255},    // Sky Blue
        (Color){255, 192, 203, 255},  // Pink
        (Color){50, 205, 50, 255},     // Lime Green
        (Color){255, 215, 0, 255},     // Gold
        (Color){138, 43, 226, 255},    // Violet
        (Color){165, 42, 42, 255},     // Brown
        (Color){245, 245, 220, 255},   // Beige
        (Color){255, 0, 255, 255},     // Magenta
        (Color){128, 0, 0, 255},       // Maroon
        (Color){0, 100, 0, 255},       // Dark Green
        (Color){0, 0, 139, 255},       // Dark Blue
        (Color){139, 0, 139, 255},     // Dark Purple
        (Color){101, 67, 33, 255},     // Dark Brown
        (Color){169, 169, 169, 255},   // Dark Gray
        (Color){211, 211, 211, 255},   // Light Gray
        (Color){245, 245, 245, 255},    // Almost White
        (Color){128, 128, 128, 255},   // Gray
        (Color){255, 255, 255, 255}    // White
    };

    // Rotation angle for 4D (only XW axis)
    float angleXW = 0.0f;


    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        if (IsKeyPressed(KEY_SPACE)) {
            currentScene = static_cast<Scene>((currentScene + 1) % 3);
        }

        // Handle zoom
        if (IsKeyDown(KEY_Z)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(Vector3Normalize(camera.position), 0.1f));
        }
        if (IsKeyDown(KEY_X)) {
            camera.position = Vector3Subtract(camera.position, Vector3Scale(Vector3Normalize(camera.position), 0.1f));
        }

        // Update rotation angle (only XW axis)
        angleXW += 0.02f;

        // Draw
        BeginDrawing();
            ClearBackground(RAYWHITE);

            if (currentScene == TESSERACT) {
                BeginMode3D(camera);
                    // Project and draw tesseract
                    auto projectedVertices = projectTesseract(tesseractVertices, 
                        0.0f, 0.0f, angleXW, 0.0f, 0.0f, 0.0f);
                    
                    // Draw edges in red
                    for (const auto& edge : edges) {
                        DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], RED);
                    }
                EndMode3D();
            } else if (currentScene == PLACEHOLDER) {
                // Placeholder scene - tesseract with white lines
                ClearBackground(BLACK);
                BeginMode3D(camera);
                    // Project and draw tesseract
                    auto projectedVertices = projectTesseract(tesseractVertices, 
                        0.0f, 0.0f, angleXW, 0.0f, 0.0f, 0.0f);
                    
                    // Draw edges in white
                    for (const auto& edge : edges) {
                        DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], WHITE);
                    }
                EndMode3D();
            } else {
                // Colored faces scene
                ClearBackground(BLACK);
                BeginMode3D(camera);
                    // Project tesseract
                    auto projectedVertices = projectTesseract(tesseractVertices, 
                        0.0f, 0.0f, angleXW, 0.0f, 0.0f, 0.0f);
                    
                    // Define all 24 square faces of the tesseract
                    int faces[24][4] = {
                        // Inner cube
                        {0, 1, 3, 2}, {4, 5, 7, 6}, 
                        {0, 1, 5, 4}, {2, 3, 7, 6},
                        {0, 2, 6, 4}, {1, 3, 7, 5},
                        
                        // Outer cube
                        {8, 9, 11, 10}, {12, 13, 15, 14},
                        {8, 9, 13, 12}, {10, 11, 15, 14},
                        {8, 10, 14, 12}, {9, 11, 15, 13},
                        
                        // Connecting faces between inner and outer cubes
                        {0, 1, 9, 8}, {1, 3, 11, 9},
                        {2, 3, 11, 10}, {0, 2, 10, 8},
                        {4, 5, 13, 12}, {5, 7, 15, 13},
                        {6, 7, 15, 14}, {4, 6, 14, 12},
                        {0, 4, 12, 8}, {1, 5, 13, 9},
                        {2, 6, 14, 10}, {3, 7, 15, 11}
                    };
                    
                    // Draw each face with a different color
                    for (int i = 0; i < 24; i++) {
                        Vector3 v1 = projectedVertices[faces[i][0]];
                        Vector3 v2 = projectedVertices[faces[i][1]];
                        Vector3 v3 = projectedVertices[faces[i][2]];
                        Vector3 v4 = projectedVertices[faces[i][3]];
                        
                        // Draw the face as a quad
                        rlBegin(RL_QUADS);
                            rlColor4ub(faceColors[i].r, faceColors[i].g, faceColors[i].b, faceColors[i].a);
                            rlVertex3f(v1.x, v1.y, v1.z);
                            rlVertex3f(v2.x, v2.y, v2.z);
                            rlVertex3f(v3.x, v3.y, v3.z);
                            rlVertex3f(v4.x, v4.y, v4.z);
                        rlEnd();
                    }
                    
                    // Draw edges in black for definition
                    for (const auto& edge : edges) {
                        DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], BLACK);
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
