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
    int fileFormatActive = 0;
    const char *fileFormatTextList[3] = { "PNG", "RAW", "CODE"};
    
    // Pixel format combo box
    int pixelFormatCount = 6;
    int pixelFormatActive = 0;
    const char *pixelFormatTextList[6] = { "R8G8B8A8", "R8", "R5G5B5A1" ,"R5G6B5", "R8A8", "R4G4B4A4" };
    
    // Text box
    char fileName[32] = "Untitled";

    //int dropdownSelected = 0;
    SetConfigFlags(0x11100000);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "image exporter");
    
    int format = -1;
    int pixelFormat = 0;
    bool btnExport = false;
    
    Image image;
    Texture2D texture;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsFileDropped())
        {
            int fileCount = 0;
            char **droppedFiles = GetDroppedFiles(&fileCount);

            // Check file extensions for drag-and-drop
            if ((fileCount == 1) && ((IsFileExtension(droppedFiles[0], ".raw") || IsFileExtension(droppedFiles[0], ".png"))))
            {
                strcpy(fileName, GetFileName(droppedFiles[0]));
                
                image = LoadImage(fileName);
            }
            
            if (btnExport)
            {
                format = fileFormatActive;
                pixelFormat = pixelFormatActive;
                
                //ImageFormat(image, pixelFormat);
                if (format == 0) ExportImage(fileName, image);
                else if (format == 1) 
                {
                    
                }
                else if (format == 2) 
                {
                    
                }
                
                UnloadImage(image);
            }
            

            ClearDroppedFiles();
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            // Draw message box (testing)
            //GuiMessageBox((Rectangle){600, 100, 150, 100}, "Image export options", "Hello World!\nHello World!");
            
            DrawText("Drag & drop your image!", 300, 200, 10, DARKGRAY);
            
            if (GuiButton((Rectangle){ 20, 20, 150, 30 }, "Show Export Window")) windowBoxActive = true;
            
            // Draw window box: windowBoxName
            //-----------------------------------------------------------------------------
            if (windowBoxActive)
            {
                windowBoxActive = !GuiWindowBox((Rectangle){ windowBoxRec.x + 0, windowBoxRec.y + 0, 220, 190 }, "Image Export Options");
            
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 35, 60, 25 }, "File format:");
                fileFormatActive = GuiComboBox((Rectangle){ windowBoxRec.x + 80, windowBoxRec.y + 35, 130, 25 },  fileFormatTextList, 3, fileFormatActive); 
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 70, 63, 25 }, "Pixel format:");
                pixelFormatActive = GuiComboBox((Rectangle){ windowBoxRec.x + 80, windowBoxRec.y + 70, 130, 25 },  pixelFormatTextList, 6, pixelFormatActive); 
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 105, 50, 25 }, "File name:");
                GuiTextBox((Rectangle){ windowBoxRec.x + 80, windowBoxRec.y + 105, 130, 25 }, fileName, 64, true);
                btnExport = GuiButton((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 145, 200, 30 }, "Export Image");
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
