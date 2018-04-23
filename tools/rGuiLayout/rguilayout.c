/*******************************************************************************************
*
*   rGuiLayout - raygui layout editor
*
*   This example has been created using raylib 1.9.7 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
*
*   Copyright (c) 2014-2018 Ramon Santamaria (@raysan5), Jordi Jorba & Adria Arranz
*
********************************************************************************************/

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

#define MAX_CONTROL_TEXT_LENGTH  32         // Maximum length of control text
#define MAX_CONTROL_NAME_LENGTH  32         // Maximum length of control name (used on code generation)

#define GRID_LINE_SPACING         5         // Grid line spacing in pixels

#define MOVEMENT_FRAME_SPEED     10         // Controls movement speed in pixels per frame

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { 
    LABEL = 0, 
    BUTTON, 
    VALUEBOX,
    TOGGLE, 
    TOGGLEGROUP, 
    SLIDER, 
    SLIDERBAR, 
    PROGRESSBAR, 
    SPINNER, 
    COMBOBOX, 
    CHECKBOX, 
    TEXTBOX,
    GROUPBOX,
    WINDOWBOX,
    DUMMYREC,
    DROPDOWNBOX,
    STATUSBAR,
    LISTVIEW,
    COLORPICKER
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
} GuiLayoutConfig;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static int screenWidth = 800;
static int screenHeight = 600;

static GuiLayout layout = { 0 };

const char *controlTypeName[] = { "LABEL", "BUTTON", "VALUEBOX", "TOGGLE", "TOGGLEGROUP", "SLIDER", "SLIDERBAR", "PROGRESSBAR", "SPINNER", "COMBOBOX", "CHECKBOX", "TEXTBOX", "GROUPBOX", "WINDOWBOX", "DUMMYREC", "DROPDOWNBOX", "STATUSBAR", "LISTVIEW", "COLORPICKER" };
const char *controlTypeNameLow[] = { "Label", "Button", "ValueBox", "Toggle", "ToggleGroup", "Slider", "SliderBar", "ProgressBar", "Spinner", "ComboBox", "CheckBox", "TextBox", "GroupBox", "WindowBox", "DummyRec", "DropdownBox", "StatusBar", "ListView", "ColorPicker" };
const char *controlTypeNameShort[] = { "lbl", "btn", "vlbox", "tggl", "tgroup", "sldr", "sldrb", "prgssb", "spnr", "combox", "chkbox", "txtbox", "grpbox", "wdwbox", "dmyrc", "ddwnbox", "stsb", "lstvw", "clrpckr" };

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static void DrawGrid2D(int width, int height, int spacing);             // Draw 2d grid at specific size and spacing
static void SaveLayoutRGL(const char *fileName, bool binary);           // Save gui layout project information
static void LoadLayoutRGL(const char *fileName);                        // Load gui layout project information
static void GenerateCode(const char *fileName, GuiLayoutConfig config); // Generate C code for gui layout
static void GenerateCodeFromRGL(const char *fileName);                  // Generate C code from .rgl file

