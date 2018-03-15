/*******************************************************************************************
*
*   raygui - raw image file importer
*
*   This example has been created using raylib v1.7 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2017 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_STYLE_SAVE_LOAD
#include "raygui.h"

#include <string.h>             // Required for: strcpy()
#include <stdlib.h>             // Required for: atoi()


//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, char *argv[0])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int SCREEN_WIDTH = 800;
    const int SCREEN_HEIGHT = 450;
    
    int guiHeight = 30;
    int guiWidth = 150;
    
    Rectangle panel = { SCREEN_WIDTH/2 - 130, SCREEN_HEIGHT/2 - 110, 260, 230 };
    
    int guiPosX = panel.x + 10;
    int guiPosY = panel.y + 20;
   
    
    int formatNum = 3;
    int formatActive = 0;
    int pixelNum = 8;
    int pixelActive = 0;
    //int channelsNum = 4;
    //int channelsActive = 0;
    int dropdownTest = 0;
    bool toggleRed = true;
    bool toggleGreen = true;
    bool toggleBlue = true;
    bool toggleAlpha = true;
    const char *formatText[3] = { "PNG", "RAW", "CODE (.c/.h)"};
    const char *formatPixel[8] = { "R8G8B8A8", "R8", "R5G5B5A1" ,"R5G6B5", "R8A8", "R16G16B16", "R4G4B4A4", "R2D2C3P0" };
    char fileNameText[32] = "Untitled"; 
    //const char *formatChannels[4] = { "R", "G", "B" ,"A" };
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "image exporter");
    
    Texture2D texture = { 0 };
    
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            GuiDisable();
            GuiWindowBox(panel, "Image export options");
            GuiEnable();
            //Draw file format options
            GuiLabel((Rectangle){ guiPosX, guiPosY + 20, 0, 0 }, FormatText("File format"));
            formatActive = GuiComboBox((Rectangle){ guiPosX + 90, guiPosY + 10, guiWidth, guiHeight },formatText,formatNum,formatActive);
            
            //Draw pixel format options
            GuiLabel((Rectangle){ guiPosX, guiPosY + 60, 0, 0 }, FormatText("Pixel format"));
            pixelActive = GuiComboBox((Rectangle){ guiPosX + 90, guiPosY + 50, guiWidth, guiHeight },formatPixel,pixelNum,pixelActive); 
            
            //Draw active channels options
            GuiLabel((Rectangle){ guiPosX, guiPosY + 100, 0, 0 }, FormatText("Active channels"));
            toggleRed = GuiToggleButton((Rectangle){ guiPosX + 100, guiPosY + 90, guiWidth/5, guiHeight}, "R", toggleRed);
            toggleGreen = GuiToggleButton((Rectangle){ guiPosX + 132, guiPosY + 90, guiWidth/5, guiHeight}, "G", toggleGreen);
            toggleBlue = GuiToggleButton((Rectangle){ guiPosX + 164, guiPosY + 90, guiWidth/5, guiHeight}, "B", toggleBlue);
            toggleAlpha = GuiToggleButton((Rectangle){ guiPosX + 196, guiPosY + 90, guiWidth/5, guiHeight}, "A", toggleAlpha);
            //channelsActive = GuiToggleGroup((Rectangle){ guiPosX + 100, guiPosY + 90, guiWidth/5, guiHeight },formatChannels,channelsNum,channelsActive);
            
            //Draw file name options
            GuiLabel((Rectangle){ guiPosX, guiPosY + 140, 0, 0 }, FormatText("File name"));
            GuiTextBox((Rectangle){ guiPosX + 90, guiPosY + 130, guiWidth, guiHeight }, fileNameText, 32);
            
            //Draw export image button
            if(GuiButton((Rectangle){ guiPosX, guiPosY + 170, guiWidth + 90, guiHeight }, "Export Image")){} // Call function
            
            dropdownTest = GuiDropdownBox((Rectangle){ 10, 10, 200, 20 }, formatText, formatNum, dropdownTest);
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    if (texture.id != 0) UnloadTexture(texture);
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}
