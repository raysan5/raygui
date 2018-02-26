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
    int anchorId;
    Vector2 ap;
} GuiControl;

// Anchor control type
typedef struct {
    int id;
    Vector2 position;
    Rectangle bounds;
} AnchorControl;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static char currentPath[256];       // Path to current working folder
static int screenWidth = 800;
static int screenHeight = 600;

static GuiControl layout[MAX_GUI_CONTROLS];
static AnchorControl anchorPoint[MAX_GUI_CONTROLS];  
static int controlsCounter = 0;
static int anchorCounter = 1;       // NOTE: anchorCounter starts at 1 because the global anchor is 0

//Rectangle defaultControlWidth[] = { };

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
    bool anchorMode = false; 
    int selectedAnchor = -1;

    bool lockMode = false;
    
    // Used to draw the preview of selectedControl
    Rectangle defaultRec[14] = {
        (Rectangle){ 0, 0, 80, 20},             // LABEL
        (Rectangle){ 0, 0, 100, 30},            // BUTTON
        (Rectangle){ 0, 0, 120, 40},            // IMAGEBUTTON
        (Rectangle){ 0, 0, 100, 30},            // TOGGLE
        (Rectangle){ 0, 0, 240, 30},             // TOGGLEGROUP  
        (Rectangle){ 0, 0, 200, 20},            // SLIDER
        (Rectangle){ 0, 0, 200, 20},            // SLIDERBAR
        (Rectangle){ 0, 0, 200, 20},            // PROGRESSBAR
        (Rectangle){ 0, 0, 150, 30},            // SPINNER
        (Rectangle){ 0, 0, 150, 30},            // COMBOBOX
        (Rectangle){ 0, 0, 20, 20},             // CHECKBOX
        (Rectangle){ 0, 0, 120, 30},             // TEXTBOX    
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
    
    // Defines anchorPoint parent(0, 0)
    anchorPoint[0].position = (Vector2){ 0, 0 };
    anchorPoint[0].id = 0;
    anchorPoint[0].bounds = (Rectangle){ anchorPoint[0].position.x - 10, anchorPoint[0].position.y - 10, 20, 20 };

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
    
    // TODO: Initialize layout controls to default values
    
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
        if (IsKeyPressed(KEY_LEFT_CONTROL))
        {
            startPosXListViewControlsCounter = listViewControlsCounter.x;
            deltaPosXListViewControlsCounter = GetScreenWidth() -listViewControlsCounter.width - startPosXListViewControlsCounter;
            counterListViewControlsCounter = 0;
        }
        
        if (IsKeyDown(KEY_LEFT_CONTROL))
        {
            counterListViewControlsCounter++;
            if (counterListViewControlsCounter >= 60) counterListViewControlsCounter = 60;
            listViewControlsCounter.x = (int)EaseCubicInOut(counterListViewControlsCounter, startPosXListViewControlsCounter, deltaPosXListViewControlsCounter, 60);
        }  
        else if (IsKeyReleased(KEY_LEFT_CONTROL))
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
            layout[controlsCounter].anchorId = anchorPoint[0].id;                   // Sets the anchorId to the global anchorPoint
            layout[controlsCounter].ap.x = mouseX - anchorPoint[0].position.x;      // Gets the distance between anchorPoint and control in X
            layout[controlsCounter].ap.y = mouseY - anchorPoint[0].position.y;      // Gets the distance between anchorPoint and control in Y
            // Defines the control rectangle in reference with the anchorPoint
            layout[controlsCounter].rec = (Rectangle){ anchorPoint[layout[controlsCounter].anchorId].position.x + layout[controlsCounter].ap.x - layout[controlsCounter].rec.width/2, anchorPoint[layout[controlsCounter].anchorId].position.y + layout[controlsCounter].ap.y - layout[controlsCounter].rec.height/2, defaultRec[selectedType].width, defaultRec[selectedType].height };
            strcpy(layout[controlsCounter].text, "SAMPLE TEXT\0");
            
            controlsCounter++;
        }
        
        for (int i = 0; i < controlsCounter; i++)
        {
            if (controlDrag || lockMode) break;
            if (CheckCollisionPointRec(GetMousePosition(), layout[i].rec)) 
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
                layout[selectedControl].rec.x = mouseX - layout[selectedControl].rec.width/2;
                layout[selectedControl].rec.y = mouseY - layout[selectedControl].rec.height/2;
                
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
            if (IsKeyDown(KEY_DELETE))
            {
                for (int i = selectedControl; i < controlsCounter; i++) layout[i] = layout[i + 1];

                controlsCounter--;
                selectedControl = -1;
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
            //Enables the defaultRec[selectedType]
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
        
        // Enables or disables snapMode if not in textEditMode
        if (IsKeyPressed(KEY_S) && (!textEditMode)) snapMode = !snapMode;
        
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
        if (IsKeyPressed(KEY_T) && (selectedControl != -1) && 
           ((selectedType == LABEL) || (selectedType == BUTTON) || (selectedType == TOGGLE) || (selectedType == IMAGEBUTTON)))
        {   
            textEditMode = true;
            saveControlSelected = selectedControl;
        }
        
        if (lockMode)
        {
            selectedControl = saveControlSelected;
        }
        
        if (IsKeyPressed(KEY_SPACE) && !textEditMode && (selectedControl != -1) && !lockMode) 
        {
            lockMode = true;
            saveControlSelected = selectedControl;
        }
        else if (IsKeyPressed(KEY_SPACE) && (selectedControl != -1))
        {
            lockMode = false;
        }

        
        // TODO: Create anchor points
        // NOTE: There is a bug if TEXTBOX is the selectedType
        if (anchorMode)
        {
            // TODO: On mouse click anchor is created
            //selectedControl = -1;
            
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !controlCollision && selectedAnchor == -1 && selectedControl == -1)
            {
                anchorPoint[anchorCounter].id = anchorCounter;
                anchorPoint[anchorCounter].position = (Vector2){ mouseX, mouseY };
                anchorPoint[anchorCounter].bounds = (Rectangle){ anchorPoint[anchorCounter].position.x - 5, anchorPoint[anchorCounter].position.y - 5, 10, 10 };
                anchorCounter++;
            }
            
            for (int i = 0; i < anchorCounter; i++)
            {
                if (CheckCollisionPointRec(GetMousePosition(), anchorPoint[i].bounds)) 
                {
                    selectedAnchor = i;
                    break;
                }
                else selectedAnchor = -1;
            }  
            
            if (selectedAnchor != -1 && selectedAnchor != 0)
            {
                if (IsKeyDown(KEY_DELETE))
                {
                    for (int i = selectedAnchor; i < anchorCounter; i++) anchorPoint[i] = anchorPoint[i + 1];

                    anchorCounter--;
                    selectedAnchor = -1;
                }
            }
        }
        
        if (IsKeyPressed(KEY_A) && !textEditMode && (layout[selectedControl].type != TEXTBOX) && !anchorMode)  // Anchor mode creation, consider SNAP!
        {
            anchorMode = true;
            
        }
        else if (IsKeyPressed(KEY_A) && !textEditMode && (layout[selectedControl].type != TEXTBOX) && anchorMode) anchorMode = false;
        
        
        // Updates the position of the control when it has an anchor point
        for (int i = 0; i < controlsCounter; i++)
        {
            //layout[i].rec = (Rectangle){ anchorPoint[layout[i].anchorId].position.x + layout[i].ap.x - layout[i].rec.width/2, anchorPoint[layout[i].anchorId].position.y + layout[i].ap.y - layout[i].rec.height/2, layout[i].rec.width, layout[i].rec.height };
        }
       
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
        
        // TODO: If mouse over anchor (define default bounds) and click, start anchor line
        // TODO: On mouse up over an existing control, anchor is created (draw line for reference)
        // TODO: On anchor line created, control (x, y) will be (x - ap, y - ap), and anchorId will be saved
        // TODO: When create new anchor (anchorId++)
        
        // TODO: if (IsKeyPressed(KEY_R)) remove control anchors (reset)
            
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
                    case TEXTBOX: GuiTextBox(defaultRec[selectedTypeDraw], "test text", 32); break;
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
                    case LABEL: GuiLabel(layout[i].rec, layout[i].text); break;
                    case BUTTON: GuiButton(layout[i].rec, layout[i].text); break;
                    case IMAGEBUTTON: GuiImageButtonEx(layout[i].rec, texture, (Rectangle){ 0, 0, texture.width/3, texture.height/6 }, layout[i].text); break;
                    case TOGGLE: GuiToggleButton(layout[i].rec, layout[i].text, false); break;
                    case TOGGLEGROUP: GuiToggleGroup(layout[i].rec, list, 3, 1); break;
                    case SLIDER: GuiSlider(layout[i].rec, 40, 0, 100); break;
                    case SLIDERBAR: GuiSliderBar(layout[i].rec, 40, 0, 100); break;
                    case PROGRESSBAR: GuiProgressBar(layout[i].rec, 40, 0, 100); break;
                    case SPINNER: GuiSpinner(layout[i].rec, 40, 0, 100); break;
                    case COMBOBOX: GuiComboBox(layout[i].rec, list, 3, 1); break;
                    case CHECKBOX: GuiCheckBox(layout[i].rec, false); break;
                    case TEXTBOX: GuiTextBox(layout[i].rec, layout[i].text, 32); break;
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
            GuiListView(listViewControlsCounter, guiControlsCounter, controlsCounter, selectedControl);
            
            // Draw the global anchorPoint
            DrawRectangleRec(anchorPoint[0].bounds, Fade(BLACK, 0.5f));
            DrawLine(anchorPoint[0].position.x - 15, anchorPoint[0].position.y, anchorPoint[0].position.x + 15, anchorPoint[0].position.y, BLACK);
            DrawLine(anchorPoint[0].position.x, anchorPoint[0].position.y - 15, anchorPoint[0].position.x, anchorPoint[0].position.y + 15, BLACK);
            
            // Draw the anchorPoints
            for (int i = 1; i < anchorCounter; i++)
            {
                DrawRectangleRec(anchorPoint[i].bounds, Fade(RED, 0.5f));
                DrawLine(anchorPoint[i].position.x - 8, anchorPoint[i].position.y, anchorPoint[i].position.x + 8, anchorPoint[i].position.y, RED);
                DrawLine(anchorPoint[i].position.x, anchorPoint[i].position.y - 8, anchorPoint[i].position.x, anchorPoint[i].position.y + 8, RED);
                
            }
            
            if ((selectedControl != -1) && (selectedControl < controlsCounter)) DrawRectangleRec(layout[selectedControl].rec, Fade(RED, 0.5f));
             
            if (selectedControl == -1)
            {
                DrawRectangle(mouseX - 8, mouseY, 17, 1, RED);
                DrawRectangle(mouseX, mouseY - 8, 1, 17, RED);
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
            if (anchorMode )GuiLabel((Rectangle){20, GetScreenHeight() - 32, 100, 20}, FormatText("Anchor Mode ON   | Anchors count: %i  |  Selected Anchor: %i", anchorCounter, selectedAnchor));
            else GuiLabel((Rectangle){20, GetScreenHeight() - 32, 100, 20}, FormatText("Anchor Mode OFF   | Anchors count: %i  |  Selected Anchor: %i", anchorCounter, selectedAnchor));
            
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
            
            for (int i = 0; i < controlsCounter; i++) fwrite(&layout[i], 1, sizeof(GuiControl), rglFile);

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
            fprintf(rglFile, "# Control info: c <id> <type> <rectangle> <anchor_id> <text>\n#\n", controlsCounter);

            for (int i = 0; i < controlsCounter; i++)
            {

                // fprintf(rglFile, "Control %03i : %s\n", layout[i].id, controlTypeName[layout[i].type]);
                // fprintf(rglFile, "Rec %i %i %i %i\n", layout[i].rec.x, layout[i].rec.y, layout[i].rec.width, layout[i].rec.height);
                // fprintf(rglFile, "Text %s\n", layout[i].text);
                // fprintf(rglFile, "Anchor Id %i\n\n", layout[i].anchorId);
                fprintf(rglFile, "c %03i %i %i %i %i %i %i %s\n", layout[i].id, layout[i].type, layout[i].rec.x, layout[i].rec.y, layout[i].rec.width, layout[i].rec.height, layout[i].anchorId, layout[i].text);
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
                    sscanf(buffer, "c %d %i %i %i %i %i %i %[^\n]s", &layout[controlsCounter].id, &layout[controlsCounter].type, &layout[controlsCounter].rec.x, &layout[controlsCounter].rec.y, &layout[controlsCounter].rec.width, &layout[controlsCounter].rec.height, &layout[controlsCounter].anchorId, layout[controlsCounter].text);
                    printf("c %d %i %i %i %i %i %i %s\n", layout[controlsCounter].id, layout[controlsCounter].type, layout[controlsCounter].rec.x, layout[controlsCounter].rec.y, layout[controlsCounter].rec.width, layout[controlsCounter].rec.height, layout[controlsCounter].anchorId, layout[controlsCounter].text);
                    
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

            fread(signature, 1, 4, rglFile);
            fread(&version, 1, sizeof(short), rglFile);
            fread(&controlsCounter, 1, sizeof(short), rglFile);
            fread(&reserved, 1, sizeof(int), rglFile);
           
            if ((signature[0] == 'R') &&
                (signature[1] == 'G') &&
                (signature[2] == 'L') &&
                (signature[3] == ' '))
            {
                while (!feof(rglFile))
                {
                    for (int i = 0; i < controlsCounter; i++) fread(&layout[i], 1, sizeof(GuiControl), rglFile);
                }   
            }
            else TraceLog(LOG_WARNING, "[raygui] Invalid layout file");
            
            fclose(rglFile);
        }
    }
    
    // char line[128];

    // FILE *rglFile = fopen(fileName, "rt");
    
    // controlsCounter = 0;
    
    // while (!feof(rglFile))
    // {
        // fgets(line, 128, rglFile);

        // switch (line[0])
        // {
            // case 'c':
            // {
                // sscanf(line, "c type %i rec %i %i %i %i", &layout[controlsCounter].type, 
                                                          // &layout[controlsCounter].rec.x, 
                                                          // &layout[controlsCounter].rec.y, 
                                                          // &layout[controlsCounter].rec.width, 
                                                          // &layout[controlsCounter].rec.height);
                // controlsCounter++;
            // } break;
            // default: break;
        // }
    // }

    // fclose(rglFile);
}

// Generate C code for gui layout
static void GenerateLayoutCode(const char *fileName)
{
    #define RGL_TOOL_NAME           "tool_name"
    #define RGL_TOOL_DESCRIPTION    "tool_name"
    #define RGL_TOOL_AUTHOR         "tool_name"
    #define RGL_TOOL_YEAR           "tool_name"
    
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