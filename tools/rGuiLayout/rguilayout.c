/*******************************************************************************************
*
*   raylib GUI layout editor
*
*   This example has been created using raylib 1.6 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014-2018 Ramon Santamaria (@raysan5)
*
********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_STYLE_SAVE_LOAD
#include "raygui.h"
#include "easings.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define MAX_GUI_CONTROLS    64          // Maximum number of gui controls

#define GRID_LINE_SPACING   10          // Grid line spacing in pixels
#define GRID_ALPHA          0.1f        // Grid lines alpha amount

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { 
    LABEL = 0, 
    BUTTON, 
    IMAGEBUTTON,
    TOGGLE, 
    TOGGLEGROUP, 
    SLIDER, 
    SLIDERBAR, 
    PROGRESSBAR, 
    SPINNER, 
    COMBOBOX, 
    CHECKBOX, 
    TEXTBOX,
    LISTVIEW,
    COLORPICKER
} GuiControlType;

// Gui control type
typedef struct {
    int id;
    int type;
    Rectangle rec;
    char text[32];
} GuiControl;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static int screenWidth = 1280;
static int screenHeight = 720;

static GuiControl layout[MAX_GUI_CONTROLS];
static int controlsCounter = 0;

//Rectangle defaultControlWidth[] = { };

const char *controlTypeName[] = { "LABEL", "BUTTON", "IMAGEBUTTON", "TOGGLE", "TOGGLEGROUP", "SLIDER", "SLIDERBAR", "PROGRESSBAR", "SPINNER", "COMBOBOX", "CHECKBOX", "TEXTBOX", "LISTVIEW", "COLORPICKER" };
const char *controlTypeNameLow[] = { "Label", "Button", "ImageButton", "Toggle", "ToggleGroup", "Slider", "SliderBar", "ProgressBar", "Spinner", "ComboBox", "CheckBox", "TextBox", "ListView", "ColorPicker" };
const char *controlTypeNameShort[] = { "lbl", "btn", "ibtn", "tggl", "tgroup", "sldr", "sldrb", "prgssb", "spnr", "combox", "chkbox", "txtbox", "lstvw", "clrpckr" };

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static void DrawGrid2D(int divsX, int divsY);           // Draw 2d grid with a specific number of divisions for horizontal and vertical lines
static void SaveGuiLayout(const char *fileName);        // Save gui layout project information
static void LoadGuiLayout(const char *fileName);        // Load gui layout project information
static void GenerateGuiLayoutCode(const char *fileName);// Generate C code for gui layout

//----------------------------------------------------------------------------------
// Main Entry point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "raylib tool - raygui layout editor");
    
    int controlSelected = -1;
    
    const char *list[3] = { "ONE", "TWO", "THREE" };
    
    bool snapMode = false;
    int selectedType = BUTTON;
    int mouseX, mouseY;
    
    int selectedTypeDraw = LABEL;
    bool controlCollision = false;
    Rectangle recDraw = { 0, 0, 100, 30 };
    Rectangle listViewControls = { -200, 0, 140, 500 };
    int counterListViewControls = 0;
    int startPosXListViewControls = -200;
    int deltaPosXListViewControls = 0;
    Rectangle listViewControlsCounter = { GetScreenWidth() + 140, 0, 140, 500 };
    int counterListViewControlsCounter = 0;
    int startPosXListViewControlsCounter = GetScreenWidth() + 140;
    int deltaPosXListViewControlsCounter = 0;
    bool controlDrag = false;
 
    const char *guiControls[14] = { 
        "LABEL", 
        "BUTTON", 
        "IMAGEBUTTON",
        "TOGGLE", 
        "TOGGLEGROUP", 
        "SLIDER", 
        "SLIDERBAR", 
        "PROGRESSBAR", 
        "SPINNER", 
        "COMBOBOX", 
        "CHECKBOX", 
        "TEXTBOX",
        "LISTVIEW",
        "COLORPICKER"
    };
    
    const char *guiControlsCounter[64] = { 
        "00",
        "01",
        "02",
        "03",
        "04",
        "05",
        "06",
        "07",
        "08",
        "09",
        "10",
        "11",
        "12",
        "13",
        "14",
        "15",
        "16",
        "17",
        "18",
        "19",
        "20",
        "21",
        "22",
        "23",
        "24",
        "25",
        "26",
        "27",
        "28",
        "29",
        "30",
        "31",
        "32",
        "33",
        "34",
        "35",
        "36",
        "37",
        "38",
        "39",
        "40",
        "41",
        "42",
        "43",
        "44",
        "45",
        "46",
        "47",
        "48",
        "49",
        "50",
        "51",
        "52",
        "53",
        "54",
        "55",
        "56",
        "57",
        "58",
        "59",
        "60",
        "61",
        "62",
        "63"
    };
    
    GuiLoadStyleImage("default_light.png");
    Texture2D texture = LoadTexture("default_light.png");
    
    SetTargetFPS(120);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        mouseX = GetMouseX();
        mouseY = GetMouseY();
        // Updates the recDraw position
        recDraw.x = mouseX - recDraw.width/2;
        recDraw.y = mouseY - recDraw.height/2;
        // Checks if the recDraw is colliding with the list of the controls
        if (CheckCollisionPointRec(GetMousePosition(), listViewControls))controlCollision = true;
        
        // Toggle on the controlListView
        if (IsKeyPressed(KEY_TAB))
        {
            startPosXListViewControls = listViewControls.x;
            deltaPosXListViewControls = 0 - startPosXListViewControls;
            counterListViewControls = 0;
        }
        if (IsKeyDown(KEY_TAB))
        {
            counterListViewControls++;
            if (counterListViewControls >= 60) counterListViewControls = 60;
            listViewControls.x = (int)EaseCubicInOut(counterListViewControls, startPosXListViewControls, deltaPosXListViewControls, 60);
        }  
        else if (IsKeyReleased(KEY_TAB))
        {
            startPosXListViewControls = listViewControls.x;
            deltaPosXListViewControls = -200 - startPosXListViewControls;
            counterListViewControls = 0;
        }
        else
        {
            counterListViewControls++;
            if (counterListViewControls >= 60) counterListViewControls = 60;
            listViewControls.x = (int)EaseCubicInOut(counterListViewControls, startPosXListViewControls, deltaPosXListViewControls, 60);
        }
        
        // Toggle on the controlListViewCounter
        if (IsKeyPressed(KEY_LEFT_SHIFT))
        {
            startPosXListViewControlsCounter = listViewControlsCounter.x;
            deltaPosXListViewControlsCounter = GetScreenWidth() -listViewControlsCounter.width - startPosXListViewControlsCounter;
            counterListViewControlsCounter = 0;
        }
        if (IsKeyDown(KEY_LEFT_SHIFT))
        {
            counterListViewControlsCounter++;
            if (counterListViewControlsCounter >= 60) counterListViewControlsCounter = 60;
            listViewControlsCounter.x = (int)EaseCubicInOut(counterListViewControlsCounter, startPosXListViewControlsCounter, deltaPosXListViewControlsCounter, 60);
        }  
        else if (IsKeyReleased(KEY_LEFT_SHIFT))
        {
            startPosXListViewControlsCounter = listViewControlsCounter.x;
            deltaPosXListViewControlsCounter = GetScreenWidth() + 140 - startPosXListViewControlsCounter;
            counterListViewControlsCounter = 0;
        }
        else
        {
            counterListViewControlsCounter++;
            if (counterListViewControlsCounter >= 60) counterListViewControlsCounter = 60;
            listViewControlsCounter.x = (int)EaseCubicInOut(counterListViewControlsCounter, startPosXListViewControlsCounter, deltaPosXListViewControlsCounter, 60);
        }
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (controlSelected == -1) && !controlCollision)
        {
            // Add new control (button)
            layout[controlsCounter].id = controlsCounter;
            layout[controlsCounter].type = selectedType;
            layout[controlsCounter].rec = (Rectangle){ mouseX, mouseY, 100, 30 };

            controlsCounter++;
        }
        
        for (int i = 0; i < controlsCounter; i++)
        {
            if (controlDrag) break;
            if (CheckCollisionPointRec(GetMousePosition(), layout[i].rec)) 
            {
                controlSelected = i;
                break;
            }
            else controlSelected = -1;
        }
        
        if (controlSelected != -1)
        {
            //Disables the recDraw
            controlCollision = true;
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                controlDrag = true;
            }
            else if(IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                controlDrag = false;
                controlSelected = -1;
            } 
             
            if(controlDrag)
            {
                layout[controlSelected].rec.x = mouseX - layout[controlSelected].rec.width/2;
                layout[controlSelected].rec.y = mouseY - layout[controlSelected].rec.height/2;
                
                // Snap to grid position and size
                if (snapMode)
                {
                    // Snap rectangle position to closer snap point
                    int offsetX = layout[controlSelected].rec.x%GRID_LINE_SPACING;
                    int offsetY = layout[controlSelected].rec.y%GRID_LINE_SPACING;
                    
                    if (offsetX >= GRID_LINE_SPACING/2) layout[controlSelected].rec.x += (GRID_LINE_SPACING - offsetX);
                    else layout[controlSelected].rec.x -= offsetX;
                    
                    if (offsetY >= GRID_LINE_SPACING/2) layout[controlSelected].rec.y += (GRID_LINE_SPACING - offsetY);
                    else layout[controlSelected].rec.y -= offsetY;
                    
                    // Snap rectangle size to closer snap point sizes
                    offsetX = layout[controlSelected].rec.width%GRID_LINE_SPACING;
                    offsetY = layout[controlSelected].rec.height%GRID_LINE_SPACING;
                    
                    if (offsetX >= GRID_LINE_SPACING/2) layout[controlSelected].rec.width += (GRID_LINE_SPACING - offsetX);
                    else layout[controlSelected].rec.width -= offsetX;
                    
                    if (offsetY >= GRID_LINE_SPACING/2) layout[controlSelected].rec.height += (GRID_LINE_SPACING - offsetY);
                    else layout[controlSelected].rec.height -= offsetY;
                }
            }
            
            if (snapMode)
            {
                if (IsKeyPressed(KEY_RIGHT)) layout[controlSelected].rec.width += GRID_LINE_SPACING;
                else if (IsKeyPressed(KEY_LEFT)) layout[controlSelected].rec.width -= GRID_LINE_SPACING;
            
                if (IsKeyPressed(KEY_UP)) layout[controlSelected].rec.height -= GRID_LINE_SPACING;
                else if (IsKeyPressed(KEY_DOWN)) layout[controlSelected].rec.height += GRID_LINE_SPACING;
            }
            else
            {
                if (IsKeyDown(KEY_LEFT_CONTROL))
                {
                    // Control modifier for a more precise sizing
                    if (IsKeyPressed(KEY_RIGHT)) layout[controlSelected].rec.width++;
                    else if (IsKeyPressed(KEY_LEFT)) layout[controlSelected].rec.width--;
                
                    if (IsKeyPressed(KEY_UP)) layout[controlSelected].rec.height--;
                    else if (IsKeyPressed(KEY_DOWN)) layout[controlSelected].rec.height++;
                }
                else
                {
                    if (IsKeyDown(KEY_RIGHT)) layout[controlSelected].rec.width++;
                    else if (IsKeyDown(KEY_LEFT)) layout[controlSelected].rec.width--;
                    
                    if (IsKeyDown(KEY_UP)) layout[controlSelected].rec.height--;
                    else if (IsKeyDown(KEY_DOWN)) layout[controlSelected].rec.height++;
                }
            }
            
            // Delete selected control and shift array position
            if (IsKeyDown(KEY_BACKSPACE))
            {
                for (int i = controlSelected; i < controlsCounter; i++) layout[i] = layout[i + 1];

                controlsCounter--;
                controlSelected = -1;
            }
        }
        else
        {
            /*
            if (IsKeyPressed(KEY_LEFT))
            {
                selectedType--;
                if (selectedType < LABEL) selectedType = LABEL;
            }
            else if (IsKeyPressed(KEY_RIGHT))
            {
                selectedType++;
                if (selectedType > TEXTBOX) selectedType = TEXTBOX;
            }
            */
            //Enables the recDraw
            if (!CheckCollisionPointRec(GetMousePosition(), listViewControls))controlCollision = false;
            // Updates the selectedType with the MouseWheel
            selectedType -= GetMouseWheelMove();
            if (selectedType < LABEL) selectedType = LABEL;
            else if (selectedType > COLORPICKER) selectedType = COLORPICKER;
            selectedTypeDraw = selectedType;
        }
        
        if (IsKeyPressed(KEY_S)) snapMode = !snapMode;
        
        // Mouse snap
        // NOTE: Snap point changes when GRID_LINE_SPACING/2 has been surpassed in X and Y
        if ((snapMode) && (controlSelected == -1))
        {
            int offsetX = mouseX%GRID_LINE_SPACING;
            int offsetY = mouseY%GRID_LINE_SPACING;
            
            if (offsetX >= GRID_LINE_SPACING/2) mouseX += (GRID_LINE_SPACING - offsetX);
            else mouseX -= offsetX;
            
            if (offsetY >= GRID_LINE_SPACING/2) mouseY += (GRID_LINE_SPACING - offsetY);
            else mouseY -= offsetY;
            
            // SnapMode of the DrawingControls
            // Snap rectangle position to closer snap point
            offsetX = recDraw.x%GRID_LINE_SPACING;
            offsetY = recDraw.y%GRID_LINE_SPACING;

            if (offsetX >= GRID_LINE_SPACING/2) recDraw.x += (GRID_LINE_SPACING - offsetX);
            else recDraw.x -= offsetX;

            if (offsetY >= GRID_LINE_SPACING/2) recDraw.y += (GRID_LINE_SPACING - offsetY);
            else recDraw.y -= offsetY;
            // Snap rectangle size to closer snap point sizes
            offsetX = recDraw.width%GRID_LINE_SPACING;
            offsetY = recDraw.height%GRID_LINE_SPACING;
            
            if (offsetX >= GRID_LINE_SPACING/2) recDraw.width += (GRID_LINE_SPACING - offsetX);
            else recDraw.width -= offsetX;
            
            if (offsetY >= GRID_LINE_SPACING/2) recDraw.height += (GRID_LINE_SPACING - offsetY);
            else recDraw.height -= offsetY;
        }
     
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) SaveGuiLayout("test_layout.rlyt");
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O)) LoadGuiLayout("test_layout.rlyt");
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_ENTER)) GenerateGuiLayoutCode("test_layout.c");
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            DrawGrid2D(77, 40);
            
            if (controlSelected == -1)
            {
                DrawLine(mouseX - 8, mouseY, mouseX + 8, mouseY, RED);
                DrawLine(mouseX, mouseY - 8, mouseX, mouseY + 8, RED);
            }

            // Draws the recDraw of the control selected
            if(!controlCollision)  
            switch (selectedTypeDraw)
            {
                case LABEL: GuiLabel(recDraw, "TEXT SAMPLE"); break;
                case BUTTON: GuiButton(recDraw, "BUTTON"); break;
                case IMAGEBUTTON: GuiImageButton(recDraw, texture); break;
                case TOGGLE: GuiToggleButton(recDraw, "TOGGLE", false); break;
                case TOGGLEGROUP: GuiToggleGroup(recDraw, list, 3, 1); break;
                case SLIDER: GuiSlider(recDraw, 40, 0, 100); break;
                case SLIDERBAR: GuiSliderBar(recDraw, 40, 0, 100); break;
                case PROGRESSBAR: GuiProgressBar(recDraw, 40, 0, 100); break;
                case SPINNER: GuiSpinner(recDraw, 40, 0, 100); break;
                case COMBOBOX: GuiComboBox(recDraw, list, 3, 1); break;
                case CHECKBOX: GuiCheckBox(recDraw, false); break;
                case TEXTBOX: GuiTextBox(recDraw, "test text", 32); break;
                case LISTVIEW: GuiListView(recDraw, guiControls, 14, 1); break;
                case COLORPICKER: GuiColorPicker(recDraw, RED); break;
                default: break;
            }
            
            for (int i = 0; i < controlsCounter; i++)
            {
                // Draws the Controls when placed on the grid.
                switch (layout[i].type)
                {
                    case LABEL: GuiLabel(layout[i].rec, "TEXT SAMPLE"); break;
                    case BUTTON: GuiButton(layout[i].rec, "BUTTON"); break;
                    case IMAGEBUTTON: GuiImageButton(layout[i].rec, texture); break;
                    case TOGGLE: GuiToggleButton(layout[i].rec, "TOGGLE", false); break;
                    case TOGGLEGROUP: GuiToggleGroup(layout[i].rec, list, 3, 1); break;
                    case SLIDER: GuiSlider(layout[i].rec, 40, 0, 100); break;
                    case SLIDERBAR: GuiSliderBar(layout[i].rec, 40, 0, 100); break;
                    case PROGRESSBAR: GuiProgressBar(layout[i].rec, 40, 0, 100); break;
                    case SPINNER: GuiSpinner(layout[i].rec, 40, 0, 100); break;
                    case COMBOBOX: GuiComboBox(layout[i].rec, list, 3, 1); break;
                    case CHECKBOX: GuiCheckBox(layout[i].rec, false); break;
                    case TEXTBOX: GuiTextBox(layout[i].rec, "test text", 32); break;
                    case LISTVIEW: GuiListView(layout[i].rec, guiControls, 14, 1); break;
                    case COLORPICKER: GuiColorPicker(layout[i].rec, RED); break;
                    default: break;
                }
            }
            
            // Draw the list of controls
            DrawRectangleRec(listViewControls, Fade(WHITE, 0.7f));
            selectedType = GuiListView(listViewControls, guiControls, 14, selectedType);
            
            // Draw the list of controlsCounter
            DrawRectangleRec(listViewControlsCounter, Fade(WHITE, 0.7f));
            GuiListView(listViewControlsCounter, guiControlsCounter, controlsCounter, -1);
           
            
            if ((controlSelected != -1) && (controlSelected < controlsCounter)) DrawRectangleRec(layout[controlSelected].rec, Fade(RED, 0.5f));
            
            // Debug information
            /*DrawText(FormatText("Controls count: %i", controlsCounter), 10, screenHeight - 20, 20, BLUE);
            DrawText(FormatText("Selected type: %s", controlTypeName[selectedType]), 300, screenHeight - 20, 20, BLUE);
            if (snapMode) DrawText("SNAP ON", 700, screenHeight - 20, 20, RED);
            if (controlSelected != -1) DrawText(FormatText("rec: (%i, %i, %i, %i)", 
                                                layout[controlSelected].rec.x, layout[controlSelected].rec.y, 
                                                layout[controlSelected].rec.width, layout[controlSelected].rec.height), 
                                                10, screenHeight - 40, 10, DARKGREEN);
            DrawText(FormatText("mouse: (%i, %i)", mouseX, mouseY), 700, screenHeight - 40, 10, RED);
            */
            // Draw status bar bottom with debug information
            DrawRectangle(0, GetScreenHeight() - 24, GetScreenWidth(), 24, LIGHTGRAY);
            GuiLabel((Rectangle){20, GetScreenHeight() - 16, 100, 20}, FormatText("Controls count: %i", controlsCounter));
            GuiLabel((Rectangle){150, GetScreenHeight() - 16, 100, 20}, FormatText("Selected type: %s", controlTypeName[selectedType]));
            if (snapMode) GuiLabel((Rectangle){615, GetScreenHeight() - 16, 100, 20}, "SNAP ON");
            if (controlSelected != -1) GuiLabel((Rectangle){475, GetScreenHeight() - 16, 100, 20}, FormatText("rec: (%i, %i, %i, %i)", 
                                                layout[controlSelected].rec.x, layout[controlSelected].rec.y, 
                                                layout[controlSelected].rec.width, layout[controlSelected].rec.height));
            GuiLabel((Rectangle){350, GetScreenHeight() - 16, 100, 20}, FormatText("mouse: (%i, %i)", mouseX, mouseY));

            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------   
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Draw 2d grid
static void DrawGrid2D(int divsX, int divsY)
{
    int spacing = 0;
    /*divsX = GetScreenHeight()/13;
    divsY = GetScreenWidth()/13;
    */
    // Draw vertical grid lines
    for (int i = 0; i < divsX; i++)
    {
        for (int k = 0; k < 5; k++)
        {
            DrawRectangle(-(divsX/2*GRID_LINE_SPACING*5) + spacing - 1, -1, 1, 4000, ((k == 0) ? Fade(BLACK, GRID_ALPHA*2) : Fade(GRAY, GRID_ALPHA)));
            spacing += GRID_LINE_SPACING;
        }
    }

    spacing = 0;
    
    // Draw horizontal grid lines
    for (int i = 0; i < divsY; i++)
    {
        for (int k = 0; k < 5; k++)
        {
            DrawRectangle(-1, -(divsY/2*GRID_LINE_SPACING*5) + spacing - 1, 4000, 1, ((k == 0) ? Fade(BLACK, GRID_ALPHA*2) : Fade(GRAY, GRID_ALPHA)));
            spacing += GRID_LINE_SPACING;
        }
    }
}

