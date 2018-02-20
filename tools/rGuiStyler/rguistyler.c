/*******************************************************************************************
*
*   rGuiStyler v2.0 - raygui Style Editor
*
*   Compile this program using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) external/tinyfiledialogs.c -I..\.. \ 
*       -lraylib -lglfw3 -lopengl32 -lgdi32 -lcomdlg32 -lole32 -std=c99 -Wall
*
*   CONTRIBUTORS:
*       Adria Arranz    - 2018 (v2.0)
*       Jordi Jorba     - 2018 (v2.0)
*       Sergio Martinez - 2015..2017 (v1.0)
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2016 Ramon Santamaria and Co.
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
#define RAYGUI_STYLE_DEFAULT_LIGHT
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

#define NUM_CONTROLS        13
#define NUM_STYLES_A         4
#define NUM_STYLES_B         8
#define NUM_STYLES_C        14

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
typedef enum { 
    DEFAULT = 0, 
    LABELBUTTON,
    BUTTON, 
    //IMAGEBUTTON,
    TOGGLE, 
    //TOGGLEGROUP, 
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

static int styleBackup[NUM_PROPERTIES] = { 0 };  

// NOTE: Some styles are shared by multiple controls:
// LABEL = LABELBUTTON
// BUTTON = IMAGEBUTTON
// TOGGLE = TOGGLEGROUP
const char *guiControlText[NUM_CONTROLS] = { 
    "DEFAULT", 
    "LABELBUTTON",
    "BUTTON", 
    //"IMAGEBUTTON",
    "TOGGLE", 
    //"TOGGLEGROUP", 
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

// NOTE: Used by controls: Label, LabelButton
const char *guiStylesTextA[NUM_STYLES_A] = { 
    "TEXT_COLOR_NORMAL",
    "TEXT_COLOR_FOCUSED",
    "TEXT_COLOR_PRESSED",
    "TEXT_COLOR_DISABLED"
};

//Note: Used by controls: Slider, SliderBar, ProgressBar, Checkbox, ColorPicker
const char *guiStylesTextB[NUM_STYLES_B] = { 
    "BORDER_COLOR_NORMAL",
    "BASE_COLOR_NORMAL",
    "BORDER_COLOR_FOCUSED",
    "BASE_COLOR_FOCUSED",
    "BORDER_COLOR_PRESSED",
    "BASE_COLOR_PRESSED",
    "BORDER_COLOR_DISABLED",
    "BASE_COLOR_DISABLED",
};

//Note: Used by controls: Button, ImageButton, Toggle, ToggleGroup, Spinner, ComboBox, TextBox, ListView
const char *guiStylesTextC[NUM_STYLES_C] = { 
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
    "TEXT_COLOR_DISABLED",
    "BACKGROUND_COLOR",
    "LINES_COLOR"
};

const char *guiPropertyText[NUM_PROPERTIES] = {
    "DEFAULT_BACKGROUND_COLOR",
    "DEFAULT_LINES_COLOR",
    "DEFAULT_TEXT_FONT",
    "DEFAULT_TEXT_SIZE",
    "DEFAULT_BORDER_WIDTH",
    "DEFAULT_BORDER_COLOR_NORMAL",
    "DEFAULT_BASE_COLOR_NORMAL",
    "DEFAULT_TEXT_COLOR_NORMAL",
    "DEFAULT_BORDER_COLOR_FOCUSED",
    "DEFAULT_BASE_COLOR_FOCUSED",
    "DEFAULT_TEXT_COLOR_FOCUSED",
    "DEFAULT_BORDER_COLOR_PRESSED",
    "DEFAULT_BASE_COLOR_PRESSED",
    "DEFAULT_TEXT_COLOR_PRESSED",
    "DEFAULT_BORDER_COLOR_DISABLED",
    "DEFAULT_BASE_COLOR_DISABLED",
    "DEFAULT_TEXT_COLOR_DISABLED",
    "LABEL_TEXT_COLOR_NORMAL",
    "LABEL_TEXT_COLOR_FOCUSED",
    "LABEL_TEXT_COLOR_PRESSED",
    "LABEL_TEXT_COLOR_DISABLED",
    "BUTTON_BORDER_WIDTH",
    "BUTTON_BORDER_COLOR_NORMAL",
    "BUTTON_BASE_COLOR_NORMAL",
    "BUTTON_TEXT_COLOR_NORMAL",
    "BUTTON_BORDER_COLOR_FOCUSED",
    "BUTTON_BASE_COLOR_FOCUSED",
    "BUTTON_TEXT_COLOR_FOCUSED",
    "BUTTON_BORDER_COLOR_PRESSED",
    "BUTTON_BASE_COLOR_PRESSED",
    "BUTTON_TEXT_COLOR_PRESSED",
    "BUTTON_BORDER_COLOR_DISABLED",
    "BUTTON_BASE_COLOR_DISABLED",
    "BUTTON_TEXT_COLOR_DISABLED",
    "TOGGLE_BORDER_WIDTH",
    "TOGGLE_BORDER_COLOR_NORMAL",
    "TOGGLE_BASE_COLOR_NORMAL",
    "TOGGLE_TEXT_COLOR_NORMAL",
    "TOGGLE_BORDER_COLOR_FOCUSED",
    "TOGGLE_BASE_COLOR_FOCUSED",
    "TOGGLE_TEXT_COLOR_FOCUSED",
    "TOGGLE_BORDER_COLOR_PRESSED",
    "TOGGLE_BASE_COLOR_PRESSED",
    "TOGGLE_TEXT_COLOR_PRESSED",
    "TOGGLE_BORDER_COLOR_DISABLED",
    "TOGGLE_BASE_COLOR_DISABLED",
    "TOGGLE_TEXT_COLOR_DISABLED",
    "TOGGLEGROUP_PADDING",
    "SLIDER_BORDER_WIDTH",
    "SLIDER_SLIDER_WIDTH",
    "SLIDER_BORDER_COLOR_NORMAL",
    "SLIDER_BASE_COLOR_NORMAL",
    "SLIDER_BORDER_COLOR_FOCUSED",
    "SLIDER_BASE_COLOR_FOCUSED",
    "SLIDER_BORDER_COLOR_PRESSED",
    "SLIDER_BASE_COLOR_PRESSED",
    "SLIDER_BORDER_COLOR_DISABLED",
    "SLIDER_BASE_COLOR_DISABLED",
    "SLIDERBAR_INNER_PADDING",
    "SLIDERBAR_BORDER_WIDTH",
    "SLIDERBAR_BORDER_COLOR_NORMAL",
    "SLIDERBAR_BASE_COLOR_NORMAL",
    "SLIDERBAR_BORDER_COLOR_FOCUSED",
    "SLIDERBAR_BASE_COLOR_FOCUSED",
    "SLIDERBAR_BORDER_COLOR_PRESSED",
    "SLIDERBAR_BASE_COLOR_PRESSED",
    "SLIDERBAR_BORDER_COLOR_DISABLED",
    "SLIDERBAR_BASE_COLOR_DISABLED",
    "PROGRESSBAR_INNER_PADDING",
    "PROGRESSBAR_BORDER_WIDTH",
    "PROGRESSBAR_BORDER_COLOR_NORMAL",
    "PROGRESSBAR_BASE_COLOR_NORMAL",
    "PROGRESSBAR_BORDER_COLOR_FOCUSED",
    "PROGRESSBAR_BASE_COLOR_FOCUSED",
    "PROGRESSBAR_BORDER_COLOR_PRESSED",
    "PROGRESSBAR_BASE_COLOR_PRESSED",
    "PROGRESSBAR_BORDER_COLOR_DISABLED",
    "PROGRESSBAR_BASE_COLOR_DISABLED",
    "SPINNER_BUTTON_PADDING",
    "SPINNER_BUTTONS_WIDTH",
    "SPINNER_BORDER_COLOR_NORMAL",
    "SPINNER_BASE_COLOR_NORMAL",
    "SPINNER_TEXT_COLOR_NORMAL",
    "SPINNER_BORDER_COLOR_FOCUSED",
    "SPINNER_BASE_COLOR_FOCUSED",
    "SPINNER_TEXT_COLOR_FOCUSED",
    "SPINNER_BORDER_COLOR_PRESSED",
    "SPINNER_BASE_COLOR_PRESSED",
    "SPINNER_TEXT_COLOR_PRESSED",
    "SPINNER_BORDER_COLOR_DISABLED",
    "SPINNER_BASE_COLOR_DISABLED",
    "SPINNER_TEXT_COLOR_DISABLED",
    "COMBOBOX_BORDER_WIDTH",
    "COMBOBOX_BUTTON_PADDING",
    "COMBOBOX_SELECTOR_WIDTH",
    "COMBOBOX_BORDER_COLOR_NORMAL",
    "COMBOBOX_BASE_COLOR_NORMAL",
    "COMBOBOX_TEXT_COLOR_NORMAL",
    "COMBOBOX_BORDER_COLOR_FOCUSED",
    "COMBOBOX_BASE_COLOR_FOCUSED",
    "COMBOBOX_TEXT_COLOR_FOCUSED",
    "COMBOBOX_BORDER_COLOR_PRESSED",
    "COMBOBOX_BASE_COLOR_PRESSED",
    "COMBOBOX_TEXT_COLOR_PRESSED",
    "COMBOBOX_BORDER_COLOR_DISABLED",
    "COMBOBOX_BASE_COLOR_DISABLED",
    "COMBOBOX_TEXT_COLOR_DISABLED",
    "CHECKBOX_BORDER_WIDTH",
    "CHECKBOX_INNER_PADDING",
    "CHECKBOX_BORDER_COLOR_NORMAL",
    "CHECKBOX_BASE_COLOR_NORMAL",
    "CHECKBOX_BORDER_COLOR_FOCUSED",
    "CHECKBOX_BASE_COLOR_FOCUSED",
    "CHECKBOX_BORDER_COLOR_PRESSED",
    "CHECKBOX_BASE_COLOR_PRESSED",
    "CHECKBOX_BORDER_COLOR_DISABLED",
    "CHECKBOX_BASE_COLOR_DISABLED",
    "TEXTBOX_BORDER_WIDTH",
    "TEXTBOX_BORDER_COLOR_NORMAL",
    "TEXTBOX_BASE_COLOR_NORMAL",
    "TEXTBOX_TEXT_COLOR_NORMAL",
    "TEXTBOX_BORDER_COLOR_FOCUSED",
    "TEXTBOX_BASE_COLOR_FOCUSED",
    "TEXTBOX_TEXT_COLOR_FOCUSED",
    "TEXTBOX_BORDER_COLOR_PRESSED",
    "TEXTBOX_BASE_COLOR_PRESSED",
    "TEXTBOX_TEXT_COLOR_PRESSED",
    "TEXTBOX_BORDER_COLOR_DISABLED",
    "TEXTBOX_BASE_COLOR_DISABLED",
    "TEXTBOX_TEXT_COLOR_DISABLED",
    "COLORPICKER_BARS_THICK",
    "COLORPICKER_BARS_PADDING",
    "COLORPICKER_BORDER_COLOR_NORMAL",
    "COLORPICKER_BASE_COLOR_NORMAL",
    "COLORPICKER_BORDER_COLOR_FOCUSED",
    "COLORPICKER_BASE_COLOR_FOCUSED",
    "COLORPICKER_BORDER_COLOR_PRESSED",
    "COLORPICKER_BASE_COLOR_PRESSED",
    "COLORPICKER_BORDER_COLOR_DISABLED",
    "COLORPICKER_BASE_COLOR_DISABLED",
    "LISTVIEW_ELEMENTS_HEIGHT",
    "LISTVIEW_ELEMENTS_PADDING",
    "LISTVIEW_BAR_WIDTH",       
    "LISTVIEW_BORDER_COLOR_NORMAL",
    "LISTVIEW_BASE_COLOR_NORMAL",
    "LISTVIEW_TEXT_COLOR_NORMAL",
    "LISTVIEW_BORDER_COLOR_FOCUSED",
    "LISTVIEW_BASE_COLOR_FOCUSED",
    "LISTVIEW_TEXT_COLOR_FOCUSED",
    "LISTVIEW_BORDER_COLOR_PRESSED",
    "LISTVIEW_BASE_COLOR_PRESSED",
    "LISTVIEW_TEXT_COLOR_PRESSED",
    "LISTVIEW_BORDER_COLOR_DISABLED",
    "LISTVIEW_BASE_COLOR_DISABLED",
    "LISTVIEW_TEXT_COLOR_DISABLED"
};

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void BtnLoadStyle(void);                                 // Button load style function
static void BtnSaveStyle(const char *defaultName, bool binary); // Button save style function

static void SaveStyleRGS(const char *fileName, bool binary);    // Save raygui style file (.rgs), text or binary

static int GetGuiStylePropertyIndex(int control, int property);
static Color ColorBox(Rectangle bounds, Color *colorPicker, Color color);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // Initialization
    //--------------------------------------------------------------------------------------
    const int screenWidth = 720;
    const int screenHeight = 640;
    
    //SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "rGuiStyler - raygui style editor");
    //SetWindowMinSize(700, 700);
    
    int dropsCount = 0;
    char **droppedFiles;
    
    int guiPosX = 336;
    int guiPosY = 36;
    bool saveColor = false;

    // Define gui controls rectangles
    Rectangle bounds[NUM_CONTROLS] = {
        (Rectangle){ 0 },                                           // DEFAULT
        (Rectangle){ guiPosX + 98, guiPosY + 18, 140, 10 },         // LABELBUTTON
        (Rectangle){ guiPosX + 184, guiPosY + 250, 180, 30 },       // BUTTON
        //(Rectangle){ guiPosX + 251, guiPosY + 5, 113, 32 },       // IMAGEBUTTON
        (Rectangle){ guiPosX + 20, guiPosY + 54, 60, 30 },          // TOGGLE
        //(Rectangle){ guiPosX + 98, guiPosY + 54, 65, 30 },         // TOGGLEGROUP
        (Rectangle){ guiPosX + 20, guiPosY + 104, 344, 20 },        // SLIDER
        (Rectangle){ guiPosX + 20, guiPosY + 134, 344, 20 },        // SLIDERBAR
        (Rectangle){ guiPosX + 20, guiPosY + 164, 320, 20 },        // PROGRESSBAR    
        (Rectangle){ guiPosX + 344, guiPosY + 164, 20, 20 },        // CHECKBOX
        (Rectangle){ guiPosX + 20, guiPosY + 204, 150, 30 },        // SPINNER
        (Rectangle){ guiPosX + 20, guiPosY + 250, 150, 30 },        // COMBOBOX
        (Rectangle){ guiPosX + 184, guiPosY + 204, 180, 30 },       // TEXTBOX
        (Rectangle){ 10, guiPosY + 3, 140, 563 },                   // LISTVIEW
        (Rectangle){ guiPosX + 20, guiPosY + 295, 240, 240 },       // COLORPICKER
    };

    // Get current directory
    // NOTE: Current working directory could not match current executable directory
    GetCurrentDir(currentPath, sizeof(currentPath));
    currentPath[strlen(currentPath)] = '\\';
    currentPath[strlen(currentPath) + 1] = '\0';      // Not really required
    
    //GuiLoadStyleImage("resources/rguistyle_default_dark.png");
    
    Texture2D texIcons = LoadTexture("resources/icons.png");
    
    // Keep a backup for style    
    memcpy(styleBackup, style, NUM_PROPERTIES*sizeof(int));
    
    SetTargetFPS(60);
    //------------------------------------------------------------

    // Gui controls data
    //-----------------------------------------------------------
    bool toggle = false;
    bool toggleValue = false;
    const char *toggleGuiText[4] = { "toggle", "group", "selection", "options" };

    float sliderValue = 50.0f;
    float sliderBarValue = 20.0f;
    float progressValue = 0.0f;
    
    bool checked = false;
    
    int spinnerValue = 28;

    int comboNum = 2;
    const char *comboText[2] = { "Style Text (.rgs)", "Style Binary (.rgs)" };
    int comboActive = 0;
    
    char guiText[32] =  "custom_style.rgs";
    
    Color colorPickerValue = RED;
    
    int currentSelectedControl = -1;
    int currentSelectedProperty = -1;
    int previousSelectedProperty = -1;
    int previousSelectedControl = -1;
    
    Color colorBoxValue[12];
    
    for (int i = 0; i < 12; i++) colorBoxValue[i] = GuiBackgroundColor();
    
    char colorHex[9] = "00000000";
 
    Vector3 colorHSV = { 0.0f, 0.0f, 0.0f };
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
        
        if ((previousSelectedControl != currentSelectedControl)) currentSelectedProperty = -1;
        
        if ((currentSelectedControl == 0) && (currentSelectedProperty != -1))
        {
            if ((previousSelectedProperty != currentSelectedProperty) || (previousSelectedControl != currentSelectedControl)) saveColor = false;
            
            if (!saveColor)
            {
                colorPickerValue = GetColor(style[GetGuiStylePropertyIndex(currentSelectedControl, currentSelectedProperty)]);
                saveColor = true;
            }

            style[GetGuiStylePropertyIndex(currentSelectedControl, currentSelectedProperty)] = ColorToInt(colorPickerValue);
            GuiUpdateStyleComplete();
        }
        else if ((currentSelectedControl != -1) && (currentSelectedProperty != -1))
        {
            if ((previousSelectedProperty != currentSelectedProperty) || (previousSelectedControl != currentSelectedControl)) saveColor = false;
            
            if (!saveColor)
            {
                colorPickerValue = GetColor(GuiGetStyleProperty(GetGuiStylePropertyIndex(currentSelectedControl, currentSelectedProperty)));
                saveColor = true;
            }

            GuiSetStyleProperty(GetGuiStylePropertyIndex(currentSelectedControl, currentSelectedProperty), ColorToInt(colorPickerValue));
        }
        
        
        previousSelectedProperty = currentSelectedProperty;
        previousSelectedControl = currentSelectedControl;
        
        // Update progress bar automatically
        progressValue += 0.0005f;
        if (progressValue > 1.0f) progressValue = 0.0f;

        // TODO: Support text editing on GuiTextBox()
        // NOTE: Editing mode should be detected (status = MOUSE_HOVER) and update colorPicker properly...
        sprintf(colorHex, "%02X%02X%02X%02X", colorPickerValue.r, colorPickerValue.g, colorPickerValue.b, colorPickerValue.a);
        
        colorHSV = ColorToHSV(colorPickerValue);
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(RAYWHITE);
            
            // Draw background rectangle
            DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), GuiBackgroundColor());
            
            // Draw info bar top
            DrawRectangle(0, 0, GetScreenWidth(), 24, GetColor(style[DEFAULT_BASE_COLOR_NORMAL]));
            DrawRectangle(0, 24, GetScreenWidth(), 1, GuiLinesColor());
            DrawText("CHOOSE CONTROL", 35, 8, style[DEFAULT_TEXT_SIZE], GetColor(style[DEFAULT_TEXT_COLOR_NORMAL]));
            DrawText(">      CHOOSE PROPERTY STYLE", 152, 8, style[DEFAULT_TEXT_SIZE], GetColor(style[DEFAULT_TEXT_COLOR_NORMAL]));
            DrawText(">                            STYLE VIEWER", guiPosX + 10, 8, style[DEFAULT_TEXT_SIZE], GetColor(style[DEFAULT_TEXT_COLOR_NORMAL]));
            
            // Draw status bar bottom
            DrawRectangle(0, GetScreenHeight() - 24, GetScreenWidth(), 24, GetColor(style[DEFAULT_BASE_COLOR_NORMAL]));
            DrawRectangle(0, GetScreenHeight() - 24, GetScreenWidth(), 1, GuiLinesColor());
            DrawText("rGuiStyler powered by raylib (github.com/raysan5/raylib) and raygui (github.com/raysan5/raygui)", 20, GetScreenHeight() - 16, style[DEFAULT_TEXT_SIZE], GetColor(style[DEFAULT_TEXT_COLOR_NORMAL]));
 
            // Draw Gui controls
            currentSelectedControl = GuiListView(bounds[LISTVIEW], guiControlText, NUM_CONTROLS, currentSelectedControl);
            
            if (currentSelectedControl < 0) GuiDisable();
            
            switch (currentSelectedControl)
            {
                case DEFAULT: currentSelectedProperty = GuiListView((Rectangle){ 156, guiPosY + 3, 180, 563 }, guiStylesTextC, NUM_STYLES_C, currentSelectedProperty); break;
                case LABELBUTTON: currentSelectedProperty = GuiListView((Rectangle){ 156, guiPosY + 3, 180, 563 }, guiStylesTextA, NUM_STYLES_A, currentSelectedProperty); break;
                case SLIDER: case SLIDERBAR: case PROGRESSBAR: case CHECKBOX:
                case COLORPICKER: currentSelectedProperty = GuiListView((Rectangle){ 156, guiPosY + 3, 180, 563 }, guiStylesTextB, NUM_STYLES_B, currentSelectedProperty); break;
                case BUTTON: case TOGGLE: case COMBOBOX: case TEXTBOX: case SPINNER: case LISTVIEW:
                default: currentSelectedProperty = GuiListView((Rectangle){ 156, guiPosY + 3, 180, 563 }, guiStylesTextC, NUM_STYLES_C - 2, currentSelectedProperty); break;
            }

            GuiEnable();
            
            checked = GuiCheckBox(bounds[CHECKBOX], checked);
            
            if (checked) GuiDisable();

            GuiLabel((Rectangle){ guiPosX + 23, guiPosY + 18, 50, 10 }, "rGuiStyler");

            if (GuiLabelButton(bounds[LABELBUTTON], "github.com/raysan5/raygui")) {}
            
            // Draw load style button
            if (GuiImageButtonEx((Rectangle){ guiPosX + 251, guiPosY + 5, 113, 32 }, texIcons , (Rectangle){ 0, 0, texIcons.width/3, texIcons.height/6 }, "Load Style")) {  currentSelectedProperty = -1; BtnLoadStyle(); }
            
            toggle = GuiToggleButton(bounds[TOGGLE], "toggle", toggle);
            
            toggleValue = GuiToggleGroup((Rectangle){ guiPosX + 98, guiPosY + 54, 65, 30 }, toggleGuiText, 4, toggleValue);
            
            sliderValue = GuiSlider(bounds[SLIDER], sliderValue, 0, 100);
            
            sliderBarValue = GuiSliderBar(bounds[SLIDERBAR], sliderBarValue, -10.0f, 40.0f);
            
            progressValue = GuiProgressBar(bounds[PROGRESSBAR], progressValue, 0.0f, 1.0f);
            
            spinnerValue = GuiSpinner(bounds[SPINNER], spinnerValue, 12, 31);
            
            comboActive = GuiComboBox(bounds[COMBOBOX], comboText, comboNum, comboActive);

            GuiTextBox(bounds[TEXTBOX], guiText, spinnerValue);
            
            colorPickerValue = GuiColorPicker(bounds[COLORPICKER], colorPickerValue);
            
            // Draw labels for GuiColorPicker information (RGBA)
            GuiGroupBox((Rectangle){ guiPosX + 303, guiPosY + 299, 60, 74 }, "RGBA");
            GuiLabel((Rectangle){ guiPosX + 313, guiPosY + 310, 60, 70 }, FormatText("R:   %03i", colorPickerValue.r));
            GuiLabel((Rectangle){ guiPosX + 313, guiPosY + 325, 60, 70 }, FormatText("G:   %03i", colorPickerValue.g));
            GuiLabel((Rectangle){ guiPosX + 313, guiPosY + 340, 60, 70 }, FormatText("B:   %03i", colorPickerValue.b));
            GuiLabel((Rectangle){ guiPosX + 313, guiPosY + 355, 60, 70 }, FormatText("A:   %03i", colorPickerValue.a));
            
            // Draw labels for GuiColorPicker information (HSV)
            GuiGroupBox((Rectangle){ guiPosX + 303, guiPosY + 385, 60, 60 }, "HSV");
            GuiLabel((Rectangle){ guiPosX + 313, guiPosY + 395, 60, 70 }, FormatText("H:  %.0f º", colorHSV.x));
            GuiLabel((Rectangle){ guiPosX + 313, guiPosY + 412, 60, 70 }, FormatText("S:  %.0f %%", colorHSV.y*100));
            GuiLabel((Rectangle){ guiPosX + 313, guiPosY + 429, 60, 70 }, FormatText("V:  %.0f %%", colorHSV.z*100));

            GuiTextBox((Rectangle){ guiPosX + 303, guiPosY + 545, 60, 20 }, colorHex, 8);
            
            for(int i = 0; i < 12; i++) colorBoxValue[i] = ColorBox((Rectangle){ guiPosX + 303 + 20*(i%3), guiPosY +  455 + 20*(i/3), 20, 20 }, &colorPickerValue, colorBoxValue[i]);
            
            DrawRectangleLinesEx((Rectangle){ guiPosX + 303, guiPosY +  455, 60, 80 }, 2, GetColor(style[DEFAULT_BORDER_COLOR_NORMAL]));
            
            GuiEnable();

            // Draw save style button
            if (GuiButton(bounds[BUTTON], "Save Style")) BtnSaveStyle(guiText, comboActive);
            
            // Draw selected control rectangles
            if (currentSelectedControl >= 0) DrawRectangleLinesEx((Rectangle){ bounds[currentSelectedControl].x - 2, bounds[currentSelectedControl].y -2, bounds[currentSelectedControl].width + 4, bounds[currentSelectedControl].height + 4 }, 1, RED);

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------    
    UnloadTexture(texIcons);

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
    const char *filters[] = { "*.rgs" };

    const char *fileName = tinyfd_openFileDialog("Load raygui style file", currentPath, 1, filters, "raygui Style Files (*.rgs)", 0);

    if (fileName != NULL) GuiLoadStyle(fileName);
}

// Button save style function
static void BtnSaveStyle(const char *defaultName, bool binary)
{
    char currrentPathFile[256];

    // Add sample file name to currentPath
    strcpy(currrentPathFile, currentPath);
    strcat(currrentPathFile, defaultName);

    // Save file dialog
    const char *filters[] = { "*.rgs" };
    const char *fileName = tinyfd_saveFileDialog("Save raygui style text file", currrentPathFile, 1, filters, "raygui Style Files (*.rgs)");

    if (fileName != NULL)
    {
        // Save style file (text or binary)
        SaveStyleRGS(fileName, binary);
        fileName = "";
    }
}

static int GetGuiStylePropertyIndex(int control, int property)
{
    int guiProp = -1;
    
    switch (control)
    {
        case DEFAULT: 
        {
            if (property == 12) guiProp = 0;
            else if (property == 13) guiProp = 1;
            else guiProp = DEFAULT_BORDER_COLOR_NORMAL + property; 
            
        } break;
        case LABELBUTTON: guiProp = LABEL_TEXT_COLOR_NORMAL + property; break;
        case BUTTON: guiProp = BUTTON_BORDER_COLOR_NORMAL + property; break;
        //case IMAGEBUTTON: guiProp = BUTTON_BORDER_COLOR_NORMAL + property; break;
        case TOGGLE: guiProp = TOGGLE_BORDER_COLOR_NORMAL + property; break; 
        //case TOGGLEGROUP: guiProp = TOGGLE_BORDER_COLOR_NORMAL + property; break;
        case SLIDER: guiProp = SLIDER_BORDER_COLOR_NORMAL + property; break;
        case SLIDERBAR: guiProp = SLIDERBAR_BORDER_COLOR_NORMAL + property; break;
        case PROGRESSBAR: guiProp = PROGRESSBAR_BORDER_COLOR_NORMAL + property; break;
        case CHECKBOX: guiProp = CHECKBOX_BORDER_COLOR_NORMAL + property; break;
        case SPINNER: guiProp = SPINNER_BORDER_COLOR_NORMAL + property; break;
        case COMBOBOX: guiProp = COMBOBOX_BORDER_COLOR_NORMAL + property; break;
        case TEXTBOX: guiProp = TEXTBOX_BORDER_COLOR_NORMAL + property; break;
        case LISTVIEW: guiProp = LISTVIEW_BORDER_COLOR_NORMAL + property; break;
        case COLORPICKER: guiProp = COLORPICKER_BORDER_COLOR_NORMAL + property; break;
        default: break;
    }

    //guiProp = LABEL_TEXT_COLOR_NORMAL + property/3;                   // type A
    //guiProp = SLIDER_BORDER_COLOR_NORMAL + property + property/2;     // type B
    //guiProp = TOGGLE_BORDER_COLOR_NORMAL + property;                  // type C

    return guiProp;
}

// Color box control to save color samples from color picker
// NOTE: It requires colorPicker pointer for updating in case of selection
static Color ColorBox(Rectangle bounds, Color *colorPicker, Color color)
{
    Vector2 mousePoint = GetMousePosition();
    
    // Update color box
    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) *colorPicker = (Color){ color.r, color.g, color.b, color.a };
        else if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) color = *colorPicker;
    }
    
    // Draw color box
    DrawRectangleRec(bounds, color);
    DrawRectangleLinesEx(bounds, 1, GetColor(style[DEFAULT_BORDER_COLOR_NORMAL]));
    
    return color;
}

// Save raygui style file (.rgs), text or binary
static void SaveStyleRGS(const char *fileName, bool binary)
{
    if (binary)
    {
        #define RGS_FILE_VERSION_BINARY   200
        
        FILE *rgsFile = fopen(fileName, "wb");
        
        if (rgsFile != NULL)
        {
            // Write some header info (12 bytes)
            // id: "RGS "       - 4 bytes
            // version: 200     - 2 bytes
            // NUM_PROPERTIES   - 2 bytes
            // reserved         - 4 bytes
            
            char signature[5] = "RGS ";
            short version = RGS_FILE_VERSION_BINARY;
            short numProperties = NUM_PROPERTIES;
            int reserved = 0;

            fwrite(signature, 1, 4, rgsFile);
            fwrite(&version, 1, sizeof(short), rgsFile);
            fwrite(&numProperties, 1, sizeof(short), rgsFile);
            fwrite(&reserved, 1, sizeof(int), rgsFile);
            
            short id = 0;
            
            for (int i = 0; i < NUM_PROPERTIES; i++)
            {
                if (styleBackup[i] != style[i])
                {
                    id = (short)i;
                    
                    fwrite(&id, 1, 2, rgsFile);
                    fwrite(&style[i], 1, sizeof(int), rgsFile);
                }
            }
            
            fclose(rgsFile);
        }
    }
    else
    {
        #define RGS_FILE_VERSION_TEXT   "2.0"
        
        int counter = 0;
        FILE *rgsFile = fopen(fileName, "wt");
        
        if (rgsFile != NULL)
        {
            for (int i = 0; i < NUM_PROPERTIES; i++) if (styleBackup[i] != style[i]) counter++;
            
            // Write some description comments
            fprintf(rgsFile, "#\n# rgst file (v%s) - raygui style text file generated using rGuiStyler\n#\n", RGS_FILE_VERSION_TEXT);
            fprintf(rgsFile, "# Total number of properties:     %i\n", NUM_PROPERTIES);
            fprintf(rgsFile, "# Number of properties changed:   %i\n", counter);
            fprintf(rgsFile, "# Required base default style:    %s\n#\n", "LIGHT");     // TODO: check base style

            for (int i = 0; i < NUM_PROPERTIES; i++)
            {
                if (styleBackup[i] != style[i]) fprintf(rgsFile, "%03i 0x%08x // %s\n", i, style[i], guiPropertyText[i]);      
            }

            fclose(rgsFile);
        }
    }
}
