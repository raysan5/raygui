/*******************************************************************************************
*
*   raygui v1.5 - A simple and easy-to-use IMGUI (Immedite Mode GUI) library for raylib (www.raylib.com)
*
*   DESCRIPTION:
*
*   raygui is a library for creating simple IMGUI interfaces using raylib. Basic controls provided:
*
*       - Label
*       - Button
*       - ToggleButton
*       - ToggleGroup
*       - CheckBox
*       - ComboBox
*       - Slider
*       - SliderBar
*       - ProgressBar
*       - Spinner
*       - TextBox
*       - Texture
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
*   #define RAYGUI_STYLE_SAVE_LOAD
*       Include style customization and save/load functions, useful when required.
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
*       1.5 (21-Jun-2017) Working in an improved styles system
*       1.4 (15-Jun-2017) Rewritten all GUI functions (removed useless ones)
*       1.3 (12-Jun-2017) Redesigned styles system
*       1.1 (01-Jun-2017) Complete review of the library
*       1.0 (07-Jun-2016) Converted to header-only by Ramon Santamaria.
*       0.9 (07-Mar-2016) Reviewed and tested by Albert Martos, Ian Eito, Sergio Martinez and Ramon Santamaria.
*       0.8 (27-Aug-2015) Initial release. Implemented by Kevin Gato, Daniel Nicolás and Ramon Santamaria.
*
*   CONTRIBUTORS:
*       Kevin Gato:         Initial implementation of basic components (2014)
*       Daniel Nicolas:     Initial implementation of basic components (2014)
*       Albert Martos:      Review and testing of the library (2015)
*       Ian Eito:           Review and testing of the library (2015)
*       Sergio Martinez:    Review and testing of the library (2015)
*       Ramon Santamaria:   Supervision, review, update and maintenance... and 2017 redesign
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

// Gui properties enumeration
typedef enum GuiProperty {
    // Label
    LABEL_TEXT_COLOR_NORMAL,
    LABEL_TEXT_COLOR_FOCUSED,
    LABEL_TEXT_COLOR_PRESSED,
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
    // Slider
    SLIDER_BORDER_WIDTH,
    SLIDER_BORDER_COLOR_NORMAL,
    SLIDER_BASE_COLOR_NORMAL,
    SLIDER_BORDER_COLOR_FOCUSED,
    SLIDER_BASE_COLOR_FOCUSED,
    SLIDER_BORDER_COLOR_PRESSED,
    SLIDER_BASE_COLOR_PRESSED,
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
    PROGRESSBAR_BORDER_COLOR_FOCUSED,
    PROGRESSBAR_BASE_COLOR_FOCUSED,
    // Spinner
    SPINNER_BUTTON_PADDING,
    SPINNER_BORDER_COLOR_NORMAL,
    SPINNER_BASE_COLOR_NORMAL,
    SPINNER_TEXT_COLOR_NORMAL,
    SPINNER_BORDER_COLOR_FOCUSED,
    SPINNER_BASE_COLOR_FOCUSED,
    SPINNER_TEXT_COLOR_FOCUSED,
    SPINNER_BORDER_COLOR_PRESSED,
    SPINNER_BASE_COLOR_PRESSED,
    SPINNER_TEXT_COLOR_PRESSED,
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
    CHECKBOX_INNER_PADDING,
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
RAYGUIDEF Color GuiTextColor(void);                                                                           // Get text color for normal state
RAYGUIDEF void GuiLabel(Rectangle bounds, const char *text);                                                  // Label control, show text
RAYGUIDEF bool GuiButton(Rectangle bounds, const char *text);                                                 // Button control, returns true when clicked
RAYGUIDEF bool GuiToggleButton(Rectangle bounds, const char *text, bool toggle);                              // Toggle Button control, returns true when active
RAYGUIDEF int GuiToggleGroup(Rectangle bounds, int toggleNum, char **toggleText, int toggleActive);           // Toggle Group control, returns toggled button index
RAYGUIDEF int GuiComboBox(Rectangle bounds, int comboNum, char **comboText, int comboActive);                 // Combo Box control, returns selected item index
RAYGUIDEF bool GuiCheckBox(Rectangle bounds, bool checked);                                                   // Check Box control, returns true when active
RAYGUIDEF float GuiSlider(Rectangle bounds, float value, float minValue, float maxValue);                     // Slider control, returns selected value
RAYGUIDEF float GuiSliderBar(Rectangle bounds, float value, float minValue, float maxValue);                  // Slider Bar control, returns selected value
RAYGUIDEF float GuiProgressBar(Rectangle bounds, float value, float minValue, float maxValue);                // Progress Bar control, shows current progress value
RAYGUIDEF int GuiSpinner(Rectangle bounds, int value, int minValue, int maxValue);                            // Spinner control, returns selected value
RAYGUIDEF void GuiTextBox(Rectangle bounds, char *text, int textSize);                                        // Text Box control, updates input text
RAYGUIDEF void GuiGroupBox(Rectangle bounds, const char *text);                                               // Group Box control with title name
RAYGUIDEF Color GuiColorPicker(Rectangle bounds, Color color);                                                // Color Picker control

#if defined RAYGUI_STANDALONE
// NOTE: raygui depend on some raylib input and drawing functions
// TODO: To use raygui as standalone library, those functions must be called per frame

//RAYGUIDEF void UpdateMouse(bool isMouseDown, bool isMouseUp, Vector2 mousePosition);
//RAYGUIDEF void UpdateKeys(int keyPressed);
#endif

#if defined(RAYGUI_STYLE_SAVE_LOAD)
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
#if !defined(RAYGUI_MALLOC) && defined(RAYGUI_STYLE_SAVE_LOAD)
    #include <stdlib.h>     // Required for: malloc(), free() [Used only on LoadGuiStyle()]

    #define RAYGUI_MALLOC(size)  malloc(size)
    #define RAYGUI_FREE(ptr)     free(ptr)
#endif

#if defined(RAYGUI_STYLE_SAVE_LOAD)
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

    #define DEFAULT_BACKGROUND_COLOR        0xf5f5f5ff
    #define DEFAULT_LINES_COLOR             0x90abb5ff
    #define DEFAULT_TEXT_FONT               0
    #define DEFAULT_TEXT_SIZE               10
    #define DEFAULT_BORDER_WIDTH            1

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

#elif defined(RAYGUI_STYLE_DEFAULT_DARK)

    #define DEFAULT_BACKGROUND_COLOR        0xf5f5f5ff
    #define DEFAULT_LINES_COLOR             0x90abb5ff
    #define DEFAULT_TEXT_FONT               0
    #define DEFAULT_TEXT_SIZE               10
    #define DEFAULT_BORDER_WIDTH            1

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

#endif

// Current GUI style (default light)
static int style[NUM_PROPERTIES] = {
    DEFAULT_TEXT_COLOR_NORMAL,          // LABEL_TEXT_COLOR_NORMAL,
    DEFAULT_TEXT_COLOR_FOCUSED,         // LABEL_TEXT_COLOR_FOCUSED,
    DEFAULT_TEXT_COLOR_PRESSED,         // LABEL_TEXT_COLOR_PRESSED,
    DEFAULT_BORDER_WIDTH*2,             // BUTTON_BORDER_WIDTH,
    DEFAULT_BORDER_COLOR_NORMAL,        // BUTTON_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,          // BUTTON_BASE_COLOR_NORMAL,
    DEFAULT_TEXT_COLOR_NORMAL,          // BUTTON_TEXT_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // BUTTON_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,         // BUTTON_BASE_COLOR_FOCUSED,
    DEFAULT_TEXT_COLOR_FOCUSED,         // BUTTON_TEXT_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,       // BUTTON_BORDER_COLOR_PRESSED,
    DEFAULT_BASE_COLOR_PRESSED,         // BUTTON_BASE_COLOR_PRESSED,
    DEFAULT_TEXT_COLOR_PRESSED,         // BUTTON_TEXT_COLOR_PRESSED,
    DEFAULT_BORDER_WIDTH,               // TOGGLE_BORDER_WIDTH,
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
    DEFAULT_BORDER_WIDTH,               // SLIDER_BORDER_WIDTH,
    DEFAULT_BORDER_COLOR_NORMAL,        // SLIDER_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,          // SLIDER_BASE_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // SLIDER_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,         // SLIDER_BASE_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,       // SLIDER_BORDER_COLOR_PRESSED,
    DEFAULT_BASE_COLOR_PRESSED,         // SLIDER_BASE_COLOR_PRESSED,
    DEFAULT_BORDER_WIDTH,               // SLIDERBAR_BORDER_WIDTH,
    DEFAULT_BORDER_COLOR_NORMAL,        // SLIDERBAR_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,          // SLIDERBAR_BASE_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // SLIDERBAR_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,         // SLIDERBAR_BASE_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,       // SLIDERBAR_BORDER_COLOR_PRESSED,
    DEFAULT_BASE_COLOR_PRESSED,         // SLIDERBAR_BASE_COLOR_PRESSED,
    DEFAULT_BORDER_WIDTH,               // PROGRESSBAR_BORDER_WIDTH,
    DEFAULT_BORDER_COLOR_NORMAL,        // PROGRESSBAR_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_FOCUSED,         // PROGRESSBAR_BASE_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // PROGRESSBAR_BORDER_COLOR_FOCUSED
    DEFAULT_BASE_COLOR_PRESSED,         // PROGRESSBAR_BASE_COLOR_FOCUSED,
    2,                                  // SPINNER_BUTTON_PADDING
    DEFAULT_BORDER_COLOR_NORMAL,        // SPINNER_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,          // SPINNER_BASE_COLOR_NORMAL,
    DEFAULT_TEXT_COLOR_NORMAL,          // SPINNER_TEXT_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // SPINNER_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,         // SPINNER_BASE_COLOR_FOCUSED,
    DEFAULT_TEXT_COLOR_FOCUSED,         // SPINNER_TEXT_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,       // SPINNER_BORDER_COLOR_PRESSED,
    DEFAULT_BASE_COLOR_PRESSED,         // SPINNER_BASE_COLOR_PRESSED,
    DEFAULT_TEXT_COLOR_PRESSED,         // SPINNER_TEXT_COLOR_PRESSED,
    DEFAULT_BORDER_WIDTH,               // COMBOBOX_BORDER_WIDTH,
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
    DEFAULT_BORDER_WIDTH,               // CHECKBOX_BORDER_WIDTH,
    1,                                  // CHECKBOX_INNER_PADDING,
    DEFAULT_BORDER_COLOR_NORMAL,        // CHECKBOX_BORDER_COLOR_NORMAL,
    DEFAULT_BACKGROUND_COLOR,           // CHECKBOX_BASE_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // CHECKBOX_BORDER_COLOR_FOCUSED,
    DEFAULT_TEXT_COLOR_FOCUSED,         // CHECKBOX_BASE_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,       // CHECKBOX_BORDER_COLOR_PRESSED,
    DEFAULT_TEXT_COLOR_PRESSED,         // CHECKBOX_BASE_COLOR_PRESSED,
    DEFAULT_BORDER_WIDTH,               // TEXTBOX_BORDER_WIDTH,
    DEFAULT_BORDER_COLOR_NORMAL,        // TEXTBOX_BORDER_COLOR_NORMAL,
    DEFAULT_BACKGROUND_COLOR,           // TEXTBOX_BASE_COLOR_NORMAL,
    DEFAULT_TEXT_COLOR_NORMAL,          // TEXTBOX_TEXT_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,       // TEXTBOX_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,         // TEXTBOX_BASE_COLOR_FOCUSED,
    DEFAULT_TEXT_COLOR_FOCUSED,         // TEXTBOX_TEXT_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,       // TEXTBOX_BORDER_COLOR_PRESSED,
    DEFAULT_BASE_COLOR_PRESSED,         // TEXTBOX_BASE_COLOR_PRESSED,
    DEFAULT_TEXT_COLOR_PRESSED,         // TEXTBOX_TEXT_COLOR_PRESSED,
};

#if defined(RAYGUI_STYLE_SAVE_LOAD)
// GUI property names (to read/write style text files)
static const char *guiPropertyName[] = {
    "LABEL_TEXT_COLOR_NORMAL",
    "LABEL_TEXT_COLOR_FOCUSED",
    "LABEL_TEXT_COLOR_PRESSED",
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
    "TOGGLEGROUP_PADDING",
    "SLIDER_BORDER_WIDTH",
    "SLIDER_BORDER_COLOR_NORMAL",
    "SLIDER_BASE_COLOR_NORMAL",
    "SLIDER_BORDER_COLOR_FOCUSED",
    "SLIDER_BASE_COLOR_FOCUSED",
    "SLIDER_BORDER_COLOR_PRESSED",
    "SLIDER_BASE_COLOR_PRESSED",
    "SLIDERBAR_BORDER_WIDTH",
    "SLIDERBAR_BORDER_COLOR_NORMAL",
    "SLIDERBAR_BASE_COLOR_NORMAL",
    "SLIDERBAR_BORDER_COLOR_FOCUSED",
    "SLIDERBAR_BASE_COLOR_FOCUSED",
    "SLIDERBAR_BORDER_COLOR_PRESSED",
    "SLIDERBAR_BASE_COLOR_PRESSED",
    "PROGRESSBAR_BORDER_WIDTH",
    "PROGRESSBAR_BORDER_COLOR_NORMAL",
    "PROGRESSBAR_BASE_COLOR_NORMAL",
    "PROGRESSBAR_BORDER_COLOR_FOCUSED",
    "PROGRESSBAR_BASE_COLOR_FOCUSED",
    "SPINNER_BUTTON_PADDING",
    "SPINNER_BORDER_COLOR_NORMAL",
    "SPINNER_BASE_COLOR_NORMAL",
    "SPINNER_TEXT_COLOR_NORMAL",
    "SPINNER_BORDER_COLOR_FOCUSED",
    "SPINNER_BASE_COLOR_FOCUSED",
    "SPINNER_TEXT_COLOR_FOCUSED",
    "SPINNER_BORDER_COLOR_PRESSED",
    "SPINNER_BASE_COLOR_PRESSED",
    "SPINNER_TEXT_COLOR_PRESSED",
    "COMBOBOX_BORDER_WIDTH",
    "COMBOBOX_BUTTON_PADDING",
    "COMBOBOX_BORDER_COLOR_NORMAL",
    "COMBOBOX_BASE_COLOR_NORMAL",
    "COMBOBOX_TEXT_COLOR_NORMAL",
    "COMBOBOX_BORDER_COLOR_FOCUSED",
    "COMBOBOX_BASE_COLOR_FOCUSED",
    "COMBOBOX_TEXT_COLOR_FOCUSED",
    "COMBOBOX_BORDER_COLOR_PRESSED",
    "COMBOBOX_BASE_COLOR_PRESSED",
    "COMBOBOX_TEXT_COLOR_PRESSED",
    "CHECKBOX_BORDER_WIDTH",
    "CHECKBOX_INNER_PADDING",
    "CHECKBOX_BORDER_COLOR_NORMAL",
    "CHECKBOX_BASE_COLOR_NORMAL",
    "CHECKBOX_BORDER_COLOR_FOCUSED",
    "CHECKBOX_BASE_COLOR_FOCUSED",
    "CHECKBOX_BORDER_COLOR_PRESSED",
    "CHECKBOX_BASE_COLOR_PRESSED",
    "TEXTBOX_BORDER_WIDTH",
    "TEXTBOX_BORDER_COLOR_NORMAL",
    "TEXTBOX_BASE_COLOR_NORMAL",
    "TEXTBOX_TEXT_COLOR_NORMAL",
    "TEXTBOX_BORDER_COLOR_FOCUSED",
    "TEXTBOX_BASE_COLOR_FOCUSED",
    "TEXTBOX_TEXT_COLOR_FOCUSED",
    "TEXTBOX_BORDER_COLOR_PRESSED",
    "TEXTBOX_BASE_COLOR_PRESSED",
    "TEXTBOX_TEXT_COLOR_PRESSED"
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

// Drawing related functions
static int MeasureText(const char *text, int fontSize) { /* TODO */ return 0; }
static void DrawText(const char *text, int posX, int posY, int fontSize, Color color) { /* TODO */ }
static void DrawRectangleRecT(Rectangle rec, Color color) { /* TODO */ }
#endif

