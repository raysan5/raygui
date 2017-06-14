/*******************************************************************************************
*
*   raygui v1.1 - IMGUI (Immedite Mode GUI) library for raylib (www.raylib.com)
*
*   DESCRIPTION:
*
*   raygui is a library for creating simple IMGUI interfaces using raylib.
*   It provides a set of basic controls:
*
*       - Label
*       - Button
*       - ToggleButton
*       - ToggleGroup
*       - ComboBox
*       - CheckBox
*       - Slider
*       - SliderBar
*       - ProgressBar
*       - Spinner
*       - TextBox
*
*   It also provides a set of functions for styling the controls based on its properties (size, color).
*
*   CONFIGURATION:
*
*   #define RAYGUI_IMPLEMENTATION
*       Generates the implementation of the library into the included file.
*       If not defined, the library is in header only mode and can be included in other headers
*       or source files without problems. But only ONE file should hold the implementation.
*
*   #define RAYGUI_STATIC (defined by default)
*       The generated implementation will stay private inside implementation file and all
*       internal symbols and functions will only be visible inside that file.
*
*   #define RAYGUI_STANDALONE
*       Avoid raylib.h header inclusion in this file. Data types defined on raylib are defined
*       internally in the library and input management and drawing functions must be provided by
*       the user (check library implementation for further details).
*
*   #define RAYGUI_NO_STYLE_SAVE_LOAD
*       Avoid including style customization and save/load functions, useful when just using default
*       raygui style. Simplyfies library functionality and code size.
*
*   #define RAYGUI_MALLOC()
*   #define RAYGUI_FREE()
*       You can define your own malloc/free implementation replacing stdlib.h malloc()/free() functions.
*       Otherwise it will include stdlib.h and use the C standard library malloc()/free() function.
*
*   LIMITATIONS:
*       Some controls missing, like panels.
*
*   VERSIONS HISTORY:
*       1.1 (01-Jun-2017) Complete review of the library
*       1.0 (07-Jun-2016) Converted to header-only by Ramon Santamaria.
*       0.9 (07-Mar-2016) Reviewed and tested by Albert Martos, Ian Eito, Sergio Martinez and Ramon Santamaria.
*       0.8 (27-Aug-2015) Initial release. Implemented by Kevin Gato, Daniel Nicolás and Ramon Santamaria.
*
*   CONTRIBUTORS:
*       Kevin Gato:         Initial implementation of basic components (2013)
*       Daniel Nicolas:     Initial implementation of basic components (2013)
*       Albert Martos:      Review and testing of the library (2015)
*       Ian Eito:           Review and testing of the library (2015)
*       Sergio Martinez:    Review and testing of the library (2015)
*       Ramon Santamaria:   Supervision, review, update and maintenance
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2015-2017 Ramon Santamaria (@raysan5)
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

#ifndef RAYGUI_H
#define RAYGUI_H

#if !defined(RAYGUI_STANDALONE)
    #include "raylib.h"
#endif

#define RAYGUI_STATIC
#ifdef RAYGUI_STATIC
    #define RAYGUIDEF static            // Functions just visible to module including this file
#else
    #ifdef __cplusplus
        #define RAYGUIDEF extern "C"    // Functions visible from other files (no name mangling of functions in C++)
    #else
        #define RAYGUIDEF extern        // Functions visible from other files
    #endif
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define NUM_PROPERTIES       100

//----------------------------------------------------------------------------------
// Types and Structures Definition
// NOTE: Some types are required for RAYGUI_STANDALONE usage
//----------------------------------------------------------------------------------
#if defined(RAYGUI_STANDALONE)
    #ifndef __cplusplus
    // Boolean type
        #ifndef true
            typedef enum { false, true } bool;
        #endif
    #endif

    // Vector2 type
    typedef struct Vector2 {
        float x;
        float y;
    } Vector2;

    // Color type, RGBA (32bit)
    typedef struct Color {
        unsigned char r;
        unsigned char g;
        unsigned char b;
        unsigned char a;
    } Color;

    // Rectangle type
    typedef struct Rectangle {
        int x;
        int y;
        int width;
        int height;
    } Rectangle;
#endif

#define DEFAULT_BORDER_COLOR_NORMAL     0x828282ff
#define DEFAULT_BASE_COLOR_NORMAL       0xc8c8c8ff
#define DEFAULT_TEXT_COLOR_NORMAL       0x686868ff
#define DEFAULT_BORDER_COLOR_FOCUSED    0x7bb0d6ff
#define DEFAULT_BASE_COLOR_FOCUSED      0xc9effeff
#define DEFAULT_TEXT_COLOR_FOCUSED      0x6c9bbcff
#define DEFAULT_BORDER_COLOR_PRESSED    0x048cc7ff
#define DEFAULT_BASE_COLOR_PRESSED      0x7ceafeff
#define DEFAULT_TEXT_COLOR_PRESSED      0x0480b5ff
#define DEFAULT_BORDER_COLOR_DISABLED   0xb1b1b1ff
#define DEFAULT_BASE_COLOR_DISABLED     0xfafafaff
#define DEFAULT_TEXT_COLOR_DISABLED     0xc8c8c8ff

// Gui properties enumeration
typedef enum GuiProperty {
    DEFAULT_BACKGROUND_COLOR = 0,
    DEFAULT_LINES_COLOR,
    DEFAULT_BORDER_COLOR,
    DEFAULT_BORDER_WIDTH,
    DEFAULT_BASE_COLOR,
    DEFAULT_TEXT_COLOR,
    DEFAULT_TEXT_FONT,
    DEFAULT_TEXT_SIZE,
    // Label
    LABEL_BORDER_COLOR_NORMAL,
    LABEL_BASE_COLOR_NORMAL,
    LABEL_TEXT_COLOR_NORMAL,
    // Button
    BUTTON_BORDER_WIDTH,
    BUTTON_BORDER_COLOR_NORMAL,
    BUTTON_BASE_COLOR_NORMAL,
    BUTTON_TEXT_COLOR_NORMAL,
    BUTTON_BORDER_COLOR_FOCUSED,
    BUTTON_BASE_COLOR_FOCUSED,
    BUTTON_TEXT_COLOR_FOCUSED,
    BUTTON_BORDER_COLOR_PRESSED,
    BUTTON_BASE_COLOR_PRESSED,
    BUTTON_TEXT_COLOR_PRESSED,
    // Toggle
    TOGGLE_BORDER_WIDTH,
    TOGGLE_BORDER_COLOR_NORMAL,
    TOGGLE_BASE_COLOR_NORMAL,
    TOGGLE_TEXT_COLOR_NORMAL,
    TOGGLE_BORDER_COLOR_FOCUSED,
    TOGGLE_BASE_COLOR_FOCUSED,
    TOGGLE_TEXT_COLOR_FOCUSED,
    TOGGLE_BORDER_COLOR_PRESSED,
    TOGGLE_BASE_COLOR_PRESSED,
    TOGGLE_TEXT_COLOR_PRESSED,
    TOGGLEGROUP_PADDING,
    // SliderBar
    SLIDERBAR_BORDER_WIDTH,
    SLIDERBAR_BORDER_COLOR_NORMAL,
    SLIDERBAR_BASE_COLOR_NORMAL,
    SLIDERBAR_BORDER_COLOR_FOCUSED,
    SLIDERBAR_BASE_COLOR_FOCUSED,
    SLIDERBAR_BORDER_COLOR_PRESSED,
    SLIDERBAR_BASE_COLOR_PRESSED,
    // ProgressBar
    PROGRESSBAR_BORDER_WIDTH,
    PROGRESSBAR_BORDER_COLOR_NORMAL,
    PROGRESSBAR_BASE_COLOR_NORMAL,
    PROGRESSBAR_FRONT_COLOR_NORMAL,
    // Spinner
    SPINNER_BUTTON_PADDING,
    SPINNER_BORDER_COLOR_NORMAL,
    SPINNER_BASE_COLOR_NORMAL,
    SPINNER_TEXT_COLOR_NORMAL,
    SPINNER_BORDER_COLOR_FOCUSED,
    SPINNER_BASE_COLOR_FOCUSED,
    SPINNER_TEXT_COLOR_FOCUSED,
    // ComboBox
    COMBOBOX_BORDER_WIDTH,
    COMBOBOX_BUTTON_PADDING,
    COMBOBOX_BORDER_COLOR_NORMAL,
    COMBOBOX_BASE_COLOR_NORMAL,
    COMBOBOX_TEXT_COLOR_NORMAL,
    COMBOBOX_BORDER_COLOR_FOCUSED,
    COMBOBOX_BASE_COLOR_FOCUSED,
    COMBOBOX_TEXT_COLOR_FOCUSED,
    COMBOBOX_BORDER_COLOR_PRESSED,
    COMBOBOX_BASE_COLOR_PRESSED,
    COMBOBOX_TEXT_COLOR_PRESSED,
    // CheckBox
    CHECKBOX_BORDER_WIDTH,
    CHECKBOX_BORDER_COLOR_NORMAL,
    CHECKBOX_BASE_COLOR_NORMAL,
    CHECKBOX_BORDER_COLOR_FOCUSED,
    CHECKBOX_BASE_COLOR_FOCUSED,
    CHECKBOX_BORDER_COLOR_PRESSED,
    CHECKBOX_BASE_COLOR_PRESSED,
    // TextBox
    TEXTBOX_BORDER_WIDTH,
    TEXTBOX_BORDER_COLOR_NORMAL,
    TEXTBOX_BASE_COLOR_NORMAL,
    TEXTBOX_TEXT_COLOR_NORMAL,
    //TEXTBOX_CURSOR_COLOR_NORMAL,
    TEXTBOX_BORDER_COLOR_FOCUSED,
    TEXTBOX_BASE_COLOR_FOCUSED,
    TEXTBOX_TEXT_COLOR_FOCUSED,
    TEXTBOX_BORDER_COLOR_PRESSED,
    TEXTBOX_BASE_COLOR_PRESSED,
    TEXTBOX_TEXT_COLOR_PRESSED,
} GuiProperty;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
RAYGUIDEF Color GuiBackgroundColor(void);                                                                     // Get background color
RAYGUIDEF Color GuiLinesColor(void);                                                                          // Get lines color
RAYGUIDEF void GuiLabel(Rectangle bounds, const char *text);                                                  // Label element, show text
RAYGUIDEF void GuiLabelEx(Rectangle bounds, const char *text, Color textColor, Color border, Color inner);    // Label element extended, configurable colors
RAYGUIDEF bool GuiButton(Rectangle bounds, const char *text);                                                 // Button element, returns true when clicked
RAYGUIDEF bool GuiToggleButton(Rectangle bounds, const char *text, bool toggle);                              // Toggle Button element, returns true when active
RAYGUIDEF int GuiToggleGroup(Rectangle bounds, int toggleNum, char **toggleText, int toggleActive);           // Toggle Group element, returns toggled button index
RAYGUIDEF int GuiComboBox(Rectangle bounds, int comboNum, char **comboText, int comboActive);                 // Combo Box element, returns selected item index
RAYGUIDEF bool GuiCheckBox(Rectangle bounds, const char *text, bool checked);                                 // Check Box element, returns true when active
RAYGUIDEF float GuiSlider(Rectangle bounds, float value, float minValue, float maxValue);                     // Slider element, returns selected value
RAYGUIDEF float GuiSliderBar(Rectangle bounds, float value, float minValue, float maxValue);                  // Slider Bar element, returns selected value
RAYGUIDEF void GuiProgressBar(Rectangle bounds, float value);                                                 // Progress Bar element, shows current progress value
RAYGUIDEF int GuiSpinner(Rectangle bounds, int value, int minValue, int maxValue);                            // Spinner element, returns selected value
RAYGUIDEF char *GuiTextBox(Rectangle bounds, char *text);                                                     // Text Box element, returns input text

//RAYGUIDEF void UpdateMouse(bool isMouseDown, bool isMouseUp, Vector2 mousePosition);
//RAYGUIDEF void UpdateKeys(int keyPressed);

#if !defined(RAYGUI_NO_STYLE_SAVE_LOAD)
RAYGUIDEF void SaveGuiStyle(const char *fileName);                        // Save GUI style file
RAYGUIDEF void LoadGuiStyle(const char *fileName);                        // Load GUI style file
RAYGUIDEF void SetStyleProperty(int guiProperty, int value);              // Set one style property
RAYGUIDEF int GetStyleProperty(int guiProperty);                          // Get one style property
#endif

#endif // RAYGUI_H


/***********************************************************************************
*
*   RAYGUI IMPLEMENTATION
*
************************************************************************************/

