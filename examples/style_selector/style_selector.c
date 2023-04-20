/*******************************************************************************************
*
*   raygui - style selector
*
*   DEPENDENCIES:
*       raylib 4.5          - Windowing/input management and drawing
*       raygui 3.5          - Immediate-mode GUI controls with custom styling and icons
*
*   COMPILATION (Windows - MinGW):
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -I../../src -lraylib -lopengl32 -lgdi32 -std=c99
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2016-2023 Ramon Santamaria (@raysan5)
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
//#define RAYGUI_CUSTOM_ICONS     // It requires providing gui_icons.h in the same directory
//#include "gui_icons.h"          // External icons data provided, it can be generated with rGuiIcons tool
#include "../../src/raygui.h"

// raygui embedded styles
#include "styles/style_cyber.h"             // raygui style: cyber
#include "styles/style_jungle.h"            // raygui style: jungle
#include "styles/style_lavanda.h"           // raygui style: lavanda
#include "styles/style_dark.h"              // raygui style: dark
#include "styles/style_bluish.h"            // raygui style: bluish
#include "styles/style_terminal.h"          // raygui style: terminal

#include <string.h>             // Required for: strcpy()

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    const int screenWidth = 690;
    const int screenHeight = 560;

    InitWindow(screenWidth, screenHeight, "raygui - controls test suite");
    SetExitKey(0);

    // Custom GUI font loading
    //Font font = LoadFontEx("fonts/custom_font.ttf", 12, 0, 0);
    //GuiSetFont(font);

    bool exitWindow = false;
    bool showMessageBox = false;

    char textInput[256] = { 0 };
    bool showTextInputBox = false;

    char textInputFileName[256] = { 0 };

    // Load default style
    GuiLoadStyleBluish();
    int visualStyleActive = 4;
    int prevVisualStyleActive = 4;

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
            FilePathList droppedFiles = LoadDroppedFiles();

            if ((droppedFiles.count > 0) && IsFileExtension(droppedFiles.paths[0], ".rgs")) GuiLoadStyle(droppedFiles.paths[0]);

            UnloadDroppedFiles(droppedFiles);    // Clear internal buffers
        }

        if (visualStyleActive != prevVisualStyleActive)
        {
            GuiLoadStyleDefault();

            switch (visualStyleActive)
            {
                case 0: break;      // Default style
                case 1: GuiLoadStyleJungle(); break;
                case 2: GuiLoadStyleLavanda(); break;
                case 3: GuiLoadStyleDark(); break;
                case 4: GuiLoadStyleBluish(); break;
                case 5: GuiLoadStyleCyber(); break;
                case 6: GuiLoadStyleTerminal(); break;
                default: break;
            }

            GuiSetStyle(LABEL, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);

            prevVisualStyleActive = visualStyleActive;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            // Visuals options
            GuiLabel((Rectangle){ 10, 10, 60, 24 }, "Style:");
            visualStyleActive = GuiComboBox((Rectangle){ 60,10, 120, 24 }, "default;Jungle;Lavanda;Dark;Bluish;Cyber;Terminal", visualStyleActive);

            GuiSetIconScale(2);
            GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_RIGHT);
            GuiButton((Rectangle){ 25, 255, 300, 30 }, GuiIconText(ICON_FILE_SAVE, "Save File"));
            GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);
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