// Draw rectangle from Rectangle data (using default dont texture)
static void DrawRectangleRecT(Rectangle rec, Color color) { DrawRectangleT(rec.x, rec.y, rec.width, rec.height, color); }

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Get background color
RAYGUIDEF Color GuiBackgroundColor(void) { return GetColor(DEFAULT_BACKGROUND_COLOR); }

// Get lines color
RAYGUIDEF Color GuiLinesColor(void) { return GetColor(DEFAULT_LINES_COLOR); }

// Get text color for normal state
RAYGUIDEF Color GuiTextColor(void) { return GetColor(DEFAULT_TEXT_COLOR_NORMAL); }

// Label control
RAYGUIDEF void GuiLabel(Rectangle bounds, const char *text)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();
    
    // Update control
    //--------------------------------------------------------------------
    int textWidth = MeasureText(text, DEFAULT_TEXT_SIZE);
    int textHeight = DEFAULT_TEXT_SIZE;

    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;
    
    // Check label state
    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
        else state = FOCUSED;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL: DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, DEFAULT_TEXT_SIZE, GetColor(style[LABEL_TEXT_COLOR_NORMAL])); break;
        case FOCUSED: DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, DEFAULT_TEXT_SIZE, GetColor(style[LABEL_TEXT_COLOR_FOCUSED])); break;
        case PRESSED: DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, DEFAULT_TEXT_SIZE, GetColor(style[LABEL_TEXT_COLOR_PRESSED])); break;
        default: break;
    }
    //--------------------------------------------------------------------
}

