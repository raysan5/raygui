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
#include "../../src/raygui.h"

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again

#define GUI_TEXTBOX_EXTENDED_IMPLEMENTATION
#include "../../src/gui_textbox_extended.h"

#include <limits.h>

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define SIZEOF(A) (sizeof(A)/sizeof(A[0]))  // Get number of elements in array `A`. Total size of `A` should be known at compile time.

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static char text01[92] = "Lorem ipsum dolor sit amet, \xE7\x8C\xBF\xE3\x82\x82\xE6\x9C\xA8\xE3\x81\x8B\xE3\x82\x89\xE8\x90\xBD\xE3\x81\xA1\xE3\x82\x8B consectetur adipiscing elit"; // including some hiragana/kanji
static char text02[128] = "Here's another, much bigger textboxBounds." "\xf4\xa1\xa1\xff" " TIP: try COPY/PASTE ;)"; // including some invalid UTF8 

static int textboxActive = 0; // Keeps traks of the active textboxBounds

static Rectangle textboxBounds[4] = {
    { 20, 60, 160, 25 }, 
    { 220, 60, 300, 25 },
    { 565, 60, 95, 25 }, 
    { 680, 60, 100, 25 },
};

static Texture2D checked = { 0 };

// Draw a color button, if clicked return true
bool ColorButton(Rectangle bounds, Color color)
{
    Rectangle body = {bounds.x + 2, bounds.y + 2, bounds.width - 4, bounds.height - 4};
    bool pressed = false;
    
    // Update control
    Vector2 mouse = GetMousePosition();
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, bounds)) pressed = true;

    // Draw control
    DrawTexture(checked, body.x, body.y, WHITE);
    DrawRectangleRec(body, color);
    DrawRectangleLinesEx(bounds, 1, BLACK);
    
    return pressed;
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

    InitWindow(screenWidth, screenHeight, "raygui - gui textboxBounds extended demo");

    // Generate a checked checked used by the color buttons
	Image img = GenImageChecked(26, 26, 5, 5, RAYWHITE, DARKGRAY);
	checked = LoadTextureFromImage(img);
	UnloadImage(img);
    
    Font font = { 0 };
    int fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    int fontSpacing = GuiGetStyle(DEFAULT, TEXT_SPACING);
    int padding = GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING);
    int border = GuiGetStyle(TEXTBOX, BORDER_WIDTH);
    Color colorFG = GetColor(GuiGetStyle(TEXTBOX, COLOR_SELECTED_FG)); 
    Color colorBG = GetColor(GuiGetStyle(TEXTBOX, COLOR_SELECTED_BG));
    Color *colorSelected = &colorFG;

    bool showMenu = false;
    Rectangle menuRect = { 0 };
    
    int spinnerValue = 0;
    int valueBoxValue = 0;
    
    bool textBox01EditMode = false;
    bool textBox02EditMode = false;
    bool spinnerEditMode = false;
    bool valueBoxEditMode = false;
    
    SetTargetFPS(60);
    //---------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        Vector2 mouse = GetMousePosition();
        
        // Check all textboxes to get the active textboxBounds
        if (textBox01EditMode) textboxActive = 0; 
        else if (textBox02EditMode) textboxActive = 1; 
        else if (spinnerEditMode) textboxActive = 2; 
        else if (valueBoxEditMode) textboxActive = 3; 
    
        // Show/Hide the textboxBounds menu
        /*
        if ((textboxActive < 2) && !showMenu && CheckCollisionPointRec(mouse, textboxBounds[textboxActive]) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) 
        {
            showMenu = true;
            menuRect = (Rectangle){mouse.x, mouse.y, 80, 110};
        }
        */
    
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
                    GuiSetFont(fnt);
                    fontSize = fnt.baseSize;

                    // Remove old font
                    if (font.texture.id != 0) UnloadFont(font);
                    font = fnt;
                }
            }
            
            ClearDroppedFiles();
        }
        
        // Convert text to hex representation and draw it on screen
        char hex[(((textboxActive == 1) ? SIZEOF(text02) : SIZEOF(text01)) + 1)*3 + 1];
        char *text = (textboxActive == 1) ? text02 : text01;
        int maxSize = (textboxActive == 1) ? SIZEOF(text02) : SIZEOF(text01);
        for (int i = 0, j = 0; i < maxSize; ++i, j += 3) sprintf(&hex[j], "%02Xh ", (char)text[i]);

        int startIdx = 0, endIdx = 0;
        if (textboxActive < 2)
        {
            Vector2 sel = GuiTextBoxGetSelection();
            startIdx = GuiTextBoxGetByteIndex(text, 0, 0, sel.x);
            endIdx = GuiTextBoxGetByteIndex(text, 0, 0, sel.x + sel.y);
        }
        
        int len = endIdx - startIdx;
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
		BeginDrawing();
		
            ClearBackground(RAYWHITE);
            
            // Draw textboxes extended
            //---------------------------------------------------------------------------------------
            // Set custom style
            GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
            GuiSetStyle(DEFAULT, TEXT_SPACING, fontSpacing);
            GuiSetStyle(TEXTBOX, TEXT_INNER_PADDING, padding);
            GuiSetStyle(TEXTBOX, BORDER_WIDTH, border);
            GuiSetStyle(TEXTBOX, COLOR_SELECTED_BG, ColorToInt(colorBG));
            GuiSetStyle(TEXTBOX, COLOR_SELECTED_FG, ColorToInt(colorFG));

            // TODO: GuiTextBoxEx() is not following editMode logic like GuiTextBox()
            if (GuiTextBoxEx(textboxBounds[0], text01, SIZEOF(text01) - 1, textBox01EditMode)) textBox01EditMode = !textBox01EditMode;
            if (GuiTextBoxEx(textboxBounds[1], text02, SIZEOF(text02) - 1, textBox02EditMode)) textBox02EditMode = !textBox02EditMode;
            
            if (GuiSpinner(textboxBounds[2], NULL, &spinnerValue, INT_MIN, INT_MAX, spinnerEditMode)) spinnerEditMode = !spinnerEditMode;
            if (GuiValueBox(textboxBounds[3], NULL, &valueBoxValue, INT_MIN, INT_MAX, valueBoxEditMode)) valueBoxEditMode = !valueBoxEditMode;
            //---------------------------------------------------------------------------------------
            
            // Reset style to default
            GuiLoadStyleDefault();
            GuiSetFont(GetFontDefault());
            GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    
            // Draw hexadecimal values viewer
            DrawTextRecEx(guiFont, hex, (Rectangle){ 20, 95, 760, 205 }, 20, 1, true, BLACK, startIdx*3, len*3, colorFG, colorBG);

            // Draw right-mouse-button-click menu and logic
            //---------------------------------------------------------------------------------------
            if (showMenu)
            {
                GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 24); // make items look a little bigger
                const char *menuItems[] = { "#17# Cut", "#16# Copy", "#18# Paste", "#101# SelectAll" };
                //int enabledItems[] = { textboxActive < 2 ? 1 : 0, textboxActive < 2 ? 1 : 0, GetClipboardText() != NULL, 1 };
                int active = -1, focus = 0, scroll = 0;
                
                GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT); // Fixes visual glitch with other alignments
                active = GuiListViewEx(menuRect, menuItems, SIZEOF(menuItems), &focus, &scroll, active);
                
                if (active != -1)
                {
                    showMenu = false;
                    char *text = (textboxActive == 1) ? text02 : text01;
                    switch(active)
                    {
                        case 0: GuiTextBoxCut(text); break;
                        case 1: GuiTextBoxCopy(text); break;
                        case 2: GuiTextBoxPaste(text, (textboxActive == 1) ? SIZEOF(text02) : SIZEOF(text01)); break;
                        case 3: GuiTextBoxSelectAll(text); break;
                        default: break;
                    }
                }
            }
            //---------------------------------------------------------------------------------------
    
            // Draw GUI to change textboxes parameters
            //---------------------------------------------------------------------------------------
            /*
            // UI for changing the font size and spacing of all textboxes
            GuiLine((Rectangle){25,280,750,10}, NULL);
            GuiGroupBox((Rectangle){20,320,190,100},  GuiIconText(RICON_GEAR, "FONT"));
            GuiLabel((Rectangle){30,340,60,20}, "Size");
            GuiSpinner((Rectangle){95,340,100,20}, NULL, &fontSize, 10, 40, true);
            GuiLabel((Rectangle){30,380,60,20}, "Spacing");
            GuiSpinner((Rectangle){95,380,100,20}, NULL, &fontSpacing, 1, 10, true);
            
            // UI for changing the style of all textboxes
            GuiGroupBox((Rectangle){225,320,190,100}, GuiIconText(RICON_COLOR_BUCKET, "STYLE"));
            GuiLabel((Rectangle){240,340,60,20}, "Padding");
            GuiSpinner((Rectangle){305,340,100,20}, NULL, &padding, 2, 30, true);
            GuiLabel((Rectangle){240,380,60,20}, "Border");
            GuiSpinner((Rectangle){305,380,100,20}, NULL, &border, 0, 8, true);

            // UI for changing the width/height of the active textboxBounds
            bool changed = false;
            int width = textboxBounds[textboxActive].width, height = textboxBounds[textboxActive].height;
            GuiGroupBox((Rectangle){430,320,175,100}, GuiIconText(RICON_CURSOR_SCALE, "SCALE"));
            GuiLabel((Rectangle){435,340,55,20}, "Width");
            if (GuiSpinner((Rectangle){495,340,100,20}, NULL, &width, 30, 300, true)) changed = true;
            GuiLabel((Rectangle){435,380,55,20}, "Height");
            if (GuiSpinner((Rectangle){495,380,100,20}, NULL, &height, 12, 60, true)) changed = true;
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
            GuiLabel((Rectangle){30,290,740,10}, GuiIconText(RICON_TEXT_T, " DRAG A FONT FILE (*.TTF, *.FNT) ANYWHERE TO CHANGE THE DEFAULT FONT!"));
            GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
            textboxBounds[textboxActive].y = 85 - height;
            textboxBounds[textboxActive].height = height;
            textboxBounds[textboxActive].width = width;
            if (changed) GuiTextBoxSetActive(textboxBounds[textboxActive]);
            
            // UI for selecting the selected text background and foreground color
            if (ColorButton((Rectangle){625,320,30,30}, colorFG)) colorSelected = &colorFG;
            if (ColorButton((Rectangle){625,389,30,30}, colorBG)) colorSelected = &colorBG;
            *colorSelected = GuiColorPicker((Rectangle){660,320,90,85}, *colorSelected);
            
            float alpha = colorSelected->a;
            GuiSetStyle(SLIDER, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT); // Slider for the selected color alpha value
            colorSelected->a = GuiSlider((Rectangle){664,420,100,20}, GuiIconText(RICON_CROP_ALPHA, "Alpha"), NULL, alpha, 0.0f, 255.0f);
            */
            //---------------------------------------------------------------------------------------
                
		EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}