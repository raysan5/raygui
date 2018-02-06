/*******************************************************************************************
*
*   raygui v1.x - A simple and easy-to-use IMGUI (Immedite Mode GUI) library for raylib (www.raylib.com)
*
*   DESCRIPTION:
*
*   raygui is a library for creating simple IMGUI interfaces using raylib. Basic controls provided:
*
*       - Label
*       - Button
*       - LabelButton
*       - ImageButton
*       - ImageButtonEx
*       - ToggleButton
*       - ToggleGroup
*       - CheckBox
*       - ComboBox
*       - GroupBox
*       - TextBox
*       - Slider
*       - SliderBar
*       - ProgressBar
*       - Spinner
*       - ListView
*       - ColorPicker
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
*       1.x (Jan-2018)    LOTS of CHANGES!
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
*       Adria Arranz:       Testing and Implementation of additional controls (2018)
*       Jordi Jorba:        Testing and Implementation of additional controls (2018)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2015-2018 Ramon Santamaria (@raysan5)
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
#define NUM_PROPERTIES       152

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
    
    // Vector3 type
    typedef struct Vector3 {
        float x;
        float y;
        float z;
    } Vector3;

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

typedef enum GuiPropertyGeneric {
    DEFAULT_BACKGROUND_COLOR = 0,
    DEFAULT_LINES_COLOR,
    DEFAULT_TEXT_FONT,
    DEFAULT_TEXT_SIZE,
    DEFAULT_BORDER_WIDTH,
    // Generic controls color style
    DEFAULT_BORDER_COLOR_NORMAL,
    DEFAULT_BASE_COLOR_NORMAL,
    DEFAULT_TEXT_COLOR_NORMAL,
    DEFAULT_BORDER_COLOR_FOCUSED,
    DEFAULT_BASE_COLOR_FOCUSED,
    DEFAULT_TEXT_COLOR_FOCUSED,
    DEFAULT_BORDER_COLOR_PRESSED,
    DEFAULT_BASE_COLOR_PRESSED,
    DEFAULT_TEXT_COLOR_PRESSED,
    DEFAULT_BORDER_COLOR_DISABLED,
    DEFAULT_BASE_COLOR_DISABLED,
    DEFAULT_TEXT_COLOR_DISABLED,
} GuiPropertyGeneric;

// Gui properties enumeration
typedef enum GuiProperty {
    // Label
    LABEL_TEXT_COLOR_NORMAL = 0,
    LABEL_TEXT_COLOR_FOCUSED,
    LABEL_TEXT_COLOR_PRESSED,
    LABEL_TEXT_COLOR_DISABLED,
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
    BUTTON_BORDER_COLOR_DISABLED,
    BUTTON_BASE_COLOR_DISABLED,
    BUTTON_TEXT_COLOR_DISABLED,
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
    TOGGLE_BORDER_COLOR_DISABLED,
    TOGGLE_BASE_COLOR_DISABLED,
    TOGGLE_TEXT_COLOR_DISABLED,
    TOGGLEGROUP_PADDING,
    // Slider
    SLIDER_BORDER_WIDTH,
    SLIDER_SLIDER_WIDTH,
    SLIDER_BORDER_COLOR_NORMAL,
    SLIDER_BASE_COLOR_NORMAL,
    SLIDER_BORDER_COLOR_FOCUSED,
    SLIDER_BASE_COLOR_FOCUSED,
    SLIDER_BORDER_COLOR_PRESSED,
    SLIDER_BASE_COLOR_PRESSED,
    SLIDER_BORDER_COLOR_DISABLED,
    SLIDER_BASE_COLOR_DISABLED,

    // SliderBar
    SLIDERBAR_INNER_PADDING,
    SLIDERBAR_BORDER_WIDTH,
    SLIDERBAR_BORDER_COLOR_NORMAL,
    SLIDERBAR_BASE_COLOR_NORMAL,
    SLIDERBAR_BORDER_COLOR_FOCUSED,
    SLIDERBAR_BASE_COLOR_FOCUSED,
    SLIDERBAR_BORDER_COLOR_PRESSED,
    SLIDERBAR_BASE_COLOR_PRESSED,
    SLIDERBAR_BORDER_COLOR_DISABLED,
    SLIDERBAR_BASE_COLOR_DISABLED,
    // ProgressBar
    PROGRESSBAR_INNER_PADDING,
    PROGRESSBAR_BORDER_WIDTH,
    PROGRESSBAR_BORDER_COLOR_NORMAL,
    PROGRESSBAR_BASE_COLOR_NORMAL,
    PROGRESSBAR_BORDER_COLOR_FOCUSED,
    PROGRESSBAR_BASE_COLOR_FOCUSED,
    PROGRESSBAR_BORDER_COLOR_PRESSED,
    PROGRESSBAR_BASE_COLOR_PRESSED,
    PROGRESSBAR_BORDER_COLOR_DISABLED,
    PROGRESSBAR_BASE_COLOR_DISABLED,
    // Spinner
    SPINNER_BUTTON_PADDING,
    SPINNER_BUTTONS_WIDTH,
    SPINNER_BORDER_COLOR_NORMAL,
    SPINNER_BASE_COLOR_NORMAL,
    SPINNER_TEXT_COLOR_NORMAL,
    SPINNER_BORDER_COLOR_FOCUSED,
    SPINNER_BASE_COLOR_FOCUSED,
    SPINNER_TEXT_COLOR_FOCUSED,
    SPINNER_BORDER_COLOR_PRESSED,
    SPINNER_BASE_COLOR_PRESSED,
    SPINNER_TEXT_COLOR_PRESSED,
    SPINNER_BORDER_COLOR_DISABLED,
    SPINNER_BASE_COLOR_DISABLED,
    SPINNER_TEXT_COLOR_DISABLED,
    // ComboBox
    COMBOBOX_BORDER_WIDTH,
    COMBOBOX_BUTTON_PADDING,
    COMBOBOX_SELECTOR_WIDTH,
    COMBOBOX_BORDER_COLOR_NORMAL,
    COMBOBOX_BASE_COLOR_NORMAL,
    COMBOBOX_TEXT_COLOR_NORMAL,
    COMBOBOX_BORDER_COLOR_FOCUSED,
    COMBOBOX_BASE_COLOR_FOCUSED,
    COMBOBOX_TEXT_COLOR_FOCUSED,
    COMBOBOX_BORDER_COLOR_PRESSED,
    COMBOBOX_BASE_COLOR_PRESSED,
    COMBOBOX_TEXT_COLOR_PRESSED,
    COMBOBOX_BORDER_COLOR_DISABLED,
    COMBOBOX_BASE_COLOR_DISABLED,
    COMBOBOX_TEXT_COLOR_DISABLED,
    // CheckBox
    CHECKBOX_BORDER_WIDTH,
    CHECKBOX_INNER_PADDING,
    CHECKBOX_BORDER_COLOR_NORMAL,
    CHECKBOX_BASE_COLOR_NORMAL,
    CHECKBOX_BORDER_COLOR_FOCUSED,
    CHECKBOX_BASE_COLOR_FOCUSED,
    CHECKBOX_BORDER_COLOR_PRESSED,
    CHECKBOX_BASE_COLOR_PRESSED,
    CHECKBOX_BORDER_COLOR_DISABLED,
    CHECKBOX_BASE_COLOR_DISABLED,
    // TextBox
    TEXTBOX_BORDER_WIDTH,
    TEXTBOX_BORDER_COLOR_NORMAL,
    TEXTBOX_BASE_COLOR_NORMAL,
    TEXTBOX_TEXT_COLOR_NORMAL,
    TEXTBOX_BORDER_COLOR_FOCUSED,
    TEXTBOX_BASE_COLOR_FOCUSED,
    TEXTBOX_TEXT_COLOR_FOCUSED,
    TEXTBOX_BORDER_COLOR_PRESSED,
    TEXTBOX_BASE_COLOR_PRESSED,
    TEXTBOX_TEXT_COLOR_PRESSED,
    TEXTBOX_BORDER_COLOR_DISABLED,
    TEXTBOX_BASE_COLOR_DISABLED,
    TEXTBOX_TEXT_COLOR_DISABLED,
    // ColorPicker
    COLORPICKER_BARS_THICK,
    COLORPICKER_BARS_PADDING,
    COLORPICKER_BORDER_COLOR_NORMAL,
    COLORPICKER_BASE_COLOR_NORMAL,
    COLORPICKER_BORDER_COLOR_FOCUSED,
    COLORPICKER_BASE_COLOR_FOCUSED,
    COLORPICKER_BORDER_COLOR_PRESSED,
    COLORPICKER_BASE_COLOR_PRESSED,
    COLORPICKER_BORDER_COLOR_DISABLED,
    COLORPICKER_BASE_COLOR_DISABLED,
    // ListView
    LISTVIEW_ELEMENTS_HEIGHT,
    LISTVIEW_ELEMENTS_PADDING,
    LISTVIEW_BAR_WIDTH,       
    LISTVIEW_BORDER_COLOR_NORMAL,
    LISTVIEW_BASE_COLOR_NORMAL,
    LISTVIEW_TEXT_COLOR_NORMAL,
    LISTVIEW_BORDER_COLOR_FOCUSED,
    LISTVIEW_BASE_COLOR_FOCUSED,
    LISTVIEW_TEXT_COLOR_FOCUSED,
    LISTVIEW_BORDER_COLOR_PRESSED,
    LISTVIEW_BASE_COLOR_PRESSED,
    LISTVIEW_TEXT_COLOR_PRESSED,
    LISTVIEW_BORDER_COLOR_DISABLED,
    LISTVIEW_BASE_COLOR_DISABLED,
    LISTVIEW_TEXT_COLOR_DISABLED,
} GuiProperty;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
RAYGUIDEF void GuiEnable(void);                                         // Enable gui controls (global state)
RAYGUIDEF void GuiDisable(void);                                        // Disable gui controls (global state)

RAYGUIDEF Color GuiBackgroundColor(void);                               // Get background color
RAYGUIDEF Color GuiLinesColor(void);                                    // Get lines color
RAYGUIDEF Color GuiTextColor(void);                                     // Get text color for normal state

RAYGUIDEF void GuiLabel(Rectangle bounds, const char *text);                                            // Label control, shows text
RAYGUIDEF bool GuiButton(Rectangle bounds, const char *text);                                           // Button control, returns true when clicked
RAYGUIDEF bool GuiLabelButton(Rectangle bounds, const char *text);                                      // Label button control, show true when clicked
RAYGUIDEF bool GuiImageButton(Rectangle bounds, Texture2D texture);                                     // Image button control, returns true when clicked
RAYGUIDEF bool GuiImageButtonEx(Rectangle bounds, Texture2D texture, Rectangle texSource, const char *text); // Image button extended control, returns true when clicked
RAYGUIDEF bool GuiToggleButton(Rectangle bounds, const char *text, bool toggle);                        // Toggle Button control, returns true when active
RAYGUIDEF int GuiToggleGroup(Rectangle bounds, const char **text, int count, int active);               // Toggle Group control, returns toggled button index
RAYGUIDEF bool GuiCheckBox(Rectangle bounds, bool checked);                                             // Check Box control, returns true when active
RAYGUIDEF int GuiComboBox(Rectangle bounds, const char **text, int count, int active);                  // Combo Box control, returns selected item index
RAYGUIDEF void GuiGroupBox(Rectangle bounds, const char *text);                                         // Group Box control with title name
RAYGUIDEF void GuiTextBox(Rectangle bounds, char *text, int textSize);                                  // Text Box control, updates input text
RAYGUIDEF float GuiSlider(Rectangle bounds, float value, float minValue, float maxValue);               // Slider control, returns selected value
RAYGUIDEF float GuiSliderBar(Rectangle bounds, float value, float minValue, float maxValue);            // Slider Bar control, returns selected value
RAYGUIDEF float GuiProgressBar(Rectangle bounds, float value, float minValue, float maxValue);          // Progress Bar control, shows current progress value
RAYGUIDEF int GuiSpinner(Rectangle bounds, int value, int minValue, int maxValue);                      // Spinner control, returns selected value
RAYGUIDEF bool GuiListElement(Rectangle bounds, const char *text, bool active);                         // List Element control, returns element state
RAYGUIDEF int GuiListView(Rectangle bounds, const char **text, int count, int active);                  // List View control, returns selected list element index

RAYGUIDEF Color GuiColorPicker(Rectangle bounds, Color color);          // Color Picker control

#if defined(RAYGUI_STYLE_SAVE_LOAD)
RAYGUIDEF void GuiLoadStyleImage(const char *fileName);                 // Load style from an image style file
RAYGUIDEF void GuiLoadStylePalette(Color *palette);                     // Load style from a color palette array (14 values required)
RAYGUIDEF void GuiUpdateStyleComplete(void);                            // Updates full style properties set with generic values

RAYGUIDEF void GuiSaveStyle(const char *fileName);                      // Save style file
RAYGUIDEF void GuiLoadStyle(const char *fileName);                      // Load style file
RAYGUIDEF void GuiSetStyleProperty(int guiProperty, int value);         // Set one style property
RAYGUIDEF int GuiGetStyleProperty(int guiProperty);                     // Get one style property
#endif

#endif // RAYGUI_H


/***********************************************************************************
*
*   RAYGUI IMPLEMENTATION
*
************************************************************************************/

#if defined(RAYGUI_IMPLEMENTATION)

#include <stdio.h>          // Required for: FILE, fopen(), fclose(), fprintf(), feof(), fscanf(), vsprintf()
#include <math.h>           // Required for: NAN
#include <string.h>         // Required for: strlen() on GuiTextBox()