// Button control, returns true when clicked
RAYGUIDEF bool GuiButton(Rectangle bounds, const char *text)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();
    bool clicked = false;

    // Update control
    //--------------------------------------------------------------------
    int textWidth = MeasureText(text, DEFAULT_TEXT_SIZE);
    int textHeight = DEFAULT_TEXT_SIZE;
    
    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;

    // Check button state
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
            DrawRectangleRecT(bounds, GetColor(style[BUTTON_BORDER_COLOR_NORMAL]));
            DrawRectangleT(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_NORMAL]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, DEFAULT_TEXT_SIZE, GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[BUTTON_BORDER_COLOR_FOCUSED]));
            DrawRectangleT(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_FOCUSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, DEFAULT_TEXT_SIZE, GetColor(style[BUTTON_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[BUTTON_BORDER_COLOR_PRESSED]));
            DrawRectangleT(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_PRESSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, DEFAULT_TEXT_SIZE, GetColor(style[BUTTON_TEXT_COLOR_PRESSED]));
        } break;
        default: break;
    }
    //------------------------------------------------------------------

    if (clicked) return true;
    else return false;
}

// Toggle Button control, returns true when active
RAYGUIDEF bool GuiToggleButton(Rectangle bounds, const char *text, bool active)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();

    // Update control
    //--------------------------------------------------------------------
    int textWidth = MeasureText(text, DEFAULT_TEXT_SIZE);
    int textHeight = DEFAULT_TEXT_SIZE;
    
    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;

    // Check toggle button state
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
                DrawRectangleRecT(bounds, GetColor(style[TOGGLE_BORDER_COLOR_PRESSED]));
                DrawRectangleT(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_PRESSED]));
                DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, DEFAULT_TEXT_SIZE, GetColor(style[TOGGLE_TEXT_COLOR_PRESSED]));
            }
            else
            {
                DrawRectangleRecT(bounds, GetColor(style[TOGGLE_BORDER_COLOR_NORMAL]));
                DrawRectangleT(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_NORMAL]));
                DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, DEFAULT_TEXT_SIZE, GetColor(style[TOGGLE_TEXT_COLOR_NORMAL]));
            }
        } break;
        case FOCUSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[TOGGLE_BORDER_COLOR_FOCUSED]));
            DrawRectangleT(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_FOCUSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, DEFAULT_TEXT_SIZE, GetColor(style[TOGGLE_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[TOGGLE_BORDER_COLOR_PRESSED]));
            DrawRectangleT(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_PRESSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, DEFAULT_TEXT_SIZE, GetColor(style[TOGGLE_TEXT_COLOR_PRESSED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------

    return active;
}

// Toggle Group control, returns toggled button index
RAYGUIDEF int GuiToggleGroup(Rectangle bounds, int toggleCount, char **toggleText, int active)
{
    for (int i = 0; i < toggleCount; i++)
    {
        if (i == active) GuiToggleButton((Rectangle){bounds.x + i*(bounds.width + style[TOGGLEGROUP_PADDING]),bounds.y,bounds.width,bounds.height}, toggleText[i], true);
        else if (GuiToggleButton((Rectangle){bounds.x + i*(bounds.width + style[TOGGLEGROUP_PADDING]),bounds.y,bounds.width,bounds.height}, toggleText[i], false) == true) active = i;
    }

    return active;
}

// Check Box control, returns true when active
RAYGUIDEF bool GuiCheckBox(Rectangle bounds, bool checked)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();

    // Update control
    //--------------------------------------------------------------------
    
    // Check checkbox state
    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) checked = !checked;
        else state = FOCUSED;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRecT(bounds, GetColor(style[CHECKBOX_BORDER_COLOR_NORMAL]));
            DrawRectangleT(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[CHECKBOX_BASE_COLOR_NORMAL]));
            if (checked) DrawRectangleT(bounds.x + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING],
                                       bounds.y + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING], 
                                       bounds.width - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), 
                                       bounds.height - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), GetColor(style[CHECKBOX_BASE_COLOR_PRESSED]));        
        } break;
        case FOCUSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[CHECKBOX_BORDER_COLOR_FOCUSED]));
            DrawRectangleT(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[CHECKBOX_BASE_COLOR_NORMAL]));
            if (checked) DrawRectangleT(bounds.x + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING],
                                       bounds.y + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING], 
                                       bounds.width - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), 
                                       bounds.height - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), GetColor(style[CHECKBOX_BASE_COLOR_FOCUSED]));           
        } break;
        case PRESSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[CHECKBOX_BORDER_COLOR_PRESSED]));
            DrawRectangleT(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[CHECKBOX_BASE_COLOR_NORMAL]));
            if (checked) DrawRectangleT(bounds.x + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING],
                                       bounds.y + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING], 
                                       bounds.width - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), 
                                       bounds.height - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), GetColor(style[CHECKBOX_BASE_COLOR_PRESSED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------

    return checked;
}


// Combo Box control, returns selected item index
RAYGUIDEF int GuiComboBox(Rectangle bounds, int comboCount, char **comboText, int active)
{
    ControlState state = NORMAL;

    Vector2 mousePoint = GetMousePosition();

    #define COMBOBOX_SELECTOR_WIDTH     35
    
    bounds.width -= (COMBOBOX_SELECTOR_WIDTH + style[COMBOBOX_BUTTON_PADDING]);
    
    Rectangle selector = { bounds.x + bounds.width + style[COMBOBOX_BUTTON_PADDING], 
                           bounds.y, COMBOBOX_SELECTOR_WIDTH, bounds.height };

    // Update control
    //--------------------------------------------------------------------
    if (active < 0) active = 0;
    else if (active > comboCount - 1) active = comboCount - 1;
    
    int textWidth = MeasureText(comboText[active], DEFAULT_TEXT_SIZE);
    int textHeight = DEFAULT_TEXT_SIZE;

    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;

    if (CheckCollisionPointRec(mousePoint, bounds) || 
        CheckCollisionPointRec(mousePoint, selector))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) 
        {
            active += 1;
            if (active >= comboCount) active = 0;
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
            DrawRectangleRecT(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_NORMAL]));
            DrawRectangleT(bounds.x + style[COMBOBOX_BORDER_WIDTH], bounds.y + style[COMBOBOX_BORDER_WIDTH], bounds.width - 2*style[COMBOBOX_BORDER_WIDTH], bounds.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_NORMAL]));

            DrawRectangleRecT(selector, GetColor(style[COMBOBOX_BORDER_COLOR_NORMAL]));
            DrawRectangleT(selector.x + style[COMBOBOX_BORDER_WIDTH], selector.y + style[COMBOBOX_BORDER_WIDTH], selector.width - 2*style[COMBOBOX_BORDER_WIDTH], selector.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_NORMAL]));
            
            DrawText(comboText[active], bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - DEFAULT_TEXT_SIZE/2, DEFAULT_TEXT_SIZE, GetColor(style[COMBOBOX_TEXT_COLOR_NORMAL]));   
            DrawText(FormatText("%i/%i", active + 1, comboCount), 
                     selector.x + selector.width/2 - (MeasureText(FormatText("%i/%i", active + 1, comboCount), 
                     DEFAULT_TEXT_SIZE)/2), selector.y + selector.height/2 - DEFAULT_TEXT_SIZE/2, 
                     DEFAULT_TEXT_SIZE, GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_FOCUSED]));
            DrawRectangleT(bounds.x + style[COMBOBOX_BORDER_WIDTH], bounds.y + style[COMBOBOX_BORDER_WIDTH], bounds.width - 2*style[COMBOBOX_BORDER_WIDTH], bounds.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_FOCUSED]));

            DrawRectangleRecT(selector, GetColor(style[COMBOBOX_BORDER_COLOR_FOCUSED]));
            DrawRectangleT(selector.x + style[COMBOBOX_BORDER_WIDTH], selector.y + style[COMBOBOX_BORDER_WIDTH], selector.width - 2*style[COMBOBOX_BORDER_WIDTH], selector.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_FOCUSED]));
            
            DrawText(comboText[active], bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - DEFAULT_TEXT_SIZE/2, DEFAULT_TEXT_SIZE, GetColor(style[COMBOBOX_TEXT_COLOR_FOCUSED]));   
            DrawText(FormatText("%i/%i", active + 1, comboCount), 
                     selector.x + selector.width/2 - (MeasureText(FormatText("%i/%i", active + 1, comboCount), 
                     DEFAULT_TEXT_SIZE)/2), selector.y + selector.height/2 - DEFAULT_TEXT_SIZE/2, 
                     DEFAULT_TEXT_SIZE, GetColor(style[BUTTON_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_PRESSED]));
            DrawRectangleT(bounds.x + style[COMBOBOX_BORDER_WIDTH], bounds.y + style[COMBOBOX_BORDER_WIDTH], bounds.width - 2*style[COMBOBOX_BORDER_WIDTH], bounds.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_PRESSED]));

            DrawRectangleRecT(selector, GetColor(style[COMBOBOX_BORDER_COLOR_PRESSED]));
            DrawRectangleT(selector.x + style[COMBOBOX_BORDER_WIDTH], selector.y + style[COMBOBOX_BORDER_WIDTH], selector.width - 2*style[COMBOBOX_BORDER_WIDTH], selector.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_PRESSED]));
            
            DrawText(comboText[active], bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - DEFAULT_TEXT_SIZE/2, DEFAULT_TEXT_SIZE, GetColor(style[COMBOBOX_TEXT_COLOR_PRESSED]));   
            DrawText(FormatText("%i/%i", active + 1, comboCount), 
                     selector.x + selector.width/2 - (MeasureText(FormatText("%i/%i", active + 1, comboCount), 
                     DEFAULT_TEXT_SIZE)/2), selector.y + selector.height/2 - DEFAULT_TEXT_SIZE/2, 
                     DEFAULT_TEXT_SIZE, GetColor(style[BUTTON_TEXT_COLOR_PRESSED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------

    return active;
}

// Slider control, returns selected value
RAYGUIDEF float GuiSlider(Rectangle bounds, float value, float minValue, float maxValue)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();

    // Update control
    //--------------------------------------------------------------------
    if (value < minValue) value = minValue;
    else if (value > maxValue) value = maxValue;

    Rectangle slider = { bounds.x + (int)((value/(maxValue - minValue))*(bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH])) - 10,
                         bounds.y + style[SLIDERBAR_BORDER_WIDTH], // + style[SLIDER_INNER_PADDING];
                         20, bounds.height - 2*style[SLIDERBAR_BORDER_WIDTH] };// - 2*style[SLIDER_INNER_PADDING];

    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            state = PRESSED;
            
            // Get equivalent value from mousePoint.x
            value = (((maxValue - minValue)*(mousePoint.x - (float)bounds.x))/(float)bounds.width) + minValue;
            
            if (value > maxValue) value = maxValue;
            else if (value < minValue) value = minValue;

            slider.x = bounds.x + (int)((value/(maxValue - minValue))*(bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH])) - slider.width/2;
            
            //if (slider.x < (bounds.x + style[PROGRESSBAR_BORDER_WIDTH])) slider.x = bounds.x + style[PROGRESSBAR_BORDER_WIDTH];
            //else if ((slider.x + slider.width) > (bounds.x + bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH])) slider.x = (bounds.x + bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH]) - slider.width;
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
            DrawRectangleRecT(bounds, GetColor(style[SLIDER_BORDER_COLOR_NORMAL]));
            DrawRectangleT(bounds.x + style[SLIDER_BORDER_WIDTH], bounds.y + style[SLIDER_BORDER_WIDTH], bounds.width - 2*style[SLIDER_BORDER_WIDTH], bounds.height - 2*style[SLIDER_BORDER_WIDTH], GetColor(DEFAULT_BACKGROUND_COLOR));
            DrawRectangleRecT(slider, GetColor(style[SLIDER_BASE_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[SLIDER_BORDER_COLOR_FOCUSED]));
            DrawRectangleT(bounds.x + style[SLIDER_BORDER_WIDTH], bounds.y + style[SLIDER_BORDER_WIDTH], bounds.width - 2*style[SLIDER_BORDER_WIDTH], bounds.height - 2*style[SLIDER_BORDER_WIDTH], GetColor(DEFAULT_BACKGROUND_COLOR));
            DrawRectangleRecT(slider, GetColor(style[SLIDER_BASE_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[SLIDER_BORDER_COLOR_PRESSED]));
            DrawRectangleT(bounds.x + style[SLIDER_BORDER_WIDTH], bounds.y + style[SLIDER_BORDER_WIDTH], bounds.width - 2*style[SLIDER_BORDER_WIDTH], bounds.height - 2*style[SLIDER_BORDER_WIDTH], GetColor(DEFAULT_BACKGROUND_COLOR));
            DrawRectangleRecT(slider, GetColor(style[SLIDER_BASE_COLOR_PRESSED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------

    return value;
}

// Slider Bar control, returns selected value
// NOTE: If minimum value is <0 support multidirection
RAYGUIDEF float GuiSliderBar(Rectangle bounds, float value, float minValue, float maxValue)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();
    
    Rectangle slider = { bounds.x + style[PROGRESSBAR_BORDER_WIDTH], 
                         bounds.y + style[PROGRESSBAR_BORDER_WIDTH], // + style[PROGRESSBAR_INNER_PADDING] 
                         (int)((value/(maxValue - minValue))*(bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH])),
                         bounds.height - 2*style[PROGRESSBAR_BORDER_WIDTH] }; // - 2*style[PROGRESSBAR_INNER_PADDING]

    // Update control
    //--------------------------------------------------------------------
    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            state = PRESSED;
            
            // Get equivalent value from mousePoint.x
            value = (((maxValue - minValue)*(mousePoint.x - (float)bounds.x))/(float)bounds.width) + minValue;
            
            if (value > maxValue) value = maxValue;
            else if (value < minValue) value = minValue;

            slider.width = (int)((value/(maxValue - minValue))*(bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH]));
        }
        state = FOCUSED;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRecT(bounds, GetColor(style[SLIDERBAR_BORDER_COLOR_NORMAL]));
            DrawRectangleT(bounds.x + style[SLIDERBAR_BORDER_WIDTH], bounds.y + style[SLIDERBAR_BORDER_WIDTH], bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH], bounds.height - 2*style[SLIDERBAR_BORDER_WIDTH], GetColor(style[SLIDERBAR_BASE_COLOR_NORMAL]));
            DrawRectangleRecT(slider, GetColor(style[SLIDERBAR_BORDER_COLOR_PRESSED]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[SLIDERBAR_BORDER_COLOR_FOCUSED]));
            DrawRectangleT(bounds.x + style[SLIDERBAR_BORDER_WIDTH], bounds.y + style[SLIDERBAR_BORDER_WIDTH], bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH], bounds.height - 2*style[SLIDERBAR_BORDER_WIDTH], GetColor(style[SLIDERBAR_BASE_COLOR_FOCUSED]));
            DrawRectangleRecT(slider, GetColor(style[SLIDERBAR_BORDER_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[SLIDERBAR_BORDER_COLOR_PRESSED]));
            DrawRectangleT(bounds.x + style[SLIDERBAR_BORDER_WIDTH], bounds.y + style[SLIDERBAR_BORDER_WIDTH], bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH], bounds.height - 2*style[SLIDERBAR_BORDER_WIDTH], GetColor(style[SLIDERBAR_BASE_COLOR_PRESSED]));
            DrawRectangleRecT(slider, GetColor(style[SLIDERBAR_BORDER_COLOR_PRESSED])); 
        } break;
        default: break;
    }    
    //--------------------------------------------------------------------

    return value;
}

