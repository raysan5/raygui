/*******************************************************************************************
*
*   rGuiStyler - raygui Style Editor
*
*   Compile this program using:
*       gcc -o $(NAME_PART).exe $(FILE_NAME) external/tinyfiledialogs.c -I..\.. \ 
*       -lraylib -lglfw3 -lopengl32 -lgdi32 -lcomdlg32 -lole32 -std=c99 -Wall
*
*
*   This program has been created using raylib v1.5 (www.raylib.com)
*   raylib is licensed under an unmodified zlib/libpng license (View raylib.h for details)
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
#include "raygui.h"

#include "external/tinyfiledialogs.h"   // Open/Save file dialogs
#include "colorpicker.h"                // Color picker image data

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

#define NUM_ELEMENTS        13

// NOTE: Be extremely careful when defining: NUM_ELEMENTS, GuiElement, guiElementText, guiPropertyNum, guiPropertyType and guiPropertyPos
// All those variables must be coherent, one small mistake breaks the program (and it could take hours to find the error!)

typedef enum { GLOBAL, BACKGROUND, LABEL, BUTTON, TOGGLE, TOGGLEGROUP, SLIDER, SLIDERBAR, PROGRESSBAR, SPINNER, COMBOBOX, CHECKBOX, TEXTBOX } GuiElement;

const char *guiElementText[NUM_ELEMENTS] = { "GLOBAL", "BACKGROUND", "LABEL", "BUTTON", "TOGGLE", "TOGGLEGROUP", "SLIDER", "SLIDERBAR", "PROGRESSBAR", "SPINNER", "COMBOBOX", "CHECKBOX", "TEXTBOX" };

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static char currentPath[256];       // Path to current working folder

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

    const int guiPropertyNum[NUM_ELEMENTS] = { 5, 1, 3, 11, 14, 1, 7, 6, 4, 14, 18, 8, 6 };

    // Defines if the property to change is a Color or a value to update it accordingly
    // NOTE: 0 - Color, 1 - value
    const unsigned char guiPropertyType[NUM_PROPERTIES] = { 0, 0, 0, 1, 1, 0, 1, 1, 0, 1, 
                                                            1, 1, 0, 0, 0, 0, 0, 0, 0, 0,  
                                                            0, 1, 1, 0, 0, 0, 0, 0, 0, 0,   
                                                            0, 0, 0, 0, 0, 1, 1, 1, 0, 0,  
                                                            0, 0, 0, 0, 0, 0, 0, 0, 0, 0,  
                                                            0, 0, 1, 0, 0, 0, 0, 0, 0, 0,  
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
    
    int count = 0;
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
    
    int deltaX = 153;
    int deltaY = 60;
    
    int selectPosX = 401;
    //int selectPosY = 0;
    int selectWidth = screenWidth - 723;
    //int selectHeight = screenHeight;
    //------------------------------------------------------------
    
    // Color picker cursor texture generation
    //-----------------------------------------------------------
    int sizeCursor = 16; // size must be POT
    unsigned char *cursorData = (unsigned char *)malloc(sizeCursor*sizeCursor*2*sizeof(unsigned char));
    
    for (int w = 0; w < sizeCursor; w++)
    {
        for (int h = 0; h < sizeCursor; h++)
        {
            cursorData[w*sizeCursor*2 + 2*h] = 0;
            if ((sizeCursor%2) == 0)
            {
                if (((w == (sizeCursor/2 - 1)) || (w == sizeCursor/2)) && ((h == (sizeCursor/2 - 1)) || (h == sizeCursor/2)))
                {
                    cursorData[w*sizeCursor*2 + 2*h + 1] = 0;
                }
                else if ((w == (sizeCursor/2 - 1)) || (w == sizeCursor/2)) cursorData[w*sizeCursor*2 + 2*h + 1] = 255;
                else if ((h == (sizeCursor/2 - 1)) || (h == sizeCursor/2)) cursorData[w*sizeCursor*2 + 2*h + 1] = 255;
                else cursorData[w*sizeCursor*2 + 2*h + 1] = 0;
            }            
        }
    }
    
    Texture2D cursorTexture = LoadTextureEx(cursorData, sizeCursor, sizeCursor, UNCOMPRESSED_GRAY_ALPHA);    
    free(cursorData);
    //-----------------------------------------------------------
    
    // Color picker
    //-----------------------------------------------------------
    Vector2 colorPickerPos = { (float)screenWidth - 287, 20.0f };
    
    Image colorPickerImage;
    colorPickerImage.data = pickerData;
    colorPickerImage.width = 256;
    colorPickerImage.height = 256;
    colorPickerImage.mipmaps = 1;
    colorPickerImage.format = UNCOMPRESSED_R8G8B8;   

    Texture2D colorPickerTexture = LoadTextureFromImage(colorPickerImage);

    Vector2 cursorPickerPos = {colorPickerPos.x +(colorPickerTexture.width/2) - cursorTexture.width/2, colorPickerPos.y + (colorPickerTexture.height/2) - cursorTexture.height/2};
    
    Color *colorPickerPixels = GetImageData(colorPickerImage);
    
    Rectangle colorPickerBounds = (Rectangle){ (int)colorPickerPos.x, (int)colorPickerPos.y, colorPickerTexture.width, colorPickerTexture.height };

    Vector2 colorPosition;
    Color colorPickerValue;
    Color colorSample[NUM_COLOR_SAMPLES];
    
    for (int i = 0; i < NUM_COLOR_SAMPLES; i++) colorSample[i] = RAYWHITE;   
    
    int sampleDelta = 18;
    int rgbWidthLabel = 30;
    int rgbHeightLabel = 20;
    int rgbDelta = 6;
    int redValue = 0;
    int greenValue = 0;
    int blueValue = 0;
    int alphaValue = 255;
    
    // Color samples
    Rectangle colorSelectedBoundsRec = {colorPickerPos.x, colorPickerPos.y + colorPickerTexture.height + 2*rgbDelta, 2*rgbWidthLabel, 2*rgbWidthLabel};
    bool colorSelectedHover = false;
    
    Rectangle sampleBoundsRec[NUM_COLOR_SAMPLES];
    int sampleHover = -1; 
    int sampleSelected = 0;
        
    for (int i = 0; i < NUM_COLOR_SAMPLES/2; i++) sampleBoundsRec[i] = (Rectangle) {colorPickerPos.x + 2*rgbWidthLabel + i*rgbWidthLabel + 3*rgbDelta + i*rgbDelta, colorPickerPos.y + colorPickerTexture.height + 2*rgbDelta, rgbWidthLabel, rgbWidthLabel - 2};
    for (int i = NUM_COLOR_SAMPLES/2; i < NUM_COLOR_SAMPLES; i++) sampleBoundsRec[i] = (Rectangle) {colorPickerPos.x + 2*rgbWidthLabel + (i-5)*rgbWidthLabel + 3*rgbDelta + (i-5)*rgbDelta, colorPickerPos.y + colorPickerTexture.height + 2*rgbDelta + rgbWidthLabel + 2, rgbWidthLabel, rgbWidthLabel - 2};
    //------------------------------------------------------------
    
    // Value size selection
    //-----------------------------------------------------------
    int sizeValueSelected = 10;
    //-----------------------------------------------------------

    Color bgColor = RAYWHITE;    
    
    bool toggleActive = false;
    int toggleNum = 3;
    char *toggleGuiText[3] = { "toggle", "group", "selection" };

    float sliderValue = 50;
    float sliderBarValue = 50;

    float progressValue = 0;
    
    int spinnerValue = 20;

    int comboNum = 5;
    char *comboText[5] = { "this", "is", "a" ,"combo", "box" };
    int comboActive = 0;
    
    char *guiText = (char *)malloc(20);    
    for (int i = 0; i < 20; i++) guiText[i] = '\0';

    bool isModified = false;
    
    // Checked texture generation
    //-----------------------------------------------------------
    int size = 8;
    Color *pixels = (Color *)malloc(size*size*sizeof(Color)); 
    
    for (int y = 0; y < size; y++)
    {
        for (int x = 0; x < size; x++)
        {
            if (((x/(size/2) + y/(size/2)))%2 == 0) pixels[y*size + x] = RAYWHITE;
            else pixels[y*size + x] = Fade(LIGHTGRAY, 0.5f);
        }
    }
    
    Texture2D checkerTexture = LoadTextureEx(pixels, size, size, UNCOMPRESSED_R8G8B8A8);
    
    free(pixels);
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
            /*
            guiPropertySelected = -1;
            droppedFiles = GetDroppedFiles(&count);
            fileName = droppedFiles[0];
            
            //BtnLoadStyle();
            
            //ClearDroppedFiles();
            */
        }
        
        if (guiElementSelected == PROGRESSBAR)
        {
            if (progressValue > 1.0f) progressValue = 0;
            progressValue += 0.005f;
        }
        
        for (int i = 0; i < NUM_ELEMENTS; i++)
        {
            if (CheckCollisionPointRec(GetMousePosition(), guiElementRec[i]))
            {
                guiElementSelected = i;
                guiElementHover = i;

                guiPropertySelected = -1;
                guiPropertyHover = -1;
            }
        }

        if (!CheckCollisionPointRec(GetMousePosition(), guiElementRec[guiElementHover])) guiElementHover = -1;

        // Check for selected property
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
                
                // TODO: REVIEW: Can make the application crash...
                /*
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) 
                {
                    if (guiPropertySelected == i) guiPropertySelected = -1;
                    else 
                    {
                        guiPropertySelected = i;
                        
                        if (guiPropertyType[guiPropertyHover] == 0) 
                        {
                            colorPickerValue = GetColor(GetStyleProperty(guiPropertySelected));
                            redValue = colorPickerValue.r;
                            greenValue = colorPickerValue.g;
                            blueValue = colorPickerValue.b;  
                        }
                        else sizeValueSelected = GetStyleProperty(guiPropertySelected);
                    }
                }
                */

                break;
            }
            else guiPropertyHover = -1;
        }
       
        // Update style size value
        if ((guiPropertySelected >= 0) && (guiPropertyType[guiPropertySelected] == 1)) 
        {
            if (GetStyleProperty(guiPropertySelected) != sizeValueSelected)
            {
                isModified = true;
                SetStyleProperty(guiPropertySelected, sizeValueSelected);
            }
        }
        
        // Color picker logic
        if (CheckCollisionPointRec(GetMousePosition(), colorPickerBounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                if (!IsCursorHidden()) HideCursor();
                
                cursorPickerPos = (Vector2){ GetMousePosition().x - cursorTexture.width/2, GetMousePosition().y - cursorTexture.height/2};
                colorPosition = (Vector2){ GetMousePosition().x - colorPickerPos.x, GetMousePosition().y - colorPickerPos.y};
                
                colorPickerValue = colorPickerPixels[(int)colorPosition.x + (int)colorPosition.y*colorPickerTexture.width];
                redValue = colorPickerValue.r;
                greenValue = colorPickerValue.g;
                blueValue = colorPickerValue.b;
                alphaValue = colorPickerValue.a;
            }            
            
            if (IsMouseButtonUp(MOUSE_LEFT_BUTTON) && IsCursorHidden()) ShowCursor();
        }
        else
        {
            if (IsCursorHidden()) ShowCursor();
            
            colorPickerValue.r = redValue;
            colorPickerValue.g = greenValue;
            colorPickerValue.b = blueValue;
            colorPickerValue.a = alphaValue;
            
            if ((guiPropertySelected >= 0) && (guiPropertyType[guiPropertySelected] == 0))
            {                
                if (GetStyleProperty(guiPropertySelected) != GetHexValue(colorPickerValue))
                {
                    SetStyleProperty(guiPropertySelected, GetHexValue(colorPickerValue));
                    isModified = true;
                }
            }
        }
        
        // Color samples
        if (CheckCollisionPointRec(GetMousePosition(), colorSelectedBoundsRec)) 
        {
            colorSelectedHover = true;
            
            if (IsMouseButtonDown (MOUSE_RIGHT_BUTTON)) colorSample[sampleSelected] = colorPickerValue;
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

        // Update style color value --> PROGRAM CRASH!!!
        /*
        if (guiPropertySelected == BACKGROUND_COLOR) bgColor = colorPickerValue;
        else if ((guiPropertySelected != BACKGROUND_COLOR) && (guiPropertyType[guiPropertySelected] == 0)) 
        {
            bgColor = GetColor(GetStyleProperty(BACKGROUND_COLOR));
            SetStyleProperty(guiPropertySelected, GetHexValue(colorPickerValue));
        }
        */
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
            GuiLabelEx((Rectangle){guiPosX + deltaX, guiPosY, guiWidth, guiHeight}, "LabelEx", BLACK, BLACK, WHITE);
            
            if (GuiButton((Rectangle){guiPosX, guiPosY + deltaY, guiWidth, guiHeight}, "Button")) { }
            
            GuiToggleButton((Rectangle){guiPosX, guiPosY + 2*deltaY, guiWidth, guiHeight}, "Toggle inactive", false);
            GuiToggleButton((Rectangle){guiPosX + deltaX, guiPosY + 2*deltaY, guiWidth, guiHeight}, "Toggle active", true);
            
            toggleActive = GuiToggleGroup((Rectangle){guiPosX, guiPosY + 3*deltaY, guiWidth, guiHeight}, toggleNum, toggleGuiText, toggleActive);
            
            sliderValue = GuiSlider((Rectangle){guiPosX, guiPosY + 4*deltaY, 3*guiWidth, guiHeight}, sliderValue, 0, 100);
            
            sliderBarValue = GuiSliderBar((Rectangle){guiPosX, guiPosY + 5*deltaY, 3*guiWidth, guiHeight}, sliderBarValue, -50, 50);
            
            GuiProgressBar((Rectangle){guiPosX, guiPosY + 6*deltaY, 3*guiWidth, guiHeight}, progressValue);
            
            spinnerValue = GuiSpinner((Rectangle){guiPosX, guiPosY + 7*deltaY, guiWidth, guiHeight}, spinnerValue, 0, 100);
            
            comboActive = GuiComboBox((Rectangle){guiPosX, guiPosY + 8*deltaY, guiWidth, guiHeight}, comboNum, comboText, comboActive);
            
            GuiCheckBox((Rectangle){guiPosX, guiPosY + 9*deltaY, guiWidth/5, guiHeight}, "", false);
            GuiCheckBox((Rectangle){guiPosX + deltaX/4, guiPosY + 9*deltaY, guiWidth/5, guiHeight}, "", true);
            
            guiText = GuiTextBox((Rectangle){guiPosX, guiPosY + 10*deltaY, guiWidth, guiHeight}, guiText);

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
                case GLOBAL: DrawRectangleLines(selectPosX + 2, 2, selectWidth - 4, GetScreenHeight() - 4 - STATUS_BAR_HEIGHT, COLOR_REC); break;
                case BACKGROUND: DrawRectangleLines(selectPosX + 10, 10, selectWidth - 20, GetScreenHeight() - 20 - STATUS_BAR_HEIGHT, Fade(COLOR_REC, 0.8f)); break;
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
            
            // Value editor
            // -- Color picker
            DrawRectangleRec((Rectangle){colorPickerPos.x - 2, colorPickerPos.y - 2, colorPickerTexture.width + 4, colorPickerTexture.height + 4}, COLOR_REC);
            DrawTextureV(colorPickerTexture, colorPickerPos, WHITE);
            DrawTextureV(cursorTexture, cursorPickerPos, WHITE);
            
            //DrawRectangleV(GetMousePosition(), (Vector2){ 4, 4 }, RED);

            // -- Color value result
            if (colorSelectedHover) DrawRectangle(colorPickerPos.x - 3, colorPickerPos.y - 3 + colorPickerTexture.height + 2*rgbDelta, 2*rgbWidthLabel + 6, 2*rgbWidthLabel + 6, BLACK);
            else DrawRectangle(colorPickerPos.x - 2, colorPickerPos.y - 2 + colorPickerTexture.height + 2*rgbDelta, 2*rgbWidthLabel + 4, 2*rgbWidthLabel + 4, Fade(COLOR_REC, 0.8f));
            DrawRectangleRec (colorSelectedBoundsRec, WHITE);
            DrawRectangleRec (colorSelectedBoundsRec, colorPickerValue);
            
            // -- Color samples
            for (int i = 0; i < NUM_COLOR_SAMPLES/2; i++) 
            {
                if (i == sampleSelected) DrawRectangle (colorPickerPos.x + 2*rgbWidthLabel + i*rgbWidthLabel + 3*rgbDelta + i*rgbDelta - 2, colorPickerPos.y - 2 + colorPickerTexture.height + 2*rgbDelta, rgbWidthLabel + 4, rgbWidthLabel + 2, BLACK); 
                else if (i == sampleHover) DrawRectangle (colorPickerPos.x + 2*rgbWidthLabel + i*rgbWidthLabel + 3*rgbDelta + i*rgbDelta - 2, colorPickerPos.y - 2 + colorPickerTexture.height + 2*rgbDelta, rgbWidthLabel + 4, rgbWidthLabel + 2, Fade(COLOR_REC, 0.8f));
                else DrawRectangle (colorPickerPos.x + 2*rgbWidthLabel + i*rgbWidthLabel + 3*rgbDelta + i*rgbDelta - 1, colorPickerPos.y - 1 + colorPickerTexture.height + 2*rgbDelta, rgbWidthLabel + 2, rgbWidthLabel, Fade(COLOR_REC, 0.6f));
                DrawRectangle(colorPickerPos.x + 2*rgbWidthLabel + i*rgbWidthLabel + 3*rgbDelta + i*rgbDelta, colorPickerPos.y + colorPickerTexture.height + 2*rgbDelta, rgbWidthLabel, rgbWidthLabel - 2, colorSample[i]);  
            }
            
            for (int i = NUM_COLOR_SAMPLES/2; i < NUM_COLOR_SAMPLES; i++) 
            {
                if (i == sampleSelected) DrawRectangle (colorPickerPos.x + 2*rgbWidthLabel + (i-5)*rgbWidthLabel + 3*rgbDelta + (i-5)*rgbDelta - 2, colorPickerPos.y - 2 + colorPickerTexture.height + 2*rgbDelta + rgbWidthLabel + 2, rgbWidthLabel + 4, rgbWidthLabel + 2, BLACK); 
                else if (i == sampleHover) DrawRectangle (colorPickerPos.x + 2*rgbWidthLabel + (i-5)*rgbWidthLabel + 3*rgbDelta + (i-5)*rgbDelta - 2, colorPickerPos.y - 2 + colorPickerTexture.height + 2*rgbDelta + rgbWidthLabel + 2, rgbWidthLabel + 4, rgbWidthLabel + 2, Fade(COLOR_REC, 0.8f));
                else DrawRectangle (colorPickerPos.x + 2*rgbWidthLabel + (i-5)*rgbWidthLabel + 3*rgbDelta + (i-5)*rgbDelta - 1, colorPickerPos.y - 1 + colorPickerTexture.height + 2*rgbDelta + rgbWidthLabel + 2, rgbWidthLabel + 2, rgbWidthLabel, Fade(COLOR_REC, 0.6f));
                DrawRectangle(colorPickerPos.x + 2*rgbWidthLabel + (i-5)*rgbWidthLabel + 3*rgbDelta + (i-5)*rgbDelta, colorPickerPos.y + colorPickerTexture.height + 2*rgbDelta + rgbWidthLabel + 2, rgbWidthLabel, rgbWidthLabel - 2, colorSample[i]);  
            }
              
            // -- RGBA sliders
            GuiLabelEx((Rectangle){colorPickerPos.x, colorPickerPos.y + colorPickerTexture.height + 14*rgbDelta, rgbWidthLabel, rgbHeightLabel}, FormatText("%d", redValue), BLACK, COLOR_REC, RED);
            redValue = GuiSlider((Rectangle){colorPickerPos.x + rgbWidthLabel + rgbDelta, colorPickerPos.y + colorPickerTexture.height + 14*rgbDelta, colorPickerTexture.height - rgbWidthLabel - rgbDelta, rgbHeightLabel}, redValue, 0, 255);

            GuiLabelEx((Rectangle){colorPickerPos.x, colorPickerPos.y + colorPickerTexture.height + 15*rgbDelta + rgbHeightLabel, rgbWidthLabel, rgbHeightLabel}, FormatText("%d", greenValue), BLACK, COLOR_REC, GREEN);
            greenValue = GuiSlider((Rectangle){colorPickerPos.x + rgbWidthLabel + rgbDelta, colorPickerPos.y + colorPickerTexture.height + 15*rgbDelta + rgbHeightLabel, colorPickerTexture.height - rgbWidthLabel - rgbDelta, rgbHeightLabel}, greenValue, 0, 255);

            GuiLabelEx((Rectangle){colorPickerPos.x, colorPickerPos.y + colorPickerTexture.height + 16*rgbDelta + 2*rgbHeightLabel, rgbWidthLabel, rgbHeightLabel}, FormatText("%d", blueValue), BLACK, COLOR_REC, BLUE);
            blueValue = GuiSlider((Rectangle){colorPickerPos.x + rgbWidthLabel + rgbDelta, colorPickerPos.y + colorPickerTexture.height + 16*rgbDelta + 2*rgbHeightLabel, colorPickerTexture.height - rgbWidthLabel - rgbDelta, rgbHeightLabel}, blueValue, 0, 255);
            
            DrawTextureRec(checkerTexture, (Rectangle){0,0,rgbWidthLabel, rgbHeightLabel}, (Vector2){colorPickerPos.x, colorPickerPos.y + colorPickerTexture.height + 17*rgbDelta + 3*rgbHeightLabel}, WHITE);   
            DrawRectangle(colorPickerPos.x, colorPickerPos.y + colorPickerTexture.height + 17*rgbDelta + 3*rgbHeightLabel, rgbWidthLabel, rgbHeightLabel, Fade(colorPickerValue, (float)alphaValue/100));            
            alphaValue = GuiSlider((Rectangle){colorPickerPos.x + rgbWidthLabel + rgbDelta, colorPickerPos.y + colorPickerTexture.height + 17*rgbDelta + 3*rgbHeightLabel, colorPickerTexture.height - rgbWidthLabel - rgbDelta, rgbHeightLabel}, alphaValue, 0, 255);
            DrawRectangleLines(colorPickerPos.x,colorPickerPos.y + colorPickerTexture.height + 17*rgbDelta + 3*rgbHeightLabel,rgbWidthLabel, rgbHeightLabel, COLOR_REC);
            
            // -- VALUE Spinner 
            GuiLabel((Rectangle){ colorPickerPos.x + 2*rgbDelta, colorPickerPos.y + colorPickerTexture.height + 10*rgbHeightLabel, rgbWidthLabel, rgbWidthLabel}, "Value");
            sizeValueSelected = GuiSpinner((Rectangle){ colorPickerPos.x + 2*rgbWidthLabel, colorPickerPos.y + colorPickerTexture.height + 10*rgbHeightLabel, colorPickerTexture.height - 2*rgbWidthLabel, rgbWidthLabel}, sizeValueSelected, 0, 50);

            // -- Load and Save buttons
            if (GuiButton((Rectangle){ colorPickerPos.x, screenHeight - 3*rgbWidthLabel - rgbDelta - STATUS_BAR_HEIGHT, colorPickerTexture.width, rgbWidthLabel}, "Load Style")) BtnLoadStyle();
            if (GuiButton((Rectangle){ colorPickerPos.x, screenHeight - 2*rgbWidthLabel - STATUS_BAR_HEIGHT, colorPickerTexture.width, rgbWidthLabel}, "Save Style")) BtnSaveStyle();

            //GuiLabel((Rectangle){colorPickerPos.x, screenHeight - 2*rgbWidthLabel - STATUS_BAR_HEIGHT + rgbDelta, 2*rgbWidthLabel, rgbWidthLabel}, "File name");
            //fileName = GuiTextBox((Rectangle){colorPickerPos.x + 2*rgbWidthLabel, screenHeight - 2*rgbWidthLabel - STATUS_BAR_HEIGHT + rgbDelta, colorPickerTexture.width - 2*rgbWidthLabel, rgbWidthLabel}, fileName);
            
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
    // Unload all loaded data (textures, fonts, audio)
    //UnloadImage(colorPickerImage);
    UnloadTexture(colorPickerTexture);
    UnloadTexture(checkerTexture);
    UnloadTexture(cursorTexture);
    
    free(guiText);
    free(colorPickerPixels);
    
    ClearDroppedFiles();    // Clear internal buffers
    
    CloseWindow();        // Close window and OpenGL context
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

    if (fileName != NULL)
    {
        LoadGuiStyle(fileName);
        
        //guiPropertySelected = -1;
        fileName = "";
        //isModified = false;
    }
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
