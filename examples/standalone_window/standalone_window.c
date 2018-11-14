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
    Vector2 position = { 500, 200 };
    Vector2 prevPosition = position;
    Vector2 panOffset = mousePos;
    bool dragWindow = false;

    SetTargetFPS(120);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!exitWindow && !WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        mousePos = GetMousePosition();
        
        if ((CheckCollisionPointRec(mousePos, (Rectangle){ 0, 0, screenWidth, 20 })) && 
             IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
            dragWindow = true;
            panOffset = mousePos;
        }
            
        if (dragWindow)
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                position.x = prevPosition.x + (mousePos.x - panOffset.x),
                position.y = prevPosition.y + (mousePos.y - panOffset.y);
            }
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) 
            {
                prevPosition = position;
                dragWindow = false;
            }
        }
        
        // NOTE: Input reading and window positioning should be executed in a second thread,
        // actually, full drawing should probably be in the second thread...
        SetWindowPosition(position.x, position.y);
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            exitWindow = GuiWindowBox((Rectangle){ 0, 0, screenWidth, screenHeight }, "STANDALONE WINDOW");
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
