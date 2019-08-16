/*******************************************************************************************
*
*   raygui - Controls test
*
*   TEST CONTROLS:
*       - GuiScrollPanel()
*
*   DEPENDENCIES:
*       raylib 2.5  - Windowing/input management and drawing.
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
#define RAYGUI_SUPPORT_RICONS
#define RAYGUI_TEXTBOX_EXTENDED
#include "../../src/raygui.h"

#include <stdio.h>
#include <limits.h>

// -----------------
// DEFINES
// -----------------
#define SIZEOF(A) (sizeof(A)/sizeof(A[0]))  // Get number of elements in array `A`. Total size of `A` should be known at compile time.
#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 450
#define TEXTBOX_MAX_HEIGHT 55

// -----------------
// GLOBAL VARIABLES
// -----------------
char text0[92] = "Lorem ipsum dolor sit amet, \xE7\x8C\xBF\xE3\x82\x82\xE6\x9C\xA8\xE3\x81\x8B\xE3\x82\x89\xE8\x90\xBD\xE3\x81\xA1\xE3\x82\x8B consectetur adipiscing elit"; // including some hiragana/kanji
char text1[128] = "Here's another, much bigger textbox." "\xf4\xa1\xa1\xff" " TIP: try COPY/PASTE ;)"; // including some invalid UTF8 
int spinnerValue = 0, boxValue = 0;
int textboxActive = 0; // Keeps traks of the active textbox

struct {
    Rectangle bounds;
    int maxWidth;
} textbox[] = {     // Params for each of the 4 textboxes
    { .bounds = (Rectangle){20,60,160,25}, .maxWidth = 160 },
    { .bounds = (Rectangle){220,60,300,25}, .maxWidth = 300 },
    { .bounds = (Rectangle){565,60,95,25}, .maxWidth = 95 },
    { .bounds = (Rectangle){680,60,100,25}, .maxWidth = 100 }
};

int fontSize = 10, fontSpacing = 1, padding = 0, border = 0;
Font font = {0};
Color colorBG = {0}, colorFG = {0}, *colorSelected = NULL;

bool showMenu = false;
Rectangle menuRect = {0};
Texture2D pattern = {0};

// -----------------
// FUNCTIONS
// -----------------

// Draw a color button, if clicked return true.
bool ColorButton(Rectangle bounds, Color color)
{
    Rectangle body = {bounds.x + 2, bounds.y + 2, bounds.width - 4, bounds.height - 4};
    bool pressed = false;
    
    // Update control
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, bounds)) pressed = true;

    // Draw control
    DrawTexture(pattern, body.x, body.y, WHITE);
    DrawRectangleRec(body, color);
    DrawRectangleLinesEx(bounds, 1, BLACK);
    
    return pressed;
}

// Handles GUI logic
void UpdateGUI()
{
    // Check all of the 4 textboxes to get the active textbox
    for (int i = 0; i < SIZEOF(textbox); i++)
    {
        if (GuiTextBoxIsActive(textbox[i].bounds))
        {
            textboxActive = i;
            break;
        }
    }
    
    // Show/Hide the textbox menu
    Vector2 mouse = GetMousePosition();
    if ((textboxActive < 2) && !showMenu && CheckCollisionPointRec(mouse, textbox[textboxActive].bounds) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) 
    {
        showMenu = true;
        menuRect = (Rectangle){mouse.x, mouse.y, 80, 110};
    }
    
    // Menu hidding logic
    if (showMenu && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mouse, menuRect)) showMenu = false;
    
    // Fonts drag & drop logic
    if (IsFileDropped()) 
    {
        int count = 0;
        char **files = GetDroppedFiles(&count);
        
        if (IsFileExtension(files[0], ".ttf") || 
            IsFileExtension(files[0], ".otf") || 
            IsFileExtension(files[0], ".fnt"))
        {
            Font fnt = LoadFont(files[0]);
            
            if (fnt.texture.id != 0)
            {
                // Font was loaded, only change font on success
                GuiFont(fnt);
                fontSize = fnt.baseSize;

                // Remove old font
                if (font.texture.id != 0) UnloadFont(font);
                font = fnt;
            }
        }
        
        ClearDroppedFiles();
    }
}

// Handles GUI drawing
void DrawGUI()
{
    // DRAW TEXTBOXES TO TEST
    // Set custom style for textboxes
    if (font.texture.id != 0) GuiFont(font); // Use our custom font if valid
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    GuiSetStyle(DEFAULT, TEXT_SPACING, fontSpacing);
    GuiSetStyle(TEXTBOX, INNER_PADDING, padding);
    GuiSetStyle(TEXTBOX, BORDER_WIDTH, border);
    GuiSetStyle(TEXTBOX, COLOR_SELECTED_BG, ColorToInt(colorBG));
    GuiSetStyle(TEXTBOX, COLOR_SELECTED_FG, ColorToInt(colorFG));

    // Draw textboxes
    GuiTextBox(textbox[0].bounds, text0, SIZEOF(text0) - 1, true);
    GuiTextBox(textbox[1].bounds, text1, SIZEOF(text1) - 1, true);
    GuiSpinner(textbox[2].bounds, &spinnerValue, INT_MIN, INT_MAX, true);
    GuiValueBox(textbox[3].bounds, &boxValue, INT_MIN, INT_MAX, true);
    
    // RESET STYLE TO DEFAULT
    GuiLoadStyleDefault();
    GuiFont(GetFontDefault());
    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    
    // DRAW HEX VIEW
    // Convert text to hex representation and draw it on screen
    char hex[(((textboxActive == 1) ? SIZEOF(text1) : SIZEOF(text0)) +1)*3 + 1];
    char* text = (textboxActive == 1) ? text1 : text0;
    int maxSize = (textboxActive == 1) ? SIZEOF(text1) : SIZEOF(text0);
    for (int i=0, j=0; i < maxSize; ++i, j+=3) 
    {
        sprintf(&hex[j], "%02Xh ", (char)text[i]);
    }
    
    int startIdx = 0, endIdx = 0;
    if (textboxActive < 2)
    {
        Vector2 sel = GuiTextBoxGetSelection();
        startIdx = GuiTextBoxGetByteIndex(text, 0, 0, sel.x);
        endIdx = GuiTextBoxGetByteIndex(text, 0, 0, sel.x + sel.y);
    }
    int len = endIdx - startIdx;
    DrawTextRecEx(guiFont, hex, (Rectangle){20,95,760,205}, 20, 1, true, BLACK, startIdx*3, len*3, colorFG, colorBG);
    
    // DISPLAY A MENU ON RIGHT CLICKING A TEXTBOX
     // draw the menu and handle clicked item
    if (showMenu)
    {
        GuiSetStyle(LISTVIEW, ELEMENTS_HEIGHT, 24); // make items look a little bigger
        const char *menuItems[] = { "#17# Cut", "#16# Copy", "#18# Paste", "#101# SelectAll" };
        int enabledItems[] = { textboxActive < 2 ? 1 : 0, textboxActive < 2 ? 1 : 0, GetClipboardText() != NULL, 1 };
        int active = -1, focus = 0, scroll = 0;
        
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT); // Fixes visual glitch with other alignments
        active = GuiListViewEx(menuRect, menuItems, SIZEOF(menuItems), &focus, &scroll, active);
        
        if (active != -1)
        {
            showMenu = false;
            char *text = (textboxActive == 1) ? text1 : text0;
            switch(active)
            {
                case 0: GuiTextBoxCut(text); break;
                case 1: GuiTextBoxCopy(text); break;
                case 2: GuiTextBoxPaste(text, (textboxActive == 1) ? SIZEOF(text1) : SIZEOF(text0)); break;
                case 3: GuiTextBoxSelectAll(text); break;
                default: break;
            }
        }
    }
    
    // DRAW THE GUI FOR CHANGING THE TEXTBOXES
    // UI for changing the font size and spacing of all textboxes
    GuiLine((Rectangle){25,280,750,10}, NULL);
    GuiGroupBox((Rectangle){20,320,190,100},  GuiIconText(RICON_GEAR, "FONT"));
    GuiLabel((Rectangle){30,340,60,20}, "Size");
    GuiSpinner((Rectangle){95,340,100,20},&fontSize, 10, 40, true);
    GuiLabel((Rectangle){30,380,60,20}, "Spacing");
    GuiSpinner((Rectangle){95,380,100,20},&fontSpacing, 1, 10, true);
    
    // UI for changing the style of all textboxes
    GuiGroupBox((Rectangle){225,320,190,100}, GuiIconText(RICON_COLOR_BUCKET, "STYLE"));
    GuiLabel((Rectangle){240,340,60,20}, "Padding");
    GuiSpinner((Rectangle){305,340,100,20},&padding, 2, 30, true);
    GuiLabel((Rectangle){240,380,60,20}, "Border");
    GuiSpinner((Rectangle){305,380,100,20},&border, 0, 8, true);

    // UI for changing the width/height of the active textbox
    bool changed = false;
    int width = textbox[textboxActive].bounds.width, height = textbox[textboxActive].bounds.height;
    GuiGroupBox((Rectangle){430,320,175,100}, GuiIconText(RICON_CURSOR_SCALE, "SCALE"));
    GuiLabel((Rectangle){435,340,55,20}, "Width");
    if (GuiSpinner((Rectangle){495,340,100,20}, &width, 30, textbox[textboxActive].maxWidth, true)) changed = true;
    GuiLabel((Rectangle){435,380,55,20}, "Height");
    if (GuiSpinner((Rectangle){495,380,100,20}, &height, 12, TEXTBOX_MAX_HEIGHT, true)) changed = true;
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    GuiLabel((Rectangle){30,290,740,10}, GuiIconText(RICON_TEXT_T, " DRAG A FONT FILE (*.TTF, *.FNT) ANYWHERE TO CHANGE THE DEFAULT FONT!"));
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
    textbox[textboxActive].bounds.y = 85 - height;
    textbox[textboxActive].bounds.height = height;
    textbox[textboxActive].bounds.width = width;
    if (changed) GuiTextBoxSetActive(textbox[textboxActive].bounds);
    
    // UI for selecting the selected text background and foreground color
    if (ColorButton((Rectangle){625,320,30,30}, colorFG)) colorSelected = &colorFG;
    if (ColorButton((Rectangle){625,389,30,30}, colorBG)) colorSelected = &colorBG;
    *colorSelected = GuiColorPicker((Rectangle){660,320,90,85}, *colorSelected);
    float alpha = colorSelected->a;
    GuiSetStyle(SLIDER, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT); // Slider for the selected color alpha value
    colorSelected->a = GuiSlider((Rectangle){664,420,100,20}, GuiIconText(RICON_CROP_ALPHA, "Alpha"), alpha, 0.f, 255.f, true);
}

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, char **argv)
{
    // Initialization
    //---------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 450;

    InitWindow(screenWidth, screenHeight, "raygui - GuiTextBoxEx()");

    // Generate a checked pattern used by the color buttons
	Image timg = GenImageChecked(26, 26, 5, 5, RAYWHITE, DARKGRAY);
	pattern = LoadTextureFromImage(timg);
	UnloadImage(timg);
    
    // Load initial style
	GuiLoadStyleDefault();
    //font = LoadFont("resources/notoCJK.fnt");
    //GuiFont(font);
    
    fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    fontSpacing = GuiGetStyle(DEFAULT, TEXT_SPACING);
    padding = GuiGetStyle(TEXTBOX, INNER_PADDING);
    border = GuiGetStyle(TEXTBOX, BORDER_WIDTH);
    colorFG = GetColor(GuiGetStyle(TEXTBOX, COLOR_SELECTED_FG)); 
    colorBG = GetColor(GuiGetStyle(TEXTBOX, COLOR_SELECTED_BG));
    colorSelected = &colorFG;
    
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
            
            UpdateGUI();
            DrawGUI();
		
		EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}