#if defined(RAYGUI_STANDALONE)
    #include <stdarg.h>     // Required for: va_list, va_start(), vfprintf(), va_end()
#endif

// Check if custom malloc/free functions defined, if not, using standard ones
#if !defined(RAYGUI_MALLOC)
    #define RAYGUI_MALLOC(size)  malloc(size)
    #define RAYGUI_FREE(ptr)     free(ptr)
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#if !defined(RAYGUI_STYLE_DEFAULT_LIGHT) && !defined(RAYGUI_STYLE_DEFAULT_DARK)
    #define RAYGUI_STYLE_DEFAULT_LIGHT
#endif

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// GUI controls state
typedef enum GuiControlState { 
    DISABLED = 0, 
    NORMAL, 
    FOCUSED, 
    PRESSED 
} GuiControlState;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static int guiState = NORMAL;

static int styleGeneric[18] = {
#if defined(RAYGUI_STYLE_DEFAULT_LIGHT)
    0xf5f5f5ff,     // DEFAULT_BACKGROUND_COLOR
    0x90abb5ff,     // DEFAULT_LINES_COLOR
    0,              // DEFAULT_TEXT_FONT
    10,             // DEFAULT_TEXT_SIZE
    1,              // DEFAULT_BORDER_WIDTH
    // Generic controls color style
    0x828282ff,     // DEFAULT_BORDER_COLOR_NORMAL
    0xc8c8c8ff,     // DEFAULT_BASE_COLOR_NORMAL
    0x686868ff,     // DEFAULT_TEXT_COLOR_NORMAL
    0x7bb0d6ff,     // DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // DEFAULT_TEXT_COLOR_FOCUSED
    0x048cc7ff,     // DEFAULT_BORDER_COLOR_PRESSED
    0x7ceafeff,     // DEFAULT_BASE_COLOR_PRESSED
    0x0480b5ff,     // DEFAULT_TEXT_COLOR_PRESSED
    0xb1b1b1ff,     // DEFAULT_BORDER_COLOR_DISABLED
    0xfafafaff,     // DEFAULT_BASE_COLOR_DISABLED
    0xc8c8c8ff,     // DEFAULT_TEXT_COLOR_DISABLED
    0               // RESERVED
#elif defined(RAYGUI_STYLE_DEFAULT_DARK)
    0x2b3a3aff,     // DEFAULT_BACKGROUND_COLOR
    0x638465ff,     // DEFAULT_LINES_COLOR
    0,              // DEFAULT_TEXT_FONT
    10,             // DEFAULT_TEXT_SIZE
    1,              // DEFAULT_BORDER_WIDTH
    // Generic controls color style
    0x60827dff,     // DEFAULT_BORDER_COLOR_NORMAL
    0x344041ff,     // DEFAULT_BASE_COLOR_NORMAL
    0x90acb4ff,     // DEFAULT_TEXT_COLOR_NORMAL
    0x5f9aa8ff,     // DEFAULT_BORDER_COLOR_FOCUSED
    0x334f59ff,     // DEFAULT_BASE_COLOR_FOCUSED
    0x5f9aa8ff,     // DEFAULT_TEXT_COLOR_FOCUSED
    0xa9cb8dff,     // DEFAULT_BORDER_COLOR_PRESSED
    0x7ceafeff,     // DEFAULT_BASE_COLOR_PRESSED
    0x97af81ff,     // DEFAULT_TEXT_COLOR_PRESSED
    0x72817eff,     // DEFAULT_BORDER_COLOR_DISABLED
    0x344041ff,     // DEFAULT_BASE_COLOR_DISABLED
    0x606666ff,     // DEFAULT_TEXT_COLOR_DISABLED
    0               // RESERVED
#endif
};

// Current GUI style (default light)
static int style[NUM_PROPERTIES] = {
    0x686868ff,     // LABEL_TEXT_COLOR_NORMAL -----> DEFAULT_TEXT_COLOR_NORMAL
    0x6c9bbcff,     // LABEL_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x480b5ff,      // LABEL_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x606666ff,      // LABEL_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x2,            // BUTTON_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x828282ff,     // BUTTON_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc8c8c8ff,     // BUTTON_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x686868ff,     // BUTTON_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x7bb0d6ff,     // BUTTON_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // BUTTON_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // BUTTON_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x48cc7ff,      // BUTTON_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // BUTTON_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x480b5ff,      // BUTTON_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x72817eff,     // BUTTON_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x344041ff,     // BUTTON_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x606666ff,     // BUTTON_TEXT_COLOR_DISABLED ----> DEFAULT_COLOR_COLOR_DISABLED
        
    0x1,            // TOGGLE_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x828282ff,     // TOGGLE_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc8c8c8ff,     // TOGGLE_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x686868ff,     // TOGGLE_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x7bb0d6ff,     // TOGGLE_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // TOGGLE_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // TOGGLE_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x48cc7ff,      // TOGGLE_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // TOGGLE_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x480b5ff,      // TOGGLE_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x72817eff,     // TOGGLE_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x344041ff,     // TOGGLE_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x606666ff,     // TOGGLE_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x2,            // TOGGLEGROUP_PADDING
    0x1,            // SLIDER_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0X14,           // SLIDER_SLIDER_WIDTH 
    0x828282ff,     // SLIDER_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc8c8c8ff,     // SLIDER_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x7bb0d6ff,     // SLIDER_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // SLIDER_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x48cc7ff,      // SLIDER_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // SLIDER_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x72817eff,      // SLIDER_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x344041ff,     // SLIDER_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x1,            // SLIDERBAR_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x828282ff,     // SLIDERBAR_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc8c8c8ff,     // SLIDERBAR_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x7bb0d6ff,     // SLIDERBAR_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // SLIDERBAR_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x48cc7ff,      // SLIDERBAR_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // SLIDERBAR_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x72817eff,      // SLIDERBAR_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x344041ff,     // SLIDERBAR_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x1,            // PROGRESSBAR_INNER_PADDING
    0x1,            // PROGRESSBAR_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x828282ff,     // PROGRESSBAR_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc9effeff,     // PROGRESSBAR_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_PRESSED
    0x7bb0d6ff,     // PROGRESSBAR_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x97e8ffff,     // PROGRESSBAR_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x48cc7ff,      // PROGRESSBAR_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // PROGRESSBAR_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x72817eff,      // PROGRESSBAR_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x344041ff,     // PROGRESSBAR_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x2,            // SPINNER_BUTTON_PADDING
    0x23,           // SPINNER_BUTTONS_WIDTH
    0x828282ff,     // SPINNER_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc8c8c8ff,     // SPINNER_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x686868ff,     // SPINNER_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x7bb0d6ff,     // SPINNER_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // SPINNER_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // SPINNER_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x48cc7ff,      // SPINNER_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // SPINNER_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x480b5ff,      // SPINNER_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x72817eff,     // SPINNER_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x344041ff,     // SPINNER_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x606666ff,     // SPINNER_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x1,            // COMBOBOX_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x2,            // COMBOBOX_BUTTON_PADDING
    0X23,           // COMBOBOX_SELECTOR_WIDTH
    0x828282ff,     // COMBOBOX_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc8c8c8ff,     // COMBOBOX_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x686868ff,     // COMBOBOX_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x7bb0d6ff,     // COMBOBOX_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // COMBOBOX_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // COMBOBOX_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x48cc7ff,      // COMBOBOX_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // COMBOBOX_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x480b5ff,      // COMBOBOX_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x72817eff,     // COMBOBOX_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x344041ff,     // COMBOBOX_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x606666ff,     // COMBOBOX_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x1,            // CHECKBOX_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x1,            // CHECKBOX_INNER_PADDING
    0x828282ff,     // CHECKBOX_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xf5f5f5ff,     // CHECKBOX_BASE_COLOR_NORMAL ----> DEFAULT_BACKGROUND_COLOR
    0x7bb0d6ff,     // CHECKBOX_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x6c9bbcff,     // CHECKBOX_BASE_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x48cc7ff,      // CHECKBOX_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x480b5ff,      // CHECKBOX_BASE_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x72817eff,     // CHECKBOX_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x344041ff,     // CHECKBOX_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x1,            // TEXTBOX_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x828282ff,     // TEXTBOX_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xf5f5f5ff,     // TEXTBOX_BASE_COLOR_NORMAL ----> DEFAULT_BACKGROUND_COLOR
    0x686868ff,     // TEXTBOX_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x7bb0d6ff,     // TEXTBOX_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // TEXTBOX_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // TEXTBOX_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x48cc7ff,      // TEXTBOX_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // TEXTBOX_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x480b5ff,      // TEXTBOX_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x72817eff,     // TEXTBOX_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x344041ff,     // TEXTBOX_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x606666ff,     // TEXTBOX_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x14,           // COLORPICKER_BARS_THICK
    0xa,            // COLORPICKER_BARS_PADDING
    0x828282ff,     // COLORPICKER_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xf5f5f5ff,     // COLORPICKER_BASE_COLOR_NORMAL ----> DEFAULT_BACKGROUND_COLOR
    0x7bb0d6ff,     // COLORPICKER_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x6c9bbcff,     // COLORPICKER_BASE_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x48cc7ff,      // COLORPICKER_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x480b5ff,      // COLORPICKER_BASE_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x72817eff,     // COLORPICKER_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x344041ff,     // COLORPICKER_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x1e,           // LISTVIEW_ELEMENTS_HEIGHT
    0x2,            // LISTVIEW_ELEMENTS_PADDING
    0xa,            // LISTVIEW_BAR_WIDTH
    0x828282ff,     // LISTVIEW_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc8c8c8ff,     // LISTVIEW_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x686868ff,     // LISTVIEW_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x7bb0d6ff,     // LISTVIEW_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // LISTVIEW_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // LISTVIEW_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x48cc7ff,      // LISTVIEW_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // LISTVIEW_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x480b5ff,      // LISTVIEW_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x72817eff,     // LISTVIEW_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x344041ff,     // LISTVIEW_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x606666ff,     // LISTVIEW_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED        
};

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
#if defined(RAYGUI_STANDALONE)

#define KEY_RIGHT           262
#define KEY_LEFT            263
#define KEY_DOWN            264
#define KEY_UP              265
#define MOUSE_LEFT_BUTTON     0

#ifdef __cplusplus
    #define CLITERAL
#else
    #define CLITERAL    (Color)
#endif

#define WHITE      CLITERAL{ 255, 255, 255, 255 }   // White
#define BLACK      CLITERAL{ 0, 0, 0, 255 }         // Black
#define LIGHTGRAY  CLITERAL{ 200, 200, 200, 255 }   // Light Gray
#define RAYWHITE   CLITERAL{ 245, 245, 245, 255 }   // My own White (raylib logo)

// This functions are directly implemented in raygui
static Color GetColor(int hexValue);            // Returns a Color struct from hexadecimal value
static int GetHexValue(Color color);            // Returns hexadecimal value for a Color
static Color Fade(Color color, float alpha);    // Color fade-in or fade-out, alpha goes from 0.0f to 1.0f
static bool CheckCollisionPointRec(Vector2 point, Rectangle rec);  // Check if point is inside rectangle
static const char *FormatText(const char *text, ...);   // Formatting of text with variables to 'embed'

// raygui depend on some raylib input and drawing functions
// NOTE: To use raygui as standalone library, below functions must be defined by the user

static int MeasureText(const char *text, int fontSize) { /* TODO */ return 0; }
static void DrawText(const char *text, int posX, int posY, int fontSize, Color color) { /* TODO */ }
static void DrawRectangle(int x, int y, int width, int height, Color color) { /* TODO */ }
static void DrawRectangleRec(Rectangle rec, Color color) { DrawRectangle(rec.x, rec.y, rec.width, rec.height, color); }

// Input related functions
static Vector2 GetMousePosition(void) { return (Vector2){ 0, 0 }; }
static bool IsMouseButtonDown(int button);
static bool IsMouseButtonReleased(int button);

static bool IsMouseButtonUp(int button);                       // -- GuiSpinner(), GuiColorPicker()
static int GetKeyPressed(void);                                // -- GuiTextBox()
static bool IsKeyPressed(int key);                             // -- GuiTextBox()
static bool IsKeyDown(int key);                                // -- GuiColorPicker()

// Control specific functions
static void DrawRectangleLines(int x, int y, int width, int height, Color color) { /* TODO */ }           // -- GuiColorPicker() *
static void DrawRectangleGradientV(int posX, int posY, int width, int height, Color color1, Color color2);// -- GuiColorPicker() *
static void DrawRectangleGradientH(int posX, int posY, int width, int height, Color color1, Color color2);// -- GuiColorPicker() *
static void DrawRectangleGradientEx(Rectangle rec, Color col1, Color col2, Color col3, Color col4);       // -- GuiColorPicker()

static void DrawTexture(Texture2D texture, int posX, int posY, Color tint);         // -- GuiTexture()

#if defined(RAYGUI_STYLE_SAVE_LOAD)
static Image LoadImage(const char *fileName);       // -- GuiLoadStyleImage()
static Color *GetImageData(Image image);            // -- GuiLoadStyleImage()
static void UnloadImage(Image image);               // -- GuiLoadStyleImage()
#endif

#endif      // RAYGUI_STANDALONE


static Vector3 ConvertHSVtoRGB(Vector3 hsv);        // Convert color data from HSV to RGB
static Vector3 ConvertRGBtoHSV(Vector3 rgb);        // Convert color data from RGB to HSV

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Enable gui global state
RAYGUIDEF void GuiEnable(void) { guiState = NORMAL; }

// Disable gui global state
RAYGUIDEF void GuiDisable(void) { guiState = DISABLED; }

