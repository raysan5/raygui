/*******************************************************************************************
*
*   raygui - Controls test
*
*   TEST CONTROLS:
*       - GuiScrollPanel()
*
*   DEPENDENCIES:
*       raylib 2.4  - Windowing/input management and drawing.
*       raygui 2.0  - Immediate-mode GUI controls.
*
*   COMPILATION (Windows - MinGW):
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -I../../src -lraylib -lopengl32 -lgdi32 -std=c99
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2019 Vlad Adrian (@Demizdor) and Ramon Santamaria (@raysan5)
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../src/raygui.h"

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    const int screenWidth = 400;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raygui - GuiScrollPanel()");
    
    Rectangle panelRec = { 20,40,200,150 };
    Rectangle panelContentRec = { 0,0,340,340 };
    Vector2 panelScroll = { 99, -20 };
    
    SetTargetFPS(60);
    //---------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        // TODO: Implement required update logic
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
		BeginDrawing();
		
		ClearBackground(RAYWHITE);
        
            DrawText(TextFormat("[%f, %f]", panelScroll.x, panelScroll.y), 4, 4, 20, RED);

            Rectangle view = GuiScrollPanel(panelRec, panelContentRec, &panelScroll);

            BeginScissorMode(view.x, view.y, view.width, view.height);
                GuiGrid((Rectangle){panelRec.x+panelScroll.x, panelRec.y+panelScroll.y, panelContentRec.width, panelContentRec.height}, 16, 3);
            EndScissorMode();
            
            DrawRectangle(panelRec.x+panelScroll.x, panelRec.y+panelScroll.y, panelContentRec.width, panelContentRec.height, Fade(RED, 0.1));
                
		EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}