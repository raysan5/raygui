/*******************************************************************************************
*
*   raygui - controls test suite
*
*   TEST CONTROLS:
*       - GuiDropdownBox()
*       - GuiCheckBox()
*       - GuiSpinner()
*       - GuiValueBox()
*       - GuiTextBox()
*       - GuiButton()
*       - GuiComboBox()
*       - GuiListView()
*       - GuiToggleGroup()
*       - GuiTextBoxMulti()
*       - GuiColorPicker()
*       - GuiSlider()
*       - GuiSliderBar()
*       - GuiProgressBar()
*       - GuiColorBarAlpha()
*       - GuiScrollPanel()
*
*
*   DEPENDENCIES:
*       raylib 2.6-dev  - Windowing/input management and drawing.
*       raygui 2.6-dev  - Immediate-mode GUI controls.
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
#define RAYGUI_SUPPORT_RICONS
#include "../../src/raygui.h"

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again

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

    int listViewExScrollIndex = 0;
    int listViewExActive = 2;
    int listViewExFocus = -1;
    const char *listViewExList[8] = { "This", "is", "a", "list view", "with", "disable", "elements", "amazing!" };

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
    //GuiSetFont(font);
    
    bool exitWindow = false;
    bool showMessageBox = false;
    
    char textInput[256] = { 0 };
    bool showTextInputBox = false;
    
    char textInputFileName[256] = { 0 };

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!exitWindow)    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        exitWindow = WindowShouldClose();
        
        if (IsKeyPressed(KEY_ESCAPE)) showMessageBox = !showMessageBox;
        
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) showTextInputBox = true;
        
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
            //GuiSetStyle(CHECKBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
            forceSquaredChecked = GuiCheckBox((Rectangle){ 25, 108, 15, 15 }, "FORCE CHECK!", forceSquaredChecked);
            
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
            //GuiSetStyle(VALUEBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
            if (GuiSpinner((Rectangle){ 25, 135, 125, 30 }, NULL, &spinner001Value, 0, 100, spinnerEditMode)) spinnerEditMode = !spinnerEditMode;
            if (GuiValueBox((Rectangle){ 25, 175, 125, 30 }, NULL, &valueBox002Value, 0, 100, valueBoxEditMode)) valueBoxEditMode = !valueBoxEditMode;
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
            if (GuiTextBox((Rectangle){ 25, 215, 125, 30 }, textBoxText, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;
            
            GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
            
            if (GuiButton((Rectangle){ 25, 255, 125, 30 }, GuiIconText(RICON_FILE_SAVE, "Save File"))) showTextInputBox = true;
            
            GuiGroupBox((Rectangle){ 25, 310, 125, 150 }, "STATES");
            GuiLock();
            GuiSetState(GUI_STATE_NORMAL); if (GuiButton((Rectangle){ 30, 320, 115, 30 }, "NORMAL")) { }
            GuiSetState(GUI_STATE_FOCUSED); if (GuiButton((Rectangle){ 30, 355, 115, 30 }, "FOCUSED")) { }
            GuiSetState(GUI_STATE_PRESSED); if (GuiButton((Rectangle){ 30, 390, 115, 30 }, "#15#PRESSED")) { }
            GuiSetState(GUI_STATE_DISABLED); if (GuiButton((Rectangle){ 30, 425, 115, 30 }, "DISABLED")) { }
            GuiSetState(GUI_STATE_NORMAL);
            GuiUnlock();
            
            comboBoxActive = GuiComboBox((Rectangle){ 25, 470, 125, 30 }, "ONE;TWO;THREE;FOUR", comboBoxActive);
            
            // NOTE: GuiDropdownBox must draw after any other control that can be covered on unfolding
            GuiSetStyle(DROPDOWNBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
            if (GuiDropdownBox((Rectangle){ 25, 65, 125, 30 }, "#01#ONE;#02#TWO;#03#THREE;#04#FOUR", &dropdownBox001Active, dropDown001EditMode)) dropDown001EditMode = !dropDown001EditMode;

            GuiSetStyle(DROPDOWNBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
            if (GuiDropdownBox((Rectangle){ 25, 25, 125, 30 }, "ONE;TWO;THREE", &dropdownBox000Active, dropDown000EditMode)) dropDown000EditMode = !dropDown000EditMode;

            // Second GUI column
            listViewActive = GuiListView((Rectangle){ 165, 25, 140, 140 }, "Charmander;Bulbasaur;#18#Squirtel;Pikachu;Eevee;Pidgey", &listViewScrollIndex, listViewActive);
            listViewExActive = GuiListViewEx((Rectangle){ 165, 180, 140, 200 }, listViewExList, 8, &listViewExFocus, &listViewExScrollIndex, listViewExActive);

            toggleGroupActive = GuiToggleGroup((Rectangle){ 165, 400, 140, 25 }, "#1#ONE\n#3#TWO\n#8#THREE\n#23#", toggleGroupActive);
            
            // Third GUI column
            if (GuiTextBoxMulti((Rectangle){ 320, 25, 225, 140 }, multiTextBoxText, 141, multiTextBoxEditMode)) multiTextBoxEditMode = !multiTextBoxEditMode;
            colorPickerValue = GuiColorPicker((Rectangle){ 320, 185, 196, 192 }, colorPickerValue);
            
            sliderValue = GuiSlider((Rectangle){ 355, 400, 165, 20 }, "TEST", TextFormat("%2.2f", (float)sliderValue), sliderValue, -50, 100);
            sliderBarValue = GuiSliderBar((Rectangle){ 320, 430, 200, 20 }, NULL, TextFormat("%i", (int)sliderBarValue), sliderBarValue, 0, 100);
            progressValue = GuiProgressBar((Rectangle){ 320, 460, 200, 20 }, NULL, NULL, progressValue, 0, 1);

            // NOTE: View rectangle could be used to perform some scissor test
            Rectangle view = GuiScrollPanel((Rectangle){ 560, 25, 100, 160 }, (Rectangle){ 560, 25, 200, 400 }, &viewScroll);
            
            GuiStatusBar((Rectangle){ 0, GetScreenHeight() - 20, GetScreenWidth(), 20 }, "This is a status bar");
            
            alphaValue = GuiColorBarAlpha((Rectangle){ 320, 490, 200, 30 }, alphaValue);

            if (showMessageBox)
            {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RAYWHITE, 0.8f));
                int result = GuiMessageBox((Rectangle){ GetScreenWidth()/2 - 125, GetScreenHeight()/2 - 50, 250, 100 }, GuiIconText(RICON_EXIT, "Close Window"), "Do you really want to exit?", "Yes;No"); 
            
                if ((result == 0) || (result == 2)) showMessageBox = false;
                else if (result == 1) exitWindow = true;
            }
            
            if (showTextInputBox)
            {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RAYWHITE, 0.8f));
                int result = GuiTextInputBox((Rectangle){ GetScreenWidth()/2 - 120, GetScreenHeight()/2 - 60, 240, 140 }, GuiIconText(RICON_FILE_SAVE, "Save file as..."), "Introduce a save file name", "Ok;Cancel", textInput);
                
                if (result == 1)
                {
                    // TODO: Validate textInput value and save
                    
                    strcpy(textInputFileName, textInput);
                }
                
                if ((result == 0) || (result == 1) || (result == 2)) 
                {
                    showTextInputBox = false;
                    strcpy(textInput, "\0");
                }
            }
            
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