// Get background color
RAYGUIDEF Color GuiBackgroundColor(void) { return GetColor(styleGeneric[DEFAULT_BACKGROUND_COLOR]); }

// Get lines color
RAYGUIDEF Color GuiLinesColor(void) { return GetColor(styleGeneric[DEFAULT_LINES_COLOR]); }

// Get text color for normal state
RAYGUIDEF Color GuiTextColor(void) { return GetColor(styleGeneric[DEFAULT_TEXT_COLOR_NORMAL]); }

// Label control
RAYGUIDEF void GuiLabel(Rectangle bounds, const char *text)
{
    GuiControlState state = guiState;

    int textWidth = MeasureText(text, styleGeneric[DEFAULT_TEXT_SIZE]);
    int textHeight = styleGeneric[DEFAULT_TEXT_SIZE];

    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;

    // Update control
    //--------------------------------------------------------------------
    // ...
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL: 
        case FOCUSED:
        case PRESSED: DrawText(text, bounds.x, bounds.y, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_NORMAL])); break;
        case DISABLED: DrawText(text, bounds.x, bounds.y, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_DISABLED])); break;
        default: break;
    }
    //--------------------------------------------------------------------
}

// Button control, returns true when clicked
RAYGUIDEF bool GuiButton(Rectangle bounds, const char *text)
{
    GuiControlState state = guiState;
    bool clicked = false;
    
    int textWidth = MeasureText(text, styleGeneric[DEFAULT_TEXT_SIZE]);
    int textHeight = styleGeneric[DEFAULT_TEXT_SIZE];
    
    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check button state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) clicked = true;
            else state = FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_NORMAL]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_NORMAL]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_FOCUSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_PRESSED]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_PRESSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BASE_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_DISABLED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //------------------------------------------------------------------

    return clicked;
}

// Label button control
RAYGUIDEF bool GuiLabelButton(Rectangle bounds, const char *text)
{
    GuiControlState state = guiState;
    bool clicked = false;
    
    int textWidth = MeasureText(text, styleGeneric[DEFAULT_TEXT_SIZE]);
    int textHeight = styleGeneric[DEFAULT_TEXT_SIZE];

    //if (bounds.width < textWidth) bounds.width = textWidth;
    //if (bounds.height < textHeight) bounds.height = textHeight;
    bounds.width = textWidth;
    bounds.height = textHeight;

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check label state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) clicked = true;
            else state = FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL: DrawText(text, bounds.x, bounds.y, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_NORMAL])); break;
        case FOCUSED: DrawText(text, bounds.x, bounds.y, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_FOCUSED])); break;
        case PRESSED: DrawText(text, bounds.x, bounds.y, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_PRESSED])); break;
        case DISABLED: DrawText(text, bounds.x, bounds.y, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_DISABLED])); break;
        default: break;
    }
    //--------------------------------------------------------------------

    return clicked;
}

// Image button control, returns true when clicked
// TODO: Just provide textureId instead of full Texture2D
RAYGUIDEF bool GuiImageButtonEx(Rectangle bounds, Texture2D texture, Rectangle texSource, const char *text)
{
    #define IMAGEBUTTON_PADDING 4
    
    GuiControlState state = guiState;
    bool clicked = false;
    int textWidth = 0;
    int textHeight = 0;
    
    if (bounds.width < texSource.width) bounds.width = texSource.width;
    if (bounds.height < texSource.height) bounds.height = texSource.height;
    
    if (text != NULL)
    {
        textWidth = MeasureText(text, styleGeneric[DEFAULT_TEXT_SIZE]);
        textHeight = styleGeneric[DEFAULT_TEXT_SIZE];
        
        if (bounds.width < textWidth + texSource.width) bounds.width = textWidth + texSource.width + IMAGEBUTTON_PADDING;
        if (bounds.height < textHeight) bounds.height = textHeight;
    }

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();
        
        // Check button state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) clicked = true;
            else state = FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_NORMAL]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_NORMAL]));
            DrawTextureRec(texture, texSource, (Vector2){ bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING/2 + textWidth)/2, bounds.y + bounds.height/2 - texSource.height/2 }, GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));
            
            if (text != NULL) DrawText(text, (bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING + textWidth)/2) + texSource.width, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_FOCUSED]));
            DrawTextureRec(texture, texSource, (Vector2){ bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING/2 + textWidth)/2, bounds.y + bounds.height/2 - texSource.height/2 }, GetColor(style[BUTTON_TEXT_COLOR_FOCUSED]));
            
            if (text != NULL) DrawText(text, (bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING + textWidth)/2) + texSource.width, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_PRESSED]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_PRESSED]));
            DrawTextureRec(texture, texSource, (Vector2){ bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING/2 + textWidth)/2, bounds.y + bounds.height/2 - texSource.height/2 }, GetColor(style[BUTTON_TEXT_COLOR_PRESSED]));
            
            if (text != NULL) DrawText(text, (bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING + textWidth)/2) + texSource.width, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_DISABLED]));
            DrawTextureRec(texture, texSource, (Vector2){ bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING/2 + textWidth)/2, bounds.y + bounds.height/2 - texSource.height/2 }, GetColor(style[BUTTON_TEXT_COLOR_DISABLED]));
            
            if (text != NULL) DrawText(text, (bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING + textWidth)/2) + texSource.width, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //------------------------------------------------------------------

    return clicked;
}

// Image button control, returns true when clicked
// TODO: Just provide textureId instead of full Texture2D
RAYGUIDEF bool GuiImageButton(Rectangle bounds, Texture2D texture)
{
    return GuiImageButtonEx(bounds, texture, (Rectangle){ 0, 0, texture.width, texture.height }, NULL);
}

// Toggle Button control, returns true when active
RAYGUIDEF bool GuiToggleButton(Rectangle bounds, const char *text, bool active)
{
    GuiControlState state = guiState;
    
    int textWidth = MeasureText(text, styleGeneric[DEFAULT_TEXT_SIZE]);
    int textHeight = styleGeneric[DEFAULT_TEXT_SIZE];
    
    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

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
            
            // TODO: DrawTextureV(texCursor, mousePoint, WHITE);
        }
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
                DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_PRESSED]));
                DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_PRESSED]));
            }
            else
            {
                DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_NORMAL]));
                DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_NORMAL]));
                DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_NORMAL]));
            }
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_FOCUSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_PRESSED]));
            DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_PRESSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_DISABLED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------

    return active;
}

// Toggle Group control, returns toggled button index
RAYGUIDEF int GuiToggleGroup(Rectangle bounds, const char **text, int count, int active)
{
    for (int i = 0; i < count; i++)
    {
        if (i == active) GuiToggleButton((Rectangle){ bounds.x + i*(bounds.width + style[TOGGLEGROUP_PADDING]), bounds.y, bounds.width, bounds.height }, text[i], true);
        else if (GuiToggleButton((Rectangle){ bounds.x + i*(bounds.width + style[TOGGLEGROUP_PADDING]), bounds.y, bounds.width, bounds.height }, text[i], false) == true) active = i;
    }

    return active;
}

// Check Box control, returns true when active
RAYGUIDEF bool GuiCheckBox(Rectangle bounds, bool checked)
{
    GuiControlState state = guiState;

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check checkbox state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) checked = !checked;
            else state = FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRec(bounds, GetColor(style[CHECKBOX_BORDER_COLOR_NORMAL]));
            DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[CHECKBOX_BASE_COLOR_NORMAL]));
            if (checked) DrawRectangle(bounds.x + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING],
                                       bounds.y + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING], 
                                       bounds.width - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), 
                                       bounds.height - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), GetColor(style[CHECKBOX_BASE_COLOR_PRESSED]));        
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[CHECKBOX_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[CHECKBOX_BASE_COLOR_NORMAL]));
            if (checked) DrawRectangle(bounds.x + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING],
                                       bounds.y + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING], 
                                       bounds.width - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), 
                                       bounds.height - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), GetColor(style[CHECKBOX_BASE_COLOR_FOCUSED]));           
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[CHECKBOX_BORDER_COLOR_PRESSED]));
            DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[CHECKBOX_BASE_COLOR_NORMAL]));
            if (checked) DrawRectangle(bounds.x + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING],
                                       bounds.y + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING], 
                                       bounds.width - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), 
                                       bounds.height - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), GetColor(style[CHECKBOX_BASE_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(styleGeneric[DEFAULT_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[CHECKBOX_BASE_COLOR_DISABLED]));
            if (checked) DrawRectangle(bounds.x + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING],
                                       bounds.y + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING], 
                                       bounds.width - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), 
                                       bounds.height - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), GetColor(styleGeneric[DEFAULT_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------

    return checked;
}

// Combo Box control, returns selected item index
RAYGUIDEF int GuiComboBox(Rectangle bounds, const char **text, int count, int active)
{
    GuiControlState state = guiState;
    
    bounds.width -= (style[COMBOBOX_SELECTOR_WIDTH] + style[COMBOBOX_BUTTON_PADDING]);
        
    Rectangle selector = { bounds.x + bounds.width + style[COMBOBOX_BUTTON_PADDING], 
                               bounds.y, style[COMBOBOX_SELECTOR_WIDTH], bounds.height };
    
    if (active < 0) active = 0;
    else if (active > count - 1) active = count - 1;

    int textWidth = MeasureText(text[active], styleGeneric[DEFAULT_TEXT_SIZE]);
    int textHeight = styleGeneric[DEFAULT_TEXT_SIZE];

    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds) || 
            CheckCollisionPointRec(mousePoint, selector))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) 
            {
                active += 1;
                if (active >= count) active = 0;
            }
            else state = FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRec(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_NORMAL]));
            DrawRectangle(bounds.x + style[COMBOBOX_BORDER_WIDTH], bounds.y + style[COMBOBOX_BORDER_WIDTH], bounds.width - 2*style[COMBOBOX_BORDER_WIDTH], bounds.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_NORMAL]));

            DrawRectangleRec(selector, GetColor(style[COMBOBOX_BORDER_COLOR_NORMAL]));
            DrawRectangle(selector.x + style[COMBOBOX_BORDER_WIDTH], selector.y + style[COMBOBOX_BORDER_WIDTH], selector.width - 2*style[COMBOBOX_BORDER_WIDTH], selector.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_NORMAL]));
            
            DrawText(text[active], bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_NORMAL]));   
            DrawText(FormatText("%i/%i", active + 1, count), 
                     selector.x + selector.width/2 - (MeasureText(FormatText("%i/%i", active + 1, count), 
                     styleGeneric[DEFAULT_TEXT_SIZE])/2), selector.y + selector.height/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, 
                     styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[COMBOBOX_BORDER_WIDTH], bounds.y + style[COMBOBOX_BORDER_WIDTH], bounds.width - 2*style[COMBOBOX_BORDER_WIDTH], bounds.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_FOCUSED]));

            DrawRectangleRec(selector, GetColor(style[COMBOBOX_BORDER_COLOR_FOCUSED]));
            DrawRectangle(selector.x + style[COMBOBOX_BORDER_WIDTH], selector.y + style[COMBOBOX_BORDER_WIDTH], selector.width - 2*style[COMBOBOX_BORDER_WIDTH], selector.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_FOCUSED]));
            
            DrawText(text[active], bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_FOCUSED]));   
            DrawText(FormatText("%i/%i", active + 1, count), 
                     selector.x + selector.width/2 - (MeasureText(FormatText("%i/%i", active + 1, count), 
                     styleGeneric[DEFAULT_TEXT_SIZE])/2), selector.y + selector.height/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, 
                     styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_PRESSED]));
            DrawRectangle(bounds.x + style[COMBOBOX_BORDER_WIDTH], bounds.y + style[COMBOBOX_BORDER_WIDTH], bounds.width - 2*style[COMBOBOX_BORDER_WIDTH], bounds.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_PRESSED]));

            DrawRectangleRec(selector, GetColor(style[COMBOBOX_BORDER_COLOR_PRESSED]));
            DrawRectangle(selector.x + style[COMBOBOX_BORDER_WIDTH], selector.y + style[COMBOBOX_BORDER_WIDTH], selector.width - 2*style[COMBOBOX_BORDER_WIDTH], selector.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_PRESSED]));
            
            DrawText(text[active], bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_PRESSED]));   
            DrawText(FormatText("%i/%i", active + 1, count), 
                     selector.x + selector.width/2 - (MeasureText(FormatText("%i/%i", active + 1, count), 
                     styleGeneric[DEFAULT_TEXT_SIZE])/2), selector.y + selector.height/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, 
                     styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[COMBOBOX_BORDER_WIDTH], bounds.y + style[COMBOBOX_BORDER_WIDTH], bounds.width - 2*style[COMBOBOX_BORDER_WIDTH], bounds.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_DISABLED]));

            DrawRectangleRec(selector, GetColor(style[COMBOBOX_BORDER_COLOR_DISABLED]));
            DrawRectangle(selector.x + style[COMBOBOX_BORDER_WIDTH], selector.y + style[COMBOBOX_BORDER_WIDTH], selector.width - 2*style[COMBOBOX_BORDER_WIDTH], selector.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_DISABLED]));
            
            DrawText(text[active], bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_DISABLED]));
            DrawText(FormatText("%i/%i", active + 1, count), 
                     selector.x + selector.width/2 - (MeasureText(FormatText("%i/%i", active + 1, count), 
                     styleGeneric[DEFAULT_TEXT_SIZE])/2), selector.y + selector.height/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, 
                     styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------

    return active;
}