#if defined(RAYGUI_IMPLEMENTATION)

#include <stdio.h>          // Required for: FILE, fopen(), fclose(), fprintf(), feof(), fscanf()
                            // NOTE: Those functions are only used in SaveGuiStyle() and LoadGuiStyle()

// Check if custom malloc/free functions defined, if not, using standard ones
#if !defined(RAYGUI_MALLOC) && !defined(RAYGUI_NO_STYLE_SAVE_LOAD)
    #include <stdlib.h>     // Required for: malloc(), free() [Used only on LoadGuiStyle()]

    #define RAYGUI_MALLOC(size)  malloc(size)
    #define RAYGUI_FREE(ptr)     free(ptr)
#endif

#if !defined(RAYGUI_NO_STYLE_SAVE_LOAD)
    #include <string.h>     // Required for: strcmp() [Used only on LoadGuiStyle()]
#endif

#include <stdarg.h>         // Required for: va_list, va_start(), vfprintf(), va_end()

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#if defined(RAYGUI_STANDALONE)
    #define KEY_LEFT            263
    #define KEY_RIGHT           262
    #define MOUSE_LEFT_BUTTON     0
#endif

#if !defined(RAYGUI_STYLE_DEFAULT_LIGHT) && !defined(RAYGUI_STYLE_DEFAULT_DARK)
    #define RAYGUI_STYLE_DEFAULT_LIGHT
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// GUI controls state
typedef enum { DISABLED, NORMAL, FOCUSED, PRESSED } ControlState;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//static Vector2 panelOffset = { 0, 0 };
//static ControlState state = NORMAL;

#if defined(RAYGUI_STYLE_DEFAULT_LIGHT)
// Current GUI style (default light)
static int style[NUM_PROPERTIES] = {
    0xf5f5f5ff,                         // DEFAULT_BACKGROUND_COLOR
    0x90abb5ff,                         // DEFAULT_LINES_COLOR,
    DEFAULT_BORDER_COLOR_NORMAL,        // DEFAULT_BORDER_COLOR,
    1,                                  // DEFAULT_BORDER_WIDTH,
    DEFAULT_BASE_COLOR_NORMAL,          // DEFAULT_BASE_COLOR,
    DEFAULT_TEXT_COLOR_NORMAL,          // DEFAULT_TEXT_COLOR,
    0,                                  // DEFAULT_TEXT_FONT,
    10,                                 // DEFAULT_TEXT_SIZE,
    DEFAULT_BORDER_COLOR_NORMAL,        // LABEL_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,          // LABEL_BASE_COLOR_NORMAL,
    DEFAULT_TEXT_COLOR_NORMAL,          // LABEL_TEXT_COLOR_NORMAL,
    2,                                  // BUTTON_BORDER_WIDTH,
    DEFAULT_BORDER_COLOR_NORMAL,        // BUTTON_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,          // BUTTON_BASE_COLOR_NORMAL,
    DEFAULT_TEXT_COLOR_NORMAL,          // BUTTON_TEXT_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // BUTTON_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,         // BUTTON_BASE_COLOR_FOCUSED,
    DEFAULT_TEXT_COLOR_FOCUSED,         // BUTTON_TEXT_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,       // BUTTON_BORDER_COLOR_PRESSED,
    DEFAULT_BASE_COLOR_PRESSED,         // BUTTON_BASE_COLOR_PRESSED,
    DEFAULT_TEXT_COLOR_PRESSED,         // BUTTON_TEXT_COLOR_PRESSED,
    1,                                  // TOGGLE_BORDER_WIDTH,
    DEFAULT_BORDER_COLOR_NORMAL,        // TOGGLE_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,          // TOGGLE_BASE_COLOR_NORMAL,
    DEFAULT_TEXT_COLOR_NORMAL,          // TOGGLE_TEXT_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // TOGGLE_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,         // TOGGLE_BASE_COLOR_FOCUSED,
    DEFAULT_TEXT_COLOR_FOCUSED,         // TOGGLE_TEXT_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,       // TOGGLE_BORDER_COLOR_PRESSED,
    DEFAULT_BASE_COLOR_PRESSED,         // TOGGLE_BASE_COLOR_PRESSED,
    DEFAULT_TEXT_COLOR_PRESSED,         // TOGGLE_TEXT_COLOR_PRESSED,
    2,                                  // TOGGLEGROUP_PADDING,
    1,                                  // SLIDERBAR_BORDER_WIDTH,
    DEFAULT_BORDER_COLOR_NORMAL,        // SLIDERBAR_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_PRESSED,         // SLIDERBAR_BASE_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // SLIDERBAR_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,         // SLIDERBAR_BASE_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,       // SLIDERBAR_BORDER_COLOR_PRESSED,
    DEFAULT_BASE_COLOR_PRESSED,         // SLIDERBAR_BASE_COLOR_PRESSED,
    1,                                  // PROGRESSBAR_BORDER_WIDTH,
    DEFAULT_BORDER_COLOR_NORMAL,        // PROGRESSBAR_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,          // PROGRESSBAR_BASE_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_FOCUSED,         // PROGRESSBAR_FRONT_COLOR_NORMAL,
    2,                                  // SPINNER_BUTTON_PADDING
    DEFAULT_BORDER_COLOR_NORMAL,        // SPINNER_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,          // SPINNER_BASE_COLOR_NORMAL,
    DEFAULT_TEXT_COLOR_NORMAL,          // SPINNER_TEXT_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // SPINNER_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,         // SPINNER_BASE_COLOR_FOCUSED,
    DEFAULT_TEXT_COLOR_FOCUSED,         // SPINNER_TEXT_COLOR_FOCUSED,
    1,                                  // COMBOBOX_BORDER_WIDTH,
    2,                                  // COMBOBOX_BUTTON_PADDING,
    DEFAULT_BORDER_COLOR_NORMAL,        // COMBOBOX_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,          // COMBOBOX_BASE_COLOR_NORMAL,
    DEFAULT_TEXT_COLOR_NORMAL,          // COMBOBOX_TEXT_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // COMBOBOX_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,         // COMBOBOX_BASE_COLOR_FOCUSED,
    DEFAULT_TEXT_COLOR_FOCUSED,         // COMBOBOX_TEXT_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,       // COMBOBOX_BORDER_COLOR_PRESSED,
    DEFAULT_BASE_COLOR_PRESSED,         // COMBOBOX_BASE_COLOR_PRESSED,
    DEFAULT_TEXT_COLOR_PRESSED,         // COMBOBOX_TEXT_COLOR_PRESSED,
    1,                                  // CHECKBOX_BORDER_WIDTH,
    DEFAULT_BORDER_COLOR_NORMAL,        // CHECKBOX_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,          // CHECKBOX_BASE_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // CHECKBOX_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,         // CHECKBOX_BASE_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,       // CHECKBOX_BORDER_COLOR_PRESSED,
    DEFAULT_BASE_COLOR_PRESSED,         // CHECKBOX_BASE_COLOR_PRESSED,
    1,                                  // TEXTBOX_BORDER_WIDTH,
    DEFAULT_BORDER_COLOR_NORMAL,        // TEXTBOX_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,          // TEXTBOX_BASE_COLOR_NORMAL,
    DEFAULT_TEXT_COLOR_NORMAL,          // TEXTBOX_TEXT_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // TEXTBOX_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,         // TEXTBOX_BASE_COLOR_FOCUSED,
    DEFAULT_TEXT_COLOR_FOCUSED,         // TEXTBOX_TEXT_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,       // TEXTBOX_BORDER_COLOR_PRESSED,
    DEFAULT_BASE_COLOR_PRESSED,         // TEXTBOX_BASE_COLOR_PRESSED,
    DEFAULT_TEXT_COLOR_PRESSED,         // TEXTBOX_TEXT_COLOR_PRESSED,
};
#elif defined(RAYGUI_STYLE_DEFAULT_DARK)
// Current GUI style (default dark)
static int style[NUM_PROPERTIES] = {
    0x00
};
#endif