// Save gui layout project information
// NOTE: Exported as text file
static void SaveGuiLayout(const char *fileName)
{
    FILE *flayout = fopen(fileName, "wt");
    
    fprintf(flayout, "# Num Controls : %i\n\n", controlsCounter);
    
    for (int i = 0; i < controlsCounter; i++)
    {
        fprintf(flayout, "# Control %03i : %s\n", i, controlTypeName[layout[i].type]);
        fprintf(flayout, "type %i rec %i %i %i %i\n\n", layout[i].type, layout[i].rec.x, layout[i].rec.y, layout[i].rec.width, layout[i].rec.height);
    }
    
    fclose(flayout);
}

// Import gui layout project information
// NOTE: Imported from text file
static void LoadGuiLayout(const char *fileName)
{
    char line[128];

    FILE *flayout = fopen(fileName, "rt");
    
    controlsCounter = 0;
    
    while (!feof(flayout))
    {
        fgets(line, 128, flayout);

        switch (line[0])
        {
            case 'c':
            {
                sscanf(line, "c type %i rec %i %i %i %i", &layout[controlsCounter].type, 
                                                          &layout[controlsCounter].rec.x, 
                                                          &layout[controlsCounter].rec.y, 
                                                          &layout[controlsCounter].rec.width, 
                                                          &layout[controlsCounter].rec.height);
                controlsCounter++;
            } break;
            default: break;
        }
    }

    fclose(flayout);
}