// Group Box control with title name
RAYGUIDEF void GuiGroupBox(Rectangle bounds, const char *text)
{
    #define GROUPBOX_LINE_THICK    1
    #define GROUPBOX_TEXT_PADDING 10
    #define GROUPBOX_PADDING 2
    
    DrawRectangle(bounds.x, bounds.y, GROUPBOX_LINE_THICK, bounds.height, GuiLinesColor());
    DrawRectangle(bounds.x, bounds.y + bounds.height, bounds.width + GROUPBOX_LINE_THICK, GROUPBOX_LINE_THICK, GuiLinesColor());
    DrawRectangle(bounds.x + bounds.width, bounds.y, GROUPBOX_LINE_THICK, bounds.height, GuiLinesColor());
    DrawRectangle(bounds.x, bounds.y, GROUPBOX_TEXT_PADDING, GROUPBOX_LINE_THICK, GuiLinesColor());
    DrawRectangle(bounds.x + 2*GROUPBOX_TEXT_PADDING + MeasureText(text, styleGeneric[DEFAULT_TEXT_SIZE]), bounds.y, bounds.width - 2*GROUPBOX_TEXT_PADDING - MeasureText(text, styleGeneric[DEFAULT_TEXT_SIZE]), GROUPBOX_LINE_THICK, GuiLinesColor());
    
    DrawText(text, bounds.x + GROUPBOX_TEXT_PADDING + 2*GROUPBOX_PADDING, bounds.y - 2*GROUPBOX_PADDING - GROUPBOX_LINE_THICK, styleGeneric[DEFAULT_TEXT_SIZE], GuiTextColor());
}