// Progress Bar control, shows current progress value
RAYGUIDEF float GuiProgressBar(Rectangle bounds, float value, float minValue, float maxValue)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();
    
    Rectangle progress = { bounds.x + style[PROGRESSBAR_BORDER_WIDTH], 
                           bounds.y + style[PROGRESSBAR_BORDER_WIDTH], // + style[PROGRESSBAR_INNER_PADDING] 
                           0, // TODO
                           bounds.height - 2*style[PROGRESSBAR_BORDER_WIDTH] }; // - 2*style[PROGRESSBAR_INNER_PADDING]

    // Update control
    //--------------------------------------------------------------------
    if (value > maxValue) value = maxValue;
    else if (value < minValue) value = minValue;
    
     progress.width = (int)(value/(maxValue - minValue)*(float)(bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH]));
        
    if (CheckCollisionPointRec(mousePoint, bounds)) state = FOCUSED;
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRecT(bounds, GetColor(style[PROGRESSBAR_BORDER_COLOR_NORMAL]));
            DrawRectangleT(bounds.x + style[PROGRESSBAR_BORDER_WIDTH], bounds.y + style[PROGRESSBAR_BORDER_WIDTH], bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH], bounds.height - 2*style[PROGRESSBAR_BORDER_WIDTH], GetColor(DEFAULT_BACKGROUND_COLOR));
            DrawRectangleRecT(progress, GetColor(style[PROGRESSBAR_BASE_COLOR_NORMAL])); 
        } break;
        case FOCUSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[SLIDERBAR_BORDER_COLOR_FOCUSED]));
            DrawRectangleT(bounds.x + style[SLIDERBAR_BORDER_WIDTH], bounds.y + style[SLIDERBAR_BORDER_WIDTH], bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH], bounds.height - 2*style[SLIDERBAR_BORDER_WIDTH], GetColor(DEFAULT_BACKGROUND_COLOR));
            DrawRectangleRecT(progress, GetColor(style[PROGRESSBAR_BASE_COLOR_FOCUSED]));
        } break;
        case PRESSED: break;
        default: break;
    }    
    //--------------------------------------------------------------------

    return value;
}

