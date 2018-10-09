/*******************************************************************************************
*
*   layout_file_name - tool description
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2018 raylib technologies
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../../src/raygui.h"

//----------------------------------------------------------------------------------
// Controls Functions Declaration
//----------------------------------------------------------------------------------
static void Button005();        // Button: Button005 logic

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 800;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "layout_file_name");

    // layout_file_name: controls initialization
    //----------------------------------------------------------------------------------

    int DropdownBox000Active = 0;
    const char *DropdownBox000TextList[3] = { "ONE", "TWO", "THREE" };
    int Spinner001Value = 0;
    int ValueBox002Value = 0;
    char TextBox003Text[64] = "SAMPLE TEXT";
    int ListView004Active = 0;
    const char *ListView004TextList[3] = { "ONE", "TWO", "THREE" };
    char TextBox006Text[64] = "SAMPLE TEXT";
    
    
    bool spinnerEditMode = false;
    bool valueBoxEditMode = false;
    bool textBoxEditMode = false;
    //----------------------------------------------------------------------------------

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

            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            DropdownBox000Active = GuiDropdownBox((Rectangle){ 25, 25, 125, 30 }, DropdownBox000TextList, 3, DropdownBox000Active);
            if (GuiSpinner((Rectangle){ 25, 75, 125, 30 }, &Spinner001Value, 0, 100, 25, spinnerEditMode)) spinnerEditMode = !spinnerEditMode;
            if (GuiValueBox((Rectangle){ 25, 125, 125, 30 }, &ValueBox002Value, 0, 100, valueBoxEditMode)) valueBoxEditMode = !valueBoxEditMode;
            if (GuiTextBox((Rectangle){ 25, 175, 125, 30 }, TextBox003Text, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;
            
            ListView004Active = GuiListView((Rectangle){ 175, 25, 125, 325 }, ListView004TextList, 3, ListView004Active);
            if (GuiButton((Rectangle){ 25, 225, 125, 30 }, "SAMPLE TEXT")) Button005(); 

            GuiTextBoxMulti((Rectangle){ 325, 25, 225, 175 }, TextBox006Text, 64, true);
            //GuiScrollPanel((Rectangle){ 325, 225, 225, 125 }, "SAMPLE TEXT");
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

//------------------------------------------------------------------------------------
// Controls Functions Definitions (local)
//------------------------------------------------------------------------------------
// Button: Button005 logic
static void Button005()
{
    // TODO: Implement control logic
}