// Text Box control, updates input text
// NOTE: Requires static variables: framesCounter
RAYGUIDEF void GuiTextBox(Rectangle bounds, char *text, int textSize)
{
    #define GUITEXTBOX_PADDING 4
    #define GUITEXTBOX_LINE_PADDING 4

    GuiControlState state = guiState;
    static int framesCounter = 0;         // Required for blinking cursor

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

        #define KEY_BACKSPACE_TEXT    259     // GLFW BACKSPACE: 3 + 256

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            state = FOCUSED;        // NOTE: PRESSED state is not used on this control

            framesCounter++;
            
            int key = GetKeyPressed();
            int keyCount = strlen(text);
            
            // NOTE: Only allow keys in range [32..125]
            if ((key >= 32) && (key <= 125) && (keyCount < textSize))
            {
                text[keyCount] = (char)key;
                keyCount++;
            }
            
            if (IsKeyPressed(KEY_BACKSPACE_TEXT))
            {
                keyCount--;
                text[keyCount] = '\0';
                
                if (keyCount < 0) keyCount = 0;
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
            DrawRectangleRec(bounds, GetColor(style[TEXTBOX_BORDER_COLOR_NORMAL]));
            DrawRectangle(bounds.x + style[TEXTBOX_BORDER_WIDTH], bounds.y + style[TEXTBOX_BORDER_WIDTH], bounds.width - 2*style[TEXTBOX_BORDER_WIDTH], bounds.height - 2*style[TEXTBOX_BORDER_WIDTH], GetColor(style[TEXTBOX_BASE_COLOR_NORMAL]));
            DrawText(text, bounds.x + GUITEXTBOX_PADDING, bounds.y + style[TEXTBOX_BORDER_WIDTH] + bounds.height/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[TEXTBOX_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[TEXTBOX_BORDER_WIDTH], bounds.y + style[TEXTBOX_BORDER_WIDTH], bounds.width - 2*style[TEXTBOX_BORDER_WIDTH], bounds.height - 2*style[TEXTBOX_BORDER_WIDTH], GetColor(style[TEXTBOX_BASE_COLOR_FOCUSED]));
            DrawText(text, bounds.x + GUITEXTBOX_PADDING, bounds.y + style[TEXTBOX_BORDER_WIDTH] + bounds.height/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[TEXTBOX_TEXT_COLOR_PRESSED]));
            
            if ((framesCounter/20)%2 == 0) DrawRectangle(bounds.x + GUITEXTBOX_LINE_PADDING + MeasureText(text, styleGeneric[DEFAULT_TEXT_SIZE]), bounds.y + GUITEXTBOX_LINE_PADDING/2, 1, bounds.height - GUITEXTBOX_LINE_PADDING, GetColor(style[TEXTBOX_BORDER_COLOR_FOCUSED]));
        } break;
        case PRESSED: break; // NOTE: State not used on this control
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[TEXTBOX_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[TEXTBOX_BORDER_WIDTH], bounds.y + style[TEXTBOX_BORDER_WIDTH], bounds.width - 2*style[TEXTBOX_BORDER_WIDTH], bounds.height - 2*style[TEXTBOX_BORDER_WIDTH], GetColor(style[TEXTBOX_BASE_COLOR_DISABLED]));
            DrawText(text, bounds.x + GUITEXTBOX_PADDING, bounds.y + style[TEXTBOX_BORDER_WIDTH] + bounds.height/2 - styleGeneric[DEFAULT_TEXT_SIZE]/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[TEXTBOX_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------
}

// Slider control, returns selected value
RAYGUIDEF float GuiSlider(Rectangle bounds, float value, float minValue, float maxValue)
{
    #define GUISLIDER_SLIDER_LINE_THICK  1
    
    GuiControlState state = guiState;
    
    Rectangle slider = { bounds.x + (int)((value/(maxValue - minValue))*(bounds.width - 2*style[SLIDER_BORDER_WIDTH])) - style[SLIDER_SLIDER_WIDTH]/2,
                         bounds.y + style[SLIDER_BORDER_WIDTH], style[SLIDER_SLIDER_WIDTH], bounds.height - 2*style[SLIDER_BORDER_WIDTH] };
                         
    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                state = PRESSED;
                
                // Get equivalent value and slider position from mousePoint.x
                value = (((maxValue - minValue)*(mousePoint.x - (float)bounds.x))/(float)bounds.width) + minValue;
                slider.x = bounds.x + (int)(((value - minValue)/(maxValue - minValue))*(bounds.width - 2*style[SLIDER_BORDER_WIDTH])) - slider.width/2;
                
                // Snap to limits if mouse down moved outside limits
                //if (slider.x < (bounds.x + style[SLIDER_BORDER_WIDTH])) slider.x = bounds.x + style[SLIDER_BORDER_WIDTH];
                //else if ((slider.x + slider.width) > (bounds.x + bounds.width - 2*style[SLIDER_BORDER_WIDTH])) slider.x = (bounds.x + bounds.width - 2*style[SLIDER_BORDER_WIDTH]) - slider.width;
            }
            else state = FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL: 
        {
            DrawRectangleRec(bounds, GetColor(style[SLIDER_BORDER_COLOR_NORMAL]));
            DrawRectangle(bounds.x + style[SLIDER_BORDER_WIDTH], bounds.y + style[SLIDER_BORDER_WIDTH], bounds.width - 2*style[SLIDER_BORDER_WIDTH], bounds.height - 2*style[SLIDER_BORDER_WIDTH], GetColor(style[SLIDER_BASE_COLOR_NORMAL]));
            
            DrawRectangleRec(slider, GetColor(style[SLIDER_BASE_COLOR_PRESSED]));
            DrawRectangle(slider.x + slider.width/2, slider.y, GUISLIDER_SLIDER_LINE_THICK, slider.height, GetColor(style[SLIDER_BORDER_COLOR_PRESSED]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[SLIDER_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[SLIDER_BORDER_WIDTH], bounds.y + style[SLIDER_BORDER_WIDTH], bounds.width - 2*style[SLIDER_BORDER_WIDTH], bounds.height - 2*style[SLIDER_BORDER_WIDTH], GetColor(style[SLIDER_BASE_COLOR_NORMAL]));
            
            DrawRectangleRec(slider, GetColor(style[SLIDER_BASE_COLOR_FOCUSED]));
            DrawRectangle(slider.x + slider.width/2, slider.y, GUISLIDER_SLIDER_LINE_THICK, slider.height, GetColor(style[SLIDER_BORDER_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[SLIDER_BORDER_COLOR_PRESSED]));
            DrawRectangle(bounds.x + style[SLIDER_BORDER_WIDTH], bounds.y + style[SLIDER_BORDER_WIDTH], bounds.width - 2*style[SLIDER_BORDER_WIDTH], bounds.height - 2*style[SLIDER_BORDER_WIDTH], GetColor(style[SLIDER_BASE_COLOR_NORMAL]));
            
            DrawRectangleRec(slider, GetColor(style[SLIDER_BASE_COLOR_PRESSED]));
            DrawRectangle(slider.x + slider.width/2, slider.y, GUISLIDER_SLIDER_LINE_THICK, slider.height, GetColor(style[SLIDER_BORDER_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[SLIDER_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[SLIDER_BORDER_WIDTH], bounds.y + style[SLIDER_BORDER_WIDTH], bounds.width - 2*style[SLIDER_BORDER_WIDTH], bounds.height - 2*style[SLIDER_BORDER_WIDTH], GetColor(style[SLIDER_BASE_COLOR_DISABLED]));
            DrawRectangleRec(slider, GetColor(styleGeneric[DEFAULT_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------

    return value;
}

// Slider Bar control, returns selected value
RAYGUIDEF float GuiSliderBar(Rectangle bounds, float value, float minValue, float maxValue)
{
    GuiControlState state = guiState;
    
    Rectangle slider = { bounds.x + style[SLIDERBAR_BORDER_WIDTH], bounds.y + style[SLIDERBAR_BORDER_WIDTH] + style[SLIDERBAR_INNER_PADDING], 
                         (int)(((value - minValue)/(maxValue - minValue))*(bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH])),
                         bounds.height - 2*style[SLIDERBAR_BORDER_WIDTH] - 2*style[SLIDERBAR_INNER_PADDING] };

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                state = PRESSED;
                
                // Get equivalent value and slider width from mousePoint.x
                value = (maxValue - minValue)*((mousePoint.x - (float)bounds.x)/(float)bounds.width) + minValue;
                slider.width = (int)(((value - minValue)/(maxValue - minValue))*(bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH]));
            }
            else state = FOCUSED;
        }
        
        if (slider.width > bounds.width) slider.width = bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH];
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRec(bounds, GetColor(style[SLIDERBAR_BORDER_COLOR_NORMAL]));
            DrawRectangle(bounds.x + style[SLIDERBAR_BORDER_WIDTH], bounds.y + style[SLIDERBAR_BORDER_WIDTH], bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH], bounds.height - 2*style[SLIDERBAR_BORDER_WIDTH], GetColor(style[SLIDERBAR_BASE_COLOR_NORMAL]));
            DrawRectangleRec(slider, GetColor(style[SLIDERBAR_BASE_COLOR_PRESSED]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[SLIDERBAR_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[SLIDERBAR_BORDER_WIDTH], bounds.y + style[SLIDERBAR_BORDER_WIDTH], bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH], bounds.height - 2*style[SLIDERBAR_BORDER_WIDTH], GetColor(style[SLIDERBAR_BASE_COLOR_NORMAL]));
            DrawRectangleRec(slider, GetColor(style[SLIDERBAR_BASE_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[SLIDERBAR_BORDER_COLOR_PRESSED]));
            DrawRectangle(bounds.x + style[SLIDERBAR_BORDER_WIDTH], bounds.y + style[SLIDERBAR_BORDER_WIDTH], bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH], bounds.height - 2*style[SLIDERBAR_BORDER_WIDTH], GetColor(style[SLIDERBAR_BASE_COLOR_NORMAL]));
            DrawRectangleRec(slider, GetColor(style[SLIDERBAR_BASE_COLOR_PRESSED])); 
        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[SLIDERBAR_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[SLIDERBAR_BORDER_WIDTH], bounds.y + style[SLIDERBAR_BORDER_WIDTH], bounds.width - 2*style[SLIDERBAR_BORDER_WIDTH], bounds.height - 2*style[SLIDERBAR_BORDER_WIDTH], GetColor(style[SLIDER_BASE_COLOR_DISABLED]));
            DrawRectangleRec(slider, GetColor(styleGeneric[DEFAULT_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }    
    //--------------------------------------------------------------------

    return value;
}

// Progress Bar control, shows current progress value
RAYGUIDEF float GuiProgressBar(Rectangle bounds, float value, float minValue, float maxValue)
{
    GuiControlState state = guiState;
    
    Rectangle progress = { bounds.x + style[PROGRESSBAR_BORDER_WIDTH], 
                           bounds.y + style[PROGRESSBAR_BORDER_WIDTH] + style[PROGRESSBAR_INNER_PADDING], 
                           0, // TODO
                           bounds.height - 2*style[PROGRESSBAR_BORDER_WIDTH] - 2*style[PROGRESSBAR_INNER_PADDING] };

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

        progress.width = (int)(value/(maxValue - minValue)*(float)(bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH]));

        //if (CheckCollisionPointRec(mousePoint, bounds)) state = FOCUSED;      // State not required on ProgressBar
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            DrawRectangleRec(bounds, GetColor(style[PROGRESSBAR_BORDER_COLOR_NORMAL]));
            DrawRectangle(bounds.x + style[PROGRESSBAR_BORDER_WIDTH], bounds.y + style[PROGRESSBAR_BORDER_WIDTH], bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH], bounds.height - 2*style[PROGRESSBAR_BORDER_WIDTH], GetColor(styleGeneric[DEFAULT_BACKGROUND_COLOR]));
            DrawRectangleRec(progress, GetColor(style[PROGRESSBAR_BASE_COLOR_NORMAL]));
        } break;
        case FOCUSED: break;    // NOTE: State not used on this control
        case PRESSED: break;    // NOTE: State not used on this control
        case DISABLED: 
        {
            DrawRectangleRec(bounds, GetColor(style[PROGRESSBAR_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[PROGRESSBAR_BORDER_WIDTH], bounds.y + style[PROGRESSBAR_BORDER_WIDTH], bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH], bounds.height - 2*style[PROGRESSBAR_BORDER_WIDTH], GetColor(styleGeneric[DEFAULT_BACKGROUND_COLOR]));
            DrawRectangleRec(progress, GetColor(style[PROGRESSBAR_BASE_COLOR_DISABLED]));
        } break;
        default: break;
    }    
    //--------------------------------------------------------------------

    return value;
}

// Spinner control, returns selected value
// NOTE: Requires static variables: framesCounter, valueSpeed - ERROR!
RAYGUIDEF int GuiSpinner(Rectangle bounds, int value, int minValue, int maxValue)
{
    GuiControlState state = guiState;
    
    static int framesCounter = 0;
    static bool valueSpeed = false;;
    
    Rectangle spinner = { bounds.x + style[SPINNER_BUTTONS_WIDTH] + style[SPINNER_BUTTON_PADDING], bounds.y, bounds.width - 2*(style[SPINNER_BUTTONS_WIDTH] + style[SPINNER_BUTTON_PADDING]), bounds.height };
    Rectangle leftButtonBound = { bounds.x, bounds.y, style[SPINNER_BUTTONS_WIDTH], bounds.height };
    Rectangle rightButtonBound = { bounds.x + bounds.width - style[SPINNER_BUTTONS_WIDTH], bounds.y, style[SPINNER_BUTTONS_WIDTH], bounds.height };

    int textWidth = MeasureText(FormatText("%i", value), styleGeneric[DEFAULT_TEXT_SIZE]);
    int textHeight = styleGeneric[DEFAULT_TEXT_SIZE];

    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

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
    }
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
            DrawRectangleRec(spinner, GetColor(style[SPINNER_BORDER_COLOR_NORMAL]));
            DrawRectangle(spinner.x + style[SPINNER_BUTTON_PADDING], spinner.y + style[SPINNER_BUTTON_PADDING], spinner.width - style[SPINNER_BUTTON_PADDING]*2, spinner.height - style[SPINNER_BUTTON_PADDING]*2, GetColor(style[SPINNER_BASE_COLOR_NORMAL]));
            DrawText(FormatText("%i", value), spinner.x + (spinner.width/2 - textWidth/2), spinner.y + (spinner.height/2 - (styleGeneric[DEFAULT_TEXT_SIZE]/2)), styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(spinner, GetColor(style[SPINNER_BORDER_COLOR_FOCUSED]));
            DrawRectangle(spinner.x + style[SPINNER_BUTTON_PADDING], spinner.y + style[SPINNER_BUTTON_PADDING], spinner.width - style[SPINNER_BUTTON_PADDING]*2, spinner.height - style[SPINNER_BUTTON_PADDING]*2, GetColor(style[SPINNER_BASE_COLOR_FOCUSED]));
            DrawText(FormatText("%i", value), spinner.x + (spinner.width/2 - textWidth/2), spinner.y + (spinner.height/2 - (styleGeneric[DEFAULT_TEXT_SIZE]/2)), styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(spinner, GetColor(style[SPINNER_BORDER_COLOR_PRESSED]));
            DrawRectangle(spinner.x + style[SPINNER_BUTTON_PADDING], spinner.y + style[SPINNER_BUTTON_PADDING], spinner.width - style[SPINNER_BUTTON_PADDING]*2, spinner.height - style[SPINNER_BUTTON_PADDING]*2, GetColor(style[SPINNER_BASE_COLOR_PRESSED]));
            DrawText(FormatText("%i", value), spinner.x + (spinner.width/2 - textWidth/2), spinner.y + (spinner.height/2 - (styleGeneric[DEFAULT_TEXT_SIZE]/2)), styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(spinner, GetColor(style[SPINNER_BORDER_COLOR_DISABLED]));
            DrawRectangle(spinner.x + style[SPINNER_BUTTON_PADDING], spinner.y + style[SPINNER_BUTTON_PADDING], spinner.width - style[SPINNER_BUTTON_PADDING], spinner.height - style[SPINNER_BUTTON_PADDING], GetColor(style[SPINNER_BASE_COLOR_DISABLED]));
            DrawText(FormatText("%i", value), spinner.x + (spinner.width/2 - textWidth/2), spinner.y + (spinner.height/2 - (styleGeneric[DEFAULT_TEXT_SIZE]/2)), styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }

    return value;
}

// List Element control, returns element state
RAYGUIDEF bool GuiListElement(Rectangle bounds, const char *text, bool active)
{
    #define GUILISTELEMENT_PADDING  2
    
    GuiControlState state = guiState;
    
    int textWidth = MeasureText(text, styleGeneric[DEFAULT_TEXT_SIZE]);
    int textHeight = styleGeneric[DEFAULT_TEXT_SIZE];
    
    if (bounds.width < textWidth) bounds.width = textWidth + GUILISTELEMENT_PADDING*2;
    if (bounds.height < textHeight) bounds.height = textHeight;
    
    
    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

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
                DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BASE_COLOR_PRESSED]));
                DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BORDER_COLOR_PRESSED]));
                DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[LISTVIEW_TEXT_COLOR_PRESSED]));
            }
            else
            {
                //DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[TOGGLE_BORDER_COLOR_NORMAL]));
                DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[LISTVIEW_TEXT_COLOR_NORMAL]));
            }
        } break;
        case FOCUSED:
        {
            DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BASE_COLOR_FOCUSED]));
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BORDER_COLOR_FOCUSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[LISTVIEW_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BASE_COLOR_PRESSED]));
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BORDER_COLOR_PRESSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(style[LISTVIEW_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            //DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[DEFAULT_BASE_COLOR_DISABLED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, styleGeneric[DEFAULT_TEXT_SIZE], GetColor(styleGeneric[LISTVIEW_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------
    
    return active;
}

// List View control, returns selected list element index
RAYGUIDEF int GuiListView(Rectangle bounds, const char **text, int count, int active)
{
    #define LISTVIEW_LINE_THICK       1
    // TODO: Implement list view with scrolling bars and selectable elements (hover/press)

    GuiControlState state = guiState;
    
    static int startIndex = 0;
    int endIndex = count;

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        for(int i = 0; i < count; i++)
        {
            int textWidth = MeasureText(text[i], styleGeneric[DEFAULT_TEXT_SIZE]);
            if (bounds.width - style[LISTVIEW_BAR_WIDTH] - 2*style[LISTVIEW_ELEMENTS_PADDING] - LISTVIEW_LINE_THICK < textWidth)
            {
                 bounds.width = textWidth + style[LISTVIEW_BAR_WIDTH] + 2*style[LISTVIEW_ELEMENTS_PADDING] + LISTVIEW_LINE_THICK;
            }               
       }
      
        Vector2 mousePoint = GetMousePosition();
        
        endIndex = bounds.height/(style[LISTVIEW_ELEMENTS_HEIGHT] + style[LISTVIEW_ELEMENTS_PADDING]);
        
        if (endIndex < count)
        {
            startIndex -= GetMouseWheelMove();
            if (startIndex < 0) startIndex = 0;

            endIndex += startIndex;
            
            if (startIndex > (count - (endIndex - startIndex)))
            {
                startIndex = count - (endIndex - startIndex);
            }
        }
        
        if (endIndex > count) endIndex = count;
        
        if (count*style[LISTVIEW_ELEMENTS_HEIGHT] <= bounds.height) startIndex = 0;
        
        
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) 
            {
                
                //active += 1;
                //if (active >= count) active = 0;
            }
            else state = FOCUSED;
        }
        
        
        //(maxIndexCount + indexOffset) > count) ? count : (maxIndexCount + indexOffset)
        
        //if (maxIndexCount + indexOffset) > count) return count;
        //else return (maxIndexCount + indexOffset);
        

    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    for (int i = startIndex; i < endIndex; i++)
    {
        if (i == active) 
        {
            if (GuiListElement((Rectangle){ bounds.x + style[LISTVIEW_BAR_WIDTH]  + style[LISTVIEW_ELEMENTS_PADDING], bounds.y + style[LISTVIEW_ELEMENTS_PADDING] + LISTVIEW_LINE_THICK + (i - startIndex)*(style[LISTVIEW_ELEMENTS_HEIGHT] + style[LISTVIEW_ELEMENTS_PADDING]), bounds.width - style[LISTVIEW_BAR_WIDTH] - 2*style[LISTVIEW_ELEMENTS_PADDING] - LISTVIEW_LINE_THICK, style[LISTVIEW_ELEMENTS_HEIGHT] }, text[i], true) == false) active = -1;
        }
        else
        {
            if (GuiListElement((Rectangle){ bounds.x + style[LISTVIEW_BAR_WIDTH]  + style[LISTVIEW_ELEMENTS_PADDING], bounds.y  + style[LISTVIEW_ELEMENTS_PADDING] + LISTVIEW_LINE_THICK + (i - startIndex)*(style[LISTVIEW_ELEMENTS_HEIGHT] + style[LISTVIEW_ELEMENTS_PADDING]), bounds.width - style[LISTVIEW_BAR_WIDTH] - 2*style[LISTVIEW_ELEMENTS_PADDING] - LISTVIEW_LINE_THICK, style[LISTVIEW_ELEMENTS_HEIGHT] }, text[i], false) == true) active = i;
        }
    }
    
    DrawRectangle(bounds.x, bounds.y, style[LISTVIEW_BAR_WIDTH], bounds.height, LIGHTGRAY);
    
    int barHeight = bounds.height - (count - (endIndex - startIndex))*style[LISTVIEW_ELEMENTS_HEIGHT];
    
    // TODO: Review bar logic when bar size should be shorter than LISTVIEW_ELEMENT_HEIGHT
    if (bounds.height < ((count - (endIndex - startIndex))*style[LISTVIEW_ELEMENTS_HEIGHT]))
    {
        float newHeight = (float)(endIndex - startIndex)*(float)(style[LISTVIEW_ELEMENTS_HEIGHT]/2)/(float)(endIndex - startIndex);
        barHeight = (float)bounds.height - (float)((count - (endIndex - startIndex))*newHeight);
    }
       
    //DrawText(FormatText("MAX INDEX: %i", endIndex), 200, 60, 20, RED);
    //DrawText(FormatText("INDEX OFFSET: %i", startIndex), 200, 120, 20, RED);
    
    switch (state)
    {
        case NORMAL:
        {
            if(barHeight >= bounds.height) DrawRectangle(bounds.x, startIndex*style[LISTVIEW_ELEMENTS_HEIGHT] + bounds.y, style[LISTVIEW_BAR_WIDTH], barHeight, GetColor(style[LISTVIEW_BASE_COLOR_DISABLED]));
            else DrawRectangle(bounds.x, startIndex*style[LISTVIEW_ELEMENTS_HEIGHT] + bounds.y, style[LISTVIEW_BAR_WIDTH], barHeight, GetColor(style[SLIDERBAR_BASE_COLOR_NORMAL]));
            
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BORDER_COLOR_NORMAL]));
            
        } break;
        case FOCUSED:
        {
            if(barHeight >= bounds.height) DrawRectangle(bounds.x, startIndex*style[LISTVIEW_ELEMENTS_HEIGHT] + bounds.y, style[LISTVIEW_BAR_WIDTH], barHeight, GetColor(style[LISTVIEW_BASE_COLOR_DISABLED]));
            else DrawRectangle(bounds.x, startIndex*style[LISTVIEW_ELEMENTS_HEIGHT] + bounds.y, style[LISTVIEW_BAR_WIDTH], barHeight, GetColor(style[SLIDERBAR_BASE_COLOR_FOCUSED]));
            
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BORDER_COLOR_FOCUSED]));
            
        } break;
        case PRESSED:
        {
            if(barHeight >= bounds.height) DrawRectangle(bounds.x, startIndex*style[LISTVIEW_ELEMENTS_HEIGHT] + bounds.y, style[LISTVIEW_BAR_WIDTH], barHeight, GetColor(style[LISTVIEW_BASE_COLOR_DISABLED]));
            else DrawRectangle(bounds.x, startIndex*style[LISTVIEW_ELEMENTS_HEIGHT] + bounds.y, style[LISTVIEW_BAR_WIDTH], barHeight, GetColor(style[SLIDERBAR_BASE_COLOR_PRESSED]));
            
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BORDER_COLOR_PRESSED]));
            
        } break;
        case DISABLED:
        {
            DrawRectangle(bounds.x, startIndex*style[LISTVIEW_ELEMENTS_HEIGHT] + bounds.y, style[LISTVIEW_BAR_WIDTH], barHeight, GetColor(style[LISTVIEW_BASE_COLOR_DISABLED]));
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BORDER_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------
    
    return active;
}

