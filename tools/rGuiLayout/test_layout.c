/*******************************************************************************************
*
*   $(tool_name) - $(tool_description)
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) $(year) $(author)
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------
static void Button000();
static void Button001();

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "rFXGen");

    Rectangle layoutRecs[12] = {
        (Rectangle){ 100, 50, 100, 30 },  // BUTTON 000
        (Rectangle){ 100, 100, 150, 30 },  // BUTTON 001
        (Rectangle){ 100, 150, 100, 30 },  // LABEL 002
        (Rectangle){ 100, 190, 100, 30 },  // TOGGLE 003
        (Rectangle){ 100, 240, 90, 30 },  // TOGGLEGROUP 004
        (Rectangle){ 100, 290, 300, 20 },  // SLIDER 005
        (Rectangle){ 100, 330, 250, 20 },  // PROGRESSBAR 006
        (Rectangle){ 350, 50, 150, 30 },  // SPINNER 007
        (Rectangle){ 350, 100, 120, 30 },  // COMBOBOX 008
        (Rectangle){ 420, 160, 20, 20 },  // CHECKBOX 009
        (Rectangle){ 420, 210, 20, 20 },  // CHECKBOX 010
        (Rectangle){ 450, 250, 150, 30 }  // TEXTBOX 011
    };

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

            ClearBackground(GuiBackground());

            if (GuiButton(layoutRecs[0], "BUTTON")) Button000(); 

            if (GuiButton(layoutRecs[1], "BUTTON")) Button001(); 

            GuiLabel(layoutRecs[2], "TEXT SAMPLE");

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------
static void Button000()
{
    // TODO: Implement control logic
}

static void Button001()
{
    // TODO: Implement control logic
}

