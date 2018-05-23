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
    
    Rectangle panel = { SCREEN_WIDTH/2 - 100, SCREEN_HEIGHT/2 - 390/2, 200, 390 };
    
    char textImageWidth[16] = "\0";
    char textImageHeight[16] = "\0";
    char textImageSize[16] = "\0";
    
    bool btnLoadPressed = false;

    int buttonToggleChannel = 1;
    int buttonToggleDepth = 0;
    const char *arrayChannel[4] = { "1", "2", "3", "4" };
    const char *arrayDepth[3] = { "8", "16", "32" };
    
    // Image file info
    int dataSize = 0;
    char fileName[32] = "\0";
    
    bool showImportPanel = false;
    bool imageLoaded = false;
    float imageScale = 1.0f;

    // Raw image import values
    int width = 0;
    int height = 0;
    int format = UNCOMPRESSED_R8G8B8A8;
    int headerSize = 0;
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raw image importer");

    Texture2D texture = { 0 };
    
    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        
        // Check if a file is dropped
        if (IsFileDropped())
        {
            int fileCount = 0;
            char **droppedFiles = GetDroppedFiles(&fileCount);

            // Check file extensions for drag-and-drop
            if ((fileCount == 1) && IsFileExtension(droppedFiles[0], ".raw"))
            {
                FILE *imageFile = fopen(droppedFiles[0], "rb");
                fseek(imageFile, 0L, SEEK_END);
                dataSize = ftell(imageFile);
                fclose(imageFile);
                
                // NOTE: Returned string is just a pointer to droppedFiles[0],
                // we need to make a copy of that data somewhere else: fileName
                strcpy(fileName, GetFileName(droppedFiles[0]));
                
                // Try to guess possible raw values
                // Let's assume image is square, RGBA, 8 bit per channel
                width = round(sqrt(dataSize/4));
                height = width;
                headerSize = dataSize - width*height*4;
                
                sprintf(textImageWidth, "%i", width);
                sprintf(textImageHeight, "%i", height);
                sprintf(textImageSize, "%i", headerSize);

                showImportPanel = true;
            }

            ClearDroppedFiles();
        }
        
        // Check if load button has been pressed
        if (btnLoadPressed)
        {
            // Convert input textImageWidth, textImageHeight to int
            width = atoi(textImageWidth);
            height = atoi(textImageHeight);
            format = UNCOMPRESSED_R8G8B8A8;
            
            int channels = atoi(arrayChannel[buttonToggleChannel]);
            int bpp = atoi(arrayDepth[buttonToggleDepth]);
            
            headerSize = atoi(textImageSize);
            
            // Depending on channels and bit depth, select correct pixel format
            if ((width != 0) && (height != 0) && (bpp == 8))
            {
                switch (channels)
                {
                    case 1: format = UNCOMPRESSED_GRAYSCALE; break;
                    case 2: format = UNCOMPRESSED_GRAY_ALPHA; break;
                    case 3: format = UNCOMPRESSED_R8G8B8; break;
                    case 4: format = UNCOMPRESSED_R8G8B8A8; break;
                    default: break;
                }
                
                Image image = LoadImageRaw(fileName, width, height, format, headerSize);
                texture = LoadTextureFromImage(image);
                UnloadImage(image);
                
                showImportPanel = false;
                btnLoadPressed = false;
                
                if (texture.id > 0)
                {
                    imageLoaded = true;
                    imageScale = 400.0f/texture.height;
                }
            }
        }
        
        if (imageLoaded) imageScale += (float)GetMouseWheelMove();   // Image scale control
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            DrawRectangleLines(10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20, BLUE);

            if (texture.id != 0) 
            {
                DrawTextureEx(texture, (Vector2){ SCREEN_WIDTH/2 - texture.width*imageScale/2, SCREEN_HEIGHT/2 - texture.height*imageScale/2 }, 0, imageScale, WHITE);
                DrawText(FormatText("SCALE x%.0f", imageScale), 20, SCREEN_HEIGHT - 40, 20, BLUE);
                
            }
            else DrawText("drag & drop RAW image file", 320, 180, 10, BLUE);

            if (showImportPanel)
            {
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(LIGHTGRAY, 0.5f));
                
                DrawRectangleRec(panel, RAYWHITE);
                DrawRectangleLines(panel.x, panel.y, panel.width, panel.height, BLUE);

                GuiLabel((Rectangle){ panel.x + 10, panel.y + 10, 0, 0 }, FormatText("Import file: %s", fileName));
                GuiLabel((Rectangle){ panel.x + 10, panel.y + 30, 0, 0 }, FormatText("File size: %i bytes", dataSize));
                
                // ----- Resolution panel-----
                GuiGroupBox((Rectangle){ panel.x + 10, panel.y + 55, 180, 80 }, "Resolution");
                
                GuiLabel((Rectangle){ panel.x + 20, panel.y + 80, 0, 0 }, "Width:");
                GuiLabel((Rectangle){ panel.x + 150, panel.y + 80, 0, 0 }, "pixels");
                
                GuiTextBox((Rectangle){ panel.x + 60, panel.y + 75, 80, 20 }, textImageWidth, 4, false);
                
                GuiLabel((Rectangle){ panel.x + 20, panel.y + 105, 0, 0 }, "Height:");
                GuiLabel((Rectangle){ panel.x + 150, panel.y + 105, 0, 0 }, "pixels");
                
                GuiTextBox((Rectangle){ panel.x + 60, panel.y + 100, 80, 20 }, textImageHeight, 4, false);
                
                // ----- Pixel data panel -----
                GuiGroupBox((Rectangle){ panel.x + 10, panel.y + 155, 180, 110 }, "Pixels Data");
                
                GuiLabel((Rectangle){ panel.x + 20, panel.y + 170, 0, 0 }, "Channels:");
                GuiLabel((Rectangle){ panel.x + 20, panel.y + 215, 0, 0 }, "Bit Depth:");
                
                buttonToggleChannel = GuiToggleGroup((Rectangle){ panel.x + 20, panel.y + 185, 38, 20 }, arrayChannel, 4, buttonToggleChannel);
                buttonToggleDepth = GuiToggleGroup((Rectangle){ panel.x + 20, panel.y + 230, 51, 20 }, arrayDepth, 3, buttonToggleDepth);
                
                // ----- Header panel -----
                GuiGroupBox((Rectangle){ panel.x + 10, panel.y + 285, 180, 50 }, "Header");
                
                GuiLabel((Rectangle){ panel.x + 20, panel.y + 305, 0, 0 }, "Size:");
                GuiLabel((Rectangle){ panel.x + 150, panel.y + 305, 0, 0 }, "bytes");
                
                GuiTextBox((Rectangle){ panel.x + 60, panel.y + 300, 80, 20 }, textImageSize, 4, false);
                
                btnLoadPressed = GuiButton((Rectangle){ panel.x + 10, panel.y + 350, 180, 24 }, "Load RAW Image");
            }
           
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