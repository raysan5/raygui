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

//#include "untitled.h"

// Static functions
static void ImageToCode(Image image, char *fileName);       // Exports image to code (.h)

const char *pixelFormatTextList[7] = { "GRAYSCALE", "GRAY ALPHA", "R5G6B5", "R8G8B8", "R5G5B5A1", "R4G4B4A4", "R8G8B8A8" };
int pixelFormatActive = 0;

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, char *argv[0])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    //SetConfigFlags();
    InitWindow(screenWidth, screenHeight, "image exporter");
    
    // Image export window variables
    Rectangle windowBoxRec = { screenWidth/2 - 130, screenHeight/2 - 110, 260, 230 };
    bool windowBoxActive = false;
    int fileFormatActive = 0;
    const char *fileFormatTextList[3] = { "IMAGE (.png)", "DATA (.raw)", "CODE (.h)" };
    char fileName[32] = "untitled";

    //Image image = { untitled_data, untitled_width, untitled_height, 1, untitled_format };
    //Texture2D texture = LoadTextureFromImage(image);
    Image image = { 0 };
    Texture2D texture = { 0 };
    bool imageLoaded = false;
    bool btnExport = false;
    float imageScale = 1.0f;

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
            if (fileCount == 1)
            {
                Image imTemp = LoadImage(droppedFiles[0]);
                
                if (imTemp.data != NULL)
                {
                    UnloadImage(image);
                    image = imTemp;
                    
                    UnloadTexture(texture);
                    texture = LoadTextureFromImage(image);
                    
                    imageLoaded = true;
                    pixelFormatActive = image.format - 1;
                    
                    if (texture.height > texture.width) imageScale = (float)(screenHeight - 100)/(float)texture.height;
                    else imageScale = (float)(screenWidth - 100)/(float)texture.width;
                }
            }

            ClearDroppedFiles();
        }
    
        if (btnExport)
        {
            if (imageLoaded)
            {
                ImageFormat(&image, pixelFormatActive + 1);
                
                if (fileFormatActive == 0)        // PNG
                {
                    if ((GetExtension(fileName) == NULL) || (!IsFileExtension(fileName, ".png"))) strcat(fileName, ".png\0");     // No extension provided
                    ExportImage(fileName, image);
                }
                else if (fileFormatActive == 1)   // RAW
                {
                    if ((GetExtension(fileName) == NULL) || (!IsFileExtension(fileName, ".raw"))) strcat(fileName, ".raw\0");     // No extension provided
                    
                    int dataSize = GetPixelDataSize(image.width, image.height, image.format);
                    
                    FILE *rawFile = fopen(fileName, "wb");  
                    fwrite(image.data, dataSize, 1, rawFile);
                    fclose(rawFile);
                }
                else if (fileFormatActive == 2)   // CODE
                {
                    ImageToCode(image, fileName);
                }
            }
            
            windowBoxActive = false;
        }
        
        if (imageLoaded)
        {
            imageScale += (float)GetMouseWheelMove()*0.05f;   // Image scale control
            if (imageScale <= 0.1f) imageScale = 0.1f;
            else if (imageScale >= 5) imageScale = 5;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            if (texture.id > 0)
            {
                DrawTextureEx(texture, (Vector2){ screenWidth/2 - (float)texture.width*imageScale/2, screenHeight/2 - (float)texture.height*imageScale/2 }, 0.0f, imageScale, WHITE);
                DrawText(FormatText("SCALE: %.2f%%", imageScale*100.0f), 20, screenHeight - 40, 20, GetColor(style[DEFAULT_LINES_COLOR]));
            }
            else
            {
                DrawText("Drag & drop your image!", 300, 200, 10, DARKGRAY);
                GuiDisable();
            }
            
            if (GuiButton((Rectangle){ screenWidth - 170, screenHeight - 50, 150, 30 }, "Show Export Window")) windowBoxActive = true;
            GuiEnable();
            
            // Draw window box: windowBoxName
            //-----------------------------------------------------------------------------
            if (windowBoxActive)
            {
                DrawRectangle(0,0, screenWidth, screenHeight, Fade(GetColor(style[DEFAULT_BACKGROUND_COLOR]), 0.7f));
                windowBoxActive = !GuiWindowBox((Rectangle){ windowBoxRec.x + 0, windowBoxRec.y + 0, 220, 190 }, "Image Export Options");
            
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 35, 60, 25 }, "File format:");
                fileFormatActive = GuiComboBox((Rectangle){ windowBoxRec.x + 80, windowBoxRec.y + 35, 130, 25 },  fileFormatTextList, 3, fileFormatActive); 
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 70, 63, 25 }, "Pixel format:");
                pixelFormatActive = GuiComboBox((Rectangle){ windowBoxRec.x + 80, windowBoxRec.y + 70, 130, 25 },  pixelFormatTextList, 7, pixelFormatActive); 
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 105, 50, 25 }, "File name:");
                GuiTextBox((Rectangle){ windowBoxRec.x + 80, windowBoxRec.y + 105, 130, 25 }, fileName, 64, true);
                
                btnExport = GuiButton((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 145, 200, 30 }, "Export Image");
            }
            else btnExport = false;
            
            if (btnExport) DrawText("Image exported!", 20, screenHeight - 20, 20, RED);
            //-----------------------------------------------------------------------------

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadImage(image);
    UnloadTexture(texture);
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

static void ImageToCode(Image image, char *fileName)
{
    char outName[32] = "\0";
    
    strcpy(outName, fileName);
    if ((GetExtension(fileName) == NULL) || (!IsFileExtension(fileName, ".h"))) strcat(fileName, ".h\0");     // No extension provided
    
    int dataSize = GetPixelDataSize(image.width, image.height, image.format);
    
    FILE *txtFile = fopen(fileName, "wt");
    
    // TODO: Add image information
    fprintf(txtFile, "//\n//  Image exported using raygui image_exporter example\n//\n//  Copyright (c) 2018 raylib technologies (@raylibtech)\n//\n\n");
    fprintf(txtFile, "#define %s_width    %i\n", outName, image.width);
    fprintf(txtFile, "#define %s_height   %i\n", outName, image.height);
    fprintf(txtFile, "#define %s_format   %i    // raylib internal pixel format: %s\n\n", outName, pixelFormatActive + 1, pixelFormatTextList[pixelFormatActive]);

    fprintf(txtFile, "static unsigned char %s_data[%i] = { ", outName, dataSize);
    for (int i = 0; i < dataSize - 1; i++) fprintf(txtFile, "0x%x, ", ((unsigned char *)image.data)[i]);
    fprintf(txtFile, "0x%x };\n", ((unsigned char *)image.data)[dataSize - 1]);

    fclose(txtFile);
}
