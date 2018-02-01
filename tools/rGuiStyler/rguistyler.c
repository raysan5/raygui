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
#define NUM_STYLES        12

// NOTE: Be extremely careful when defining: NUM_CONTROLS, GuiElement, guiControlText, guiPropertyNum, guiPropertyType and guiPropertyPos
// All those variables must be coherent, one small mistake breaks the program (and it could take hours to find the error!)

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
/*
const char *guiListText[30] = {
    "This",
    "is",
    "afghj",
    "test",
    "aaaaaaa",
    "bbbbbbb",
    "ccccccc",
    "ddddddd",
    "eeeeeee",
    "fffffff",
    "ggggggg",
    "hhhhhhh",
    "iiiiiii",
    "jjjjjjj",
    "kkkkkkk",
    "lllllll",
    "mmmmmmm",
    "nnnnnnn",
    "ooooooo",
    "ppppppp",
    "qqqqqqq",
    "rrrrrrr",
    "sssssss",
    "ttttttt",
    "uuuuuuu",
    "vvvvvvv",
    "wwwwwww",
    "xxxxxxx",
    "yyyyyyy",
    "zzzzzzz"
};
*/
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
    
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screenWidth, screenHeight, "rGuiStyler - raygui style editor");
    SetWindowMinSize(700, 700);
    
    int dropsCount = 0;
    char **droppedFiles;
    
    
    
    const int guiPropertyNum[NUM_CONTROLS] = { 3, 11, 14, 1, 7, 6, 4, 14, 18, 8, 6 };

    // Defines if the property to change is a Color or a value to update it accordingly
    // NOTE: 0 - Color, 1 - value
    const unsigned char guiPropertyType[NUM_PROPERTIES] = { 1, 1, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 
                                                            0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 0, 
                                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 
                                                            0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 
                                                            1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 
                                                            0, 0, 1, 1, 0, 0, 0, 0, 1 };
    int aux = 0;
    int guiPropertyPos[NUM_CONTROLS];

    for (int i = 0; i < NUM_CONTROLS; i++)
    {
        guiPropertyPos[i] = aux;
        aux += guiPropertyNum[i];
    }
    
    Rectangle guiControlListRec[NUM_CONTROLS];

    for (int i = 0; i < NUM_CONTROLS; i++) guiControlListRec[i] = (Rectangle){ 0, 0 + i*CONTROL_LIST_HEIGHT, 140, CONTROL_LIST_HEIGHT };

    int guiControlSelected = -1;
    int guiControlHover = -1;
    

    Rectangle guiPropertyListRec[NUM_PROPERTIES];

    for (int j = 0; j < NUM_CONTROLS; j++)
    {
        for (int i = 0; i < guiPropertyNum[j]; i++)
        {
            if ((j + guiPropertyNum[j]) > 18)  guiPropertyListRec[guiPropertyPos[j] + i] = (Rectangle){ guiControlListRec[0].width, guiControlListRec[18 - guiPropertyNum[j]].y + i*CONTROL_LIST_HEIGHT, 260, CONTROL_LIST_HEIGHT };
            else guiPropertyListRec[guiPropertyPos[j] + i] = (Rectangle){ guiControlListRec[0].width, guiControlListRec[j].y + i*CONTROL_LIST_HEIGHT, 260, CONTROL_LIST_HEIGHT };
        }
    }

    int guiPropertySelected = -1;
    int guiPropertyHover = -1;
    //------------------------------------------------------------

    // Gui area variables
    //-----------------------------------------------------------
    int guiPosX = 455;
    int guiPosY = 35;
    
    int guiHeight = 30;
    int guiWidth = 150;

    int deltaY = 45;
    
    int selectPosX = 401;
    int selectWidth = screenWidth - 723;
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
    Texture2D texIcons = LoadTexture("resources/icons.png");
    Texture2D texLogo = LoadTexture("resources/logo128x128.png");
    
    int listViewActive = -1;
    int listViewStyleActive = -1;
    
    int controlsAnchorPos = 340;
    //-----------------------------------------------------------
    
    // Get current directory
    // NOTE: Current working directory could not match current executable directory
    GetCurrentDir(currentPath, sizeof(currentPath));
    currentPath[strlen(currentPath)] = '\\';
    currentPath[strlen(currentPath) + 1] = '\0';      // Not really required
    
    GuiLoadStyleImage("resources/rguistyle_default_light.png");

    SetTargetFPS(60);
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
            GuiLabel((Rectangle){20, GetScreenHeight() - 18, guiWidth/2, guiHeight}, FormatText("CURRENT SELECTION: %s_%s", guiControlText[listViewActive], guiStylesText[listViewStyleActive]));
            GuiLabel((Rectangle){controlsAnchorPos + 100, GetScreenHeight() - 18, guiWidth/2, guiHeight}, FormatText("SAVE STATUS: %s", guiText));
            
            DrawText("CHOOSE CONTROL", 25, 10, styleGeneric[DEFAULT_TEXT_SIZE], LIGHTGRAY);
            DrawText(">   CHOOSE PROPERTY STYLE", 140, 10, styleGeneric[DEFAULT_TEXT_SIZE], LIGHTGRAY);
            DrawText(">                            STYLE VIEWER", controlsAnchorPos, 10, styleGeneric[DEFAULT_TEXT_SIZE], LIGHTGRAY);
            
            //Draw texture of the logo
            DrawTexture(texLogo, 10, 540, WHITE);
            
            // Draw selected control rectangles
            /*switch (listViewActive)
            {
                case LABEL: DrawRectangleLines(selectPosX + 10, guiPosY - 10, selectWidth - 20, guiHeight + 20, Fade(COLOR_REC, 0.8f)); break;
                case BUTTON: DrawRectangleLines(selectPosX + 10, guiPosY + deltaY - 10, selectWidth - 20, guiHeight + 20, Fade(COLOR_REC, 0.8f)); break;
                case TOGGLE: 
                case TOGGLEGROUP: DrawRectangleLines(selectPosX + 10, guiPosY + (2 * deltaY) - 10, selectWidth - 20, guiHeight + 80, Fade(COLOR_REC, 0.8f)); break;
                case SLIDER: DrawRectangleLines(selectPosX + 10, guiPosY + (4 * deltaY) - 10, selectWidth - 20, guiHeight + 80, Fade(COLOR_REC, 0.8f)); break;
                case PROGRESSBAR: DrawRectangleLines(selectPosX + 10, guiPosY + (6 * deltaY) - 10, selectWidth - 20, guiHeight + 20, Fade(COLOR_REC, 0.8f)); break;
                case SPINNER: DrawRectangleLines(selectPosX + 10, guiPosY + (7 * deltaY) - 10, selectWidth - 20, guiHeight + 20, Fade(COLOR_REC, 0.8f)); break;
                case COMBOBOX: DrawRectangleLines(selectPosX + 10, guiPosY + (8 * deltaY) - 10, selectWidth - 20, guiHeight + 20, Fade(COLOR_REC, 0.8f)); break;
                case CHECKBOX: DrawRectangleLines(selectPosX + 10, guiPosY + (9 * deltaY) - 10, selectWidth - 20, guiHeight + 20, Fade(COLOR_REC, 0.8f)); break;
                case TEXTBOX: DrawRectangleLines(selectPosX + 10, guiPosY + (10 * deltaY) - 10, selectWidth - 20, guiHeight + 20, Fade(COLOR_REC, 0.8f)); break;
                default: break;
            }*/

            listViewActive = GuiListView((Rectangle){ 0, guiPosY, 140, 720/1.5f }, guiControlText, NUM_CONTROLS, listViewActive);
            
            listViewStyleActive = GuiListView((Rectangle){ 145, guiPosY, 140, 720/1.5f }, guiStylesText, NUM_STYLES, listViewStyleActive);

            GuiLabel((Rectangle){controlsAnchorPos, guiPosY + guiHeight/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, guiWidth/2, guiHeight}, "Label");

            //if (GuiButton((Rectangle){controlsAnchorPos, guiPosY + deltaY, guiWidth, guiHeight}, "Button")) { }
            
            if (GuiLabelButton((Rectangle){controlsAnchorPos + guiWidth/3, guiPosY + guiHeight/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, guiWidth, guiHeight}, "LabelButton")) {}
            
            if (GuiImageButtonEx((Rectangle){ controlsAnchorPos + guiWidth*2.3f - guiWidth/3, guiPosY, texIcons.width/3, texIcons.height/6 }, texIcons , (Rectangle){ 0, 0, texIcons.width/3, texIcons.height/6 })) { }
            
            if (toggle) toggle = GuiToggleButton((Rectangle){controlsAnchorPos, guiPosY + deltaY, guiWidth/2, guiHeight}, "Toggle ACT", toggle);
            else toggle = GuiToggleButton((Rectangle){controlsAnchorPos, guiPosY + deltaY, guiWidth/2, guiHeight}, "Toggle INE", toggle);
            
            toggleValue = GuiToggleGroup((Rectangle){controlsAnchorPos + guiWidth/1.5f, guiPosY + deltaY, guiWidth/2, guiHeight}, toggleGuiText, 3, toggleValue);
            
            sliderValue = GuiSlider((Rectangle){controlsAnchorPos, guiPosY + 2*deltaY, 2.2f*guiWidth, guiHeight}, sliderValue, 0, 100);
            
            sliderBarValue = GuiSliderBar((Rectangle){controlsAnchorPos, guiPosY + 3*deltaY, 2.2f*guiWidth, guiHeight}, sliderBarValue, -10.0f, 40.0f);
            
            progressValue = GuiProgressBar((Rectangle){controlsAnchorPos, guiPosY + 4*deltaY, 1.9f*guiWidth, guiHeight}, progressValue, 0.0f, 1.0f);
            
            spinnerValue = GuiSpinner((Rectangle){controlsAnchorPos, guiPosY + 5*deltaY, guiWidth, guiHeight}, spinnerValue, 0, 100);
            
            comboActive = GuiComboBox((Rectangle){controlsAnchorPos + guiWidth*1.2f, guiPosY + 5*deltaY, guiWidth, guiHeight}, comboText, comboNum, comboActive);

            checked = GuiCheckBox((Rectangle){controlsAnchorPos + guiWidth*2, guiPosY + 4*deltaY, guiWidth/5, guiHeight}, checked);
            
            GuiTextBox((Rectangle){controlsAnchorPos, guiPosY + 6*deltaY, guiWidth, guiHeight}, guiText, 16);
            
            colorPickerValue = GuiColorPicker((Rectangle){ controlsAnchorPos, guiPosY + 7*deltaY, 240, 240 }, colorPickerValue);

            //Draw labels for GuiColorPicker information
            GuiLabel((Rectangle){controlsAnchorPos + guiWidth*1.9f, guiPosY + 7*deltaY + guiHeight/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, guiWidth/2, guiHeight}, FormatText("R: %i", colorPickerValue.r));
            GuiLabel((Rectangle){controlsAnchorPos + guiWidth*1.9f, guiPosY + 7.4f*deltaY + guiHeight/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, guiWidth/2, guiHeight}, FormatText("G: %i", colorPickerValue.g));
            GuiLabel((Rectangle){controlsAnchorPos + guiWidth*1.9f, guiPosY + 7.8f*deltaY + guiHeight/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, guiWidth/2, guiHeight}, FormatText("B: %i", colorPickerValue.b));
            GuiLabel((Rectangle){controlsAnchorPos + guiWidth*1.9f, guiPosY + 8.2f*deltaY + guiHeight/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, guiWidth/2, guiHeight}, FormatText("A: %i", colorPickerValue.a));
            GuiLabel((Rectangle){controlsAnchorPos + guiWidth*1.9f, guiPosY + 12.5f*deltaY + guiHeight/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, guiWidth/2, guiHeight}, FormatText("#%x", GetHexValue(colorPickerValue)));
            
            // Draw Load and Save buttons
            if (GuiButton((Rectangle){ controlsAnchorPos + guiWidth*1.2f - guiWidth/3, guiPosY, guiWidth, guiHeight }, "Load Style")) BtnLoadStyle();
            if (GuiButton((Rectangle){ controlsAnchorPos + guiWidth*1.2f, guiPosY + 6*deltaY, guiWidth, guiHeight }, "Save Style")) BtnSaveStyle();

        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------    
    UnloadTexture(texIcons);
    UnloadTexture(texLogo);
    ClearDroppedFiles();    // Clear internal buffers
    
    CloseWindow();          // Close window and OpenGL context
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
