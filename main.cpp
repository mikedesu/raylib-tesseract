#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <vector>
#include <cmath>

// Function to project 4D vertices to 3D
std::vector<Vector3> projectTesseract(
    const std::vector<std::vector<float>>& vertices, float xy, float xz, float xw, float yz, float yw, float zw) {
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
        Vector3 proj = {x3 * scale, y3 * scale, z3 * scale};
        projected.push_back(proj);
    }
    return projected;
}

int main(void) {
    // Initialization
    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "3D Cube Example");

    // Define the camera to look into our 3d world
    Camera3D camera = {0};
    camera.position = (Vector3){10.0f, 10.0f, 10.0f}; // Camera position
    camera.target = (Vector3){0.0f, 0.0f, 0.0f}; // Camera looking at point
    camera.up = (Vector3){0.0f, 1.0f, 0.0f}; // Camera up vector
    camera.fovy = 45.0f; // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE; // Camera projection type

    // Define 4D cube (tesseract) vertices
    std::vector<std::vector<float>> tesseractVertices = {{-1, -1, -1, -1},
                                                         {-1, -1, -1, 1},
                                                         {-1, -1, 1, -1},
                                                         {-1, -1, 1, 1},
                                                         {-1, 1, -1, -1},
                                                         {-1, 1, -1, 1},
                                                         {-1, 1, 1, -1},
                                                         {-1, 1, 1, 1},
                                                         {1, -1, -1, -1},
                                                         {1, -1, -1, 1},
                                                         {1, -1, 1, -1},
                                                         {1, -1, 1, 1},
                                                         {1, 1, -1, -1},
                                                         {1, 1, -1, 1},
                                                         {1, 1, 1, -1},
                                                         {1, 1, 1, 1}};

    // Define edges between vertices
    std::vector<std::pair<int, int>> edges = {
        {0, 1},   {0, 2},   {0, 4},  {1, 3},  {1, 5},  {2, 3},  {2, 6},   {3, 7},   {4, 5},   {4, 6},   {5, 7},
        {6, 7},   {8, 9},   {8, 10}, {8, 12}, {9, 11}, {9, 13}, {10, 11}, {10, 14}, {11, 15}, {12, 13}, {12, 14},
        {13, 15}, {14, 15}, {0, 8},  {1, 9},  {2, 10}, {3, 11}, {4, 12},  {5, 13},  {6, 14},  {7, 15}};

    // Scene management
    enum Scene {
        TESSERACT,
        PLACEHOLDER,
        COLORED_FACES,
        PYRAMID_BLACK_LINES,
        PYRAMID_WHITE_LINES,
        PYRAMID_COLORED_FACES,
        PENTAGON_BLACK_LINES,
        PENTAGON_WHITE_LINES,
        PENTAGON_COLORED_FACES,
        HEXAGON_BLACK_LINES,
        HEXAGON_WHITE_LINES,
        HEXAGON_COLORED_FACES
    };
    Scene currentScene = TESSERACT;

    // Colors for tesseract faces (24 unique colors)
    const Color faceColors[24] = {
        (Color){255, 0, 0, 255}, // Red
        (Color){0, 255, 0, 255}, // Green
        (Color){0, 0, 255, 255}, // Blue
        (Color){255, 255, 0, 255}, // Yellow
        (Color){255, 165, 0, 255}, // Orange
        (Color){128, 0, 128, 255}, // Purple
        (Color){0, 191, 255, 255}, // Sky Blue
        (Color){255, 192, 203, 255}, // Pink
        (Color){50, 205, 50, 255}, // Lime Green
        (Color){255, 215, 0, 255}, // Gold
        (Color){138, 43, 226, 255}, // Violet
        (Color){165, 42, 42, 255}, // Brown
        (Color){245, 245, 220, 255}, // Beige
        (Color){255, 0, 255, 255}, // Magenta
        (Color){128, 0, 0, 255}, // Maroon
        (Color){0, 100, 0, 255}, // Dark Green
        (Color){0, 0, 139, 255}, // Dark Blue
        (Color){139, 0, 139, 255}, // Dark Purple
        (Color){101, 67, 33, 255}, // Dark Brown
        (Color){169, 169, 169, 255}, // Dark Gray
        (Color){211, 211, 211, 255}, // Light Gray
        (Color){245, 245, 245, 255}, // Almost White
        (Color){128, 128, 128, 255}, // Gray
        (Color){255, 255, 255, 255} // White
    };

    // Rotation angles for 4D
    float angleXY = 0.0f;
    float angleXZ = 0.0f;
    float angleXW = 0.0f;
    float angleYZ = 0.0f;
    float angleYW = 0.0f;
    float angleZW = 0.0f;

    // Current rotation axis index
    int currentAxis = 2; // Start with XW
    const char* axisNames[6] = {"XY", "XZ", "XW", "YZ", "YW", "ZW"};


    SetTargetFPS(60); // Set our game to run at 60 frames-per-second

    // Main game loop
    while (!WindowShouldClose()) // Detect window close button or ESC key
    {
        // Update
        if (IsKeyPressed(KEY_SPACE)) {
            currentScene = static_cast<Scene>((currentScene + 1) % 12);
        }

        // Handle zoom
        if (IsKeyDown(KEY_Z)) {
            camera.position = Vector3Add(camera.position, Vector3Scale(Vector3Normalize(camera.position), 0.1f));
        }
        if (IsKeyDown(KEY_X)) {
            camera.position = Vector3Subtract(camera.position, Vector3Scale(Vector3Normalize(camera.position), 0.1f));
        }

        // Handle axis cycling
        if (IsKeyPressed(KEY_RIGHT)) {
            currentAxis = (currentAxis + 1) % 6;
        }
        if (IsKeyPressed(KEY_LEFT)) {
            currentAxis = (currentAxis + 5) % 6; // Equivalent to -1 but wraps correctly
        }

        // Update rotation angle for current axis
        switch (currentAxis) {
        case 0:
            angleXY += 0.02f;
            break;
        case 1:
            angleXZ += 0.02f;
            break;
        case 2:
            angleXW += 0.02f;
            break;
        case 3:
            angleYZ += 0.02f;
            break;
        case 4:
            angleYW += 0.02f;
            break;
        case 5:
            angleZW += 0.02f;
            break;
        }

        // Define 4D pyramid vertices
        std::vector<std::vector<float>> pyramidVertices = {
            {0, 0, 0, 0}, // Apex
            {1, 1, 1, 1}, // Base vertex 1
            {1, -1, 1, 1}, // Base vertex 2
            {-1, -1, 1, 1}, // Base vertex 3
            {-1, 1, 1, 1}, // Base vertex 4
            {1, 1, -1, 1}, // Base vertex 5
            {1, -1, -1, 1}, // Base vertex 6
            {-1, -1, -1, 1}, // Base vertex 7
            {-1, 1, -1, 1} // Base vertex 8
        };

        // Define pyramid edges
        std::vector<std::pair<int, int>> pyramidEdges = {
            {0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}, {0, 8}, // Apex to base
            {1, 2}, {2, 3}, {3, 4}, {4, 1}, // Base square 1
            {5, 6}, {6, 7}, {7, 8}, {8, 5}, // Base square 2
            {1, 5}, {2, 6}, {3, 7}, {4, 8} // Connecting edges
        };

        // Define pyramid faces
        int pyramidFaces[16][4] = {// Triangular faces from apex
                                   {0, 1, 2, 2},
                                   {0, 2, 3, 3},
                                   {0, 3, 4, 4},
                                   {0, 4, 1, 1},
                                   {0, 5, 6, 6},
                                   {0, 6, 7, 7},
                                   {0, 7, 8, 8},
                                   {0, 8, 5, 5},
                                   // Base faces
                                   {1, 2, 6, 5},
                                   {2, 3, 7, 6},
                                   {3, 4, 8, 7},
                                   {4, 1, 5, 8}};

        // Define 4D pentagon vertices
        std::vector<std::vector<float>> pentagonVertices = {
            {0, 0, 0, 0}, // Center
            {1, 0, 0, 1}, // Vertex 1
            {0.309, 0.951, 0, 1}, // Vertex 2
            {-0.809, 0.588, 0, 1}, // Vertex 3
            {-0.809, -0.588, 0, 1}, // Vertex 4
            {0.309, -0.951, 0, 1}, // Vertex 5
            {1, 0, 0, -1}, // Vertex 6
            {0.309, 0.951, 0, -1}, // Vertex 7
            {-0.809, 0.588, 0, -1}, // Vertex 8
            {-0.809, -0.588, 0, -1}, // Vertex 9
            {0.309, -0.951, 0, -1} // Vertex 10
        };

        // Define pentagon edges
        std::vector<std::pair<int, int>> pentagonEdges = {// Bottom pentagon
                                                          {0, 1},
                                                          {0, 2},
                                                          {0, 3},
                                                          {0, 4},
                                                          {0, 5},
                                                          {1, 2},
                                                          {2, 3},
                                                          {3, 4},
                                                          {4, 5},
                                                          {5, 1},
                                                          // Top pentagon
                                                          {0, 6},
                                                          {0, 7},
                                                          {0, 8},
                                                          {0, 9},
                                                          {0, 10},
                                                          {6, 7},
                                                          {7, 8},
                                                          {8, 9},
                                                          {9, 10},
                                                          {10, 6},
                                                          // Connecting edges
                                                          {1, 6},
                                                          {2, 7},
                                                          {3, 8},
                                                          {4, 9},
                                                          {5, 10}};

        // Define pentagon faces
        int pentagonFaces[15][5] = {// Bottom faces
                                    {0, 1, 2, 2, 2},
                                    {0, 2, 3, 3, 3},
                                    {0, 3, 4, 4, 4},
                                    {0, 4, 5, 5, 5},
                                    {0, 5, 1, 1, 1},
                                    // Top faces
                                    {0, 6, 7, 7, 7},
                                    {0, 7, 8, 8, 8},
                                    {0, 8, 9, 9, 9},
                                    {0, 9, 10, 10, 10},
                                    {0, 10, 6, 6, 6},
                                    // Side faces
                                    {1, 2, 7, 6, 6},
                                    {2, 3, 8, 7, 7},
                                    {3, 4, 9, 8, 8},
                                    {4, 5, 10, 9, 9},
                                    {5, 1, 6, 10, 10}};

        // Define 4D hexagon vertices
        std::vector<std::vector<float>> hexagonVertices = {
            {0, 0, 0, 0},    // Center
            {1, 0, 0, 1},    // Vertex 1
            {0.5, 0.866, 0, 1}, // Vertex 2
            {-0.5, 0.866, 0, 1}, // Vertex 3
            {-1, 0, 0, 1}, // Vertex 4
            {-0.5, -0.866, 0, 1}, // Vertex 5
            {0.5, -0.866, 0, 1}, // Vertex 6
            {1, 0, 0, -1},   // Vertex 7
            {0.5, 0.866, 0, -1}, // Vertex 8
            {-0.5, 0.866, 0, -1}, // Vertex 9
            {-1, 0, 0, -1}, // Vertex 10
            {-0.5, -0.866, 0, -1}, // Vertex 11
            {0.5, -0.866, 0, -1}  // Vertex 12
        };

        // Define hexagon edges
        std::vector<std::pair<int, int>> hexagonEdges = {
            // Bottom hexagon
            {0,1}, {0,2}, {0,3}, {0,4}, {0,5}, {0,6},
            {1,2}, {2,3}, {3,4}, {4,5}, {5,6}, {6,1},
            // Top hexagon
            {0,7}, {0,8}, {0,9}, {0,10}, {0,11}, {0,12},
            {7,8}, {8,9}, {9,10}, {10,11}, {11,12}, {12,7},
            // Connecting edges
            {1,7}, {2,8}, {3,9}, {4,10}, {5,11}, {6,12}
        };

        // Define hexagon faces
        int hexagonFaces[14][6] = {
            // Bottom faces
            {0,1,2,2,2,2}, {0,2,3,3,3,3}, {0,3,4,4,4,4},
            {0,4,5,5,5,5}, {0,5,6,6,6,6}, {0,6,1,1,1,1},
            // Top faces
            {0,7,8,8,8,8}, {0,8,9,9,9,9}, {0,9,10,10,10,10},
            {0,10,11,11,11,11}, {0,11,12,12,12,12}, {0,12,7,7,7,7},
            // Side faces
            {1,2,8,7,7,7}, {2,3,9,8,8,8}, {3,4,10,9,9,9},
            {4,5,11,10,10,10}, {5,6,12,11,11,11}, {6,1,7,12,12,12}
        };

        // Draw
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (currentScene == TESSERACT) {
            BeginMode3D(camera);
            // Project and draw tesseract
            auto projectedVertices =
                projectTesseract(tesseractVertices, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);

            // Draw edges in red
            for (const auto& edge : edges) {
                DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], RED);
            }
            rlEnableBackfaceCulling(); // Re-enable backface culling
            EndMode3D();
        } else if (currentScene == PLACEHOLDER) {
            // Placeholder scene - tesseract with white lines
            ClearBackground(BLACK);
            BeginMode3D(camera);
            // Project and draw tesseract
            auto projectedVertices =
                projectTesseract(tesseractVertices, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);

            // Draw edges in white
            for (const auto& edge : edges) {
                DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], WHITE);
            }
            EndMode3D();
        } else if (currentScene == COLORED_FACES) {
            // Colored faces scene
            ClearBackground(BLACK);
            BeginMode3D(camera);
            rlDisableBackfaceCulling(); // Disable backface culling
            // Project tesseract
            auto projectedVertices =
                projectTesseract(tesseractVertices, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);

            // Define all 24 square faces of the tesseract
            int faces[24][4] = {// Inner cube
                                {0, 1, 3, 2},
                                {4, 5, 7, 6},
                                {0, 1, 5, 4},
                                {2, 3, 7, 6},
                                {0, 2, 6, 4},
                                {1, 3, 7, 5},

                                // Outer cube
                                {8, 9, 11, 10},
                                {12, 13, 15, 14},
                                {8, 9, 13, 12},
                                {10, 11, 15, 14},
                                {8, 10, 14, 12},
                                {9, 11, 15, 13},

                                // Connecting faces between inner and outer cubes
                                {0, 1, 9, 8},
                                {1, 3, 11, 9},
                                {2, 3, 11, 10},
                                {0, 2, 10, 8},
                                {4, 5, 13, 12},
                                {5, 7, 15, 13},
                                {6, 7, 15, 14},
                                {4, 6, 14, 12},
                                {0, 4, 12, 8},
                                {1, 5, 13, 9},
                                {2, 6, 14, 10},
                                {3, 7, 15, 11}};




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
        } else if (currentScene == PYRAMID_BLACK_LINES) {
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
            // Project and draw pyramid
            auto projectedVertices =
                projectTesseract(pyramidVertices, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);

            // Draw edges in black
            for (const auto& edge : pyramidEdges) {
                DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], BLACK);
            }
            EndMode3D();
        } else if (currentScene == PYRAMID_WHITE_LINES) {
            ClearBackground(BLACK);
            BeginMode3D(camera);
            // Project and draw pyramid
            auto projectedVertices =
                projectTesseract(pyramidVertices, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);

            // Draw edges in white
            for (const auto& edge : pyramidEdges) {
                DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], WHITE);
            }
            EndMode3D();
        } else if (currentScene == PYRAMID_COLORED_FACES) {
            ClearBackground(BLACK);
            BeginMode3D(camera);
            rlDisableBackfaceCulling();
            // Project pyramid
            auto projectedVertices =
                projectTesseract(pyramidVertices, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);

            // Draw each face with a different color
            for (int i = 0; i < 16; i++) {
                Vector3 v1 = projectedVertices[pyramidFaces[i][0]];
                Vector3 v2 = projectedVertices[pyramidFaces[i][1]];
                Vector3 v3 = projectedVertices[pyramidFaces[i][2]];
                Vector3 v4 = projectedVertices[pyramidFaces[i][3]];

                rlBegin(RL_QUADS);
                rlColor4ub(faceColors[i].r, faceColors[i].g, faceColors[i].b, faceColors[i].a);
                rlVertex3f(v1.x, v1.y, v1.z);
                rlVertex3f(v2.x, v2.y, v2.z);
                rlVertex3f(v3.x, v3.y, v3.z);
                rlVertex3f(v4.x, v4.y, v4.z);
                rlEnd();
            }

            // Draw edges in black for definition
            for (const auto& edge : pyramidEdges) {
                DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], BLACK);
            }
            EndMode3D();
        } else if (currentScene == PENTAGON_BLACK_LINES) {
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
            // Project and draw pentagon
            auto projectedVertices =
                projectTesseract(pentagonVertices, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);

            // Draw edges in black
            for (const auto& edge : pentagonEdges) {
                DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], BLACK);
            }
            EndMode3D();
        } else if (currentScene == PENTAGON_WHITE_LINES) {
            ClearBackground(BLACK);
            BeginMode3D(camera);
            // Project and draw pentagon
            auto projectedVertices =
                projectTesseract(pentagonVertices, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);

            // Draw edges in white
            for (const auto& edge : pentagonEdges) {
                DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], WHITE);
            }
            EndMode3D();
        } else if (currentScene == PENTAGON_COLORED_FACES) {
            ClearBackground(BLACK);
            BeginMode3D(camera);
            rlDisableBackfaceCulling();
            // Project pentagon
            auto projectedVertices =
                projectTesseract(pentagonVertices, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);

            // Draw each face with a different color
            for (int i = 0; i < 12; i++) {
                Vector3 v1 = projectedVertices[pentagonFaces[i][0]];
                Vector3 v2 = projectedVertices[pentagonFaces[i][1]];
                Vector3 v3 = projectedVertices[pentagonFaces[i][2]];
                Vector3 v4 = projectedVertices[pentagonFaces[i][3]];
                Vector3 v5 = projectedVertices[pentagonFaces[i][4]];

                rlBegin(RL_TRIANGLES);
                rlColor4ub(faceColors[i].r, faceColors[i].g, faceColors[i].b, faceColors[i].a);
                rlVertex3f(v1.x, v1.y, v1.z);
                rlVertex3f(v2.x, v2.y, v2.z);
                rlVertex3f(v3.x, v3.y, v3.z);
                rlVertex3f(v1.x, v1.y, v1.z);
                rlVertex3f(v3.x, v3.y, v3.z);
                rlVertex3f(v4.x, v4.y, v4.z);
                rlVertex3f(v1.x, v1.y, v1.z);
                rlVertex3f(v4.x, v4.y, v4.z);
                rlVertex3f(v5.x, v5.y, v5.z);
                rlEnd();
            }

            // Draw edges in black for definition
            for (const auto& edge : pentagonEdges) {
                DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], BLACK);
            }
            EndMode3D();
        } else if (currentScene == HEXAGON_BLACK_LINES) {
            ClearBackground(RAYWHITE);
            BeginMode3D(camera);
                // Project and draw hexagon
                auto projectedVertices = 
                    projectTesseract(hexagonVertices, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);
                
                // Draw edges in black
                for (const auto& edge : hexagonEdges) {
                    DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], BLACK);
                }
            EndMode3D();
        } else if (currentScene == HEXAGON_WHITE_LINES) {
            ClearBackground(BLACK);
            BeginMode3D(camera);
                // Project and draw hexagon
                auto projectedVertices = 
                    projectTesseract(hexagonVertices, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);
                
                // Draw edges in white
                for (const auto& edge : hexagonEdges) {
                    DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], WHITE);
                }
            EndMode3D();
        } else if (currentScene == HEXAGON_COLORED_FACES) {
            ClearBackground(BLACK);
            BeginMode3D(camera);
                rlDisableBackfaceCulling();
                // Project hexagon
                auto projectedVertices = 
                    projectTesseract(hexagonVertices, angleXY, angleXZ, angleXW, angleYZ, angleYW, angleZW);
                
                // Draw each face with a different color
                for (int i = 0; i < 14; i++) {
                    Vector3 v1 = projectedVertices[hexagonFaces[i][0]];
                    Vector3 v2 = projectedVertices[hexagonFaces[i][1]];
                    Vector3 v3 = projectedVertices[hexagonFaces[i][2]];
                    Vector3 v4 = projectedVertices[hexagonFaces[i][3]];
                    Vector3 v5 = projectedVertices[hexagonFaces[i][4]];
                    Vector3 v6 = projectedVertices[hexagonFaces[i][5]];
                    
                    rlBegin(RL_TRIANGLES);
                        rlColor4ub(faceColors[i].r, faceColors[i].g, faceColors[i].b, faceColors[i].a);
                        rlVertex3f(v1.x, v1.y, v1.z);
                        rlVertex3f(v2.x, v2.y, v2.z);
                        rlVertex3f(v3.x, v3.y, v3.z);
                        rlVertex3f(v1.x, v1.y, v1.z);
                        rlVertex3f(v3.x, v3.y, v3.z);
                        rlVertex3f(v4.x, v4.y, v4.z);
                        rlVertex3f(v1.x, v1.y, v1.z);
                        rlVertex3f(v4.x, v4.y, v4.z);
                        rlVertex3f(v5.x, v5.y, v5.z);
                        rlVertex3f(v1.x, v1.y, v1.z);
                        rlVertex3f(v5.x, v5.y, v5.z);
                        rlVertex3f(v6.x, v6.y, v6.z);
                    rlEnd();
                }
                
                // Draw edges in black for definition
                for (const auto& edge : hexagonEdges) {
                    DrawLine3D(projectedVertices[edge.first], projectedVertices[edge.second], BLACK);
                }
            EndMode3D();
        }

        // Draw current rotation axis and info text (centered)
        const char* axisText = TextFormat("Rotation Axis: %s", axisNames[currentAxis]);
        int axisWidth = MeasureText(axisText, 30);
        DrawText(axisText, (screenWidth - axisWidth) / 2, 40, 30, LIGHTGRAY);

        const char* titleText = "4D Tesseract";
        int titleWidth = MeasureText(titleText, 30);
        DrawText(titleText, (screenWidth - titleWidth) / 2, 80, 30, LIGHTGRAY);

        const char* projectionText = "(3D Projection)";
        int projectionWidth = MeasureText(projectionText, 30);
        DrawText(projectionText, (screenWidth - projectionWidth) / 2, 120, 30, LIGHTGRAY);

        const char* creditText = "Vibe Coded With Deepseek";
        int creditWidth = MeasureText(creditText, 30);
        DrawText(creditText, (screenWidth - creditWidth) / 2, 160, 30, LIGHTGRAY);

        DrawFPS(10, 10);
        EndDrawing();
    }

    // De-Initialization
    CloseWindow(); // Close window and OpenGL context

    return 0;
}
