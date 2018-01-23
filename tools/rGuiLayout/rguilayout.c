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
#include "../../src/raygui.h"

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define MAX_GUI_CONTROLS    128         // Maximum number of gui controls

#define GRID_LINE_SPACING   10          // Grid line spacing in pixels
#define GRID_ALPHA          0.1f        // Grid lines alpha amount

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { 
    LABEL = 0, 
    BUTTON, 
    TOGGLE, 
    TOGGLEGROUP, 
    SLIDER, 
    SLIDERBAR, 
    PROGRESSBAR, 
    SPINNER, 
    COMBOBOX, 
    CHECKBOX, 
    TEXTBOX 
} GuiControlType;

// Gui control type
typedef struct {
    int id;
    GuiControlType type;
    Rectangle rec;
} GuiControl;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static int screenWidth = 1280;
static int screenHeight = 720;

static GuiControl layout[MAX_GUI_CONTROLS];
static int controlsCounter = 0;

//Rectangle defaultControlWidth[] = { };

const char *controlTypeName[] = { "LABEL", "BUTTON", "TOGGLE", "TOGGLEGROUP", "SLIDER", "SLIDERBAR", "PROGRESSBAR", "SPINNER", "COMBOBOX", "CHECKBOX", "TEXTBOX" };
const char *controlTypeNameLow[] = { "Label", "Button", "Toggle", "ToggleGroup", "Slider", "SliderBar", "ProgressBar", "Spinner", "ComboBox", "CheckBox", "TextBox" };
const char *controlTypeNameShort[] = { "lbl", "btn", "tggl", "tgroup", "sldr", "sldrb", "prgssb", "spnr", "combox", "chkbox", "txtbox" };

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
    
    SetTargetFPS(120);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        mouseX = GetMouseX();
        mouseY = GetMouseY();
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (controlSelected == -1))
        {
            // Add new control (button)
            layout[controlsCounter].id = controlsCounter;
            layout[controlsCounter].type = selectedType;
            layout[controlsCounter].rec = (Rectangle){ mouseX, mouseY, 100, 30 };

            controlsCounter++;
        }
        
        for (int i = 0; i < controlsCounter; i++)
        {
            if (CheckCollisionPointRec(GetMousePosition(), layout[i].rec)) 
            {
                controlSelected = i;
                break;
            }
            else controlSelected = -1;
        }
        
        if (controlSelected != -1)
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
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
        }
        
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) SaveGuiLayout("test_layout.rlyt");
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O)) LoadGuiLayout("test_layout.rlyt");
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_ENTER)) GenerateGuiLayoutCode("test_layout.c");
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            DrawGrid2D(60, 40);
            
            if (controlSelected == -1)
            {
                DrawLine(mouseX - 8, mouseY, mouseX + 8, mouseY, RED);
                DrawLine(mouseX, mouseY - 8, mouseX, mouseY + 8, RED);
            }

            for (int i = 0; i < controlsCounter; i++)
            {
                switch (layout[i].type)
                {
                    case LABEL: GuiLabel(layout[i].rec, "TEXT SAMPLE"); break;
                    case BUTTON: GuiButton(layout[i].rec, "BUTTON"); break;
                    case TOGGLE: GuiToggleButton(layout[i].rec, "TOGGLE", false); break;
                    case TOGGLEGROUP: GuiToggleGroup(layout[i].rec, list, 3, 1); break;
                    case SLIDER: GuiSlider(layout[i].rec, 40, 0, 100); break;
                    case SLIDERBAR: GuiSliderBar(layout[i].rec, 40, 0, 100); break;
                    case PROGRESSBAR: GuiProgressBar(layout[i].rec, 40, 0, 100); break;
                    case SPINNER: GuiSpinner(layout[i].rec, 40, 0, 100); break;
                    case COMBOBOX: GuiComboBox(layout[i].rec, list, 3, 1); break;
                    case CHECKBOX: GuiCheckBox(layout[i].rec, false); break;
                    case TEXTBOX: GuiTextBox(layout[i].rec, "test text", 32); break;
                    default: break;
                }
            }
            
            if ((controlSelected != -1) && (controlSelected < controlsCounter)) DrawRectangleRec(layout[controlSelected].rec, Fade(RED, 0.5f));
            
            // Debug information
            DrawText(FormatText("Controls count: %i", controlsCounter), 10, screenHeight - 20, 20, BLUE);
            DrawText(FormatText("Selected type: %s", controlTypeName[selectedType]), 300, screenHeight - 20, 20, BLUE);
            if (snapMode) DrawText("SNAP ON", 700, screenHeight - 20, 20, RED);
            if (controlSelected != -1) DrawText(FormatText("rec: (%i, %i, %i, %i)", 
                                                layout[controlSelected].rec.x, layout[controlSelected].rec.y, 
                                                layout[controlSelected].rec.width, layout[controlSelected].rec.height), 
                                                10, screenHeight - 40, 10, DARKGREEN);
            DrawText(FormatText("mouse: (%i, %i)", mouseX, mouseY), 700, screenHeight - 40, 10, RED);
            
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