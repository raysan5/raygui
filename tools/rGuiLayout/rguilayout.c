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
#include "external/tinyfiledialogs.h"   // Open/Save file dialogs

#include <stdlib.h>

#if defined(_WIN32)
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define MAX_GUI_CONTROLS    64          // Maximum number of gui controls
#define MAX_ANCHOR_POINTS    8          // Maximum number of anchor points

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

// Anchor point type
typedef struct {
    int id;
    int x;
    int y;
    float radius;
    bool enabled;
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
static char currentPath[256];       // Path to current working folder
static int screenWidth = 800;
static int screenHeight = 600;

static GuiControl layout[MAX_GUI_CONTROLS] = { 0 };
static AnchorPoint anchors[MAX_ANCHOR_POINTS];
static int controlsCounter = 0;
static int anchorCounter = 1;       // NOTE: anchorCounter starts at 1 because the global anchor is 0

const char *controlTypeName[] = { "LABEL", "BUTTON", "IMAGEBUTTON", "TOGGLE", "TOGGLEGROUP", "SLIDER", "SLIDERBAR", "PROGRESSBAR", "SPINNER", "COMBOBOX", "CHECKBOX", "TEXTBOX", "LISTVIEW", "COLORPICKER" };
const char *controlTypeNameLow[] = { "Label", "Button", "ImageButton", "Toggle", "ToggleGroup", "Slider", "SliderBar", "ProgressBar", "Spinner", "ComboBox", "CheckBox", "TextBox", "ListView", "ColorPicker" };
const char *controlTypeNameShort[] = { "lbl", "btn", "ibtn", "tggl", "tgroup", "sldr", "sldrb", "prgssb", "spnr", "combox", "chkbox", "txtbox", "lstvw", "clrpckr" };

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static void DrawGrid2D(int divsX, int divsY);                   // Draw 2d grid with horizontal and vertical lines depending on the screen size
static void SaveLayoutRGL(const char *fileName, bool binary);   // Save gui layout project information
static void LoadLayoutRGL(const char *fileName);                // Load gui layout project information
static void GenerateLayoutCode(const char *fileName);           // Generate C code for gui layout

//----------------------------------------------------------------------------------
// Main Entry point
//----------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "rGuiLayout - raygui layout editor");
    
    // Initialize layout data
    for (int i = 0; i < MAX_GUI_CONTROLS; i++)
    {
        layout[i].id = 0;
        layout[i].type = 0;
        layout[i].rec = (Rectangle){ 0, 0, 0, 0 };
        layout[i].text = (unsigned char *)malloc(32);// strcpy(layout[i].text, "SAMPLE TEXT\0");
        layout[i].ap = NULL;
    }
    
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

    bool lockMode = false;
    
    // Used to draw the preview of selectedControl
    Rectangle defaultRec[14] = {
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
        (Rectangle){ 0, 0, 120, 250},           // LISTVIEW
        (Rectangle){ 0, 0, 120, 120}            // COLORPICKER
    };
    
    // List view required variables
    Rectangle listViewControls = { -200, 0, 140, 500 };
    int counterListViewControls = 0;
    int startPosXListViewControls = -200;
    int deltaPosXListViewControls = 0;
    Rectangle listViewControlsCounter = { GetScreenWidth() + 140, 0, 140, 500 };
    int counterListViewControlsCounter = 0;
    int startPosXListViewControlsCounter = GetScreenWidth() + 140;
    int deltaPosXListViewControlsCounter = 0;

    const char *list[3] = { "ONE", "TWO", "THREE" };
 
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
    
    // Get current directory
    // NOTE: Current working directory could not match current executable directory
    GetCurrentDir(currentPath, sizeof(currentPath));
    currentPath[strlen(currentPath)] = '\\';
    currentPath[strlen(currentPath) + 1] = '\0';      // Not really required
    
    // Initialize anchor points to default values
    for (int i = 0; i < MAX_ANCHOR_POINTS; i++)
    {
        anchors[i].id = i;
        anchors[i].x = 0;
        anchors[i].y = 0;
        anchors[i].radius = 20;
        anchors[i].enabled = false;
    }

    anchors[0].enabled = true;       // Enable anchors parent (0, 0)
    
    //GuiSetStyleProperty(TOGGLEGROUP_PADDING, 5);
    
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
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && (selectedControl == -1) && !controlCollision && !anchorMode)
        {
            // Add new control (button)
            layout[controlsCounter].id = controlsCounter;
            layout[controlsCounter].type = selectedType;
            layout[controlsCounter].rec = (Rectangle){  mouseX - defaultRec[selectedType].width/2, mouseY - defaultRec[selectedType].height/2, defaultRec[selectedType].width, defaultRec[selectedType].height };
            strcpy(layout[controlsCounter].text, "SAMPLE TEXT\0");
            layout[controlsCounter].ap = &anchors[0];        // Default anchor point (0, 0)
            
            controlsCounter++;
        }
        
        // Check selected control (on mouse hover)
        for (int i = 0; i < controlsCounter; i++)
        {
            if (controlDrag || lockMode) break;
            if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height })) 
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
                    
                    // Snap rectangle size to closer snap point sizes
                    offsetX = layout[selectedControl].rec.width%GRID_LINE_SPACING;
                    offsetY = layout[selectedControl].rec.height%GRID_LINE_SPACING;
                    
                    if (offsetX >= GRID_LINE_SPACING/2) layout[selectedControl].rec.width += (GRID_LINE_SPACING - offsetX);
                    else layout[selectedControl].rec.width -= offsetX;
                    
                    if (offsetY >= GRID_LINE_SPACING/2) layout[selectedControl].rec.height += (GRID_LINE_SPACING - offsetY);
                    else layout[selectedControl].rec.height -= offsetY;
                }
            }
            
            if (snapMode)
            {
                if (IsKeyPressed(KEY_RIGHT)) layout[selectedControl].rec.width += GRID_LINE_SPACING;
                else if (IsKeyPressed(KEY_LEFT)) layout[selectedControl].rec.width -= GRID_LINE_SPACING;

                if (IsKeyPressed(KEY_UP)) layout[selectedControl].rec.height -= GRID_LINE_SPACING;
                else if (IsKeyPressed(KEY_DOWN)) layout[selectedControl].rec.height += GRID_LINE_SPACING;
                
                int offsetX = layout[selectedControl].rec.width%GRID_LINE_SPACING;
                int offsetY = layout[selectedControl].rec.height%GRID_LINE_SPACING;
                
                if (offsetX >= GRID_LINE_SPACING/2) layout[selectedControl].rec.width += (GRID_LINE_SPACING - offsetX);
                else layout[selectedControl].rec.width -= offsetX;
                
                if (offsetY >= GRID_LINE_SPACING/2) layout[selectedControl].rec.height += (GRID_LINE_SPACING - offsetY);
                else layout[selectedControl].rec.height -= offsetY;
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
                for (int i = selectedControl; i < controlsCounter; i++) layout[i] = layout[i + 1];

                controlsCounter--;
                selectedControl = -1;
            }
            
            // Unlinks the control selected from its current anchor
            if(IsKeyPressed(KEY_R))
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
            offsetX = defaultRec[selectedType].width%GRID_LINE_SPACING;
            offsetY = defaultRec[selectedType].height%GRID_LINE_SPACING;
            
            if (offsetX >= GRID_LINE_SPACING/2) defaultRec[selectedType].width += (GRID_LINE_SPACING - offsetX);
            else defaultRec[selectedType].width -= offsetX;
            
            if (offsetY >= GRID_LINE_SPACING/2) defaultRec[selectedType].height += (GRID_LINE_SPACING - offsetY);
            else defaultRec[selectedType].height -= offsetY;
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
            if ((key >= 32) && (key <= 125) && (keyCount < 32))
            {
                layout[selectedControl].text[keyCount] = (char)key;
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
           ((layout[selectedControl].type == LABEL) || (layout[selectedControl].type == BUTTON) || (layout[selectedControl].type == TOGGLE) || (layout[selectedControl].type == IMAGEBUTTON)))
        {   
            textEditMode = true;
            saveControlSelected = selectedControl;
        }
        
        // Selected control lock logic
        if (lockMode)
        {
            selectedControl = saveControlSelected;
        }
        
        if (IsKeyPressed(KEY_SPACE) && !textEditMode && (selectedControl != -1) && !lockMode && !anchorMode) 
        {
            lockMode = true;
            saveControlSelected = selectedControl;
        }
        else if (IsKeyPressed(KEY_SPACE) && (selectedControl != -1))
        {
            lockMode = false;
        }

        // Checks if mouse is over an anchor
        for (int i = 1; i < MAX_ANCHOR_POINTS; i++)
        {
            if (anchorLinkMode || controlDrag) break;
            
            if (CheckCollisionPointCircle(GetMousePosition(), (Vector2){ anchors[i].x, anchors[i].y }, anchors[i].radius))
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
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !controlCollision && selectedAnchor == -1 && selectedControl == -1)
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
                    anchorMode = false;
                }
                
                // Allows to drag an anchor without losing collision
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) controlDrag = true;
                
                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                {                   
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
            }
        }
        
        
        // Enable anchor mode editing
        if (IsKeyDown(KEY_A) && !textEditMode && (layout[selectedControl].type != TEXTBOX)) anchorMode = true;

        // Checks the minimum size of the rec
        if (selectedControl != -1)
        {
            // Sets the minimum limit of the width
            if (layout[selectedControl].type == LABEL || layout[selectedControl].type == BUTTON || layout[selectedControl].type == TOGGLE)
            {
                if (layout[selectedControl].rec.width <  MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE])) layout[selectedControl].rec.width = MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE]);
            }
            if (layout[selectedControl].type == IMAGEBUTTON)
            {
                if (layout[selectedControl].rec.width <  MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE]) + texture.width/3) layout[selectedControl].rec.width = MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE]) + texture.width/3 + 5;
            }
            else if (layout[selectedControl].rec.width <= 20) layout[selectedControl].rec.width = 20;

            // Sets the minimum limit of the height
            if (layout[selectedControl].type == IMAGEBUTTON) 
            { 
                if (layout[selectedControl].rec.height <= texture.height/6 + 5) layout[selectedControl].rec.height = texture.height/6 + 5;
            }
            else if (layout[selectedControl].rec.height <= 20) layout[selectedControl].rec.height = 20;
        }
        

            
        // TODO: Draw global app screen limits (black rectangle with black default anchor)
        
        // Shows or hides the grid if not in textEditMode
        if (IsKeyPressed(KEY_G) && (!textEditMode)) showGrid = !showGrid;
     
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_S))
        {
            char currrentPathFile[256];

            // Add sample file name to currentPath
            strcpy(currrentPathFile, currentPath);
            //strcat(currrentPathFile, defaultName);

            // Save file dialog
            const char *filters[] = { "*.rgl" };
            const char *fileName = tinyfd_saveFileDialog("Save raygui layout text file", currrentPathFile, 1, filters, "raygui Layout Files (*.rgl)");

            if (fileName != NULL)
            {
                // Save layout file (text or binary)
                SaveLayoutRGL("test_layout.rgl", true);
                fileName = "";
            }
        }
        
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_O))
        {
            // Open file dialog
            const char *filters[] = { "*.rgl" };
            const char *fileName = tinyfd_openFileDialog("Load raygui layout file", currentPath, 1, filters, "raygui Layout Files (*.rgl)", 0);
            
            if (fileName != NULL) LoadLayoutRGL(fileName);
        }
        
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_ENTER)) GenerateLayoutCode("test_layout.c");
        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();

            ClearBackground(RAYWHITE);
            
            if (showGrid) DrawGrid2D(GetScreenWidth()/13, GetScreenHeight()/13);
           
            // Draws the defaultRec[selectedType] of the control selected
            if (selectedControl == -1 && !anchorMode)
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
                    case LISTVIEW: GuiListView(defaultRec[selectedTypeDraw], guiControls, 14, 1); break;
                    case COLORPICKER: GuiColorPicker(defaultRec[selectedTypeDraw], RED); break;
                    default: break;
                }
            }
            
            for (int i = 0; i < controlsCounter; i++)
            {
                // Draws the Controls when placed on the grid.
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
                    case LISTVIEW: GuiListView((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, guiControls, 14, 1); break;
                    case COLORPICKER: GuiColorPicker((Rectangle){ layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, layout[i].rec.width, layout[i].rec.height }, RED); break;
                    default: break;
                }

                if ((layout[i].ap->id == selectedAnchor) && (layout[i].ap->id > 0)) DrawLine(layout[i].ap->x, layout[i].ap->y, layout[i].ap->x + layout[i].rec.x, layout[i].ap->y + layout[i].rec.y, RED);
                // Draw Control anchor information
                // DrawText(FormatText("Id: %i | X: %i | Y: %i | Enabled: %i", layout[0].ap->id, layout[0].ap->x, layout[0].ap->y, layout[0].ap->enabled), 100, 100, style[DEFAULT_TEXT_SIZE], RED);
            }
            
            // Draw the list of controls
            DrawRectangleRec(listViewControls, Fade(WHITE, 0.7f));
            selectedType = GuiListView(listViewControls, guiControls, 14, selectedType);
            
            // Draw the list of controlsCounter
            DrawRectangleRec(listViewControlsCounter, Fade(WHITE, 0.7f));
            GuiListView(listViewControlsCounter, guiControlsCounter, controlsCounter, selectedControl);
            
            // Draw the anchorPoints
            for (int i = 1; i < MAX_ANCHOR_POINTS; i++)
            {
                if (anchors[i].id == selectedAnchor) DrawCircle(anchors[i].x, anchors[i].y, anchors[i].radius, Fade(RED, 0.5f));
                else DrawCircleLines(anchors[i].x, anchors[i].y, anchors[i].radius, Fade(RED, 0.5f));
                DrawRectangle(anchors[i].x - anchors[i].radius - 5, anchors[i].y, anchors[i].radius*2 + 10, 1, RED);
                DrawRectangle(anchors[i].x, anchors[i].y - anchors[i].radius - 5, 1, anchors[i].radius*2 + 10, RED);
            }
            
            if ((selectedControl != -1) && (selectedControl < controlsCounter))
            {
                
                DrawRectangleRec((Rectangle){ layout[selectedControl].ap->x + layout[selectedControl].rec.x, layout[selectedControl].ap->y + layout[selectedControl].rec.y, layout[selectedControl].rec.width, layout[selectedControl].rec.height }, Fade(RED, 0.5f));
                if (layout[selectedControl].ap->id > 0) DrawLine(layout[selectedControl].ap->x, layout[selectedControl].ap->y, layout[selectedControl].ap->x + layout[selectedControl].rec.x, layout[selectedControl].ap->y + layout[selectedControl].rec.y, RED);
            }
             
            if ((selectedControl == -1)  && (selectedAnchor == -1))
            {
                if (anchorMode)
                {
                    DrawCircleLines(mouseX, mouseY, anchors[0].radius, Fade(RED, 0.5f));
                    DrawRectangle(mouseX - anchors[0].radius - 5, mouseY, anchors[0].radius*2 + 10, 1, RED);
                    DrawRectangle(mouseX, mouseY - anchors[0].radius - 5, 1, anchors[0].radius*2 + 10, RED);
                }
                else 
                {
                    DrawRectangle(mouseX - 8, mouseY, 17, 1, RED);
                    DrawRectangle(mouseX, mouseY - 8, 1, 17, RED);
                }
            }

            // Draws the cursor of textEditMode
            if (textEditMode)
            {
               if (((framesCounter/20)%2) == 0)
               {
                    if (layout[selectedControl].type == LABEL) DrawText("|", layout[selectedControl].rec.x + MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE]) + 2, layout[selectedControl].rec.y - 1, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else if (layout[selectedControl].type == IMAGEBUTTON) DrawText("|", layout[selectedControl].rec.x + layout[selectedControl].rec.width/2 + MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE])/2 + texture.width/6, layout[selectedControl].rec.y + layout[selectedControl].rec.height/2 - 6, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
                    else DrawText("|", layout[selectedControl].rec.x + layout[selectedControl].rec.width/2 + MeasureText(layout[selectedControl].text , style[DEFAULT_TEXT_SIZE])/2 + 2, layout[selectedControl].rec.y + layout[selectedControl].rec.height/2 - 6, style[DEFAULT_TEXT_SIZE] + 2, BLACK);
               }
            }
            
            if (anchorLinkMode) DrawLine(anchors[linkedAnchor].x, anchors[linkedAnchor].y, mouseX, mouseY, BLACK);
            
            // Debug information
            /*DrawText(FormatText("Controls count: %i", controlsCounter), 10, screenHeight - 20, 20, BLUE);
            DrawText(FormatText("Selected type: %s", controlTypeName[selectedType]), 300, screenHeight - 20, 20, BLUE);
            if (snapMode) DrawText("SNAP ON", 700, screenHeight - 20, 20, RED);
            if (selectedControl != -1) DrawText(FormatText("rec: (%i, %i, %i, %i)", 
                                                layout[selectedControl].rec.x, layout[selectedControl].rec.y, 
                                                layout[selectedControl].rec.width, layout[selectedControl].rec.height), 
                                                10, screenHeight - 40, 10, DARKGREEN);
            DrawText(FormatText("mouse: (%i, %i)", mouseX, mouseY), 700, screenHeight - 40, 10, RED);
            */
            // Draw status bar bottom with debug information
            DrawRectangle(0, GetScreenHeight() - 24, GetScreenWidth(), 24, LIGHTGRAY);
            GuiLabel((Rectangle){20, GetScreenHeight() - 16, 100, 20}, FormatText("Controls count: %i", controlsCounter));
            GuiLabel((Rectangle){125, GetScreenHeight() - 16, 100, 20}, FormatText("|  Mouse: (%i, %i)", mouseX, mouseY));
            GuiLabel((Rectangle){230, GetScreenHeight() - 16, 100, 20}, FormatText("|  Selected Control: %s (%i, %i, %i, %i)", controlTypeName[selectedType], layout[selectedControl].rec.x, layout[selectedControl].rec.y, layout[selectedControl].rec.width, layout[selectedControl].rec.height));
            GuiLabel((Rectangle){590, GetScreenHeight() - 16, 100, 20}, "|");
            if (snapMode) DrawText("SNAP ON", 600, GetScreenHeight() - 16, style[DEFAULT_TEXT_SIZE], RED);
            else DrawText("SNAP OFF", 600, GetScreenHeight() - 16, style[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_NORMAL]));
            if (anchorMode )GuiLabel((Rectangle){20, GetScreenHeight() - 34, 100, 20}, FormatText("Anchor Mode ON   | Anchors count: %i  |  Selected Anchor: %i  |  Linked Anchor: %i", anchorCounter, selectedAnchor, linkedAnchor));
            else GuiLabel((Rectangle){20, GetScreenHeight() - 34, 100, 20}, FormatText("Anchor Mode OFF   | Anchors count: %i  |  Selected Anchor: %i  |  Linked Anchor: %i", anchorCounter, selectedAnchor, linkedAnchor));
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    // De-Initialization
    //--------------------------------------------------------------------------------------
    UnloadTexture(texture);    
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

            for (int i = 0; i < controlsCounter; i++) 
            {
                // Export data in independent way
                fwrite(&layout[i].id, 1, sizeof(int), rglFile);
                fwrite(&layout[i].type, 1, sizeof(int), rglFile);
                fwrite(&layout[i].rec, 1, sizeof(Rectangle), rglFile);
                fwrite(layout[i].text, 1, 32, rglFile);
                
                // TODO: Export anchors data
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
            fprintf(rglFile, "# Control info: c <id> <type> <rectangle> <text> <anchor_id> <anchor_pos>\n#\n");

            for (int i = 0; i < controlsCounter; i++)
            {
                fprintf(rglFile, "c %03i %i %i %i %i %i %i %i %i %s\n", layout[i].id, layout[i].type, layout[i].rec.x, layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].ap->id, layout[i].ap->x, layout[i].ap->y, layout[i].text);
            }

            fclose(rglFile);
        }
    }
}