// Color Panel control
RAYGUIDEF Color GuiColorPanel(Rectangle bounds, Color color)
{
    #define GUICOLORPANEL_SELECTOR_THICK    6
    
    GuiControlState state = guiState;
    Vector2 pickerSelector = { 0 };
    
    Vector3 vcolor = { (float)color.r/255.0f, (float)color.g/255.0f, (float)color.b/255.0f };
    Vector3 hsv = ConvertRGBtoHSV(vcolor);

    pickerSelector.x = bounds.x + (float)hsv.y*bounds.width;            // HSV: Saturation
    pickerSelector.y = bounds.y + (1.0f - (float)hsv.z)*bounds.height;  // HSV: Value
    
    Vector3 maxHue = { hsv.x, 1.0f, 1.0f };
    Vector3 rgbHue = ConvertHSVtoRGB(maxHue);
    Color maxHueCol = { (unsigned char)(255.0f*rgbHue.x), 
                      (unsigned char)(255.0f*rgbHue.y), 
                      (unsigned char)(255.0f*rgbHue.z), 255 };
                      
    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) 
            {
                state = PRESSED;
                pickerSelector = mousePoint;
            
                // Calculate color from picker
                Vector2 colorPick = { pickerSelector.x - bounds.x, pickerSelector.y - bounds.y }; 
                
                colorPick.x /= (float)bounds.width;     // Get normalized value on x
                colorPick.y /= (float)bounds.height;    // Get normalized value on y
                
                hsv.y = colorPick.x;
                hsv.z = 1.0f - colorPick.y;
                
                Vector3 rgb = ConvertHSVtoRGB(hsv);

                // NOTE: Vector3ToColor() only available on raylib 1.8.1
                color = (Color){ (unsigned char)(255.0f*rgb.x), 
                                 (unsigned char)(255.0f*rgb.y), 
                                 (unsigned char)(255.0f*rgb.z), 
                                 (unsigned char)(255.0f*(float)color.a/255.0f) };
                
            }
            else state = FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        DrawRectangleGradientEx(bounds, WHITE, WHITE, maxHueCol, maxHueCol);
        DrawRectangleGradientEx(bounds, Fade(BLACK, 0), BLACK, BLACK, Fade(BLACK, 0));
        
        // Draw color picker: selector
        DrawRectangle(pickerSelector.x - GUICOLORPANEL_SELECTOR_THICK/2, pickerSelector.y - GUICOLORPANEL_SELECTOR_THICK/2, GUICOLORPANEL_SELECTOR_THICK, GUICOLORPANEL_SELECTOR_THICK, WHITE);
        
    }
 
    switch (state)
    {
        case NORMAL: DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_NORMAL])); break;
        case FOCUSED: DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_FOCUSED])); break;
        case PRESSED: DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_PRESSED])); break;
        case DISABLED: DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(styleGeneric[DEFAULT_TEXT_COLOR_DISABLED])); break;
        default: break;
    }
    //--------------------------------------------------------------------
    
    return color;
}

// Color Bar Alpha control
// NOTE: Returns alpha value normalized [0..1]
RAYGUIDEF float GuiColorBarAlpha(Rectangle bounds, float alpha)
{
    #define GUICOLORBARALPHA_SELECTOR_WIDTH     6
    #define GUICOLORBARALPHA_SELECTOR_PADDING   2
    #define CHECKER_SIZE                       10
    
    //#define GUICOLORBAR_SELECTOR_THICK          6
    
    GuiControlState state = guiState;
    Rectangle selector = { bounds.x + alpha*bounds.width - GUICOLORBARALPHA_SELECTOR_PADDING, bounds.y - GUICOLORBARALPHA_SELECTOR_PADDING, GUICOLORBARALPHA_SELECTOR_WIDTH, bounds.height + GUICOLORBARALPHA_SELECTOR_PADDING*2 };

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds) || 
            CheckCollisionPointRec(mousePoint, selector))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                state = PRESSED;
                selector.x = mousePoint.x - selector.width/2;
                
                alpha = (mousePoint.x - bounds.x)/bounds.width;
                if (alpha <= 0.0f) alpha = 0.0f;
                if (alpha >= 1.0f) alpha = 1.0f;
                //selector.x = bounds.x + (int)(((alpha - 0)/(100 - 0))*(bounds.width - 2*style[SLIDER_BORDER_WIDTH])) - selector.width/2;
            }
            else state = FOCUSED; //state = FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    // Draw alpha bar: checked background
    for (int i = 0; i < bounds.width/CHECKER_SIZE; i++) DrawRectangle(bounds.x + CHECKER_SIZE*(i%(bounds.width/CHECKER_SIZE)), bounds.y, bounds.width/(bounds.width/CHECKER_SIZE), CHECKER_SIZE, (i%2) ? LIGHTGRAY : RAYWHITE);
    for (int i = 0; i < bounds.width/CHECKER_SIZE; i++) DrawRectangle(bounds.x + CHECKER_SIZE*(i%(bounds.width/CHECKER_SIZE)), bounds.y + CHECKER_SIZE, bounds.width/(bounds.width/CHECKER_SIZE), CHECKER_SIZE, (i%2) ? RAYWHITE : LIGHTGRAY);
    DrawRectangleGradientH(bounds.x, bounds.y, bounds.width, bounds.height, (Color){ 255,255,255,0 }, (Color){ 0,0,0,255 });
    
    switch (state)
    {
        case NORMAL: 
        {
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_NORMAL]));
            DrawRectangle(selector.x , selector.y, selector.width, selector.height, GetColor(style[COLORPICKER_BORDER_COLOR_PRESSED]));
        } break;
        case FOCUSED:
        {
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_FOCUSED]));
            DrawRectangle(selector.x, selector.y, selector.width, selector.height, GetColor(style[COLORPICKER_BORDER_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_PRESSED]));
            DrawRectangle(selector.x, selector.y, selector.width, selector.height, GetColor(style[COLORPICKER_BORDER_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[COLORPICKER_BORDER_COLOR_DISABLED]));
            DrawRectangleRec(selector, GetColor(styleGeneric[DEFAULT_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------
    
    return alpha;
}

// Color Bar Hue control
// NOTE: Returns hue value normalized [0..1]
RAYGUIDEF float GuiColorBarHue(Rectangle bounds, float hue)
{
    #define GUICOLORBARHUE_SELECTOR_HEIGHT   6
    #define GUICOLORBARHUE_SELECTOR_PADDING  2
    
    GuiControlState state = guiState;
    Rectangle selector = { bounds.x - GUICOLORBARHUE_SELECTOR_PADDING, bounds.y + hue/360.0f*bounds.height - GUICOLORBARHUE_SELECTOR_PADDING, bounds.width + GUICOLORBARHUE_SELECTOR_PADDING*2, GUICOLORBARHUE_SELECTOR_HEIGHT };
    
    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds) || 
            CheckCollisionPointRec(mousePoint, selector))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) 
            {
                state = PRESSED;
                selector.y = mousePoint.y - selector.height/2;
                
                hue = (mousePoint.y - bounds.y)*360/bounds.height;  
                if (hue <= 0.0f) hue = 0.0f;
                if (hue >= 359.0f) hue = 359.0f;
                
            }
            else state = FOCUSED;
            
            /*if (IsKeyDown(KEY_UP))
            {
                hue -= 2.0f;
                if (hue <= 0.0f) hue = 0.0f;
            }
            else if (IsKeyDown(KEY_DOWN))
            {
                hue += 2.0f;
                if (hue >= 360.0f) hue = 360.0f;
            }*/
        }
    }
    //--------------------------------------------------------------------
    // Draw control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        //Draw hue bar:color bars
        DrawRectangleGradientV(bounds.x + GUICOLORBARHUE_SELECTOR_PADDING/2, bounds.y + GUICOLORBARHUE_SELECTOR_PADDING/2, bounds.width - GUICOLORBARHUE_SELECTOR_PADDING, bounds.height/6, (Color){ 255,0,0,255 }, (Color){ 255,255,0,255 });
        DrawRectangleGradientV(bounds.x + GUICOLORBARHUE_SELECTOR_PADDING/2, bounds.y + bounds.height/6 + GUICOLORBARHUE_SELECTOR_PADDING/2, bounds.width - GUICOLORBARHUE_SELECTOR_PADDING, bounds.height/6, (Color){ 255,255,0,255 }, (Color){ 0,255,0,255 });
        DrawRectangleGradientV(bounds.x + GUICOLORBARHUE_SELECTOR_PADDING/2, bounds.y + 2*(bounds.height/6) + GUICOLORBARHUE_SELECTOR_PADDING/2, bounds.width - GUICOLORBARHUE_SELECTOR_PADDING, bounds.height/6, (Color){ 0,255,0,255 }, (Color){ 0,255,255,255 });
        DrawRectangleGradientV(bounds.x + GUICOLORBARHUE_SELECTOR_PADDING/2, bounds.y + 3*(bounds.height/6) + GUICOLORBARHUE_SELECTOR_PADDING/2, bounds.width - GUICOLORBARHUE_SELECTOR_PADDING, bounds.height/6, (Color){ 0,255,255,255 }, (Color){ 0,0,255,255 });
        DrawRectangleGradientV(bounds.x + GUICOLORBARHUE_SELECTOR_PADDING/2, bounds.y + 4*(bounds.height/6) + GUICOLORBARHUE_SELECTOR_PADDING/2, bounds.width - GUICOLORBARHUE_SELECTOR_PADDING, bounds.height/6, (Color){ 0,0,255,255 }, (Color){ 255,0,255,255 });
        DrawRectangleGradientV(bounds.x + GUICOLORBARHUE_SELECTOR_PADDING/2, bounds.y + 5*(bounds.height/6) + GUICOLORBARHUE_SELECTOR_PADDING/2, bounds.width - GUICOLORBARHUE_SELECTOR_PADDING, bounds.height/6 - GUICOLORBARHUE_SELECTOR_PADDING, (Color){ 255,0,255,255 }, (Color){ 255,0,0,255 });
    }
    else
    {
        DrawRectangleGradientV(bounds.x, bounds.y, bounds.width, bounds.height, RAYWHITE, GRAY);
    }
    
    switch (state)
    {
        case NORMAL:
        {
            // Draw hue bar: selector
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_NORMAL]));
            DrawRectangle(selector.x, selector.y, selector.width, selector.height, GetColor(style[COLORPICKER_BORDER_COLOR_PRESSED]));
        } break;
        case FOCUSED:
        {
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_FOCUSED]));
            DrawRectangle(selector.x, selector.y , selector.width, selector.height, GetColor(style[COLORPICKER_BORDER_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_PRESSED]));
            DrawRectangle(selector.x, selector.y, selector.width, selector.height, GetColor(style[COLORPICKER_BORDER_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_DISABLED]));
            DrawRectangle(selector.x, selector.y, selector.width, selector.height, GetColor(style[COLORPICKER_BASE_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------
    
    return hue;
}

// TODO: Color GuiColorBarSat() [WHITE->color]
// TODO: Color GuiColorBarValue() [BLACK->color]), HSV / HSL
// TODO: float GuiColorBarLuminance() [BLACK->WHITE]


// Color Picker control
// NOTE: It's divided in multiple controls:
//      Color GuiColorPanel() - Color select panel
//      float GuiColorBarAlpha(Rectangle bounds, float alpha)
//      float GuiColorBarHue(Rectangle bounds, float value)
// NOTE: bounds define GuiColorPanel() size
RAYGUIDEF Color GuiColorPicker(Rectangle bounds, Color color)
{    
    color = GuiColorPanel(bounds, color);
    
    Rectangle boundsAlpha = { bounds.x, bounds.y + bounds.height + style[COLORPICKER_BARS_PADDING], bounds.width, style[COLORPICKER_BARS_THICK] };
    Rectangle boundsHue = { bounds.x + bounds.width + style[COLORPICKER_BARS_PADDING], bounds.y, style[COLORPICKER_BARS_THICK], bounds.height };
    
    Vector3 hsv = ConvertRGBtoHSV((Vector3){ color.r/255.0f, color.g/255.0f, color.b/255.0f });
    
    hsv.x = GuiColorBarHue(boundsHue, hsv.x);
    
    color.a = (unsigned char)(GuiColorBarAlpha(boundsAlpha, (float)color.a/255.0f)*255.0f);
    
    Vector3 rgb = ConvertHSVtoRGB(hsv);

    color = (Color){ (unsigned char)(rgb.x*255.0f), (unsigned char)(rgb.y*255.0f), (unsigned char)(rgb.z*255.0f), color.a };
    
    
    // Draw control
    //--------------------------------------------------------------------
    // Draw color selected panel
    for (int i = 0; i < 2; i++) DrawRectangle(bounds.x + style[COLORPICKER_BARS_PADDING]*(i%(bounds.width/(style[COLORPICKER_BARS_THICK]/2))) + bounds.width + style[COLORPICKER_BARS_PADDING], bounds.y + bounds.height + style[COLORPICKER_BARS_PADDING], bounds.width/(bounds.width/(style[COLORPICKER_BARS_THICK]/2)), style[COLORPICKER_BARS_THICK]/2, (i%2) ? LIGHTGRAY : RAYWHITE);
    for (int i = 0; i < 2; i++) DrawRectangle(bounds.x + style[COLORPICKER_BARS_PADDING]*(i%(bounds.width/(style[COLORPICKER_BARS_THICK]/2))) + bounds.width + style[COLORPICKER_BARS_PADDING], bounds.y + style[COLORPICKER_BARS_PADDING] + bounds.height + style[COLORPICKER_BARS_PADDING], bounds.width/(bounds.width/(style[COLORPICKER_BARS_THICK]/2)), style[COLORPICKER_BARS_THICK]/2, (i%2) ? RAYWHITE : LIGHTGRAY);
    DrawRectangle(bounds.x + bounds.width + style[COLORPICKER_BARS_PADDING], bounds.y + bounds.height + style[COLORPICKER_BARS_PADDING], style[COLORPICKER_BARS_THICK], style[COLORPICKER_BARS_THICK], color);
    DrawRectangleLines(bounds.x + bounds.width + style[COLORPICKER_BARS_PADDING], bounds.y + bounds.height + style[COLORPICKER_BARS_PADDING], style[COLORPICKER_BARS_THICK], style[COLORPICKER_BARS_THICK], GetColor(style[COLORPICKER_BORDER_COLOR_NORMAL]));
    //--------------------------------------------------------------------

    return color;
}

#if defined(RAYGUI_STYLE_SAVE_LOAD)
// Load GUI style from an image style file
RAYGUIDEF void GuiLoadStyleImage(const char *fileName)
{
    // NOTE: Image data only defines color properties
    Image imStyle = LoadImage(fileName);
    Color *pixels = GetImageData(imStyle);

    // Load generic style color palette
    styleGeneric[DEFAULT_BACKGROUND_COLOR] = GetHexValue(pixels[1 + imStyle.width*1]);
    styleGeneric[DEFAULT_LINES_COLOR] = GetHexValue(pixels[0 + imStyle.width*0]);
    styleGeneric[DEFAULT_BORDER_COLOR_NORMAL] = GetHexValue(pixels[2 + imStyle.width*2]);
    styleGeneric[DEFAULT_BASE_COLOR_NORMAL] = GetHexValue(pixels[3 + imStyle.width*3]);
    styleGeneric[DEFAULT_TEXT_COLOR_NORMAL] = GetHexValue(pixels[9 + imStyle.width*4]);
    styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED] = GetHexValue(pixels[17 + imStyle.width*2]);
    styleGeneric[DEFAULT_BASE_COLOR_FOCUSED] = GetHexValue(pixels[18 + imStyle.width*3]);
    styleGeneric[DEFAULT_TEXT_COLOR_FOCUSED] = GetHexValue(pixels[24 + imStyle.width*4]);
    styleGeneric[DEFAULT_BORDER_COLOR_PRESSED] = GetHexValue(pixels[32 + imStyle.width*2]);
    styleGeneric[DEFAULT_BASE_COLOR_PRESSED] = GetHexValue(pixels[33 + imStyle.width*3]);
    styleGeneric[DEFAULT_TEXT_COLOR_PRESSED] = GetHexValue(pixels[39 + imStyle.width*4]);
    styleGeneric[DEFAULT_BORDER_COLOR_DISABLED] = GetHexValue(pixels[47 + imStyle.width*2]);
    styleGeneric[DEFAULT_BASE_COLOR_DISABLED] = GetHexValue(pixels[48 + imStyle.width*3]);
    styleGeneric[DEFAULT_TEXT_COLOR_DISABLED] = GetHexValue(pixels[54 + imStyle.width*4]);
    
    // Update full style with generic values
    GuiUpdateStyleComplete();

    UnloadImage(imStyle);
}

// Load style from a color palette array (14 values required)
RAYGUIDEF void GuiLoadStylePalette(Color *palette)
{
    // Load generic style color palette
    styleGeneric[DEFAULT_BACKGROUND_COLOR] = GetHexValue(palette[0]);
    styleGeneric[DEFAULT_LINES_COLOR] = GetHexValue(palette[1]);
    styleGeneric[DEFAULT_BORDER_COLOR_NORMAL] = GetHexValue(palette[2]);
    styleGeneric[DEFAULT_BASE_COLOR_NORMAL] = GetHexValue(palette[3]);
    styleGeneric[DEFAULT_TEXT_COLOR_NORMAL] = GetHexValue(palette[4]);
    styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED] = GetHexValue(palette[5]);
    styleGeneric[DEFAULT_BASE_COLOR_FOCUSED] = GetHexValue(palette[6]);
    styleGeneric[DEFAULT_TEXT_COLOR_FOCUSED] = GetHexValue(palette[7]);
    styleGeneric[DEFAULT_BORDER_COLOR_PRESSED] = GetHexValue(palette[8]);
    styleGeneric[DEFAULT_BASE_COLOR_PRESSED] = GetHexValue(palette[9]);
    styleGeneric[DEFAULT_TEXT_COLOR_PRESSED] = GetHexValue(palette[10]);
    styleGeneric[DEFAULT_BORDER_COLOR_DISABLED] = GetHexValue(palette[11]);
    styleGeneric[DEFAULT_BASE_COLOR_DISABLED] = GetHexValue(palette[12]);
    styleGeneric[DEFAULT_TEXT_COLOR_DISABLED] = GetHexValue(palette[13]);
    
    // Update full style with generic values
    GuiUpdateStyleComplete();
}

