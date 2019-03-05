#include <stdio.h>
#include <limits.h>

#include <raylib.h>

#define RAYGUI_RICONS_SUPPORT
#define RAYGUI_IMPLEMENTATION
#include "../src/raygui.h"

#define SIZEOF(A) (sizeof(A)/sizeof(A[0]))


// CONSTANTS
const int screenWidth = 800;
const int screenHeight = 450;
const int widgetsMaxHeight = 55;
const Rectangle selectionArea[] = {
    (Rectangle){0,-10,200,100},
    (Rectangle){210,-10,345,100},
    (Rectangle){565,-10,105,100},
    (Rectangle){675,-10,105,100}
};
const Rectangle selectionLines[] = {
    (Rectangle){20,85,35,10},
    (Rectangle){220,85,35,10},
    (Rectangle){565,85,35,10},
    (Rectangle){680,85,35,10}
};


// FORWARD DECLARE
Texture2D pattern;
bool ColorButton(Rectangle bounds, Color color);


// VARIABLES
int selected = 0;
int widgetsMaxWidth[] = { 160, 300, 95, 100 };
Rectangle widgetRect[] = {
    (Rectangle){20,60,160,25},
    (Rectangle){220,60,300,25},
    (Rectangle){565,60,95,25},
    (Rectangle){680,60,100,25}
};

char text0[64] = "Lorem ipsum dolor sit amet, consectetur adipiscing elit";
int text0Cursor = -1, text0Selection = -1, text0TextStart = 0;

char text1[128] = "Here's another, much bigger textbox. TIP: try COPY/PASTE ;)";
int text1Cursor = -1, text1Selection = -1, text1TextStart = 0;

int spinnerValue = 1, spinnerCursor = -1, spinnerSelection = 0, spinnerTextStart = 0;
int boxValue = 100, boxCursor=-1, boxSelection = 0, boxTextStart = 0;

// font
int fontSize = 10, fontSpacing = 1, padding = 0, border = 0;
Color BG = {0}, FG = {0}, *clr = NULL;
Font font = {0};

// menu
bool showMenu = false;
Rectangle menuRect = {0};


// FUNCTIONS

