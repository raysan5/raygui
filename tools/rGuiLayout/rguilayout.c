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

#include "external/easings.h"
#include "external/tinyfiledialogs.h"   // Open/Save file dialogs

#include <stdlib.h>

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define MAX_GUI_CONTROLS   256          // Maximum number of gui controls
#define MAX_ANCHOR_POINTS    8          // Maximum number of anchor points

#define GRID_LINE_SPACING    5          // Grid line spacing in pixels
#define GRID_ALPHA        0.1f          // Grid lines alpha amount

#define ANCHOR_RADIUS       20          // Default anchor radius

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
    unsigned char *text;
    AnchorPoint *ap;
} GuiControl;


//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static int screenWidth = 800;
static int screenHeight = 600;

static GuiControl layout[MAX_GUI_CONTROLS] = { 0 };
static AnchorPoint anchors[MAX_ANCHOR_POINTS];
static int controlsCounter = 0;

const char *controlTypeName[] = { "LABEL", "BUTTON", "IMAGEBUTTON", "TOGGLE", "TOGGLEGROUP", "SLIDER", "SLIDERBAR", "PROGRESSBAR", "SPINNER", "COMBOBOX", "CHECKBOX", "TEXTBOX", "GROUPBOX", "WINDOWBOX", "DUMMYREC", "DROPDOWNBOX", "STATUSBAR", "LISTVIEW", "COLORPICKER" };
const char *controlTypeNameLow[] = { "Label", "Button", "ImageButton", "Toggle", "ToggleGroup", "Slider", "SliderBar", "ProgressBar", "Spinner", "ComboBox", "CheckBox", "TextBox", "GroupBox", "WindowBox", "DummyRec", "DropdownBox", "StatusBar", "ListView", "ColorPicker" };
const char *controlTypeNameShort[] = { "lbl", "btn", "ibtn", "tggl", "tgroup", "sldr", "sldrb", "prgssb", "spnr", "combox", "chkbox", "txtbox", "grpbox", "wdwbox", "dmyrc", "ddwnbox", "stsb", "lstvw", "clrpckr" };

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static void DrawGrid2D(int divsX, int divsY);                           // Draw 2d grid with horizontal and vertical lines depending on the screen size
static void SaveLayoutRGL(const char *fileName, bool binary);           // Save gui layout project information
static void LoadLayoutRGL(const char *fileName);                        // Load gui layout project information
static void GenerateCode(const char *fileName , bool noStaticData);     // Generate C code for gui layout
static void GenerateCodeFromRGL(const char *fileName);                  // Generate C code from .rgl file

