/*******************************************************************************************
*
*   raygui - image exporter window box test
*
*   This example has been created using raylib v1.9 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2018 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, char *argv[0])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 450;
    
    // Window box 
    Rectangle windowBoxRec = { SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT/2 - 110, 260, 230 };
    bool windowBoxActive = false;
    
    // File format combo box
    int formatCount = 3;
    int formatSelected = 0;
    const char *formatText[3] = { "PNG", "RAW", "CODE"};
    
    // Pixel format combo box
    int pixelFormatCount = 6;
    int pixelActiveSelected = 0;
    const char *pixelFormatText[6] = { "R8G8B8A8", "R8", "R5G5B5A1" ,"R5G6B5", "R8A8", "R4G4B4A4" };
    
    // Toggle buttons
    bool toggleRed = true;
    bool toggleGreen = true;
    bool toggleBlue = true;
    bool toggleAlpha = true;
    
    // Text box
    char fileNameText[32] = "Untitled";

    //int dropdownSelected = 0;

    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "image exporter");

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // ...
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            // Draw message box (testing)
            //GuiMessageBox((Rectangle){600, 100, 150, 100}, "Image export options", "Hello World!\nHello World!");
            
            if (GuiButton((Rectangle){ 20, 20, 150, 30 }, "Show Export Window")) windowBoxActive = true;
            
            // Draw window box
            //-----------------------------------------------------------------------------
            if (windowBoxActive)
            {
                windowBoxActive = !GuiWindowBox(windowBoxRec, "Image export options");

                // Draw file format options
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 20 + 20, 0, 0 }, FormatText("File format"));
                formatSelected = GuiComboBox((Rectangle){ windowBoxRec.x + 10 + 90, windowBoxRec.y + 20 + 10, 150, 30 }, formatText, formatCount, formatSelected);
                
                // Draw pixel format options
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 20 + 60, 0, 0 }, FormatText("Pixel format"));
                pixelActiveSelected = GuiComboBox((Rectangle){ windowBoxRec.x + 10 + 90, windowBoxRec.y + 20 + 50, 150, 30 }, pixelFormatText, pixelFormatCount, pixelActiveSelected); 
                
                // Draw active channels options
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 20 + 100, 0, 0 }, FormatText("Active channels"));
                toggleRed = GuiToggleButton((Rectangle){ windowBoxRec.x + 10 + 90, windowBoxRec.y + 20 + 90, 36, 30}, "R", toggleRed);
                toggleGreen = GuiToggleButton((Rectangle){ windowBoxRec.x + 10 + 128, windowBoxRec.y + 20 + 90, 36, 30}, "G", toggleGreen);
                toggleBlue = GuiToggleButton((Rectangle){ windowBoxRec.x + 10 + 166, windowBoxRec.y + 20 + 90, 36, 30}, "B", toggleBlue);
                toggleAlpha = GuiToggleButton((Rectangle){ windowBoxRec.x + 10 + 204, windowBoxRec.y + 20 + 90, 36, 30}, "A", toggleAlpha);

                //Draw file name options
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 20 + 140, 0, 0 }, FormatText("File name"));
                GuiTextBox((Rectangle){ windowBoxRec.x + 10 + 90, windowBoxRec.y + 20 + 130, 150, 30 }, fileNameText, 32);
                
                // Draw export image button
                if (GuiButton((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 20 + 170, 150 + 90, 30 }, "Export Image")) { /* Call function */ }
            }
            //-----------------------------------------------------------------------------
            
            // Draw dropdown box (testing)
            //dropdownSelected = GuiDropdownBox((Rectangle){ 10, 10, 150, 30 }, formatText, formatCount, dropdownSelected);
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
