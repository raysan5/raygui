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
*   Copyright (c) 2020 Ramon Santamaria (@raysan5)
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
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

    char multiTextBoxText[256] = "Multi text box";
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
    
    // Setup skin
    GuiSkin skin = {0};
    skin.useColor = true;
    skin.tex = LoadTexture("skin.png");
    //SetTextureFilter(skin.tex, FILTER_TRILINEAR);
    
    // BUTTONS
    skin.button = (GuiSkinStyle){.width=20, .height=20, .nx=4, .ny=4, .px=25, .py=4, .fx=46, .fy=4, .dx=67, .dy=4, .margins=2, .top=6, .left=6, .right=6, .bottom=6, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.imagebutton = skin.button;
    
    // DROPDOWNBOX
    skin.dropbox.body = (GuiSkinStyle){.width=24, .height=24, .nx=4, .ny=26, .px=28, .py=26, .fx=52, .fy=26, .dx=76, .dy=26, .margins=3, .top=7, .left=7, .right=7, .bottom=7, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.dropbox.listpanel = (GuiSkinStyle){.width=20, .height=20, .nx=4, .ny=5, .margins=6, .top=6, .left=6, .right=6, .bottom=6, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.dropbox.item = (GuiSkinStyle){.width=20, .height=20, .nx=5, .ny=52, .px=27, .py=52, .fx=5, .fy=52, .margins=1, .top=5, .left=5, .right=5, .bottom=5, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    
    // CHECKBOX
    skin.checkbox.body = (GuiSkinStyle){.width=24, .height=24, .nx=105, .ny=3, .px=105, .py=29, .fx=130, .fy=29, .dx=130, .dy=3, .margins=2, .top=11, .left=11, .right=11, .bottom=11, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.checkbox.checked = (GuiSkinStyle){.width=24, .height=24, .nx=157, .ny=3, .px=157, .py=29, .fx=183, .fy=29, .dx=183, .dy=3, .margins=2, .top=11, .left=11, .right=11, .bottom=11, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    
    // SPINBOX
    skin.spinner.valuebox = (GuiSkinStyle){.width=12, .height=32, .nx=252, .ny=5, .px=252, .py=74, .fx=252, .fy=39, .dx=252, .dy=108, .margins=0, .top=2, .left=2, .right=2, .bottom=2, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.spinner.left = (GuiSkinStyle){.width=22, .height=32, .nx=217, .ny=5, .px=217, .py=74, .fx=217, .fy=39, .dx=217, .dy=108, .margins=0, .top=6, .left=6, .right=6, .bottom=6, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.spinner.right = (GuiSkinStyle){.width=22, .height=32, .nx=277, .ny=5, .px=277, .py=74, .fx=277, .fy=39, .dx=277, .dy=108, .margins=0, .top=6, .left=6, .right=6, .bottom=6, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    
    // VALUEBOX
    skin.valuebox = (GuiSkinStyle){.width=24, .height=32, .nx=305, .ny=5, .px=305, .py=74, .fx=305, .fy=39, .dx=305, .dy=108, .margins=0, .top=7, .left=7, .right=7, .bottom=7, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    
    // TEXTBOX
    skin.textbox = (GuiSkinStyle){.width=47, .height=25, .nx=339, .ny=5, .px=339, .py=65, .fx=339, .fy=35, .dx=305, .dy=108, .margins=1, .top=6, .left=6, .right=6, .bottom=6, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    
    // COMBOBOX
    skin.combobox.body = (GuiSkinStyle){.width=36, .height=16, .nx=5, .ny=77, .px=5, .py=77, .fx=5, .fy=96, .dx=0, .dy=0, .margins=0, .top=5, .left=5, .right=5, .bottom=5, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.combobox.button = (GuiSkinStyle){.width=21, .height=10, .nx=44, .ny=77, .px=44, .py=77, .fx=44, .fy=96, .dx=0, .dy=0, .margins=0, .top=3, .left=3, .right=3, .bottom=3, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_TILED };
    
    // LISTVIEW
    skin.list.body = (GuiSkinStyle){.width=52, .height=46, .nx=76, .ny=63, .px=76, .py=63, .fx=133, .fy=63, .dx=0, .dy=0, .margins=14, .top=16, .left=16, .right=16, .bottom=16, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_TILED };
    skin.list.first = skin.list.last = skin.list.item = (GuiSkinStyle){.width=24, .height=32, .nx=450, .ny=450, .px=305, .py=5, .fx=305, .fy=39, .dx=305, .dy=108, .margins=0, .top=7, .left=7, .right=7, .bottom=7, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.list.scrollbar.body =(GuiSkinStyle){.width=18, .height=64, .nx=408, .ny=16, .px=408, .py=16, .fx=408, .fy=16, .dx=408, .dy=16, .margins=0, .top=8, .left=8, .right=8, .bottom=8, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.list.scrollbar.rail = (GuiSkinStyle){.width=1, .height=1, .nx=0, .ny=0, .px=0, .py=0, .fx=0, .fy=0, .dx=0, .dy=0, .margins=0, .top=0, .left=0, .right=0, .bottom=0, .type=GUI_SKIN_TYPE_SLICE1, .mode=GUI_SKIN_DRAW_CENTERED };
    skin.list.scrollbar.slider = (GuiSkinStyle){.width=12, .height=58, .nx=439, .ny=19, .px=439, .py=19, .fx=458, .fy=19, .dx=436, .dy=19, .margins=0, .top=5, .left=5, .right=5, .bottom=5, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    
    // TOGGLE GROUP
    skin.togglegroup.item.off = (GuiSkinStyle){.width=50, .height=42, .fx=8, .fy=123, .px=114, .py=123, .nx=60, .ny=123, .dx=0, .dy=0, .margins=12, .top=16, .left=20, .right=16, .bottom=20, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.togglegroup.item.on = (GuiSkinStyle){.width=50, .height=42, .fx=114, .fy=123, .nx=114, .ny=123, .px=60, .py=123, .dx=0, .dy=0, .margins=12, .top=16, .left=20, .right=16, .bottom=20, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.togglegroup.first = skin.togglegroup.item;
    skin.togglegroup.last = skin.togglegroup.item;
    
    // TEXTBOXMULTI
    skin.textboxmulti = (GuiSkinStyle){.width=76, .height=37, .nx=340, .ny=105, .px=340, .py=105, .fx=340, .fy=105, .dx=340, .dy=105, .margins=7, .top=7, .left=10, .right=10, .bottom=7, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_TILED };
    
    // COLORBARALPHA
    skin.colorbaralpha.body = (GuiSkinStyle){.width=96, .height=64, .nx=8, .ny=175, .px=8, .py=175, .fx=109, .fy=175, .dx=8, .dy=175, .margins=5, .top=12, .left=12, .right=12, .bottom=12, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.colorbaralpha.selector = (GuiSkinStyle){.width=8, .height=35, .nx=221, .ny=174, .px=221, .py=174, .fx=221, .fy=174, .dx=221, .dy=174, .margins=6, .top=4, .left=4, .right=4, .bottom=4, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    
    // COLORBARHUE
    skin.colorbarhue.body = skin.colorbaralpha.body;
    skin.colorbarhue.selector = (GuiSkinStyle){.width=35, .height=8, .nx=233, .ny=201, .px=233, .py=201, .fx=233, .fy=201, .dx=233, .dy=201, .margins=4, .top=4, .left=4, .right=4, .bottom=4, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    
    // COLORPANEL
    skin.colorpanel.body = skin.colorbaralpha.body;
    skin.colorpanel.selector = (GuiSkinStyle){.width=18, .height=18, .nx=232, .ny=173, .px=232, .py=173, .fx=232, .fy=173, .dx=232, .dy=173, .margins=6, .top=0, .left=0, .right=0, .bottom=0, .type=GUI_SKIN_TYPE_SLICE1, .mode=GUI_SKIN_DRAW_CENTERED };
    
    
    // SLIDER
    skin.slider.body = (GuiSkinStyle){.width=82, .height=40, .nx=285, .ny=153, .px=285, .py=153, .fx=285, .fy=153, .dx=285, .dy=153, .margins=10, .top=20, .left=15, .right=15, .bottom=20, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.slider.slider = (GuiSkinStyle){.width=20, .height=14, .nx=298, .ny=202, .px=298, .py=202, .fx=298, .fy=218, .dx=298, .dy=202, .margins=0, .top=6, .left=6, .right=6, .bottom=6, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.slider.bar = (GuiSkinStyle){.width=20, .height=14, .nx=298, .ny=234, .px=298, .py=234, .fx=298, .fy=218, .dx=298, .dy=234, .margins=0, .top=6, .left=6, .right=6, .bottom=6, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    
    // PROGRESS
    skin.progressbar.body = (GuiSkinStyle){.width=26, .height=12, .nx=431, .ny=91, .margins=0, .top=6, .left=7, .right=7, .bottom=6, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.progressbar.progress = (GuiSkinStyle){.width=21, .height=12, .nx=434, .ny=104, .margins=1, .top=6, .left=7, .right=7, .bottom=6, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    
    // WINDOWBOX
    skin.window.panel = (GuiSkinStyle){.width=26, .height=16, .nx=425, .ny=125, .px=425, .py=125, .fx=425, .fy=125, .dx=425, .dy=125, .margins=0, .top=4, .left=4, .right=4, .bottom=4, .type=GUI_SKIN_TYPE_SLICE9, .mode=GUI_SKIN_DRAW_STRECHED };
    skin.window.titlebar = skin.window.panel;
    skin.window.titlebar.margins = 1;
    
    // SCROLLPANEL
    skin.scrollpanel.body = skin.textboxmulti;
    skin.scrollpanel.hscroll = skin.scrollpanel.vscroll = skin.list.scrollbar;
    skin.scrollpanel.corner = (GuiSkinStyle){.width=6, .height=6, .nx=455, .ny=129, .px=455, .py=129, .fx=455, .fy=137, .margins=6, .type=GUI_SKIN_TYPE_SLICE1, .mode=GUI_SKIN_DRAW_CENTERED };
    
    GuiSetSkin(&skin);
    
    
    // Save skin style(s)
    /*
    GuiSkin skin2 = {0};
    GuiSkin* skins[] = {&skin, &skin2};
    GuiSaveSkin("single.skin", "test.png", &skin);
    GuiSaveSkinEx("multi.skin", "test.png", skins, 2);
    */
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
            
            if (dropsCount > 0) {
                if(IsFileExtension(droppedFiles[0], ".rgs")) GuiLoadStyle(droppedFiles[0]);
                else if(IsFileExtension(droppedFiles[0], ".ttf")) {
                    Font font = LoadFontEx(droppedFiles[0], GuiGetStyle(DEFAULT, TEXT_SIZE), NULL, 0);
                    GuiSetFont(font); 
                }
                else if(IsFileExtension(droppedFiles[0], ".skin")) {
                    GuiSkin s = {0};
                    if(GuiLoadSkin(droppedFiles[0], &s, 0)) {
                        Texture t = skin.tex;
                        skin = s;
                        UnloadTexture(t);
                    }
                }
            }
            
            ClearDroppedFiles();    // Clear internal buffers
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground( GetColor(0xf5f5f5ff) );
 
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
            
            GuiSetTooltip("Save current file.");
            if (GuiButton((Rectangle){ 25, 255, 125, 30 }, GuiIconText(RICON_FILE_SAVE, "Save File"))) showTextInputBox = true;
            GuiClearTooltip();
            
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
            if (GuiTextBoxMulti((Rectangle){ 320, 25, 225, 140 }, multiTextBoxText, 256, multiTextBoxEditMode)) multiTextBoxEditMode = !multiTextBoxEditMode;
            colorPickerValue = GuiColorPicker((Rectangle){ 320, 185, 196, 192 }, colorPickerValue);
            
            sliderValue = GuiSlider((Rectangle){ 355, 400, 165, 20 }, "TEST", TextFormat("%2.2f", (float)sliderValue), sliderValue, -50, 100);
            sliderBarValue = GuiSliderBar((Rectangle){ 320, 430, 200, 20 }, NULL, TextFormat("%i", (int)sliderBarValue), sliderBarValue, 0, 100);
            GuiProgressBar((Rectangle){ 320, 460, 200, 20 }, NULL, NULL, (float)sliderBarValue/100, 0, 1);

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
            DrawFPS(600, 500);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(skin.tex);
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