// Import gui layout project information
// NOTE: Imported from text file
static void LoadLayoutRGL(const char *fileName)
{
    char buffer[256];
    bool tryBinary = false;
    
    int anchorId = 0;       // TODO: Review!!!
    int anchorX = 0;
    int anchorY = 0;

    FILE *rglFile = fopen(fileName, "rt");
    
    if (rglFile != NULL)
    {
        fgets(buffer, 256, rglFile);

        if (buffer[0] != 'R')   // Text file!
        {
            controlsCounter = 0;
            
            while (!feof(rglFile))
            {
                if ((buffer[0] != '\n') && (buffer[0] != '#'))
                {
                    sscanf(buffer, "c %d %i %i %i %i %i %d %d %d %[^\n]s", &layout[controlsCounter].id, &layout[controlsCounter].type, &layout[controlsCounter].rec.x, &layout[controlsCounter].rec.y, &layout[controlsCounter].rec.width, &layout[controlsCounter].rec.height, &anchorId, &anchorX, &anchorY, layout[controlsCounter].text);
                    printf("c %d %i %i %i %i %i %s %i %i %i\n", layout[controlsCounter].id, layout[controlsCounter].type, layout[controlsCounter].rec.x, layout[controlsCounter].rec.y, layout[controlsCounter].rec.width, layout[controlsCounter].rec.height, layout[controlsCounter].text, anchorId, anchorX, anchorY);
                    
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
                for (int i = 0; i < controlsCounter; i++)
                {
                    // Import data in independent way
                    fread(&layout[i].id, 1, sizeof(int), rglFile);
                    fread(&layout[i].type, 1, sizeof(int), rglFile);
                    fread(&layout[i].rec, 1, sizeof(Rectangle), rglFile);
                    fread(layout[i].text, 1, 32, rglFile);
                    /*
                    // Import anchor id and position
                    // fread(&layout[i].ap->id, 1, sizeof(int), rglFile);
                    // fread(&layout[i].ap->x, 1, sizeof(int), rglFile);
                    // fread(&layout[i].ap->y, 1, sizeof(int), rglFile);
                    */

                    printf("[READ] Controls info: id-%i type-%i rec-%i,%i,%i,%i text-%s\n", layout[i].id, layout[i].type, layout[i].rec.x, layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].text);
                }
            }
            else TraceLog(LOG_WARNING, "[raygui] Invalid layout file");
            
            fclose(rglFile);
        }
    }
    
    printf("[READ] Layout data loaded successfully!\n");
}

// Generate C code for gui layout
static void GenerateLayoutCode(const char *fileName)
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
    
    // Define controls rectangles
    fprintf(ftool, "    // Define controls rectangles\n");
    fprintf(ftool, "    Rectangle layoutRecs[%i] = {\n", controlsCounter);
    
    for (int i = 0; i < controlsCounter; i++)
    {
        fprintf(ftool, "        (Rectangle){ %i, %i, %i, %i }", (layout[i].rec.x + layout[i].ap->x), (layout[i].rec.y + layout[i].ap->y), layout[i].rec.width, layout[i].rec.height);
        
        if (i == controlsCounter - 1) fprintf(ftool, "\t\t// %s %03i\n    };\n\n", controlTypeName[layout[i].type], i);
        else fprintf(ftool, ",\t\t// %s %03i\n", controlTypeName[layout[i].type], i);
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
            case COLORPICKER: fprintf(ftool, "\t\t\t%sColor%03i = GuiColorPicker(layoutRecs[%i], %sColor%03i);\n\n", controlTypeNameShort[layout[i].type], i, i, controlTypeNameShort[layout[i].type], i); break;
            
            default: break;
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
