/*******************************************************************************************
*
*   raygui - standalone window
*
*   DEPENDENCIES:
*       raylib 2.1  - Windowing/input management and drawing.
*       raygui 2.0  - Immediate-mode GUI controls.
*
*   COMPILATION (Windows - MinGW):
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -I../../src -lraylib -lopengl32 -lgdi32 -std=c99
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2018 raylib technologies (@raylibtech)
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 600;
    
    SetConfigFlags(FLAG_WINDOW_UNDECORATED);
    InitWindow(screenWidth, screenHeight, "raygui - standalone window");

    // GUI controls initialization
    //----------------------------------------------------------------------------------
    bool exitWindow = false;
    //----------------------------------------------------------------------------------
    
    // General variables
    Vector2 mousePos = { 0 };
    Vector2 windowPos = { 500, 200 };
    Vector2 panOffset = mousePos;
    bool dragWindow = false;
    
    SetWindowPosition(windowPos.x, windowPos.y);
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!exitWindow && !WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        mousePos = GetMousePosition();
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            if (CheckCollisionPointRec(mousePos, (Rectangle){ 0, 0, screenWidth, 20 }))
            {
                dragWindow = true;
                panOffset = mousePos;
            }
        }

        if (dragWindow)
        {            
            windowPos.x += (mousePos.x - panOffset.x);
            windowPos.y += (mousePos.y - panOffset.y);
            
            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) dragWindow = false;

            SetWindowPosition(windowPos.x, windowPos.y);
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            exitWindow = GuiWindowBox((Rectangle){ 0, 0, screenWidth, screenHeight }, "STANDALONE WINDOW");
            DrawText(FormatText("Mouse Position: [ %.0f, %.0f ]", mousePos.x, mousePos.y), 10, 40, 10, DARKGRAY);
            //----------------------------------------------------------------------------------

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
