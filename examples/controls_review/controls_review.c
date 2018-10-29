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
    int DropdownBox001Active = 0;
    const char *DropdownBox000TextList[3] = { "ONE", "TWO", "THREE" };
    int Spinner001Value = 0;
    int ValueBox002Value = 0;
    char TextBox003Text[64] = "SAMPLE TEXT";
    int ListView004Active = -1;
    const char *ListView004TextList[6] = { "Charmander", "Bulbasaur", "Squirtel", "Pikachu", "Eevee", "Pidgey" };
    char TextBox006Text[141] = "SAMPLE TEXT";
    
    bool spinnerEditMode = false;
    bool valueBoxEditMode = false;
    bool textBoxEditMode = false;
    bool multiTextBoxEditMode = false;

    bool listViewEditMode = false;

    bool dropDown000EditMode = false;
    bool dropDown001EditMode = false;
    
     bool forceSquaredChecked = false;
     
    //int *enableElements = (int *)malloc(6*sizeof(int));
    int enableElements[6] = {0, 0, 0, 0, 0, 0};
    
    for (int i = 0; i < 6; i++)
    {
        enableElements[i] = 1;
    }
     
     Color colorPickerValue = RED;
    //----------------------------------------------------------------------------------
    
    Font font = LoadFontEx("fonts/NorthernLights.ttf", 30, 0, 0);
    GuiFont(font);

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
            if (dropDown000EditMode || dropDown001EditMode) GuiLock();
            GuiDisable();
            //if (GuiSpinner((Rectangle){ 25, 75, 125, 30 }, &Spinner001Value, 0, 100, 25, spinnerEditMode)) spinnerEditMode = !spinnerEditMode;
            if (GuiValueBox((Rectangle){ 25, 125, 125, 30 }, &ValueBox002Value, 0, 100, valueBoxEditMode)) valueBoxEditMode = !valueBoxEditMode;
            if (GuiTextBox((Rectangle){ 25, 175, 125, 30 }, TextBox003Text, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;

            //if (GuiListView((Rectangle){ 175, 25, 120, 100 }, ListView004TextList, 6, &ListView004Active, listViewEditMode)) listViewEditMode = !listViewEditMode;
            if (GuiListViewEx((Rectangle){ 175, 25, 120, 100 }, ListView004TextList, enableElements, 6, &ListView004Active, listViewEditMode)) listViewEditMode = !listViewEditMode;
            if (GuiButton((Rectangle){ 25, 225, 125, 30 }, "SAMPLE TEXT")) Button005(); 

            if (GuiTextBoxMulti((Rectangle){ 325, 25, 225, 175 }, TextBox006Text, 141, multiTextBoxEditMode)) multiTextBoxEditMode = !multiTextBoxEditMode;

            //GuiScrollPanel((Rectangle){ 325, 225, 225, 125 }, "SAMPLE TEXT");
            if (GuiDropdownBox((Rectangle){ 25, 75, 125, 30 }, DropdownBox000TextList, 3, &DropdownBox001Active, dropDown001EditMode)) dropDown001EditMode = !dropDown001EditMode;
            forceSquaredChecked = GuiCheckBoxEx((Rectangle){ 25, 65, 15, 15 }, forceSquaredChecked, "Force Square");
            //GuiDisable();
            if (GuiDropdownBox((Rectangle){ 25, 25, 125, 30 }, DropdownBox000TextList, 3, &DropdownBox000Active, dropDown000EditMode)) dropDown000EditMode = !dropDown000EditMode;
            
            colorPickerValue = GuiColorPicker((Rectangle){ 325, 220, 240, 240 }, colorPickerValue);
            GuiEnable();
            GuiUnlock();
            
            GuiLock();
            GuiState(0); if (GuiButton((Rectangle){ 25, 400, 125, 30 }, "SAMPLE TEXT")) Button005();
            GuiState(1); if (GuiButton((Rectangle){ 25, 440, 125, 30 }, "SAMPLE TEXT")) Button005();
            GuiState(2); if (GuiButton((Rectangle){ 25, 480, 125, 30 }, "SAMPLE TEXT")) Button005();
            GuiState(3); if (GuiButton((Rectangle){ 25, 520, 125, 30 }, "SAMPLE TEXT")) Button005();
            GuiState(1);            
            GuiUnlock();
            //----------------------------------------------------------------------------------

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    //free(enableElements);
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

