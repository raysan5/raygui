/*******************************************************************************************
*
*   raygui - controls test suite
*
*   TEST CONTROLS:
*       - GuiDropdownBox()
*       - GuiValueBox()
*       - GuiSpinner()
*       - GuiTextBox()
*       - GuiTextBoxMulti()
*       - GuiListView()
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
#include "../../src/raygui.h"


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    int screenWidth = 600;
    int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "raygui - controls test suite");

    // GUI controls initialization
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
    
    int listViewScrollIndex = 0;
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
    Color colorPickerValue = RED;
    
    int sliderValue = 50;
    int sliderBarValue = 60;
    float progressValue = 0.4f;
    
    bool forceSquaredChecked = false;
    
    int comboBoxActive = 1;
    //----------------------------------------------------------------------------------
    
    // Custom GUI font loading
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

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
 
            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            if (dropDown000EditMode || dropDown001EditMode) GuiLock();
            //GuiDisable();
            
            // First GUI column
            forceSquaredChecked = GuiCheckBoxEx((Rectangle){ 25, 108, 15, 15 }, forceSquaredChecked, "Force Square");
            if (GuiSpinner((Rectangle){ 25, 135, 125, 30 }, &spinner001Value, 0, 100, 25, spinnerEditMode)) spinnerEditMode = !spinnerEditMode;
            if (GuiValueBox((Rectangle){ 25, 175, 125, 30 }, &valueBox002Value, 0, 100, valueBoxEditMode)) valueBoxEditMode = !valueBoxEditMode;
            if (GuiTextBox((Rectangle){ 25, 215, 125, 30 }, textBoxText, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;
            if (GuiButton((Rectangle){ 25, 255, 125, 30 }, "SAMPLE TEXT")) { }
            
            GuiGroupBox((Rectangle){ 25, 310, 125, 150 }, "CONTROL STATES");
            GuiLock();
            GuiState(GUI_STATE_NORMAL); if (GuiButton((Rectangle){ 30, 320, 115, 30 }, "NORMAL")) { }
            GuiState(GUI_STATE_FOCUSED); if (GuiButton((Rectangle){ 30, 355, 115, 30 }, "FOCUSED")) { }
            GuiState(GUI_STATE_PRESSED); if (GuiButton((Rectangle){ 30, 390, 115, 30 }, "PRESSED")) { }
            GuiState(GUI_STATE_DISABLED); if (GuiButton((Rectangle){ 30, 425, 115, 30 }, "DISABLED")) { }
            GuiState(GUI_STATE_NORMAL);            
            GuiUnlock();
            
            comboBoxActive = GuiComboBox((Rectangle){25, 470, 125, 30}, dropdownBox001TextList, 5, comboBoxActive);
            
            // NOTE: GuiDropdownBox must draw after any other control that can be covered on unfolding
            if (GuiDropdownBox((Rectangle){ 25, 65, 125, 30 }, dropdownBox001TextList, 5, &dropdownBox001Active, dropDown001EditMode)) dropDown001EditMode = !dropDown001EditMode;
            if (GuiDropdownBox((Rectangle){ 25, 25, 125, 30 }, dropdownBox000TextList, 3, &dropdownBox000Active, dropDown000EditMode)) dropDown000EditMode = !dropDown000EditMode;
            
            // Second GUI column      
            if (GuiListView((Rectangle){ 165, 25, 140, 140 }, listViewList, 6, &listViewScrollIndex, &listViewActive, listViewEditMode)) listViewEditMode = !listViewEditMode;
            if (GuiListViewEx((Rectangle){ 165, 180, 140, 200 }, listViewExList, listViewExElementsEnable, 8, &listViewExScrollIndex, &listViewExActive, &listViewExFocus, listViewExEditMode)) listViewExEditMode = !listViewExEditMode;
            if (listViewExFocus >= 0 && listViewExFocus < 8) DrawText(FormatText("FOCUS: %s", listViewExList[listViewExFocus]), 165, 390, 10, listViewExElementsEnable[listViewExFocus] ? LIME : MAROON);
            
            // Third GUI column
            if (GuiTextBoxMulti((Rectangle){ 320, 25, 225, 140 }, multiTextBoxText, 141, multiTextBoxEditMode)) multiTextBoxEditMode = !multiTextBoxEditMode;
            colorPickerValue = GuiColorPicker((Rectangle){ 320, 185, 196, 192 }, colorPickerValue);
            
            sliderValue = GuiSliderEx((Rectangle){ 320, 400, 200, 20 }, sliderValue, -50, 100, "SLIDER", true);
            sliderBarValue = GuiSliderBarEx((Rectangle){ 320, 430, 200, 20 }, sliderBarValue, 0, 100, "SLIDERBAR", true);
            progressValue = GuiProgressBarEx((Rectangle){ 320, 460, 200, 20 }, progressValue, 0, 1, true);
            
            //GuiEnable();
            GuiUnlock();
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
