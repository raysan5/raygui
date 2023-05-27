/*******************************************************************************************
*
*   raygui - image raw importer
*
*   DEPENDENCIES:
*       raylib 4.0  - Windowing/input management and drawing.
*       raygui 3.0  - Immediate-mode GUI controls.
*
*   COMPILATION (Windows - MinGW):
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -I../../src -lraylib -lopengl32 -lgdi32 -std=c99
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2015-2023 Ramon Santamaria (@raysan5)
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../../src/raygui.h"

#include <string.h>             // Required for: strcpy()
#include <stdlib.h>             // Required for: atoi()
#include <math.h>               // Required for: round()

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 600;

    InitWindow(screenWidth, screenHeight, "raygui - image raw importer");
    
    Texture2D texture = { 0 };

    // GUI controls initialization
    //----------------------------------------------------------------------------------
    Vector2 windowOffset = { screenWidth/2 - 200/2, screenHeight/2 - 465/2 };

    bool importWindowActive = false;
    
    int widthValue = 0;
    bool widthEditMode = false;
    int heightValue = 0;
    bool heightEditMode = false;
    
    int pixelFormatActive = 0;
    const char *pixelFormatTextList[8] = { "CUSTOM", "GRAYSCALE", "GRAY ALPHA", "R5G6B5", "R8G8B8", "R5G5B5A1", "R4G4B4A4", "R8G8B8A8" };

    int channelsActive = 3;
    const char *channelsTextList[4] = { "1", "2", "3", "4" };
    int bitDepthActive = 0;
    const char *bitDepthTextList[3] = { "8", "16", "32" };
    
    int headerSizeValue = 0;
    bool headerSizeEditMode = false;
    //----------------------------------------------------------------------------------
    
    // Image file info
    int dataSize = 0;
    char fileNamePath[256] = "\0";
    char fileName[64] = "\0";
    
    bool btnLoadPressed = false;
    
    bool imageLoaded = false;
    float imageScale = 1.0f;

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
            FilePathList droppedFiles = LoadDroppedFiles();

            // Check file extensions for drag-and-drop
            if ((droppedFiles.count == 1) && IsFileExtension(droppedFiles.paths[0], ".raw"))
            {
                FILE *imageFile = fopen(droppedFiles.paths[0], "rb");
                fseek(imageFile, 0L, SEEK_END);
                dataSize = ftell(imageFile);
                fclose(imageFile);
                
                // NOTE: Returned string is just a pointer to droppedFiles[0],
                // we need to make a copy of that data somewhere else: fileName
                strcpy(fileNamePath, droppedFiles.paths[0]);
                strcpy(fileName, GetFileName(droppedFiles.paths[0]));
                
                // Try to guess possible raw values
                // Let's assume image is square, RGBA, 8 bit per channel
                widthValue = round(sqrt(dataSize/4));
                heightValue = widthValue;
                headerSizeValue = dataSize - widthValue*heightValue*4;
                if (headerSizeValue < 0) headerSizeValue = 0;

                importWindowActive = true;
            }

            UnloadDroppedFiles(droppedFiles);
        }
        
        // Check if load button has been pressed
        if (btnLoadPressed)
        {
            // Depending on channels and bit depth, select correct pixel format
            if ((widthValue != 0) && (heightValue != 0))
            {
                int format = -1;

                if (pixelFormatActive == 0)
                {
                    int channels = atoi(channelsTextList[channelsActive]);
                    int bpp = atoi(bitDepthTextList[bitDepthActive]);
            
                    // Select correct format depending on channels and bpp
                    if (bpp == 8)
                    {
                        if (channels == 1) format = PIXELFORMAT_UNCOMPRESSED_GRAYSCALE;
                        else if (channels == 2) format = PIXELFORMAT_UNCOMPRESSED_GRAY_ALPHA;
                        else if (channels == 3) format = PIXELFORMAT_UNCOMPRESSED_R8G8B8;
                        else if (channels == 4) format = PIXELFORMAT_UNCOMPRESSED_R8G8B8A8;
                    }
                    else if (bpp == 32)
                    {
                        if (channels == 1) format = PIXELFORMAT_UNCOMPRESSED_R32;
                        else if (channels == 2) TraceLog(LOG_WARNING, "Channel bit-depth not supported!");
                        else if (channels == 3) format = PIXELFORMAT_UNCOMPRESSED_R32G32B32;
                        else if (channels == 4) format = PIXELFORMAT_UNCOMPRESSED_R32G32B32A32;
                    }
                    else if (bpp == 16) TraceLog(LOG_WARNING, "Channel bit-depth not supported!");
                }
                else format = pixelFormatActive;
                
                if (format != -1)
                {
                    Image image = LoadImageRaw(fileNamePath, widthValue, heightValue, format, headerSizeValue);
                    texture = LoadTextureFromImage(image);
                    UnloadImage(image);
                    
                    importWindowActive = false;
                    btnLoadPressed = false;
                    
                    if (texture.id > 0)
                    {
                        imageLoaded = true;
                        imageScale = (float)(screenHeight - 100)/texture.height;
                    }
                }
            }
        }
        
        if (imageLoaded) imageScale += (float)GetMouseWheelMove();   // Image scale control
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
            
            if (texture.id != 0) 
            {
                DrawTextureEx(texture, (Vector2){ screenWidth/2 - texture.width*imageScale/2, screenHeight/2 - texture.height*imageScale/2 }, 0, imageScale, WHITE);
                DrawText(TextFormat("SCALE x%.0f", imageScale), 20, screenHeight - 40, 20, GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)));
            }
            else DrawText("drag & drop RAW image file", 320, 180, 10, GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)));

            // raygui: controls drawing
            //----------------------------------------------------------------------------------
            if (importWindowActive)
            {
                importWindowActive = !GuiWindowBox((Rectangle){ windowOffset.x + 0, windowOffset.y + 0, 200, 465 }, "Image RAW Import Options");
            
                GuiLabel((Rectangle){ windowOffset.x + 10, windowOffset.y + 30, 65, 20 }, "Import file:");
                GuiLabel((Rectangle){ windowOffset.x + 85, windowOffset.y + 30, 75, 20 }, fileName);
                GuiLabel((Rectangle){ windowOffset.x + 10, windowOffset.y + 50, 65, 20 }, "File size:");
                GuiLabel((Rectangle){ windowOffset.x + 85, windowOffset.y + 50, 75, 20 }, TextFormat("%i bytes", dataSize));
                GuiGroupBox((Rectangle){ windowOffset.x + 10, windowOffset.y + 85, 180, 80 }, "Resolution");
                GuiLabel((Rectangle){ windowOffset.x + 20, windowOffset.y + 100, 33, 25 }, "Width:");
                if (GuiValueBox((Rectangle){ windowOffset.x + 60, windowOffset.y + 100, 80, 25 }, NULL, &widthValue, 0, 8192, widthEditMode)) widthEditMode = !widthEditMode; 
                GuiLabel((Rectangle){ windowOffset.x + 145, windowOffset.y + 100, 30, 25 }, "pixels");
                GuiLabel((Rectangle){ windowOffset.x + 20, windowOffset.y + 130, 33, 25 }, "Height:");
                if (GuiValueBox((Rectangle){ windowOffset.x + 60, windowOffset.y + 130, 80, 25 }, NULL, &heightValue, 0, 8192, heightEditMode)) heightEditMode = !heightEditMode; 
                GuiLabel((Rectangle){ windowOffset.x + 145, windowOffset.y + 130, 30, 25 }, "pixels");
                GuiGroupBox((Rectangle){ windowOffset.x + 10, windowOffset.y + 180, 180, 160 }, "Pixel Format");
                GuiComboBox((Rectangle){ windowOffset.x + 20, windowOffset.y + 195, 160, 25 }, TextJoin(pixelFormatTextList, 8, ";"), &pixelFormatActive);
                GuiLine((Rectangle){ windowOffset.x + 20, windowOffset.y + 220, 160, 20 }, NULL);
                
                if (pixelFormatActive != 0) GuiDisable();
                GuiLabel((Rectangle){ windowOffset.x + 20, windowOffset.y + 235, 50, 20 }, "Channels:");
                GuiToggleGroup((Rectangle){ windowOffset.x + 20, windowOffset.y + 255, 156/4, 25 }, TextJoin(channelsTextList, 4, ";"), &channelsActive);
                GuiLabel((Rectangle){ windowOffset.x + 20, windowOffset.y + 285, 50, 20 }, "Bit Depth:");
                GuiToggleGroup((Rectangle){ windowOffset.x + 20, windowOffset.y + 305, 160/3, 25 }, TextJoin(bitDepthTextList, 3, ";"), &bitDepthActive);
                GuiEnable();
                
                GuiGroupBox((Rectangle){ windowOffset.x + 10, windowOffset.y + 355, 180, 50 }, "Header");
                GuiLabel((Rectangle){ windowOffset.x + 25, windowOffset.y + 370, 27, 25 }, "Size:");
                if (GuiValueBox((Rectangle){ windowOffset.x + 55, windowOffset.y + 370, 85, 25 }, NULL, &headerSizeValue, 0, 10000, headerSizeEditMode)) headerSizeEditMode = !headerSizeEditMode; 
                GuiLabel((Rectangle){ windowOffset.x + 145, windowOffset.y + 370, 30, 25 }, "bytes");
                
                btnLoadPressed = GuiButton((Rectangle){ windowOffset.x + 10, windowOffset.y + 420, 180, 30 }, "Import RAW");
            }
            //----------------------------------------------------------------------------------

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