// Updates full style property set with generic values
// NOTE: Requires globals styleGeneric[] and style[]
RAYGUIDEF void GuiUpdateStyleComplete(void)
{
    // Label
    style[LABEL_TEXT_COLOR_NORMAL] = styleGeneric[DEFAULT_TEXT_COLOR_NORMAL];
    style[LABEL_TEXT_COLOR_FOCUSED] = styleGeneric[DEFAULT_TEXT_COLOR_FOCUSED];
    style[LABEL_TEXT_COLOR_PRESSED] = styleGeneric[DEFAULT_TEXT_COLOR_PRESSED];
    style[LABEL_TEXT_COLOR_DISABLED] = styleGeneric[DEFAULT_TEXT_COLOR_DISABLED];
    // Button
    style[BUTTON_BORDER_WIDTH] = styleGeneric[DEFAULT_BORDER_WIDTH]*2;
    style[BUTTON_BORDER_COLOR_NORMAL] = styleGeneric[DEFAULT_BORDER_COLOR_NORMAL];
    style[BUTTON_BASE_COLOR_NORMAL] = styleGeneric[DEFAULT_BASE_COLOR_NORMAL];
    style[BUTTON_TEXT_COLOR_NORMAL] = styleGeneric[DEFAULT_TEXT_COLOR_NORMAL];
    style[BUTTON_BORDER_COLOR_FOCUSED] = styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED];
    style[BUTTON_BASE_COLOR_FOCUSED] = styleGeneric[DEFAULT_BASE_COLOR_FOCUSED];
    style[BUTTON_TEXT_COLOR_FOCUSED] = styleGeneric[DEFAULT_TEXT_COLOR_FOCUSED];
    style[BUTTON_BORDER_COLOR_PRESSED] = styleGeneric[DEFAULT_BORDER_COLOR_PRESSED];
    style[BUTTON_BASE_COLOR_PRESSED] = styleGeneric[DEFAULT_BASE_COLOR_PRESSED];
    style[BUTTON_TEXT_COLOR_PRESSED] = styleGeneric[DEFAULT_TEXT_COLOR_PRESSED];
    style[BUTTON_BORDER_COLOR_DISABLED] = styleGeneric[DEFAULT_BORDER_COLOR_DISABLED];
    style[BUTTON_BASE_COLOR_DISABLED] = styleGeneric[DEFAULT_BASE_COLOR_DISABLED];
    style[BUTTON_TEXT_COLOR_DISABLED] = styleGeneric[DEFAULT_TEXT_COLOR_DISABLED];
    // ToggleButton / ToggleGroup
    style[TOGGLE_BORDER_WIDTH] = styleGeneric[DEFAULT_BORDER_WIDTH];
    style[TOGGLE_BORDER_COLOR_NORMAL] = styleGeneric[DEFAULT_BORDER_COLOR_NORMAL];
    style[TOGGLE_BASE_COLOR_NORMAL] = styleGeneric[DEFAULT_BASE_COLOR_NORMAL];
    style[TOGGLE_TEXT_COLOR_NORMAL] = styleGeneric[DEFAULT_TEXT_COLOR_NORMAL];
    style[TOGGLE_BORDER_COLOR_FOCUSED] = styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED];
    style[TOGGLE_BASE_COLOR_FOCUSED] = styleGeneric[DEFAULT_BASE_COLOR_FOCUSED];
    style[TOGGLE_TEXT_COLOR_FOCUSED] = styleGeneric[DEFAULT_TEXT_COLOR_FOCUSED];
    style[TOGGLE_BORDER_COLOR_PRESSED] = styleGeneric[DEFAULT_BORDER_COLOR_PRESSED];
    style[TOGGLE_BASE_COLOR_PRESSED] = styleGeneric[DEFAULT_BASE_COLOR_PRESSED];
    style[TOGGLE_TEXT_COLOR_PRESSED] = styleGeneric[DEFAULT_TEXT_COLOR_PRESSED];
    style[TOGGLE_BORDER_COLOR_DISABLED] = styleGeneric[DEFAULT_BORDER_COLOR_DISABLED];
    style[TOGGLE_BASE_COLOR_DISABLED] = styleGeneric[DEFAULT_BASE_COLOR_DISABLED];
    style[TOGGLE_TEXT_COLOR_DISABLED] = styleGeneric[DEFAULT_TEXT_COLOR_DISABLED];
    style[TOGGLEGROUP_PADDING] = 2;
    // Slider
    style[SLIDER_BORDER_WIDTH] = styleGeneric[DEFAULT_BORDER_WIDTH];
    style[SLIDER_SLIDER_WIDTH] = 20;
    style[SLIDER_BORDER_COLOR_NORMAL] = styleGeneric[DEFAULT_BORDER_COLOR_NORMAL];
    style[SLIDER_BASE_COLOR_NORMAL] = styleGeneric[DEFAULT_BASE_COLOR_NORMAL];
    style[SLIDER_BORDER_COLOR_FOCUSED] = styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED];
    style[SLIDER_BASE_COLOR_FOCUSED] = styleGeneric[DEFAULT_BASE_COLOR_FOCUSED];
    style[SLIDER_BORDER_COLOR_PRESSED] = styleGeneric[DEFAULT_BORDER_COLOR_PRESSED];
    style[SLIDER_BASE_COLOR_PRESSED] = styleGeneric[DEFAULT_BASE_COLOR_PRESSED];
    style[SLIDER_BORDER_COLOR_DISABLED] = styleGeneric[DEFAULT_BORDER_COLOR_DISABLED];
    style[SLIDER_BASE_COLOR_DISABLED] = styleGeneric[DEFAULT_BASE_COLOR_DISABLED];
    // SliderBar
    style[SLIDERBAR_INNER_PADDING] = 1;
    style[SLIDERBAR_BORDER_WIDTH] = styleGeneric[DEFAULT_BORDER_WIDTH];
    style[SLIDERBAR_BORDER_COLOR_NORMAL] = styleGeneric[DEFAULT_BORDER_COLOR_NORMAL];
    style[SLIDERBAR_BASE_COLOR_NORMAL] = styleGeneric[DEFAULT_BASE_COLOR_NORMAL];
    style[SLIDERBAR_BORDER_COLOR_FOCUSED] = styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED];
    style[SLIDERBAR_BASE_COLOR_FOCUSED] = styleGeneric[DEFAULT_BASE_COLOR_FOCUSED];
    style[SLIDERBAR_BORDER_COLOR_PRESSED] = styleGeneric[DEFAULT_BORDER_COLOR_PRESSED];
    style[SLIDERBAR_BASE_COLOR_PRESSED] = styleGeneric[DEFAULT_BASE_COLOR_PRESSED];
    style[SLIDERBAR_BORDER_COLOR_DISABLED] = styleGeneric[DEFAULT_BORDER_COLOR_DISABLED];
    style[SLIDERBAR_BASE_COLOR_DISABLED] = styleGeneric[DEFAULT_BASE_COLOR_DISABLED];
    // ProgressBar
    style[PROGRESSBAR_INNER_PADDING] = 1;
    style[PROGRESSBAR_BORDER_WIDTH] = styleGeneric[DEFAULT_BORDER_WIDTH];
    style[PROGRESSBAR_BORDER_COLOR_NORMAL] = styleGeneric[DEFAULT_BORDER_COLOR_NORMAL];
    style[PROGRESSBAR_BASE_COLOR_NORMAL] = styleGeneric[DEFAULT_BASE_COLOR_PRESSED];
    style[PROGRESSBAR_BORDER_COLOR_FOCUSED] = styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED];
    style[PROGRESSBAR_BASE_COLOR_FOCUSED] = styleGeneric[DEFAULT_BASE_COLOR_FOCUSED];
    style[PROGRESSBAR_BORDER_COLOR_PRESSED] = styleGeneric[DEFAULT_BORDER_COLOR_PRESSED];
    style[PROGRESSBAR_BASE_COLOR_PRESSED] = styleGeneric[DEFAULT_BASE_COLOR_PRESSED];
    style[PROGRESSBAR_BORDER_COLOR_DISABLED] = styleGeneric[DEFAULT_BORDER_COLOR_DISABLED];
    style[PROGRESSBAR_BASE_COLOR_DISABLED] = styleGeneric[DEFAULT_BASE_COLOR_DISABLED];
    // Spinner
    style[SPINNER_BUTTON_PADDING] = 2;
    style[SPINNER_BUTTONS_WIDTH] = 35;
    style[SPINNER_BORDER_COLOR_NORMAL] = styleGeneric[DEFAULT_BORDER_COLOR_NORMAL];
    style[SPINNER_BASE_COLOR_NORMAL] = styleGeneric[DEFAULT_BASE_COLOR_NORMAL];
    style[SPINNER_TEXT_COLOR_NORMAL] = styleGeneric[DEFAULT_TEXT_COLOR_NORMAL];
    style[SPINNER_BORDER_COLOR_FOCUSED] = styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED];
    style[SPINNER_BASE_COLOR_FOCUSED] = styleGeneric[DEFAULT_BASE_COLOR_FOCUSED];
    style[SPINNER_TEXT_COLOR_FOCUSED] = styleGeneric[DEFAULT_TEXT_COLOR_FOCUSED];
    style[SPINNER_BORDER_COLOR_PRESSED] = styleGeneric[DEFAULT_BORDER_COLOR_PRESSED];
    style[SPINNER_BASE_COLOR_PRESSED] = styleGeneric[DEFAULT_BASE_COLOR_PRESSED];
    style[SPINNER_TEXT_COLOR_PRESSED] = styleGeneric[DEFAULT_TEXT_COLOR_PRESSED];
    style[SPINNER_BORDER_COLOR_DISABLED] = styleGeneric[DEFAULT_BORDER_COLOR_DISABLED];
    style[SPINNER_BASE_COLOR_DISABLED] = styleGeneric[DEFAULT_BASE_COLOR_DISABLED];
    style[SPINNER_TEXT_COLOR_DISABLED] = styleGeneric[DEFAULT_TEXT_COLOR_DISABLED];
    // ComboBox
    style[COMBOBOX_BORDER_WIDTH] = styleGeneric[DEFAULT_BORDER_WIDTH];
    style[COMBOBOX_BUTTON_PADDING] = 2;              
    style[COMBOBOX_SELECTOR_WIDTH] = 35;              
    style[COMBOBOX_BORDER_COLOR_NORMAL] = styleGeneric[DEFAULT_BORDER_COLOR_NORMAL];
    style[COMBOBOX_BASE_COLOR_NORMAL] = styleGeneric[DEFAULT_BASE_COLOR_NORMAL];
    style[COMBOBOX_TEXT_COLOR_NORMAL] = styleGeneric[DEFAULT_TEXT_COLOR_NORMAL];
    style[COMBOBOX_BORDER_COLOR_FOCUSED] = styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED];
    style[COMBOBOX_BASE_COLOR_FOCUSED] = styleGeneric[DEFAULT_BASE_COLOR_FOCUSED];
    style[COMBOBOX_TEXT_COLOR_FOCUSED] = styleGeneric[DEFAULT_TEXT_COLOR_FOCUSED];
    style[COMBOBOX_BORDER_COLOR_PRESSED] = styleGeneric[DEFAULT_BORDER_COLOR_PRESSED];
    style[COMBOBOX_BASE_COLOR_PRESSED] = styleGeneric[DEFAULT_BASE_COLOR_PRESSED];
    style[COMBOBOX_TEXT_COLOR_PRESSED] = styleGeneric[DEFAULT_TEXT_COLOR_PRESSED];
    style[COMBOBOX_BORDER_COLOR_DISABLED] = styleGeneric[DEFAULT_BORDER_COLOR_DISABLED];
    style[COMBOBOX_BASE_COLOR_DISABLED] = styleGeneric[DEFAULT_BASE_COLOR_DISABLED];
    style[COMBOBOX_TEXT_COLOR_DISABLED] = styleGeneric[DEFAULT_TEXT_COLOR_DISABLED];
    // CheckBox
    style[CHECKBOX_BORDER_WIDTH] = styleGeneric[DEFAULT_BORDER_WIDTH];
    style[CHECKBOX_INNER_PADDING] = 1;
    style[CHECKBOX_BORDER_COLOR_NORMAL] = styleGeneric[DEFAULT_BORDER_COLOR_NORMAL];
    style[CHECKBOX_BASE_COLOR_NORMAL] = styleGeneric[DEFAULT_BACKGROUND_COLOR];
    style[CHECKBOX_BORDER_COLOR_FOCUSED] = styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED];
    style[CHECKBOX_BASE_COLOR_FOCUSED] = styleGeneric[DEFAULT_TEXT_COLOR_FOCUSED];
    style[CHECKBOX_BORDER_COLOR_PRESSED] = styleGeneric[DEFAULT_BORDER_COLOR_PRESSED];
    style[CHECKBOX_BASE_COLOR_PRESSED] = styleGeneric[DEFAULT_TEXT_COLOR_PRESSED];
    style[CHECKBOX_BORDER_COLOR_DISABLED] = styleGeneric[DEFAULT_BORDER_COLOR_DISABLED];
    style[CHECKBOX_BASE_COLOR_DISABLED] = styleGeneric[DEFAULT_BASE_COLOR_DISABLED];
    // TextBox
    style[TEXTBOX_BORDER_WIDTH] = styleGeneric[DEFAULT_BORDER_WIDTH];
    style[TEXTBOX_BORDER_COLOR_NORMAL] = styleGeneric[DEFAULT_BORDER_COLOR_NORMAL];
    style[TEXTBOX_BASE_COLOR_NORMAL] = styleGeneric[DEFAULT_BACKGROUND_COLOR];
    style[TEXTBOX_TEXT_COLOR_NORMAL] = styleGeneric[DEFAULT_TEXT_COLOR_NORMAL];
    style[TEXTBOX_BORDER_COLOR_FOCUSED] = styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED];
    style[TEXTBOX_BASE_COLOR_FOCUSED] = styleGeneric[DEFAULT_BASE_COLOR_FOCUSED];
    style[TEXTBOX_TEXT_COLOR_FOCUSED] = styleGeneric[DEFAULT_TEXT_COLOR_FOCUSED];
    style[TEXTBOX_BORDER_COLOR_PRESSED] = styleGeneric[DEFAULT_BORDER_COLOR_PRESSED];
    style[TEXTBOX_BASE_COLOR_PRESSED] = styleGeneric[DEFAULT_BASE_COLOR_PRESSED];
    style[TEXTBOX_TEXT_COLOR_PRESSED] = styleGeneric[DEFAULT_TEXT_COLOR_PRESSED];
    style[TEXTBOX_BORDER_COLOR_DISABLED] = styleGeneric[DEFAULT_BORDER_COLOR_DISABLED];
    style[TEXTBOX_BASE_COLOR_DISABLED] = styleGeneric[DEFAULT_BASE_COLOR_DISABLED];
    style[TEXTBOX_TEXT_COLOR_DISABLED] = styleGeneric[DEFAULT_TEXT_COLOR_DISABLED];
    // ColorPicker
    style[COLORPICKER_BARS_THICK] = 20;
    style[COLORPICKER_BARS_PADDING] = 10;
    style[COLORPICKER_BORDER_COLOR_NORMAL] = styleGeneric[DEFAULT_BORDER_COLOR_NORMAL];
    style[COLORPICKER_BASE_COLOR_NORMAL] = styleGeneric[DEFAULT_BACKGROUND_COLOR];
    style[COLORPICKER_BORDER_COLOR_FOCUSED] = styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED];
    style[COLORPICKER_BASE_COLOR_FOCUSED] = styleGeneric[DEFAULT_TEXT_COLOR_FOCUSED];
    style[COLORPICKER_BORDER_COLOR_PRESSED] = styleGeneric[DEFAULT_BORDER_COLOR_PRESSED];
    style[COLORPICKER_BASE_COLOR_PRESSED] = styleGeneric[DEFAULT_TEXT_COLOR_PRESSED];
    style[COLORPICKER_BORDER_COLOR_DISABLED] = styleGeneric[DEFAULT_BORDER_COLOR_DISABLED];
    style[COLORPICKER_BASE_COLOR_DISABLED] = styleGeneric[DEFAULT_BASE_COLOR_DISABLED];
    // ListView
    style[LISTVIEW_ELEMENTS_HEIGHT] = 30;
    style[LISTVIEW_ELEMENTS_PADDING] = 2;
    style[LISTVIEW_BAR_WIDTH] = 10;
    style[LISTVIEW_BORDER_COLOR_NORMAL] = styleGeneric[DEFAULT_BORDER_COLOR_NORMAL];
    style[LISTVIEW_BASE_COLOR_NORMAL] = styleGeneric[DEFAULT_BACKGROUND_COLOR];
    style[LISTVIEW_TEXT_COLOR_NORMAL] = styleGeneric[DEFAULT_TEXT_COLOR_NORMAL];
    style[LISTVIEW_BORDER_COLOR_FOCUSED] = styleGeneric[DEFAULT_BORDER_COLOR_FOCUSED];
    style[LISTVIEW_BASE_COLOR_FOCUSED] = styleGeneric[DEFAULT_BASE_COLOR_FOCUSED];
    style[LISTVIEW_TEXT_COLOR_FOCUSED] = styleGeneric[DEFAULT_TEXT_COLOR_FOCUSED];
    style[LISTVIEW_BORDER_COLOR_PRESSED] = styleGeneric[DEFAULT_BORDER_COLOR_PRESSED];
    style[LISTVIEW_BASE_COLOR_PRESSED] = styleGeneric[DEFAULT_BASE_COLOR_PRESSED];
    style[LISTVIEW_TEXT_COLOR_PRESSED] = styleGeneric[DEFAULT_TEXT_COLOR_PRESSED];
    style[LISTVIEW_BORDER_COLOR_DISABLED] = styleGeneric[DEFAULT_BORDER_COLOR_DISABLED];
    style[LISTVIEW_BASE_COLOR_DISABLED] = styleGeneric[DEFAULT_BASE_COLOR_DISABLED];
    style[LISTVIEW_TEXT_COLOR_DISABLED] = styleGeneric[DEFAULT_TEXT_COLOR_DISABLED];
}

