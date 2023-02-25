/*******************************************************************************************
*
*   raygui - image exporter
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
********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../../src/raygui.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;
    
    InitWindow(screenWidth, screenHeight, "raygui - image exporter");
    
    // GUI controls initialization
    //----------------------------------------------------------------------------------
    Rectangle windowBoxRec = { screenWidth/2 - 110, screenHeight/2 - 100, 220, 190 };
    bool windowBoxActive = false;
    
    int fileFormatActive = 0;
    const char *fileFormatTextList[3] = { "IMAGE (.png)", "DATA (.raw)", "CODE (.h)" };

    int pixelFormatActive = 0;
    const char *pixelFormatTextList[7] = { "GRAYSCALE", "GRAY ALPHA", "R5G6B5", "R8G8B8", "R5G5B5A1", "R4G4B4A4", "R8G8B8A8" };

    bool textBoxEditMode = false;
    char fileName[32] = "untitled";
    //--------------------------------------------------------------------------------------
    
    Image image = { 0 };
    Texture2D texture = { 0 };
    
    bool imageLoaded = false;
    float imageScale = 1.0f;
    Rectangle imageRec = { 0 };
    
    bool btnExport = false;

    SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsFileDropped())
        {
            FilePathList droppedFiles = LoadDroppedFiles();

            if (droppedFiles.count == 1)
            {
                Image imTemp = LoadImage(droppedFiles.paths[0]);
                
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

            UnloadDroppedFiles(droppedFiles);
        }
    
        if (btnExport)
        {
            if (imageLoaded)
            {
                ImageFormat(&image, pixelFormatActive + 1);
                
                if (fileFormatActive == 0)        // PNG
                {
                    if ((GetFileExtension(fileName) == NULL) || (!IsFileExtension(fileName, ".png"))) strcat(fileName, ".png\0");     // No extension provided
                    ExportImage(image, fileName);
                }
                else if (fileFormatActive == 1)   // RAW
                {
                    if ((GetFileExtension(fileName) == NULL) || (!IsFileExtension(fileName, ".raw"))) strcat(fileName, ".raw\0");     // No extension provided
                    
                    int dataSize = GetPixelDataSize(image.width, image.height, image.format);
                    
                    FILE *rawFile = fopen(fileName, "wb");  
                    fwrite(image.data, 1, dataSize, rawFile);
                    fclose(rawFile);
                }
                else if (fileFormatActive == 2)   // CODE
                {
                    ExportImageAsCode(image, fileName);
                }
            }
            
            windowBoxActive = false;
        }
        
        if (imageLoaded)
        {
            imageScale += (float)GetMouseWheelMove()*0.05f;   // Image scale control
            if (imageScale <= 0.1f) imageScale = 0.1f;
            else if (imageScale >= 5) imageScale = 5;
            
            imageRec = (Rectangle){ screenWidth/2 - (float)image.width*imageScale/2, 
                                    screenHeight/2 - (float)image.height*imageScale/2, 
                                    (float)image.width*imageScale, (float)image.height*imageScale };
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);

            if (texture.id > 0)
            {
                DrawTextureEx(texture, (Vector2){ screenWidth/2 - (float)texture.width*imageScale/2, screenHeight/2 - (float)texture.height*imageScale/2 }, 0.0f, imageScale, WHITE);
                
                DrawRectangleLinesEx(imageRec, 1, CheckCollisionPointRec(GetMousePosition(), imageRec) ? RED : DARKGRAY); 
                DrawText(TextFormat("SCALE: %.2f%%", imageScale*100.0f), 20, screenHeight - 40, 20, GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)));
            }
            else
            {
                DrawText("DRAG & DROP YOUR IMAGE!", 350, 200, 10, DARKGRAY);
                GuiDisable();
            }
            
            if (GuiButton((Rectangle){ screenWidth - 170, screenHeight - 50, 150, 30 }, "Image Export")) windowBoxActive = true;
            GuiEnable();
            
            // Draw window box: windowBoxName
            //-----------------------------------------------------------------------------
            if (windowBoxActive)
            {
                DrawRectangle(0, 0, screenWidth, screenHeight, Fade(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), 0.7f));
                windowBoxActive = !GuiWindowBox((Rectangle){ windowBoxRec.x, windowBoxRec.y, 220, 190 }, "Image Export Options");
            
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 35, 60, 25 }, "File format:");
                fileFormatActive = GuiComboBox((Rectangle){ windowBoxRec.x + 80, windowBoxRec.y + 35, 130, 25 }, TextJoin(fileFormatTextList, 3, ";"), fileFormatActive); 
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 70, 63, 25 }, "Pixel format:");
                pixelFormatActive = GuiComboBox((Rectangle){ windowBoxRec.x + 80, windowBoxRec.y + 70, 130, 25 }, TextJoin(pixelFormatTextList, 7, ";"), pixelFormatActive); 
                GuiLabel((Rectangle){ windowBoxRec.x + 10, windowBoxRec.y + 105, 50, 25 }, "File name:");
                if (GuiTextBox((Rectangle){ windowBoxRec.x + 80, windowBoxRec.y + 105, 130, 25 }, fileName, 64, textBoxEditMode)) textBoxEditMode = !textBoxEditMode;

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
