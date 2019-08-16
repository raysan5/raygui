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
*   COMPILATION (Linux - gcc):
*	gcc -o $(NAME_PART) $(FILE_NAME) -I../../src -lraylib -std=c99
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2019 Vlad Adrian (@Demizdor) and Ramon Santamaria (@raysan5)
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../../src/raygui.h"


static void DrawStyleEditControls(void);  // Draw and process scroll bar style edition controls

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raygui - GuiScrollPanel()");
    
    Rectangle panelRec = { 20, 40, 200, 150 };
    Rectangle panelContentRec = {0, 0, 340, 340 };
    Vector2 panelScroll = { 99, -20 };
    
    bool showContentArea = true;
    
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
                GuiGrid((Rectangle){panelRec.x + panelScroll.x, panelRec.y + panelScroll.y, panelContentRec.width, panelContentRec.height}, 16, 3);
            EndScissorMode();
            
            if (showContentArea) DrawRectangle(panelRec.x + panelScroll.x, panelRec.y + panelScroll.y, panelContentRec.width, panelContentRec.height, Fade(RED, 0.1));
            
            DrawStyleEditControls();
            
            showContentArea = GuiCheckBox((Rectangle){ 565, 80, 20, 20 }, "SHOW CONTENT AREA", showContentArea);
                
            panelContentRec.width = GuiSliderBar((Rectangle){ 590, 385, 145, 15}, "WIDTH", panelContentRec.width, 1, 600, true);
            panelContentRec.height = GuiSliderBar((Rectangle){ 590, 410, 145, 15 }, "HEIGHT", panelContentRec.height, 1, 400, true);
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

// Draw and process scroll bar style edition controls
static void DrawStyleEditControls(void)
{
    // ScrollPanel style controls
    //----------------------------------------------------------
    GuiGroupBox((Rectangle){ 550, 170, 220, 205 }, "SCROLLBAR STYLE");
    
    int style = GuiGetStyle(SCROLLBAR, BORDER_WIDTH);
    GuiLabel((Rectangle){ 555, 195, 110, 10 }, "BORDER_WIDTH");
    GuiSpinner((Rectangle){ 670, 190, 90, 20 }, &style, 0, 6, false);
    GuiSetStyle(SCROLLBAR, BORDER_WIDTH, style);
    
    style = GuiGetStyle(SCROLLBAR, ARROWS_SIZE);
    GuiLabel((Rectangle){ 555, 220, 110, 10 }, "ARROWS_SIZE");
    GuiSpinner((Rectangle){ 670, 215, 90, 20 }, &style, 4, 14, false);
    GuiSetStyle(SCROLLBAR, ARROWS_SIZE, style);
    
    style = GuiGetStyle(SCROLLBAR, INNER_PADDING);
    GuiLabel((Rectangle){ 555, 245, 110, 10 }, "INNER_PADDING");
    GuiSpinner((Rectangle){ 670, 240, 90, 20 }, &style, 0, 14, false);
    GuiSetStyle(SCROLLBAR, INNER_PADDING, style);
    
    style = GuiCheckBox((Rectangle){ 565, 280, 20, 20 }, "ARROWS_VISIBLE", GuiGetStyle(SCROLLBAR, ARROWS_VISIBLE));
    GuiSetStyle(SCROLLBAR, ARROWS_VISIBLE, style);
    
    style = GuiGetStyle(SCROLLBAR, SLIDER_PADDING);
    GuiLabel((Rectangle){ 555, 325, 110, 10 }, "SLIDER_PADDING");
    GuiSpinner((Rectangle){ 670, 320, 90, 20 }, &style, 0, 14, false);
    GuiSetStyle(SCROLLBAR, SLIDER_PADDING, style);
    
    style = GuiGetStyle(SCROLLBAR, SLIDER_SIZE);
    GuiLabel((Rectangle){ 555, 350, 110, 10 }, "SLIDER_SIZE");
    GuiSpinner((Rectangle){ 670, 345, 90, 20 }, &style, 2, 100, false);
    GuiSetStyle(SCROLLBAR, SLIDER_SIZE, style);
    
    const char *text = GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE? "SCROLLBAR: LEFT" : "SCROLLBAR: RIGHT";
    style = GuiToggle((Rectangle){ 560, 110, 200, 35 }, text, GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE));
    GuiSetStyle(LISTVIEW, SCROLLBAR_SIDE, style);
    //----------------------------------------------------------

    // ScrollBar style controls
    //----------------------------------------------------------
    GuiGroupBox((Rectangle){ 550, 20, 220, 135 }, "SCROLLPANEL STYLE");
    
    style = GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);
    GuiLabel((Rectangle){ 555, 35, 110, 10 }, "SCROLLBAR_WIDTH");
    GuiSpinner((Rectangle){ 670, 30, 90, 20 }, &style, 6, 30, false);
    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, style);
    
    style = GuiGetStyle(DEFAULT, BORDER_WIDTH);
    GuiLabel((Rectangle){ 555, 60, 110, 10 }, "BORDER_WIDTH");
    GuiSpinner((Rectangle){ 670, 55, 90, 20 }, &style, 0, 20, false);
    GuiSetStyle(DEFAULT, BORDER_WIDTH, style);
    //----------------------------------------------------------
}