#if !defined(RAYGUI_NO_STYLE_SAVE_LOAD)
// GUI property names (to read/write style text files)
static const char *guiPropertyName[] = {
    "GLOBAL_BASE_COLOR",
    "GLOBAL_BORDER_COLOR",
    "GLOBAL_TEXT_COLOR",
    "DEFAULT_TEXT_SIZE",
    "GLOBAL_BORDER_WIDTH",
    "DEFAULT_BACKGROUND_COLOR",
    "DEFAULT_LINES_COLOR",
    "LABEL_BORDER_WIDTH",
    "LABEL_TEXT_COLOR",
    "LABEL_TEXT_PADDING",
    "BUTTON_BORDER_WIDTH",
    "BUTTON_TEXT_PADDING",
    "BUTTON_BORDER_COLOR_NORMAL",
    "BUTTON_BASE_COLOR_NORMAL",
    "BUTTON_TEXT_COLOR_NORMAL",
    "BUTTON_BORDER_COLOR_FOCUSED",
    "BUTTON_BASE_COLOR_FOCUSED",
    "BUTTON_TEXT_COLOR_FOCUSED",
    "BUTTON_BORDER_COLOR_PRESSED",
    "BUTTON_BASE_COLOR_PRESSED",
    "BUTTON_TEXT_COLOR_PRESSED",
    "TOGGLE_TEXT_PADDING",
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
    "TOGGLE_BORDER_COLOR_PRESSED",
    "TOGGLE_BASE_COLOR_PRESSED",
    "TOGGLE_TEXT_COLOR_PRESSED",
    "TOGGLEGROUP_PADDING",
    "SLIDERBAR_BORDER_WIDTH",
    "SLIDER_BUTTON_BORDER_WIDTH",
    "SLIDERBAR_BORDER_COLOR_NORMAL",
    "SLIDERBAR_BASE_COLOR_NORMAL",
    "SLIDERBAR_BASE_COLOR_NORMAL",
    "SLIDERBAR_BASE_COLOR_FOCUSED",
    "SLIDERBAR_BASE_COLOR_PRESSED",
    "SLIDERBAR_BORDER_COLOR",
    "SLIDERBAR_INSIDE_COLOR",
    "SLIDERBAR_DEFAULT_COLOR",
    "SLIDERBAR_HOVER_COLOR",
    "SLIDERBAR_ACTIVE_COLOR",
    "SLIDERBAR_ZERO_LINE_COLOR",
    "PROGRESSBAR_BORDER_COLOR_NORMAL",
    "PROGRESSBAR_BASE_COLOR_NORMAL",
    "PROGRESSBAR_FRONT_COLOR_NORMAL",
    "PROGRESSBAR_BORDER_WIDTH",
    "SPINNER_BORDER_COLOR_NORMAL",
    "SPINNER_BASE_COLOR_NORMAL",
    "SPINNER_DEFAULT_BUTTON_BORDER_COLOR",
    "SPINNER_DEFAULT_BUTTON_INSIDE_COLOR",
    "SPINNER_DEFAULT_SYMBOL_COLOR",
    "SPINNER_TEXT_COLOR_NORMAL",
    "SPINNER_HOVER_BUTTON_BORDER_COLOR",
    "SPINNER_HOVER_BUTTON_INSIDE_COLOR",
    "SPINNER_HOVER_SYMBOL_COLOR",
    "SPINNER_TEXT_COLOR_FOCUSED",
    "SPINNER_PRESSED_BUTTON_BORDER_COLOR",
    "SPINNER_PRESSED_BUTTON_INSIDE_COLOR",
    "SPINNER_PRESSED_SYMBOL_COLOR",
    "SPINNER_PRESSED_TEXT_COLOR",
    "COMBOBOX_PADDING",
    "COMBOBOX_BUTTON_WIDTH",
    "COMBOBOX_BUTTON_HEIGHT",
    "COMBOBOX_BORDER_WIDTH",
    "COMBOBOX_BORDER_COLOR_NORMAL",
    "COMBOBOX_BASE_COLOR_NORMAL",
    "COMBOBOX_TEXT_COLOR_NORMAL",
    "BUTTON_TEXT_COLOR_NORMAL",
    "COMBOBOX_BORDER_COLOR_FOCUSED",
    "COMBOBOX_BASE_COLOR_FOCUSED",
    "COMBOBOX_TEXT_COLOR_FOCUSED",
    "BUTTON_TEXT_COLOR_FOCUSED",
    "COMBOBOX_BORDER_COLOR_PRESSED",
    "COMBOBOX_BASE_COLOR_PRESSED",
    "COMBOBOX_TEXT_COLOR_PRESSED",
    "COMBOBOX_PRESSED_LIST_BORDER_COLOR",
    "COMBOBOX_PRESSED_LIST_INSIDE_COLOR",
    "COMBOBOX_PRESSED_LIST_TEXT_COLOR",
    "CHECKBOX_BORDER_COLOR_NORMAL",
    "CHECKBOX_BASE_COLOR_NORMAL",
    "CHECKBOX_BORDER_COLOR_FOCUSED",
    "CHECKBOX_BASE_COLOR_FOCUSED",
    "CHECKBOX_BORDER_COLOR_PRESSED",
    "CHECKBOX_BASE_COLOR_PRESSED",
    "CHECKBOX_BASE_COLOR_PRESSED",
    "CHECKBOX_BORDER_WIDTH",
    "TEXTBOX_BORDER_WIDTH",
    "TEXTBOX_BORDER_COLOR_NORMAL",
    "TEXTBOX_BASE_COLOR_NORMAL",
    "TEXTBOX_TEXT_COLOR_NORMAL",
    "TEXTBOX_LINE_COLOR",
    "TEXTBOX_TEXT_FONTSIZE"
};
#endif

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
#if defined RAYGUI_STANDALONE
static Color GetColor(int hexValue);   // Returns a Color struct from hexadecimal value
static int GetHexValue(Color color);   // Returns hexadecimal value for a Color
static bool CheckCollisionPointRec(Vector2 point, Rectangle rec);  // Check if point is inside rectangle
static const char *FormatText(const char *text, ...);   // Formatting of text with variables to 'embed'