void DrawGui() 
{
    Vector2 mouse = GetMousePosition();
    
    // SELECT ACTIVE TEXTBOX
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && mouse.y <= 100)
    {
        if (CheckCollisionPointRec(mouse, selectionArea[0])) selected = 0;
        else if(CheckCollisionPointRec(mouse, selectionArea[1])) selected = 1;
        else if(CheckCollisionPointRec(mouse, selectionArea[2])) selected = 2;
        else if(CheckCollisionPointRec(mouse, selectionArea[3])) selected = 3;
        else selected = 0;
    }
    
    // Draw a line under the selected widget
    Rectangle line = selectionLines[selected];
    const int lineclr = GuiGetStyle(DEFAULT, LINE_COLOR);
    GuiSetStyle(DEFAULT, LINE_COLOR, ColorToInt(RED));
    GuiLine(line, NULL);
    GuiSetStyle(DEFAULT, LINE_COLOR, lineclr);
    
    
    // SHOW STATUS
    int* cursor = &text0Cursor, *selection = &text0Selection;
    if(selected == 1)
    {
        cursor = &text1Cursor; 
        selection = &text1Selection;
    }
    DrawText(TextFormat("widget:[%i] cursor:[%i] selection: [%i]", selected, *cursor, *selection), 4, 4, 20, RED);
    
    
    // TEXBOXES
    // Set custom style for textboxes
    if(font.texture.id != 0) GuiFont(font);
    GuiSetStyle(DEFAULT, TEXT_SIZE, fontSize);
    GuiSetStyle(DEFAULT, TEXT_SPACING, fontSpacing);
    GuiSetStyle(TEXTBOX, INNER_PADDING, padding);
    GuiSetStyle(TEXTBOX, BORDER_WIDTH, border);
    GuiSetStyle(TEXTBOX, COLOR_SELECTED_BG, ColorToInt(BG));
    GuiSetStyle(TEXTBOX, COLOR_SELECTED_FG, ColorToInt(FG));
    // Draw textboxes
    GuiTextBox(widgetRect[0], text0, SIZEOF(text0) - 1, &text0Cursor, &text0Selection, &text0TextStart, selected == 0 ? true: false);
    GuiTextBox(widgetRect[1], text1, SIZEOF(text1) - 1, &text1Cursor, &text1Selection, &text1TextStart, selected == 1 ? true: false);
    GuiSpinner(widgetRect[2], &spinnerValue, INT_MIN, INT_MAX, &spinnerCursor, &spinnerSelection, &spinnerTextStart, selected == 2 ? true: false);
    GuiValueBox(widgetRect[3], &boxValue, INT_MIN, INT_MAX, &boxCursor, &boxSelection, &boxTextStart, selected == 3 ? true: false);
    // Convert text to hex representation and draw it on screen
    char hex[(SIZEOF(text1)+1)*3 + 1];
    char* text = selected == 1 ? text1 : text0;
    int maxSize = selected == 1 ? SIZEOF(text1) : SIZEOF(text0);
    for(int i=0, j=0; i<maxSize; ++i, j+=3) 
    {
        sprintf(&hex[j], "%02hhX ", (char)text[i]);
    }
    int len = *selection < 0 ? -*selection : *selection;
    int start = *selection < 0 ? *cursor + *selection : *cursor;
    DrawTextRecEx(guiFont, hex, (Rectangle){20,95,760,205}, 20, 1, true, BLACK, start > -1 ? start*3 : 0, len > -1 ? len*3 : 0, FG, BG);
    
    
    // RESET STYLE TO DEFAULT
    GuiLoadStyleDefault();
    GuiFont(GetFontDefault());
    
    
    // DISPLAY A MENU ON RIGHT CLICKING A TEXTBOX
    if(selected < 2 && !showMenu && CheckCollisionPointRec(mouse, widgetRect[selected]) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) 
    {
        showMenu = true;
        menuRect = (Rectangle){mouse.x, mouse.y, 80, 110};
    }
    // code for hiding the menu
    if(showMenu && IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !CheckCollisionPointRec(mouse, menuRect)) showMenu = false;
    // draw the menu and handle clicked item
    if(showMenu)
    {
        GuiSetStyle(LISTVIEW, ELEMENTS_HEIGHT, 24); // make items look a little bigger
        const char* menuItems[] = { "#17# Cut", "#16# Copy", "#18# Paste", "#101# SelectAll" };
        int enabledItems[] = { *selection != 0 ? 1 : 0, *selection != 0 ? 1 : 0, 1, 1 };
        int active=-1, focus=0, scroll=0;
        GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT); // Fixes visual glitch with other alignments
        GuiListViewEx(menuRect, menuItems, SIZEOF(menuItems), enabledItems, &active, &focus, &scroll, true);
        if(active != -1)
        {
            showMenu = false;
            // TraceLog(LOG_INFO, TextFormat("%s", menuItems[active]));
            char* text = text0; 
            int* textStart = &text0TextStart;
            if(selected == 1) 
            {
                text = text1;
                textStart = &text1TextStart;
            }
            
            switch(active)
            {
                case 0: GuiTextBoxCut(text, cursor, selection, textStart); break;
                case 1: GuiTextBoxCopy(text, cursor, selection); break;
                case 2: GuiTextBoxPaste(text, cursor, selection, textStart, maxSize); break;
                case 3: GuiTextBoxSelectAll(text, cursor, selection); break;
                default: break;
            }
        }
    }
    
    
    // STYLE UI
    // UI for changing the font size and spacing of all textboxes
    static int params0[6] = {0};
    GuiLine((Rectangle){25,280,750,10}, NULL);
    GuiGroupBox((Rectangle){20,320,190,100}, TextFormat("#%i#FONT", RICON_GEAR));
    GuiLabel((Rectangle){30,340,60,20}, "Size");
    GuiSpinner((Rectangle){95,340,100,20},&fontSize, 10, 40, &params0[0], &params0[1], &params0[2], CheckCollisionPointRec(mouse, (Rectangle){95,340,100,20}));
    GuiLabel((Rectangle){30,380,60,20}, "Spacing");
    GuiSpinner((Rectangle){95,380,100,20},&fontSpacing, 1, 10, &params0[3], &params0[4], &params0[5], CheckCollisionPointRec(mouse, (Rectangle){95,380,100,20}));
    
    // UI for changing the selected widget style
    static int params1[6] = {0};
    GuiGroupBox((Rectangle){225,320,190,100}, TextFormat("#%i#STYLE", RICON_COLOR_BUCKET));
    GuiLabel((Rectangle){240,340,60,20}, "Padding");
    GuiSpinner((Rectangle){305,340,100,20},&padding, 2, 30, &params1[0], &params1[1], &params1[2], CheckCollisionPointRec(mouse, (Rectangle){305,340,100,20}));
    GuiLabel((Rectangle){240,380,60,20}, "Border");
    GuiSpinner((Rectangle){305,380,100,20},&border, 0, 8, &params1[3], &params1[4], &params1[5], CheckCollisionPointRec(mouse, (Rectangle){305,380,100,20}));

    // UI for changing the selected widget width/height
    static int params2[6] = {0};
    int width = widgetRect[selected].width, height = widgetRect[selected].height;
    GuiGroupBox((Rectangle){430,320,175,100}, TextFormat("#%i#SCALE", RICON_CURSOR_SCALE));
    GuiLabel((Rectangle){435,340,55,20}, "Width");
    GuiSpinner((Rectangle){495,340,100,20}, &width, 30, widgetsMaxWidth[selected], &params2[0], &params2[1], &params2[2], CheckCollisionPointRec(mouse, (Rectangle){495,340,100,20}));
    GuiLabel((Rectangle){435,380,55,20}, "Height");
    GuiSpinner((Rectangle){495,380,100,20}, &height, 12, widgetsMaxHeight, &params1[3], &params1[4], &params1[5], CheckCollisionPointRec(mouse, (Rectangle){495,380,100,20}));
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    GuiLabel((Rectangle){30,290,740,10}, TextFormat("#%i# DRAG A FONT FILE (*.TTF, *.FNT) ANYWHERE TO CHANGE THE DEFAULT FONT!", RICON_TEXT_T));
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
    widgetRect[selected].y = 85 - height;
    widgetRect[selected].height = height;
    widgetRect[selected].width = width;
    
    // UI for selecting the selected text background and foreground color
    if(ColorButton((Rectangle){625,320,30,30}, FG)) clr = &FG;
    if(ColorButton((Rectangle){625,389,30,30}, BG)) clr = &BG;
    *clr = GuiColorPicker((Rectangle){660,320,90,85}, *clr);
    float alpha = clr->a;
    GuiSetStyle(SLIDER, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
    clr->a = GuiSlider((Rectangle){664,420,100,20}, TextFormat("#%i#Alpha", RICON_CROP_ALPHA), alpha, 0.f, 255.f, true);
    
    
    
    // HANDLE DROPPED FONT FILES, IF ANY
    if(IsFileDropped()) 
    {
        int count = 0;
        char** files = GetDroppedFiles(&count);
        if(IsFileExtension(files[0], ".ttf") || IsFileExtension(files[0], ".fnt"))
        {
            Font fnt = LoadFont(files[0]);
            if(fnt.texture.id != 0)
            {
                // Font was loaded, only change font on success
                GuiFont(fnt);
                
                // Remove old font
                if(font.texture.id != 0) UnloadFont(font);
                font = fnt;
            }
        }
        ClearDroppedFiles();
    }
}

