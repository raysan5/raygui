/*******************************************************************************************
*
*   Some new GUI controls testing program
*
*   Controls to test:
*       - GuiScrollPanel()
*       - GuiTextBoxMulti()
*
*   Compile this program using:
*       gcc -o $(NAME_PART).exe $(NAME_PART).c -I..\..\src -lraylib -lopengl32 -lgdi32 -std=c99 -Wall
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2018 raylib technologies
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "Controls testing suite");

    // GuiTextBoxMulti() variables
    int textMultiSize = 128;
    char textMulti[128] = "";
    
    // GuiScrollPanel() variables
    Rectangle scrollBox = { 100, 100, 200, 300 };
    Rectangle scrollContent = { 0, 0, 200, 600 };
    Vector2 scrollPosition = { 0, 0 };

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Implement required update logic
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(style[DEFAULT_BACKGROUND_COLOR]));

			GuiTextBoxMulti((Rectangle){ 500, 100, 100, 200 }, textMulti, textMultiSize, true);
            
            scrollPosition = GuiScrollPanel(scrollBox, scrollContent, scrollPosition);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}