// NOTE: raygui depend on some raylib input and drawing functions
// TODO: To use raygui as standalone library, those functions must be overwrite by custom ones

// Input management functions
static Vector2 GetMousePosition() { /* TODO */ return (Vector2){ 0.0f, 0.0f }; }
static int IsMouseButtonDown(int button) { /* TODO */ return 0; }
static int IsMouseButtonPressed(int button) { /* TODO */ return 0; }
static int IsMouseButtonReleased(int button) { /* TODO */ return 0; }
static int IsMouseButtonUp(int button) { /* TODO */ return 0; }

static int GetKeyPressed(void) { /* TODO */ return 0; }    // NOTE: Only used by GuiTextBox()
//static int IsKeyDown(int key) { /* TODO */ return 0; }     // NOTE: Only used by GuiSpinner()

// Drawing related functions
static int MeasureText(const char *text, int fontSize) { /* TODO */ return 0; }
static void DrawText(const char *text, int posX, int posY, int fontSize, Color color) { /* TODO */ }
static void DrawRectangleRec(Rectangle rec, Color color) { /* TODO */ }
static void DrawRectangle(int posX, int posY, int width, int height, Color color) { DrawRectangleRec((Rectangle){ posX, posY, width, height }, color); }
#endif

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Get background color
RAYGUIDEF Color GuiBackgroundColor(void) { return GetColor(style[DEFAULT_BACKGROUND_COLOR]); }

// Get lines color
RAYGUIDEF Color GuiLinesColor(void) { return GetColor(style[DEFAULT_LINES_COLOR]); }

// Label element, show text
RAYGUIDEF void GuiLabel(Rectangle bounds, const char *text)
{
    GuiLabelEx(bounds, text, GetColor(style[LABEL_TEXT_COLOR_NORMAL]), (Color){ 0, 0, 0, 0 }, (Color){ 0, 0, 0, 0 });
}

// Label element extended, configurable colors
RAYGUIDEF void GuiLabelEx(Rectangle bounds, const char *text, Color textColor, Color border, Color inner)
{
    // Update control
    //--------------------------------------------------------------------
    int textWidth = MeasureText(text, style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];

    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleRec(bounds, border);
    DrawRectangle(bounds.x + style[DEFAULT_BORDER_WIDTH], bounds.y + style[DEFAULT_BORDER_WIDTH], bounds.width - (2*style[DEFAULT_BORDER_WIDTH]), bounds.height - (2*style[DEFAULT_BORDER_WIDTH]), inner);
    DrawText(text, bounds.x + ((bounds.width/2) - (textWidth/2)), bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], textColor);
    //--------------------------------------------------------------------
}

// Button element, returns true when clicked
RAYGUIDEF bool GuiButton(Rectangle bounds, const char *text)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();
    bool clicked = false;

    int textWidth = MeasureText(text, style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];

    // Update control
    //--------------------------------------------------------------------
    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;

    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) clicked = true;
        else state = FOCUSED;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_NORMAL]));
            DrawRectangle((int)(bounds.x + style[BUTTON_BORDER_WIDTH]), (int)(bounds.y + style[BUTTON_BORDER_WIDTH]) , (int)(bounds.width - (2*style[BUTTON_BORDER_WIDTH])), (int)(bounds.height - (2*style[BUTTON_BORDER_WIDTH])), GetColor(style[BUTTON_BASE_COLOR_NORMAL]));
            DrawText(text, bounds.x + ((bounds.width/2) - (MeasureText(text, style[DEFAULT_TEXT_SIZE])/2)), bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_FOCUSED]));
            DrawRectangle((int)(bounds.x + style[BUTTON_BORDER_WIDTH]), (int)(bounds.y + style[BUTTON_BORDER_WIDTH]) , (int)(bounds.width - (2*style[BUTTON_BORDER_WIDTH])), (int)(bounds.height - (2*style[BUTTON_BORDER_WIDTH])), GetColor(style[BUTTON_BASE_COLOR_FOCUSED]));
            DrawText(text, bounds.x + ((bounds.width/2) - (MeasureText(text, style[DEFAULT_TEXT_SIZE])/2)), bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_PRESSED]));
            DrawRectangle((int)(bounds.x + style[BUTTON_BORDER_WIDTH]), (int)(bounds.y + style[BUTTON_BORDER_WIDTH]) , (int)(bounds.width - (2*style[BUTTON_BORDER_WIDTH])), (int)(bounds.height - (2*style[BUTTON_BORDER_WIDTH])), GetColor(style[BUTTON_BASE_COLOR_PRESSED]));
            DrawText(text, bounds.x + ((bounds.width/2) - (MeasureText(text, style[DEFAULT_TEXT_SIZE])/2)), bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_PRESSED]));
        } break;
        default: break;
    }
    //------------------------------------------------------------------

    if (clicked) return true;
    else return false;
}

// Toggle Button element, returns true when active
RAYGUIDEF bool GuiToggleButton(Rectangle bounds, const char *text, bool active)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();

    int textWidth = MeasureText(text, style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];

    // Update control
    //--------------------------------------------------------------------
    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;

    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            state = NORMAL;
            active = !active;
        }
        else state = FOCUSED;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            if (active)
            {
                DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_PRESSED]));
                DrawRectangle((int)(bounds.x + style[TOGGLE_BORDER_WIDTH]), (int)(bounds.y + style[TOGGLE_BORDER_WIDTH]) , (int)(bounds.width - (2*style[TOGGLE_BORDER_WIDTH])), (int)(bounds.height - (2*style[TOGGLE_BORDER_WIDTH])), GetColor(style[TOGGLE_BASE_COLOR_PRESSED]));
                DrawText(text, bounds.x + ((bounds.width/2) - (MeasureText(text, style[DEFAULT_TEXT_SIZE])/2)), bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_PRESSED]));
            }
            else
            {
                DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_NORMAL]));
                DrawRectangle((int)(bounds.x + style[TOGGLE_BORDER_WIDTH]), (int)(bounds.y + style[TOGGLE_BORDER_WIDTH]) , (int)(bounds.width - (2*style[TOGGLE_BORDER_WIDTH])), (int)(bounds.height - (2*style[TOGGLE_BORDER_WIDTH])), GetColor(style[TOGGLE_BASE_COLOR_NORMAL]));
                DrawText(text, bounds.x + ((bounds.width/2) - (MeasureText(text, style[DEFAULT_TEXT_SIZE])/2)), bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_NORMAL]));
            }
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_FOCUSED]));
            DrawRectangle((int)(bounds.x + style[TOGGLE_BORDER_WIDTH]), (int)(bounds.y + style[TOGGLE_BORDER_WIDTH]) , (int)(bounds.width - (2*style[TOGGLE_BORDER_WIDTH])), (int)(bounds.height - (2*style[TOGGLE_BORDER_WIDTH])), GetColor(style[TOGGLE_BASE_COLOR_FOCUSED]));
            DrawText(text, bounds.x + ((bounds.width/2) - (MeasureText(text, style[DEFAULT_TEXT_SIZE])/2)), bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_PRESSED]));
            DrawRectangle((int)(bounds.x + style[TOGGLE_BORDER_WIDTH]), (int)(bounds.y + style[TOGGLE_BORDER_WIDTH]) , (int)(bounds.width - (2*style[TOGGLE_BORDER_WIDTH])), (int)(bounds.height - (2*style[TOGGLE_BORDER_WIDTH])), GetColor(style[TOGGLE_BASE_COLOR_PRESSED]));
            DrawText(text, bounds.x + ((bounds.width/2) - (MeasureText(text, style[DEFAULT_TEXT_SIZE])/2)), bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_PRESSED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------

    return active;
}

// Toggle Group element, returns toggled button index
RAYGUIDEF int GuiToggleGroup(Rectangle bounds, int toggleCount, char **toggleText, int active)
{
    for (int i = 0; i < toggleCount; i++)
    {
        if (i == active) GuiToggleButton((Rectangle){bounds.x + i*(bounds.width + style[TOGGLEGROUP_PADDING]),bounds.y,bounds.width,bounds.height}, toggleText[i], true);
        else if (GuiToggleButton((Rectangle){bounds.x + i*(bounds.width + style[TOGGLEGROUP_PADDING]),bounds.y,bounds.width,bounds.height}, toggleText[i], false) == true) active = i;
    }

    return active;
}