int main(int argc, char **argv)
{
    InitWindow(screenWidth, screenHeight, "raygui textbox test");
	SetTargetFPS(60);
	GuiLoadStyleDefault();
    
    // Generate a checked pattern used by the color buttons
	Image timg = GenImageChecked(26,26,5,5,RAYWHITE,DARKGRAY);
	pattern = LoadTextureFromImage(timg);
	UnloadImage(timg);
    
    // Set initial style
    fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    fontSpacing = GuiGetStyle(DEFAULT, TEXT_SPACING);
    padding = GuiGetStyle(TEXTBOX, INNER_PADDING);
    border = GuiGetStyle(TEXTBOX, BORDER_WIDTH);
    FG = GetColor(GuiGetStyle(TEXTBOX, COLOR_SELECTED_FG)); 
    BG = GetColor(GuiGetStyle(TEXTBOX, COLOR_SELECTED_BG));
    clr = &BG;
    
	while (!WindowShouldClose())    // Detect window close button or ESC key
    {
		BeginDrawing();
		
		ClearBackground(RAYWHITE);
		DrawGui();
		
		EndDrawing();
	}

	CloseWindow();
	return 0;
}

// Draw a color button, if clicked return true.
bool ColorButton(Rectangle bounds, Color color)
{
    Rectangle body = {bounds.x + 2, bounds.y + 2, bounds.width - 4, bounds.height - 4};
    bool pressed = false;
    
    // Update control
    Vector2 mouse = GetMousePosition();
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, bounds))
    {
        pressed = true;
    }
    
    // Draw control
    DrawTexture(pattern, body.x, body.y, WHITE);
    DrawRectangleRec(body, color);
    DrawRectangleLinesEx(bounds, 1, BLACK);
    
    return pressed;
}