//----------------------------------------------------------------------------------
// Main Entry point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "rGuiLayout v1.0 - raygui layout editor");
    SetExitKey(0);
       
    int selectedControl = -1;
    int selectedType = BUTTON;
    int selectedTypeDraw = LABEL;

    int mouseX, mouseY;
    bool snapMode = false;
    bool showGrid = true;
    bool controlCollision = false;          // Checks if the mouse is colliding with a control or list
    bool controlDrag = false;               // Allows the control to be moved with the mouse without detecting collision every frame
    
    bool textEditMode = false;
    bool nameEditMode = false;
    int framesCounter = 0;
    int saveControlSelected = -1;
    char previewText[8] = "TEXTBOX";
    bool anchorMode = false;
    bool anchorLinkMode = false;
    int selectedAnchor = -1;
    int linkedAnchor = -1;
    
    AnchorPoint auxAnchor = { 9, 0, 0, 0};
    bool anchorNewPos = false;
    bool lockAnchorMode = false;
    int saveAnchorSelected = -1;

    int snapFrameCounter = 0;

    bool lockMode = false;
    bool globalReference = false;

    int helpPosX = -300;
    int helpCounter = 0;
    int startPosXHelp = -300;
    int deltaPosXHelp = 0;
    
    int fileCount = 0;
    char **droppedFiles = { 0 };
    
    // Used to draw the preview of selectedControl
    Rectangle defaultRec[19] = {
        (Rectangle){ 0, 0, 80, 20},             // LABEL
        (Rectangle){ 0, 0, 100, 30},            // BUTTON
        (Rectangle){ 0, 0, 120, 30},            // VALUEBOX
        (Rectangle){ 0, 0, 100, 30},            // TOGGLE
        (Rectangle){ 0, 0, 240, 30},            // TOGGLEGROUP  
        (Rectangle){ 0, 0, 200, 20},            // SLIDER
        (Rectangle){ 0, 0, 200, 20},            // SLIDERBAR
        (Rectangle){ 0, 0, 200, 20},            // PROGRESSBAR
        (Rectangle){ 0, 0, 150, 30},            // SPINNER
        (Rectangle){ 0, 0, 150, 30},            // COMBOBOX
        (Rectangle){ 0, 0, 20, 20},             // CHECKBOX
        (Rectangle){ 0, 0, 120, 30},            // TEXTBOX    
        (Rectangle){ 0, 0, 120, 40},            // GROUPBOX    
        (Rectangle){ 0, 0, 120, 48},            // WINDOWBOX    
        (Rectangle){ 0, 0, 100, 100},           // DUMMYREC    
        (Rectangle){ 0, 0, 120, 20},            // DROPDOWNBOX    
        (Rectangle){ 0, 0, 200, 30},            // STATUSBAR
        (Rectangle){ 0, 0, 120, 250},           // LISTVIEW
        (Rectangle){ 0, 0, 120, 120}            // COLORPICKER
    };
    
    // List view required variables
    Rectangle listViewControls = { -200, 0, 140, 500 };

    Rectangle listViewControlsCounter = { GetScreenWidth() + 140, 0, 140, 500 };
    int counterListViewControlsCounter = 0;
    int startPosXListViewControlsCounter = GetScreenWidth() + 140;
    int deltaPosXListViewControlsCounter = 0;

    // ToggleGroup, ComboBox, DropdownBox use this data
    const char *list[3] = { "ONE", "TWO", "THREE" };
    
 
    const char *guiControls[19] = { 
        "LABEL", 
        "BUTTON", 
        "VALUEBOX",
        "TOGGLE", 
        "TOGGLEGROUP", 
        "SLIDER", 
        "SLIDERBAR", 
        "PROGRESSBAR", 
        "SPINNER", 
        "COMBOBOX", 
        "CHECKBOX", 
        "TEXTBOX",
        "GROUPBOX",
        "WINDOWBOX",
        "DUMMYREC",
        "DROPDOWNBOX",
        "STATUSBAR",
        "LISTVIEW",
        "COLORPICKER"
    };
    
    const char *guiControlsCounter[16] = { 
        "Control 000",
        "Control 001",
        "Control 002",
        "Control 003",
        "Control 004",
        "Control 005",
        "Control 006",
        "Control 007",
        "Control 008",
        "Control 009",
        "Control 010",
        "Control 011",
        "Control 012",
        "Control 013",
        "Control 014",
        "Control 015",
    };
    
    Texture2D tracemap = { 0 };
    Rectangle tracemapRec = { 0 };
    bool tracemapEditMode = false;
    float tracemapFade = 0.5f;
    Vector2 panOffset = { 0 };
    Vector2 prevPosition = { 0 };
    
    char prevControlText[32];
    char prevControlName[32];
    
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
        memset(layout.controls[i].text, 0, 32);
        memset(layout.controls[i].name, 0, 32);
        layout.controls[i].ap = &layout.anchors[0];  // By default, set parent anchor
    }
    
    // Very basic undo system (only for last selected rectangle changes)
    int undoSelectedControl = -1;
    Rectangle undoLastRec;     // Update when a control is selected
    
    //if (CTRL+Z) currentSelectedRec = undoLastRec;
    
    bool exitWindow = false;
    bool ultimateMessage = false;
    
    SetTargetFPS(120);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!exitWindow)    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_ESCAPE) && !textEditMode && !nameEditMode)/* || WindowShouldClose()*/
        {
            ultimateMessage = !ultimateMessage;
            selectedControl = -1;
            lockMode = false;
        }
        
        if (WindowShouldClose()) exitWindow = true;
        
        mouseX = GetMouseX();
        mouseY = GetMouseY();
        
        snapFrameCounter++;
        
        // Enables or disables snapMode if not in textEditMode
        if (IsKeyPressed(KEY_S) && (!textEditMode) && (!nameEditMode)) snapMode = !snapMode;
        
        // Enables or disables position reference information(anchor reference or global reference)
        if (IsKeyPressed(KEY_F) && (!textEditMode) && (!nameEditMode)) globalReference = !globalReference;
        
        // Checks if the defaultRec[selectedType] is colliding with the list of the controls
        if (CheckCollisionPointRec(GetMousePosition(), listViewControls)) controlCollision = true;
        else if (CheckCollisionPointRec(GetMousePosition(), listViewControlsCounter)) controlCollision = true;
        
        // Toggle on help info
        if (IsKeyPressed(KEY_TAB))
        {
            startPosXHelp = helpPosX;
            deltaPosXHelp = 0 - startPosXHelp;
            helpCounter = 0;
        }
        
        if (IsKeyDown(KEY_TAB))
        {
            helpCounter++;
            if (helpCounter >= 60) helpCounter = 60;
            helpPosX = (int)EaseCubicInOut(helpCounter, startPosXHelp, deltaPosXHelp, 60);
        }
        else if (IsKeyReleased(KEY_TAB))
        {
            startPosXHelp = helpPosX;
            deltaPosXHelp = -300 - startPosXHelp;
            helpCounter = 0;
        }
        else
        {
            helpCounter++;
            if (helpCounter >= 60) helpCounter = 60;
            helpPosX = (int)EaseCubicInOut(helpCounter, startPosXHelp, deltaPosXHelp, 60);
        }
        
        // Create new control 
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (selectedControl == -1) && !controlCollision && !anchorMode && !tracemapEditMode && !ultimateMessage)
        {
            // Add new control (button)
            layout.controls[layout.controlsCount].id = layout.controlsCount;
            layout.controls[layout.controlsCount].type = selectedType;
            layout.controls[layout.controlsCount].rec = (Rectangle){  mouseX - defaultRec[selectedType].width/2, mouseY - defaultRec[selectedType].height/2, defaultRec[selectedType].width, defaultRec[selectedType].height };
            if ((layout.controls[layout.controlsCount].type == LABEL) || (layout.controls[layout.controlsCount].type == TEXTBOX) || (layout.controls[layout.controlsCount].type == BUTTON) || (layout.controls[layout.controlsCount].type == TOGGLE)
                || (layout.controls[layout.controlsCount].type == GROUPBOX) || (layout.controls[layout.controlsCount].type == WINDOWBOX) || (layout.controls[layout.controlsCount].type == STATUSBAR) || (layout.controls[layout.controlsCount].type == DUMMYREC)) strcpy(layout.controls[layout.controlsCount].text, "SAMPLE TEXT");
            strcpy(layout.controls[layout.controlsCount].name, FormatText("%s%03i", controlTypeNameLow[layout.controls[layout.controlsCount].type], layout.controlsCount));
            layout.controls[layout.controlsCount].ap = &layout.anchors[0];        // Default anchor point (0, 0)
            
            for (int i = 0; i < layout.controlsCount; i++)
            {
                if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }) && layout.controls[i].type == WINDOWBOX) layout.controls[layout.controlsCount].ap =layout.controls[i].ap;
            }
            if (layout.controls[layout.controlsCount].type == WINDOWBOX)
            {
                for (int i = 1; i < MAX_ANCHOR_POINTS; i++)
                {
                    if (!layout.anchors[i].enabled)
                    {
                        layout.anchors[i].x = layout.controls[layout.controlsCount].rec.x;
                        layout.anchors[i].y = layout.controls[layout.controlsCount].rec.y;
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
        
        // Change controls layer order (position inside array)
        if (IsKeyDown(KEY_LEFT_ALT) && !lockMode)
        {
            if ((IsKeyPressed(KEY_UP)) && (selectedControl < layout.controlsCount - 1))
            {
                // Move control towards beginning of array
                GuiControl auxControl = layout.controls[selectedControl];
                layout.controls[selectedControl] = layout.controls[selectedControl + 1];
                layout.controls[selectedControl].id -= 1;
                layout.controls[selectedControl + 1] = auxControl;
                layout.controls[selectedControl + 1].id += 1;
            }
            else if ((IsKeyPressed(KEY_DOWN)) && (selectedControl > 0))
            {
                // Move control towards end of array
                GuiControl auxControl = layout.controls[selectedControl];
                layout.controls[selectedControl] = layout.controls[selectedControl - 1];
                layout.controls[selectedControl].id += 1;
                layout.controls[selectedControl - 1] = auxControl;
                layout.controls[selectedControl - 1].id -= 1;
            }
        }
        
        if (!(controlDrag || lockMode || tracemapEditMode || lockAnchorMode || ultimateMessage))
        {
            // Check selected control (on mouse hover)
            for (int i = layout.controlsCount; i >= 0; i--)
            {
                if (controlDrag || lockMode || tracemapEditMode || lockAnchorMode) break;
                if ((layout.controls[i].type == WINDOWBOX) && (!layout.controls[i].ap->hidding) && (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, 24 })))
                {
                    selectedControl = i;
                    if (undoSelectedControl != selectedControl) 
                    {
                        undoSelectedControl = selectedControl;
                        undoLastRec = layout.controls[i].rec;
                    }
                    break;
                }
                else if ((!layout.controls[i].ap->hidding) && (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }) && layout.controls[i].type != WINDOWBOX))
                {
                    selectedControl = i;
                    if (undoSelectedControl != selectedControl) 
                    {
                        undoSelectedControl = selectedControl;
                        undoLastRec = layout.controls[i].rec;
                    }
                    break;
                }
                else selectedControl = -1;
            }
        }
        
        if (selectedControl != -1 && !textEditMode && !nameEditMode && !anchorMode)
        {
            // Disables the defaultRec[selectedType]
            controlCollision = true;
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                controlDrag = true;
                panOffset = (Vector2){ mouseX, mouseY };
                prevPosition = (Vector2){ layout.controls[selectedControl].rec.x, layout.controls[selectedControl].rec.y  };
            }
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                controlDrag = false;
            } 
             
            if (controlDrag && !lockMode)
            {       

                layout.controls[selectedControl].rec.x = prevPosition.x + (mouseX - panOffset.x);
                layout.controls[selectedControl].rec.y = prevPosition.y + (mouseY - panOffset.y);
                                  

                // Snap to grid position and size
                if (snapMode)
                {
                    // Snap rectangle position to closer snap point
                    int offsetX = layout.controls[selectedControl].rec.x%GRID_LINE_SPACING;
                    int offsetY = layout.controls[selectedControl].rec.y%GRID_LINE_SPACING;
                    
                    if (offsetX >= GRID_LINE_SPACING/2) layout.controls[selectedControl].rec.x += (GRID_LINE_SPACING - offsetX);
                    else layout.controls[selectedControl].rec.x -= offsetX;
                    
                    if (offsetY >= GRID_LINE_SPACING/2) layout.controls[selectedControl].rec.y += (GRID_LINE_SPACING - offsetY);
                    else layout.controls[selectedControl].rec.y -= offsetY;
                }
            }
            
            if (!IsKeyDown(KEY_LEFT_ALT))
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
                            if (IsKeyDown(KEY_RIGHT) && ((snapFrameCounter%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.width += GRID_LINE_SPACING;
                            else if (IsKeyDown(KEY_LEFT) && ((snapFrameCounter%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.width -= GRID_LINE_SPACING;

                            if (IsKeyDown(KEY_UP) && ((snapFrameCounter%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.height -= GRID_LINE_SPACING;
                            else if (IsKeyDown(KEY_DOWN) && ((snapFrameCounter%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.height += GRID_LINE_SPACING;
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
                            if (IsKeyDown(KEY_RIGHT) && ((snapFrameCounter%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.x += GRID_LINE_SPACING;
                            else if (IsKeyDown(KEY_LEFT) && ((snapFrameCounter%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.x -= GRID_LINE_SPACING;
                            
                            if (IsKeyDown(KEY_UP) && ((snapFrameCounter%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.y -= GRID_LINE_SPACING;
                            else if (IsKeyDown(KEY_DOWN) && ((snapFrameCounter%MOVEMENT_FRAME_SPEED) == 0)) layout.controls[selectedControl].rec.y += GRID_LINE_SPACING;
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
            // Delete selected control and shift array position
            if (IsKeyPressed(KEY_DELETE))
            {
                for (int i = selectedControl; i < layout.controlsCount; i++)
                {
                    layout.controls[i].type = layout.controls[i + 1].type;
                    layout.controls[i].rec = layout.controls[i + 1].rec;
                    strcpy(layout.controls[i].text, "\0");
                    strcpy(layout.controls[i].name, "\0");
                    strcpy(layout.controls[i].text, layout.controls[i + 1].text);
                    strcpy(layout.controls[i].name, layout.controls[i + 1].name);
                    layout.controls[i].ap = layout.controls[i + 1].ap;
                }

                layout.controlsCount--;
                selectedControl = -1;
            }
            
            // Unlinks the control selected from its current anchor
            if (IsKeyPressed(KEY_U))
            {
                layout.controls[selectedControl].rec.x += layout.controls[selectedControl].ap->x;
                layout.controls[selectedControl].rec.y += layout.controls[selectedControl].ap->y;
                layout.controls[selectedControl].ap = &layout.anchors[0];
            }
        }
        else
        {
            // Check mouse hover a list view
            if (!CheckCollisionPointRec(GetMousePosition(), listViewControls)) controlCollision = false;
            if (!CheckCollisionPointRec(GetMousePosition(), listViewControlsCounter)) controlCollision = false;
            
            // Updates the selectedType with the MouseWheel
            selectedType -= GetMouseWheelMove();
            
            if (selectedType < LABEL) selectedType = COLORPICKER;
            else if (selectedType > COLORPICKER) selectedType = LABEL;
            
            selectedTypeDraw = selectedType;
        }
        
        // Updates the defaultRec[selectedType] position
        defaultRec[selectedType].x = mouseX - defaultRec[selectedType].width/2;
        defaultRec[selectedType].y = mouseY - defaultRec[selectedType].height/2;

        // Mouse snap
        // NOTE: Snap point changes when GRID_LINE_SPACING/2 has been surpassed in X and Y
        if ((snapMode) && (selectedControl == -1))
        {
            int offsetX = mouseX%GRID_LINE_SPACING;
            int offsetY = mouseY%GRID_LINE_SPACING;
            
            if (offsetX >= GRID_LINE_SPACING/2) mouseX += (GRID_LINE_SPACING - offsetX);
            else mouseX -= offsetX;
            
            if (offsetY >= GRID_LINE_SPACING/2) mouseY += (GRID_LINE_SPACING - offsetY);
            else mouseY -= offsetY;
            
            // SnapMode of the DrawingControls
            // Snap rectangle position to closer snap point
            offsetX = defaultRec[selectedType].x%GRID_LINE_SPACING;
            offsetY = defaultRec[selectedType].y%GRID_LINE_SPACING;

            if (offsetX >= GRID_LINE_SPACING/2) defaultRec[selectedType].x += (GRID_LINE_SPACING - offsetX);
            else defaultRec[selectedType].x -= offsetX;

            if (offsetY >= GRID_LINE_SPACING/2) defaultRec[selectedType].y += (GRID_LINE_SPACING - offsetY);
            else defaultRec[selectedType].y -= offsetY;      
        }
        
        // Resize the controller aplying the snap
        if (!textEditMode && !nameEditMode && IsKeyPressed(KEY_R) && selectedControl != -1)
        {
            int offsetX = layout.controls[selectedControl].rec.width%GRID_LINE_SPACING;
            int offsetY = layout.controls[selectedControl].rec.height%GRID_LINE_SPACING;
            
            if (offsetX >= GRID_LINE_SPACING/2) layout.controls[selectedControl].rec.width += (GRID_LINE_SPACING - offsetX);
            else layout.controls[selectedControl].rec.width -= offsetX;
            
            if (offsetY >= GRID_LINE_SPACING/2) layout.controls[selectedControl].rec.height += (GRID_LINE_SPACING - offsetY);
            else layout.controls[selectedControl].rec.height -= offsetY;
        }
        
        // Check if control has text to edit
        if (textEditMode)
        {
            // Locks the selectedControl for text editing
            selectedControl = saveControlSelected;
            int key = GetKeyPressed();
            int keyCount = strlen(layout.controls[selectedControl].text); // Keeps track of text length
       
            // Replaces characters with pressed keys or '\0' in case of backspace
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (keyCount < 31))
            {
                layout.controls[selectedControl].text[keyCount] = (unsigned char)key;
            }
            
            if (IsKeyPressed(KEY_BACKSPACE_TEXT))
            {
                layout.controls[selectedControl].text[keyCount - 1] = '\0';
                if (keyCount < 0) keyCount = 0;
            }
            
            // Used to show the cursor('|') in textEditMode 
            if (keyCount < 32) framesCounter++;
            else if (keyCount == 32) framesCounter = 21;
        }
        
        if ((nameEditMode))
        {
            // Locks the selectedControl for text editing
            selectedControl = saveControlSelected;
            int key = GetKeyPressed();
            int keyCount = strlen(layout.controls[selectedControl].name); // Keeps track of name length
       
            // Replaces characters with pressed keys or '\0' in case of backspace
            // NOTE: Only allow keys in range [32..125]
            if ((((key >= 65) && (key < 91)) || ((key > 96) && (key <= 122))) && (keyCount < 31))
            {
                layout.controls[selectedControl].name[keyCount] = (unsigned char)key;
            }
            
            if (IsKeyPressed(KEY_BACKSPACE_TEXT))
            {
                layout.controls[selectedControl].name[keyCount - 1] = '\0';
                if (keyCount < 0) keyCount = 0;
            }
            
            // Used to show the cursor('|') in textEditMode 
            if (keyCount < 32) framesCounter++;
            else if (keyCount == 32) framesCounter = 21;
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
            strcpy(layout.controls[selectedControl].name, prevControlName);
            framesCounter = 0;
        }
        
        // Turns on textEditMode
        if (IsKeyPressed(KEY_T) && !nameEditMode && (selectedControl != -1) && (!anchorMode) &&
           ((layout.controls[selectedControl].type == LABEL) || (layout.controls[selectedControl].type == TEXTBOX) || (layout.controls[selectedControl].type == BUTTON) || (layout.controls[selectedControl].type == TOGGLE) || (layout.controls[selectedControl].type == GROUPBOX) || (layout.controls[selectedControl].type == WINDOWBOX) || (layout.controls[selectedControl].type == STATUSBAR) || (layout.controls[selectedControl].type == DUMMYREC)))
        {   
            textEditMode = true;
            saveControlSelected = selectedControl;
            strcpy(prevControlText, layout.controls[selectedControl].text);
        }
        
        // Turns on NameEditMode
        if (IsKeyPressed(KEY_N) && !textEditMode && (selectedControl != -1))
        {
            nameEditMode = true;
            strcpy(prevControlName, layout.controls[selectedControl].name);
            saveControlSelected = selectedControl;
        }
       
        // Selected control lock logic
        if (lockMode) selectedControl = saveControlSelected;

        if (IsKeyPressed(KEY_SPACE) && !nameEditMode && !textEditMode && (selectedControl != -1) && !lockMode && !anchorMode) 
        {
            lockMode = true;
            saveControlSelected = selectedControl;
        }
        else if (IsKeyPressed(KEY_SPACE) && (selectedControl != -1)) lockMode = false;

        // Checks if mouse is over an anchor
        if (!(anchorLinkMode || controlDrag || lockAnchorMode))
        {
            for (int i = 1; i < MAX_ANCHOR_POINTS; i++)
            {
                if (CheckCollisionPointCircle(GetMousePosition(), (Vector2){ layout.anchors[i].x, layout.anchors[i].y }, ANCHOR_RADIUS))
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
        if (lockAnchorMode)
        {
            selectedAnchor = saveAnchorSelected;
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
                    if (IsKeyDown(KEY_RIGHT) && ((snapFrameCounter%MOVEMENT_FRAME_SPEED) == 0)) layout.anchors[selectedAnchor].x+= GRID_LINE_SPACING;
                    else if (IsKeyDown(KEY_LEFT) && ((snapFrameCounter%MOVEMENT_FRAME_SPEED) == 0)) layout.anchors[selectedAnchor].x-= GRID_LINE_SPACING;
                    
                    if (IsKeyDown(KEY_UP) && ((snapFrameCounter%MOVEMENT_FRAME_SPEED) == 0)) layout.anchors[selectedAnchor].y-= GRID_LINE_SPACING;
                    else if (IsKeyDown(KEY_DOWN) && ((snapFrameCounter%MOVEMENT_FRAME_SPEED) == 0)) layout.anchors[selectedAnchor].y+= GRID_LINE_SPACING;
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
        if (anchorMode)
        {
            // On mouse click anchor is created
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !controlCollision && (selectedAnchor == -1) && (selectedControl == -1))
            {
                for (int i = 1; i < MAX_ANCHOR_POINTS; i++)
                {
                    if (!layout.anchors[i].enabled)
                    {
                        layout.anchors[i].x = mouseX;
                        layout.anchors[i].y = mouseY;
                        layout.anchors[i].enabled = true;
                        anchorMode = false;
                        break;
                    }
                }
            }

            if (IsKeyPressed(KEY_SPACE) && !nameEditMode && !textEditMode && (selectedAnchor != -1) && !lockAnchorMode && anchorMode) 
            {
                lockAnchorMode = true;
                saveAnchorSelected = selectedAnchor;
            }
            else if (IsKeyPressed(KEY_SPACE) && (selectedAnchor != -1)) lockAnchorMode = false;
            
            if (selectedAnchor > 0)
            {
                // Unlinks and deletes the selected anchor point
                if (IsKeyPressed(KEY_DELETE))
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
                    lockAnchorMode = false;
                }
                
                if (!lockAnchorMode)
                {
                    // Allows to drag an anchor without losing collision
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && IsKeyDown(KEY_A))
                    {
                        controlDrag = true;
                        anchorNewPos = true;
                        
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
                        if (anchorNewPos)
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
                            
                            anchorNewPos = false;
                        }
                        
                        controlDrag = false;
                        selectedAnchor = -1;
                        anchorMode = false;
                    }
                    
                    // Moves the anchor to the mouse position
                    if (controlDrag)
                    {
                        layout.anchors[selectedAnchor].x = mouseX;
                        layout.anchors[selectedAnchor].y = mouseY;
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
                    if (selectedControl != -1 && !lockMode)
                    {
                        layout.controls[selectedControl].rec.x += layout.controls[selectedControl].ap->x;
                        layout.controls[selectedControl].rec.y += layout.controls[selectedControl].ap->y;
                        layout.controls[selectedControl].ap = &layout.anchors[linkedAnchor];
                        layout.controls[selectedControl].rec.x -= layout.anchors[linkedAnchor].x;
                        layout.controls[selectedControl].rec.y -= layout.anchors[linkedAnchor].y;
                    }
                    
                    anchorLinkMode = false;  
                    anchorMode = false;                    
                }
                
                // Hide/Unhide selected anchor linked controls
                if (IsKeyPressed(KEY_H)) layout.anchors[selectedAnchor].hidding = !layout.anchors[selectedAnchor].hidding;
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
            else if (layout.controls[selectedControl].type == PROGRESSBAR || layout.controls[selectedControl].type == SLIDER || layout.controls[selectedControl].type == SLIDERBAR || layout.controls[selectedControl].type == CHECKBOX)
            {
                if (layout.controls[selectedControl].rec.height <= 10 ) layout.controls[selectedControl].rec.height = 10;
            }
            else if (layout.controls[selectedControl].rec.height <= 20) layout.controls[selectedControl].rec.height = 20;
        }
  
        // Shows or hides the grid
        if (IsKeyPressed(KEY_G) && (!nameEditMode) && (!textEditMode)) showGrid = !showGrid;
        
        if (IsFileDropped())
        {
            char name[256]; 
            droppedFiles = GetDroppedFiles(&fileCount);
            strcpy(name, droppedFiles[0]);
            
            if (IsFileExtension(name, ".rgl")) LoadLayoutRGL(name);
            else if (IsFileExtension(name, ".rgs")) GuiLoadStyle(name);
            else if (IsFileExtension(name, ".png"))
            {
                if (tracemap.id > 0) UnloadTexture(tracemap);
                tracemap = LoadTexture(name);
                
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
     
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
        {
            // Save file dialog
            const char *filters[] = { "*.rgl" };
            char *fileName = tinyfd_saveFileDialog("Save raygui layout text file", "", 1, filters, "raygui Layout Files (*.rgl)");

            // Save layout.controls file (text or binary)
            if (fileName != NULL) 
            {
                if (GetExtension(fileName) == NULL) strcat(fileName, ".rgl\0");     // No extension provided
                SaveLayoutRGL(fileName, false);
            }
        }
        
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O))
        {
            // Open file dialog
            const char *filters[] = { "*.rgl" };
            const char *fileName = tinyfd_openFileDialog("Load raygui layout file", "", 1, filters, "raygui Layout Files (*.rgl)", 0);
            
            if (fileName != NULL) LoadLayoutRGL(fileName);
        }
        
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_ENTER)) 
        {
            // Save file dialog
            // TODO: Support additional languages (.lua, .go...) code generation
            const char *filters[] = { "*.c", "*.go", "*.lua" };
            const char *fileName = tinyfd_saveFileDialog("Generate code file", "layout.c", 3, filters, "Code file");
            
            if (fileName != NULL)
            {
                GuiLayoutConfig config;
                memset(&config, 0, sizeof(GuiLayoutConfig));
                
                config.width = 800;
                config.height = 600;
                strcpy(config.name, "file_name");
                strcpy(config.version, "1.0-dev");
                strcpy(config.company, "raylib tech");
                strcpy(config.description, "tool description sample");
                config.defineRecs = false;
                config.exportAnchors = true;
                config.exportAnchor0 = false;
                config.fullComments = true;
                
                GenerateCode(fileName, config);
            }
        }
        
        // Tracemap texture control logic
        if (tracemap.id > 0)
        {
            // Toggles Texture editting mode between true or false
            if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_T)) tracemapEditMode = !tracemapEditMode;
            
            if (tracemapEditMode)
            {
                int offsetX = mouseX%GRID_LINE_SPACING;
                int offsetY = mouseY%GRID_LINE_SPACING;
                
                // Moves the texture with the mouse
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    panOffset = (Vector2){ mouseX, mouseY };
                    prevPosition = (Vector2){ tracemapRec.x, tracemapRec.y };
                }
                
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                {
                    tracemapRec.x = prevPosition.x + (mouseX - panOffset.x);
                    tracemapRec.y = prevPosition.y + (mouseY - panOffset.y);
                        
                    if (snapMode)
                    {
                        if (offsetX >= GRID_LINE_SPACING/2) mouseX += (GRID_LINE_SPACING - offsetX);
                        else mouseX -= offsetX;
                        
                        if (offsetY >= GRID_LINE_SPACING/2) mouseY += (GRID_LINE_SPACING - offsetY);
                        else mouseY -= offsetY;
                        
                        offsetX = tracemapRec.x%GRID_LINE_SPACING;
                        offsetY = tracemapRec.y%GRID_LINE_SPACING;
                        
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
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            // TODO: Draw global app screen limits (black rectangle with black default anchor)

            if (showGrid) DrawGrid2D(GetScreenWidth(), GetScreenHeight(), GRID_LINE_SPACING);
            
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
                        case LABEL: GuiLabel((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text); break;
                        case BUTTON: GuiButton((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text); break;
                        case VALUEBOX: GuiValueBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, 42, 100); break;
                        case TOGGLE: GuiToggleButton((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text, false); break;
                        case TOGGLEGROUP: GuiToggleGroup((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, list, 3, 1); break;
                        case SLIDER: GuiSlider((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, 42, 0, 100); break;
                        case SLIDERBAR: GuiSliderBar((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, 40, 0, 100); break;
                        case PROGRESSBAR: GuiProgressBar((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, 40, 0, 100); break;
                        case SPINNER: GuiSpinner((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, 42, 3, 25); break;
                        case COMBOBOX: GuiComboBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, list, 3, 1); break;
                        case CHECKBOX: GuiCheckBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, false); break;
                        case TEXTBOX: GuiTextBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text, 32, false); break;
                        case GROUPBOX: GuiGroupBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text); break;
                        case WINDOWBOX: 
                        {
                            GuiFade(0.8f);
                            GuiWindowBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text);
                            GuiFade(1.0f);
                        }break;
                        case DUMMYREC: GuiDummyRec((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text); break;
                        case DROPDOWNBOX: GuiDropdownBox((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, list, 3, 2); break;
                        case STATUSBAR: GuiStatusBar((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, layout.controls[i].text, 15); break;
                        case LISTVIEW: GuiListView((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, guiControls, 14, 1); break;
                        case COLORPICKER: GuiColorPicker((Rectangle){ layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height }, RED); break;
                        default: break;
                    }
                    
                    if ((layout.controls[i].ap->id == selectedAnchor) && (layout.controls[i].ap->id > 0)) DrawLine(layout.controls[i].ap->x, layout.controls[i].ap->y, layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, RED);
                    
                }
                else if ((layout.controls[i].ap->id == selectedAnchor) && (layout.controls[i].ap->id > 0)) DrawLine(layout.controls[i].ap->x, layout.controls[i].ap->y, layout.controls[i].ap->x + layout.controls[i].rec.x, layout.controls[i].ap->y + layout.controls[i].rec.y, BLUE);
            }
            
            // Draws the defaultRec[selectedType] of the control selected
            if (selectedControl == -1 && !anchorMode && !tracemapEditMode && !ultimateMessage)
            {
                switch (selectedTypeDraw)
                {
                    case LABEL: GuiLabel(defaultRec[selectedTypeDraw], "TEXT SAMPLE"); break;
                    case BUTTON: GuiButton(defaultRec[selectedTypeDraw], "BUTTON"); break;
                    case VALUEBOX: GuiValueBox(defaultRec[selectedTypeDraw], 42, 100); break;
                    case TOGGLE: GuiToggleButton(defaultRec[selectedTypeDraw], "TOGGLE", false); break;
                    case TOGGLEGROUP: GuiToggleGroup(defaultRec[selectedTypeDraw], list, 3, 1); break;
                    case SLIDER: GuiSlider(defaultRec[selectedTypeDraw], 42, 0, 100); break;
                    case SLIDERBAR: GuiSliderBar(defaultRec[selectedTypeDraw], 40, 0, 100); break;
                    case PROGRESSBAR: GuiProgressBar(defaultRec[selectedTypeDraw], 40, 0, 100); break;
                    case SPINNER: GuiSpinner(defaultRec[selectedTypeDraw], 42, 3, 25); break;
                    case COMBOBOX: GuiComboBox(defaultRec[selectedTypeDraw], list, 3, 1); break;
                    case CHECKBOX: GuiCheckBox(defaultRec[selectedTypeDraw], false); break;
                    case TEXTBOX: GuiTextBox(defaultRec[selectedTypeDraw], previewText, 7, false); break;
                    case GROUPBOX: GuiGroupBox(defaultRec[selectedTypeDraw], "GROUP BOX"); break;
                    case WINDOWBOX: GuiWindowBox(defaultRec[selectedTypeDraw], "WINDOW BOX"); break;
                    case DUMMYREC: GuiDummyRec(defaultRec[selectedTypeDraw], "DUMMY REC"); break;
                    case DROPDOWNBOX: GuiDropdownBox(defaultRec[selectedTypeDraw], list, 3, 2); break;
                    case STATUSBAR: GuiStatusBar(defaultRec[selectedTypeDraw], "STATUS BAR", 15); break;
                    case LISTVIEW: GuiListView(defaultRec[selectedTypeDraw], guiControls, 14, 1); break;
                    case COLORPICKER: GuiColorPicker(defaultRec[selectedTypeDraw], RED); break;
                    default: break;
                }
            }

            // Draw the anchorPoints
            for (int i = 0; i < MAX_ANCHOR_POINTS; i++)
            {
                if ((layout.anchors[i].enabled) && (layout.anchors[i].x != 0) && (layout.anchors[i].y != 0))
                {
                    if (layout.anchors[i].id == selectedAnchor && anchorNewPos)
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
            
            // Draw selected control selection rectangle (transparent RED)
            if ((selectedControl != -1) && (selectedControl < layout.controlsCount))
            {
                DrawRectangleRec((Rectangle){ layout.controls[selectedControl].ap->x + layout.controls[selectedControl].rec.x, layout.controls[selectedControl].ap->y + layout.controls[selectedControl].rec.y, layout.controls[selectedControl].rec.width, layout.controls[selectedControl].rec.height }, Fade(RED, 0.5f));
                
                // Draw anchor lines (if not hidden)
                if (layout.controls[selectedControl].ap->id > 0 && !layout.controls[selectedControl].ap->hidding) DrawLine(layout.controls[selectedControl].ap->x, layout.controls[selectedControl].ap->y, layout.controls[selectedControl].ap->x + layout.controls[selectedControl].rec.x, layout.controls[selectedControl].ap->y + layout.controls[selectedControl].rec.y, RED);
                else if (layout.controls[selectedControl].ap->id > 0 && layout.controls[selectedControl].ap->hidding) DrawLine(layout.controls[selectedControl].ap->x, layout.controls[selectedControl].ap->y, layout.controls[selectedControl].ap->x + layout.controls[selectedControl].rec.x, layout.controls[selectedControl].ap->y + layout.controls[selectedControl].rec.y, BLUE);
            }
            
            // Draw cursor (control mode or anchor mode)
            if ((selectedControl == -1)  && (selectedAnchor == -1))
            {
                if (anchorMode)
                {
                    DrawCircleLines(mouseX, mouseY, ANCHOR_RADIUS, Fade(RED, 0.5f));
                    DrawRectangle(mouseX - ANCHOR_RADIUS - 5, mouseY, ANCHOR_RADIUS*2 + 10, 1, RED);
                    DrawRectangle(mouseX , mouseY - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, RED);
                }
                else 
                {
                    DrawRectangle(mouseX - 8, mouseY, 17, 1, RED);
                    DrawRectangle(mouseX, mouseY - 8, 1, 17, RED);
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
                    else DrawText("|", layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x + layout.controls[selectedControl].rec.width/2 + MeasureText(layout.controls[selectedControl].text , style[DEFAULT_TEXT_SIZE])/2 + 2, layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y + layout.controls[selectedControl].rec.height/2 - 6, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
               }
            }
            
            // Draw nameEditMode
            if (nameEditMode)
            {
                DrawText(FormatText("%s", layout.controls[selectedControl].name), layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x, layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y + layout.controls[selectedControl].rec.height + 10, style[DEFAULT_TEXT_SIZE]*2, BLACK);
                    
                if (((framesCounter/20)%2) == 0) DrawText("|", layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x + MeasureText(layout.controls[selectedControl].name, style[DEFAULT_TEXT_SIZE]*2) + 2, layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y + layout.controls[selectedControl].rec.height + 10, style[DEFAULT_TEXT_SIZE]*2 + 2, BLACK);                   
            }
            else if (IsKeyDown(KEY_N))
            {
                for (int i = 0; i < layout.controlsCount; i++)
                {
                    // Draws the Controls when placed on the grid.
                    if (!layout.controls[i].ap->hidding)
                    DrawText(FormatText("%s", layout.controls[i].name), layout.controls[i].rec.x + layout.controls[i].ap->x, layout.controls[i].rec.y + layout.controls[i].ap->y + layout.controls[i].rec.height + 10, style[DEFAULT_TEXT_SIZE]*2, BLACK);
                }
            }


            // Draw anchor linking line
            if (anchorLinkMode) DrawLine(layout.anchors[linkedAnchor].x, layout.anchors[linkedAnchor].y, mouseX, mouseY, BLACK);

            // Draw Rectangle Info
            if (selectedControl != -1)
            {
                if (!globalReference) DrawText(FormatText("[%i, %i, %i, %i]", layout.controls[selectedControl].rec.x, layout.controls[selectedControl].rec.y, layout.controls[selectedControl].rec.width, layout.controls[selectedControl].rec.height), layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x, layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y - 30, 20, MAROON);
                else DrawText(FormatText("[%i, %i, %i, %i]", layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x, layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y, layout.controls[selectedControl].rec.width, layout.controls[selectedControl].rec.height), layout.controls[selectedControl].rec.x + layout.controls[selectedControl].ap->x, layout.controls[selectedControl].rec.y + layout.controls[selectedControl].ap->y - 30, 20, RED);
            }
                
            // Draw Image info
            if (tracemapEditMode) DrawText(FormatText("[%i, %i, %i, %i]", tracemapRec.x, tracemapRec.y, tracemapRec.width, tracemapRec.height), tracemapRec.x + 25, tracemapRec.y + 25, 20, MAROON);

            // Draw right panel for easy-finding controls
            
            // Draw the help list (by default is out of screen)
            if (helpPosX > -280)
            {
                DrawRectangleRec((Rectangle){ helpPosX + 20, 20, 275, 390 }, GetColor(style[DEFAULT_BACKGROUND_COLOR]));
                GuiGroupBox((Rectangle){ helpPosX + 20, 20, 275, 390 }, "Shortcuts");
                GuiLabel((Rectangle){ helpPosX + 30, 30, 0, 0 }, "G - Show/hide grid");
                GuiLabel((Rectangle){ helpPosX + 30, 50, 0, 0 }, "S - Toggle snap");
                GuiLabel((Rectangle){ helpPosX + 30, 70, 0, 0 }, "R - Resize to grid");
                GuiLabel((Rectangle){ helpPosX + 30, 90, 0, 0 }, "A - Anchor mode");
                GuiLabel((Rectangle){ helpPosX + 30, 110, 0, 0 }, "F - Reference position(global/anchor)");
                GuiLabel((Rectangle){ helpPosX + 30, 130, 0, 0 }, "H - Hide controls of selected anchor");
                GuiLabel((Rectangle){ helpPosX + 30, 150, 0, 0 }, "U - Unlink anchor");
                GuiLabel((Rectangle){ helpPosX + 30, 170, 0, 0 }, "Space - Lock/unlock control");
                GuiLabel((Rectangle){ helpPosX + 30, 190, 0, 0 }, "T - Enter text mode(if possible)");
                GuiLabel((Rectangle){ helpPosX + 30, 210, 0, 0 }, "Enter - Exit text mode");
                GuiLabel((Rectangle){ helpPosX + 30, 230, 0, 0 }, "Delete - Delete a control");
                GuiLabel((Rectangle){ helpPosX + 30, 250, 0, 0 }, "Arrows - Modify position(+ shift smooth)");
                GuiLabel((Rectangle){ helpPosX + 30, 270, 0, 0 }, "L. Ctrl + Arrows - Modify scale(+ shift smooth)");
                GuiLabel((Rectangle){ helpPosX + 30, 290, 0, 0 }, "L. Alt + Arrows(Up/Down) - Changes drawing order");
                GuiLabel((Rectangle){ helpPosX + 30, 310, 0, 0 }, "L. Ctrl + Enter - Export layout controls to code");
                GuiLabel((Rectangle){ helpPosX + 30, 330, 0, 0 }, "L. Ctrl + S - Save layout.controls(.rgl)");
                GuiLabel((Rectangle){ helpPosX + 30, 350, 0, 0 }, "L. Ctrl + O - Open layout.controls(.rgl)");
                GuiLabel((Rectangle){ helpPosX + 30, 370, 0, 0 }, "L. Ctrl + D - Duplicate selected control");
                GuiLabel((Rectangle){ helpPosX + 30, 390, 0, 0 }, "L. Ctrl + N - Rename control");
            }
            
            if (IsKeyDown(KEY_LEFT_ALT)) 
            {
                for (int i = layout.controlsCount - 1; i >= 0; i--) DrawText(FormatText("[%i]", layout.controls[i].id), layout.controls[i].rec.x + layout.controls[i].ap->x + layout.controls[i].rec.width, layout.controls[i].rec.y + layout.controls[i].ap->y - 10, 10, BLUE);  
            }
            
            // Draw status bar bottom with debug information
            GuiStatusBar((Rectangle){ 0, GetScreenHeight() - 24, 126, 24}, FormatText("MOUSE: (%i, %i)", mouseX, mouseY), 15);
            GuiStatusBar((Rectangle){ 124, GetScreenHeight() - 24, 81, 24}, (snapMode ? "SNAP: ON" : "SNAP: OFF"), 10);
            GuiStatusBar((Rectangle){ 204, GetScreenHeight() - 24, 145, 24}, FormatText("CONTROLS COUNT: %i", layout.controlsCount), 20);
            if (selectedControl != -1) GuiStatusBar((Rectangle){ 348, GetScreenHeight() - 24, GetScreenWidth() - 348, 24}, FormatText("SELECTED CONTROL: #%03i  |  %s  |  REC(%i, %i, %i, %i)  |  %s", selectedControl, controlTypeName[layout.controls[selectedControl].type], layout.controls[selectedControl].rec.x, layout.controls[selectedControl].rec.y, layout.controls[selectedControl].rec.width, layout.controls[selectedControl].rec.height, layout.controls[selectedControl].name), 15);
            else GuiStatusBar((Rectangle){ 348, GetScreenHeight() - 24, GetScreenWidth() - 348, 24}, "", 15);
            
            if (ultimateMessage)
            {
                if (GuiWindowBox((Rectangle){ GetScreenWidth()/2 - 125, GetScreenHeight()/2 - 50, 250, 100 }, "rGuiLayout")) ultimateMessage = false;
                GuiLabel((Rectangle){ GetScreenWidth()/2 - 95, GetScreenHeight()/2 - 60, 200, 100 }, "Do you want to save before quitting?");
                if (GuiButton((Rectangle){ GetScreenWidth()/2 - 90, GetScreenHeight()/2 + 10, 70, 25 }, "Yes")) 
                { 
                    // Save file dialog
                    const char *filters[] = { "*.rgl" };
                    char *fileName = tinyfd_saveFileDialog("Save raygui layout text file", "", 1, filters, "raygui Layout Files (*.rgl)");

                    // Save layout.controls file (text or binary)
                    if (fileName != NULL) 
                    {
                        if (GetExtension(fileName) == NULL) strcat(fileName, ".rgl\0");     // No extension provided
                        SaveLayoutRGL(fileName, false);
                    }
                    
                    exitWindow = true;
                }
                else if (GuiButton((Rectangle){ GetScreenWidth()/2 + 20, GetScreenHeight()/2 + 10, 70, 25 }, "No")) { exitWindow = true; }
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

// Draw 2d grid at specific size and spacing
static void DrawGrid2D(int width, int height, int spacing)
{
    #define GRID_ALPHA      0.1f          // Grid lines alpha amount

    int offset = 0;
    
    // Draw vertical grid lines
    for (int i = 0; i < (width/spacing + 1)*5; i++)
    {
        DrawRectangle(offset + spacing*i, 0, 1, height, ((i%5) == 0) ? Fade(BLACK, GRID_ALPHA*2) : Fade(GRAY, GRID_ALPHA));
    }

    // Draw horizontal grid lines
    for (int i = 0; i < (height/spacing + 1)*5; i++)
    {
        DrawRectangle(0, offset + spacing*i, width, 1, ((i%5) == 0) ? Fade(BLACK, GRID_ALPHA*2) : Fade(GRAY, GRID_ALPHA));
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
                fprintf(rglFile, "c %03i %i %s %i %i %i %i %i %s\n", layout.controls[i].id, layout.controls[i].type, layout.controls[i].name, layout.controls[i].rec.x, layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].ap->id, layout.controls[i].text);
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
                    sscanf(buffer, "c %d %i %s %i %i %i %i %d %[^\n]s", &layout.controls[layout.controlsCount].id, &layout.controls[layout.controlsCount].type, layout.controls[layout.controlsCount].name, &layout.controls[layout.controlsCount].rec.x, &layout.controls[layout.controlsCount].rec.y, &layout.controls[layout.controlsCount].rec.width, &layout.controls[layout.controlsCount].rec.height, &anchorId, layout.controls[layout.controlsCount].text);
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
    
    printf("get here!\n");
    
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
    fprintf(ftool, "    // Anchor points\n");

    for(int i = 0; i < MAX_ANCHOR_POINTS; i++)
    {
        for (int j = 0; j < layout.controlsCount; j++)
        {
            if (layout.controls[j].ap->id == layout.anchors[i].id)
            {
                fprintf(ftool, "    Vector2 %s%02i = { %i, %i };\n", "anchor", i, layout.anchors[i].x, layout.anchors[i].y);
                break;
            }
        }
    }

    // Define controls variables
    for (int i = 0; i < layout.controlsCount; i++)
    {
        switch (layout.controls[i].type)
        {
            // Bools
            case WINDOWBOX:
            {
                fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    bool %sActive = true;\n", layout.controls[i].name);
            }
            case TOGGLE: 
            case CHECKBOX: 
            {
                fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    bool %s = false;\n", layout.controls[i].name);
            }
            break;
            
            case DROPDOWNBOX:
            case COMBOBOX:
            case LISTVIEW:
            case TOGGLEGROUP:
            {
                fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    int %sCount = 3;\n", layout.controls[i].name);
                fprintf(ftool, "    int %sActive = 0;\n", layout.controls[i].name);
                fprintf(ftool, "    const char *%sList[3] = { \"ONE\", \"TWO\", \"THREE\" };\n", layout.controls[i].name);
            }
            break;
            
            // Floats
            case SLIDER: 
            case SLIDERBAR: 
            case PROGRESSBAR:
            {
                fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    float %sValue = 50.0f;\n", layout.controls[i].name);
            }            
            break;
            
            // Ints
            case VALUEBOX:
            case SPINNER:
            {
                fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    int %sValue = 0;\n", layout.controls[i].name);
            }     
            break;
            
            // Colors
            case COLORPICKER:
            {
                fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    Color %sColor;\n", layout.controls[i].name);
            }
            break;

            case TEXTBOX:
            {
                fprintf(ftool, "    \n\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
                fprintf(ftool, "    char %s[32] = \"%s\";\n", layout.controls[i].name, layout.controls[i].text);        
                fprintf(ftool, "    int %sSize = 32;\n", layout.controls[i].name);        
            }
            default: break;
        }
    }
    
    fprintf(ftool, "\n");

    if (config.defineRecs)
    {
        // Define controls rectangles
        fprintf(ftool, "    // Define controls rectangles\n");
        fprintf(ftool, "    Rectangle layoutRecs[%i] = {\n", layout.controlsCount);
        
        for (int i = 0; i < layout.controlsCount; i++)
        {
            fprintf(ftool, "        (Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }", "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height);
            
            if (i == layout.controlsCount - 1) fprintf(ftool, "\t\t// %s: %s\n    };\n\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
            else fprintf(ftool, ",\t\t// %s: %s\n", controlTypeNameLow[layout.controls[i].type], layout.controls[i].name);
        }
    }
    
    fprintf(ftool, "    //----------------------------------------------------------------------------------\n");
    
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
    fprintf(ftool, "            ClearBackground(GuiBackgroundColor());\n\n");

    fprintf(ftool, "\t\t\t// raygui: controls drawing\n");
    fprintf(ftool, "\t\t\t//----------------------------------------------------------------------------------\n");
    
    // Draw all controls
    if (!config.defineRecs)
    {
        for (int i = 0; i < layout.controlsCount; i++)
        {
            switch (layout.controls[i].type)
            {
                case LABEL: fprintf(ftool, "\t\t\tGuiLabel((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\");\n", "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].text); break;
                case BUTTON: fprintf(ftool, "\t\t\tif (GuiButton((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\")) %s(); \n", "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].text, layout.controls[i].name); break;
                case VALUEBOX: fprintf(ftool, "\t\t\t%sValue = GuiValueBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sValue, 100); \n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].name); break;
                case TOGGLE: fprintf(ftool, "\t\t\t%s = GuiToggleButton((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\", %s); \n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].text, layout.controls[i].name); break;
                case TOGGLEGROUP: fprintf(ftool, "\t\t\t%sActive = GuiToggleGroup((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sList, %sCount, %sActive); \n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].name, layout.controls[i].name, layout.controls[i].name); break;
                case SLIDER: fprintf(ftool, "\t\t\t%sValue = GuiSlider((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sValue, 0, 100);\n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].name); break;
                case SLIDERBAR: fprintf(ftool, "\t\t\t%sValue = GuiSliderBar((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sValue, 0, 100);\n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].name); break;
                case PROGRESSBAR: fprintf(ftool, "\t\t\t%sValue = GuiProgressBar((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sValue, 0, 100);\n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].name); break;
                case SPINNER: fprintf(ftool, "\t\t\t%sValue = GuiSpinner((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sValue, 100, 25);\n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].name); break;
                case COMBOBOX: fprintf(ftool, "\t\t\t%sActive = GuiComboBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i },  %sList, %sCount, %sActive); \n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].name, layout.controls[i].name, layout.controls[i].name); break;
                case CHECKBOX: fprintf(ftool, "\t\t\t%s = GuiCheckBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %s); \n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].name); break;
                case LISTVIEW: fprintf(ftool, "\t\t\t%sActive = GuiListView((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sList, %sCount, %sActive); \n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].name, layout.controls[i].name, layout.controls[i].name); break;
                case TEXTBOX: fprintf(ftool, "\t\t\tGuiTextBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %s, %sSize, true);\n", "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].name, layout.controls[i].name); break;
                case GROUPBOX: fprintf(ftool, "\t\t\tGuiGroupBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\");\n", "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].text); break;
                case WINDOWBOX:
                {
                    fprintf(ftool, "\t\t\tif (%sActive)\n\t\t\t{\n", layout.controls[i].name);
                    fprintf(ftool, "\t\t\t\t%sActive = !GuiWindowBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\");\n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].text);
                    fprintf(ftool, "\t\t\t}\n");
                }break;
                case DUMMYREC: fprintf(ftool, "\t\t\tGuiDummyRec((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\");\n", "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].text); break;
                case DROPDOWNBOX: fprintf(ftool, "\t\t\t%sActive = GuiDropdownBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sList, %sCount, %sActive); \n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].name, layout.controls[i].name, layout.controls[i].name); break;
                case STATUSBAR: fprintf(ftool, "\t\t\tGuiStatusBar((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\", 10);\n", "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].text); break;
                case COLORPICKER: fprintf(ftool, "\t\t\t%sColor = GuiColorPicker((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sColor);\n", layout.controls[i].name, "anchor", layout.controls[i].ap->id, ".x", layout.controls[i].rec.x, "anchor", layout.controls[i].ap->id, ".y", layout.controls[i].rec.y, layout.controls[i].rec.width, layout.controls[i].rec.height, layout.controls[i].name); break;
                
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
                case LABEL: fprintf(ftool, "\t\t\tGuiLabel(layoutRecs[%i], \"%s\");\n", i, layout.controls[i].text); break;
                case BUTTON: fprintf(ftool, "\t\t\tif (GuiButton(layoutRecs[%i], \"%s\")) %s(); \n", i, layout.controls[i].text, layout.controls[i].name); break;
                case VALUEBOX: fprintf(ftool, "\t\t\t%sValue = GuiValueBox(layoutRecs[%i], %sValue, 100); \n",layout.controls[i].name, i, layout.controls[i].name); break;
                case TOGGLE: fprintf(ftool, "\t\t\t%s = GuiToggleButton(layoutRecs[%i], \"%s\", %s); \n", layout.controls[i].name, i, layout.controls[i].text, layout.controls[i].name); break;
                case TOGGLEGROUP: fprintf(ftool, "\t\t\t%sActive = GuiToggleGroup(layoutRecs[%i], %sList, %sCount, %sActive); \n", layout.controls[i].name, i, layout.controls[i].name, layout.controls[i].name, layout.controls[i].name); break;
                case SLIDER: fprintf(ftool, "\t\t\t%sValue = GuiSlider(layoutRecs[%i], %sValue, %sMinValue, %sMaxValue);\n", layout.controls[i].name, i, layout.controls[i].name, layout.controls[i].name, layout.controls[i].name); break;
                case SLIDERBAR: fprintf(ftool, "\t\t\t%sValue = GuiSliderBar(layoutRecs[%i], %sValue, %sMinValue, %sMaxValue);\n", layout.controls[i].name, i, layout.controls[i].name, layout.controls[i].name, layout.controls[i].name); break;
                case PROGRESSBAR: fprintf(ftool, "\t\t\t%sValue = GuiProgressBar(layoutRecs[%i], %sValue, %sMinValue, %sMaxValue);\n", layout.controls[i].name, i, layout.controls[i].name, layout.controls[i].name, layout.controls[i].name); break;
                case SPINNER: fprintf(ftool, "\t\t\t%sValue = GuiSpinner(layoutRecs[%i], %sValue, 100, 25);\n", layout.controls[i].name, i, layout.controls[i].name); break;
                case COMBOBOX: fprintf(ftool, "\t\t\t%sActive = GuiComboBox(layoutRecs[%i],  %sList, %sCount, %sActive); \n", layout.controls[i].name, i, layout.controls[i].name, layout.controls[i].name, layout.controls[i].name); break;
                case CHECKBOX: fprintf(ftool, "\t\t\t%s = GuiCheckBox(layoutRecs[%i], %s); \n", layout.controls[i].name, i, layout.controls[i].name); break;
                case LISTVIEW: fprintf(ftool, "\t\t\t%sActive = GuiListView(layoutRecs[%i], %sList, %sCount, %sActive); \n", layout.controls[i].name, i, layout.controls[i].name, layout.controls[i].name, layout.controls[i].name); break;
                case TEXTBOX: fprintf(ftool, "\t\t\tGuiTextBox(layoutRecs[%i], %s, %sSize, true);\n", i, layout.controls[i].name, layout.controls[i].name); break;
                case GROUPBOX: fprintf(ftool, "\t\t\tGuiGroupBox(layoutRecs[%i], \"%s\");\n", i, layout.controls[i].text); break;
                case WINDOWBOX:
                {
                    fprintf(ftool, "\t\t\tif (%sActive)\n\t\t\t{\n", layout.controls[i].name);
                    fprintf(ftool, "\t\t\t\t%sActive = !GuiWindowBox(layoutRecs[%i], \"%s\");\n", layout.controls[i].name, i, layout.controls[i].text);
                    fprintf(ftool, "\t\t\t}\n");
                }break;
                case DUMMYREC: fprintf(ftool, "\t\t\tGuiDummyRec(layoutRecs[%i], \"%s\");\n", i, layout.controls[i].text); break;
                case DROPDOWNBOX: fprintf(ftool, "\t\t\t%sActive = GuiDropdownBox(layoutRecs[%i], %sList, %sCount, %sActive); \n", layout.controls[i].name, i, layout.controls[i].name, layout.controls[i].name, layout.controls[i].name); break;
                case STATUSBAR: fprintf(ftool, "\t\t\tGuiStatusBar(layoutRecs[%i], \"%s\", 10);\n", i, layout.controls[i].text); break;
                case COLORPICKER: fprintf(ftool, "\t\t\t%sColor = GuiColorPicker(layoutRecs[%i], %sColor);\n", layout.controls[i].name, i, layout.controls[i].name); break;
                
                default: break;
            }
        }
    }
    fprintf(ftool, "\t\t\t//----------------------------------------------------------------------------------\n");
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
        char outName[256] = "\0";
        strcpy(outName, fileName);
        outName[len - 3] = 'c';
        outName[len - 2] = '\0';
        
        GuiLayoutConfig config;
        memset(&config, 0, sizeof(GuiLayoutConfig));
        
        config.width = 800;
        config.height = 600;
        strcpy(config.name, "file_name");
        strcpy(config.version, "1.0-dev");
        strcpy(config.company, "raylib tech");
        strcpy(config.description, "tool description sample");
        config.defineRecs = false;
        config.exportAnchors = true;
        config.exportAnchor0 = false;
        config.fullComments = true;
        
        // Generate C code for gui layout.controls
        GenerateCode(outName, config);
    }
    else printf("Input RGL file not valid\n");
}