// Combo Box element, returns selected item index
RAYGUIDEF int GuiComboBox(Rectangle bounds, int comboCount, char **comboText, int active)
{
    ControlState state = NORMAL;

    Rectangle click = { bounds.x + bounds.width + style[COMBOBOX_BUTTON_PADDING], bounds.y, bounds.width/comboCount, bounds.height };
    Vector2 mousePoint = GetMousePosition();

    int textHeight = style[DEFAULT_TEXT_SIZE];
    
    bool clicked = false;

    for (int i = 0; i < comboCount; i++)
    {
        if (i == active)
        {
            // Update control
            //--------------------------------------------------------------------
            int textWidth = MeasureText(comboText[i], style[DEFAULT_TEXT_SIZE]);

            if (bounds.width < textWidth) bounds.width = textWidth;
            if (bounds.height < textHeight) bounds.height = textHeight;

            if (CheckCollisionPointRec(mousePoint, bounds) || CheckCollisionPointRec(mousePoint, click))
            {
                if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
                else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) clicked = true;
                else state = FOCUSED;
            }
            //--------------------------------------------------------------------

            // Draw control
            //--------------------------------------------------------------------
            switch (state)
            {
                case NORMAL:
                {
                    DrawRectangleRec(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_NORMAL]));
                    DrawRectangle((int)(bounds.x + style[COMBOBOX_BORDER_WIDTH]), (int)(bounds.y + style[COMBOBOX_BORDER_WIDTH]), (int)(bounds.width - (2*style[COMBOBOX_BORDER_WIDTH])), (int)(bounds.height - (2*style[COMBOBOX_BORDER_WIDTH])), GetColor(style[COMBOBOX_BASE_COLOR_NORMAL]));

                    DrawRectangleRec(click, GetColor(style[COMBOBOX_BORDER_COLOR_NORMAL]));
                    DrawRectangle((int)(click.x + style[COMBOBOX_BORDER_WIDTH]), (int)(click.y + style[COMBOBOX_BORDER_WIDTH]) , (int)(click.width - (2*style[COMBOBOX_BORDER_WIDTH])), (int)(click.height - (2*style[COMBOBOX_BORDER_WIDTH])), GetColor(style[COMBOBOX_BASE_COLOR_NORMAL]));
                    DrawText(FormatText("%i/%i", active + 1, comboCount), click.x + ((click.width/2) - (MeasureText(FormatText("%i/%i", active + 1, comboCount), style[DEFAULT_TEXT_SIZE])/2)), click.y + ((click.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));
                    DrawText(comboText[i], bounds.x + ((bounds.width/2) - (MeasureText(comboText[i], style[DEFAULT_TEXT_SIZE])/2)), bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_NORMAL]));
                } break;
                case FOCUSED:
                {
                    DrawRectangleRec(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_FOCUSED]));
                    DrawRectangle((int)(bounds.x + style[COMBOBOX_BORDER_WIDTH]), (int)(bounds.y + style[COMBOBOX_BORDER_WIDTH]), (int)(bounds.width - (2*style[COMBOBOX_BORDER_WIDTH])), (int)(bounds.height - (2*style[COMBOBOX_BORDER_WIDTH])), GetColor(style[COMBOBOX_BASE_COLOR_FOCUSED]));

                    DrawRectangleRec(click, GetColor(style[COMBOBOX_BORDER_COLOR_FOCUSED]));
                    DrawRectangle((int)(click.x + style[COMBOBOX_BORDER_WIDTH]), (int)(click.y + style[COMBOBOX_BORDER_WIDTH]) , (int)(click.width - (2*style[COMBOBOX_BORDER_WIDTH])), (int)(click.height - (2*style[COMBOBOX_BORDER_WIDTH])), GetColor(style[COMBOBOX_BASE_COLOR_FOCUSED]));
                    DrawText(FormatText("%i/%i", active + 1, comboCount), click.x + ((click.width/2) - (MeasureText(FormatText("%i/%i", active + 1, comboCount), style[DEFAULT_TEXT_SIZE])/2)), click.y + ((click.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_FOCUSED]));
                    DrawText(comboText[i], bounds.x + ((bounds.width/2) - (MeasureText(comboText[i], style[DEFAULT_TEXT_SIZE])/2)), bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_FOCUSED]));
                } break;
                case PRESSED:
                {
                    DrawRectangleRec(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_PRESSED]));
                    DrawRectangle((int)(bounds.x + style[COMBOBOX_BORDER_WIDTH]), (int)(bounds.y + style[COMBOBOX_BORDER_WIDTH]), (int)(bounds.width - (2*style[COMBOBOX_BORDER_WIDTH])), (int)(bounds.height - (2*style[COMBOBOX_BORDER_WIDTH])), GetColor(style[COMBOBOX_BASE_COLOR_PRESSED]));

                    DrawRectangleRec(click, GetColor(style[COMBOBOX_BORDER_COLOR_PRESSED]));
                    DrawRectangle((int)(click.x + style[COMBOBOX_BORDER_WIDTH]), (int)(click.y + style[COMBOBOX_BORDER_WIDTH]) , (int)(click.width - (2*style[COMBOBOX_BORDER_WIDTH])), (int)(click.height - (2*style[COMBOBOX_BORDER_WIDTH])), GetColor(style[COMBOBOX_BASE_COLOR_PRESSED]));
                    DrawText(FormatText("%i/%i", active + 1, comboCount), click.x + ((click.width/2) - (MeasureText(FormatText("%i/%i", active + 1, comboCount), style[DEFAULT_TEXT_SIZE])/2)), click.y + ((click.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_PRESSED]));
                    DrawText(comboText[i], bounds.x + ((bounds.width/2) - (MeasureText(comboText[i], style[DEFAULT_TEXT_SIZE])/2)), bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_PRESSED]));
                } break;
                default: break;
            }
            
            if (clicked)
            {
                DrawRectangleRec(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_PRESSED]));
                DrawRectangle((int)(bounds.x + style[COMBOBOX_BORDER_WIDTH]), (int)(bounds.y + style[COMBOBOX_BORDER_WIDTH]), (int)(bounds.width - (2 * style[COMBOBOX_BORDER_WIDTH])), (int)(bounds.height - (2*style[COMBOBOX_BORDER_WIDTH])), GetColor(style[COMBOBOX_BASE_COLOR_PRESSED]));

                DrawRectangleRec(click, GetColor(style[COMBOBOX_BORDER_COLOR_PRESSED]));
                DrawRectangle((int)(click.x + style[COMBOBOX_BORDER_WIDTH]), (int)(click.y + style[COMBOBOX_BORDER_WIDTH]) , (int)(click.width - (2*style[COMBOBOX_BORDER_WIDTH])), (int)(click.height - (2*style[COMBOBOX_BORDER_WIDTH])), GetColor(style[COMBOBOX_BASE_COLOR_PRESSED]));
                DrawText(FormatText("%i/%i", active + 1, comboCount), click.x + ((click.width/2) - (MeasureText(FormatText("%i/%i", active + 1, comboCount), style[DEFAULT_TEXT_SIZE])/2)), click.y + ((click.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_PRESSED]));
                DrawText(comboText[i], bounds.x + ((bounds.width/2) - (MeasureText(comboText[i], style[DEFAULT_TEXT_SIZE])/2)), bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_PRESSED]));
            }

            //DrawText(comboText[i], bounds.x + ((bounds.width/2) - (MeasureText(comboText[i], style[]globalTextFontSize)/2)), bounds.y + ((bounds.height/2) - (style[]globalTextFontSize/2)), style[]globalTextFontSize, COMBOBOX_TEXT_COLOR_PRESSED);
            //--------------------------------------------------------------------
        }
    }

    if (CheckCollisionPointRec(mousePoint, bounds) || 
        CheckCollisionPointRec(mousePoint, click))
    {
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
        {
           active += 1;
           if (active >= comboCount) active = 0;
        }
    }

    return active;
}

// Check Box element, returns true when active
RAYGUIDEF bool GuiCheckBox(Rectangle bounds, const char *text, bool checked)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();

    // Update control
    //--------------------------------------------------------------------
    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            state = NORMAL;
            checked = !checked;
        }
        else state = FOCUSED;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRec(bounds, GetColor(style[CHECKBOX_BORDER_COLOR_NORMAL]));
            DrawRectangle((int)(bounds.x + style[TOGGLE_BORDER_WIDTH]), (int)(bounds.y + style[TOGGLE_BORDER_WIDTH]), (int)(bounds.width - (2*style[TOGGLE_BORDER_WIDTH])), (int)(bounds.height - (2*style[TOGGLE_BORDER_WIDTH])), GetColor(style[CHECKBOX_BASE_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[CHECKBOX_BORDER_COLOR_FOCUSED]));
            DrawRectangle((int)(bounds.x + style[TOGGLE_BORDER_WIDTH]), (int)(bounds.y + style[TOGGLE_BORDER_WIDTH]), (int)(bounds.width - (2*style[TOGGLE_BORDER_WIDTH])), (int)(bounds.height - (2*style[TOGGLE_BORDER_WIDTH])), GetColor(style[CHECKBOX_BASE_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[CHECKBOX_BORDER_COLOR_PRESSED]));
            DrawRectangle((int)(bounds.x + style[TOGGLE_BORDER_WIDTH]), (int)(bounds.y + style[TOGGLE_BORDER_WIDTH]), (int)(bounds.width - (2*style[TOGGLE_BORDER_WIDTH])), (int)(bounds.height - (2*style[TOGGLE_BORDER_WIDTH])), GetColor(style[CHECKBOX_BASE_COLOR_PRESSED]));
        } break;
        default: break;
    }

    if (text != NULL) DrawText(text, bounds.x + bounds.width + 2, bounds.y + ((bounds.height/2) - (style[DEFAULT_TEXT_SIZE]/2) + 1), style[DEFAULT_TEXT_SIZE], GetColor(style[DEFAULT_TEXT_COLOR]));

    if (checked)
    {
        DrawRectangle((int)(bounds.x + style[CHECKBOX_BORDER_WIDTH]), (int)(bounds.y + style[CHECKBOX_BORDER_WIDTH]), (int)(bounds.width - (2*style[CHECKBOX_BORDER_WIDTH])), (int)(bounds.height - (2*style[CHECKBOX_BORDER_WIDTH])), GetColor(style[CHECKBOX_BASE_COLOR_PRESSED]));
    }
    //--------------------------------------------------------------------

    return checked;
}

