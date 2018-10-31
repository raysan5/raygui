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
    int dropdownBox000Active = 0;
    const char *dropdownBox000TextList[3] = { "ONE", "TWO", "THREE" };
    bool dropDown000EditMode = false;
    
    int dropdownBox001Active = 0;
    const char *dropdownBox001TextList[5] = { "ONE", "TWO", "THREE", "FOUR", "FIVE" };
    bool dropDown001EditMode = false;    
    
    int spinner001Value = 0;
    bool spinnerEditMode = false;
    
    int valueBox002Value = 0;
    bool valueBoxEditMode = false;
    
    char textBoxText[64] = "Text box";
    bool textBoxEditMode = false;
    
    int listViewScrollIndex = 1;
    int listViewActive = -1;
    const char *listViewList[6] = { "Charmander", "Bulbasaur", "Squirtel", "Pikachu", "Eevee", "Pidgey" };
    bool listViewEditMode = false;
    
    int listViewExScrollIndex = 0;
    int listViewExActive = -1;
    int listViewExFocus = -1;
    const char *listViewExList[8] = { "This", "is", "a", "list view", "with", "disable", "elements", "amazing!" };
    int listViewExElementsEnable[8] = {1, 0, 1, 1, 0, 0, 1};
    bool listViewExEditMode = false;
    
    char multiTextBoxText[141] = "Multi text box";    
    bool multiTextBoxEditMode = false;    
    
    bool forceSquaredChecked = false;
    //----------------------------------------------------------------------------------
    
    //Font font = LoadFontEx("fonts/rainyhearts16.ttf", 12, 0, 0);
    //GuiFont(font);

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
            //GuiDisable();
            
            // First GUI column
            forceSquaredChecked = GuiCheckBoxEx((Rectangle){ 25, 108, 15, 15 }, forceSquaredChecked, "Force Square");
            if (GuiSpinner((Rectangle){ 25, 135, 125, 30 }, &spinner001Value, 0, 100, 25, spinnerEditMode)) spinnerEditMode = !spinnerEditMode;
            if (GuiValueBox((Rectangle){ 25, 175, 125, 30 }, &valueBox002Value, 0, 100, valueBoxEditMode)) valueBoxEditMode = !valueBoxEditMode;
            if (GuiTextBox((Rectangle){ 25, 215, 125, 30 }, textBoxText, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;
            if (GuiButton((Rectangle){ 25, 255, 125, 30 }, "SAMPLE TEXT")) Button005(); 
                // NOTE: GuiDropdownBox must draw at the end of the column
            if (GuiDropdownBox((Rectangle){ 25, 65, 125, 30 }, dropdownBox001TextList, 5, &dropdownBox001Active, dropDown001EditMode)) dropDown001EditMode = !dropDown001EditMode;            
            if (GuiDropdownBox((Rectangle){ 25, 25, 125, 30 }, dropdownBox000TextList, 3, &dropdownBox000Active, dropDown000EditMode)) dropDown000EditMode = !dropDown000EditMode;           
            
            // Second GUI column      
            if (GuiListView((Rectangle){ 185, 25, 120, 100 }, listViewList, 6, &listViewScrollIndex, &listViewActive, listViewEditMode)) listViewEditMode = !listViewEditMode;
            if (GuiListViewEx((Rectangle){ 185, 155, 120, 200 }, listViewExList, listViewExElementsEnable, 8, &listViewExScrollIndex, &listViewExActive, &listViewExFocus, listViewExEditMode)) listViewExEditMode = !listViewExEditMode;
            if (listViewExFocus >= 0 && listViewExFocus < 8) DrawText(listViewExList[listViewExFocus], 195, 370, 20, RED);
            
            // Third GUI column
            if (GuiTextBoxMulti((Rectangle){ 325, 25, 225, 175 }, multiTextBoxText, 141, multiTextBoxEditMode)) multiTextBoxEditMode = !multiTextBoxEditMode;
            
            //GuiEnable();
            GuiUnlock();
            
            // Fourth GUI column
            GuiLock();
            GuiState(0); if (GuiButton((Rectangle){ 600, 25, 125, 30 }, "DISABLE")) Button005();
            GuiState(1); if (GuiButton((Rectangle){ 600, 65, 125, 30 }, "NORMAL")) Button005();
            GuiState(2); if (GuiButton((Rectangle){ 600, 105, 125, 30 }, "FOCUSED")) Button005();
            GuiState(3); if (GuiButton((Rectangle){ 600, 145, 125, 30 }, "PRESSED")) Button005();
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
