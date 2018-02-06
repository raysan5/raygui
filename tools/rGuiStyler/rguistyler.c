/*******************************************************************************************
*
*   rGuiStyler - raygui Style Editor
*
*   Compile this program using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) external/tinyfiledialogs.c -I..\.. \ 
*       -lraylib -lglfw3 -lopengl32 -lgdi32 -lcomdlg32 -lole32 -std=c99 -Wall
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2016 Sergio Martinez and Ramon Santamaria
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
#include "raygui.h"

#include "external/tinyfiledialogs.h"   // Open/Save file dialogs

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

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
#define FONT_SIZE           10
#define COLOR_REC           GuiLinesColor()
#define CONTROL_LIST_HEIGHT      38
#define STATUS_BAR_HEIGHT   25

#define NUM_CONTROLS        15
#define NUM_STYLES          12

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { 
    LABEL = 0, 
    LABELBUTTON,
    BUTTON, 
    IMAGEBUTTON,
    TOGGLE, 
    TOGGLEGROUP, 
    SLIDER, 
    SLIDERBAR, 
    PROGRESSBAR, 
    CHECKBOX, 
    SPINNER, 
    COMBOBOX, 
    TEXTBOX,
    LISTVIEW,
    COLORPICKER
} GuiControlType;

typedef enum { 
    BORDER_COLOR_NORMAL = 0,
    BASE_COLOR_NORMAL,
    TEXT_COLOR_NORMAL,
    BORDER_COLOR_FOCUSED,
    BASE_COLOR_FOCUSED,
    TEXT_COLOR_FOCUSED,
    BORDER_COLOR_PRESSED,
    BASE_COLOR_PRESSED,
    TEXT_COLOR_PRESSED,
    BORDER_COLOR_DISABLED,
    BASE_COLOR_DISABLED,
    TEXT_COLOR_DISABLED
} GuiStyleType;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static char currentPath[256];       // Path to current working folder

const char *guiControlText[NUM_CONTROLS] = { 
    "LABEL", 
    "LABELBUTTON",
    "BUTTON", 
    "IMAGEBUTTON",
    "TOGGLE", 
    "TOGGLEGROUP", 
    "SLIDER", 
    "SLIDERBAR", 
    "PROGRESSBAR", 
    "CHECKBOX", 
    "SPINNER", 
    "COMBOBOX", 
    "TEXTBOX",
    "LISTVIEW",
    "COLORPICKER"
};

const char *guiStylesText[NUM_STYLES] = { 
    "BORDER_COLOR_NORMAL",
    "BASE_COLOR_NORMAL",
    "TEXT_COLOR_NORMAL",
    "BORDER_COLOR_FOCUSED",
    "BASE_COLOR_FOCUSED",
    "TEXT_COLOR_FOCUSED",
    "BORDER_COLOR_PRESSED",
    "BASE_COLOR_PRESSED",
    "TEXT_COLOR_PRESSED",
    "BORDER_COLOR_DISABLED",
    "BASE_COLOR_DISABLED",
    "TEXT_COLOR_DISABLED"
};

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void BtnLoadStyle(void);     // Button load style function
static void BtnSaveStyle(void);     // Button save style function

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 700;
    const int screenHeight = 700;
    
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "rGuiStyler - raygui style editor");
    //SetWindowMinSize(700, 700);
    
    int dropsCount = 0;
    char **droppedFiles;
    
    int guiPosX = 340;
    int guiPosY = 35;

    // TODO: Define gui controls rectangles
    Rectangle bounds[NUM_CONTROLS] = {
        (Rectangle){ guiPosX + 20, guiPosY + 20, 30, 10 },          // LABEL
        (Rectangle){ guiPosX + 60, guiPosY + 20, 62, 10 },          // LABELBUTTON
        (Rectangle){ guiPosX + 140, guiPosY + 10, 150, 30 },        // BUTTON
        (Rectangle){ guiPosX + 300, guiPosY + 10, 30, 30 },         // IMAGEBUTTON
        (Rectangle){ guiPosX + 20, guiPosY + 60, 80, 30 },          // TOGGLE
        (Rectangle){ guiPosX + 120, guiPosY + 60, 75, 30 },         // TOGGLEGROUP
        (Rectangle){ guiPosX + 20, guiPosY + 110, 330, 30 },        // SLIDER
        (Rectangle){ guiPosX + 20, guiPosY + 150, 330, 30 },        // SLIDERBAR
        (Rectangle){ guiPosX + 20, guiPosY + 190, 290, 30 },        // PROGRESSBAR    
        (Rectangle){ guiPosX + 320, guiPosY + 190, 30, 30 },        // CHECKBOX
        (Rectangle){ guiPosX + 20, guiPosY + 240, 150, 30 },        // SPINNER
        (Rectangle){ guiPosX + 200, guiPosY + 240, 150, 30 },       // COMBOBOX
        (Rectangle){ guiPosX + 20, guiPosY + 290, 150, 30 },        // TEXTBOX
        (Rectangle){ 0, guiPosY, 140, GetScreenHeight() - 100 },    // LISTVIEW
        (Rectangle){ guiPosX + 20, guiPosY + 330, 240, 240 },       // COLORPICKER
        
        
    };
        
    // Get current directory
    // NOTE: Current working directory could not match current executable directory
    GetCurrentDir(currentPath, sizeof(currentPath));
    currentPath[strlen(currentPath)] = '\\';
    currentPath[strlen(currentPath) + 1] = '\0';      // Not really required
    
    GuiLoadStyleImage("resources/rguistyle_default_light.png");
    
    Texture2D texIcons = LoadTexture("resources/icons.png");
    Texture2D texLogo = LoadTexture("resources/logo128x128.png");
    
    SetTargetFPS(60);
    //------------------------------------------------------------

    // Gui controls data
    //-----------------------------------------------------------
    bool toggle = false;
    bool toggleValue = false;
    const char *toggleGuiText[3] = { "toggle", "group", "selection" };

    float sliderValue = 50.0f;
    float sliderBarValue = 20.0f;
    float progressValue = 0.0f;
    
    bool checked = false;
    
    int spinnerValue = 20;

    int comboNum = 5;
    const char *comboText[5] = { "this", "is", "a" ,"combo", "box" };
    int comboActive = 0;
    
    char guiText[17] =  { '\0' };

    Vector2 colorPickerPos = { (float)screenWidth - 287, 20.0f };
    Color colorPickerValue = RED;
    
    int listViewActive = -1;
    int listViewStyleActive = -1;
    //--------------------------------------------------------------------------------------
    
    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (IsFileDropped())
        {
            droppedFiles = GetDroppedFiles(&dropsCount);
            GuiLoadStyle(droppedFiles[0]);
            ClearDroppedFiles();
        }
        
        //colorPickerValue = GetColor(GuiGetStyleProperty(BUTTON_BASE_COLOR_NORMAL));
        //GuiSetStyleProperty(BUTTON_BASE_COLOR_NORMAL, GetHexValue(colorPickerValue));
        
        // Update progress bar automatically
        progressValue += 0.0005f;
        if (progressValue > 1.0f) progressValue = 0.0f;
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), GuiBackgroundColor());
            
            //Draw top and bottom bars
            DrawRectangle(0, 0, GetScreenWidth(), 25, GRAY);
            DrawRectangle(0, GetScreenHeight() - 25, GetScreenWidth(), 25, LIGHTGRAY);
            
            //Draw top and bottom bars' text
            GuiLabel((Rectangle){20, GetScreenHeight() - 18, 100, 20}, FormatText("CURRENT SELECTION: %s_%s", guiControlText[listViewActive], guiStylesText[listViewStyleActive]));
            GuiLabel((Rectangle){guiPosX + 100, GetScreenHeight() - 18, 100, 20}, FormatText("SAVE STATUS: %s", guiText));
            
            DrawText("CHOOSE CONTROL", 25, 10, styleGeneric[DEFAULT_TEXT_SIZE], LIGHTGRAY);
            DrawText(">   CHOOSE PROPERTY STYLE", 140, 10, styleGeneric[DEFAULT_TEXT_SIZE], LIGHTGRAY);
            DrawText(">                            STYLE VIEWER", guiPosX, 10, styleGeneric[DEFAULT_TEXT_SIZE], LIGHTGRAY);
                       
            

            // Gui controls
            listViewActive = GuiListView(bounds[LISTVIEW], guiControlText, NUM_CONTROLS, listViewActive);
            
            if (listViewActive < 0) GuiDisable();
            listViewStyleActive = GuiListView((Rectangle){ 145, guiPosY, 180, GetScreenHeight() - 100 }, guiStylesText, NUM_STYLES, listViewStyleActive);
            GuiEnable();

            GuiLabel(bounds[LABEL], "Label");

            if (GuiLabelButton(bounds[LABELBUTTON], "LabelButton")) {}
            
            if (GuiImageButtonEx(bounds[IMAGEBUTTON], texIcons , (Rectangle){ 0, 0, texIcons.width/3, texIcons.height/6 }, "test")) { }
            
            if (toggle) toggle = GuiToggleButton(bounds[TOGGLE], "Toggle ACT", toggle);
            else toggle = GuiToggleButton(bounds[TOGGLE], "Toggle INE", toggle);
            
            toggleValue = GuiToggleGroup(bounds[TOGGLEGROUP], toggleGuiText, 3, toggleValue);
            
            sliderValue = GuiSlider(bounds[SLIDER], sliderValue, 0, 100);
            
            sliderBarValue = GuiSliderBar(bounds[SLIDERBAR], sliderBarValue, -10.0f, 40.0f);
            
            progressValue = GuiProgressBar(bounds[PROGRESSBAR], progressValue, 0.0f, 1.0f);
            
            spinnerValue = GuiSpinner(bounds[SPINNER], spinnerValue, 0, 100);
            
            comboActive = GuiComboBox(bounds[COMBOBOX], comboText, comboNum, comboActive);

            checked = GuiCheckBox(bounds[CHECKBOX], checked);
            
            GuiTextBox(bounds[TEXTBOX], guiText, 16);
            
            colorPickerValue = GuiColorPicker(bounds[COLORPICKER], colorPickerValue);

            // Draw labels for GuiColorPicker information
            GuiGroupBox((Rectangle){ guiPosX + 300, guiPosY + 340, 50, 80 }, "RGBA");
            // GuiLabel((Rectangle){controlsAnchorPos + guiWidth*1.9f, guiPosY + 7.2f*deltaY + guiHeight/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, guiWidth/2, guiHeight}, FormatText("R: %i", colorPickerValue.r));
            // GuiLabel((Rectangle){controlsAnchorPos + guiWidth*1.9f, guiPosY + 7.5f*deltaY + guiHeight/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, guiWidth/2, guiHeight}, FormatText("G: %i", colorPickerValue.g));
            // GuiLabel((Rectangle){controlsAnchorPos + guiWidth*1.9f, guiPosY + 7.8f*deltaY + guiHeight/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, guiWidth/2, guiHeight}, FormatText("B: %i", colorPickerValue.b));
            // GuiLabel((Rectangle){controlsAnchorPos + guiWidth*1.9f, guiPosY + 8.1f*deltaY + guiHeight/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, guiWidth/2, guiHeight}, FormatText("A: %i", colorPickerValue.a));
            // GuiLabel((Rectangle){controlsAnchorPos + guiWidth*1.9f, guiPosY + 12.5f*deltaY + guiHeight/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, guiWidth/2, guiHeight}, FormatText("#%x", GetHexValue(colorPickerValue)));
            
            // Draw Load and Save buttons
            if (GuiButton(bounds[BUTTON], "Load Style")) BtnLoadStyle();
            if (GuiButton((Rectangle){ guiPosX + 200, guiPosY + 290, 150, 30 }, "Save Style")) BtnSaveStyle();
            
            // TODO: Draw selected control rectangles
            if (listViewActive >= 0) DrawRectangleLinesEx(bounds[listViewActive], 1, RED);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------    
    UnloadTexture(texIcons);
    UnloadTexture(texLogo);
    ClearDroppedFiles();        // Clear internal buffers
    
    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------
    
    return 0;
}

//--------------------------------------------------------------------------------------------
// Module functions
//--------------------------------------------------------------------------------------------

// Button load style function
static void BtnLoadStyle(void)
{
    // Open file dialog
    const char *filters[] = { "*.rstyle" };

    const char *fileName; // = tinyfd_openFileDialog("Load raygui style file", currentPath, 1, filters, "raygui Style Files (*.rstyle)", 0);

    if (fileName != NULL) GuiLoadStyle(fileName);
}

// Button save style function
static void BtnSaveStyle(void)
{
    char currrentPathFile[256];

    // Add sample file name to currentPath
    strcpy(currrentPathFile, currentPath);
    strcat(currrentPathFile, "mystyle.rstyle\0");

    // Save file dialog
    const char *filters[] = { "*.rstyle" };
    const char *fileName; // = tinyfd_saveFileDialog("Save raygui style file", currrentPathFile, 1, filters, "raygui Style Files (*.rstyle)");

    if (fileName != NULL)
    {
        GuiSaveStyle(fileName);
        fileName = "";
    }
}

static int GetGuiStylePropertyIndex(int control, int property)
{
    int guiProp = -1;
    
    switch (control)
    {
        case LABEL: break;
        case LABELBUTTON: break;
        case BUTTON: guiProp = BUTTON_BORDER_COLOR_NORMAL + property; break;
        case IMAGEBUTTON: break;
        case TOGGLE: 
        case TOGGLEGROUP: guiProp = TOGGLE_BORDER_COLOR_NORMAL + property; break; 
        case SLIDER: break; 
        case SLIDERBAR: break; 
        case PROGRESSBAR: break; 
        case CHECKBOX: break; 
        case SPINNER: guiProp = SPINNER_BORDER_COLOR_NORMAL + property; break;
        case COMBOBOX: guiProp = COMBOBOX_BORDER_COLOR_NORMAL + property; break;
        case TEXTBOX: guiProp = TEXTBOX_BORDER_COLOR_NORMAL + property; break;
        case LISTVIEW: guiProp = LISTVIEW_BORDER_COLOR_NORMAL + property; break;
        case COLORPICKER: break;
        default: break;
    }

    return guiProp;
}