// Slider element, returns selected value
RAYGUIDEF float GuiSlider(Rectangle bounds, float value, float minValue, float maxValue)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();
    float buttonTravelDistance = 0;
    float sliderPos = 0;

    // Update control
    //--------------------------------------------------------------------
    if (value < minValue) value = minValue;
    else if (value >= maxValue) value = maxValue;

    sliderPos = (value - minValue)/(maxValue - minValue);

    Rectangle sliderButton;
    sliderButton.width = ((int)(bounds.width - (2*style[SLIDERBAR_BORDER_WIDTH]))/10 - 8);
    sliderButton.height = ((int)(bounds.height - (2*style[SLIDERBAR_BORDER_WIDTH] + 2*style[SLIDERBAR_BORDER_WIDTH])));

    float sliderButtonMinPos = bounds.x + style[SLIDERBAR_BORDER_WIDTH] + style[SLIDERBAR_BORDER_WIDTH];
    float sliderButtonMaxPos = bounds.x + bounds.width - (style[SLIDERBAR_BORDER_WIDTH] + style[SLIDERBAR_BORDER_WIDTH] + sliderButton.width);

    buttonTravelDistance = sliderButtonMaxPos - sliderButtonMinPos;

    sliderButton.x = ((int)(bounds.x + style[SLIDERBAR_BORDER_WIDTH] + style[SLIDERBAR_BORDER_WIDTH]) + (sliderPos * buttonTravelDistance));
    sliderButton.y = ((int)(bounds.y + style[SLIDERBAR_BORDER_WIDTH] + style[SLIDERBAR_BORDER_WIDTH]));

    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        state = FOCUSED;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;

        if ((state == PRESSED) && (IsMouseButtonDown(MOUSE_LEFT_BUTTON)))
        {
            sliderButton.x = mousePoint.x - sliderButton.width/2;

            if (sliderButton.x <= sliderButtonMinPos) sliderButton.x = sliderButtonMinPos;
            else if (sliderButton.x >= sliderButtonMaxPos) sliderButton.x = sliderButtonMaxPos;

            sliderPos = (sliderButton.x - sliderButtonMinPos)/buttonTravelDistance;
        }
    }
    else state = NORMAL;
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleRec(bounds, GetColor(style[SLIDERBAR_BORDER_COLOR_NORMAL]));
    DrawRectangle((int)(bounds.x + style[SLIDERBAR_BORDER_WIDTH]), (int)(bounds.y + style[SLIDERBAR_BORDER_WIDTH]), (int)(bounds.width - (2*style[SLIDERBAR_BORDER_WIDTH])), (int)(bounds.height - (2*style[SLIDERBAR_BORDER_WIDTH])), GetColor(style[SLIDERBAR_BASE_COLOR_NORMAL]));

    switch (state)
    {
        case NORMAL: DrawRectangleRec(sliderButton, GetColor(style[SLIDERBAR_BASE_COLOR_NORMAL])); break;
        case FOCUSED: DrawRectangleRec(sliderButton, GetColor(style[SLIDERBAR_BASE_COLOR_FOCUSED])); break;
        case PRESSED: DrawRectangleRec(sliderButton, GetColor(style[SLIDERBAR_BASE_COLOR_PRESSED])); break;
        default: break;
    }
    //--------------------------------------------------------------------

    return minValue + (maxValue - minValue)*sliderPos;
}

// Slider Bar element, returns selected value
RAYGUIDEF float GuiSliderBar(Rectangle bounds, float value, float minValue, float maxValue)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();
    float fixedValue;
    float fixedMinValue;

    fixedValue = value - minValue;
    maxValue = maxValue - minValue;
    fixedMinValue = 0;

    // Update control
    //--------------------------------------------------------------------
    if (fixedValue <= fixedMinValue) fixedValue = fixedMinValue;
    else if (fixedValue >= maxValue) fixedValue = maxValue;

    Rectangle sliderBar;

    sliderBar.x = bounds.x + style[SLIDERBAR_BORDER_WIDTH];
    sliderBar.y = bounds.y + style[SLIDERBAR_BORDER_WIDTH];
    sliderBar.width = ((fixedValue*((float)bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH]))/(maxValue - fixedMinValue));
    sliderBar.height = bounds.height - 2*style[SLIDERBAR_BORDER_WIDTH];

    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        state = FOCUSED;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            state = PRESSED;

            sliderBar.width = (mousePoint.x - bounds.x - style[SLIDERBAR_BORDER_WIDTH]);

            if (mousePoint.x <= (bounds.x + style[SLIDERBAR_BORDER_WIDTH])) sliderBar.width = 0;
            else if (mousePoint.x >= (bounds.x + bounds.width - style[SLIDERBAR_BORDER_WIDTH])) sliderBar.width = bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH];
        }
    }
    else state = NORMAL;

    fixedValue = ((float)sliderBar.width*(maxValue - fixedMinValue))/((float)bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH]);
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRec(bounds, GetColor(style[SLIDERBAR_BORDER_COLOR_NORMAL]));
            DrawRectangle((int)(bounds.x + style[SLIDERBAR_BORDER_WIDTH]), (int)(bounds.y + style[SLIDERBAR_BORDER_WIDTH]) , (int)(bounds.width - (2*style[SLIDERBAR_BORDER_WIDTH])), (int)(bounds.height - (2*style[SLIDERBAR_BORDER_WIDTH])), GetColor(style[SLIDERBAR_BASE_COLOR_NORMAL]));
            DrawRectangleRec(sliderBar, GetColor(style[SLIDERBAR_BASE_COLOR_NORMAL])); 
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[SLIDERBAR_BORDER_COLOR_FOCUSED]));
            DrawRectangle((int)(bounds.x + style[SLIDERBAR_BORDER_WIDTH]), (int)(bounds.y + style[SLIDERBAR_BORDER_WIDTH]) , (int)(bounds.width - (2*style[SLIDERBAR_BORDER_WIDTH])), (int)(bounds.height - (2*style[SLIDERBAR_BORDER_WIDTH])), GetColor(style[SLIDERBAR_BASE_COLOR_FOCUSED]));
            DrawRectangleRec(sliderBar, GetColor(style[SLIDERBAR_BASE_COLOR_FOCUSED])); 
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[SLIDERBAR_BORDER_COLOR_PRESSED]));
            DrawRectangle((int)(bounds.x + style[SLIDERBAR_BORDER_WIDTH]), (int)(bounds.y + style[SLIDERBAR_BORDER_WIDTH]) , (int)(bounds.width - (2*style[SLIDERBAR_BORDER_WIDTH])), (int)(bounds.height - (2*style[SLIDERBAR_BORDER_WIDTH])), GetColor(style[SLIDERBAR_BASE_COLOR_PRESSED]));
            DrawRectangleRec(sliderBar, GetColor(style[SLIDERBAR_BASE_COLOR_PRESSED])); 
        } break;
        default: break;
    }

    if (minValue < 0 && maxValue > 0) DrawRectangle((bounds.x + style[SLIDERBAR_BORDER_WIDTH]) - (minValue*((bounds.width - (style[SLIDERBAR_BORDER_WIDTH]*2))/maxValue)), sliderBar.y, 1, sliderBar.height, GetColor(style[SLIDERBAR_BORDER_COLOR_NORMAL]));
    //--------------------------------------------------------------------

    return fixedValue + minValue;
}

