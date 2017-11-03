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
#define COLOR_REC           BEIGE
#define NUM_COLOR_SAMPLES   10
#define ELEMENT_HEIGHT      38
#define STATUS_BAR_HEIGHT   25

#define NUM_ELEMENTS        11

// NOTE: Be extremely careful when defining: NUM_ELEMENTS, GuiElement, guiElementText, guiPropertyNum, guiPropertyType and guiPropertyPos
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
} GuiElement;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static char currentPath[256];       // Path to current working folder

const char *guiElementText[NUM_ELEMENTS] = { 
    "LABEL", 
    "BUTTON", 
    "TOGGLE", 
    "TOGGLEGROUP", 
    "SLIDER", 
    "SLIDERBAR", 
    "PROGRESSBAR", 
    "SPINNER", 
    "COMBOBOX", 
    "CHECKBOX", 
    "TEXTBOX" 
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
    const int screenWidth = 1280;
    const int screenHeight = 720;

    const int guiPropertyNum[NUM_ELEMENTS] = { 3, 11, 14, 1, 7, 6, 4, 14, 18, 8, 6 };

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
    int guiPropertyPos[NUM_ELEMENTS];

    for (int i = 0; i < NUM_ELEMENTS; i++)
    {
        guiPropertyPos[i] = aux;
        aux += guiPropertyNum[i];
    }
    
    //SetConfigFlags(FLAG_FULLSCREEN_MODE);
    InitWindow(screenWidth, screenHeight, "rGuiStyler - raygui style editor");
    
    int dropsCount = 0;
    char **droppedFiles;
    
    Rectangle guiElementRec[NUM_ELEMENTS];

    for (int i = 0; i < NUM_ELEMENTS; i++) guiElementRec[i] = (Rectangle){ 0, 0 + i*ELEMENT_HEIGHT, 140, ELEMENT_HEIGHT };

    int guiElementSelected = -1;
    int guiElementHover = -1;

    // Generate properties rectangles depending on guiPropertyNum[] and guiPropertyPos[]
    //-----------------------------------------------------------------------------------
    Rectangle propertyRec[NUM_PROPERTIES];

    for (int j = 0; j < NUM_ELEMENTS; j++)
    {
        for (int i = 0; i < guiPropertyNum[j]; i++)
        {
            if ((j + guiPropertyNum[j]) > 18)  propertyRec[guiPropertyPos[j] + i] = (Rectangle){ guiElementRec[0].width, guiElementRec[18 - guiPropertyNum[j]].y + i*ELEMENT_HEIGHT, 260, ELEMENT_HEIGHT };
            else propertyRec[guiPropertyPos[j] + i] = (Rectangle){ guiElementRec[0].width, guiElementRec[j].y + i*ELEMENT_HEIGHT, 260, ELEMENT_HEIGHT };
        }
    }

    int guiPropertySelected = -1;
    int guiPropertyHover = -1;
    //------------------------------------------------------------

    // GUI
    //-----------------------------------------------------------
    int guiPosX = 455;
    int guiPosY = 35;
    
    int guiHeight = 30;
    int guiWidth = 150;

    int deltaY = 60;
    
    int selectPosX = 401;
    //int selectPosY = 0;
    int selectWidth = screenWidth - 723;
    //int selectHeight = screenHeight;
    //------------------------------------------------------------
        
    // Color picker
    //-----------------------------------------------------------
    Vector2 colorPickerPos = { (float)screenWidth - 287, 20.0f };
    Vector2 cursorPickerPos = colorPickerPos;
    Color colorPickerValue = RED;
    Color colorSample[NUM_COLOR_SAMPLES];
    
    for (int i = 0; i < NUM_COLOR_SAMPLES; i++) colorSample[i] = RAYWHITE;   
    
    int rgbWidthLabel = 30;
    int rgbHeightLabel = 20;
    int rgbDelta = 6;
    int redValue = 0;
    int greenValue = 0;
    int blueValue = 0;
    int alphaValue = 255;
    
    // Color samples
    Rectangle sampleBoundsRec[NUM_COLOR_SAMPLES];
    int sampleHover = -1; 
    int sampleSelected = 0;
        
    for (int i = 0; i < NUM_COLOR_SAMPLES/2; i++) sampleBoundsRec[i] = (Rectangle) {colorPickerPos.x + 2*rgbWidthLabel + i*rgbWidthLabel + 3*rgbDelta + i*rgbDelta, colorPickerPos.y + 300 + 2*rgbDelta, rgbWidthLabel, rgbWidthLabel - 2};
    for (int i = NUM_COLOR_SAMPLES/2; i < NUM_COLOR_SAMPLES; i++) sampleBoundsRec[i] = (Rectangle) {colorPickerPos.x + 2*rgbWidthLabel + (i-5)*rgbWidthLabel + 3*rgbDelta + (i-5)*rgbDelta, colorPickerPos.y + 300 + 2*rgbDelta + rgbWidthLabel + 2, rgbWidthLabel, rgbWidthLabel - 2};
    //------------------------------------------------------------
    
    // Value size selection
    //-----------------------------------------------------------
    int sizeValueSelected = 10;
    //-----------------------------------------------------------

    Color bgColor = RAYWHITE;    
    
    bool toggle = false;
    bool toggleValue = false;
    char *toggleGuiText[3] = { "toggle", "group", "selection" };

    float sliderValue = 50.0f;
    float sliderBarValue = 50.0f;
    float progressValue = 0.0f;
    
    bool checked = false;
    
    int spinnerValue = 20;

    int comboNum = 5;
    char *comboText[5] = { "this", "is", "a" ,"combo", "box" };
    int comboActive = 0;
    
    char guiText[16] = "\0";

    bool isModified = false;
    //-----------------------------------------------------------
    
    // Get current directory
    // NOTE: Current working directory could not match current executable directory
    GetCurrentDir(currentPath, sizeof(currentPath));
    currentPath[strlen(currentPath)] = '\\';
    currentPath[strlen(currentPath) + 1] = '\0';      // Not really required

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
            LoadGuiStyle(droppedFiles[0]);
            ClearDroppedFiles();
        }
        
        // NOTE: We must verify that guiElementSelected and guiPropertySelected are valid

        // Check gui element selected
        for (int i = 0; i < NUM_ELEMENTS; i++)
        {
            if (CheckCollisionPointRec(GetMousePosition(), guiElementRec[i]))
            {
                guiElementSelected = i;
                guiElementHover = i;

                guiPropertySelected = -1;
                guiPropertyHover = -1;
                break;
            }
            else guiElementHover = -1;
        }
        
        // Check gui property selected
        if (guiElementSelected != -1)
        {
            for (int i = guiPropertyPos[guiElementSelected]; i < guiPropertyPos[guiElementSelected] + guiPropertyNum[guiElementSelected]; i++)
            {
                if (CheckCollisionPointRec(GetMousePosition(), propertyRec[i]))
                {
                    guiPropertyHover = i;
                    
                    // Show current value in color picker or spinner
                    if (guiPropertySelected == -1)
                    {
                        if (guiPropertyType[guiPropertyHover] == 0)         // Color type
                        {
                            // Update color picker color value
                            colorPickerValue = GetColor(GetStyleProperty(guiPropertyHover));
                            redValue = colorPickerValue.r;
                            greenValue = colorPickerValue.g;
                            blueValue = colorPickerValue.b;   
                            cursorPickerPos = (Vector2){screenWidth, screenHeight};                    
                        }
                                                                            // Value Type
                        else if (guiPropertyType[guiPropertyHover] == 1) sizeValueSelected = GetStyleProperty(guiPropertyHover);
                    }   
                    
                    // Check if gui property is clicked
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
                    {
                        if (guiPropertySelected == i) guiPropertySelected = -1;
                        else 
                        {
                            guiPropertySelected = i;
                            
                            if ((guiPropertyHover > -1) && (guiPropertyType[guiPropertyHover] == 0))
                            {
                                if (guiPropertySelected > -1) colorPickerValue = GetColor(GetStyleProperty(guiPropertySelected));
                                
                                redValue = colorPickerValue.r;
                                greenValue = colorPickerValue.g;
                                blueValue = colorPickerValue.b;  
                            }
                            else if (guiPropertySelected > -1) sizeValueSelected = GetStyleProperty(guiPropertySelected);
                        }
                    }
                    break;
                }
                else guiPropertyHover = -1;
            }
        }
        
        //if (guiElementSelected == PROGRESSBAR)
        {
            if (progressValue > 1.0f) progressValue = 0.0f;
            progressValue += 0.0005f;
        }
        
        if (guiPropertySelected != -1)
        {
            // Update style size value
            if (guiPropertyType[guiPropertySelected] == 1)
            {
                if (GetStyleProperty(guiPropertySelected) != sizeValueSelected)
                {
                    isModified = true;
                    SetStyleProperty(guiPropertySelected, sizeValueSelected);
                }
            }

            // Color samples
            /*
            if (CheckCollisionPointRec(GetMousePosition(), colorSelectedBoundsRec)) 
            {
                colorSelectedHover = true;

                if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) colorSample[sampleSelected] = colorPickerValue;
            }
            else colorSelectedHover = false;
            
            for (int i = 0; i < NUM_COLOR_SAMPLES; i++)
            {
                if (CheckCollisionPointRec(GetMousePosition(), sampleBoundsRec[i])) 
                {
                    sampleHover = i;                
                    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) 
                    {
                        sampleSelected = i;
                    }
                    if (IsMouseButtonDown(MOUSE_RIGHT_BUTTON)) 
                    {
                        sampleSelected = i;
                        colorPickerValue = colorSample[sampleSelected];
                        redValue = colorPickerValue.r;
                        greenValue = colorPickerValue.g;
                        blueValue = colorPickerValue.b;
                        alphaValue = colorPickerValue.a;
                    }               
                }            
            }
            */

            // Update style color value
            if (guiPropertySelected == DEFAULT_BACKGROUND_COLOR) bgColor = colorPickerValue;
            else if ((guiPropertySelected >= 0) && (guiPropertyType[guiPropertySelected] == 0))
            {
                bgColor = GetColor(GetStyleProperty(DEFAULT_BACKGROUND_COLOR));
                SetStyleProperty(guiPropertySelected, GetHexValue(colorPickerValue));
            }
        }
        //----------------------------------------------------------------------------------
        
        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
        
            ClearBackground(RAYWHITE); 
            
            // Show selected properties
            if (guiPropertySelected >= 0)  DrawText(FormatText("SELECTED PROPERTY: <%s>", guiPropertyName[guiPropertySelected]), 5, screenHeight - STATUS_BAR_HEIGHT + 8, FONT_SIZE , BLACK);
            else DrawText("SELECTED PROPERTY: <style property> / none", 5, screenHeight - STATUS_BAR_HEIGHT + 8, FONT_SIZE , BLACK);
            
            // Show if have been a modification
            if (!isModified) DrawText("SAVE STATUS: SAVED (filename.style)", screenWidth - 230 , screenHeight - STATUS_BAR_HEIGHT + 8, FONT_SIZE , BLACK);
            else DrawText("SAVE STATUS: NOT SAVED", screenWidth - 230, screenHeight - STATUS_BAR_HEIGHT + 8, FONT_SIZE , BLACK);
            
            DrawText(FormatText("ColorPicker hexadecimal value #%x", GetHexValue(colorPickerValue)), screenWidth/2 - 180 , screenHeight - STATUS_BAR_HEIGHT + 8, FONT_SIZE , BLACK); 
            
            // Background color
            DrawRectangle(0,0, guiElementRec[0].width, GetScreenHeight() - STATUS_BAR_HEIGHT, Fade(COLOR_REC, 0.3f));
            DrawRectangle(guiElementRec[0].width,0, propertyRec[0].width, GetScreenHeight() - STATUS_BAR_HEIGHT, Fade(COLOR_REC, 0.1f));
            
            // UI sample
            // Background
            DrawRectangle(400,0, screenWidth - 320 - 400, GetScreenHeight() - STATUS_BAR_HEIGHT, bgColor);
            
            GuiLabel((Rectangle){guiPosX, guiPosY, guiWidth, guiHeight}, "Label");
            //GuiLabelEx((Rectangle){guiPosX + deltaX, guiPosY, guiWidth, guiHeight}, "LabelEx", BLACK, BLACK, WHITE);
            
            if (GuiButton((Rectangle){guiPosX, guiPosY + deltaY, guiWidth, guiHeight}, "Button")) { }
            
            if (toggle) toggle = GuiToggleButton((Rectangle){guiPosX, guiPosY + 2*deltaY, guiWidth, guiHeight}, "Toggle ACTIVE", toggle);
            else toggle = GuiToggleButton((Rectangle){guiPosX, guiPosY + 2*deltaY, guiWidth, guiHeight}, "Toggle INACTIVE", toggle);
            
            toggleValue = GuiToggleGroup((Rectangle){guiPosX, guiPosY + 3*deltaY, guiWidth, guiHeight}, 3, toggleGuiText, toggleValue);
            
            sliderValue = GuiSlider((Rectangle){guiPosX, guiPosY + 4*deltaY, 3*guiWidth, guiHeight}, sliderValue, 0, 100);
            
            sliderBarValue = GuiSliderBar((Rectangle){guiPosX, guiPosY + 5*deltaY, 3*guiWidth, guiHeight}, sliderBarValue, 0, 100);
            
            progressValue = GuiProgressBar((Rectangle){guiPosX, guiPosY + 6*deltaY, 3*guiWidth, guiHeight}, progressValue, 0.0f, 1.0f);
            
            spinnerValue = GuiSpinner((Rectangle){guiPosX, guiPosY + 7*deltaY, guiWidth, guiHeight}, spinnerValue, 0, 100);
            
            comboActive = GuiComboBox((Rectangle){guiPosX, guiPosY + 8*deltaY, guiWidth, guiHeight}, comboNum, comboText, comboActive);

            checked = GuiCheckBox((Rectangle){guiPosX, guiPosY + 9*deltaY, guiWidth/5, guiHeight}, checked);
            
            GuiTextBox((Rectangle){guiPosX, guiPosY + 10*deltaY, guiWidth, guiHeight}, guiText, 16);

            if (guiElementSelected >= 0) DrawRectangleRec(guiElementRec[guiElementSelected], COLOR_REC);
            if (guiElementHover >= 0) DrawRectangleRec(guiElementRec[guiElementHover], Fade(COLOR_REC, 0.5f));
            if (guiElementSelected >= 0) DrawRectangleLines(guiElementRec[guiElementSelected].x, guiElementRec[guiElementSelected].y, guiElementRec[guiElementSelected].width, guiElementRec[guiElementSelected].height, Fade(COLOR_REC,0.5f));

            // Draw corresponding selected properties depending on guiElementSelected
            //----------------------------------------------------------------------------------------------
            if (guiElementSelected >= 0)
            {
                if (guiPropertyHover >= 0) DrawRectangleRec(propertyRec[guiPropertyHover], Fade(COLOR_REC,0.3f));
 
                if (guiPropertySelected >= 0) DrawRectangleRec(propertyRec[guiPropertySelected], Fade(COLOR_REC, 0.4f));
                if (guiPropertySelected >= 0) DrawRectangleLines(propertyRec[guiPropertySelected].x, propertyRec[guiPropertySelected].y, propertyRec[guiPropertySelected].width, propertyRec[guiPropertySelected].height, Fade(COLOR_REC, 0.8f));

                for (int i = guiPropertyPos[guiElementSelected]; i < guiPropertyPos[guiElementSelected] + guiPropertyNum[guiElementSelected]; i++)
                {
                    DrawText(guiPropertyName[i], propertyRec[i].x + propertyRec[i].width/2 - MeasureText(guiPropertyName[i], FONT_SIZE)/2, propertyRec[i].y + 15, FONT_SIZE, BLACK);
                }
            }
            
            //-----------------------------------------------------------------------------------------------

            // Draw Element selected rectangles
            switch (guiElementSelected)
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
            }
            
            // Draw color picker
            colorPickerValue = GuiColorPicker((Rectangle){ colorPickerPos.x, colorPickerPos.y, 240, 240 }, colorPickerValue);
            
            // Draw color samples
            /*
            for (int i = 0; i < NUM_COLOR_SAMPLES/2; i++) 
            {
                if (i == sampleSelected) DrawRectangle (colorPickerPos.x + 2*rgbWidthLabel + i*rgbWidthLabel + 3*rgbDelta + i*rgbDelta - 2, colorPickerPos.y - 2 + 300 + 2*rgbDelta, rgbWidthLabel + 4, rgbWidthLabel + 2, BLACK); 
                else if (i == sampleHover) DrawRectangle (colorPickerPos.x + 2*rgbWidthLabel + i*rgbWidthLabel + 3*rgbDelta + i*rgbDelta - 2, colorPickerPos.y - 2 + 300 + 2*rgbDelta, rgbWidthLabel + 4, rgbWidthLabel + 2, Fade(COLOR_REC, 0.8f));
                else DrawRectangle (colorPickerPos.x + 2*rgbWidthLabel + i*rgbWidthLabel + 3*rgbDelta + i*rgbDelta - 1, colorPickerPos.y - 1 + 300 + 2*rgbDelta, rgbWidthLabel + 2, rgbWidthLabel, Fade(COLOR_REC, 0.6f));
                DrawRectangle(colorPickerPos.x + 2*rgbWidthLabel + i*rgbWidthLabel + 3*rgbDelta + i*rgbDelta, colorPickerPos.y + 300 + 2*rgbDelta, rgbWidthLabel, rgbWidthLabel - 2, colorSample[i]);  
            }
            
            for (int i = NUM_COLOR_SAMPLES/2; i < NUM_COLOR_SAMPLES; i++) 
            {
                if (i == sampleSelected) DrawRectangle (colorPickerPos.x + 2*rgbWidthLabel + (i-5)*rgbWidthLabel + 3*rgbDelta + (i-5)*rgbDelta - 2, colorPickerPos.y - 2 + 300 + 2*rgbDelta + rgbWidthLabel + 2, rgbWidthLabel + 4, rgbWidthLabel + 2, BLACK); 
                else if (i == sampleHover) DrawRectangle (colorPickerPos.x + 2*rgbWidthLabel + (i-5)*rgbWidthLabel + 3*rgbDelta + (i-5)*rgbDelta - 2, colorPickerPos.y - 2 + 300 + 2*rgbDelta + rgbWidthLabel + 2, rgbWidthLabel + 4, rgbWidthLabel + 2, Fade(COLOR_REC, 0.8f));
                else DrawRectangle (colorPickerPos.x + 2*rgbWidthLabel + (i-5)*rgbWidthLabel + 3*rgbDelta + (i-5)*rgbDelta - 1, colorPickerPos.y - 1 + 300 + 2*rgbDelta + rgbWidthLabel + 2, rgbWidthLabel + 2, rgbWidthLabel, Fade(COLOR_REC, 0.6f));
                DrawRectangle(colorPickerPos.x + 2*rgbWidthLabel + (i-5)*rgbWidthLabel + 3*rgbDelta + (i-5)*rgbDelta, colorPickerPos.y + 300 + 2*rgbDelta + rgbWidthLabel + 2, rgbWidthLabel, rgbWidthLabel - 2, colorSample[i]);  
            }
            */

            // Draw Load and Save buttons
            if (GuiButton((Rectangle){ colorPickerPos.x, screenHeight - 3*rgbWidthLabel - rgbDelta - STATUS_BAR_HEIGHT, 260, rgbWidthLabel}, "Load Style")) BtnLoadStyle();
            if (GuiButton((Rectangle){ colorPickerPos.x, screenHeight - 2*rgbWidthLabel - STATUS_BAR_HEIGHT, 260, rgbWidthLabel}, "Save Style")) BtnSaveStyle();

            
            // Draw GUI Elements text
            for (int i = 0; i < NUM_ELEMENTS; i++) DrawText(guiElementText[i], guiElementRec[i].width/2 - MeasureText(guiElementText[i], FONT_SIZE)/2, 15 + i*ELEMENT_HEIGHT, FONT_SIZE, BLACK);

            // Draw Borders
            DrawLine(guiElementRec[0].width, 0, guiElementRec[0].width, screenHeight - STATUS_BAR_HEIGHT, COLOR_REC);
            DrawLine(400, 0, 400, screenHeight - STATUS_BAR_HEIGHT, COLOR_REC);
            DrawLine(401, 0, 401, screenHeight - STATUS_BAR_HEIGHT, Fade(COLOR_REC, 0.4f));
            DrawLine(screenWidth - 320, 0, screenWidth - 320, screenHeight - STATUS_BAR_HEIGHT, COLOR_REC);
            DrawLine(screenWidth - 321, 0, screenWidth - 321, screenHeight - STATUS_BAR_HEIGHT, Fade(COLOR_REC, 0.4f));
            
            // Draw info bar            
            DrawLine(0, screenHeight - STATUS_BAR_HEIGHT, screenWidth, screenHeight - STATUS_BAR_HEIGHT, Fade(COLOR_REC, 0.8f));
            DrawLine(0, screenHeight - STATUS_BAR_HEIGHT + 1, screenWidth, screenHeight - STATUS_BAR_HEIGHT + 1, Fade(COLOR_REC, 0.4f));
            DrawRectangle(0, screenHeight - STATUS_BAR_HEIGHT, screenWidth, STATUS_BAR_HEIGHT, Fade(COLOR_REC, 0.1f));
            
        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    // De-Initialization
    //--------------------------------------------------------------------------------------    
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

    const char *fileName = tinyfd_openFileDialog("Load raygui style file", currentPath, 1, filters, "raygui Style Files (*.rstyle)", 0);

    if (fileName != NULL) LoadGuiStyle(fileName);
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
    const char *fileName = tinyfd_saveFileDialog("Save raygui style file", currrentPathFile, 1, filters, "raygui Style Files (*.rstyle)");

    if (fileName != NULL)
    {
        SaveGuiStyle(fileName);
        fileName = "";
        //isModified = false;
    }
}