// Spinner control, returns selected value
// NOTE: Requires static variables: framesCounter, valueSpeed - ERROR!
RAYGUIDEF int GuiSpinner(Rectangle bounds, int value, int minValue, int maxValue)
{   
    ControlState state = NORMAL;
    
    #define SPINNER_LEFT_BUTTON      1
    #define SPINNER_RIGHT_BUTTON     2
    
    #define SPINNER_BUTTON_WIDTH    35
    
    Vector2 mousePoint = GetMousePosition();
    
    static int framesCounter = 0;
    static bool valueSpeed = false;;
    
    Rectangle spinner = { bounds.x + SPINNER_BUTTON_WIDTH + style[SPINNER_BUTTON_PADDING], bounds.y, bounds.width - 2*(SPINNER_BUTTON_WIDTH + style[SPINNER_BUTTON_PADDING]), bounds.height };
    Rectangle leftButtonBound = { bounds.x, bounds.y, SPINNER_BUTTON_WIDTH, bounds.height };
    Rectangle rightButtonBound = { bounds.x + bounds.width - SPINNER_BUTTON_WIDTH, bounds.y, SPINNER_BUTTON_WIDTH, bounds.height };

    int textWidth = MeasureText(FormatText("%i", value), DEFAULT_TEXT_SIZE);
    int textHeight = DEFAULT_TEXT_SIZE;

    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;

    // Update control
    //--------------------------------------------------------------------   
    if (CheckCollisionPointRec(mousePoint, leftButtonBound))
    {
        state = FOCUSED;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            state = PRESSED;
             
            if (!valueSpeed)
            {
                if (value > minValue) value--;
                valueSpeed = true;
            }
            else framesCounter++;

            if (value > minValue)
            {
                if (framesCounter >= 30) value -= 1;
            }
        }
    }
    else if (CheckCollisionPointRec(mousePoint, rightButtonBound))
    {
        state = FOCUSED;

        if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
        {
            state = PRESSED;
            
            if (!valueSpeed)
            {
                if (value < maxValue) value++;
                valueSpeed = true;
            }
            else framesCounter++;

            if (value < maxValue)
            {
                if (framesCounter >= 30) value += 1;
            }
       }
    }

    if (IsMouseButtonUp(MOUSE_LEFT_BUTTON))
    {
        valueSpeed = false;
        framesCounter = 0;
    }
    
    /*
    if (CheckCollisionPointRec(mousePoint, leftButtonBound) || 
        CheckCollisionPointRec(mousePoint, rightButtonBound) || 
        CheckCollisionPointRec(mousePoint, spinner))
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
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (GuiButton(leftButtonBound, "-")) 
    {
        /*
        if (!valueSpeed)
        {
            if (value > minValue) value--;
            valueSpeed = true;
        }
        else framesCounter++;

        if (value > minValue)
        {
            if (framesCounter >= 30) value -= 1;
        }
        */
    }
    else if (GuiButton(rightButtonBound, "+")) { }

    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRecT(spinner, GetColor(style[SPINNER_BORDER_COLOR_NORMAL]));
            DrawRectangleT(spinner.x + 1, spinner.y + 1, spinner.width - 2, spinner.height - 2, GetColor(style[SPINNER_BASE_COLOR_NORMAL]));

            DrawText(FormatText("%i", value), spinner.x + (spinner.width/2 - textWidth/2), spinner.y + (spinner.height/2 - (DEFAULT_TEXT_SIZE/2)), DEFAULT_TEXT_SIZE, GetColor(style[SPINNER_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRecT(spinner, GetColor(style[SPINNER_BORDER_COLOR_FOCUSED]));
            DrawRectangleT(spinner.x + 1, spinner.y + 1, spinner.width - 2, spinner.height - 2, GetColor(style[SPINNER_BASE_COLOR_FOCUSED]));

            DrawText(FormatText("%i", value), spinner.x + (spinner.width/2 - textWidth/2), spinner.y + (spinner.height/2 - (DEFAULT_TEXT_SIZE/2)), DEFAULT_TEXT_SIZE, GetColor(style[SPINNER_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRecT(spinner, GetColor(style[SPINNER_BORDER_COLOR_PRESSED]));
            DrawRectangleT(spinner.x + 1, spinner.y + 1, spinner.width - 2, spinner.height - 2, GetColor(style[SPINNER_BASE_COLOR_PRESSED]));

            DrawText(FormatText("%i", value), spinner.x + (spinner.width/2 - textWidth/2), spinner.y + (spinner.height/2 - (DEFAULT_TEXT_SIZE/2)), DEFAULT_TEXT_SIZE, GetColor(style[SPINNER_TEXT_COLOR_PRESSED]));
        } break;
        default: break;
    }

    return value;
}

// Text Box control, updates input text
// NOTE: Requires static variables: framesCounter
RAYGUIDEF void GuiTextBox(Rectangle bounds, char *text, int textSize)
{
    static int framesCounter = 0;         // Required for blinking cursor

    #define KEY_BACKSPACE_TEXT    259     // GLFW BACKSPACE: 3 + 256
    
    ControlState state = NORMAL;

    Vector2 mousePoint = GetMousePosition();

    // Update control
    //--------------------------------------------------------------------
    if (CheckCollisionPointRec(mousePoint, bounds))
    {
        state = FOCUSED;        // NOTE: PRESSED state is not used on this control
        
        framesCounter++;
        
        int letter = -1;
        letter = GetKeyPressed();
        
        if (letter != -1)
        {
            if (letter == KEY_BACKSPACE_TEXT)
            {
                for (int i = 0; i < textSize; i++)
                {
                    if ((text[i] == '\0') && (i > 0))
                    {
                        text[i - 1] = '\0';
                        break;
                    }
                }

                text[textSize - 1] = '\0';
            }
            else
            {
                if ((letter >= 32) && (letter < 127))
                {
                    for (int i = 0; i < textSize; i++)
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
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRecT(bounds, GetColor(style[TEXTBOX_BORDER_COLOR_NORMAL]));
            DrawRectangleT(bounds.x + style[TEXTBOX_BORDER_WIDTH], bounds.y + style[TEXTBOX_BORDER_WIDTH], bounds.width - 2*style[TEXTBOX_BORDER_WIDTH], bounds.height - 2*style[TEXTBOX_BORDER_WIDTH], GetColor(style[TEXTBOX_BASE_COLOR_NORMAL]));
            DrawText(text, bounds.x + 4, bounds.y + style[TEXTBOX_BORDER_WIDTH] + bounds.height/2 - DEFAULT_TEXT_SIZE/2, DEFAULT_TEXT_SIZE, GetColor(style[TEXTBOX_TEXT_COLOR_NORMAL]));
            
        } break;
        case FOCUSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[TOGGLE_BORDER_COLOR_FOCUSED]));
            DrawRectangleT(bounds.x + style[TEXTBOX_BORDER_WIDTH], bounds.y + style[TEXTBOX_BORDER_WIDTH], bounds.width - 2*style[TEXTBOX_BORDER_WIDTH], bounds.height - 2*style[TEXTBOX_BORDER_WIDTH], GetColor(style[TEXTBOX_BASE_COLOR_FOCUSED]));
            DrawText(text, bounds.x + 4, bounds.y + style[TEXTBOX_BORDER_WIDTH] + bounds.height/2 - DEFAULT_TEXT_SIZE/2, DEFAULT_TEXT_SIZE, GetColor(style[TEXTBOX_TEXT_COLOR_PRESSED]));

            if ((framesCounter/20)%2 == 0) DrawRectangleT(bounds.x + 4 + MeasureText(text, DEFAULT_TEXT_SIZE), bounds.y + 2, 1, bounds.height - 4, GetColor(style[TEXTBOX_BORDER_COLOR_FOCUSED]));

        } break;
        case PRESSED: break; // NOTE: PRESSED state is not used on this control
        default: break;
    }
    //--------------------------------------------------------------------
}

// Texture button control, returns true when clicked
RAYGUIDEF bool GuiTexture(Rectangle bounds, Texture2D texture)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();
    bool clicked = false;

    // Update control
    //--------------------------------------------------------------------
    if (CheckCollisionPointRec(mousePoint, bounds))     // Check button state
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
            DrawRectangleRecT(bounds, GetColor(style[BUTTON_BORDER_COLOR_NORMAL]));
            DrawRectangleT(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_NORMAL]));
            DrawTexture(texture, bounds.x, bounds.y, WHITE);
        } break;
        case FOCUSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[BUTTON_BORDER_COLOR_FOCUSED]));
            DrawRectangleT(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_FOCUSED]));
            DrawTexture(texture, bounds.x, bounds.y, GetColor(style[BUTTON_BASE_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRecT(bounds, GetColor(style[BUTTON_BORDER_COLOR_PRESSED]));
            DrawRectangleT(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_PRESSED]));
            DrawTexture(texture, bounds.x, bounds.y, GetColor(style[BUTTON_BASE_COLOR_PRESSED]));
        } break;
        default: break;
    }
    //------------------------------------------------------------------

    if (clicked) return true;
    else return false;
}

