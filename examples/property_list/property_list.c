/*******************************************************************************************
*
*   raygui - a custom property list control
*
*   DEPENDENCIES:
*       raylib 3.0
*       raygui 2.7
*
*   COMPILATION (Windows - MinGW):
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -I../../src -lraylib -lopengl32 -lgdi32 -std=c99
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2020 Vlad Adrian (@Demizdor - https://github.com/Demizdor)
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "../../src/raygui.h"

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again

#define GUI_PROPERTY_LIST_IMPLEMENTATION
#include "dm_property_list.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define SIZEOF(A) (sizeof(A)/sizeof(A[0]))

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------

    GuiDMProperty prop[] = {
        PBOOL("Bool", 0, true),
        PSECTION("#102#SECTION", 0, 2),
        PINT("Int", 0, 123),
        PFLOAT("Float", 0, 0.99f),
        PTEXT("Text", 0, (char*)&(char[30]){"Hello!"}, 30),
        PSELECT("Select", 0, "ONE;TWO;THREE;FOUR", 0),
        PINT_RANGE("Int Range", 0, 32, 1, 0, 100),
        PRECT("Rect", 0, 0, 0, 100, 200),
        PVEC2("Vec2", 0, 20, 20),
        PVEC3("Vec3", 0, 12, 13, 14),
        PVEC4("Vec4", 0, 12, 13, 14, 15),
        PCOLOR("Color", 0, 0, 255, 0, 255),
    };
    int focus = 0, scroll = 0; // needed by GuiDMPropertyList()
    
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "raygui - property list");
    SetTargetFPS(60);
    
    GuiLoadStyleDefault();
    // adjust the default raygui style a bit
    GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 24);
    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 12);
    //--------------------------------------------------------------------------------------
    
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));
        
            GuiGrid((Rectangle){0,0,SCREEN_WIDTH,SCREEN_HEIGHT},20.0f, 2); // draw a fancy grid
            
            GuiDMPropertyList((Rectangle){(SCREEN_WIDTH - 180)/2, (SCREEN_HEIGHT - 280)/2, 180, 280}, prop, SIZEOF(prop), &focus, &scroll);
        
            if (prop[0].value.vbool >= 1)
            {
                DrawText(TextFormat("FOCUS:%i | SCROLL:%i | FPS:%i", focus, scroll, GetFPS()), prop[8].value.v2.x, prop[8].value.v2.y, 20, prop[11].value.vcolor);
            }
            
		EndDrawing();
		//----------------------------------------------------------------------------------
    }
    
    GuiDMSaveProperties("test.props", prop, SIZEOF(prop));  // Save properties to `test.props` file at exit
    CloseWindow();              // Close window and OpenGL context
    
    return 0;
}

