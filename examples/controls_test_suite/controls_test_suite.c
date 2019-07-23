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
*   Copyright (c) 2019 raylib technologies (@raylibtech)
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
    int screenWidth = 690;
    int screenHeight = 560;

    InitWindow(screenWidth, screenHeight, "raygui - controls test suite");
    SetExitKey(0);

    // GUI controls initialization
    //----------------------------------------------------------------------------------
    int dropdownBox000Active = 0;
    bool dropDown000EditMode = false;
    
    int dropdownBox001Active = 0;
    bool dropDown001EditMode = false;    
    
    int spinner001Value = 0;
    bool spinnerEditMode = false;
    
    int valueBox002Value = 0;
    bool valueBoxEditMode = false;
    
    char textBoxText[64] = "Text box";
    bool textBoxEditMode = false;
    
    int listViewScrollIndex = 0;
    int listViewActive = -1;
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
    
    float alphaValue = 0.5f;
    
    int comboBoxActive = 1;
    
    int toggleGroupActive = 0;
    
    Vector2 viewScroll = { 0, 0 };
    //----------------------------------------------------------------------------------
    
    // Custom GUI font loading
    //Font font = LoadFontEx("fonts/rainyhearts16.ttf", 12, 0, 0);
    //GuiFont(font);
    
    bool exitWindow = false;
    bool showMessageBox = false;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!exitWindow)    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        exitWindow = WindowShouldClose();
        
        if (IsKeyPressed(KEY_ESCAPE)) showMessageBox = !showMessageBox;
        
        if (IsFileDropped())
        {
            int dropsCount = 0;
            char **droppedFiles = GetDroppedFiles(&dropsCount);
            
            if ((dropsCount > 0) && IsFileExtension(droppedFiles[0], ".rgs")) GuiLoadStyle(droppedFiles[0]);
            
            ClearDroppedFiles();    // Clear internal buffers
        }
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
            forceSquaredChecked = GuiCheckBox((Rectangle){ 25, 108, 15, 15 }, "Force Square", forceSquaredChecked);
            
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
            if (GuiSpinner((Rectangle){ 25, 135, 125, 30 }, &spinner001Value, 0, 100, spinnerEditMode)) spinnerEditMode = !spinnerEditMode;
            if (GuiValueBox((Rectangle){ 25, 175, 125, 30 }, &valueBox002Value, 0, 100, valueBoxEditMode)) valueBoxEditMode = !valueBoxEditMode;
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
            if (GuiTextBox((Rectangle){ 25, 215, 125, 30 }, textBoxText, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;
            
            GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
            
            //if (GuiButton((Rectangle){ 25, 255, 125, 30 }, "#05#Open File")) { };

            if (GuiButton((Rectangle){ 25, 255, 125, 30 }, GuiIconText(RICON_FILE_OPEN, "Open File"))) { };
            
            GuiGroupBox((Rectangle){ 25, 310, 125, 150 }, "STATES");
            GuiLock();
            GuiState(GUI_STATE_NORMAL); if (GuiButton((Rectangle){ 30, 320, 115, 30 }, "NORMAL")) { }
            GuiState(GUI_STATE_FOCUSED); if (GuiButton((Rectangle){ 30, 355, 115, 30 }, "FOCUSED")) { }
            GuiState(GUI_STATE_PRESSED); if (GuiButton((Rectangle){ 30, 390, 115, 30 }, "#15#PRESSED")) { }
            GuiState(GUI_STATE_DISABLED); if (GuiButton((Rectangle){ 30, 425, 115, 30 }, "DISABLED")) { }
            GuiState(GUI_STATE_NORMAL);
            GuiUnlock();
            
            comboBoxActive = GuiComboBox((Rectangle){ 25, 470, 125, 30 }, "ONE;TWO;THREE;FOUR", comboBoxActive);
            
            // NOTE: GuiDropdownBox must draw after any other control that can be covered on unfolding
            GuiSetStyle(DROPDOWNBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
            if (GuiDropdownBox((Rectangle){ 25, 65, 125, 30 }, "#01#ONE;#02#TWO;#03#THREE;#04#FOUR", &dropdownBox001Active, dropDown001EditMode)) dropDown001EditMode = !dropDown001EditMode;
            GuiSetStyle(DROPDOWNBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
            if (GuiDropdownBox((Rectangle){ 25, 25, 125, 30 }, "ONE;TWO;THREE", &dropdownBox000Active, dropDown000EditMode)) dropDown000EditMode = !dropDown000EditMode;
            
            // Second GUI column      
            if (GuiListView((Rectangle){ 165, 25, 140, 140 }, "Charmander;Bulbasaur;#18#Squirtel;Pikachu;Eevee;Pidgey", &listViewActive, &listViewScrollIndex, listViewEditMode)) listViewEditMode = !listViewEditMode;
            if (GuiListViewEx((Rectangle){ 165, 180, 140, 200 }, listViewExList, 8, listViewExElementsEnable, &listViewExActive, &listViewExFocus, &listViewExScrollIndex, listViewExEditMode)) listViewExEditMode = !listViewExEditMode;
            //if ((listViewExFocus >= 0) && (listViewExFocus < 8)) DrawText(FormatText("FOCUS: %s", listViewExList[listViewExFocus]), 165, 385, 10, (listViewExElementsEnable[listViewExFocus] > 0)? LIME : MAROON);
            
            toggleGroupActive = GuiToggleGroup((Rectangle){ 165, 400, 140, 25 }, "#1#ONE\n#3#TWO\n#8#THREE\n#23#", toggleGroupActive);
            
            // Third GUI column
            if (GuiTextBoxMulti((Rectangle){ 320, 25, 225, 140 }, multiTextBoxText, 141, multiTextBoxEditMode)) multiTextBoxEditMode = !multiTextBoxEditMode;
            colorPickerValue = GuiColorPicker((Rectangle){ 320, 185, 196, 192 }, colorPickerValue);
            
            sliderValue = GuiSlider((Rectangle){ 370, 400, 200, 20 }, "#49#TEST", sliderValue, -50, 100, true);
            sliderBarValue = GuiSliderBar((Rectangle){ 320, 430, 200, 20 }, NULL, sliderBarValue, 0, 100, true);
            progressValue = GuiProgressBar((Rectangle){ 320, 460, 200, 20 }, NULL, progressValue, 0, 1, true);

            Rectangle view = GuiScrollPanel((Rectangle){ 560, 25, 100, 160 }, (Rectangle){ 560, 25, 200, 400 }, &viewScroll);
            
            GuiSetStyle(DEFAULT, INNER_PADDING, 10);
            GuiStatusBar((Rectangle){ 0, GetScreenHeight() - 20, GetScreenWidth(), 20 }, "This is a status bar");
            GuiSetStyle(DEFAULT, INNER_PADDING, 2);
            
            alphaValue = GuiColorBarAlpha((Rectangle){ 320, 490, 200, 30 }, alphaValue);
            
            if (showMessageBox)
            {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RAYWHITE, 0.8f));
                int message = GuiMessageBox((Rectangle){ GetScreenWidth()/2 - 125, GetScreenHeight()/2 - 50, 250, 100 }, GuiIconText(RICON_EXIT, "Closing rTexPacker"), "Do you really want to exit?", "Yes;No"); 
            
                if ((message == 0) || (message == 2)) showMessageBox = false;
                else if (message == 1) exitWindow = true;
            }

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
