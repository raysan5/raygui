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
#include "raygui.h"

#include <string.h>

char *GetFileName(char *path);
void GuiPanel(Rectangle bounds, char const *text);

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

    int buttonToggleChannel = 3;
    int buttonToggleDepth = 0;
    char *arrayChannel[4] = { "1", "2", "3", "4" };
    char *arrayDepth[3] = { "8", "16", "32" };
    
    // Image file info
    int dataSize = 0;
    char fileName[32] = "\0";
    
    bool showImportPanel = false;
    bool imageLoaded = false;
    
    float imageScale = 1.0f;
    
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
                
                strcpy(fileName, GetFileName(droppedFiles[0]));

                showImportPanel = true;
            }

            ClearDroppedFiles();
        }
        
        if (btnLoadPressed)
        {
            // Convert input textImageWidth, textImageHeight to int
            int width = atoi(textImageWidth);
            int height = atoi(textImageHeight);
            int format = UNCOMPRESSED_R8G8B8A8;
            
            int channels = atoi(arrayChannel[buttonToggleChannel]);
            int bpp = atoi(arrayDepth[buttonToggleDepth]);
            
            int headerSize = atoi(textImageSize);
            
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
                imageLoaded = true;
            }
        }
        
        if (imageLoaded)
        {
            // Image zoom control
            imageScale += (float)GetMouseWheelMove();
        }
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            DrawRectangleLines(10, 10, SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20, GuiLinesColor());
            
            if (texture.id != 0) DrawTextureEx(texture, (Vector2){ SCREEN_WIDTH/2 - texture.width*imageScale/2, SCREEN_HEIGHT/2 - texture.height*imageScale/2 }, 0, imageScale, WHITE);
            else DrawText("drag & drop RAW image file", 320, 180, 10, GRAY);

            if (showImportPanel)
            {
                DrawRectangle(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Fade(LIGHTGRAY, 0.5f));
                
                DrawRectangleRec(panel, GuiBackgroundColor());
                DrawRectangleLines(panel.x, panel.y, panel.width, panel.height, GuiLinesColor());

                GuiLabel((Rectangle){ panel.x + 10, panel.y + 10, 0, 0 }, FormatText("Import file: %s", fileName));
                GuiLabel((Rectangle){ panel.x + 10, panel.y + 30, 0, 0 }, FormatText("File size: %i bytes", dataSize));
                
                // ----- Resolution panel-----
                GuiPanel((Rectangle){ panel.x + 10, panel.y + 55, 180, 80 }, "Resolution");
                
                GuiLabel((Rectangle){ panel.x + 20, panel.y + 80, 0, 0 }, "Width:");
                GuiLabel((Rectangle){ panel.x + 150, panel.y + 80, 0, 0 }, "pixels");
                
                GuiTextBox((Rectangle){ panel.x + 60, panel.y + 75, 80, 20 }, textImageWidth, 16);
                
                GuiLabel((Rectangle){ panel.x + 20, panel.y + 105, 0, 0 }, "Height:");
                GuiLabel((Rectangle){ panel.x + 150, panel.y + 105, 0, 0 }, "pixels");
                
                GuiTextBox((Rectangle){ panel.x + 60, panel.y + 100, 80, 20 }, textImageHeight, 16);
                
                // ----- Pixel data panel -----
                GuiPanel((Rectangle){ panel.x + 10, panel.y + 155, 180, 110 }, "Pixels Data");
                
                GuiLabel((Rectangle){ panel.x + 20, panel.y + 170, 0, 0 }, "Channels:");
                GuiLabel((Rectangle){ panel.x + 20, panel.y + 215, 0, 0 }, "Bit Depth:");
                
                buttonToggleChannel = GuiToggleGroup((Rectangle){ panel.x + 20, panel.y + 185, 38, 20 }, 4, arrayChannel, buttonToggleChannel);
                buttonToggleDepth = GuiToggleGroup((Rectangle){ panel.x + 20, panel.y + 230, 51, 20 }, 3, arrayDepth, buttonToggleDepth);
                
                // ----- Header panel -----
                GuiPanel((Rectangle){ panel.x + 10, panel.y + 285, 180, 50 }, "Header");
                
                GuiLabel((Rectangle){ panel.x + 20, panel.y + 305, 0, 0 }, "Size:");
                GuiLabel((Rectangle){ panel.x + 150, panel.y + 305, 0, 0 }, "bytes");
                
                GuiTextBox((Rectangle){ panel.x + 60, panel.y + 300, 80, 20 }, textImageSize, 4);
                
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

void GuiPanel(Rectangle bounds, char const *text)
{
    DrawLineEx((Vector2){ bounds.x + 1, bounds.y }, (Vector2){ bounds.x, bounds.y + bounds.height }, 1, GuiLinesColor());
    DrawLineEx((Vector2){ bounds.x, bounds.y + bounds.height }, (Vector2){ bounds.x + bounds.width, bounds.y + bounds.height }, 1, GuiLinesColor());
    DrawLineEx((Vector2){ bounds.x + bounds.width, bounds.y + bounds.height }, (Vector2){ bounds.x + bounds.width, bounds.y }, 1, GuiLinesColor());
    DrawLineEx((Vector2){ bounds.x, bounds.y }, (Vector2){ bounds.x + 10, bounds.y }, 1, GuiLinesColor());
    DrawLineEx((Vector2){ bounds.x + bounds.width, bounds.y }, (Vector2){ bounds.x + 20 + MeasureText(text, 10), bounds.y }, 1, GuiLinesColor());
    
    DrawText(text, bounds.x + 14, bounds.y - 5, 10, GuiTextColor());
}

char *GetFileName(char *path)
{
    char *base = strrchr(path, '\\');

    return base ? base + 1 : path;
}