// Save current GUI style into a text file
RAYGUIDEF void GuiSaveStyle(const char *fileName)
{
    FILE *styleFile = fopen(fileName, "wt");

    //for (int i = 0; i < NUM_PROPERTIES; i++) fprintf(styleFile, "%-40s0x%x\n", guiPropertyName[i], GuiGetStyleProperty(i));
    if (styleFile != NULL) 
    {
        for (int i = 0; i < NUM_PROPERTIES; i++) fprintf(styleFile, "0x%x\n", GuiGetStyleProperty(i));
    
        fclose(styleFile);
    }
}

// Load GUI style from a text file
RAYGUIDEF void GuiLoadStyle(const char *fileName)
{
    int counter = 0;
    
    FILE *styleFile = fopen(fileName, "rt");
    
    if (styleFile != NULL)
    {
        while (!feof(styleFile))
        {
            fscanf(styleFile, "0x%x\n", &style[counter]);
            counter++;
        }
        
        //if (counter < NUM_PROPERTIES) TraceLog(LOG_WARNING, "loaded properties do not match style file properties...");

        fclose(styleFile);
    }
}

// Set one style property value
RAYGUIDEF void GuiSetStyleProperty(int guiProperty, int value) { style[guiProperty] = value; }

// Get one style property value
RAYGUIDEF int GuiGetStyleProperty(int guiProperty) { return style[guiProperty]; }

#endif  // defined(RAYGUI_STYLE_SAVE_LOAD)

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------

// Convert color data from RGB to HSV
// NOTE: Color data should be passed normalized
static Vector3 ConvertRGBtoHSV(Vector3 rgb)
{
    Vector3 hsv;
    float min, max, delta;

    min = rgb.x < rgb.y ? rgb.x : rgb.y;
    min = min  < rgb.z ? min  : rgb.z;

    max = rgb.x > rgb.y ? rgb.x : rgb.y;
    max = max  > rgb.z ? max  : rgb.z;

    hsv.z = max;            // Value
    delta = max - min;
    
    if (delta < 0.00001f)
    {
        hsv.y = 0.0f;
        hsv.x = 0.0f;       // Undefined, maybe NAN?
        return hsv;
    }
    
    if (max > 0.0f) 
    {
        // NOTE: If max is 0, this divide would cause a crash
        hsv.y = (delta/max);    // Saturation
    } 
    else 
    {
        // NOTE: If max is 0, then r = g = b = 0, s = 0, h is undefined
        hsv.y = 0.0f;
        hsv.x = NAN;        // Undefined
        return hsv;
    }
    
    // NOTE: Comparing float values could not work properly
    if (rgb.x >= max) hsv.x = (rgb.y - rgb.z)/delta;    // Between yellow & magenta
    else
    {
        if (rgb.y >= max) hsv.x = 2.0f + (rgb.z - rgb.x)/delta;  // Between cyan & yellow
        else hsv.x = 4.0f + (rgb.x - rgb.y)/delta;      // Between magenta & cyan
    }
    
    hsv.x *= 60.0f;     // Convert to degrees

    if (hsv.x < 0.0f) hsv.x += 360.0f;

    return hsv;
}

// Convert color data from HSV to RGB
// NOTE: Color data should be passed normalized
static Vector3 ConvertHSVtoRGB(Vector3 hsv)
{
    Vector3 rgb;
    float hh, p, q, t, ff;
    long i;

    // NOTE: Comparing float values could not work properly
    if (hsv.y <= 0.0f) 
    {
        rgb.x = hsv.z;
        rgb.y = hsv.z;
        rgb.z = hsv.z;
        return rgb;
    }
    
    hh = hsv.x;
    if (hh >= 360.0f) hh = 0.0f;
    hh /= 60.0f;
    
    i = (long)hh;
    ff = hh - i;
    p = hsv.z*(1.0f - hsv.y);
    q = hsv.z*(1.0f - (hsv.y*ff));
    t = hsv.z*(1.0f - (hsv.y*(1.0f - ff)));

    switch (i) 
    {
        case 0:
        {
            rgb.x = hsv.z;
            rgb.y = t;
            rgb.z = p;
        } break;
        case 1:
        {
            rgb.x = q;
            rgb.y = hsv.z;
            rgb.z = p;
        } break;
        case 2:
        {
            rgb.x = p;
            rgb.y = hsv.z;
            rgb.z = t;
        } break;
        case 3:
        {
            rgb.x = p;
            rgb.y = q;
            rgb.z = hsv.z;
        } break;
        case 4:
        {
            rgb.x = t;
            rgb.y = p;
            rgb.z = hsv.z;
        } break;
        case 5:
        default:
        {
            rgb.x = hsv.z;
            rgb.y = p;
            rgb.z = q;
        } break;
    }
    
    return rgb;     
}

#if defined(RAYGUI_STANDALONE)
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

// Color fade-in or fade-out, alpha goes from 0.0f to 1.0f
Color Fade(Color color, float alpha)
{
    if (alpha < 0.0f) alpha = 0.0f;
    else if (alpha > 1.0f) alpha = 1.0f;

    return (Color){color.r, color.g, color.b, (unsigned char)(255.0f*alpha)};
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