// Progress Bar element, shows current progress value
RAYGUIDEF void GuiProgressBar(Rectangle bounds, float value)
{
    if (value > 1.0f) value = 1.0f;
    else if (value < 0.0f) value = 0.0f;

    Rectangle progressBar = { bounds.x + style[PROGRESSBAR_BORDER_WIDTH], bounds.y + style[PROGRESSBAR_BORDER_WIDTH], bounds.width - (style[PROGRESSBAR_BORDER_WIDTH]*2), bounds.height - (style[PROGRESSBAR_BORDER_WIDTH] * 2)};
    Rectangle progressValue = { bounds.x + style[PROGRESSBAR_BORDER_WIDTH], bounds.y + style[PROGRESSBAR_BORDER_WIDTH], value * (bounds.width - (style[PROGRESSBAR_BORDER_WIDTH]*2)), bounds.height - (style[PROGRESSBAR_BORDER_WIDTH] * 2)};

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleRec(bounds, GetColor(style[PROGRESSBAR_BORDER_COLOR_NORMAL]));
    DrawRectangleRec(progressBar, GetColor(style[PROGRESSBAR_BASE_COLOR_NORMAL]));
    DrawRectangleRec(progressValue, GetColor(style[PROGRESSBAR_FRONT_COLOR_NORMAL]));
    //--------------------------------------------------------------------
}

// Spinner element, returns selected value
// NOTE: Requires static variables: framesCounter, valueSpeed - ERROR!
RAYGUIDEF int GuiSpinner(Rectangle bounds, int value, int minValue, int maxValue)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();
    Rectangle labelBoxBound = { bounds.x + bounds.width/4 + 1, bounds.y, bounds.width/2, bounds.height };
    Rectangle leftButtonBound = { bounds.x, bounds.y, bounds.width/4, bounds.height };
    Rectangle rightButtonBound = { bounds.x + bounds.width - bounds.width/4 + 1, bounds.y, bounds.width/4, bounds.height };

    int textWidth = MeasureText(FormatText("%i", value), style[DEFAULT_TEXT_SIZE]);
    //int textHeight = style[DEFAULT_TEXT_SIZE];     // Unused variable

    int buttonSide = 0;

    static int framesCounter = 0;
    static bool valueSpeed = false;;

    //if (bounds.width < textWidth) bounds.width = textWidth;
    //if (bounds.height < textHeight) bounds.height = textHeight;

    // Update control
    //--------------------------------------------------------------------
    /*
    if (CheckCollisionPointRec(mousePoint, leftButtonBound) || 
        CheckCollisionPointRec(mousePoint, rightButtonBound) || 
        CheckCollisionPointRec(mousePoint, labelBoxBound))
    {
        if (IsKeyDown(KEY_LEFT))
        {
            state = PRESSED;
            buttonSide = 1;

            if (value > minValue) value -= 1;
        }
        else if (IsKeyDown(KEY_RIGHT))
        {
            state = PRESSED;
            buttonSide = 2;

            if (value < maxValue) value += 1;
        }
    }
    */
    
    if (CheckCollisionPointRec(mousePoint, leftButtonBound))
    {
       buttonSide = 1;
       state = FOCUSED;

       if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
       {
            if (!valueSpeed)
            {
                if (value > minValue) value--;
                valueSpeed = true;
            }
            else framesCounter++;

            state = PRESSED;

            if (value > minValue)
            {
                if (framesCounter >= 30) value -= 1;
            }
       }
    }
    else if (CheckCollisionPointRec(mousePoint, rightButtonBound))
    {
        buttonSide = 2;
        state = FOCUSED;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
       {
            if (!valueSpeed)
            {
                if (value < maxValue) value++;
                valueSpeed = true;
            }
            else framesCounter++;

            state = PRESSED;

            if (value < maxValue)
            {
                if (framesCounter >= 30) value += 1;
            }
       }
    }
    else if (!CheckCollisionPointRec(mousePoint, labelBoxBound)) buttonSide = 0;

    if (IsMouseButtonUp(MOUSE_LEFT_BUTTON))
    {
        valueSpeed = false;
        framesCounter = 0;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRec(leftButtonBound, GetColor(style[BUTTON_BORDER_COLOR_NORMAL]));
            DrawRectangle(leftButtonBound.x + 2, leftButtonBound.y + 2, leftButtonBound.width - 4, leftButtonBound.height - 4, GetColor(style[BUTTON_BASE_COLOR_NORMAL]));

            DrawRectangleRec(rightButtonBound, GetColor(style[BUTTON_BORDER_COLOR_NORMAL]));
            DrawRectangle(rightButtonBound.x + 2, rightButtonBound.y + 2, rightButtonBound.width - 4, rightButtonBound.height - 4, GetColor(style[BUTTON_BASE_COLOR_NORMAL]));

            DrawText("-", leftButtonBound.x + (leftButtonBound.width/2 - (MeasureText("+", style[DEFAULT_TEXT_SIZE]))/2), leftButtonBound.y + (leftButtonBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));
            DrawText("+", rightButtonBound.x + (rightButtonBound.width/2 - (MeasureText("-", style[DEFAULT_TEXT_SIZE]))/2), rightButtonBound.y + (rightButtonBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));

            DrawRectangleRec(labelBoxBound, GetColor(style[SPINNER_BORDER_COLOR_NORMAL]));
            DrawRectangle(labelBoxBound.x + 1, labelBoxBound.y + 1, labelBoxBound.width - 2, labelBoxBound.height - 2, GetColor(style[SPINNER_BASE_COLOR_NORMAL]));

            DrawText(FormatText("%i", value), labelBoxBound.x + (labelBoxBound.width/2 - textWidth/2), labelBoxBound.y + (labelBoxBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            /*
            if (buttonSide == 1)
            {
                DrawRectangleRec(leftButtonBound, GetColor(style[BUTTON_BORDER_COLOR_FOCUSED]));
                DrawRectangle(leftButtonBound.x + 2, leftButtonBound.y + 2, leftButtonBound.width - 4, leftButtonBound.height - 4, GetColor(style[BUTTON_BASE_COLOR_FOCUSED]));

                DrawRectangleRec(rightButtonBound, GetColor(style[BUTTON_BORDER_COLOR_NORMAL]));
                DrawRectangle(rightButtonBound.x + 2, rightButtonBound.y + 2, rightButtonBound.width - 4, rightButtonBound.height - 4, GetColor(style[BUTTON_BORDER_COLOR_NORMAL]));

                DrawText("-", leftButtonBound.x + (leftButtonBound.width/2 - (MeasureText("+", style[DEFAULT_TEXT_SIZE]))/2), leftButtonBound.y + (leftButtonBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_HOVER_SYMBOL_COLOR]));
                DrawText("+", rightButtonBound.x + (rightButtonBound.width/2 - (MeasureText("-", style[DEFAULT_TEXT_SIZE]))/2), rightButtonBound.y + (rightButtonBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_DEFAULT_SYMBOL_COLOR]));
            }
            else if (buttonSide == 2)
            {
                DrawRectangleRec(leftButtonBound, GetColor(style[BUTTON_BORDER_COLOR_FOCUSED]));
                DrawRectangle(leftButtonBound.x + 2, leftButtonBound.y + 2, leftButtonBound.width - 4, leftButtonBound.height - 4, GetColor(style[SPINNER_DEFAULT_BUTTON_INSIDE_COLOR]));

                DrawRectangleRec(rightButtonBound, GetColor(style[SPINNER_HOVER_BUTTON_BORDER_COLOR]));
                DrawRectangle(rightButtonBound.x + 2, rightButtonBound.y + 2, rightButtonBound.width - 4, rightButtonBound.height - 4, GetColor(style[SPINNER_HOVER_BUTTON_INSIDE_COLOR]));

                DrawText("-", leftButtonBound.x + (leftButtonBound.width/2 - (MeasureText("+", style[DEFAULT_TEXT_SIZE]))/2), leftButtonBound.y + (leftButtonBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_DEFAULT_SYMBOL_COLOR]));
                DrawText("+", rightButtonBound.x + (rightButtonBound.width/2 - (MeasureText("-", style[DEFAULT_TEXT_SIZE]))/2), rightButtonBound.y + (rightButtonBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_HOVER_SYMBOL_COLOR]));
            }
            */

            DrawRectangleRec(labelBoxBound, GetColor(style[SPINNER_BORDER_COLOR_NORMAL]));
            DrawRectangle(labelBoxBound.x + 1, labelBoxBound.y + 1, labelBoxBound.width - 2, labelBoxBound.height - 2, GetColor(style[SPINNER_BASE_COLOR_NORMAL]));

            DrawText(FormatText("%i", value), labelBoxBound.x + (labelBoxBound.width/2 - textWidth/2), labelBoxBound.y + (labelBoxBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            /*
            if (buttonSide == 1)
            {
                DrawRectangleRec(leftButtonBound, GetColor(style[SPINNER_PRESSED_BUTTON_BORDER_COLOR]));
                DrawRectangle(leftButtonBound.x + 2, leftButtonBound.y + 2, leftButtonBound.width - 4, leftButtonBound.height - 4, GetColor(style[SPINNER_PRESSED_BUTTON_INSIDE_COLOR]));

                DrawRectangleRec(rightButtonBound, GetColor(style[SPINNER_DEFAULT_BUTTON_BORDER_COLOR]));
                DrawRectangle(rightButtonBound.x + 2, rightButtonBound.y + 2, rightButtonBound.width - 4, rightButtonBound.height - 4, GetColor(style[SPINNER_DEFAULT_BUTTON_INSIDE_COLOR]));

                DrawText("-", leftButtonBound.x + (leftButtonBound.width/2 - (MeasureText("+", style[DEFAULT_TEXT_SIZE]))/2), leftButtonBound.y + (leftButtonBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_PRESSED_SYMBOL_COLOR]));
                DrawText("+", rightButtonBound.x + (rightButtonBound.width/2 - (MeasureText("-", style[DEFAULT_TEXT_SIZE]))/2), rightButtonBound.y + (rightButtonBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_DEFAULT_SYMBOL_COLOR]));
            }
            else if (buttonSide == 2)
            {
                DrawRectangleRec(leftButtonBound, GetColor(style[SPINNER_DEFAULT_BUTTON_BORDER_COLOR]));
                DrawRectangle(leftButtonBound.x + 2, leftButtonBound.y + 2, leftButtonBound.width - 4, leftButtonBound.height - 4, GetColor(style[SPINNER_DEFAULT_BUTTON_INSIDE_COLOR]));

                DrawRectangleRec(rightButtonBound, GetColor(style[SPINNER_PRESSED_BUTTON_BORDER_COLOR]));
                DrawRectangle(rightButtonBound.x + 2, rightButtonBound.y + 2, rightButtonBound.width - 4, rightButtonBound.height - 4, GetColor(style[SPINNER_PRESSED_BUTTON_INSIDE_COLOR]));

                DrawText("-", leftButtonBound.x + (leftButtonBound.width/2 - (MeasureText("+", style[DEFAULT_TEXT_SIZE]))/2), leftButtonBound.y + (leftButtonBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_DEFAULT_SYMBOL_COLOR]));
                DrawText("+", rightButtonBound.x + (rightButtonBound.width/2 - (MeasureText("-", style[DEFAULT_TEXT_SIZE]))/2), rightButtonBound.y + (rightButtonBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_PRESSED_SYMBOL_COLOR]));
            }
            */
            
            DrawRectangleRec(labelBoxBound, GetColor(style[SPINNER_BORDER_COLOR_NORMAL]));
            DrawRectangle(labelBoxBound.x + 1, labelBoxBound.y + 1, labelBoxBound.width - 2, labelBoxBound.height - 2, GetColor(style[SPINNER_BASE_COLOR_NORMAL]));

            //DrawText(FormatText("%i", value), labelBoxBound.x + (labelBoxBound.width/2 - textWidth/2), labelBoxBound.y + (labelBoxBound.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_TEXT_COLOR_PRESSED]));
        } break;
        default: break;
    }

    return value;
}

// Text Box element, returns input text
// NOTE: Requires static variables: framesCounter
RAYGUIDEF char *GuiTextBox(Rectangle bounds, char *text)
{
    static int framesCounter = 0;         // Required for blinking cursor
    
    #define MAX_CHARS_LENGTH       20
    #define KEY_BACKSPACE_TEXT    259     // GLFW BACKSPACE: 3 + 256

    Vector2 mousePoint = GetMousePosition();
    int initPos = bounds.x + 4;
    int letter = -1;

    // Update control
    //--------------------------------------------------------------------
    framesCounter++;

    letter = GetKeyPressed();

    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        if (letter != -1)
        {
            if (letter == KEY_BACKSPACE_TEXT)
            {
                for (int i = 0; i < MAX_CHARS_LENGTH; i++)
                {
                    if ((text[i] == '\0') && (i > 0))
                    {
                        text[i - 1] = '\0';
                        break;
                    }
                }

                text[MAX_CHARS_LENGTH - 1] = '\0';
            }
            else
            {
                if ((letter >= 32) && (letter < 127))
                {
                    for (int i = 0; i < MAX_CHARS_LENGTH; i++)
                    {
                        if (text[i] == '\0')
                        {
                            text[i] = (char)letter;
                            break;
                        }
                    }
                }
            }
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (CheckCollisionPointRec(mousePoint, bounds)) DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_PRESSED]));
    else DrawRectangleRec(bounds, GetColor(style[TEXTBOX_BORDER_COLOR_NORMAL]));

    DrawRectangle(bounds.x + style[TEXTBOX_BORDER_WIDTH], bounds.y + style[TEXTBOX_BORDER_WIDTH], bounds.width - (style[TEXTBOX_BORDER_WIDTH]*2), bounds.height - (style[TEXTBOX_BORDER_WIDTH]*2), GetColor(style[TEXTBOX_BASE_COLOR_NORMAL]));

    DrawText(text, initPos, bounds.y + style[DEFAULT_TEXT_SIZE], style[DEFAULT_TEXT_SIZE], GetColor(style[TEXTBOX_TEXT_COLOR_NORMAL]));

    if ((framesCounter/20)%2 && CheckCollisionPointRec(mousePoint, bounds)) DrawRectangle(initPos + MeasureText(text, style[DEFAULT_TEXT_SIZE]) + 2, bounds.y + 5, 1, 20, GetColor(style[TEXTBOX_BORDER_COLOR_NORMAL]));
    //--------------------------------------------------------------------

    return text;
}

