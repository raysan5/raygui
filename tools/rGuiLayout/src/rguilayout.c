/*******************************************************************************************
*
*   rGuiLayout v1.1 - raygui layout editor
*
*   Compile this program using:
*       gcc -o rguilayout.exe rguilayout.c external/tinyfiledialogs.c -I..\.. \ 
*       -lraylib -lopengl32 -lgdi32 -lcomdlg32 -lole32 -std=c99 -Wall
*
*   CONTRIBUTORS:
*       Ramon Santamaria:   Supervision, review, design, update and maintenance...
*       Adria Arranz:       Design and implementation v1.0 (2018)
*       Jordi Jorba:        Design and implementation v1.0 (2018)
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2018 raylib technologies (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_STYLE_SAVE_LOAD
//#define RAYGUI_STYLE_DEFAULT_DARK
#include "raygui.h"

#include "external/easings.h"
#include "external/tinyfiledialogs.h"   // Open/Save file dialogs

#include <stdlib.h>

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define MAX_GUI_CONTROLS        256         // Maximum number of gui controls
#define MAX_ANCHOR_POINTS         8         // Maximum number of anchor points
#define ANCHOR_RADIUS            20         // Default anchor radius

#define MAX_CONTROL_TEXT_LENGTH  64         // Maximum length of control text
#define MAX_CONTROL_NAME_LENGTH  32         // Maximum length of control name (used on code generation)

#define GRID_LINE_SPACING         5         // Grid line spacing in pixels

#define MOVEMENT_FRAME_SPEED     10         // Controls movement speed in pixels per frame

#define PALETTE_EASING_FRAMES    30         // Controls the easing time in frames     

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum {
    WINDOWBOX = 0,
    GROUPBOX,
    LINE,
    PANEL,
    LABEL, 
    BUTTON,
    TOGGLE, 
    TOGGLEGROUP,
    CHECKBOX,
    COMBOBOX,
    DROPDOWNBOX,
    SPINNER,
    VALUEBOX,
    TEXTBOX,
    SLIDER, 
    SLIDERBAR, 
    PROGRESSBAR,
    STATUSBAR,
    LISTVIEW,
    COLORPICKER,
    DUMMYREC
} GuiControlType;

// Anchor point type
typedef struct {
    int id;
    int x;
    int y;
    bool enabled;
    bool hidding;
} AnchorPoint;

// Gui control type
typedef struct {
    int id;
    int type;
    Rectangle rec;
    unsigned char name[MAX_CONTROL_NAME_LENGTH];
    unsigned char text[MAX_CONTROL_TEXT_LENGTH];
    AnchorPoint *ap;
} GuiControl;

// Gui layout type
typedef struct {
    int controlsCount;
    AnchorPoint anchors[MAX_ANCHOR_POINTS];
    GuiControl controls[MAX_GUI_CONTROLS];
} GuiLayout;

// Gui layout configuration type
typedef struct {
    int width;
    int height;
    unsigned char name[64];
    unsigned char version[32];
    unsigned char company[128];
    unsigned char description[256];
    bool defineRecs;
    bool exportAnchors;
    bool exportAnchor0;
    bool fullComments;
    bool defineTexts;
    bool cropWindow;
    bool fullVariables;
} GuiLayoutConfig;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static int screenWidth = 800;
static int screenHeight = 600;

static GuiLayout layout = { 0 };

const char *controlTypeName[] = { "WINDOWBOX", "GROUPBOX", "LINE", "PANEL", "LABEL", "BUTTON", "TOGGLE", "TOGGLEGROUP", "CHECKBOX", "COMBOBOX", "DROPDOWNBOX", "SPINNER", "VALUEBOX", "TEXTBOX", "SLIDER", "SLIDERBAR", "PROGRESSBAR", "STATUSBAR", "LISTVIEW", "COLORPICKER", "DUMMYREC" };
const char *controlTypeNameLow[] = { "WindowBox", "GroupBox", "Line", "Panel", "Label", "Button", "Toggle", "ToggleGroup", "CheckBox", "ComboBox", "DropdownBox", "Spinner", "ValueBox", "TextBox", "Slider", "SliderBar", "ProgressBar", "StatusBar", "ListView", "ColorPicker", "DummyRec" };
const char *controlTypeNameShort[] = { "wdwbox", "grpbox", "lne", "pnl", "lbl", "btn", "tgl", "tglgrp", "chkbox", "combox", "ddwnbox", "spnr", "vlbox", "txtbox", "sldr", "sldrb", "prgssb", "stsb", "lstvw", "clrpckr", "dmyrc" };

static bool cancelSave = false;
static char loadedFileName[128] = "\0";
//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static void ShowSaveLayoutDialog(void);                                 // Show save layout dialog
static void ShowExportLayoutDialog(GuiLayoutConfig config);             // Show export layout dialog
static void SaveLayoutRGL(const char *fileName, bool binary);           // Save gui layout project information
static void LoadLayoutRGL(const char *fileName);                        // Load gui layout project information
static void GenerateCode(const char *fileName, GuiLayoutConfig config); // Generate C code for gui layout
static void GenerateCodeFromRGL(const char *fileName);                  // Generate C code from .rgl file
static char *GetControlAnchorRec(int anchorId, Rectangle controlRec, GuiLayoutConfig config);   // Get control rectangle

//----------------------------------------------------------------------------------
// Main Entry point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "rGuiLayout v1.0");
    SetExitKey(0);

    // General app variables
    Vector2 mouse;
    bool exitWindow = false;            // Exit window flag
    bool snapMode = false;              // Snap mode flag (KEY_S)
    bool showGrid = true;               // Show grid flag (KEY_G)
    bool controlDrag = false;           // Control drag mode
    bool controlGlobalPos = false;      // Control global position mode
    bool textEditMode = false;          // Control text edit mode (KEY_T)
    bool nameEditMode = false;          // Control name edit mode (KEY_N)
    
    int framesCounter = 0;
    int framesCounterSnap = 0;
    int selectedControl = -1;
    int storedControl = -1;
    int focusedControl = -1;
    int selectedType = WINDOWBOX;
    int selectedTypeDraw = LABEL;
    Vector2 panControlOffset = { 0 };
    Vector2 prevControlPosition = { 0 };
    
    const char *listData[3] = { "ONE", "TWO", "THREE" };    // ToggleGroup, ComboBox, DropdownBox default data
    const char *listViewData[4] = { "WINDOWBOX", "GROUPBOX", "LINE", "PANEL" }; // ListView default data

    // Anchors control variables
    AnchorPoint auxAnchor = { 9, 0, 0, 0 };
    bool anchorMode = false;
    bool anchorLinkMode = false;
    bool anchorLockMode = false;
    bool anchorPosEditMode = false;
    int selectedAnchor = -1;
    int linkedAnchor = -1;
    int storedAnchor = -1;

    // Help panel variables
    int helpPositionX = -300;
    int helpCounter = 0;
    int helpStartPositionX = -300;
    int helpDeltaPositionX = 0;
    bool helpActive = false;

    // Rectangles used on controls preview drawing
    Rectangle defaultRec[21] = {
        (Rectangle){ 0, 0, 125, 50},            // WINDOWBOX
        (Rectangle){ 0, 0, 125, 30},            // GROUPBOX
        (Rectangle){ 0, 0, 125, 25 },           // LINE
        (Rectangle){ 0, 0, 125, 35 },           // PANEL
        (Rectangle){ 0, 0, 126, 25 },           // LABEL
        (Rectangle){ 0, 0, 125, 30 },           // BUTTON
        (Rectangle){ 0, 0, 90, 25 },            // TOGGLE
        (Rectangle){ 0, 0, 125, 25 },           // TOGGLEGROUP
        (Rectangle){ 0, 0, 15, 15},             // CHECKBOX
        (Rectangle){ 0, 0, 125, 25 },           // COMBOBOX
        (Rectangle){ 0, 0, 125, 25 },           // DROPDOWNBOX
        (Rectangle){ 0, 0, 125, 25 },           // SPINNER
        (Rectangle){ 0, 0, 125, 25 },           // VALUEBOX
        (Rectangle){ 0, 0, 125, 25 },           // TEXTBOX
        (Rectangle){ 0, 0, 125, 15 },           // SLIDER
        (Rectangle){ 0, 0, 125, 15 },           // SLIDERBAR
        (Rectangle){ 0, 0, 125, 15 },           // PROGRESSBAR
        (Rectangle){ 0, 0, 125, 25 },           // STATUSBAR
        (Rectangle){ 0, 0, 125, 75 },           // LISTVIEW
        (Rectangle){ 0, 0, 95, 95 },            // COLORPICKER
        (Rectangle){ 0, 0, 125, 30 }            // DUMMYREC
    };
    
    // Initialize anchor points to default values
    for (int i = 0; i < MAX_ANCHOR_POINTS; i++)
    {
        layout.anchors[i].id = i;
        layout.anchors[i].x = 0;
        layout.anchors[i].y = 0;
        layout.anchors[i].enabled = false;
        layout.anchors[i].hidding = false;
    }

    layout.anchors[0].enabled = true;      // Enable layout parent anchor (0, 0)
    
    // Initialize layout controls data
    for (int i = 0; i < MAX_GUI_CONTROLS; i++)
    {
        layout.controls[i].id = 0;
        layout.controls[i].type = 0;
        layout.controls[i].rec = (Rectangle){ 0, 0, 0, 0 };
        memset(layout.controls[i].text, 0, MAX_CONTROL_TEXT_LENGTH);
        memset(layout.controls[i].name, 0, MAX_CONTROL_NAME_LENGTH);
        layout.controls[i].ap = &layout.anchors[0];  // By default, set parent anchor
    }
       
    // Define palette variables
    Rectangle palettePanel = { GetScreenWidth() + 130, 20, 135, 870 };
    bool paletteMode = false;
    int paletteSelect = -1;
    int paletteEasingIn = 0;
    int paletteEasingOut = PALETTE_EASING_FRAMES;
    int paletteStartPosX = GetScreenWidth() + 130;

    // Define palette rectangles
    Rectangle paletteRecs[21] = {
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 5, 125, 50 },		    // WindowBox
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 65, 125, 30 },		// GroupBox
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 105, 125, 25 },		// Line
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 140, 125, 35 },		// Panel
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 185, 126, 25 },		// Label
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 220, 125, 30 },		// Button
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 260, 90, 25 },		// Toggle
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 295, 125, 25 },		// ToggleGroup
        (Rectangle){ palettePanel.x + 105, palettePanel.y + 265, 15, 15 },		// CheckBox
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 330, 125, 25 },		// ComboBox
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 365, 125, 25 },		// DropdownBox
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 400, 125, 25 },		// Spinner
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 435, 125, 25 },		// ValueBox
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 470, 125, 25 },		// TextBox
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 505, 125, 15 },		// Slider
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 530, 125, 15 },		// SliderBar
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 555, 125, 15 },		// ProgressBar
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 580, 125, 25 },		// StatusBar
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 615, 125, 75 },		// ListView
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 700, 95, 95 },		// ColorPicker
        (Rectangle){ palettePanel.x + 5, palettePanel.y + 835, 125, 30 }		// DummyRec
    };
    
    // Tracemap (background image for reference) variables
    Texture2D tracemap = { 0 };
    Rectangle tracemapRec = { 0 };
    bool tracemapEditMode = false;
    float tracemapFade = 0.5f;
    
    // loadedTexture for checking if texture is a tracemap or a style
    Texture2D loadedTexture = { 0 };
    
    // Very basic undo system
    // Undo last-selected rectangle changes
    // Undo text/name editing on cancel (KEY_ESC)
    int undoSelectedControl = -1;
    Rectangle undoLastRec;
    char prevControlText[MAX_CONTROL_TEXT_LENGTH];
    char prevControlName[MAX_CONTROL_NAME_LENGTH];

    // Close layout window variables
    bool closingWindowActive = false;
    
    // Generate code options window variables
    Vector2 exportWindowPos = { 50, 50 };
    bool generateWindowActive = false;
    int toolNameSize = 32;
    int toolVersionSize = 32;
    int companySize = 32;
    int toolDescriptionSize = 32;
    
    // Generate code configuration
    GuiLayoutConfig config;
    memset(&config, 0, sizeof(GuiLayoutConfig));
    config.width = 800;
    config.height = 600;
    strcpy(config.name, "layout_file_name");
    strcpy(config.version, "1.0-dev");
    strcpy(config.company, "raylib technologies");
    strcpy(config.description, "tool description");
    config.defineRecs = false;
    config.exportAnchors = false;
    config.exportAnchor0 = false;
    config.fullComments = false;
    config.defineTexts = false;
    config.fullVariables = false;
    
    // Delete current layout and reset variables
    bool resetWindowActive = false;
    bool resetLayout = false;
    

    
    SetTargetFPS(120);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!exitWindow)    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        framesCounterSnap++;
        mouse = GetMousePosition();

        if (WindowShouldClose()) exitWindow = true;
        
        // Show save layout message window on ESC
        if (IsKeyPressed(KEY_ESCAPE) && !textEditMode && !nameEditMode)
        {
            if (layout.controlsCount <= 0) exitWindow = true;
            else if (generateWindowActive) generateWindowActive = false;
            else if (resetWindowActive) resetWindowActive = false;
            else
            {
                closingWindowActive = !closingWindowActive;
                selectedControl = -1;
            }
        }

        // Enables or disables snapMode if not in textEditMode
        if (IsKeyPressed(KEY_S) && (!textEditMode) && (!nameEditMode)) snapMode = !snapMode;
        
        // Enables or disables position reference information(anchor reference or global reference)
        if (IsKeyPressed(KEY_F) && (!textEditMode) && (!nameEditMode)) controlGlobalPos = !controlGlobalPos;
        
        // Toggle help info
        if (IsKeyPressed(KEY_TAB) && !helpActive)
        {
            helpStartPositionX = helpPositionX;
            helpDeltaPositionX = 0 - helpStartPositionX;
            helpCounter = 0;
            helpActive = true;
        }
        else if (IsKeyPressed(KEY_TAB) && helpActive)
        {
            helpStartPositionX = helpPositionX;
            helpDeltaPositionX = -300 - helpStartPositionX;
            helpCounter = 0;
            helpActive = false;
        }
        
        if (helpActive)
        {
            helpCounter++;
            if (helpCounter >= 60) helpCounter = 60;
            helpPositionX = (int)EaseCubicInOut(helpCounter, helpStartPositionX, helpDeltaPositionX, 60);
        }
        else
        {
            helpCounter++;
            if (helpCounter >= 60) helpCounter = 60;
            helpPositionX = (int)EaseCubicInOut(helpCounter, helpStartPositionX, helpDeltaPositionX, 60);
        }
        
        // Controls palette selector logic
        if ((IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) && (!anchorMode) && (!paletteMode))
        {
            paletteMode = true;
            paletteEasingOut = 0;
        }
        else if ((IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) && (!anchorMode) && (paletteMode))
        {
            paletteMode = false;
            paletteEasingIn = 0;
        }
        
        if (paletteMode)
        {
            paletteEasingIn++;
            if (paletteEasingIn >= PALETTE_EASING_FRAMES) paletteEasingIn = PALETTE_EASING_FRAMES;
            palettePanel.x = (int)EaseCubicInOut(paletteEasingIn, paletteStartPosX,(GetScreenWidth() - 145) - paletteStartPosX, PALETTE_EASING_FRAMES);
            
            for (int i = 0; i < 21; i++)
            {
                if (CheckCollisionPointRec(mouse, paletteRecs[i]))
                {
                    paletteSelect = i;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) selectedType = i;
                    break;
                }
                else paletteSelect = -1;
            }
        }
        else 
        {
            paletteStartPosX = GetScreenWidth() + 130;
            paletteEasingOut++;
            if (paletteEasingOut >= PALETTE_EASING_FRAMES) paletteEasingOut = PALETTE_EASING_FRAMES;
            palettePanel.x = (int)EaseCubicInOut(paletteEasingOut, (GetScreenWidth() - 145), paletteStartPosX - (GetScreenWidth() - 145), PALETTE_EASING_FRAMES);
        }
          
        for (int i = 0; i < 21; i++)
        {
            if (i == 8) paletteRecs[i].x = palettePanel.x + 105;
            else paletteRecs[i].x = palettePanel.x + 5;
        }
        
        // Create new control 
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (focusedControl == -1) && (selectedControl == -1) && !anchorMode && !tracemapEditMode && !closingWindowActive && !generateWindowActive && (!resetWindowActive) && !CheckCollisionPointRec(mouse, palettePanel))
        {
            // Add new control (button)
            layout.controls[layout.controlsCount].id = layout.controlsCount;
            layout.controls[layout.controlsCount].type = selectedType;
            layout.controls[layout.controlsCount].rec = (Rectangle){  mouse.x - defaultRec[selectedType].width/2, mouse.y - defaultRec[selectedType].height/2, defaultRec[selectedType].width, defaultRec[selectedType].height };
            if ((layout.controls[layout.controlsCount].type == LABEL) || (layout.controls[layout.controlsCount].type == TEXTBOX) || (layout.controls[layout.controlsCount].type == BUTTON) || (layout.controls[layout.controlsCount].type == TOGGLE)
                || (layout.controls[layout.controlsCount].type == GROUPBOX) || (layout.controls[layout.controlsCount].type == WINDOWBOX) || (layout.controls[layout.controlsCount].type == STATUSBAR) || (layout.controls[layout.controlsCount].type == DUMMYREC)) strcpy(layout.controls[layout.controlsCount].text, "SAMPLE TEXT");
            strcpy(layout.controls[layout.controlsCount].name, FormatText("%s%03i", controlTypeNameLow[layout.controls[layout.controlsCount].type], layout.controlsCount));
            layout.controls[layout.controlsCount].ap = &layout.anchors[0];        // Default anchor point (0, 0)
            
            for (int i = 0; i < layout.controlsCount; i++)
            {
                if (CheckCollisionPointRec(mouse, (Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }) && layout.controls[i].type == WINDOWBOX) layout.controls[layout.controlsCount].ap = layout.controls[i].ap;
            }
            
            if (layout.controls[layout.controlsCount].type == WINDOWBOX)
            {
                for (int i = 1; i < MAX_ANCHOR_POINTS; i++)
                {
                    if (!layout.anchors[i].enabled)
                    {
                        layout.anchors[i].x = layout.controls[layout.controlsCount].rec.x;
                        layout.anchors[i].y = layout.controls[layout.controlsCount].rec.y;
                        
                        if (snapMode)
                        {
                            int offsetX = layout.anchors[i].x%GRID_LINE_SPACING;
                            int offsetY = layout.anchors[i].y%GRID_LINE_SPACING;
                            
                            if (offsetX >= GRID_LINE_SPACING/2) layout.anchors[i].x += (GRID_LINE_SPACING - offsetX);
                            else layout.anchors[i].x -= offsetX;
                            
                            if (offsetY >= GRID_LINE_SPACING/2) layout.anchors[i].y += (GRID_LINE_SPACING - offsetY);
                            else layout.anchors[i].y -= offsetY;
                        }
                        
                        layout.controls[layout.controlsCount].rec.x = layout.anchors[i].x;
                        layout.controls[layout.controlsCount].rec.y = layout.anchors[i].y;
                        
                        layout.anchors[i].enabled = true;
                        layout.controls[layout.controlsCount].ap = &layout.anchors[i];
                        break;
                    }
                }
            }
                             
            layout.controls[layout.controlsCount].rec.x -= layout.controls[layout.controlsCount].ap->x;
            layout.controls[layout.controlsCount].rec.y -= layout.controls[layout.controlsCount].ap->y;
            layout.controlsCount++;     
        }
        else
        {
            if (!(controlDrag || tracemapEditMode || anchorLockMode || closingWindowActive || generateWindowActive || resetWindowActive))
            {
                // Check selected control (on mouse hover)
                for (int i = layout.controlsCount; i >= 0; i--)
                {
                    if ((layout.controls[i].type == WINDOWBOX) && (!layout.controls[i].ap->hidding) && (CheckCollisionPointRec(mouse, (Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, 24 })))
                    {
                        focusedControl = i;
                        
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !anchorLinkMode && !anchorMode)
                        {
                            selectedControl = focusedControl;
                        }
                        
                        if (undoSelectedControl != focusedControl) 
                        {
                            undoSelectedControl = focusedControl;
                            undoLastRec = layout.controls[i].rec;
                        }
                        break;
                    }
                    else if ((!layout.controls[i].ap->hidding) && (CheckCollisionPointRec(mouse, (Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }) && layout.controls[i].type != WINDOWBOX))
                    {
                        focusedControl = i;
                        
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !anchorLinkMode && !anchorMode)
                        {
                            selectedControl = focusedControl;
                        }
                        
                        if (undoSelectedControl != focusedControl) 
                        {
                            undoSelectedControl = focusedControl;
                            undoLastRec = layout.controls[i].rec;
                        }
                        break;
                    }
                    else 
                    {
                        focusedControl = -1;
                        
                        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) selectedControl = -1;
                    }
                }
            }
        }
        
        // Change controls layer order (position inside array)
        if (IsKeyDown(KEY_LEFT_ALT) && (focusedControl != -1))
        {
            if ((IsKeyPressed(KEY_UP)) && (focusedControl < layout.controlsCount - 1))
            {
                // Move control towards beginning of array
                GuiControl auxControl = layout.controls[focusedControl];
                layout.controls[focusedControl] = layout.controls[focusedControl + 1];
                layout.controls[focusedControl].id -= 1;
                layout.controls[focusedControl + 1] = auxControl;
                layout.controls[focusedControl + 1].id += 1;
                selectedControl = -1;
            }
            else if ((IsKeyPressed(KEY_DOWN)) && (focusedControl > 0))
            {
                // Move control towards end of array
                GuiControl auxControl = layout.controls[focusedControl];
                layout.controls[focusedControl] = layout.controls[focusedControl - 1];
                layout.controls[focusedControl].id += 1;
                layout.controls[focusedControl - 1] = auxControl;
                layout.controls[focusedControl - 1].id -= 1;
                selectedControl = -1;
            }
        }
        

        
        if (selectedControl != -1 && !textEditMode && !nameEditMode && !anchorMode)
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                controlDrag = true;
                panControlOffset = mouse;
                prevControlPosition = (Vector2){ layout.controls[selectedControl].rec.x, layout.controls[selectedControl].rec.y  };
            }
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                controlDrag = false;
            } 
             
            if (controlDrag)
            {
                layout.controls[selectedControl].rec.x = prevControlPosition.x + (mouse.x - panControlOffset.x);
                layout.controls[selectedControl].rec.y = prevControlPosition.y + (mouse.y - panControlOffset.y);

                // Snap to grid position and size
                if (snapMode)
                {
                    // Snap rectangle position to closer snap point
                    int offsetX = (int)layout.controls[selectedControl].rec.x%GRID_LINE_SPACING;
                    int offsetY = (int)layout.controls[selectedControl].rec.y%GRID_LINE_SPACING;
                    
                    if (offsetX >= GRID_LINE_SPACING/2) layout.controls[selectedControl].rec.x += (GRID_LINE_SPACING - offsetX);
                    else layout.controls[selectedControl].rec.x -= offsetX;
                    
                    if (offsetY >= GRID_LINE_SPACING/2) layout.controls[selectedControl].rec.y += (GRID_LINE_SPACING - offsetY);
                    else layout.controls[selectedControl].rec.y -= offsetY;
                }
            }
            
            if (!IsKeyDown(KEY_LEFT_ALT) && (selectedControl != -1))
            {
                if (snapMode)
                {
                    if (IsKeyDown(KEY_LEFT_CONTROL))
                    {
                        // Control modifier of width and height
                        if (IsKeyDown(KEY_LEFT_SHIFT))
                        {
                            if (IsKeyPressed(KEY_RIGHT)) layout.controls[selectedControl].rec.width += GRID_LINE_SPACING;
                            else if (IsKeyPressed(KEY_LEFT)) layout.controls[selectedControl].rec.width -= GRID_LINE_SPACING;

                            if (IsKeyPressed(KEY_UP)) layout.controls[selectedControl].rec.height -= GRID_LINE_SPACING;
                            else if (IsKeyPressed(KEY_DOWN)) layout.controls[selectedControl].rec.height += GRID_LINE_SPACING;
                        }
                        else
                        {
                            if (IsKeyDown(KEY_RIGHT) && ((framesCounterSnap%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.width += GRID_LINE_SPACING;
                            else if (IsKeyDown(KEY_LEFT) && ((framesCounterSnap%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.width -= GRID_LINE_SPACING;

                            if (IsKeyDown(KEY_UP) && ((framesCounterSnap%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.height -= GRID_LINE_SPACING;
                            else if (IsKeyDown(KEY_DOWN) && ((framesCounterSnap%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.height += GRID_LINE_SPACING;
                        }
                    }
                    else 
                    {
                        // Control modifier of position
                        if (IsKeyDown(KEY_LEFT_SHIFT))
                        {
                            if (IsKeyPressed(KEY_RIGHT)) layout.controls[selectedControl].rec.x += GRID_LINE_SPACING;
                            else if (IsKeyPressed(KEY_LEFT)) layout.controls[selectedControl].rec.x -= GRID_LINE_SPACING;
                            
                            if (IsKeyPressed(KEY_UP)) layout.controls[selectedControl].rec.y -= GRID_LINE_SPACING;
                            else if (IsKeyPressed(KEY_DOWN)) layout.controls[selectedControl].rec.y += GRID_LINE_SPACING;
                        }
                        else
                        {
                            if (IsKeyDown(KEY_RIGHT) && ((framesCounterSnap%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.x += GRID_LINE_SPACING;
                            else if (IsKeyDown(KEY_LEFT) && ((framesCounterSnap%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.x -= GRID_LINE_SPACING;
                            
                            if (IsKeyDown(KEY_UP) && ((framesCounterSnap%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.y -= GRID_LINE_SPACING;
                            else if (IsKeyDown(KEY_DOWN) && ((framesCounterSnap%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.y += GRID_LINE_SPACING;
                        }
                        
                    }
                }
                else
                {
                    if (IsKeyDown(KEY_LEFT_CONTROL))
                    {
                        // Control modifier for a more precise sizing                  
                        if (IsKeyDown(KEY_LEFT_SHIFT))
                        {
                            // Control modifier of position 
                            if (IsKeyPressed(KEY_RIGHT)) layout.controls[selectedControl].rec.width++;
                            else if (IsKeyPressed(KEY_LEFT)) layout.controls[selectedControl].rec.width--;
                            
                            if (IsKeyPressed(KEY_UP)) layout.controls[selectedControl].rec.height--;
                            else if (IsKeyPressed(KEY_DOWN)) layout.controls[selectedControl].rec.height++;
                        }
                        else
                        {
                            if (IsKeyDown(KEY_RIGHT)) layout.controls[selectedControl].rec.width++;
                            else if (IsKeyDown(KEY_LEFT)) layout.controls[selectedControl].rec.width--;

                            if (IsKeyDown(KEY_UP)) layout.controls[selectedControl].rec.height--;
                            else if (IsKeyDown(KEY_DOWN)) layout.controls[selectedControl].rec.height++;
                        }
                    }
                    else
                    {
                        if (IsKeyDown(KEY_LEFT_SHIFT))
                        {
                            // Control modifier for a more precise sizing
                            if (IsKeyPressed(KEY_RIGHT)) layout.controls[selectedControl].rec.x++;
                            else if (IsKeyPressed(KEY_LEFT)) layout.controls[selectedControl].rec.x--;

                            if (IsKeyPressed(KEY_UP)) layout.controls[selectedControl].rec.y--;
                            else if (IsKeyPressed(KEY_DOWN)) layout.controls[selectedControl].rec.y++;
                        }
                        else
                        {
                            if (IsKeyDown(KEY_RIGHT)) layout.controls[selectedControl].rec.x++;
                            else if (IsKeyDown(KEY_LEFT)) layout.controls[selectedControl].rec.x--;
                            
                            if (IsKeyDown(KEY_UP)) layout.controls[selectedControl].rec.y--;
                            else if (IsKeyDown(KEY_DOWN)) layout.controls[selectedControl].rec.y++;
                        }
                    }
                }
            }

            // Unlinks the control selected from its current anchor
            if (IsKeyPressed(KEY_U))
            {
                layout.controls[selectedControl].rec.x += layout.controls[selectedControl].ap->x;
                layout.controls[selectedControl].rec.y += layout.controls[selectedControl].ap->y;
                layout.controls[selectedControl].ap = &layout.anchors[0];
            }
        }

        // Updates the selectedType with the MouseWheel
        selectedType -= GetMouseWheelMove();
        
        if (selectedType < WINDOWBOX) selectedType = WINDOWBOX;
        else if (selectedType > DUMMYREC) selectedType = DUMMYREC;
        
        selectedTypeDraw = selectedType;
        
        if (focusedControl != -1)
        {
            // Delete focused control and shift array position
            if (IsKeyPressed(KEY_DELETE))
            {
                for (int i = focusedControl; i < layout.controlsCount; i++)
                {
                    layout.controls[i].type = layout.controls[i + 1].type;
                    layout.controls[i].rec = layout.controls[i + 1].rec;
                    memset(layout.controls[i].text, 0, MAX_CONTROL_TEXT_LENGTH);
                    memset(layout.controls[i].name, 0, MAX_CONTROL_NAME_LENGTH);
                    strcpy(layout.controls[i].text, layout.controls[i + 1].text);
                    strcpy(layout.controls[i].name, layout.controls[i + 1].name);
                    layout.controls[i].ap = layout.controls[i + 1].ap;
                }

                layout.controlsCount--;
                focusedControl = -1;
                selectedControl = -1;
            }
        }
        
        // Updates the defaultRec[selectedType] position
        defaultRec[selectedType].x = mouse.x - defaultRec[selectedType].width/2;
        defaultRec[selectedType].y = mouse.y - defaultRec[selectedType].height/2;

        // Mouse snap
        // NOTE: Snap point changes when GRID_LINE_SPACING/2 has been surpassed in X and Y
        if ((snapMode) && (selectedControl == -1))
        {
            int offsetX = (int)mouse.x%GRID_LINE_SPACING;
            int offsetY = (int)mouse.y%GRID_LINE_SPACING;
            
            if (offsetX >= GRID_LINE_SPACING/2) mouse.x += (GRID_LINE_SPACING - offsetX);
            else mouse.x -= offsetX;
            
            if (offsetY >= GRID_LINE_SPACING/2) mouse.y += (GRID_LINE_SPACING - offsetY);
            else mouse.y -= offsetY;
            
            // SnapMode of the DrawingControls
            // Snap rectangle position to closer snap point
            offsetX = (int)defaultRec[selectedType].x%GRID_LINE_SPACING;
            offsetY = (int)defaultRec[selectedType].y%GRID_LINE_SPACING;

            if (offsetX >= GRID_LINE_SPACING/2) defaultRec[selectedType].x += (GRID_LINE_SPACING - offsetX);
            else defaultRec[selectedType].x -= offsetX;

            if (offsetY >= GRID_LINE_SPACING/2) defaultRec[selectedType].y += (GRID_LINE_SPACING - offsetY);
            else defaultRec[selectedType].y -= offsetY;      
        }
        
        // Resize the controller aplying the snap
        if ((IsKeyDown(KEY_LEFT_CONTROL)) && (!textEditMode) && (!nameEditMode) && (IsKeyPressed(KEY_R)) && (selectedControl != -1))
        {
            int offsetX = (int)layout.controls[selectedControl].rec.width%GRID_LINE_SPACING;
            int offsetY = (int)layout.controls[selectedControl].rec.height%GRID_LINE_SPACING;
            
            if (offsetX >= GRID_LINE_SPACING/2) layout.controls[selectedControl].rec.width += (GRID_LINE_SPACING - offsetX);
            else layout.controls[selectedControl].rec.width -= offsetX;
            
            if (offsetY >= GRID_LINE_SPACING/2) layout.controls[selectedControl].rec.height += (GRID_LINE_SPACING - offsetY);
            else layout.controls[selectedControl].rec.height -= offsetY;
        }
        
        // Check if control has text to edit
        if (textEditMode)
        {
            // Locks the selectedControl for text editing
            selectedControl = storedControl;
            int key = GetKeyPressed();
            int keyCount = strlen(layout.controls[selectedControl].text); // Keeps track of text length
       
            // Replaces characters with pressed keys or '\0' in case of backspace
            // NOTE: Only allow keys in range [32..125]
            if (((key >= 32) && (key <= 125)) && (keyCount < MAX_CONTROL_TEXT_LENGTH - 1))
            {
                layout.controls[selectedControl].text[keyCount] = (unsigned char)key;
            }
            
            if ((keyCount > 0) && IsKeyPressed(KEY_BACKSPACE_TEXT))
            {
                layout.controls[selectedControl].text[keyCount - 1] = '\0';
                framesCounterSnap = 0;
                if (keyCount < 0) keyCount = 0;
            }
            else if ((keyCount > 0) && IsKeyDown(KEY_BACKSPACE_TEXT))
            {
                if ((framesCounterSnap > 60) && (framesCounterSnap%4) == 0) layout.controls[selectedControl].text[keyCount - 1] = '\0';
                if (keyCount < 0) keyCount = 0;
            }
            
            // Used to show the cursor('|') in textEditMode 
            if (keyCount < MAX_CONTROL_TEXT_LENGTH) framesCounter++;
            else if (keyCount == MAX_CONTROL_TEXT_LENGTH) framesCounter = 21;
        }
        
        if ((nameEditMode))
        {
            // Locks the selectedControl for text editing
            selectedControl = storedControl;
            int key = GetKeyPressed();
            int keyCount = strlen(layout.controls[selectedControl].name); // Keeps track of name length
       
            // Replaces characters with pressed keys or '\0' in case of backspace
            // NOTE: Only allow keys in range [48..57], [65..90] and [97..122]
            if ((((key >= 48) && (key <= 57)) || ((key >= 65) && (key <= 90)) || ((key >= 97) && (key <= 122))) && (keyCount < MAX_CONTROL_NAME_LENGTH - 1))
            {
                layout.controls[selectedControl].name[keyCount] = (unsigned char)key;
            }
            
            if ((keyCount > 0) && IsKeyPressed(KEY_BACKSPACE_TEXT))
            {
                layout.controls[selectedControl].name[keyCount - 1] = '\0';
                if (keyCount < 0) keyCount = 0;
            }
            
            // Used to show the cursor('|') in textEditMode 
            if (keyCount < MAX_CONTROL_NAME_LENGTH) framesCounter++;
            else if (keyCount == MAX_CONTROL_NAME_LENGTH) framesCounter = 21;
        }
        
        // Turns off textEditMode
        if (textEditMode && IsKeyPressed(KEY_ENTER)) 
        {
            textEditMode = false;
            framesCounter = 0;
        }
        else if (textEditMode && IsKeyPressed(KEY_ESCAPE)) 
        {
            textEditMode = false;
            memset(layout.controls[selectedControl].text, 0, MAX_CONTROL_TEXT_LENGTH);
            strcpy(layout.controls[selectedControl].text, prevControlText);
            framesCounter = 0;
        }
        
        if (nameEditMode && IsKeyPressed(KEY_ENTER)) 
        {
            nameEditMode = false;
            framesCounter = 0;
        }
        else if (nameEditMode && IsKeyPressed(KEY_ESCAPE)) 
        {
            nameEditMode = false;
            memset(layout.controls[selectedControl].name, 0, MAX_CONTROL_NAME_LENGTH);
            strcpy(layout.controls[selectedControl].name, prevControlName);
            framesCounter = 0;
        }
        
        // Turns on textEditMode
        if (IsKeyPressed(KEY_T) && !nameEditMode && (selectedControl != -1) && (!generateWindowActive) && (!anchorMode) &&
           ((layout.controls[selectedControl].type == LABEL) || (layout.controls[selectedControl].type == CHECKBOX) || (layout.controls[selectedControl].type == SLIDERBAR) || (layout.controls[selectedControl].type == SLIDER) || (layout.controls[selectedControl].type == TEXTBOX) || (layout.controls[selectedControl].type == BUTTON) || (layout.controls[selectedControl].type == TOGGLE) || (layout.controls[selectedControl].type == GROUPBOX) || (layout.controls[selectedControl].type == WINDOWBOX) || (layout.controls[selectedControl].type == STATUSBAR) || (layout.controls[selectedControl].type == DUMMYREC)))
        {   
            textEditMode = true;
            storedControl = selectedControl;
            strcpy(prevControlText, layout.controls[selectedControl].text);
        }
        
        // Turns on NameEditMode
        if (IsKeyPressed(KEY_N) && (!IsKeyDown(KEY_LEFT_CONTROL)) && (!resetWindowActive) && !textEditMode && (selectedControl != -1) && (!generateWindowActive))
        {
            nameEditMode = true;
            strcpy(prevControlName, layout.controls[selectedControl].name);
            storedControl = selectedControl;
        }

        // Checks if mouse is over an anchor
        if (!(anchorLinkMode || controlDrag || anchorLockMode))
        {
            for (int i = 1; i < MAX_ANCHOR_POINTS; i++)
            {
                if (CheckCollisionPointCircle(mouse, (Vector2){ layout.anchors[i].x, layout.anchors[i].y }, ANCHOR_RADIUS))
                {
                    selectedAnchor = i;
                    if (layout.anchors[selectedAnchor].enabled) anchorMode = true;
                    break;
                }
                else 
                {
                    selectedAnchor = -1;
                    if (!IsKeyDown(KEY_A)) anchorMode = false;
                }
            }
        }

        // Selected control lock logic
        if (anchorLockMode)
        {
            selectedAnchor = storedAnchor;
            anchorMode = true;
            
            if (snapMode)
            {
                if (IsKeyDown(KEY_LEFT_SHIFT))
                {
                    if (IsKeyPressed(KEY_RIGHT)) layout.anchors[selectedAnchor].x+= GRID_LINE_SPACING;
                    else if (IsKeyPressed(KEY_LEFT)) layout.anchors[selectedAnchor].x-= GRID_LINE_SPACING;
                    
                    if (IsKeyPressed(KEY_UP)) layout.anchors[selectedAnchor].y-= GRID_LINE_SPACING;
                    else if (IsKeyPressed(KEY_DOWN)) layout.anchors[selectedAnchor].y+= GRID_LINE_SPACING;
                }
                else
                {
                    if (IsKeyDown(KEY_RIGHT) && ((framesCounterSnap%MOVEMENT_FRAME_SPEED) == 0)) layout.anchors[selectedAnchor].x+= GRID_LINE_SPACING;
                    else if (IsKeyDown(KEY_LEFT) && ((framesCounterSnap%MOVEMENT_FRAME_SPEED) == 0)) layout.anchors[selectedAnchor].x-= GRID_LINE_SPACING;
                    
                    if (IsKeyDown(KEY_UP) && ((framesCounterSnap%MOVEMENT_FRAME_SPEED) == 0)) layout.anchors[selectedAnchor].y-= GRID_LINE_SPACING;
                    else if (IsKeyDown(KEY_DOWN) && ((framesCounterSnap%MOVEMENT_FRAME_SPEED) == 0)) layout.anchors[selectedAnchor].y+= GRID_LINE_SPACING;
                }
            }
            else
            {
                if (IsKeyDown(KEY_LEFT_SHIFT))
                {
                    if (IsKeyPressed(KEY_RIGHT)) layout.anchors[selectedAnchor].x++;
                    else if (IsKeyPressed(KEY_LEFT)) layout.anchors[selectedAnchor].x--;
                    
                    if (IsKeyPressed(KEY_UP)) layout.anchors[selectedAnchor].y--;
                    else if (IsKeyPressed(KEY_DOWN)) layout.anchors[selectedAnchor].y++;
                }
                else
                {
                    if (IsKeyDown(KEY_RIGHT)) layout.anchors[selectedAnchor].x++;
                    else if (IsKeyDown(KEY_LEFT)) layout.anchors[selectedAnchor].x--;
                    
                    if (IsKeyDown(KEY_UP)) layout.anchors[selectedAnchor].y--;
                    else if (IsKeyDown(KEY_DOWN)) layout.anchors[selectedAnchor].y++;
                }
            }
        }

        // Create and edit anchor points
        if (anchorMode && (!generateWindowActive) && (!resetWindowActive) && (!closingWindowActive))
        {
            // On mouse click anchor is created
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (selectedAnchor == -1) && (selectedControl == -1))
            {
                for (int i = 1; i < MAX_ANCHOR_POINTS; i++)
                {
                    if (!layout.anchors[i].enabled)
                    {
                        layout.anchors[i].x = mouse.x;
                        layout.anchors[i].y = mouse.y;
                        layout.anchors[i].enabled = true;
                        anchorMode = false;
                        break;
                    }
                }
            }

            if (IsKeyPressed(KEY_SPACE) && !nameEditMode && !textEditMode && (selectedAnchor != -1) && !anchorLockMode && anchorMode) 
            {
                anchorLockMode = true;
                storedAnchor = selectedAnchor;
            }
            else if (IsKeyPressed(KEY_SPACE) && (selectedAnchor != -1)) anchorLockMode = false;
            
            if (selectedAnchor > 0)
            {
                // Unlinks and deletes the selected anchor point
                if (IsKeyPressed(KEY_DELETE) && (!anchorLinkMode))
                {
                    for (int i = 0; i < layout.controlsCount; i++)
                    {
                        if (layout.controls[i].ap->id == selectedAnchor) 
                        {
                            layout.controls[i].rec.x += layout.controls[i].ap->x;
                            layout.controls[i].rec.y += layout.controls[i].ap->y;
                            layout.controls[i].ap = &layout.anchors[0];
                        }
                    }
                    layout.anchors[selectedAnchor].x = 0;
                    layout.anchors[selectedAnchor].y = 0;
                    layout.anchors[selectedAnchor].enabled = false;
                    layout.anchors[selectedAnchor].hidding = false;
                    anchorMode = false;
                    anchorLockMode = false;
                }
                
                if (!anchorLockMode && (!anchorLinkMode))
                {
                    // Allows to drag an anchor without losing collision
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && IsKeyDown(KEY_A))
                    {
                        controlDrag = true;
                        anchorPosEditMode = true;
                        
                        for (int i = 0; i < layout.controlsCount; i++)
                        {
                            if (layout.controls[i].ap->id == selectedAnchor) 
                            {
                                layout.controls[i].rec.x += layout.controls[i].ap->x;
                                layout.controls[i].rec.y += layout.controls[i].ap->y;
                                layout.controls[i].ap = &auxAnchor;
                            }
                        } 
                    }
                    else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) controlDrag = true;
                    
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                    {     
                        if (anchorPosEditMode)
                        {
                            for (int i = 0; i < layout.controlsCount; i++)
                            {
                                if (layout.controls[i].ap->id == 9) 
                                {
                                    layout.controls[i].rec.x += layout.controls[i].ap->x;
                                    layout.controls[i].rec.y += layout.controls[i].ap->y;
                                    layout.controls[i].ap = &layout.anchors[selectedAnchor];
                                    layout.controls[i].rec.x -= layout.anchors[selectedAnchor].x;
                                    layout.controls[i].rec.y -= layout.anchors[selectedAnchor].y;
                                }
                            } 
                            
                            anchorPosEditMode = false;
                        }
                        
                        controlDrag = false;
                        selectedAnchor = -1;
                        anchorMode = false;
                    }
                    
                    // Moves the anchor to the mouse position
                    if (controlDrag)
                    {
                        layout.anchors[selectedAnchor].x = mouse.x;
                        layout.anchors[selectedAnchor].y = mouse.y;
                    }
                }
                
                // Enables the linking between anchor and control
                if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
                {
                    linkedAnchor = selectedAnchor;
                    anchorLinkMode = true;
                }
                
                // Links the selected control to the current anchor
                if (IsMouseButtonReleased(MOUSE_RIGHT_BUTTON))
                {
                    if (focusedControl != -1)
                    {
                        layout.controls[focusedControl].rec.x += layout.controls[focusedControl].ap->x;
                        layout.controls[focusedControl].rec.y += layout.controls[focusedControl].ap->y;
                        layout.controls[focusedControl].ap = &layout.anchors[linkedAnchor];
                        layout.controls[focusedControl].rec.x -= layout.anchors[linkedAnchor].x;
                        layout.controls[focusedControl].rec.y -= layout.anchors[linkedAnchor].y;
                    }
                    
                    anchorLinkMode = false;  
                    anchorMode = false;                    
                }
                
                // Hide/Unhide selected anchor linked controls
                if (IsKeyPressed(KEY_H) && (!anchorLinkMode)) layout.anchors[selectedAnchor].hidding = !layout.anchors[selectedAnchor].hidding;
            }
        }

        // Enable anchor mode editing
        if (IsKeyDown(KEY_A) && !nameEditMode && !textEditMode && (layout.controls[selectedControl].type != TEXTBOX)) anchorMode = true;

        // Checks the minimum size of the rec
        if (selectedControl != -1)
        {
            // Sets the minimum limit of the width
            if (layout.controls[selectedControl].type == LABEL || layout.controls[selectedControl].type == BUTTON || layout.controls[selectedControl].type == TOGGLE || layout.controls[selectedControl].type == TEXTBOX)
            {
                if (layout.controls[selectedControl].rec.width <  MeasureText(layout.controls[selectedControl].text , style[DEFAULT_TEXT_SIZE])) layout.controls[selectedControl].rec.width = MeasureText(layout.controls[selectedControl].text , style[DEFAULT_TEXT_SIZE]);
            }
            else if (layout.controls[selectedControl].type == WINDOWBOX || layout.controls[selectedControl].type == GROUPBOX || layout.controls[selectedControl].type == STATUSBAR) 
            {
                if (layout.controls[selectedControl].rec.width <  MeasureText(layout.controls[selectedControl].text, style[DEFAULT_TEXT_SIZE]) + 31) layout.controls[selectedControl].rec.width = MeasureText(layout.controls[selectedControl].text , style[DEFAULT_TEXT_SIZE]) + 31;
            }
            else if (layout.controls[selectedControl].type == CHECKBOX)
            {
                if (layout.controls[selectedControl].rec.width <= 10) layout.controls[selectedControl].rec.width = 10;
            }
            else if (layout.controls[selectedControl].rec.width <= 20) layout.controls[selectedControl].rec.width = 20;
            
            // Sets the minimum limit of the height
            if (layout.controls[selectedControl].type == WINDOWBOX) 
            { 
                if (layout.controls[selectedControl].rec.height < 50) layout.controls[selectedControl].rec.height = 50;
            }
            else if (layout.controls[selectedControl].type == PROGRESSBAR || layout.controls[selectedControl].type == SLIDER || layout.controls[selectedControl].type == SLIDERBAR || layout.controls[selectedControl].type == CHECKBOX || layout.controls[selectedControl].type == LINE)
            {
                if (layout.controls[selectedControl].rec.height <= 10 ) layout.controls[selectedControl].rec.height = 10;
            }
            else if (layout.controls[selectedControl].rec.height <= 20) layout.controls[selectedControl].rec.height = 20;
        }
  
        // Shows or hides the grid
        if (IsKeyPressed(KEY_G) && (!nameEditMode) && (!textEditMode) && (!generateWindowActive)) showGrid = !showGrid;
        
        // Drop files logic
        if (IsFileDropped())
        {
            int fileCount = 0;
            char **droppedFiles = { 0 };
            char droppedFileName[256]; 
            droppedFiles = GetDroppedFiles(&fileCount);
            strcpy(droppedFileName, droppedFiles[0]);
            
            if (IsFileExtension(droppedFileName, ".rgl")) 
            {
                selectedControl = -1;
                LoadLayoutRGL(droppedFileName);
                strcpy(loadedFileName, droppedFileName);
                SetWindowTitle(FormatText("rGuiLayout v1.0 - %s", GetFileName(loadedFileName)));
            }
            else if (IsFileExtension(droppedFileName, ".rgs")) GuiLoadStyle(droppedFileName);
            else if (IsFileExtension(droppedFileName, ".png"))
            {
                if (loadedTexture.id > 0) UnloadTexture(loadedTexture);
                loadedTexture = LoadTexture(droppedFileName);
                
                if (loadedTexture.width == 64 && loadedTexture.height == 16) GuiLoadStyleImage(droppedFileName);
                else
                {
                    if (tracemap.id > 0) UnloadTexture(tracemap);
                    tracemap = LoadTexture(droppedFileName);
                }
                
                UnloadTexture(loadedTexture);
                
                SetTextureFilter(tracemap, FILTER_BILINEAR);
                
                tracemapRec.width = tracemap.width;
                tracemapRec.height = tracemap.height; 
            }

            ClearDroppedFiles();
        }
        
        // Duplicate selected control
        if ((IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_D)) && (selectedControl != -1) && !anchorMode)
        {
            // Add a copy of selected control 
            layout.controls[layout.controlsCount].id = layout.controlsCount;
            layout.controls[layout.controlsCount].type = layout.controls[selectedControl].type;
            layout.controls[layout.controlsCount].rec = layout.controls[selectedControl].rec;
            layout.controls[layout.controlsCount].rec.x += 10;
            layout.controls[layout.controlsCount].rec.y += 10;
            strcpy(layout.controls[layout.controlsCount].text, layout.controls[selectedControl].text);
            strcpy(layout.controls[layout.controlsCount].name, FormatText("%s%03i", controlTypeNameLow[layout.controls[layout.controlsCount].type], layout.controlsCount));
            layout.controls[layout.controlsCount].ap = layout.controls[selectedControl].ap;            // Default anchor point (0, 0)
            
            layout.controlsCount++;
        }
     
        // Save layout file dialog logic
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_LEFT_SHIFT) && IsKeyPressed(KEY_S)) ShowSaveLayoutDialog();
        else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S)) 
        {
            if (loadedFileName[0] == '\0') ShowSaveLayoutDialog();
            else SaveLayoutRGL(loadedFileName, false);
        }
        
        // Open laout file dialog logic
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O))
        {
            // Open file dialog
            const char *filters[] = { "*.rgl" };
            const char *fileName = tinyfd_openFileDialog("Load raygui layout file", "", 1, filters, "raygui Layout Files (*.rgl)", 0);
            
            if (fileName != NULL) LoadLayoutRGL(fileName);
        }
        
        // Activate code generation export window
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_ENTER) && !closingWindowActive) generateWindowActive = true;

        if (generateWindowActive)   // Keep window in the middle of screen
        {
            exportWindowPos.x = GetScreenWidth()/2 - 200;
            exportWindowPos.y = GetScreenHeight()/2 - 112;
        }
        
        // Tracemap texture control logic
        if (tracemap.id > 0)
        {
            // Toggles Texture editting mode between true or false
            if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_T)) tracemapEditMode = !tracemapEditMode;
            
            if (tracemapEditMode)
            {
                int offsetX = (int)mouse.x%GRID_LINE_SPACING;
                int offsetY = (int)mouse.y%GRID_LINE_SPACING;
                
                // Moves the texture with the mouse
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    panControlOffset = mouse;
                    prevControlPosition = (Vector2){ tracemapRec.x, tracemapRec.y };
                }
                
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                {
                    tracemapRec.x = prevControlPosition.x + (mouse.x - panControlOffset.x);
                    tracemapRec.y = prevControlPosition.y + (mouse.y - panControlOffset.y);
                        
                    if (snapMode)
                    {
                        if (offsetX >= GRID_LINE_SPACING/2) mouse.x += (GRID_LINE_SPACING - offsetX);
                        else mouse.x -= offsetX;
                        
                        if (offsetY >= GRID_LINE_SPACING/2) mouse.y += (GRID_LINE_SPACING - offsetY);
                        else mouse.y -= offsetY;
                        
                        offsetX = (int)tracemapRec.x%GRID_LINE_SPACING;
                        offsetY = (int)tracemapRec.y%GRID_LINE_SPACING;
                        
                        if (offsetX >= GRID_LINE_SPACING/2) tracemapRec.x += (GRID_LINE_SPACING - offsetX);
                        else tracemapRec.x -= offsetX;
                        
                        if (offsetY >= GRID_LINE_SPACING/2) tracemapRec.y += (GRID_LINE_SPACING - offsetY);
                        else tracemapRec.y -= offsetY;
                    }
                }  

                // Moves and scales the texture with snap.
                if (IsKeyDown(KEY_LEFT_CONTROL))
                {
                    tracemapRec.height -= GetMouseWheelMove();
                    tracemapRec.width -= GetMouseWheelMove();
                }
                else 
                {
                    tracemapRec.height -= 10*GetMouseWheelMove();
                    tracemapRec.width -= 10*GetMouseWheelMove();
                }
                
                tracemap.height = tracemapRec.height;
                tracemap.width = tracemapRec.width;
                
                // Change texture fade
                if (IsKeyDown(KEY_LEFT_CONTROL))
                {
                    if (IsKeyDown(KEY_LEFT_SHIFT))
                    {
                        if (IsKeyPressed(KEY_LEFT)) tracemapRec.x--;
                        else if (IsKeyPressed(KEY_RIGHT)) tracemapRec.x++;
                        
                        if (IsKeyPressed(KEY_UP)) tracemapRec.y--;
                        else if (IsKeyPressed(KEY_DOWN)) tracemapRec.y++;
                    }
                    else
                    {
                        if (IsKeyDown(KEY_LEFT)) tracemapRec.x--;
                        else if (IsKeyDown(KEY_RIGHT)) tracemapRec.x++;
                        
                        if (IsKeyDown(KEY_UP)) tracemapRec.y--;
                        else if (IsKeyDown(KEY_DOWN)) tracemapRec.y++;
                    }
                }
                else
                {
                    if (IsKeyDown(KEY_LEFT)) tracemapFade-= 0.01f;
                    else if (IsKeyDown(KEY_RIGHT)) tracemapFade+=0.01f; 
                }
                
                if (tracemapFade < 0) tracemapFade = 0;
                else if (tracemapFade > 1) tracemapFade = 1;
                
                // Deletes the texture and resets it
                if (IsKeyPressed(KEY_DELETE))
                {
                    UnloadTexture(tracemap);
                    tracemap.id = 0;
                    tracemapEditMode = false;
                    tracemapRec.x = 0;
                    tracemapRec.y = 0;
                }
            }
        }
        
        if ((IsKeyDown(KEY_LEFT_CONTROL)) && (IsKeyPressed(KEY_Z))) layout.controls[undoSelectedControl].rec = undoLastRec;
        
        if ((IsKeyDown(KEY_LEFT_CONTROL)) && (IsKeyPressed(KEY_N)) && (!generateWindowActive) && (!closingWindowActive))
        {
            resetWindowActive = true;
            resetLayout = false;
        }
        
        if (resetWindowActive && resetLayout)
        {
            // Resets all controls to default values
            for (int i = selectedControl; i < layout.controlsCount; i++)
            {
                layout.controls[i].id = 0;
                layout.controls[i].type = 0;
                layout.controls[i].rec = (Rectangle){ 0, 0, 0, 0 };
                memset(layout.controls[i].text, 0, MAX_CONTROL_TEXT_LENGTH);
                memset(layout.controls[i].name, 0, MAX_CONTROL_NAME_LENGTH);
                layout.controls[i].ap = &layout.anchors[0];  // By default, set parent anchor
            }
            
            // Resets anchor points to default values
            for (int i = 0; i < MAX_ANCHOR_POINTS; i++)
            {
                layout.anchors[i].x = 0;
                layout.anchors[i].y = 0;
                layout.anchors[i].enabled = false;
                layout.anchors[i].hidding = false;
            }
            
            SetWindowTitle("rGuiLayout v1.0");
            strcpy(loadedFileName, "\0");
            layout.controlsCount = 0;
            resetWindowActive = false;
        }
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            // TODO: Draw global app screen limits (black rectangle with black default anchor)

            if (showGrid) GuiGrid((Rectangle){ 0, 0, GetScreenWidth(), GetScreenHeight() }, GRID_LINE_SPACING, 5, false);
            
            // Draw the texture if loaded
            if (tracemap.id > 0)
            {
                DrawTexture(tracemap, tracemapRec.x, tracemapRec.y, Fade(WHITE, tracemapFade));
                
                // Draw the tracemap rectangle
                if (tracemapEditMode) DrawRectangleLines(tracemapRec.x, tracemapRec.y, tracemapRec.width, tracemapRec.height, RED);
                else DrawRectangleLines(tracemapRec.x, tracemapRec.y, tracemapRec.width, tracemapRec.height, GRAY);
            }
            
            for (int i = 0; i < layout.controlsCount; i++)
            {
                // Draws the Controls when placed on the grid.
                if (!layout.controls[i].ap->hidding)
                {
                    switch (layout.controls[i].type)
                    {
                        case WINDOWBOX: 
                        {
                            GuiFade(0.8f);
                            GuiWindowBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text);
                            GuiFade(1.0f);
                        }break;
                        case GROUPBOX: GuiGroupBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text); break;
                        case LINE: GuiLine((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, 1); break;
                        case PANEL: 
                        {
                            GuiFade(0.8f);
                            GuiPanel((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height });
                            GuiFade(1.0f);
                        }break;
                        case LABEL: GuiLabel((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text); break;
                        case BUTTON: GuiButton((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text); break;
                        case TOGGLE: GuiToggleButton((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text, false); break;
                        case TOGGLEGROUP: GuiToggleGroup((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, listData, 3, 1); break;
                        case CHECKBOX: GuiCheckBoxEx((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, false, layout.controls[i].text); break;
                        case COMBOBOX: GuiComboBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, listData, 3, 1); break;
                        case DROPDOWNBOX: GuiDropdownBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, listData, 3, 2); break;
                        case SPINNER: GuiSpinner((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, 42, 3, 25); break;
                        case VALUEBOX: GuiValueBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, 42, 100); break;
                        case TEXTBOX: GuiTextBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text, MAX_CONTROL_TEXT_LENGTH, false); break;
                        case SLIDER: GuiSliderEx((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, 42, 0, 100, layout.controls[i].text, true); break;
                        case SLIDERBAR: GuiSliderBarEx((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, 40, 0, 100, layout.controls[i].text, true); break;
                        case PROGRESSBAR: GuiProgressBarEx((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, 40, 0, 100, true); break;
                        case STATUSBAR: GuiStatusBar((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text, 15); break;
                        case LISTVIEW: GuiListView((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, listViewData, 4, 1); break;
                        case COLORPICKER: GuiColorPicker((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, RED); break;
                        case DUMMYREC: GuiDummyRec((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text); break;
                        default: break;
                    }
                    
                    if ((layout.controls[i].ap->id == selectedAnchor) && (layout.controls[i].ap->id > 0)) DrawLine(layout.controls[i].ap->x, layout.controls[i].ap->y, layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, RED);
                    
                }
                else if ((layout.controls[i].ap->id == selectedAnchor) && (layout.controls[i].ap->id > 0)) DrawLine(layout.controls[i].ap->x, layout.controls[i].ap->y, layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, BLUE);
            }
            
            // Draws the defaultRec[selectedType] of the control selected
            if ((selectedControl == -1) && (focusedControl == -1 )&& !anchorMode && !tracemapEditMode && !closingWindowActive && !generateWindowActive)
            {
                GuiFade(0.5f);
                switch (selectedTypeDraw)
                {
                    case WINDOWBOX: GuiWindowBox(defaultRec[selectedTypeDraw], "WINDOW BOX"); break;
                    case GROUPBOX: GuiGroupBox(defaultRec[selectedTypeDraw], "GROUP BOX"); break;
                    case LINE: GuiLine(defaultRec[selectedTypeDraw], 1); break;
                    case PANEL: GuiPanel(defaultRec[selectedTypeDraw]); break;
                    case LABEL: GuiLabel(defaultRec[selectedTypeDraw], "TEXT SAMPLE"); break;
                    case BUTTON: GuiButton(defaultRec[selectedTypeDraw], "BUTTON"); break;
                    case TOGGLE: GuiToggleButton(defaultRec[selectedTypeDraw], "TOGGLE", false); break;
                    case TOGGLEGROUP: GuiToggleGroup(defaultRec[selectedTypeDraw], listData, 3, 1); break;
                    case CHECKBOX: GuiCheckBoxEx(defaultRec[selectedTypeDraw], false, "TEXT SAMPLE"); break;
                    case COMBOBOX: GuiComboBox(defaultRec[selectedTypeDraw], listData, 3, 1); break;
                    case DROPDOWNBOX: GuiDropdownBox(defaultRec[selectedTypeDraw], listData, 3, 2); break;
                    case SPINNER: GuiSpinner(defaultRec[selectedTypeDraw], 42, 3, 25); break;
                    case VALUEBOX: GuiValueBox(defaultRec[selectedTypeDraw], 42, 100); break;
                    case TEXTBOX: GuiTextBox(defaultRec[selectedTypeDraw], "TEXTBOX", 7, false); break;
                    case SLIDER: GuiSliderEx(defaultRec[selectedTypeDraw], 42, 0, 100, "TEXT SAMPLE", true); break;
                    case SLIDERBAR: GuiSliderBarEx(defaultRec[selectedTypeDraw], 40, 0, 100, "TEXT SAMPLE", true); break;
                    case PROGRESSBAR: GuiProgressBarEx(defaultRec[selectedTypeDraw], 40, 0, 100, true); break;
                    case STATUSBAR: GuiStatusBar(defaultRec[selectedTypeDraw], "STATUS BAR", 15); break;
                    case LISTVIEW: GuiListView(defaultRec[selectedTypeDraw], listViewData, 4, 1); break;
                    case COLORPICKER: GuiColorPicker(defaultRec[selectedTypeDraw], RED); break;
                    case DUMMYREC: GuiDummyRec(defaultRec[selectedTypeDraw], "DUMMY REC"); break;
                    default: break;
                }
                
                GuiFade(1.0f);
            }

            // Draw the anchorPoints
            for (int i = 0; i < MAX_ANCHOR_POINTS; i++)
            {
                if ((layout.anchors[i].enabled) && (layout.anchors[i].x != 0) && (layout.anchors[i].y != 0))
                {
                    if (layout.anchors[i].id == selectedAnchor && anchorPosEditMode)
                    {
                        // Draw the anchor that is currently moving
                        DrawCircle(layout.anchors[i].x, layout.anchors[i].y, ANCHOR_RADIUS, Fade(ORANGE, 0.5f));
                        DrawRectangle(layout.anchors[i].x - ANCHOR_RADIUS - 5, layout.anchors[i].y, ANCHOR_RADIUS*2 + 10, 1, ORANGE);
                        DrawRectangle(layout.anchors[i].x, layout.anchors[i].y - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, ORANGE);
                    }
                    if (layout.anchors[i].id == selectedAnchor && IsKeyDown(KEY_A))
                    {
                        // Draw the anchor that is currently moving
                        DrawCircle(layout.anchors[i].x, layout.anchors[i].y, ANCHOR_RADIUS, Fade(ORANGE, 0.5f));
                        DrawRectangle(layout.anchors[i].x - ANCHOR_RADIUS - 5, layout.anchors[i].y, ANCHOR_RADIUS*2 + 10, 1, ORANGE);
                        DrawRectangle(layout.anchors[i].x, layout.anchors[i].y - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, ORANGE);
                    }
                    else if (layout.anchors[i].hidding && layout.anchors[i].id == selectedAnchor)
                    {
                        // Draw idle anchor
                        DrawCircle(layout.anchors[i].x, layout.anchors[i].y, ANCHOR_RADIUS, Fade(BLUE, 0.5f));
                        DrawRectangle(layout.anchors[i].x - ANCHOR_RADIUS - 5, layout.anchors[i].y, ANCHOR_RADIUS*2 + 10, 1, BLUE);
                        DrawRectangle(layout.anchors[i].x, layout.anchors[i].y - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, BLUE);
                        DrawText(FormatText("[%i, %i]", layout.anchors[i].x, layout.anchors[i].y), layout.anchors[i].x, layout.anchors[i].y - 25, 20, BLUE);
                    }
                    else if (layout.anchors[i].id == selectedAnchor)
                    {
                        // Draw the selected anchor
                        DrawCircle(layout.anchors[i].x, layout.anchors[i].y, ANCHOR_RADIUS, Fade(RED, 0.5f));
                        DrawRectangle(layout.anchors[i].x - ANCHOR_RADIUS - 5, layout.anchors[i].y, ANCHOR_RADIUS*2 + 10, 1, RED);
                        DrawRectangle(layout.anchors[i].x, layout.anchors[i].y - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, RED);
                        DrawText(FormatText("[%i, %i]", layout.anchors[i].x, layout.anchors[i].y), layout.anchors[i].x, layout.anchors[i].y - 25, 20, RED);
                    } 
                    else if (layout.anchors[i].hidding)
                    {
                        // Draw idle anchor
                        DrawCircleLines(layout.anchors[i].x, layout.anchors[i].y, ANCHOR_RADIUS, Fade(BLUE, 0.5f));
                        DrawRectangle(layout.anchors[i].x - ANCHOR_RADIUS - 5, layout.anchors[i].y, ANCHOR_RADIUS*2 + 10, 1, BLUE);
                        DrawRectangle(layout.anchors[i].x, layout.anchors[i].y - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, BLUE);
                    }
                    else
                    {
                        // Draw idle anchor
                        DrawCircleLines(layout.anchors[i].x, layout.anchors[i].y, ANCHOR_RADIUS, Fade(RED, 0.5f));
                        DrawRectangle(layout.anchors[i].x - ANCHOR_RADIUS - 5, layout.anchors[i].y, ANCHOR_RADIUS*2 + 10, 1, RED);
                        DrawRectangle(layout.anchors[i].x, layout.anchors[i].y - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, RED);
                    }
                }
            }
            
            // Draw selected control selection rectangle (transparent RED/WHITE)
            if (((selectedControl != -1) || (focusedControl != -1)) && (selectedControl < layout.controlsCount))
            {
                if (selectedControl != -1) DrawRectangleRec((Rectangle){ layout.controls[selectedControl].ap->x + layout.controls[selectedControl].rec.x, layout.controls[selectedControl].ap->y + layout.controls[selectedControl].rec.y, layout.controls[selectedControl].rec.width, layout.controls[selectedControl].rec.height }, (nameEditMode) ? Fade(WHITE, 0.7f) : Fade(RED, 0.5f));
                
                if (focusedControl != -1) DrawRectangleLinesEx((Rectangle){ layout.controls[focusedControl].ap->x + layout.controls[focusedControl].rec.x, layout.controls[focusedControl].ap->y + layout.controls[focusedControl].rec.y, layout.controls[focusedControl].rec.width, layout.controls[focusedControl].rec.height }, 1, RED);
            }
            
            // Draw anchor lines
            if ((selectedControl != -1) && layout.controls[selectedControl].ap->id > 0 && !layout.controls[selectedControl].ap->hidding) DrawLine(layout.controls[selectedControl].ap->x, layout.controls[selectedControl].ap->y, layout.controls[selectedControl].ap->x + layout.controls[selectedControl].rec.x, layout.controls[selectedControl].ap->y + layout.controls[selectedControl].rec.y, RED);
            else if ((selectedControl != -1) && layout.controls[selectedControl].ap->id > 0 && layout.controls[selectedControl].ap->hidding) DrawLine(layout.controls[selectedControl].ap->x, layout.controls[selectedControl].ap->y, layout.controls[selectedControl].ap->x + layout.controls[selectedControl].rec.x, layout.controls[selectedControl].ap->y + layout.controls[selectedControl].rec.y, BLUE);
            
            if ((focusedControl != -1) && layout.controls[focusedControl].ap->id > 0 && !layout.controls[focusedControl].ap->hidding) DrawLine(layout.controls[focusedControl].ap->x, layout.controls[focusedControl].ap->y, layout.controls[focusedControl].ap->x + layout.controls[focusedControl].rec.x, layout.controls[focusedControl].ap->y + layout.controls[focusedControl].rec.y, RED);
            else if ((focusedControl != -1) && layout.controls[focusedControl].ap->id > 0 && layout.controls[focusedControl].ap->hidding) DrawLine(layout.controls[focusedControl].ap->x, layout.controls[focusedControl].ap->y, layout.controls[focusedControl].ap->x + layout.controls[focusedControl].rec.x, layout.controls[focusedControl].ap->y + layout.controls[focusedControl].rec.y, BLUE);
                
            // Draw cursor (control mode or anchor mode)
            if ((selectedControl == -1)  && (selectedAnchor == -1))
            {
                if (anchorMode)
                {
                    DrawCircleLines(mouse.x, mouse.y, ANCHOR_RADIUS, Fade(RED, 0.5f));
                    DrawRectangle(mouse.x - ANCHOR_RADIUS - 5, mouse.y, ANCHOR_RADIUS*2 + 10, 1, RED);
                    DrawRectangle(mouse.x , mouse.y - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, RED);
                }
                else 
                {
                    DrawRectangle(mouse.x - 8, mouse.y, 17, 1, RED);
                    DrawRectangle(mouse.x, mouse.y - 8, 1, 17, RED);
                }
            }

            // Draw cursor on textEditMode
            if (textEditMode)
            {
               if (((framesCounter/20)%2) == 0)
               {
                    if (layout.controls[selectedControl].type == LABEL) DrawText("|", layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x + MeasureText(layout.controls[selectedControl].text, style[DEFAULT_TEXT_SIZE]), layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y - style[DEFAULT_TEXT_SIZE]/2 + layout.controls[selectedControl].rec.height/2, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else if (layout.controls[selectedControl].type == TEXTBOX) DrawText("|", layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x + MeasureText(layout.controls[selectedControl].text, style[DEFAULT_TEXT_SIZE]) + 4, layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y - style[DEFAULT_TEXT_SIZE]/2 + layout.controls[selectedControl].rec.height/2, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else if (layout.controls[selectedControl].type == GROUPBOX) DrawText("|", layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x + 15 + MeasureText(layout.controls[selectedControl].text, style[DEFAULT_TEXT_SIZE]), layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else if (layout.controls[selectedControl].type == WINDOWBOX) DrawText("|", layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x + 10 + MeasureText(layout.controls[selectedControl].text, style[DEFAULT_TEXT_SIZE]), layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y + style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else if (layout.controls[selectedControl].type == STATUSBAR) DrawText("|", layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x + 15 + MeasureText(layout.controls[selectedControl].text, style[DEFAULT_TEXT_SIZE]), layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y - style[DEFAULT_TEXT_SIZE]/2 + layout.controls[selectedControl].rec.height/2, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else if (layout.controls[selectedControl].type == CHECKBOX) DrawText("|", layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x + layout.controls[selectedControl].rec.width + 5 + MeasureText(layout.controls[selectedControl].text, style[DEFAULT_TEXT_SIZE]), layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y - style[DEFAULT_TEXT_SIZE]/2 + layout.controls[selectedControl].rec.height/2, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else if (layout.controls[selectedControl].type == SLIDERBAR || layout.controls[selectedControl].type == SLIDER) DrawText("|", layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x - 5, layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y - style[DEFAULT_TEXT_SIZE]/2 + layout.controls[selectedControl].rec.height/2, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else DrawText("|", layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x + layout.controls[selectedControl].rec.width/2 + MeasureText(layout.controls[selectedControl].text , style[DEFAULT_TEXT_SIZE])/2 + 2, layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y + layout.controls[selectedControl].rec.height/2 - 6, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
               }
            }
            
            // Draw nameEditMode
            if (nameEditMode)
            {
                DrawText(FormatText("%s", layout.controls[selectedControl].name), layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x + layout.controls[selectedControl].rec.width/2 - MeasureText(layout.controls[selectedControl].name, style[DEFAULT_TEXT_SIZE]*2)/2, layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y + layout.controls[selectedControl].rec.height/2 - 10, style[DEFAULT_TEXT_SIZE]*2, BLACK);
                    
                if (((framesCounter/20)%2) == 0) DrawText("|", layout.controls[selectedControl].rec.x + layout.controls[selectedControl].rec.width/2 + layout.controls[selectedControl].ap->x + MeasureText(layout.controls[selectedControl].name, style[DEFAULT_TEXT_SIZE]*2)/2 + 2, layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y + layout.controls[selectedControl].rec.height/2 - 10, style[DEFAULT_TEXT_SIZE]*2 + 2, BLACK);                   
            }
            else if ((IsKeyDown(KEY_N)) && (!textEditMode) && (!generateWindowActive) && (!resetWindowActive))
            {
                if (layout.controlsCount > 0) DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(WHITE, 0.7f));
                
                for (int i = 0; i < layout.controlsCount; i++)
                {
                    // Draws the Controls when placed on the grid.
                    if (!layout.controls[i].ap->hidding)
                    DrawText(FormatText("%s", layout.controls[i].name), layout.controls[i].rec.x + layout.controls[i].ap->x + layout.controls[i].rec.width/2 - MeasureText(layout.controls[i].name, style[DEFAULT_TEXT_SIZE]*2)/2, layout.controls[i].rec.y + layout.controls[i].ap->y + layout.controls[i].rec.height/2 - 10, style[DEFAULT_TEXT_SIZE]*2, BLACK);
                }
            }


            // Draw anchor linking line
            if (anchorLinkMode) DrawLine(layout.anchors[linkedAnchor].x, layout.anchors[linkedAnchor].y, mouse.x, mouse.y, BLACK);

            // Draw Rectangle Info
            if (selectedControl != -1)
            {
                if (!controlGlobalPos) DrawText(FormatText("[%i, %i, %i, %i]", (int)layout.controls[selectedControl].rec.x, (int)layout.controls[selectedControl].rec.y, (int)layout.controls[selectedControl].rec.width, (int)layout.controls[selectedControl].rec.height), (int)layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x, (int)layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y - 30, 20, MAROON);
                else DrawText(FormatText("[%i, %i, %i, %i]", (int)layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x, (int)layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y, (int)layout.controls[selectedControl].rec.width, (int)layout.controls[selectedControl].rec.height), (int)layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x, (int)layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y - 30, 20, RED);
            }
                
            // Draw Image info
            if (tracemapEditMode) DrawText(FormatText("[%i, %i, %i, %i]", tracemapRec.x, tracemapRec.y, tracemapRec.width, tracemapRec.height), tracemapRec.x + 25, tracemapRec.y + 25, 20, MAROON);

            // Draw the id of all controls
            if (IsKeyDown(KEY_LEFT_ALT)) 
            {
                for (int i = layout.controlsCount - 1; i >= 0; i--) DrawText(FormatText("[%i]", layout.controls[i].id), layout.controls[i].rec.x + layout.controls[i].ap->x + layout.controls[i].rec.width, layout.controls[i].rec.y + layout.controls[i].ap->y - 10, 10, BLUE);  
            }
            // Draw the help listData (by default is out of screen)
            if (helpPositionX > -280)
            {
                DrawRectangleRec((Rectangle){ helpPositionX + 20, 15, 280, 550 }, GetColor(style[DEFAULT_BACKGROUND_COLOR]));
                GuiGroupBox((Rectangle){ helpPositionX + 20, 15, 280, 550 }, "TAB - Shortcuts");
                GuiLabel((Rectangle){ helpPositionX + 30, 30, 0, 0 }, "G - Toggle grid mode");
                GuiLabel((Rectangle){ helpPositionX + 30, 50, 0, 0 }, "S - Toggle snap to grid mode");
                GuiLabel((Rectangle){ helpPositionX + 30, 70, 0, 0 }, "F - Toggle control position (global/anchor)");
                GuiLine((Rectangle){ helpPositionX + 30, 85, 260, 10 }, 1);
                GuiLabel((Rectangle){ helpPositionX + 30, 100, 0, 0 }, "SPACE - Lock/unlock control for editing");
                GuiLabel((Rectangle){ helpPositionX + 30, 120, 0, 0 }, "ARROWS - Edit control position");
                GuiLabel((Rectangle){ helpPositionX + 30, 140, 0, 0 }, "LSHIFT + ARROWS - Smooth edit position");
                GuiLabel((Rectangle){ helpPositionX + 30, 160, 0, 0 }, "LCTRL + ARROWS - Edit control scale");   
                GuiLabel((Rectangle){ helpPositionX + 30, 180, 0, 0 }, "LCTRL + LSHIFT + ARROWS - Smooth edit scale");
                GuiLabel((Rectangle){ helpPositionX + 30, 200, 0, 0 }, "LCTRL + R - Resize control to closest snap");
                GuiLabel((Rectangle){ helpPositionX + 30, 220, 0, 0 }, "LCTRL + D - Duplicate selected control");
                GuiLabel((Rectangle){ helpPositionX + 30, 240, 0, 0 }, "LCTRL + N - Resets layout");
                GuiLabel((Rectangle){ helpPositionX + 30, 260, 0, 0 }, "DEL - Delete selected control");
                GuiLine((Rectangle){ helpPositionX + 30, 275, 260, 10 }, 1);
                GuiLabel((Rectangle){ helpPositionX + 30, 290, 0, 0 }, "T - Control text editing (if possible)");
                GuiLabel((Rectangle){ helpPositionX + 30, 310, 0, 0 }, "N - Control name editing ");
                GuiLabel((Rectangle){ helpPositionX + 30, 330, 0, 0 }, "ESC - Exit text/name editing mode");
                GuiLabel((Rectangle){ helpPositionX + 30, 350, 0, 0 }, "ENTER - Validate text/name edition");
                GuiLine((Rectangle){ helpPositionX + 30, 365, 260, 10 }, 1);
                GuiLabel((Rectangle){ helpPositionX + 30, 380, 0, 0 }, "LALT + UP/DOWN - Control layer order");
                GuiLine((Rectangle){ helpPositionX + 30, 395, 260, 10 }, 1);
                GuiLabel((Rectangle){ helpPositionX + 30, 410, 0, 0 }, "A - Anchor editing mode");
                GuiLabel((Rectangle){ helpPositionX + 30, 430, 0, 0 }, "RMB - Link anchor to control");
                GuiLabel((Rectangle){ helpPositionX + 30, 450, 0, 0 }, "U - Unlink control from anchor");
                GuiLabel((Rectangle){ helpPositionX + 30, 470, 0, 0 }, "H - Hide/Unhide controls for selected anchor");
                GuiLine((Rectangle){ helpPositionX + 30, 485, 260, 10 }, 1);
                GuiLabel((Rectangle){ helpPositionX + 30, 500, 0, 0 }, "LCTRL + S - Save layout file (.rgl)");
                GuiLabel((Rectangle){ helpPositionX + 30, 520, 0, 0 }, "LCTRL + O - Open layout file (.rgl)");
                GuiLabel((Rectangle){ helpPositionX + 30, 540, 0, 0 }, "LCTRL + ENTER - Export layout to code");
            }

            // Draw right panel controls palette
            GuiPanel(palettePanel);
            
            GuiWindowBox(paletteRecs[0], "WindowBox");
            GuiGroupBox(paletteRecs[1], "GroupBox");
            GuiLine(paletteRecs[2], 1);
            GuiPanel(paletteRecs[3]);
            GuiLabel(paletteRecs[4], "Label (SAMPLE TEXT)");
            GuiButton(paletteRecs[5], "Button");
            GuiToggleButton(paletteRecs[6], "Toggle", false); 
            GuiCheckBox(paletteRecs[8], false); 
            GuiToggleGroup(paletteRecs[7], listData, 3, 0); 
            GuiComboBox(paletteRecs[9],  listData, 3, 0); 
            GuiDropdownBox(paletteRecs[10], listData, 3, 0); 
            GuiSpinner(paletteRecs[11], 42, 100, 25);
            GuiValueBox(paletteRecs[12], 42, 100); 
            GuiTextBox(paletteRecs[13], "TEXTBOX", 7, false);
            GuiSlider(paletteRecs[14], 42, 0, 100);
            GuiSliderBar(paletteRecs[15], 42, 0, 100);
            GuiProgressBar(paletteRecs[16], 42, 0, 100);
            GuiStatusBar(paletteRecs[17], "StatusBar", 10);
            GuiListView(paletteRecs[18], listData, 3, 1); 
            GuiColorPicker(paletteRecs[19], RED);
            GuiDummyRec(paletteRecs[20], "DummyRec");

            DrawRectangleRec(paletteRecs[selectedType], Fade(RED, 0.5f));
            
            if (paletteSelect > -1) DrawRectangleLinesEx(paletteRecs[paletteSelect], 1, RED);
        
            // Draw export options window
            if (generateWindowActive)
			{
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(WHITE, 0.7f));
				generateWindowActive = !GuiWindowBox((Rectangle){ exportWindowPos.x, exportWindowPos.y, 400, 225 }, "Generate Code Options - layout");
                
                GuiLabel((Rectangle){ exportWindowPos.x + 10, exportWindowPos.y + 35, 65, 25 }, "Name:");
                GuiTextBox((Rectangle){ exportWindowPos.x + 75, exportWindowPos.y + 35, 135, 25 }, config.name, toolNameSize, true);
                GuiLabel((Rectangle){ exportWindowPos.x + 225, exportWindowPos.y + 35, 50, 25 }, "Version:");
                GuiTextBox((Rectangle){ exportWindowPos.x + 275, exportWindowPos.y + 35, 115, 25 }, config.version, toolVersionSize, true);
                GuiLabel((Rectangle){ exportWindowPos.x + 10, exportWindowPos.y + 65, 65, 25 }, "Window size:");
                config.width = GuiValueBox((Rectangle){ exportWindowPos.x + 75, exportWindowPos.y + 65, 60, 25 }, config.width, 1000); 
                GuiLabel((Rectangle){ exportWindowPos.x + 140, exportWindowPos.y + 65, 10, 25 }, "x");
                config.height = GuiValueBox((Rectangle){ exportWindowPos.x + 150, exportWindowPos.y + 65, 60, 25 }, config.height, 1000);
                GuiLabel((Rectangle){ exportWindowPos.x + 225, exportWindowPos.y + 65, 50, 25 }, "Company:");
                GuiTextBox((Rectangle){ exportWindowPos.x + 275, exportWindowPos.y + 65, 115, 25 }, config.company, companySize, true);
                GuiLabel((Rectangle){ exportWindowPos.x + 10, exportWindowPos.y + 95, 65, 25 }, "Description:");
                GuiTextBox((Rectangle){ exportWindowPos.x + 75, exportWindowPos.y + 95, 315, 55 }, config.description, toolDescriptionSize, true);
                config.defineRecs = GuiCheckBoxEx((Rectangle){ exportWindowPos.x + 10, exportWindowPos.y + 160, 15, 15 }, config.defineRecs, "Define Rectangles"); 
                config.defineTexts = GuiCheckBoxEx((Rectangle){ exportWindowPos.x + 10, exportWindowPos.y + 180, 15, 15 }, config.defineTexts, "Define text const");
                config.exportAnchors = GuiCheckBoxEx((Rectangle){ exportWindowPos.x + 140, exportWindowPos.y + 160, 15, 15 }, config.exportAnchors, "Export anchors");
                config.exportAnchor0 = GuiCheckBoxEx((Rectangle){ exportWindowPos.x + 140, exportWindowPos.y + 180, 15, 15 }, config.exportAnchor0, "Export anchor 0");
                config.fullComments = GuiCheckBoxEx((Rectangle){ exportWindowPos.x + 140, exportWindowPos.y + 200, 15, 15 }, config.fullComments, "Full comments");
                config.cropWindow = GuiCheckBoxEx((Rectangle){ exportWindowPos.x + 275, exportWindowPos.y + 160, 15, 15 }, config.cropWindow, "Crop to Window");
                config.fullVariables = GuiCheckBoxEx((Rectangle){ exportWindowPos.x + 10, exportWindowPos.y + 200, 15, 15 }, config.fullVariables, "Full variables");

                if (GuiButton((Rectangle){ exportWindowPos.x + 275, exportWindowPos.y + 185, 115, 30 }, "Generate Code"))
                {
                    ShowExportLayoutDialog(config);
                    generateWindowActive = false;
                }
			}
    
            // Draw status bar bottom with debug information
            GuiStatusBar((Rectangle){ 0, GetScreenHeight() - 24, 126, 24}, FormatText("MOUSE: (%i, %i)", (int)mouse.x, (int)mouse.y), 15);
            GuiStatusBar((Rectangle){ 124, GetScreenHeight() - 24, 81, 24}, (snapMode ? "SNAP: ON" : "SNAP: OFF"), 10);
            GuiStatusBar((Rectangle){ 204, GetScreenHeight() - 24, 145, 24}, FormatText("CONTROLS COUNT: %i", layout.controlsCount), 20);
            if (selectedControl != -1) GuiStatusBar((Rectangle){ 348, GetScreenHeight() - 24, GetScreenWidth() - 348, 24}, FormatText("SELECTED CONTROL: #%03i  |  %s  |  REC (%i, %i, %i, %i)  |  %s", selectedControl, controlTypeName[layout.controls[selectedControl].type], (int)layout.controls[selectedControl].rec.x, (int)layout.controls[selectedControl].rec.y, (int)layout.controls[selectedControl].rec.width, (int)layout.controls[selectedControl].rec.height, layout.controls[selectedControl].name), 15);
            else GuiStatusBar((Rectangle){ 348, GetScreenHeight() - 24, GetScreenWidth() - 348, 24}, "", 15);
            
            // Draw ending message window (save)
            if (closingWindowActive)
            {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(WHITE, 0.7f));
                closingWindowActive = !GuiWindowBox((Rectangle){ GetScreenWidth()/2 - 125, GetScreenHeight()/2 - 50, 250, 100 }, "Closing rGuiLayout");
                
                GuiLabel((Rectangle){ GetScreenWidth()/2 - 95, GetScreenHeight()/2 - 60, 200, 100 }, "Do you want to save before quitting?");
                
                if (GuiButton((Rectangle){ GetScreenWidth()/2 - 94, GetScreenHeight()/2 + 10, 85, 25 }, "Yes")) 
                { 
                    cancelSave = false;
                    ShowSaveLayoutDialog();
                    if (cancelSave) exitWindow = true;
                }
                else if (GuiButton((Rectangle){ GetScreenWidth()/2 + 10, GetScreenHeight()/2 + 10, 85, 25 }, "No")) { exitWindow = true; }
            }
            
            // Draw reset message window (save)
            if (resetWindowActive)
            {
                DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(WHITE, 0.7f));
                resetWindowActive = !GuiWindowBox((Rectangle){ GetScreenWidth()/2 - 125, GetScreenHeight()/2 - 50, 250, 100 }, "Creating new layout");
                
                GuiLabel((Rectangle){ GetScreenWidth()/2 - 95, GetScreenHeight()/2 - 60, 200, 100 }, "Do you want to save the current layout?");
                
                if (GuiButton((Rectangle){ GetScreenWidth()/2 - 94, GetScreenHeight()/2 + 10, 85, 25 }, "Yes")) 
                { 
                    cancelSave = false;
                    ShowSaveLayoutDialog();
                    if (cancelSave) resetLayout = true;
                }
                else if (GuiButton((Rectangle){ GetScreenWidth()/2 + 10, GetScreenHeight()/2 + 10, 85, 25 }, "No")) { resetLayout = true; }
            }
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(tracemap);
    
    CloseWindow();        // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Show save layout dialog
static void ShowSaveLayoutDialog(void)
{
    const char *filters[] = { "*.rgl" };
    const char *fileName = tinyfd_saveFileDialog("Save raygui layout text file", "", 1, filters, "raygui Layout Files (*.rgl)");

    // Save layout.controls file (text or binary)
    if (fileName != NULL)
    {
        char outFileName[64] = { 0 };
        strcpy(outFileName, fileName);
        if (GetExtension(fileName) == NULL) strcat(outFileName, ".rgl\0");     // No extension provided
        SaveLayoutRGL(outFileName, false);
        strcpy(loadedFileName, outFileName);
        SetWindowTitle(FormatText("rGuiLayout v1.0 - %s", GetFileName(loadedFileName)));
        cancelSave = true;
    }
}

// Show save layout dialog
static void ShowExportLayoutDialog(GuiLayoutConfig config)
{
    const char *filters[] = { "*.c", "*.go", "*.lua" };
    const char *fileName = tinyfd_saveFileDialog("Generate code file", config.name, 3, filters, "Code file");

    if (fileName != NULL)
    {
        char outFileName[64] = { 0 };
        strcpy(outFileName, fileName);
        if (GetExtension(fileName) == NULL) strcat(outFileName, ".c\0");     // No extension provided
        GenerateCode(outFileName, config);
    }
}

// Save gui layout information
static void SaveLayoutRGL(const char *fileName, bool binary)
{
    if (binary)
    {
        #define RGL_FILE_VERSION_BINARY 100
        
        FILE *rglFile = fopen(fileName, "wb");

        if (rglFile != NULL)
        {
            // Write some header info (12 bytes)
            // id: "RGL "       - 4 bytes
            // version: 100     - 2 bytes
            // reserved         - 2 bytes
            
            char signature[5] = "RGL ";
            short version = RGL_FILE_VERSION_BINARY;
            short reserved = 0;

            fwrite(signature, 1, 4, rglFile);
            fwrite(&version, 1, sizeof(short), rglFile);
            fwrite(&reserved, 1, sizeof(short), rglFile);
            
            fwrite(&layout, 1, sizeof(GuiLayout), rglFile);

            fclose(rglFile);  
        }
    }
    else 
    {
        #define RGL_FILE_VERSION_TEXT "1.0"
        
        FILE *rglFile = fopen(fileName, "wt");

        if (rglFile != NULL)
        {
             // Write some description comments
            fprintf(rglFile, "#\n# rgl text file (v%s) - raygui layout text file generated using rGuiLayout\n#\n", RGL_FILE_VERSION_TEXT);
            fprintf(rglFile, "# Total number of controls:     %i\n", layout.controlsCount);
            fprintf(rglFile, "# Anchor info:   a <id> <posx> <posy> <enabled>\n");
            fprintf(rglFile, "# Control info:  c <id> <type> <name> <rectangle> <anchor_id> <text>\n#\n");

            for (int i = 0; i < MAX_ANCHOR_POINTS; i++)
            {
                fprintf(rglFile, "a %03i %i %i %i\n", layout.anchors[i].id, layout.anchors[i].x, layout.anchors[i].y, layout.anchors[i].enabled);
            }
            
            for (int i = 0; i < layout.controlsCount; i++)
            {
                fprintf(rglFile, "c %03i %i %s %i %i %i %i %i %s\n", layout.controls[i].id, layout.controls[i].type, layout.controls[i].name, (int)layout.controls[i].rec.x, (int)layout.controls[i].rec.y, (int)layout.controls[i].rec.width, (int)layout.controls[i].rec.height, layout.controls[i].ap->id, layout.controls[i].text);
            }

            fclose(rglFile);
        }
    }
}

// Import gui layout information
// NOTE: Updates global variable: layout
static void LoadLayoutRGL(const char *fileName)
{
    char buffer[256];
    bool tryBinary = false;
    
    int anchorId = 0;       // TODO: Review!!!
    int anchorX = 0;
    int anchorY = 0;
    int anchorCounter = 0;

    FILE *rglFile = fopen(fileName, "rt");
    
    if (rglFile != NULL)
    {
        // Reset all the controls
        for (int i = 0; i < MAX_GUI_CONTROLS; i++)
        {
            layout.controls[i].id = 0;
            layout.controls[i].type = 0;
            layout.controls[i].rec = (Rectangle){ 0, 0, 0, 0 };
            memset(layout.controls[i].text, 0, MAX_CONTROL_TEXT_LENGTH);
            memset(layout.controls[i].name, 0, MAX_CONTROL_NAME_LENGTH);
            layout.controls[i].ap = &layout.anchors[0];
        }
        for (int i = 0; i < MAX_ANCHOR_POINTS; i++) layout.anchors[i].hidding = false;
        
        fgets(buffer, 256, rglFile);

        if (buffer[0] != 'R')   // Text file!
        {
            layout.controlsCount = 0;
            
            while (!feof(rglFile))
            {
                if ((buffer[0] != '\n') && (buffer[0] != '#') && (buffer[0] == 'a'))
                {
                    sscanf(buffer, "a %03i %i %i %i", &layout.anchors[anchorCounter].id, &layout.anchors[anchorCounter].x, &layout.anchors[anchorCounter].y, &layout.anchors[anchorCounter].enabled);
                    //printf("a %03i %i %i %i\n", layout.anchors[anchorCounter].id, layout.anchors[anchorCounter].x, layout.anchors[anchorCounter].y, layout.anchors[anchorCounter].enabled);
                    anchorCounter++;
                }
                else if ((buffer[0] != '\n') && (buffer[0] != '#') && (buffer[0] == 'c'))
                {
                    sscanf(buffer, "c %d %i %s %f %f %f %f %d %[^\n]s", &layout.controls[layout.controlsCount].id, &layout.controls[layout.controlsCount].type, layout.controls[layout.controlsCount].name, &layout.controls[layout.controlsCount].rec.x, &layout.controls[layout.controlsCount].rec.y, &layout.controls[layout.controlsCount].rec.width, &layout.controls[layout.controlsCount].rec.height, &anchorId, layout.controls[layout.controlsCount].text);
                    //printf("c %d %i %i %i %i %i %i %s\n", layout.controls[layout.controlsCount].id, layout.controls[layout.controlsCount].type, layout.controls[layout.controlsCount].rec.x, layout.controls[layout.controlsCount].rec.y, layout.controls[layout.controlsCount].rec.width, layout.controls[layout.controlsCount].rec.height, anchorId, layout.controls[layout.controlsCount].text);
                    
                    layout.controls[layout.controlsCount].ap = &layout.anchors[anchorId];
                    layout.controlsCount++;
                }
                fgets(buffer, 256, rglFile);
            }
        }
        else tryBinary = true;
        
        fclose(rglFile);
    }
    
    if (tryBinary)
    {
        FILE *rglFile = fopen(fileName, "rb");
    
        if (rglFile != NULL)
        {
            char signature[5] = "";
            short version = 0;
            short reserved = 0;

            fread(signature, 1, 4, rglFile);
            fread(&version, 1, sizeof(short), rglFile);
            fread(&reserved, 1, sizeof(short), rglFile);

            if ((signature[0] == 'R') &&
                (signature[1] == 'G') &&
                (signature[2] == 'L') &&
                (signature[3] == ' ')) fread(&layout, 1, sizeof(GuiLayout), rglFile);
            else TraceLog(LOG_WARNING, "[raygui] Invalid layout file");
            
            printf("[GuiLayout] Controls counter: %i\n", layout.controlsCount);
           
            fclose(rglFile);
        }
    }

    printf("[GuiLayout] Layout data loaded successfully\n");
}

static char *GetControlAnchorRec(int anchorId, Rectangle controlRec, GuiLayoutConfig config)
{
    static char text[512];
    memset(text, 0, 512);
    
    if (config.exportAnchors && config.exportAnchor0) strcpy(text, FormatText("(Rectangle){ anchor%02i.x + %i, anchor%02i.y + %i, %i, %i }", anchorId, (int)controlRec.x, anchorId, (int)controlRec.y, (int)controlRec.width, (int)controlRec.height));
    else if (config.exportAnchors && !config.exportAnchor0) 
    {
        if (anchorId > 0) strcpy(text, FormatText("(Rectangle){ anchor%02i.x + %i, anchor%02i.y + %i, %i, %i }", anchorId, (int)controlRec.x, anchorId, (int)controlRec.y, (int)controlRec.width, (int)controlRec.height));
        else strcpy(text, FormatText("(Rectangle){ %i, %i, %i, %i }", (int)controlRec.x, (int)controlRec.y, (int)controlRec.width, (int)controlRec.height));

    }
    else if (!config.exportAnchors && config.exportAnchor0)
    {
        if (anchorId > 0) strcpy(text, FormatText("(Rectangle){ %i, %i, %i, %i }", layout.anchors[anchorId].x + (int)controlRec.x, layout.anchors[anchorId].y + (int)controlRec.y, (int)controlRec.width, (int)controlRec.height));
        else strcpy(text, FormatText("(Rectangle){ anchor%02i.x + %i, anchor%02i.y + %i, %i, %i }", anchorId, (int)controlRec.x, anchorId, (int)controlRec.y, (int)controlRec.width, (int)controlRec.height));
    }
    else strcpy(text, FormatText("(Rectangle){ %i, %i, %i, %i }", layout.anchors[anchorId].x + (int)controlRec.x, layout.anchors[anchorId].y + (int)controlRec.y, (int)controlRec.width, (int)controlRec.height));
    
    return text;
}

static char *ExportFullVariables(int controlType, char *name, GuiLayoutConfig config)
{
    static char text[512];
    memset(text, 0, 512);
    
    switch(controlType)
    {
        case SLIDER: 
        case SLIDERBAR:
        {
            if (config.fullVariables) strcpy(text, FormatText("%sMinValue, %sMaxValue", name, name));
            else strcpy(text, "0, 100");
        } break;
        
        case DROPDOWNBOX:
        case COMBOBOX:
        case LISTVIEW:
        case TOGGLEGROUP:
        {
            if (config.fullVariables) strcpy(text, FormatText("%sCount", name));
            else strcpy(text, "3");
        } break;
        
        case TEXTBOX:
        {
            if (config.fullVariables) strcpy(text, FormatText("%sSize", name));
            else strcpy(text, FormatText("%i", MAX_CONTROL_TEXT_LENGTH));
        } break;
    }
    
    return text;
}

// Generate C code for gui layout
static void GenerateCode(const char *fileName, GuiLayoutConfig config)
{
    FILE *ftool = fopen(fileName, "wt");
    
    fprintf(ftool, "/*******************************************************************************************\n");
    fprintf(ftool, "*\n");
    fprintf(ftool, "*   %s - %s\n", config.name, config.description);
    fprintf(ftool, "*\n");
    fprintf(ftool, "*   LICENSE: zlib/libpng\n");
    fprintf(ftool, "*\n");
    fprintf(ftool, "*   Copyright (c) %i %s\n", 2018, config.company);
    fprintf(ftool, "*\n");
    fprintf(ftool, "**********************************************************************************************/\n\n");
    fprintf(ftool, "#include \"raylib.h\"\n\n");
    fprintf(ftool, "#define RAYGUI_IMPLEMENTATION\n");
    fprintf(ftool, "#include \"raygui.h\"\n\n");
    fprintf(ftool, "//----------------------------------------------------------------------------------\n");
    fprintf(ftool, "// Controls Functions Declaration\n");
    fprintf(ftool, "//----------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < layout.controlsCount; i++) 
    {
        if (layout.controls[i].type == BUTTON) fprintf(ftool, "static void %s();\t\t// %s: %s logic\n", layout.controls[i].name, controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
    }

    fprintf(ftool, "\n");
    fprintf(ftool, "//------------------------------------------------------------------------------------\n");
    fprintf(ftool, "// Program main entry point\n");
    fprintf(ftool, "//------------------------------------------------------------------------------------\n");
    fprintf(ftool, "int main()\n");
    fprintf(ftool, "{\n");
    fprintf(ftool, "    // Initialization\n");
    fprintf(ftool, "    //---------------------------------------------------------------------------------------\n");
    fprintf(ftool, "    int screenWidth = %i;\n", config.width);
    fprintf(ftool, "    int screenHeight = %i;\n\n", config.height);
    fprintf(ftool, "    InitWindow(screenWidth, screenHeight, \"%s\");\n\n", config.name);
    
    fprintf(ftool, "    // %s: controls initialization\n", config.name);
    fprintf(ftool, "    //----------------------------------------------------------------------------------\n");

    // TODO: Use config.exportAnchors and config.exportAnchor0
    if (config.exportAnchors)
    {
        fprintf(ftool, "    // Anchor points\n");
        
        for(int i = 0; i < MAX_ANCHOR_POINTS; i++)
        {
            for (int j = 0; j < layout.controlsCount; j++)
            {
                if (layout.controls[j].ap->id == layout.anchors[i].id)
                {
                    if ((!config.exportAnchor0) && layout.controls[j].ap->id == 0) break;
                    fprintf(ftool, "    Vector2 %s%02i = { %i, %i };\n", "anchor", i, layout.anchors[i].x, layout.anchors[i].y);
                    break;
                }
            }
        }
    }
    
    if (!config.fullComments) fprintf(ftool, "\n");

    // Define controls variables
    // TODO: Use config.fullComments
    for (int i = 0; i < layout.controlsCount; i++)
    {
        switch (layout.controls[i].type)
        {
            // Bools
            case WINDOWBOX:
            case TOGGLE:
            {
                if (config.fullComments) fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    bool %sActive = true;\n", layout.controls[i].name);
            }
            break;
            
            case CHECKBOX: 
            {
                if (config.fullComments) fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    bool %sChecked = false;\n", layout.controls[i].name);
            }
            break;
            
            // Const text
            case LABEL:
            {
                if (config.defineTexts)
                {
                    if (config.fullComments) fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                    fprintf(ftool, "    const char *%sText = \"%s\";\n", layout.controls[i].name, layout.controls[i].text);
                }
            }
            break;
            
            case STATUSBAR:
            {
                if (config.fullComments) fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    char *%sText = \"%s\";\n", layout.controls[i].name, layout.controls[i].text);
            }
            break;
            
            case DROPDOWNBOX:
            case COMBOBOX:
            case LISTVIEW:
            case TOGGLEGROUP:
            {
                if (config.fullComments) fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                if (config.fullVariables) fprintf(ftool, "    int %sCount = 3;\n", layout.controls[i].name);
                fprintf(ftool, "    int %sActive = 0;\n", layout.controls[i].name);
                fprintf(ftool, "    const char *%sTextList[3] = { \"ONE\", \"TWO\", \"THREE\" };\n", layout.controls[i].name);
            }
            break;
            
            // Floats
            case SLIDER: 
            case SLIDERBAR:
            {
                if (config.fullComments) fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    float %sValue = 50.0f;\n", layout.controls[i].name);
                if (config.fullVariables)
                {
                    fprintf(ftool, "    const float %sMinValue = 0.0f;\n", layout.controls[i].name);
                    fprintf(ftool, "    const float %sMaxValue = 100.0f;\n", layout.controls[i].name);
                }
            }
            break;
            case PROGRESSBAR:
            {
                if (config.fullComments) fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    float %sValue = 50.0f;\n", layout.controls[i].name);
            }            
            break;
            
            // Ints
            case VALUEBOX:
            case SPINNER:
            {
                if (config.fullComments) fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    int %sValue = 0;\n", layout.controls[i].name);
            }     
            break;
            
            // Colors
            case COLORPICKER:
            {
                if (config.fullComments) fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    Color %sValue;\n", layout.controls[i].name);
            }
            break;

            case TEXTBOX:
            {
                if (config.fullComments) fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                if (config.fullVariables) fprintf(ftool, "    int %sSize = %i;\n", layout.controls[i].name, MAX_CONTROL_TEXT_LENGTH);
                fprintf(ftool, "    char %sText[%i] = \"%s\";\n", layout.controls[i].name, MAX_CONTROL_TEXT_LENGTH, layout.controls[i].text);           
            }
            default: break;
        }
    }

    if (config.defineRecs)
    {
        // Define controls rectangles
        fprintf(ftool, "\n    // Define controls rectangles\n");
        fprintf(ftool, "    Rectangle layoutRecs[%i] = {\n", layout.controlsCount);
        
        for (int i = 0; i < layout.controlsCount; i++)
        {
            fprintf(ftool, "        %s", GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config));
            
            if (i == layout.controlsCount - 1) fprintf(ftool, "\t\t// %s: %s\n    };\n\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
            else fprintf(ftool, ",\t\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
        }
    }
    
    fprintf(ftool, "    //----------------------------------------------------------------------------------\n\n");
    
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
    fprintf(ftool, "            ClearBackground(GetColor(style[DEFAULT_BACKGROUND_COLOR]));\n\n");

    fprintf(ftool, "\t\t\t// raygui: controls drawing\n");
    fprintf(ftool, "\t\t\t//----------------------------------------------------------------------------------\n");
    
    // Draw all controls
    // TODO: Use config.fullComments to add extra comments
    if (!config.defineRecs)
    {
        for (int i = 0; i < layout.controlsCount; i++)
        {
            switch (layout.controls[i].type)
            {
                case LABEL: 
                {
                    if (config.defineTexts) fprintf(ftool, "\t\t\tGuiLabel(%s, %sText);\n", GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name);
                    else fprintf(ftool, "\t\t\tGuiLabel(%s, \"%s\");\n", GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].text);
                } 
                break;
                case BUTTON: fprintf(ftool, "\t\t\tif (GuiButton(%s, \"%s\")) %s(); \n\n", GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].text, layout.controls[i].name); break;
                case VALUEBOX: fprintf(ftool, "\t\t\t%sValue = GuiValueBox(%s, %sValue, 100); \n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name); break;
                case TOGGLE: fprintf(ftool, "\t\t\t%sActive = GuiToggleButton(%s, \"%s\", %sActive); \n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].text, layout.controls[i].name); break;
                case TOGGLEGROUP: fprintf(ftool, "\t\t\t%sActive = GuiToggleGroup(%s, %sTextList, %s, %sActive); \n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config), layout.controls[i].name); break;
                case SLIDER: 
                {
                    if (layout.controls[i].text[0] != '\0') fprintf(ftool, "\t\t\t%sValue = GuiSliderEx(%s, %sValue, %s, \"%s\", true);\n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config), layout.controls[i].text);
                    else fprintf(ftool, "\t\t\t%sValue = GuiSlider(%s, %sValue, %s);\n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config));
                } break;
                case SLIDERBAR: 
                {
                    if (layout.controls[i].text[0] != '\0') fprintf(ftool, "\t\t\t%sValue = GuiSliderBarEx(%s, %sValue, %s, \"%s\", true);\n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config), layout.controls[i].text);
                    else fprintf(ftool, "\t\t\t%sValue = GuiSliderBar(%s, %sValue, %s);\n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config));
                } break;
                case PROGRESSBAR: fprintf(ftool, "\t\t\t%sValue = GuiProgressBarEx(%s, %sValue, 0, 100, true);\n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name); break;
                case SPINNER: fprintf(ftool, "\t\t\t%sValue = GuiSpinner(%s, %sValue, 100, 25);\n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name); break;
                case COMBOBOX: fprintf(ftool, "\t\t\t%sActive = GuiComboBox(%s,  %sTextList, %s, %sActive); \n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config), layout.controls[i].name); break;
                case CHECKBOX: 
                {
                    if (layout.controls[i].text[0] != '\0') fprintf(ftool, "\t\t\t%sChecked = GuiCheckBoxEx(%s, %sChecked, \"%s\"); \n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name, layout.controls[i].text);
                    else fprintf(ftool, "\t\t\t%sChecked = GuiCheckBox(%s, %sChecked); \n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name);
                } break;
                case LISTVIEW: fprintf(ftool, "\t\t\t%sActive = GuiListView(%s, %sTextList, %s, %sActive); \n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config), layout.controls[i].name); break;
                case TEXTBOX: fprintf(ftool, "\t\t\tGuiTextBox(%s, %sText, %s, true);\n", GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config)); break;
                case GROUPBOX: fprintf(ftool, "\t\t\tGuiGroupBox(%s, \"%s\");\n", GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].text); break;
                case WINDOWBOX:
                {
                    fprintf(ftool, "\t\t\tif (%sActive)\n\t\t\t{\n", layout.controls[i].name);
                    fprintf(ftool, "\t\t\t\t%sActive = !GuiWindowBox(%s, \"%s\");\n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].text);
                    fprintf(ftool, "\t\t\t}\n");
                }break;
                case DUMMYREC: fprintf(ftool, "\t\t\tGuiDummyRec(%s, \"%s\");\n", GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].text); break;
                case DROPDOWNBOX: fprintf(ftool, "\t\t\t%sActive = GuiDropdownBox(%s, %sTextList, %s, %sActive); \n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config), layout.controls[i].name); break;
                case STATUSBAR: fprintf(ftool, "\t\t\tGuiStatusBar(%s, %sText, 10);\n", GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name); break;
                case COLORPICKER: fprintf(ftool, "\t\t\t%sValue = GuiColorPicker(%s, %sValue);\n", layout.controls[i].name, GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config), layout.controls[i].name); break;
                case LINE: fprintf(ftool, "\t\t\tGuiLine(%s, 1);\n", GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config)); break;
                case PANEL: fprintf(ftool, "\t\t\tGuiPanel(%s);\n", GetControlAnchorRec(layout.controls[i].ap->id, layout.controls[i].rec, config)); break;
                
                default: break;
            }
        }
    }
    else 
    {
        for (int i = 0; i < layout.controlsCount; i++)
        {
            switch (layout.controls[i].type)
            {
                case LABEL: 
                {
                    if (config.defineTexts) fprintf(ftool, "\t\t\tGuiLabel(layoutRecs[%i], %sText);\n", i, layout.controls[i].name);
                    else fprintf(ftool, "\t\t\tGuiLabel(layoutRecs[%i], \"%s\");\n", i, layout.controls[i].text);
                } break;
                case BUTTON: fprintf(ftool, "\t\t\tif (GuiButton(layoutRecs[%i], \"%s\")) %s(); \n\n", i, layout.controls[i].text, layout.controls[i].name); break;
                case VALUEBOX: fprintf(ftool, "\t\t\t%sValue = GuiValueBox(layoutRecs[%i], %sValue, 100); \n",layout.controls[i].name, i, layout.controls[i].name); break;
                case TOGGLE: fprintf(ftool, "\t\t\t%sActive = GuiToggleButton(layoutRecs[%i], \"%s\", %sActive); \n", layout.controls[i].name, i, layout.controls[i].text, layout.controls[i].name); break;
                case TOGGLEGROUP: fprintf(ftool, "\t\t\t%sActive = GuiToggleGroup(layoutRecs[%i], %sTextList, %s, %sActive); \n", layout.controls[i].name, i, layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config), layout.controls[i].name); break;
                case SLIDER:
                {
                    if (layout.controls[i].text[0] != '\0') fprintf(ftool, "\t\t\t%sValue = GuiSliderEx(layoutRecs[%i], %sValue, %s, \"%s\", true);\n", layout.controls[i].name, i, layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config), layout.controls[i].text);
                    else fprintf(ftool, "\t\t\t%sValue = GuiSlider(layoutRecs[%i], %sValue, %s);\n", layout.controls[i].name, i, layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config));
                } break;
                case SLIDERBAR: 
                {
                    if (layout.controls[i].text[0] != '\0') fprintf(ftool, "\t\t\t%sValue = GuiSliderBarEx(layoutRecs[%i], %sValue, %s, \"%s\", true);\n", layout.controls[i].name, i, layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config), layout.controls[i].text);
                    else fprintf(ftool, "\t\t\t%sValue = GuiSliderBar(layoutRecs[%i], %sValue, %s);\n", layout.controls[i].name, i, layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config));
                } break;
                case PROGRESSBAR: fprintf(ftool, "\t\t\t%sValue = GuiProgressBarEx(layoutRecs[%i], %sValue, %s, true);\n", layout.controls[i].name, i, layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config)); break;
                case SPINNER: fprintf(ftool, "\t\t\t%sValue = GuiSpinner(layoutRecs[%i], %sValue, 100, 25);\n", layout.controls[i].name, i, layout.controls[i].name); break;
                case COMBOBOX: fprintf(ftool, "\t\t\t%sActive = GuiComboBox(layoutRecs[%i],  %sTextList, %s, %sActive); \n", layout.controls[i].name, i, layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config), layout.controls[i].name); break;
                case CHECKBOX:
                {
                    if (layout.controls[i].text[0] != '\0') fprintf(ftool, "\t\t\t%sChecked = GuiCheckBoxEx(layoutRecs[%i], %sChecked, \"%s\"); \n", layout.controls[i].name, i, layout.controls[i].name, layout.controls[i].text);
                    else fprintf(ftool, "\t\t\t%sChecked = GuiCheckBox(layoutRecs[%i], %sChecked); \n", layout.controls[i].name, i, layout.controls[i].name);
                } break;
                case LISTVIEW: fprintf(ftool, "\t\t\t%sActive = GuiListView(layoutRecs[%i], %sTextList, %s, %sActive); \n", layout.controls[i].name, i, layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config), layout.controls[i].name); break;
                case TEXTBOX: fprintf(ftool, "\t\t\tGuiTextBox(layoutRecs[%i], %sText, %s, true);\n", i, layout.controls[i].name, ExportFullVariables(layout.controls[i].type, layout.controls[i].name, config)); break;
                case GROUPBOX: fprintf(ftool, "\t\t\tGuiGroupBox(layoutRecs[%i], \"%s\");\n", i, layout.controls[i].text); break;
                case WINDOWBOX:
                {
                    fprintf(ftool, "\t\t\tif (%sActive)\n\t\t\t{\n", layout.controls[i].name);
                    fprintf(ftool, "\t\t\t\t%sActive = !GuiWindowBox(layoutRecs[%i], \"%s\");\n", layout.controls[i].name, i, layout.controls[i].text);
                    fprintf(ftool, "\t\t\t}\n");
                }break;
                case DUMMYREC: fprintf(ftool, "\t\t\tGuiDummyRec(layoutRecs[%i], \"%s\");\n", i, layout.controls[i].text); break;
                case DROPDOWNBOX: fprintf(ftool, "\t\t\t%sActive = GuiDropdownBox(layoutRecs[%i], %sTextList, %sCount, %sActive); \n", layout.controls[i].name, i, layout.controls[i].name, layout.controls[i].name, layout.controls[i].name); break;
                case STATUSBAR: fprintf(ftool, "\t\t\tGuiStatusBar(layoutRecs[%i], %sText, 10);\n", i, layout.controls[i].name); break;
                case COLORPICKER: fprintf(ftool, "\t\t\t%sValue = GuiColorPicker(layoutRecs[%i], %sValue);\n", layout.controls[i].name, i, layout.controls[i].name); break;
                case LINE: fprintf(ftool, "\t\t\tGuiLine(layoutRecs[%i], 1);\n", i); break;
                case PANEL: fprintf(ftool, "\t\t\tGuiPanel(layoutRecs[%i]);\n", i); break;
                default: break;
            }
        }
    }
    fprintf(ftool, "\t\t\t//----------------------------------------------------------------------------------\n\n");
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
    
    for (int i = 0; i < layout.controlsCount; i++)
    {
        if (layout.controls[i].type == BUTTON)
        {
            fprintf(ftool, "// %s: %s logic\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
            fprintf(ftool, "static void %s()\n{\n    // TODO: Implement control logic\n}\n\n", layout.controls[i].name);
        }
    }

    fclose(ftool);
}

// Generate C code from .rgl file
static void GenerateCodeFromRGL(const char *fileName)
{
    if (IsFileExtension(fileName, ".rgl"))
    {
        LoadLayoutRGL(fileName);    // Updates global: layout.controls
        
        int len = strlen(fileName);
        char outName[256] = { 0 };
        strcpy(outName, fileName);
        outName[len - 3] = 'c';
        outName[len - 2] = '\0';
        
        GuiLayoutConfig config;
        memset(&config, 0, sizeof(GuiLayoutConfig));
        
        config.width = 800;
        config.height = 600;
        strcpy(config.name, "layout_file_name");
        strcpy(config.version, "1.0-dev");
        strcpy(config.company, "raylib technologies");
        strcpy(config.description, "tool description");
        config.defineRecs = false;
        config.exportAnchors = true;
        config.exportAnchor0 = false;
        config.fullComments = true;
        
        // Generate C code for gui layout.controls
        GenerateCode(outName, config);
    }
    else printf("Input RGL file not valid\n");
}