// Group Box control with title name
void GuiGroupBox(Rectangle bounds, const char *text)
{
    DrawLineEx((Vector2){ bounds.x + 1, bounds.y }, (Vector2){ bounds.x, bounds.y + bounds.height }, 1, GuiLinesColor());
    DrawLineEx((Vector2){ bounds.x, bounds.y + bounds.height }, (Vector2){ bounds.x + bounds.width, bounds.y + bounds.height }, 1, GuiLinesColor());
    DrawLineEx((Vector2){ bounds.x + bounds.width, bounds.y + bounds.height }, (Vector2){ bounds.x + bounds.width, bounds.y }, 1, GuiLinesColor());
    DrawLineEx((Vector2){ bounds.x, bounds.y }, (Vector2){ bounds.x + 10, bounds.y }, 1, GuiLinesColor());
    DrawLineEx((Vector2){ bounds.x + bounds.width, bounds.y }, (Vector2){ bounds.x + 20 + MeasureText(text, 10), bounds.y }, 1, GuiLinesColor());
    
    DrawText(text, bounds.x + 14, bounds.y - 5, 10, GuiTextColor());
}

// Color Picker control
// TODO: It can be divided in multiple controls:
//      Color GuiColorPicker() 
//      unsigned char GuiColorBarAlpha() 
//      unsigned char GuiColorBarHue()
//      Color GuiColorBarSat() [WHITE->color]
//      Color GuiColorBarValue() [BLACK->color]), HSV / HSL
//      unsigned char GuiColorBarLuminance() [BLACK->WHITE]
Color GuiColorPicker(Rectangle bounds, Color color)
{
    ControlState state = NORMAL;
    
    Vector2 mousePoint = GetMousePosition();
    
    // NOTE: bounds define only the color picker box, extra bars at right and bottom

    // Update control
    //--------------------------------------------------------------------
    
    // TODO: Check mousePoint over colorSelector and right/bottom bars
    
    if (CheckCollisionPointRec(mousePoint, bounds))     // Check button state
    {
        
    }
    //--------------------------------------------------------------------
    
    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleGradientEx(bounds, WHITE, WHITE, BLUE, BLUE);
    DrawRectangleGradientEx(bounds, Fade(BLACK,0), BLACK, BLACK, Fade(BLACK,0));

    DrawRectangleGradientEx((Rectangle){bounds.x + bounds.width + 10, bounds.y, 20, bounds.height/6}, (Color){255,0,0,255}, (Color){255,255,0,255}, (Color){255,255,0,255}, (Color){255,0,0,255}); // TEST
    DrawRectangleGradientEx((Rectangle){bounds.x + bounds.width + 10, bounds.y + bounds.height/6, 20, bounds.height/6}, (Color){255,255,0,255}, (Color){0,255,0,255}, (Color){0,255,0,255}, (Color){255,255,0,255}); // TEST
    DrawRectangleGradientEx((Rectangle){bounds.x + bounds.width + 10, bounds.y + 2*(bounds.height/6), 20, bounds.height/6}, (Color){0,255,0,255}, (Color){0,255,255,255}, (Color){0,255,255,255}, (Color){0,255,0,255}); // TEST
    DrawRectangleGradientEx((Rectangle){bounds.x + bounds.width + 10, bounds.y + 3*(bounds.height/6), 20, bounds.height/6}, (Color){0,255,255,255}, (Color){0,0,255,255}, (Color){0,0,255,255}, (Color){0,255,255,255}); // TEST
    DrawRectangleGradientEx((Rectangle){bounds.x + bounds.width + 10, bounds.y + 4*(bounds.height/6), 20, bounds.height/6}, (Color){0,0,255,255}, (Color){255,0,255,255}, (Color){255,0,255,255}, (Color){0,0,255,255}); // TEST
    DrawRectangleGradientEx((Rectangle){bounds.x + bounds.width + 10, bounds.y + 5*(bounds.height/6), 20, bounds.height/6}, (Color){255,0,255,255}, (Color){255,0,0,255}, (Color){255,0,0,255}, (Color){255,0,255,255}); // TEST

    // Draw checked background
    for (int i = 0; i < 38; i++) DrawRectangle(bounds.x + 10*(i%19), bounds.x + bounds.width + 10 + 10*(i/19), bounds.width/20, bounds.width/20, (i%2) ? GRAY : LIGHTGRAY);

    DrawRectangleGradientEx((Rectangle){bounds.x, bounds.x + bounds.width + 10, bounds.width, 20}, Fade(WHITE, 0), Fade(WHITE, 0), BLUE, BLUE);

    DrawRectangle(bounds.x + bounds.width + 6, 120, 28, 5, WHITE);
    DrawRectangleLines(bounds.x + bounds.width + 6, 120, 28, 5, BLACK);
    DrawRectangle(196, bounds.y + bounds.height + 6, 5, 28, WHITE);
    DrawRectangleLines(196, bounds.y + bounds.height + 6, 5, 28, BLACK);

    DrawCircle(150, 150, 4, WHITE);
    
    switch (state)
    {
        case NORMAL:
        {

        } break;
        case FOCUSED:
        {

        } break;
        case PRESSED:
        {

        } break;
        default: break;
    }
    //------------------------------------------------------------------
    
    return color;
}

// TODO: Panel system
RAYGUIDEF void GuiBeginPanel(Rectangle rec)
{
    //offset = (Vector2){ offset.x + rec.x, offset.y + rec.y };
    
    // TODO: Limit drawing to panel limits?
}

RAYGUIDEF void GuiEndPanel()
{
    //offset = (Vector2){ 0.0f, 0.0f };
}

#if defined(RAYGUI_STYLE_SAVE_LOAD)
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

#endif  // defined(RAYGUI_STYLE_SAVE_LOAD)

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

/*
void SetDrawRectangleFunc(void (* DrawRectangle)(Rectangle rec, Color color))
{
    DrawRectangleRecT(rec, color);
}

SetDrawRectangleFunc(DrawRectangleT);
*/
#endif

#endif // RAYGUI_IMPLEMENTATION