RAYGUIDEF void GuiBeginPanel(Rectangle rec)
{
    //offset = (Vector2){ rec.x, rec.y };
    
    // TODO: Limit drawing to panel limits?
}

RAYGUIDEF void GuiEndPanel()
{
    //offset = (Vector2){ 0.0f, 0.0f };
}

#if !defined(RAYGUI_NO_STYLE_SAVE_LOAD)
// Save current GUI style into a text file
RAYGUIDEF void SaveGuiStyle(const char *fileName)
{
    FILE *styleFile = fopen(fileName, "wt");

    for (int i = 0; i < NUM_PROPERTIES; i++) fprintf(styleFile, "%-40s0x%x\n", guiPropertyName[i], GetStyleProperty(i));

    fclose(styleFile);
}

// Load GUI style from a text file
RAYGUIDEF void LoadGuiStyle(const char *fileName)
{
    #define MAX_STYLE_PROPERTIES    128

    typedef struct {
        char id[64];
        int value;
    } StyleProperty;

    StyleProperty *styleProp = (StyleProperty *)RAYGUI_MALLOC(MAX_STYLE_PROPERTIES*sizeof(StyleProperty));;
    int counter = 0;

    FILE *styleFile = fopen(fileName, "rt");
    
    if (styleFile != NULL)
    {
        while (!feof(styleFile))
        {
            fscanf(styleFile, "%s %i\n", styleProp[counter].id, &styleProp[counter].value);
            counter++;
        }

        fclose(styleFile);

        for (int i = 0; i < counter; i++)
        {
            for (int j = 0; j < NUM_PROPERTIES; j++)
            {
                if (strcmp(styleProp[i].id, guiPropertyName[j]) == 0)
                {
                    // Assign correct property to style
                    style[j] = styleProp[i].value;
                }
            }
        }
    }

    RAYGUI_FREE(styleProp);
}

// Set one style property value
RAYGUIDEF void SetStyleProperty(int guiProperty, int value) { style[guiProperty] = value; }

// Get one style property value
RAYGUIDEF int GetStyleProperty(int guiProperty) { return style[guiProperty]; }

#endif  // !defined(RAYGUI_NO_STYLE_SAVE_LOAD)

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
#if defined (RAYGUI_STANDALONE)
// Returns a Color struct from hexadecimal value
static Color GetColor(int hexValue)
{
    Color color;

    color.r = (unsigned char)(hexValue >> 24) & 0xFF;
    color.g = (unsigned char)(hexValue >> 16) & 0xFF;
    color.b = (unsigned char)(hexValue >> 8) & 0xFF;
    color.a = (unsigned char)hexValue & 0xFF;

    return color;
}

// Returns hexadecimal value for a Color
static int GetHexValue(Color color)
{
    return (((int)color.r << 24) | ((int)color.g << 16) | ((int)color.b << 8) | (int)color.a);
}

// Check if point is inside rectangle
static bool CheckCollisionPointRec(Vector2 point, Rectangle rec)
{
    bool collision = false;

    if ((point.x >= rec.x) && (point.x <= (rec.x + rec.width)) && 
        (point.y >= rec.y) && (point.y <= (rec.y + rec.height))) collision = true;

    return collision;
}

// Formatting of text with variables to 'embed'
static const char *FormatText(const char *text, ...)
{
    #define MAX_FORMATTEXT_LENGTH   64

    static char buffer[MAX_FORMATTEXT_LENGTH];

    va_list args;
    va_start(args, text);
    vsprintf(buffer, text, args);
    va_end(args);

    return buffer;
}
#endif

#endif // RAYGUI_IMPLEMENTATION