//----------------------------------------------------------------------------------
// Main Entry point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "rGuiLayout - raygui layout editor");
       
    int selectedControl = -1;
    int selectedType = BUTTON;
    int selectedTypeDraw = LABEL;

    int mouseX, mouseY;
    bool snapMode = false;
    bool showGrid = true;
    bool controlCollision = false;          // Checks if the mouse is colliding with a control or list
    bool controlDrag = false;               // Allows the control to be moved with the mouse without detecting collision every frame
    
    bool textEditMode = false;
    int framesCounter = 0;
    int saveControlSelected = -1;
    char previewText[8] = "TEXTBOX";
    bool anchorMode = false;
    bool anchorLinkMode = false;
    int selectedAnchor = -1;
    int linkedAnchor = -1;
    
    AnchorPoint auxAnchor = { 9, 0, 0, 0};
    bool anchorNewPos = false;

    bool lockMode = false;

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
        (Rectangle){ 0, 0, 120, 40},            // IMAGEBUTTON
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
    
    Texture2D texture = LoadTexture("icons.png");
    Texture2D tracemap = { 0 };
    Rectangle tracemapRec = { 0 };
    bool tracemapEditMode = false;
    float tracemapFade = 0.5f;
    Vector2 panOffset = { 0 };
    Vector2 prevPosition = { tracemapRec.x, tracemapRec.y };
    
    // Initialize anchor points to default values
    for (int i = 0; i < MAX_ANCHOR_POINTS; i++)
    {
        anchors[i].id = i;
        anchors[i].x = 0;
        anchors[i].y = 0;
        anchors[i].enabled = false;
        anchors[i].hidding = false;
    }

    anchors[0].enabled = true;      // Enable anchors parent (0, 0)
    
    // Initialize layout data
    for (int i = 0; i < MAX_GUI_CONTROLS; i++)
    {
        layout[i].id = 0;
        layout[i].type = 0;
        layout[i].rec = (Rectangle){ 0, 0, 0, 0 };
        layout[i].text = (unsigned char *)calloc(1, 32);
        strcpy(layout[i].text, "SAMPLE TEXT");
        layout[i].ap = &anchors[0];  // By default, set parent anchor
    }
    
    SetTargetFPS(120);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        mouseX = GetMouseX();
        mouseY = GetMouseY();
        
        // Enables or disables snapMode if not in textEditMode
        if (IsKeyPressed(KEY_S) && (!textEditMode)) snapMode = !snapMode;
        
        // Checks if the defaultRec[selectedType] is colliding with the list of the controls
        if (CheckCollisionPointRec(GetMousePosition(), listViewControls)) controlCollision = true;
        else if (CheckCollisionPointRec(GetMousePosition(), listViewControlsCounter)) controlCollision = true;
        
        
        // Toggle on the controlListView
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
        
        /*
        // Toggle on the controlListViewCounter
        if (IsKeyPressed(KEY_W))
        {
            startPosXListViewControlsCounter = listViewControlsCounter.x;
            deltaPosXListViewControlsCounter = GetScreenWidth() -listViewControlsCounter.width - startPosXListViewControlsCounter;
            counterListViewControlsCounter = 0;
        }
        
        if (IsKeyDown(KEY_W))
        {
            counterListViewControlsCounter++;
            if (counterListViewControlsCounter >= 60) counterListViewControlsCounter = 60;
            listViewControlsCounter.x = (int)EaseCubicInOut(counterListViewControlsCounter, startPosXListViewControlsCounter, deltaPosXListViewControlsCounter, 60);
        }  
        else if (IsKeyReleased(KEY_W))
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
        */
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (selectedControl == -1) && !controlCollision && !anchorMode && !tracemapEditMode)
        {
            // Add new control (button)
            layout[controlsCounter].id = controlsCounter;
            layout[controlsCounter].type = selectedType;
            layout[controlsCounter].rec = (Rectangle){  mouseX - defaultRec[selectedType].width/2, mouseY - defaultRec[selectedType].height/2, defaultRec[selectedType].width, defaultRec[selectedType].height };
            strcpy(layout[controlsCounter].text, "SAMPLE TEXT");
            layout[controlsCounter].ap = &anchors[0];        // Default anchor point (0, 0)
            
            for (int i = 0; i < controlsCounter; i++)
            {
                if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }) && layout[i].type == WINDOWBOX) layout[controlsCounter].ap =layout[i].ap;
            }
            if(layout[controlsCounter].type == WINDOWBOX)
            {
                for (int i = 1; i < MAX_ANCHOR_POINTS; i++)
                {
                    if (!anchors[i].enabled)
                    {
                        anchors[i].x = layout[controlsCounter].rec.x;
                        anchors[i].y = layout[controlsCounter].rec.y;
                        anchors[i].enabled = true;
                        layout[controlsCounter].ap = &anchors[i];
                        break;
                    }
                }
            }
                             
            layout[controlsCounter].rec.x -= layout[controlsCounter].ap->x;
            layout[controlsCounter].rec.y -= layout[controlsCounter].ap->y;
            controlsCounter++;
        }
        
        // Check selected control (on mouse hover)
        for (int i = 0; i < controlsCounter; i++)
        {
            if (controlDrag || lockMode || tracemapEditMode) break;
            if ((layout[i].type == WINDOWBOX) && (!layout[i].ap->hidding) && (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, 24 })))
            {
                selectedControl = i;
                break;
            }
            else if ((!layout[i].ap->hidding) && (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }) && layout[i].type != WINDOWBOX))
            {
                selectedControl = i;
                break;
            }
            else selectedControl = -1;
        }
        
        if (selectedControl != -1 && !textEditMode && !anchorMode)
        {
            // Disables the defaultRec[selectedType]
            controlCollision = true;
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                controlDrag = true;
            }
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                controlDrag = false;
                selectedControl = -1;
            } 
             
            if (controlDrag && !lockMode)
            {
                layout[selectedControl].rec.x = mouseX - layout[selectedControl].rec.width/2 - layout[selectedControl].ap->x;
                layout[selectedControl].rec.y = mouseY - layout[selectedControl].rec.height/2 - layout[selectedControl].ap->y;
                
                // Snap to grid position and size
                if (snapMode)
                {
                    // Snap rectangle position to closer snap point
                    int offsetX = layout[selectedControl].rec.x%GRID_LINE_SPACING;
                    int offsetY = layout[selectedControl].rec.y%GRID_LINE_SPACING;
                    
                    if (offsetX >= GRID_LINE_SPACING/2) layout[selectedControl].rec.x += (GRID_LINE_SPACING - offsetX);
                    else layout[selectedControl].rec.x -= offsetX;
                    
                    if (offsetY >= GRID_LINE_SPACING/2) layout[selectedControl].rec.y += (GRID_LINE_SPACING - offsetY);
                    else layout[selectedControl].rec.y -= offsetY;  
                }
            }
            
            if (snapMode)
            {
                if (IsKeyDown(KEY_LEFT_ALT))
                {
                    // Control modifier of position
                    if (IsKeyPressed(KEY_RIGHT)) layout[selectedControl].rec.x += GRID_LINE_SPACING;
                    else if (IsKeyPressed(KEY_LEFT)) layout[selectedControl].rec.x -= GRID_LINE_SPACING;
                    
                    if (IsKeyPressed(KEY_UP)) layout[selectedControl].rec.y -= GRID_LINE_SPACING;
                    else if (IsKeyPressed(KEY_DOWN)) layout[selectedControl].rec.y += GRID_LINE_SPACING;
                }
                else 
                {
                    // Control modifier of width and height
                    if (IsKeyPressed(KEY_RIGHT)) layout[selectedControl].rec.width += GRID_LINE_SPACING;
                    else if (IsKeyPressed(KEY_LEFT)) layout[selectedControl].rec.width -= GRID_LINE_SPACING;

                    if (IsKeyPressed(KEY_UP)) layout[selectedControl].rec.height -= GRID_LINE_SPACING;
                    else if (IsKeyPressed(KEY_DOWN)) layout[selectedControl].rec.height += GRID_LINE_SPACING;
                }
                
                /*
                int offsetX = layout[selectedControl].rec.width%GRID_LINE_SPACING;
                int offsetY = layout[selectedControl].rec.height%GRID_LINE_SPACING;
                
                if (offsetX >= GRID_LINE_SPACING/2) layout[selectedControl].rec.width += (GRID_LINE_SPACING - offsetX);
                else layout[selectedControl].rec.width -= offsetX;
                
                if (offsetY >= GRID_LINE_SPACING/2) layout[selectedControl].rec.height += (GRID_LINE_SPACING - offsetY);
                else layout[selectedControl].rec.height -= offsetY;
                */
            }
            
            else
            {
                if (IsKeyDown(KEY_LEFT_CONTROL))
                {
                    // Control modifier for a more precise sizing
                    if (IsKeyPressed(KEY_RIGHT)) layout[selectedControl].rec.width++;
                    else if (IsKeyPressed(KEY_LEFT)) layout[selectedControl].rec.width--;

                    if (IsKeyPressed(KEY_UP)) layout[selectedControl].rec.height--;
                    else if (IsKeyPressed(KEY_DOWN)) layout[selectedControl].rec.height++;
                }
                else if (IsKeyDown(KEY_LEFT_ALT))
                {
                    // Control modifier of position 
                    if (IsKeyPressed(KEY_RIGHT)) layout[selectedControl].rec.x ++;
                    else if (IsKeyPressed(KEY_LEFT)) layout[selectedControl].rec.x --;
                    
                    if (IsKeyPressed(KEY_UP)) layout[selectedControl].rec.y --;
                    else if (IsKeyPressed(KEY_DOWN)) layout[selectedControl].rec.y ++;
                }
                else
                {
                    if (IsKeyDown(KEY_RIGHT)) layout[selectedControl].rec.width++;
                    else if (IsKeyDown(KEY_LEFT)) layout[selectedControl].rec.width--;
                    
                    if (IsKeyDown(KEY_UP)) layout[selectedControl].rec.height--;
                    else if (IsKeyDown(KEY_DOWN)) layout[selectedControl].rec.height++;
                }
            }
            
            // Delete selected control and shift array position
            if (IsKeyPressed(KEY_DELETE))
            {
                for (int i = selectedControl; i < controlsCounter; i++)
                {
                    layout[i].id = layout[i + 1].id;
                    layout[i].type = layout[i + 1].type;
                    layout[i].rec = layout[i + 1].rec;
                    strcpy(layout[i].text, layout[i + 1].text);
                    layout[i].ap = layout[i + 1].ap;
                }

                controlsCounter--;
                selectedControl = -1;
            }
            
            // Unlinks the control selected from its current anchor
            if(IsKeyPressed(KEY_U))
            {
                layout[selectedControl].rec.x += layout[selectedControl].ap->x;
                layout[selectedControl].rec.y += layout[selectedControl].ap->y;
                layout[selectedControl].ap = &anchors[0];
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
            
            // Snap rectangle size to closer snap point sizes
            /*
            offsetX = defaultRec[selectedType].width%GRID_LINE_SPACING;
            offsetY = defaultRec[selectedType].height%GRID_LINE_SPACING;
            
            if (offsetX >= GRID_LINE_SPACING/2) defaultRec[selectedType].width += (GRID_LINE_SPACING - offsetX);
            else defaultRec[selectedType].width -= offsetX;
            
            if (offsetY >= GRID_LINE_SPACING/2) defaultRec[selectedType].height += (GRID_LINE_SPACING - offsetY);
            else defaultRec[selectedType].height -= offsetY;
            */            
        }
        
        // Resize the controller aplying the snap
        if (!textEditMode && IsKeyPressed(KEY_R) && selectedControl != -1)
        {
            int offsetX = layout[selectedControl].rec.width%GRID_LINE_SPACING;
            int offsetY = layout[selectedControl].rec.height%GRID_LINE_SPACING;
            
            if (offsetX >= GRID_LINE_SPACING/2) layout[selectedControl].rec.width += (GRID_LINE_SPACING - offsetX);
            else layout[selectedControl].rec.width -= offsetX;
            
            if (offsetY >= GRID_LINE_SPACING/2) layout[selectedControl].rec.height += (GRID_LINE_SPACING - offsetY);
            else layout[selectedControl].rec.height -= offsetY;
        }
        
        // Check if control has text to edit
        if (textEditMode)
        {
            // Locks the selectedControl for text editing
            selectedControl = saveControlSelected;
            int key = GetKeyPressed();
            int keyCount = strlen(layout[selectedControl].text); // Keeps track of text length
       
            // Replaces characters with pressed keys or '\0' in case of backspace
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (keyCount < 31))
            {
                layout[selectedControl].text[keyCount] = (unsigned char)key;
            }
            
            if (IsKeyPressed(KEY_BACKSPACE_TEXT))
            {
                layout[selectedControl].text[keyCount - 1] = '\0';
                if (keyCount < 0) keyCount = 0;
            }
            
            // Used to show the cursor('_') in textEditMode 
            if (keyCount < 32) framesCounter++;
            else if (keyCount == 32) framesCounter = 21;
        }
        
        // Turns off textEditMode
        if (textEditMode && IsKeyPressed(KEY_ENTER)) 
        {
            textEditMode = false;
            framesCounter = 0;
        }
        
        // Turns on textEditMode
        if (IsKeyPressed(KEY_T) && (selectedControl != -1) && (!anchorMode) &&
           ((layout[selectedControl].type == LABEL) || (layout[selectedControl].type == BUTTON) || (layout[selectedControl].type == TOGGLE) || (layout[selectedControl].type == IMAGEBUTTON) || (layout[selectedControl].type == GROUPBOX) || (layout[selectedControl].type == WINDOWBOX) || (layout[selectedControl].type == STATUSBAR) || (layout[selectedControl].type == DUMMYREC)))
        {   
            textEditMode = true;
            saveControlSelected = selectedControl;
        }
       
        // Selected control lock logic
        if (lockMode) selectedControl = saveControlSelected;

        if (IsKeyPressed(KEY_SPACE) && !textEditMode && (selectedControl != -1) && !lockMode && !anchorMode) 
        {
            lockMode = true;
            saveControlSelected = selectedControl;
        }
        else if (IsKeyPressed(KEY_SPACE) && (selectedControl != -1)) lockMode = false;

        // Checks if mouse is over an anchor
        for (int i = 1; i < MAX_ANCHOR_POINTS; i++)
        {
            if (anchorLinkMode || controlDrag) break;
            
            if (CheckCollisionPointCircle(GetMousePosition(), (Vector2){ anchors[i].x, anchors[i].y }, ANCHOR_RADIUS))
            {
                selectedAnchor = i;
                anchorMode = true;
                break;
            }
            else 
            {
                selectedAnchor = -1;
                if (!IsKeyDown(KEY_A)) anchorMode = false;
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
                    if (!anchors[i].enabled)
                    {
                        anchors[i].x = mouseX;
                        anchors[i].y = mouseY;
                        anchors[i].enabled = true;
                        anchorMode = false;
                        break;
                    }
                }
            }

            if (selectedAnchor > 0)
            {
                // Unlinks and deletes the selected anchor point
                if (IsKeyPressed(KEY_DELETE))
                {
                    for (int i = 0; i < controlsCounter; i++)
                    {
                        if (layout[i].ap->id == selectedAnchor) 
                        {
                            layout[i].rec.x += layout[i].ap->x;
                            layout[i].rec.y += layout[i].ap->y;
                            layout[i].ap = &anchors[0];
                        }
                    }
                    anchors[selectedAnchor].x = 0;
                    anchors[selectedAnchor].y = 0;
                    anchors[selectedAnchor].enabled = false;
                    anchors[selectedAnchor].hidding = false;
                    anchorMode = false;
                }
                
                // Allows to drag an anchor without losing collision
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && IsKeyDown(KEY_A))
                {
                    controlDrag = true;
                    anchorNewPos = true;
                    
                    for (int i = 0; i < controlsCounter; i++)
                    {
                        if (layout[i].ap->id == selectedAnchor) 
                        {
                            layout[i].rec.x += layout[i].ap->x;
                            layout[i].rec.y += layout[i].ap->y;
                            layout[i].ap = &auxAnchor;
                        }
                    } 
                }
                else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) controlDrag = true;
                
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {     
                    if (anchorNewPos)
                    {
                        for (int i = 0; i < controlsCounter; i++)
                        {
                            if (layout[i].ap->id == 9) 
                            {
                                layout[i].rec.x += layout[i].ap->x;
                                layout[i].rec.y += layout[i].ap->y;
                                layout[i].ap = &anchors[selectedAnchor];
                                layout[i].rec.x -= anchors[selectedAnchor].x;
                                layout[i].rec.y -= anchors[selectedAnchor].y;
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
                    anchors[selectedAnchor].x = mouseX;
                    anchors[selectedAnchor].y = mouseY;
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
                    if (selectedControl != -1 && !lockMode /*&& (layout[selectedControl].ap != &anchors[linkedAnchor])*/)
                    {
                        layout[selectedControl].rec.x += layout[selectedControl].ap->x;
                        layout[selectedControl].rec.y += layout[selectedControl].ap->y;
                        layout[selectedControl].ap = &anchors[linkedAnchor];
                        layout[selectedControl].rec.x -= anchors[linkedAnchor].x;
                        layout[selectedControl].rec.y -= anchors[linkedAnchor].y;
                    }
                    
                    anchorLinkMode = false;  
                    anchorMode = false;                    
                }
                
                // Hide/Unhide selected anchor linked controls
                if (IsKeyPressed(KEY_H)) anchors[selectedAnchor].hidding = !anchors[selectedAnchor].hidding;
            }
        }

        // Enable anchor mode editing
        if (IsKeyDown(KEY_A) && !textEditMode && (layout[selectedControl].type != TEXTBOX)) anchorMode = true;

        // Checks the minimum size of the rec
        if (selectedControl != -1)
        {
            // Sets the minimum limit of the width
            if (layout[selectedControl].type == LABEL || layout[selectedControl].type == BUTTON || layout[selectedControl].type == TOGGLE || layout[selectedControl].type == TEXTBOX)
            {
                if (layout[selectedControl].rec.width <  MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE])) layout[selectedControl].rec.width = MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE]);
            }
            else if (layout[selectedControl].type == IMAGEBUTTON)
            {
                if (layout[selectedControl].rec.width <  MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE]) + texture.width/3) layout[selectedControl].rec.width = MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE]) + texture.width/3 + 5;
            }
            else if (layout[selectedControl].type == WINDOWBOX || layout[selectedControl].type == GROUPBOX || layout[selectedControl].type == STATUSBAR) 
            {
                if (layout[selectedControl].rec.width <  MeasureText(layout[selectedControl].text, style[DEFAULT_TEXT_SIZE]) + 31) layout[selectedControl].rec.width = MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE]) + 31;
            }
            else if (layout[selectedControl].type == CHECKBOX)
            {
                if (layout[selectedControl].rec.width <= 10) layout[selectedControl].rec.width = 10;
            }
            else if (layout[selectedControl].rec.width <= 20) layout[selectedControl].rec.width = 20;
            
            // Sets the minimum limit of the height
            if (layout[selectedControl].type == IMAGEBUTTON) 
            { 
                if (layout[selectedControl].rec.height <= texture.height/6 + 5) layout[selectedControl].rec.height = texture.height/6 + 5;
            }
            else if (layout[selectedControl].type == WINDOWBOX) 
            { 
                if (layout[selectedControl].rec.height < 48) layout[selectedControl].rec.height = 48;
            }
            else if (layout[selectedControl].type == PROGRESSBAR || layout[selectedControl].type == SLIDER || layout[selectedControl].type == SLIDERBAR || layout[selectedControl].type == CHECKBOX)
            {
                if (layout[selectedControl].rec.height <= 10 ) layout[selectedControl].rec.height = 10;
            }
            else if (layout[selectedControl].rec.height <= 20) layout[selectedControl].rec.height = 20;
        }
  
        // Shows or hides the grid if not in textEditMode
        if (IsKeyPressed(KEY_G) && (!textEditMode)) showGrid = !showGrid;
        
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
            layout[controlsCounter].id = controlsCounter;
            layout[controlsCounter].type = layout[selectedControl].type;
            layout[controlsCounter].rec = layout[selectedControl].rec;
            layout[controlsCounter].rec.x += 5;
            layout[controlsCounter].rec.y += 5;
            strcpy(layout[controlsCounter].text, layout[selectedControl].text);
            layout[controlsCounter].ap = layout[selectedControl].ap;            // Default anchor point (0, 0)
            
            controlsCounter++;
        }
     
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
        {
            // Save file dialog
            const char *filters[] = { "*.rgl" };
            const char *fileName = tinyfd_saveFileDialog("Save raygui layout text file", "", 1, filters, "raygui Layout Files (*.rgl)");

            // Save layout file (text or binary)
            if (fileName != NULL) SaveLayoutRGL(fileName, false);
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
            
            if (fileName != NULL) GenerateCode(fileName, true);
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
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) panOffset = (Vector2){ mouseX, mouseY };
                
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                {
                    if (snapMode)
                    {
                        if (offsetX >= GRID_LINE_SPACING/2) mouseX += (GRID_LINE_SPACING - offsetX);
                        else mouseX -= offsetX;
                        
                        if (offsetY >= GRID_LINE_SPACING/2) mouseY += (GRID_LINE_SPACING - offsetY);
                        else mouseY -= offsetY;
                    }

                    tracemapRec.x = prevPosition.x + (mouseX - panOffset.x);
                    tracemapRec.y = prevPosition.y + (mouseY - panOffset.y);
                }
                
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) prevPosition = (Vector2){ tracemapRec.x, tracemapRec.y };

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
                if (IsKeyDown(KEY_LEFT)) tracemapFade-= 0.01f;
                else if (IsKeyDown(KEY_RIGHT)) tracemapFade+=0.01f;
                
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
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            // TODO: Draw global app screen limits (black rectangle with black default anchor)

            if (showGrid) DrawGrid2D(GetScreenWidth()/13, GetScreenHeight()/13);
            
            if (tracemap.id > 0) DrawTexture(tracemap, tracemapRec.x, tracemapRec.y, Fade(WHITE, tracemapFade));

            for (int i = 0; i < controlsCounter; i++)
            {
                // Draws the Controls when placed on the grid.
                if (!layout[i].ap->hidding)
                {
                    switch (layout[i].type)
                    {
                        case LABEL: GuiLabel((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, layout[i].text); break;
                        case BUTTON: GuiButton((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, layout[i].text); break;
                        case IMAGEBUTTON: GuiImageButtonEx((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, texture, (Rectangle){ 0, 0, texture.width/3, texture.height/6 }, layout[i].text); break;
                        case TOGGLE: GuiToggleButton((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, layout[i].text, false); break;
                        case TOGGLEGROUP: GuiToggleGroup((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, list, 3, 1); break;
                        case SLIDER: GuiSlider((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, 40, 0, 100); break;
                        case SLIDERBAR: GuiSliderBar((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, 40, 0, 100); break;
                        case PROGRESSBAR: GuiProgressBar((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, 40, 0, 100); break;
                        case SPINNER: GuiSpinner((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, 40, 0, 100); break;
                        case COMBOBOX: GuiComboBox((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, list, 3, 1); break;
                        case CHECKBOX: GuiCheckBox((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, false); break;
                        case TEXTBOX: GuiTextBox((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, layout[i].text, 32); break;
                        case GROUPBOX: GuiGroupBox((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, layout[i].text); break;
                        case WINDOWBOX: GuiWindowBox((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, layout[i].text); break;
                        case DUMMYREC: GuiDummyRec((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, layout[i].text); break;
                        case DROPDOWNBOX: GuiDropdownBox((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, list, 3, 2); break;
                        case STATUSBAR: GuiStatusBar((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, layout[i].text, 15); break;
                        case LISTVIEW: GuiListView((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, guiControls, 14, 1); break;
                        case COLORPICKER: GuiColorPicker((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, RED); break;
                        default: break;
                    }
                    
                    if ((layout[i].ap->id == selectedAnchor) && (layout[i].ap->id > 0)) DrawLine(layout[i].ap->x, layout[i].ap->y, layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, RED);
                }
                else if ((layout[i].ap->id == selectedAnchor) && (layout[i].ap->id > 0)) DrawLine(layout[i].ap->x, layout[i].ap->y, layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, BLUE);
            }
            
            // Draws the defaultRec[selectedType] of the control selected
            if (selectedControl == -1 && !anchorMode && !tracemapEditMode)
            {
                switch (selectedTypeDraw)
                {
                    case LABEL: GuiLabel(defaultRec[selectedTypeDraw], "TEXT SAMPLE"); break;
                    case BUTTON: GuiButton(defaultRec[selectedTypeDraw], "BUTTON"); break;
                    case IMAGEBUTTON: GuiImageButtonEx(defaultRec[selectedTypeDraw], texture , (Rectangle){ 0, 0, texture.width/3, texture.height/6 }, "IMAGE BUTTON"); break;
                    case TOGGLE: GuiToggleButton(defaultRec[selectedTypeDraw], "TOGGLE", false); break;
                    case TOGGLEGROUP: GuiToggleGroup(defaultRec[selectedTypeDraw], list, 3, 1); break;
                    case SLIDER: GuiSlider(defaultRec[selectedTypeDraw], 40, 0, 100); break;
                    case SLIDERBAR: GuiSliderBar(defaultRec[selectedTypeDraw], 40, 0, 100); break;
                    case PROGRESSBAR: GuiProgressBar(defaultRec[selectedTypeDraw], 40, 0, 100); break;
                    case SPINNER: GuiSpinner(defaultRec[selectedTypeDraw], 40, 0, 100); break;
                    case COMBOBOX: GuiComboBox(defaultRec[selectedTypeDraw], list, 3, 1); break;
                    case CHECKBOX: GuiCheckBox(defaultRec[selectedTypeDraw], false); break;
                    case TEXTBOX: GuiTextBox(defaultRec[selectedTypeDraw], previewText, 7); break;
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

            // Draw the tracemap rectangle
            if (tracemapEditMode) DrawRectangleLines(tracemapRec.x, tracemapRec.y, tracemapRec.width, tracemapRec.height, RED);

            /*
            // Draw the list of controls
            DrawRectangleRec(listViewControls, Fade(WHITE, 0.7f));
            selectedType = GuiListView(listViewControls, guiControls, 14, selectedType);
            
            // Draw the list of controlsCounter
            DrawRectangleRec(listViewControlsCounter, Fade(WHITE, 0.7f));
            GuiListView(listViewControlsCounter, guiControlsCounter, controlsCounter, selectedControl);
            */
            
            // Draw the anchorPoints
            for (int i = 1; i < MAX_ANCHOR_POINTS; i++)
            {
                if (anchors[i].id == selectedAnchor && anchorNewPos)
                {
                    // Draw the anchor that is currently moving
                    DrawCircle(anchors[i].x, anchors[i].y, ANCHOR_RADIUS, Fade(ORANGE, 0.5f));
                    DrawRectangle(anchors[i].x - ANCHOR_RADIUS - 5, anchors[i].y - 1, ANCHOR_RADIUS*2 + 10, 1, ORANGE);
                    DrawRectangle(anchors[i].x - 1, anchors[i].y - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, ORANGE);
                }
                else if (anchors[i].hidding && anchors[i].id == selectedAnchor)
                {
                    // Draw idle anchor
                    DrawCircle(anchors[i].x, anchors[i].y, ANCHOR_RADIUS, Fade(BLUE, 0.5f));
                    DrawRectangle(anchors[i].x - ANCHOR_RADIUS - 5, anchors[i].y - 1, ANCHOR_RADIUS*2 + 10, 1, BLUE);
                    DrawRectangle(anchors[i].x - 1, anchors[i].y - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, BLUE);
                }
                else if (anchors[i].id == selectedAnchor)
                {
                    // Draw the selected anchor
                    DrawCircle(anchors[i].x, anchors[i].y, ANCHOR_RADIUS, Fade(RED, 0.5f));
                    DrawRectangle(anchors[i].x - ANCHOR_RADIUS - 5, anchors[i].y - 1, ANCHOR_RADIUS*2 + 10, 1, RED);
                    DrawRectangle(anchors[i].x - 1, anchors[i].y - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, RED);
                } 
                else if (anchors[i].hidding)
                {
                    // Draw idle anchor
                    DrawCircleLines(anchors[i].x, anchors[i].y, ANCHOR_RADIUS, Fade(BLUE, 0.5f));
                    DrawRectangle(anchors[i].x - ANCHOR_RADIUS - 5, anchors[i].y - 1, ANCHOR_RADIUS*2 + 10, 1, BLUE);
                    DrawRectangle(anchors[i].x - 1, anchors[i].y - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, BLUE);
                }
                else
                {
                    // Draw idle anchor
                    DrawCircleLines(anchors[i].x, anchors[i].y, ANCHOR_RADIUS, Fade(RED, 0.5f));
                    DrawRectangle(anchors[i].x - ANCHOR_RADIUS - 5, anchors[i].y - 1, ANCHOR_RADIUS*2 + 10, 1, RED);
                    DrawRectangle(anchors[i].x - 1, anchors[i].y - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, RED);
                }
            }
            
            // Draw selected control selection rectangle (transparent RED)
            if ((selectedControl != -1) && (selectedControl < controlsCounter))
            {
                DrawRectangleRec((Rectangle){ layout[selectedControl].ap->x + layout[selectedControl].rec.x, layout[selectedControl].ap->y + layout[selectedControl].rec.y, layout[selectedControl].rec.width, layout[selectedControl].rec.height }, Fade(RED, 0.5f));
                
                // Draw anchor lines (if not hidden)
                if (layout[selectedControl].ap->id > 0 && !layout[selectedControl].ap->hidding) DrawLine(layout[selectedControl].ap->x, layout[selectedControl].ap->y, layout[selectedControl].ap->x + layout[selectedControl].rec.x, layout[selectedControl].ap->y + layout[selectedControl].rec.y, RED);
                else if (layout[selectedControl].ap->id > 0 && layout[selectedControl].ap->hidding) DrawLine(layout[selectedControl].ap->x, layout[selectedControl].ap->y, layout[selectedControl].ap->x + layout[selectedControl].rec.x, layout[selectedControl].ap->y + layout[selectedControl].rec.y, BLUE);
            }
            
            // Draw cursor (control mode or anchor mode)
            if ((selectedControl == -1)  && (selectedAnchor == -1))
            {
                if (anchorMode)
                {
                    DrawCircleLines(mouseX, mouseY, ANCHOR_RADIUS, Fade(RED, 0.5f));
                    DrawRectangle(mouseX - ANCHOR_RADIUS - 5, mouseY - 1, ANCHOR_RADIUS*2 + 10, 1, RED);
                    DrawRectangle(mouseX - 1 , mouseY - ANCHOR_RADIUS - 5, 1, ANCHOR_RADIUS*2 + 10, RED);
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
                    if (layout[selectedControl].type == LABEL) DrawText("|", layout[selectedControl].rec.x + layout[selectedControl].ap->x + MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE]) + 2, layout[selectedControl].rec.y + layout[selectedControl].ap->y - 1, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else if (layout[selectedControl].type == IMAGEBUTTON) DrawText("|", layout[selectedControl].rec.x + layout[selectedControl].ap->x + layout[selectedControl].rec.width/2 + MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE])/2 + texture.width/6, layout[selectedControl].rec.y + layout[selectedControl].ap->y + layout[selectedControl].rec.height/2 - 6, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else if (layout[selectedControl].type == GROUPBOX) DrawText("|", layout[selectedControl].rec.x + layout[selectedControl].ap->x + 15 + MeasureText(layout[selectedControl].text, style[DEFAULT_TEXT_SIZE]), layout[selectedControl].rec.y + layout[selectedControl].ap->y - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else if (layout[selectedControl].type == WINDOWBOX) DrawText("|", layout[selectedControl].rec.x + layout[selectedControl].ap->x + 10 + MeasureText(layout[selectedControl].text, style[DEFAULT_TEXT_SIZE]), layout[selectedControl].rec.y + layout[selectedControl].ap->y + style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else if (layout[selectedControl].type == STATUSBAR) DrawText("|", layout[selectedControl].rec.x + layout[selectedControl].ap->x + 15 + MeasureText(layout[selectedControl].text, style[DEFAULT_TEXT_SIZE]), layout[selectedControl].rec.y + layout[selectedControl].ap->y + style[DEFAULT_TEXT_SIZE], style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else DrawText("|", layout[selectedControl].rec.x + layout[selectedControl].ap->x + layout[selectedControl].rec.width/2 + MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE])/2 + 2, layout[selectedControl].rec.y + layout[selectedControl].ap->y + layout[selectedControl].rec.height/2 - 6, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
               }
            }

            // Draw anchor linking line
            if (anchorLinkMode) DrawLine(anchors[linkedAnchor].x, anchors[linkedAnchor].y, mouseX, mouseY, BLACK);

            // Draw the help list (by default is out of screen)
            if (helpPosX > -280)
            {
                DrawRectangleRec((Rectangle){ helpPosX + 20, 20, 260, 350 }, GetColor(style[DEFAULT_BACKGROUND_COLOR]));
                GuiGroupBox((Rectangle){ helpPosX + 20, 20, 260, 350 }, "Shortcuts");
                GuiLabel((Rectangle){ helpPosX + 30, 30, 0, 0 }, "G - Show/hide grid");
                GuiLabel((Rectangle){ helpPosX + 30, 50, 0, 0 }, "S - Toggle snap");
                GuiLabel((Rectangle){ helpPosX + 30, 70, 0, 0 }, "R - Resize to grid");
                GuiLabel((Rectangle){ helpPosX + 30, 90, 0, 0 }, "A - Anchor mode");
                GuiLabel((Rectangle){ helpPosX + 30, 110, 0, 0 }, "H - Hide controls of selected anchor");
                GuiLabel((Rectangle){ helpPosX + 30, 130, 0, 0 }, "U - Unlink anchor");
                GuiLabel((Rectangle){ helpPosX + 30, 150, 0, 0 }, "Space - Lock/unlock control");
                GuiLabel((Rectangle){ helpPosX + 30, 170, 0, 0 }, "T - Enter text mode(if possible)");
                GuiLabel((Rectangle){ helpPosX + 30, 190, 0, 0 }, "Enter - Exit text mode");
                GuiLabel((Rectangle){ helpPosX + 30, 210, 0, 0 }, "Delete - Delete a control");
                GuiLabel((Rectangle){ helpPosX + 30, 230, 0, 0 }, "Arrows - Modify width/height");
                GuiLabel((Rectangle){ helpPosX + 30, 250, 0, 0 }, "L. Ctrl + Arrows - Modify width/height(smooth)");
                GuiLabel((Rectangle){ helpPosX + 30, 270, 0, 0 }, "L. Alt + Arrows - Modify position");
                GuiLabel((Rectangle){ helpPosX + 30, 290, 0, 0 }, "L. Ctrl + Enter - Export layout to code");
                GuiLabel((Rectangle){ helpPosX + 30, 310, 0, 0 }, "L. Ctrl + S - Save layout(.rgl)");
                GuiLabel((Rectangle){ helpPosX + 30, 330, 0, 0 }, "L. Ctrl + O - Open layout(.rgl)");
                GuiLabel((Rectangle){ helpPosX + 30, 350, 0, 0 }, "L. Ctrl + D - Duplicate selected control");
            }

            // Draw status bar bottom with debug information
            GuiStatusBar((Rectangle){ 0, GetScreenHeight() - 24, 125, 24}, FormatText("Controls count: %i", controlsCounter), 20);
            GuiStatusBar((Rectangle){ 124, GetScreenHeight() - 24, 126, 24}, FormatText("Mouse: (%i, %i)", mouseX, mouseY), 15);
            if (snapMode) GuiStatusBar((Rectangle){ 249, GetScreenHeight() - 24, 81, 24}, "SNAP ON", 10);
            else GuiStatusBar((Rectangle){ 249, GetScreenHeight() - 24, 81, 24}, "SNAP OFF", 10);
            GuiStatusBar((Rectangle){ 329, GetScreenHeight() - 24, 80, 24}, "Tab - Help", 10);
            if (selectedControl != -1) GuiStatusBar((Rectangle){ 400, GetScreenHeight() - 24, GetScreenWidth() - 400, 24}, FormatText("Selected Control: #%03i  |  %s  |  rec(%i, %i, %i, %i)", selectedControl, controlTypeName[selectedType], layout[selectedControl].rec.x, layout[selectedControl].rec.y, layout[selectedControl].rec.width, layout[selectedControl].rec.height), 15);
            else GuiStatusBar((Rectangle){ 400, GetScreenHeight() - 24, GetScreenWidth() - 400, 24}, "", 15);
            
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);
    UnloadTexture(tracemap);
    
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
    int offset = 0;

    // Draw vertical grid lines
    for (int i = 0; i < divsX; i++)
    {
        for (int k = 0; k < 5; k++)
        {
            DrawRectangle(-(divsX/2*GRID_LINE_SPACING*5) + offset - 1, -1, 1, GetScreenHeight(), ((k == 0) ? Fade(BLACK, GRID_ALPHA*2) : Fade(GRAY, GRID_ALPHA)));
            offset += GRID_LINE_SPACING;
        }
    }

    offset = 0;
    
    // Draw horizontal grid lines
    for (int i = 0; i < divsY; i++)
    {
        for (int k = 0; k < 5; k++)
        {
            DrawRectangle(-1, -(divsY/2*GRID_LINE_SPACING*5) + offset - 1, GetScreenWidth(), 1, ((k == 0) ? Fade(BLACK, GRID_ALPHA*2) : Fade(GRAY, GRID_ALPHA)));
            offset += GRID_LINE_SPACING;
        }
    }
}

// Save gui layout project information
// NOTE: Exported as text file
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
            // NUM_CONTROLS     - 2 bytes
            // reserved         - 4 bytes
            
            char signature[5] = "RGL ";
            short version = RGL_FILE_VERSION_BINARY;
            short numControls = controlsCounter;
            int reserved = 0;

            fwrite(signature, 1, 4, rglFile);
            fwrite(&version, 1, sizeof(short), rglFile);
            fwrite(&numControls, 1, sizeof(short), rglFile);
            fwrite(&reserved, 1, sizeof(int), rglFile);
            
            for (int i = 0; i < MAX_ANCHOR_POINTS; i++)
            {
                fwrite(&anchors[i].id, 1, sizeof(int), rglFile);
                fwrite(&anchors[i].x, 1, sizeof(int), rglFile);
                fwrite(&anchors[i].y, 1, sizeof(int), rglFile);
                fwrite(&anchors[i].enabled, 1, sizeof(bool), rglFile);
            }

            for (int i = 0; i < controlsCounter; i++) 
            {
                // Export data in independent way
                fwrite(&layout[i].id, 1, sizeof(int), rglFile);
                fwrite(&layout[i].type, 1, sizeof(int), rglFile);
                fwrite(&layout[i].rec, 1, sizeof(Rectangle), rglFile);
                fwrite(layout[i].text, 1, 32, rglFile);
                fwrite(&layout[i].ap->id, 1, sizeof(int), rglFile);
            }

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
            fprintf(rglFile, "# Total number of controls:     %i\n", controlsCounter);
            fprintf(rglFile, "# Anchor info:   a <id> <posx> <posy> <enabled>\n");
            fprintf(rglFile, "# Control info:  c <id> <type> <rectangle> <anchor_id> <text>\n#\n");

            for (int i = 0; i < MAX_ANCHOR_POINTS; i++)
            {
                fprintf(rglFile, "a %03i %i %i %i\n", anchors[i].id, anchors[i].x, anchors[i].y, anchors[i].enabled);
            }
            
            for (int i = 0; i < controlsCounter; i++)
            {
                fprintf(rglFile, "c %03i %i %i %i %i %i %i %s\n", layout[i].id, layout[i].type, layout[i].rec.x, layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].ap->id, layout[i].text);
            }

            fclose(rglFile);
        }
    }
}

// Import gui layout project information
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
        fgets(buffer, 256, rglFile);

        if (buffer[0] != 'R')   // Text file!
        {
            controlsCounter = 0;
            
            while (!feof(rglFile))
            {
                if ((buffer[0] != '\n') && (buffer[0] != '#') && (buffer[0] == 'a'))
                {
                    sscanf(buffer, "a %03i %i %i %i", &anchors[anchorCounter].id, &anchors[anchorCounter].x, &anchors[anchorCounter].y, &anchors[anchorCounter].enabled);
                    printf("a %03i %i %i %i\n", anchors[anchorCounter].id, anchors[anchorCounter].x, anchors[anchorCounter].y, anchors[anchorCounter].enabled);
                    anchorCounter++;
                }
                else if ((buffer[0] != '\n') && (buffer[0] != '#') && (buffer[0] == 'c'))
                {
                    sscanf(buffer, "c %d %i %i %i %i %i %d %[^\n]s", &layout[controlsCounter].id, &layout[controlsCounter].type, &layout[controlsCounter].rec.x, &layout[controlsCounter].rec.y, &layout[controlsCounter].rec.width, &layout[controlsCounter].rec.height, &anchorId, layout[controlsCounter].text);
                    printf("c %d %i %i %i %i %i %i %s\n", layout[controlsCounter].id, layout[controlsCounter].type, layout[controlsCounter].rec.x, layout[controlsCounter].rec.y, layout[controlsCounter].rec.width, layout[controlsCounter].rec.height, anchorId, layout[controlsCounter].text);
                    
                    layout[controlsCounter].ap = &anchors[anchorId];
                    controlsCounter++;
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
            int reserved = 0;
            int counter = 0;

            fread(signature, 1, 4, rglFile);
            fread(&version, 1, sizeof(short), rglFile);
            fread(&controlsCounter, 1, sizeof(short), rglFile);
            fread(&reserved, 1, sizeof(int), rglFile);
            
            printf("[READ] Controls counter: %i\n", controlsCounter);
           
            if ((signature[0] == 'R') &&
                (signature[1] == 'G') &&
                (signature[2] == 'L') &&
                (signature[3] == ' '))
            {
                
                for (int i = 0; i < MAX_ANCHOR_POINTS; i++)
                {
                    fread(&anchors[i].id, 1, sizeof(int), rglFile);
                    fread(&anchors[i].x, 1, sizeof(int), rglFile);
                    fread(&anchors[i].y, 1, sizeof(int), rglFile);
                    fread(&anchors[i].enabled, 1, sizeof(bool), rglFile);
                }

                for (int i = 0; i < controlsCounter; i++)
                {
                    int anchorId = 0;
                    
                    // Import data in independent way
                    fread(&layout[i].id, 1, sizeof(int), rglFile);
                    fread(&layout[i].type, 1, sizeof(int), rglFile);
                    fread(&layout[i].rec, 1, sizeof(Rectangle), rglFile);
                    fread(layout[i].text, 1, 32, rglFile);
                    fread(&anchorId, 1, sizeof(int), rglFile);
                    layout[i].ap = &anchors[anchorId];

                    printf("[READ] Control info> id: %i, type: %i, rec: %i,%i,%i,%i, text: %s, anchorId: %i\n", layout[i].id, layout[i].type, layout[i].rec.x, layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].text, anchorId);
                }
            }
            else TraceLog(LOG_WARNING, "[raygui] Invalid layout file");
            
            fclose(rglFile);
        }
    }
    
    printf("[READ] Layout data loaded successfully!\n");
}

// Generate C code for gui layout
static void GenerateCode(const char *fileName , bool noStaticData)
{
    #define RGL_TOOL_NAME           "rGuiLayout"
    #define RGL_TOOL_DESCRIPTION    "tool_name"
    #define RGL_TOOL_AUTHOR         "tool_name"
    #define RGL_TOOL_YEAR           "2018"
    
    FILE *ftool = fopen(fileName, "wt");
    
    fprintf(ftool, "/*******************************************************************************************\n");
    fprintf(ftool, "*\n");
    fprintf(ftool, "*   %s - %s\n", RGL_TOOL_NAME, RGL_TOOL_DESCRIPTION);
    fprintf(ftool, "*\n");
    fprintf(ftool, "*   LICENSE: zlib/libpng\n");
    fprintf(ftool, "*\n");
    fprintf(ftool, "*   Copyright (c) %s %s\n", RGL_TOOL_YEAR, RGL_TOOL_AUTHOR);
    fprintf(ftool, "*\n");
    fprintf(ftool, "**********************************************************************************************/\n\n");
    fprintf(ftool, "#include \"raylib.h\"\n\n");
    fprintf(ftool, "#define RAYGUI_IMPLEMENTATION\n");
    fprintf(ftool, "#include \"raygui.h\"\n\n");
    fprintf(ftool, "//----------------------------------------------------------------------------------\n");
    fprintf(ftool, "// Controls Functions Declaration\n");
    fprintf(ftool, "//----------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < controlsCounter; i++) 
    {
        if (layout[i].type == BUTTON) fprintf(ftool, "static void Button%03i();\n", i);
        if (layout[i].type == IMAGEBUTTON) fprintf(ftool, "static void ImageButton%03i();\n", i);
    }

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
    
    fprintf(ftool, "    // Needed variables\n");
    fprintf(ftool, "    // Anchor points\n");
    fprintf(ftool, "    Vector2 %s%02i = { 0, 0 };\n", "anchor", 0);

    for(int i = 1; i < MAX_ANCHOR_POINTS; i++)
    {
        if (anchors[i].x != 0 && anchors[i].y != 0)
        {
            for (int j = 0; j < controlsCounter; j++)
            {
                if(layout[j].ap->id == anchors[i].id)
                {
                    fprintf(ftool, "    Vector2 %s%02i = { %i, %i };\n", "anchor", i, anchors[i].x, anchors[i].y);
                    break;
                }
            }
            
        }
    }
    // Define texture for IMAGEBUTTON
    fprintf(ftool, "    Texture2D texture = LoadTexture(\"icons.png\");\n\n");
    // Define controls variables
    for (int i = 0; i < controlsCounter; i++)
    {
        switch (layout[i].type)
        {
            // Bools
            case TOGGLE: 
            case CHECKBOX: 
            {
                fprintf(ftool, "    // %s%03i variables\n", controlTypeName[layout[i].type], i);
                fprintf(ftool, "    bool %s%03i = false;\n", controlTypeNameShort[layout[i].type], i);
            }
            break;
            
            case DROPDOWNBOX:
            case COMBOBOX:
            case LISTVIEW:
            case TOGGLEGROUP:
            {
                fprintf(ftool, "    // %s%03i variables\n", controlTypeName[layout[i].type], i);
                fprintf(ftool, "    int %sCount%03i = 3;\n", controlTypeNameShort[layout[i].type], i);
                fprintf(ftool, "    int %sActive%03i = 0;\n", controlTypeNameShort[layout[i].type], i);
                fprintf(ftool, "    const char *%sList%03i[3] = { \"ONE\", \"TWO\", \"THREE\" };\n", controlTypeNameShort[layout[i].type], i);
            }
            break;
            
            // Floats
            case SLIDER: 
            case SLIDERBAR: 
            case PROGRESSBAR:
            {
                fprintf(ftool, "    // %s%03i variables\n", controlTypeName[layout[i].type], i);
                fprintf(ftool, "    float %sValue%03i = 50.0f;\n", controlTypeNameShort[layout[i].type], i);
                fprintf(ftool, "    float %sMinValue%03i = 0.0f;\n", controlTypeNameShort[layout[i].type], i);
                fprintf(ftool, "    float %sMaxValue%03i = 100.0f;\n", controlTypeNameShort[layout[i].type], i);
            }            
            break;
            
            // Ints
            case SPINNER:
            {
                fprintf(ftool, "    // %s%03i variables\n", controlTypeName[layout[i].type], i);
                fprintf(ftool, "    int %sValue%03i = 0;\n", controlTypeNameShort[layout[i].type], i);
                fprintf(ftool, "    int %sMinValue%03i = 0;\n", controlTypeNameShort[layout[i].type], i);
                fprintf(ftool, "    int %sMaxValue%03i = 100;\n", controlTypeNameShort[layout[i].type], i);
            }     
            break;
            
            // Colors
            case COLORPICKER:
            {
                fprintf(ftool, "    // %s%03i variables\n", controlTypeName[layout[i].type], i);
                fprintf(ftool, "    Color %sColor%03i;\n", controlTypeNameShort[layout[i].type], i);
            }
            break;

            case TEXTBOX:
            {
                fprintf(ftool, "    // %s%03i variables\n", controlTypeName[layout[i].type], i);
                fprintf(ftool, "    char %s%03i[32] = \"%s\";\n", controlTypeNameShort[layout[i].type], i, layout[i].text);        
                fprintf(ftool, "    int %sSize%03i = 32;\n", controlTypeNameShort[layout[i].type], i);        
            }
            default: break;
        }
    }
    
    fprintf(ftool, "\n");
    
    if (!noStaticData)
    {
        // Define controls rectangles
        fprintf(ftool, "    // Define controls rectangles\n");
        fprintf(ftool, "    Rectangle layoutRecs[%i] = {\n", controlsCounter);
        
        for (int i = 0; i < controlsCounter; i++)
        {
            fprintf(ftool, "        (Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }", "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height);
            
            if (i == controlsCounter - 1) fprintf(ftool, "\t\t// %s %03i\n    };\n\n", controlTypeName[layout[i].type], i);
            else fprintf(ftool, ",\t\t// %s %03i\n", controlTypeName[layout[i].type], i);
        }
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
    fprintf(ftool, "            ClearBackground(GuiBackgroundColor());\n\n");

    fprintf(ftool, "\t\t\t// Draw all controls\n");
    
    // Draw all controls
    if (noStaticData)
    {
        for (int i = 0; i < controlsCounter; i++)
        {
            switch (layout[i].type)
            {
                case LABEL: fprintf(ftool, "\t\t\tGuiLabel((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\");\n\n", "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].text); break;
                case BUTTON: fprintf(ftool, "\t\t\tif (GuiButton((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\")) Button%03i(); \n\n", "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].text, i); break;
                case IMAGEBUTTON: fprintf(ftool, "\t\t\tif (GuiImageButtonEx((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, texture, (Rectangle){ 0, 0, texture.width/3, texture.height/6 }, \"%s\")) ImageButton%03i(); \n\n", "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].text, i); break;
                case TOGGLE: fprintf(ftool, "\t\t\t%s%03i = GuiToggleButton((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\", %s%03i); \n\n", controlTypeNameShort[layout[i].type], i, "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].text, controlTypeNameShort[layout[i].type], i); break;
                case TOGGLEGROUP: fprintf(ftool, "\t\t\t%sActive%03i = GuiToggleGroup((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sList%03i, %sCount%03i, %sActive%03i); \n\n", controlTypeNameShort[layout[i].type], i, "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case SLIDER: fprintf(ftool, "\t\t\t%sValue%03i = GuiSlider((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sValue%03i, %sMinValue%03i, %sMaxValue%03i);\n\n", controlTypeNameShort[layout[i].type], i, "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case SLIDERBAR: fprintf(ftool, "\t\t\t%sValue%03i = GuiSliderBar((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sValue%03i, %sMinValue%03i, %sMaxValue%03i);\n\n", controlTypeNameShort[layout[i].type], i, "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case PROGRESSBAR: fprintf(ftool, "\t\t\t%sValue%03i = GuiProgressBar((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sValue%03i, %sMinValue%03i, %sMaxValue%03i);\n\n", controlTypeNameShort[layout[i].type], i, "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case SPINNER: fprintf(ftool, "\t\t\t%sValue%03i = GuiSpinner((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sValue%03i, %sMinValue%03i, %sMaxValue%03i);\n\n", controlTypeNameShort[layout[i].type], i, "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case COMBOBOX: fprintf(ftool, "\t\t\t%sActive%03i = GuiComboBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i },  %sList%03i, %sCount%03i, %sActive%03i); \n\n", controlTypeNameShort[layout[i].type], i, "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case CHECKBOX: fprintf(ftool, "\t\t\t%s%03i = GuiCheckBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %s%03i); \n\n", controlTypeNameShort[layout[i].type], i, "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, controlTypeNameShort[layout[i].type], i); break;
                case LISTVIEW: fprintf(ftool, "\t\t\t%sActive%03i = GuiListView((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sList%03i, %sCount%03i, %sActive%03i); \n\n", controlTypeNameShort[layout[i].type], i, "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case TEXTBOX: fprintf(ftool, "\t\t\tGuiTextBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %s%03i, %sSize%03i);\n\n", "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case GROUPBOX: fprintf(ftool, "\t\t\tGuiGroupBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\");\n\n", "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].text); break;
                case WINDOWBOX: fprintf(ftool, "\t\t\tGuiWindowBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\");\n\n", "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].text); break;
                case DUMMYREC: fprintf(ftool, "\t\t\tGuiDummyRec((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\");\n\n", "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].text); break;
                case DROPDOWNBOX: fprintf(ftool, "\t\t\t%sActive%03i = GuiDropdownBox((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sList%03i, %sCount%03i, %sActive%03i); \n\n", controlTypeNameShort[layout[i].type], i, "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case STATUSBAR: fprintf(ftool, "\t\t\tGuiStatusBar((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, \"%s\", 10);\n\n", "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].text); break;
                case COLORPICKER: fprintf(ftool, "\t\t\t%sColor%03i = GuiColorPicker((Rectangle){ %s%02i%s + %i, %s%02i%s + %i, %i, %i }, %sColor%03i);\n\n", controlTypeNameShort[layout[i].type], i, "anchor", layout[i].ap->id, ".x", layout[i].rec.x, "anchor", layout[i].ap->id, ".y", layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, controlTypeNameShort[layout[i].type], i); break;
                
                default: break;
            }
        }
    }
    else 
    {
        for (int i = 0; i < controlsCounter; i++)
        {
            switch (layout[i].type)
            {
                case LABEL: fprintf(ftool, "\t\t\tGuiLabel(layoutRecs[%i], \"%s\");\n\n", i, layout[i].text); break;
                case BUTTON: fprintf(ftool, "\t\t\tif (GuiButton(layoutRecs[%i], \"%s\")) Button%03i(); \n\n", i, layout[i].text, i); break;
                case IMAGEBUTTON: fprintf(ftool, "\t\t\tif (GuiImageButtonEx(layoutRecs[%i], texture, (Rectangle){ 0, 0, texture.width/3, texture.height/6 }, \"%s\")) ImageButton%03i(); \n\n", i, layout[i].text, i); break;
                case TOGGLE: fprintf(ftool, "\t\t\t%s%03i = GuiToggleButton(layoutRecs[%i], \"%s\", %s%03i); \n\n", controlTypeNameShort[layout[i].type], i, i, layout[i].text, controlTypeNameShort[layout[i].type], i); break;
                case TOGGLEGROUP: fprintf(ftool, "\t\t\t%sActive%03i = GuiToggleGroup(layoutRecs[%i], %sList%03i, %sCount%03i, %sActive%03i); \n\n", controlTypeNameShort[layout[i].type], i, i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case SLIDER: fprintf(ftool, "\t\t\t%sValue%03i = GuiSlider(layoutRecs[%i], %sValue%03i, %sMinValue%03i, %sMaxValue%03i);\n\n", controlTypeNameShort[layout[i].type], i, i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case SLIDERBAR: fprintf(ftool, "\t\t\t%sValue%03i = GuiSliderBar(layoutRecs[%i], %sValue%03i, %sMinValue%03i, %sMaxValue%03i);\n\n", controlTypeNameShort[layout[i].type], i, i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case PROGRESSBAR: fprintf(ftool, "\t\t\t%sValue%03i = GuiProgressBar(layoutRecs[%i], %sValue%03i, %sMinValue%03i, %sMaxValue%03i);\n\n", controlTypeNameShort[layout[i].type], i, i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case SPINNER: fprintf(ftool, "\t\t\t%sValue%03i = GuiSpinner(layoutRecs[%i], %sValue%03i, %sMinValue%03i, %sMaxValue%03i);\n\n", controlTypeNameShort[layout[i].type], i, i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case COMBOBOX: fprintf(ftool, "\t\t\t%sActive%03i = GuiComboBox(layoutRecs[%i],  %sList%03i, %sCount%03i, %sActive%03i); \n\n", controlTypeNameShort[layout[i].type], i, i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case CHECKBOX: fprintf(ftool, "\t\t\t%s%03i = GuiCheckBox(layoutRecs[%i], %s%03i); \n\n", controlTypeNameShort[layout[i].type], i, i, controlTypeNameShort[layout[i].type], i); break;
                case LISTVIEW: fprintf(ftool, "\t\t\t%sActive%03i = GuiListView(layoutRecs[%i], %sList%03i, %sCount%03i, %sActive%03i); \n\n", controlTypeNameShort[layout[i].type], i, i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case TEXTBOX: fprintf(ftool, "\t\t\tGuiTextBox(layoutRecs[%i], %s%03i, %sSize%03i);\n\n", i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case GROUPBOX: fprintf(ftool, "\t\t\tGuiGroupBox(layoutRecs[%i], \"%s\");\n\n", i, layout[i].text); break;
                case WINDOWBOX: fprintf(ftool, "\t\t\tGuiWindowBox(layoutRecs[%i], \"%s\");\n\n", i, layout[i].text); break;
                case DUMMYREC: fprintf(ftool, "\t\t\tGuiDummyRec(layoutRecs[%i], \"%s\");\n\n", i, layout[i].text); break;
                case DROPDOWNBOX: fprintf(ftool, "\t\t\t%sActive%03i = GuiDropdownBox(layoutRecs[%i], %sList%03i, %sCount%03i, %sActive%03i); \n\n", controlTypeNameShort[layout[i].type], i, i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i, controlTypeNameShort[layout[i].type], i); break;
                case STATUSBAR: fprintf(ftool, "\t\t\tGuiStatusBar(layoutRecs[%i], \"%s\", 10);\n\n", i, layout[i].text); break;
                case COLORPICKER: fprintf(ftool, "\t\t\t%sColor%03i = GuiColorPicker(layoutRecs[%i], %sColor%03i);\n\n", controlTypeNameShort[layout[i].type], i, i, controlTypeNameShort[layout[i].type], i); break;
                
                default: break;
            }
        }
    }
    
    

    fprintf(ftool, "        EndDrawing();\n");
    fprintf(ftool, "        //----------------------------------------------------------------------------------\n");
    fprintf(ftool, "    }\n\n");
    fprintf(ftool, "    // De-Initialization\n");
    fprintf(ftool, "    //--------------------------------------------------------------------------------------\n");
    fprintf(ftool, "    UnloadTexture(texture);\n");
    fprintf(ftool, "    CloseWindow();        // Close window and OpenGL context\n");
    fprintf(ftool, "    //--------------------------------------------------------------------------------------\n\n");
    fprintf(ftool, "    return 0;\n");
    fprintf(ftool, "}\n\n");

    fprintf(ftool, "//------------------------------------------------------------------------------------\n");
    fprintf(ftool, "// Controls Functions Definitions (local)\n");
    fprintf(ftool, "//------------------------------------------------------------------------------------\n");
    
    for (int i = 0; i < controlsCounter; i++)
    {
        if (layout[i].type == BUTTON)
            fprintf(ftool, "static void Button%03i()\n{\n    // TODO: Implement control logic\n}\n\n", i);
        
        if (layout[i].type == IMAGEBUTTON)
            fprintf(ftool, "static void ImageButton%03i()\n{\n    // TODO: Implement control logic\n}\n\n", i);
    }

    fclose(ftool);
}

// Generate C code from .rgl file
static void GenerateCodeFromRGL(const char *fileName)
{
    if (IsFileExtension(fileName, ".rgl"))
    {
        LoadLayoutRGL(fileName);    // Updates global: layout
        
        int len = strlen(fileName);
        char outName[256] = "\0";
        strcpy(outName, fileName);
        outName[len - 3] = 'c';
        outName[len - 2] = '\0';
        
        // Generate C code for gui layout
        GenerateCode(outName, true);
    }
    else printf("Input RGL file not valid\n");
}