// Generate C code for gui layout
static void GenerateGuiLayoutCode(const char *fileName)
{
    FILE *ftool = fopen(fileName, "wt");
    
    fprintf(ftool, "/*******************************************************************************************\n");
    fprintf(ftool, "*\n");
    fprintf(ftool, "*   $(tool_name) - $(tool_description)\n");
    fprintf(ftool, "*\n");
    fprintf(ftool, "*   LICENSE: zlib/libpng\n");
    fprintf(ftool, "*\n");
    fprintf(ftool, "*   Copyright (c) $(year) $(author)\n");
    fprintf(ftool, "*\n");
    fprintf(ftool, "**********************************************************************************************/\n\n");
    fprintf(ftool, "#include \"raylib.h\"\n\n");
    fprintf(ftool, "#define RAYGUI_IMPLEMENTATION\n");
    fprintf(ftool, "#include \"raygui.h\"\n\n");
    fprintf(ftool, "//----------------------------------------------------------------------------------\n");
    fprintf(ftool, "// Controls Functions Declaration\n");
    fprintf(ftool, "//----------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < controlsCounter; i++) if (layout[i].type == BUTTON) fprintf(ftool, "static void Button%03i();\n", i);

    fprintf(ftool, "\n");
    fprintf(ftool, "//------------------------------------------------------------------------------------\n");
    fprintf(ftool, "// Program main entry point\n");
    fprintf(ftool, "//------------------------------------------------------------------------------------\n");
    fprintf(ftool, "int main()\n");
    fprintf(ftool, "{\n");
    fprintf(ftool, "    // Initialization\n");
    fprintf(ftool, "    //---------------------------------------------------------------------------------------\n");
    fprintf(ftool, "    int screenWidth = %i;\n", screenWidth);
    fprintf(ftool, "    int screenHeight = %i;\n\n", screenHeight);
    fprintf(ftool, "    InitWindow(screenWidth, screenHeight, \"rFXGen\");\n\n");
    
    // Define controls rectangles
    fprintf(ftool, "    Rectangle layoutRecs[%i] = {\n", controlsCounter);
    
    for (int i = 0; i < controlsCounter; i++)
    {
        fprintf(ftool, "        (Rectangle){ %i, %i, %i, %i }", layout[i].rec.x, layout[i].rec.y, layout[i].rec.width, layout[i].rec.height);
        
        if (i == controlsCounter - 1) fprintf(ftool, "  // %s %03i\n    };\n\n", controlTypeName[layout[i].type], i);
        else fprintf(ftool, ",  // %s %03i\n", controlTypeName[layout[i].type], i);
    }

    fprintf(ftool, "    SetTargetFPS(60);\n");
    fprintf(ftool, "    //--------------------------------------------------------------------------------------\n\n");
    fprintf(ftool, "    // Main game loop\n");
    fprintf(ftool, "    while (!WindowShouldClose())    // Detect window close button or ESC key\n");
    fprintf(ftool, "    {\n");
    fprintf(ftool, "        // Update\n");
    fprintf(ftool, "        //----------------------------------------------------------------------------------\n");
    fprintf(ftool, "        // TODO: Implement required update logic\n");
    fprintf(ftool, "        //----------------------------------------------------------------------------------\n\n");
    fprintf(ftool, "        // Draw\n");
    fprintf(ftool, "        //----------------------------------------------------------------------------------\n");
    fprintf(ftool, "        BeginDrawing();\n\n");
    fprintf(ftool, "            ClearBackground(GuiBackground());\n\n");

    // Draw all controls
    for (int i = 0; i < controlsCounter; i++)
    {
        switch (layout[i].type)
        {
            case LABEL: fprintf(ftool, "            GuiLabel(layoutRecs[%i], \"TEXT SAMPLE\");\n\n", i); break;
            case BUTTON: fprintf(ftool, "            if (GuiButton(layoutRecs[%i], \"BUTTON\")) Button%03i(); \n\n", i, i); break;
            /*
            case TOGGLE: GuiToggleButton(layout[i].rec, \"TOGGLE\", false); break;
            case TOGGLEGROUP: GuiToggleGroup(layout[i].rec, 3, list, 1); break;
            case SLIDER: GuiSlider(layout[i].rec, 40, 0, 100); break;
            case SLIDERBAR: GuiSliderBar(layout[i].rec, 40, 0, 100); break;
            case PROGRESSBAR: GuiProgressBar(layout[i].rec, 40); break;
            case SPINNER: GuiSpinner(layout[i].rec, 40, 0, 100); break;
            case COMBOBOX: GuiComboBox(layout[i].rec, 3, list, 1); break;
            case CHECKBOX: GuiCheckBox(layout[i].rec, \"CHECKBOX\", false); break;
            case TEXTBOX: GuiTextBox(layout[i].rec, \"test text\"); break;
            */
            default: break;
        }
    }

    fprintf(ftool, "        EndDrawing();\n");
    fprintf(ftool, "        //----------------------------------------------------------------------------------\n");
    fprintf(ftool, "    }\n\n");
    fprintf(ftool, "    // De-Initialization\n");
    fprintf(ftool, "    //--------------------------------------------------------------------------------------\n");
    fprintf(ftool, "    CloseWindow();        // Close window and OpenGL context\n");
    fprintf(ftool, "    //--------------------------------------------------------------------------------------\n\n");
    fprintf(ftool, "    return 0;\n");
    fprintf(ftool, "}\n\n");

    fprintf(ftool, "//------------------------------------------------------------------------------------\n");
    fprintf(ftool, "// Controls Functions Definitions (local)\n");
    fprintf(ftool, "//------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < controlsCounter; i++)
        if (layout[i].type == BUTTON)
            fprintf(ftool, "static void Button%03i()\n{\n    // TODO: Implement control logic\n}\n\n", i);

    fclose(ftool);
}