/*******************************************************************************************
*
*   raygui v2.0 - A simple and easy-to-use immedite-mode-gui library for raylib (www.raylib.com)
*
*   DESCRIPTION:
*
*   raygui is a library for creating simple IMGUI interfaces using raylib, intended for tools dev. 
*   
*   Basic controls provided:
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
*   LIMITATIONS:
*       Some controls missing, like panels.
*
*   VERSIONS HISTORY:
*       2.0 (Jan-2018)    Added new controls.
*       1.5 (21-Jun-2017) Working in an improved styles system
*       1.4 (15-Jun-2017) Rewritten all GUI functions (removed useless ones)
*       1.3 (12-Jun-2017) Redesigned styles system
*       1.1 (01-Jun-2017) Complete review of the library
*       1.0 (07-Jun-2016) Converted to header-only by Ramon Santamaria.
*       0.9 (07-Mar-2016) Reviewed and tested by Albert Martos, Ian Eito, Sergio Martinez and Ramon Santamaria.
*       0.8 (27-Aug-2015) Initial release. Implemented by Kevin Gato, Daniel Nicolás and Ramon Santamaria.
*
*   CONTRIBUTORS:
*       Ramon Santamaria:   Supervision, review, redesign, update and maintenance...
*       Kevin Gato:         Initial implementation of basic components (2014)
*       Daniel Nicolas:     Initial implementation of basic components (2014)
*       Albert Martos:      Review and testing of the library (2015)
*       Ian Eito:           Review and testing of the library (2015)
*       Sergio Martinez:    Review and testing of the library (2015)
*       Adria Arranz:       Testing and Implementation of additional controls (2018)
*       Jordi Jorba:        Testing and Implementation of additional controls (2018)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2018 Ramon Santamaria (@raysan5)
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
#define NUM_PROPERTIES       155

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

// Gui properties enumeration
typedef enum GuiProperty {
    //--------------------------------------------
    // NOTE: This first set of properties is for general style,
    // following control-specific properties overwritte those styles
    DEFAULT_BACKGROUND_COLOR = 0,
    DEFAULT_LINES_COLOR,
    DEFAULT_TEXT_FONT,
    DEFAULT_TEXT_SIZE,
    DEFAULT_BORDER_WIDTH,
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
    //--------------------------------------------
    // Label
    LABEL_TEXT_COLOR_NORMAL,
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
    LISTVIEW_TEXT_COLOR_DISABLED
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
RAYGUIDEF void GuiStatusBar(Rectangle bounds, const char *text, int offsetX);                           // Status Bar control, shows info text
RAYGUIDEF bool GuiWindowBox(Rectangle bounds, const char *text);                                        // Window Box control, shows a window that can be closed
RAYGUIDEF bool GuiMessageBox(Rectangle bounds, const char *windowTitle, const char *message);           // Message Box control, displays a message
RAYGUIDEF void GuiDummyRec(Rectangle bounds, const char *text);                                         // Dummy control for placeholders
RAYGUIDEF bool GuiButton(Rectangle bounds, const char *text);                                           // Button control, returns true when clicked
RAYGUIDEF bool GuiLabelButton(Rectangle bounds, const char *text);                                      // Label button control, show true when clicked
RAYGUIDEF bool GuiImageButton(Rectangle bounds, Texture2D texture);                                     // Image button control, returns true when clicked
RAYGUIDEF bool GuiImageButtonEx(Rectangle bounds, Texture2D texture, Rectangle texSource, const char *text); // Image button extended control, returns true when clicked
RAYGUIDEF bool GuiToggleButton(Rectangle bounds, const char *text, bool toggle);                        // Toggle Button control, returns true when active
RAYGUIDEF int GuiToggleGroup(Rectangle bounds, const char **text, int count, int active);               // Toggle Group control, returns toggled button index
RAYGUIDEF bool GuiCheckBox(Rectangle bounds, bool checked);                                             // Check Box control, returns true when active
RAYGUIDEF int GuiComboBox(Rectangle bounds, const char **text, int count, int active);                  // Combo Box control, returns selected item index
RAYGUIDEF int GuiDropdownBox(Rectangle bounds, const char **text, int count, int active);               // Dropdown Box control, returns selected item
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
RAYGUIDEF void GuiSaveStyle(const char *fileName, bool binary);         // Save style file (.rgs), text or binary
RAYGUIDEF void GuiLoadStyle(const char *fileName);                      // Load style file (.rgs), text or binary
RAYGUIDEF void GuiLoadStyleImage(const char *fileName);                 // Load style from an image style file
RAYGUIDEF void GuiLoadStylePalette(Color *palette);                     // Load style from a color palette array (14 values required)
RAYGUIDEF void GuiUpdateStyleComplete(void);                            // Updates full style properties set with generic values

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

// Current GUI style (light or dark)
static int style[NUM_PROPERTIES] = {
#if defined(RAYGUI_STYLE_DEFAULT_LIGHT)
    //----------------------------------------------
    0xf5f5f5ff,     // DEFAULT_BACKGROUND_COLOR
    0x90abb5ff,     // DEFAULT_LINES_COLOR
    0,              // DEFAULT_TEXT_FONT
    10,             // DEFAULT_TEXT_SIZE
    1,              // DEFAULT_BORDER_WIDTH
    0x838383ff,     // DEFAULT_BORDER_COLOR_NORMAL
    0xc9c9c9ff,     // DEFAULT_BASE_COLOR_NORMAL
    0x686868ff,     // DEFAULT_TEXT_COLOR_NORMAL
    0x5bb2d9ff,     // DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // DEFAULT_TEXT_COLOR_FOCUSED
    0x0492c7ff,     // DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // DEFAULT_BASE_COLOR_PRESSED
    0x368bafff,     // DEFAULT_TEXT_COLOR_PRESSED
    0xb5c1c2ff,     // DEFAULT_BORDER_COLOR_DISABLED
    0xe6e9e9ff,     // DEFAULT_BASE_COLOR_DISABLED
    0xaeb7b8ff,     // DEFAULT_TEXT_COLOR_DISABLED
    //----------------------------------------------
    0x686868ff,     // LABEL_TEXT_COLOR_NORMAL -----> DEFAULT_TEXT_COLOR_NORMAL
    0x6c9bbcff,     // LABEL_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x368bafff,     // LABEL_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0xaeb7b8ff,     // LABEL_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x2,            // BUTTON_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x838383ff,     // BUTTON_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc9c9c9ff,     // BUTTON_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x686868ff,     // BUTTON_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x5bb2d9ff,     // BUTTON_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // BUTTON_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // BUTTON_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x0492c7ff,     // BUTTON_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // BUTTON_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x368bafff,     // BUTTON_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0xb5c1c2ff,     // BUTTON_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0xe6e9e9ff,     // BUTTON_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0xaeb7b8ff,     // BUTTON_TEXT_COLOR_DISABLED ----> DEFAULT_COLOR_COLOR_DISABLED
    0x1,            // TOGGLE_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x838383ff,     // TOGGLE_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc9c9c9ff,     // TOGGLE_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x686868ff,     // TOGGLE_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x5bb2d9ff,     // TOGGLE_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // TOGGLE_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // TOGGLE_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x0492c7ff,     // TOGGLE_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // TOGGLE_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x368bafff,     // TOGGLE_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0xb5c1c2ff,     // TOGGLE_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0xe6e9e9ff,     // TOGGLE_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0xaeb7b8ff,     // TOGGLE_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x2,            // TOGGLEGROUP_PADDING
    0x1,            // SLIDER_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x14,           // SLIDER_SLIDER_WIDTH 
    0x838383ff,     // SLIDER_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc9c9c9ff,     // SLIDER_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x5bb2d9ff,     // SLIDER_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // SLIDER_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x0492c7ff,     // SLIDER_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // SLIDER_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0xb5c1c2ff,     // SLIDER_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0xe6e9e9ff,     // SLIDER_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x1,            // SLIDERBAR_INNER_PADDING
    0x1,            // SLIDERBAR_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x838383ff,     // SLIDERBAR_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc9c9c9ff,     // SLIDERBAR_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x5bb2d9ff,     // SLIDERBAR_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // SLIDERBAR_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x0492c7ff,     // SLIDERBAR_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // SLIDERBAR_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0xb5c1c2ff,     // SLIDERBAR_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0xe6e9e9ff,     // SLIDERBAR_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x1,            // PROGRESSBAR_INNER_PADDING
    0x1,            // PROGRESSBAR_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x838383ff,     // PROGRESSBAR_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0x97e8ffff,     // PROGRESSBAR_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_PRESSED
    0x5bb2d9ff,     // PROGRESSBAR_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // PROGRESSBAR_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x0492c7ff,     // PROGRESSBAR_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // PROGRESSBAR_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0xb5c1c2ff,     // PROGRESSBAR_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0xe6e9e9ff,     // PROGRESSBAR_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x2,            // SPINNER_BUTTON_PADDING
    0x23,           // SPINNER_BUTTONS_WIDTH
    0x838383ff,     // SPINNER_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc9c9c9ff,     // SPINNER_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x686868ff,     // SPINNER_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x5bb2d9ff,     // SPINNER_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // SPINNER_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // SPINNER_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x0492c7ff,     // SPINNER_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // SPINNER_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x368bafff,     // SPINNER_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0xb5c1c2ff,     // SPINNER_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0xe6e9e9ff,     // SPINNER_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0xaeb7b8ff,     // SPINNER_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x1,            // COMBOBOX_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x2,            // COMBOBOX_BUTTON_PADDING
    0x23,           // COMBOBOX_SELECTOR_WIDTH
    0x838383ff,     // COMBOBOX_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xc9c9c9ff,     // COMBOBOX_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x686868ff,     // COMBOBOX_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x5bb2d9ff,     // COMBOBOX_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // COMBOBOX_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // COMBOBOX_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x0492c7ff,     // COMBOBOX_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // COMBOBOX_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x368bafff,     // COMBOBOX_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0xb5c1c2ff,     // COMBOBOX_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0xe6e9e9ff,     // COMBOBOX_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0xaeb7b8ff,     // COMBOBOX_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x1,            // CHECKBOX_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x1,            // CHECKBOX_INNER_PADDING
    0x838383ff,     // CHECKBOX_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xf5f5f5ff,     // CHECKBOX_BASE_COLOR_NORMAL ----> DEFAULT_BACKGROUND_COLOR
    0x5bb2d9ff,     // CHECKBOX_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x6c9bbcff,     // CHECKBOX_BASE_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x0492c7ff,     // CHECKBOX_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x368bafff,     // CHECKBOX_BASE_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0xb5c1c2ff,     // CHECKBOX_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0xe6e9e9ff,     // CHECKBOX_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x1,            // TEXTBOX_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x838383ff,     // TEXTBOX_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xf5f5f5ff,     // TEXTBOX_BASE_COLOR_NORMAL ----> DEFAULT_BACKGROUND_COLOR
    0x686868ff,     // TEXTBOX_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x5bb2d9ff,     // TEXTBOX_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // TEXTBOX_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // TEXTBOX_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x0492c7ff,     // TEXTBOX_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // TEXTBOX_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x368bafff,     // TEXTBOX_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0xb5c1c2ff,     // TEXTBOX_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0xe6e9e9ff,     // TEXTBOX_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0xaeb7b8ff,     // TEXTBOX_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x14,           // COLORPICKER_BARS_THICK
    0xa,            // COLORPICKER_BARS_PADDING
    0x838383ff,     // COLORPICKER_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xf5f5f5ff,     // COLORPICKER_BASE_COLOR_NORMAL ----> DEFAULT_BACKGROUND_COLOR
    0x5bb2d9ff,     // COLORPICKER_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x6c9bbcff,     // COLORPICKER_BASE_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x0492c7ff,     // COLORPICKER_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x368bafff,     // COLORPICKER_BASE_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0xb5c1c2ff,     // COLORPICKER_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0xe6e9e9ff,     // COLORPICKER_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x1e,           // LISTVIEW_ELEMENTS_HEIGHT
    0x2,            // LISTVIEW_ELEMENTS_PADDING
    0xa,            // LISTVIEW_BAR_WIDTH
    0x838383ff,     // LISTVIEW_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0xf5f5f5ff,     // LISTVIEW_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x686868ff,     // LISTVIEW_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x5bb2d9ff,     // LISTVIEW_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0xc9effeff,     // LISTVIEW_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6c9bbcff,     // LISTVIEW_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x0492c7ff,     // LISTVIEW_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97e8ffff,     // LISTVIEW_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x368bafff,     // LISTVIEW_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0xb5c1c2ff,     // LISTVIEW_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0xe6e9e9ff,     // LISTVIEW_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0xaeb7b8ff,     // LISTVIEW_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
#elif defined(RAYGUI_STYLE_DEFAULT_DARK)
    //----------------------------------------------
    0x2b3a3aff,     // DEFAULT_BACKGROUND_COLOR
    0x638465ff,     // DEFAULT_LINES_COLOR
    0,              // DEFAULT_TEXT_FONT
    10,             // DEFAULT_TEXT_SIZE
    1,              // DEFAULT_BORDER_WIDTH
    0x60827dff,     // DEFAULT_BORDER_COLOR_NORMAL
    0x2c3334ff,     // DEFAULT_BASE_COLOR_NORMAL
    0x82a29fff,     // DEFAULT_TEXT_COLOR_NORMAL
    0x5f9aa8ff,     // DEFAULT_BORDER_COLOR_FOCUSED
    0x334e57ff,     // DEFAULT_BASE_COLOR_FOCUSED
    0x6aa9b8ff,     // DEFAULT_TEXT_COLOR_FOCUSED
    0xa9cb8dff,     // DEFAULT_BORDER_COLOR_PRESSED
    0x3b6357ff,     // DEFAULT_BASE_COLOR_PRESSED
    0x97af81ff,     // DEFAULT_TEXT_COLOR_PRESSED
    0x5b6462ff,     // DEFAULT_BORDER_COLOR_DISABLED
    0x2c3334ff,     // DEFAULT_BASE_COLOR_DISABLED
    0x666b69ff,     // DEFAULT_TEXT_COLOR_DISABLED
    //----------------------------------------------
    0x82a29fff,     // LABEL_TEXT_COLOR_NORMAL -----> DEFAULT_TEXT_COLOR_NORMAL
    0x6aa9b8ff,     // LABEL_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0x97af81ff,     // LABEL_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x666b69ff,     // LABEL_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x2,            // BUTTON_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x60827dff,     // BUTTON_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0x2c3334ff,     // BUTTON_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x82a29fff,     // BUTTON_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x5f9aa8ff,     // BUTTON_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x334e57ff,     // BUTTON_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6aa9b8ff,     // BUTTON_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0xa9cb8dff,     // BUTTON_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x3b6357ff,     // BUTTON_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x97af81ff,     // BUTTON_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x5b6462ff,     // BUTTON_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x2c3334ff,     // BUTTON_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x666b69ff,     // BUTTON_TEXT_COLOR_DISABLED ----> DEFAULT_COLOR_COLOR_DISABLED
    0x1,            // TOGGLE_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x60827dff,     // TOGGLE_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0x2c3334ff,     // TOGGLE_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x82a29fff,     // TOGGLE_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x5f9aa8ff,     // TOGGLE_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x334e57ff,     // TOGGLE_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6aa9b8ff,     // TOGGLE_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0xa9cb8dff,     // TOGGLE_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x3b6357ff,     // TOGGLE_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x97af81ff,     // TOGGLE_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x5b6462ff,     // TOGGLE_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x2c3334ff,     // TOGGLE_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x666b69ff,     // TOGGLE_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x2,            // TOGGLEGROUP_PADDING
    0x1,            // SLIDER_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x14,           // SLIDER_SLIDER_WIDTH 
    0x60827dff,     // SLIDER_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0x2c3334ff,     // SLIDER_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x5f9aa8ff,     // SLIDER_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x334e57ff,     // SLIDER_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0xa9cb8dff,     // SLIDER_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x3b6357ff,     // SLIDER_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x5b6462ff,     // SLIDER_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x2c3334ff,     // SLIDER_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x1,            // SLIDERBAR_INNER_PADDING
    0x1,            // SLIDERBAR_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x60827dff,     // SLIDERBAR_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0x2c3334ff,     // SLIDERBAR_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x5f9aa8ff,     // SLIDERBAR_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x334e57ff,     // SLIDERBAR_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0xa9cb8dff,     // SLIDERBAR_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x3b6357ff,     // SLIDERBAR_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x5b6462ff,     // SLIDERBAR_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x2c3334ff,     // SLIDERBAR_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x1,            // PROGRESSBAR_INNER_PADDING
    0x1,            // PROGRESSBAR_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x60827dff,     // PROGRESSBAR_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0x3b6357ff,     // PROGRESSBAR_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_PRESSED
    0x5f9aa8ff,     // PROGRESSBAR_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x334e57ff,     // PROGRESSBAR_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0xa9cb8dff,     // PROGRESSBAR_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x3b6357ff,     // PROGRESSBAR_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x5b6462ff,     // PROGRESSBAR_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x2c3334ff,     // PROGRESSBAR_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x2,            // SPINNER_BUTTON_PADDING
    0x23,           // SPINNER_BUTTONS_WIDTH
    0x60827dff,     // SPINNER_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0x2c3334ff,     // SPINNER_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x82a29fff,     // SPINNER_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x5f9aa8ff,     // SPINNER_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x334e57ff,     // SPINNER_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6aa9b8ff,     // SPINNER_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0xa9cb8dff,     // SPINNER_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x3b6357ff,     // SPINNER_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x97af81ff,     // SPINNER_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x5b6462ff,     // SPINNER_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x2c3334ff,     // SPINNER_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x666b69ff,     // SPINNER_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x1,            // COMBOBOX_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x2,            // COMBOBOX_BUTTON_PADDING
    0x23,           // COMBOBOX_SELECTOR_WIDTH
    0x60827dff,     // COMBOBOX_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0x2c3334ff,     // COMBOBOX_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x82a29fff,     // COMBOBOX_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x5f9aa8ff,     // COMBOBOX_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x334e57ff,     // COMBOBOX_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6aa9b8ff,     // COMBOBOX_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0xa9cb8dff,     // COMBOBOX_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x3b6357ff,     // COMBOBOX_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x97af81ff,     // COMBOBOX_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x5b6462ff,     // COMBOBOX_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x2c3334ff,     // COMBOBOX_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x666b69ff,     // COMBOBOX_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x1,            // CHECKBOX_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x1,            // CHECKBOX_INNER_PADDING
    0x60827dff,     // CHECKBOX_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0x2b3a3aff,     // CHECKBOX_BASE_COLOR_NORMAL ----> DEFAULT_BACKGROUND_COLOR
    0x5f9aa8ff,     // CHECKBOX_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x6aa9b8ff,     // CHECKBOX_BASE_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0xa9cb8dff,     // CHECKBOX_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97af81ff,     // CHECKBOX_BASE_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x5b6462ff,     // CHECKBOX_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x2c3334ff,     // CHECKBOX_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x1,            // TEXTBOX_BORDER_WIDTH ----> DEFAULT_BORDER_WIDTH
    0x60827dff,     // TEXTBOX_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0x2b3a3aff,     // TEXTBOX_BASE_COLOR_NORMAL ----> DEFAULT_BACKGROUND_COLOR
    0x82a29fff,     // TEXTBOX_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x5f9aa8ff,     // TEXTBOX_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x334e57ff,     // TEXTBOX_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6aa9b8ff,     // TEXTBOX_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0xa9cb8dff,     // TEXTBOX_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x3b6357ff,     // TEXTBOX_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x97af81ff,     // TEXTBOX_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x5b6462ff,     // TEXTBOX_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x2c3334ff,     // TEXTBOX_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x666b69ff,     // TEXTBOX_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
    0x14,           // COLORPICKER_BARS_THICK
    0xa,            // COLORPICKER_BARS_PADDING
    0x60827dff,     // COLORPICKER_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0x2b3a3aff,     // COLORPICKER_BASE_COLOR_NORMAL ----> DEFAULT_BACKGROUND_COLOR
    0x5f9aa8ff,     // COLORPICKER_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x6aa9b8ff,     // COLORPICKER_BASE_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0xa9cb8dff,     // COLORPICKER_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x97af81ff,     // COLORPICKER_BASE_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x5b6462ff,     // COLORPICKER_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x2c3334ff,     // COLORPICKER_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x1e,           // LISTVIEW_ELEMENTS_HEIGHT
    0x2,            // LISTVIEW_ELEMENTS_PADDING
    0xa,            // LISTVIEW_BAR_WIDTH
    0x60827dff,     // LISTVIEW_BORDER_COLOR_NORMAL ----> DEFAULT_BORDER_COLOR_NORMAL
    0x2b3a3aff,     // LISTVIEW_BASE_COLOR_NORMAL ----> DEFAULT_BASE_COLOR_NORMAL
    0x82a29fff,     // LISTVIEW_TEXT_COLOR_NORMAL ----> DEFAULT_TEXT_COLOR_NORMAL
    0x5f9aa8ff,     // LISTVIEW_BORDER_COLOR_FOCUSED ----> DEFAULT_BORDER_COLOR_FOCUSED
    0x334e57ff,     // LISTVIEW_BASE_COLOR_FOCUSED ----> DEFAULT_BASE_COLOR_FOCUSED
    0x6aa9b8ff,     // LISTVIEW_TEXT_COLOR_FOCUSED ----> DEFAULT_TEXT_COLOR_FOCUSED
    0xa9cb8dff,     // LISTVIEW_BORDER_COLOR_PRESSED ----> DEFAULT_BORDER_COLOR_PRESSED
    0x3b6357ff,     // LISTVIEW_BASE_COLOR_PRESSED ----> DEFAULT_BASE_COLOR_PRESSED
    0x97af81ff,     // LISTVIEW_TEXT_COLOR_PRESSED ----> DEFAULT_TEXT_COLOR_PRESSED
    0x5b6462ff,     // LISTVIEW_BORDER_COLOR_DISABLED ----> DEFAULT_BORDER_COLOR_DISABLED
    0x2c3334ff,     // LISTVIEW_BASE_COLOR_DISABLED ----> DEFAULT_BASE_COLOR_DISABLED
    0x666b69ff,     // LISTVIEW_TEXT_COLOR_DISABLED ----> DEFAULT_TEXT_COLOR_DISABLED
#endif
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
static int ColorToInt(Color color);            // Returns hexadecimal value for a Color
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
RAYGUIDEF Color GuiBackgroundColor(void) { return GetColor(style[DEFAULT_BACKGROUND_COLOR]); }

// Get lines color
RAYGUIDEF Color GuiLinesColor(void) { return GetColor(style[DEFAULT_LINES_COLOR]); }

// Get text color for normal state
RAYGUIDEF Color GuiTextColor(void) { return GetColor(style[DEFAULT_TEXT_COLOR_NORMAL]); }

// Label control
RAYGUIDEF void GuiLabel(Rectangle bounds, const char *text)
{
    GuiControlState state = guiState;

    int textWidth = MeasureText(text, style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];

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
        case PRESSED: DrawText(text, bounds.x, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_NORMAL])); break;
        case DISABLED: DrawText(text, bounds.x, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_DISABLED])); break;
        default: break;
    }
    //--------------------------------------------------------------------
}

// Status Bar control
RAYGUIDEF void GuiStatusBar(Rectangle bounds, const char *text, int offsetX)
{
    #define STATUSBAR_THICK 1
    
    GuiControlState state = guiState;

    int textWidth = MeasureText(text, style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];

    if (bounds.width < textWidth + offsetX) bounds.width = textWidth + offsetX;
    if (bounds.height < textHeight) bounds.height = textHeight;
    
    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL: 
        case FOCUSED:
        case PRESSED:
        {
            DrawRectangleRec(bounds, GuiLinesColor());
            DrawRectangleRec((Rectangle){ bounds.x + STATUSBAR_THICK, bounds.y + STATUSBAR_THICK, bounds.width - STATUSBAR_THICK*2, bounds.height - STATUSBAR_THICK*2 }, GetColor(style[DEFAULT_BASE_COLOR_NORMAL]));
            DrawText(text, bounds.x + offsetX, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[DEFAULT_TEXT_COLOR_NORMAL]));         
        } break;   
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[DEFAULT_BORDER_COLOR_DISABLED]));
            DrawRectangleRec((Rectangle){ bounds.x + STATUSBAR_THICK, bounds.y + STATUSBAR_THICK, bounds.width - STATUSBAR_THICK*2, bounds.height - STATUSBAR_THICK*2 }, GetColor(style[DEFAULT_BASE_COLOR_DISABLED]));
            DrawText(text, bounds.x + offsetX, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[DEFAULT_TEXT_COLOR_DISABLED]));         
        } break;
        default: break;
    }
    //--------------------------------------------------------------------
}

// Window Box control
RAYGUIDEF bool GuiWindowBox(Rectangle bounds, const char *text)
{
    #define WINDOWBOX_PADDING 2
    
    bool clicked = false;
    GuiControlState state = guiState;

    int offsetX = 10;
    int textWidth = MeasureText(text, style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];

    if (bounds.width < textWidth + offsetX*2 + 16) bounds.width = textWidth + offsetX*2 + 16;
    Rectangle statusBar = { bounds.x, bounds.y, bounds.width, 24 };
    if (bounds.height < 48) bounds.height = 48;
    
    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check button state
        if (CheckCollisionPointRec(mousePoint, (Rectangle){statusBar.x + statusBar.width - 24, statusBar.y, 20, statusBar.height }))
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
            DrawRectangleRec(bounds, GetColor(style[DEFAULT_BORDER_COLOR_NORMAL]));
            DrawRectangleRec((Rectangle){ bounds.x + 1, bounds.y + 1, bounds.width - 2, bounds.height -2 }, GetColor(style[DEFAULT_BACKGROUND_COLOR]));
            GuiStatusBar(statusBar, text, offsetX);
            DrawRectangleRec((Rectangle){statusBar.x + statusBar.width - 27 + WINDOWBOX_PADDING*2, statusBar.y + 1 + WINDOWBOX_PADDING, 25 - WINDOWBOX_PADDING*3, statusBar.height - 2 - WINDOWBOX_PADDING*2}, GetColor(style[DEFAULT_BASE_COLOR_NORMAL]));
            DrawText("x", statusBar.x + statusBar.width - 16, statusBar.y + statusBar.height/2 - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE], GetColor(style[DEFAULT_TEXT_COLOR_NORMAL]));
        } break;   
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[DEFAULT_BORDER_COLOR_NORMAL]));
            DrawRectangleRec((Rectangle){ bounds.x + 1, bounds.y + 1, bounds.width - 2, bounds.height -2 }, GetColor(style[DEFAULT_BACKGROUND_COLOR]));
            GuiStatusBar(statusBar, text, offsetX);
            DrawRectangleRec((Rectangle){statusBar.x + statusBar.width - 27 + WINDOWBOX_PADDING*2, statusBar.y + 1 + WINDOWBOX_PADDING, 25 - WINDOWBOX_PADDING*3, statusBar.height - 2 - WINDOWBOX_PADDING*2}, GetColor(style[DEFAULT_BASE_COLOR_FOCUSED]));           
            DrawText("x", statusBar.x + statusBar.width - 16, statusBar.y + statusBar.height/2 - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE], GetColor(style[DEFAULT_TEXT_COLOR_FOCUSED]));
        } break;   
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[DEFAULT_BORDER_COLOR_NORMAL]));
            DrawRectangleRec((Rectangle){ bounds.x + 1, bounds.y + 1, bounds.width - 2, bounds.height -2 }, GetColor(style[DEFAULT_BACKGROUND_COLOR]));
            GuiStatusBar(statusBar, text, offsetX); 
            DrawRectangleRec((Rectangle){statusBar.x + statusBar.width - 27 + WINDOWBOX_PADDING*2, statusBar.y + 1 + WINDOWBOX_PADDING, 25 - WINDOWBOX_PADDING*3, statusBar.height - 2 - WINDOWBOX_PADDING*2}, GetColor(style[DEFAULT_BASE_COLOR_PRESSED]));           
            DrawText("x", statusBar.x + statusBar.width - 16, statusBar.y + statusBar.height/2 - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE], GetColor(style[DEFAULT_TEXT_COLOR_PRESSED]));
        } break;   
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[DEFAULT_BORDER_COLOR_DISABLED]));
            DrawRectangleRec((Rectangle){ bounds.x + 1, bounds.y + 1, bounds.width - 2, bounds.height -2 }, GetColor(style[DEFAULT_BACKGROUND_COLOR]));
            GuiStatusBar(statusBar, text, offsetX);
            DrawRectangleRec((Rectangle){statusBar.x + statusBar.width - 27, statusBar.y + 1, 25, statusBar.height - 2}, GetColor(style[DEFAULT_BASE_COLOR_DISABLED]));   
            DrawText("x", statusBar.x + statusBar.width - 16, statusBar.y + statusBar.height/2 - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE], GetColor(style[DEFAULT_TEXT_COLOR_DISABLED]));
       } break;
        default: break;
    }
    
    //--------------------------------------------------------------------
    return clicked;
}

// Message Box control
RAYGUIDEF bool GuiMessageBox(Rectangle bounds, const char *windowTitle, const char *message)
{
    #define BUTTON_HEIGHT 26
    #define BUTTON_PADDING 10
    #define STATUSBAR_BUTTON 16
    #define STATUSBAR_HEIGHT 24
    GuiControlState state = guiState;
    bool clicked = false;

    Vector2 textSize = MeasureTextEx(GetDefaultFont(), windowTitle, style[DEFAULT_TEXT_SIZE], 1);
    int offsetX = 20;
    /*int textWidth = MeasureText(windowTitle, style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];
    */

    if (bounds.width < textSize.x + offsetX + STATUSBAR_BUTTON) bounds.width = textSize.x + offsetX + STATUSBAR_BUTTON;
    
    textSize = MeasureTextEx(GetDefaultFont(), message, style[DEFAULT_TEXT_SIZE], 1);
    if (bounds.width < textSize.x + offsetX) bounds.width = textSize.x + offsetX;

    if (bounds.height < (BUTTON_HEIGHT + BUTTON_PADDING*2 + STATUSBAR_HEIGHT + STATUSBAR_BUTTON + textSize.y)) bounds.height = (BUTTON_HEIGHT + BUTTON_PADDING*2 + STATUSBAR_HEIGHT + STATUSBAR_BUTTON + textSize.y);
    
    Rectangle buttonBounds = { bounds.x + BUTTON_PADDING, bounds.y + bounds.height - BUTTON_PADDING - BUTTON_HEIGHT, bounds.width - BUTTON_PADDING*2, BUTTON_HEIGHT };
    
    // Draw control
    //--------------------------------------------------------------------
    clicked = GuiWindowBox(bounds, windowTitle);
    GuiLabel((Rectangle){ bounds.x + bounds.width/2 - textSize.x/2, bounds.y + (STATUSBAR_HEIGHT - BUTTON_HEIGHT - BUTTON_PADDING)/2 + bounds.height/2 - textSize.y/2, 0, 0 }, message);
    
    clicked = GuiButton(buttonBounds, "OK");
    //--------------------------------------------------------------------
    
    return clicked;
}

RAYGUIDEF void GuiDummyRec(Rectangle bounds, const char *text)
{
    GuiControlState state = guiState;
    
    int textWidth = MeasureText(text, style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];
    
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
            else state = FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        case FOCUSED:
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[DEFAULT_BASE_COLOR_NORMAL]));
            //DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[DEFAULT_BORDER_COLOR_NORMAL]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));

        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[DEFAULT_BASE_COLOR_DISABLED]));
            //DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[DEFAULT_BORDER_COLOR_DISABLED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //------------------------------------------------------------------
}

// Button control, returns true when clicked
RAYGUIDEF bool GuiButton(Rectangle bounds, const char *text)
{
    GuiControlState state = guiState;
    bool clicked = false;
    
    int textWidth = MeasureText(text, style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];
    
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
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_FOCUSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_PRESSED]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_PRESSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_DISABLED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_DISABLED]));
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
    
    int textWidth = MeasureText(text, style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];

    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;

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
        case NORMAL: DrawText(text, bounds.x, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_NORMAL])); break;
        case FOCUSED: DrawText(text, bounds.x, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_FOCUSED])); break;
        case PRESSED: DrawText(text, bounds.x, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_PRESSED])); break;
        case DISABLED: DrawText(text, bounds.x, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_DISABLED])); break;
        default: break;
    }
    //--------------------------------------------------------------------

    return clicked;
}

// Image button control, returns true when clicked
// TODO: Just provide textureId instead of full Texture2D
RAYGUIDEF bool GuiImageButton(Rectangle bounds, Texture2D texture)
{
    return GuiImageButtonEx(bounds, texture, (Rectangle){ 0, 0, texture.width, texture.height }, NULL);
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
        textWidth = MeasureText(text, style[DEFAULT_TEXT_SIZE]);
        textHeight = style[DEFAULT_TEXT_SIZE];
        
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
            if (text != NULL) DrawText(text, (bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING + textWidth)/2) + texSource.width, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));
            if (texture.id > 0) DrawTextureRec(texture, texSource, (Vector2){ bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING/2 + textWidth)/2, bounds.y + bounds.height/2 - texSource.height/2 }, GetColor(style[BUTTON_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_FOCUSED]));
            if (text != NULL) DrawText(text, (bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING + textWidth)/2) + texSource.width, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_FOCUSED]));
            if (texture.id > 0) DrawTextureRec(texture, texSource, (Vector2){ bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING/2 + textWidth)/2, bounds.y + bounds.height/2 - texSource.height/2 }, GetColor(style[BUTTON_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_PRESSED]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_PRESSED]));
            if (text != NULL) DrawText(text, (bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING + textWidth)/2) + texSource.width, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_PRESSED]));
            if (texture.id > 0) DrawTextureRec(texture, texSource, (Vector2){ bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING/2 + textWidth)/2, bounds.y + bounds.height/2 - texSource.height/2 }, GetColor(style[BUTTON_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[BUTTON_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[BUTTON_BORDER_WIDTH], bounds.y + style[BUTTON_BORDER_WIDTH], bounds.width - 2*style[BUTTON_BORDER_WIDTH], bounds.height - 2*style[BUTTON_BORDER_WIDTH], GetColor(style[BUTTON_BASE_COLOR_DISABLED]));
            if (text != NULL) DrawText(text, (bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING + textWidth)/2) + texSource.width, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[BUTTON_TEXT_COLOR_DISABLED]));
            if (texture.id > 0) DrawTextureRec(texture, texSource, (Vector2){ bounds.x + bounds.width/2 - (texSource.width + IMAGEBUTTON_PADDING/2 + textWidth)/2, bounds.y + bounds.height/2 - texSource.height/2 }, GetColor(style[BUTTON_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //------------------------------------------------------------------

    return clicked;
}

// Toggle Button control, returns true when active
RAYGUIDEF bool GuiToggleButton(Rectangle bounds, const char *text, bool active)
{
    GuiControlState state = guiState;
    
    int textWidth = MeasureText(text, style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];
    
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
                DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_PRESSED]));
            }
            else
            {
                DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_NORMAL]));
                DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_NORMAL]));
                DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_NORMAL]));
            }
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_FOCUSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_PRESSED]));
            DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_PRESSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[TOGGLE_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[TOGGLE_BASE_COLOR_DISABLED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[TOGGLE_TEXT_COLOR_DISABLED]));
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
        if (i == active) GuiToggleButton((Rectangle){ bounds.x + i*(bounds.width/count + style[TOGGLEGROUP_PADDING]), bounds.y, (bounds.width - style[TOGGLEGROUP_PADDING]*(count -1))/count, bounds.height }, text[i], true);
        else if (GuiToggleButton((Rectangle){ bounds.x + i*(bounds.width/count + style[TOGGLEGROUP_PADDING]), bounds.y, (bounds.width - style[TOGGLEGROUP_PADDING]*(count -1))/count, bounds.height }, text[i], false) == true) active = i;
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
            DrawRectangleRec(bounds, GetColor(style[DEFAULT_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[TOGGLE_BORDER_WIDTH], bounds.y + style[TOGGLE_BORDER_WIDTH], bounds.width - 2*style[TOGGLE_BORDER_WIDTH], bounds.height - 2*style[TOGGLE_BORDER_WIDTH], GetColor(style[CHECKBOX_BASE_COLOR_DISABLED]));
            if (checked) DrawRectangle(bounds.x + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING],
                                       bounds.y + style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING], 
                                       bounds.width - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), 
                                       bounds.height - 2*(style[CHECKBOX_BORDER_WIDTH] + style[CHECKBOX_INNER_PADDING]), GetColor(style[DEFAULT_TEXT_COLOR_DISABLED]));
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

    int textWidth = MeasureText(text[active], style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];

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
            
            DrawText(text[active], bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_NORMAL]));   
            DrawText(FormatText("%i/%i", active + 1, count), 
                     selector.x + selector.width/2 - (MeasureText(FormatText("%i/%i", active + 1, count), 
                     style[DEFAULT_TEXT_SIZE])/2), selector.y + selector.height/2 - style[DEFAULT_TEXT_SIZE]/2, 
                     style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[COMBOBOX_BORDER_WIDTH], bounds.y + style[COMBOBOX_BORDER_WIDTH], bounds.width - 2*style[COMBOBOX_BORDER_WIDTH], bounds.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_FOCUSED]));

            DrawRectangleRec(selector, GetColor(style[COMBOBOX_BORDER_COLOR_FOCUSED]));
            DrawRectangle(selector.x + style[COMBOBOX_BORDER_WIDTH], selector.y + style[COMBOBOX_BORDER_WIDTH], selector.width - 2*style[COMBOBOX_BORDER_WIDTH], selector.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_FOCUSED]));
            
            DrawText(text[active], bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_FOCUSED]));   
            DrawText(FormatText("%i/%i", active + 1, count), 
                     selector.x + selector.width/2 - (MeasureText(FormatText("%i/%i", active + 1, count), 
                     style[DEFAULT_TEXT_SIZE])/2), selector.y + selector.height/2 - style[DEFAULT_TEXT_SIZE]/2, 
                     style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_PRESSED]));
            DrawRectangle(bounds.x + style[COMBOBOX_BORDER_WIDTH], bounds.y + style[COMBOBOX_BORDER_WIDTH], bounds.width - 2*style[COMBOBOX_BORDER_WIDTH], bounds.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_PRESSED]));

            DrawRectangleRec(selector, GetColor(style[COMBOBOX_BORDER_COLOR_PRESSED]));
            DrawRectangle(selector.x + style[COMBOBOX_BORDER_WIDTH], selector.y + style[COMBOBOX_BORDER_WIDTH], selector.width - 2*style[COMBOBOX_BORDER_WIDTH], selector.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_PRESSED]));
            
            DrawText(text[active], bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_PRESSED]));   
            DrawText(FormatText("%i/%i", active + 1, count), 
                     selector.x + selector.width/2 - (MeasureText(FormatText("%i/%i", active + 1, count), 
                     style[DEFAULT_TEXT_SIZE])/2), selector.y + selector.height/2 - style[DEFAULT_TEXT_SIZE]/2, 
                     style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[COMBOBOX_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[COMBOBOX_BORDER_WIDTH], bounds.y + style[COMBOBOX_BORDER_WIDTH], bounds.width - 2*style[COMBOBOX_BORDER_WIDTH], bounds.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_DISABLED]));

            DrawRectangleRec(selector, GetColor(style[COMBOBOX_BORDER_COLOR_DISABLED]));
            DrawRectangle(selector.x + style[COMBOBOX_BORDER_WIDTH], selector.y + style[COMBOBOX_BORDER_WIDTH], selector.width - 2*style[COMBOBOX_BORDER_WIDTH], selector.height - 2*style[COMBOBOX_BORDER_WIDTH], GetColor(style[COMBOBOX_BASE_COLOR_DISABLED]));
            
            DrawText(text[active], bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_DISABLED]));
            DrawText(FormatText("%i/%i", active + 1, count), 
                     selector.x + selector.width/2 - (MeasureText(FormatText("%i/%i", active + 1, count), 
                     style[DEFAULT_TEXT_SIZE])/2), selector.y + selector.height/2 - style[DEFAULT_TEXT_SIZE]/2, 
                     style[DEFAULT_TEXT_SIZE], GetColor(style[COMBOBOX_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------

    return active;
}

// Dropdown Box control, returns selected item
RAYGUIDEF int GuiDropdownBox(Rectangle bounds, const char **text, int count, int active)
{
    #define DROPDOWNBOX_PADDING                 1
    #define DROPDOWNBOX_ARROW_RIGHT_PADDING    16
    
    GuiControlState state = guiState;
    bool dropActive = false;
    static bool clicked = false;
   
    int textWidth = MeasureText(text[active], style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];

    if (bounds.width < textWidth) bounds.width = textWidth;
    if (bounds.height < textHeight) bounds.height = textHeight;
    
    int boundsHeight0 = bounds.height;
    
    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        Vector2 mousePoint = GetMousePosition();
        
        if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mousePoint, bounds))
        {
            clicked = true;
        }
        else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) clicked = false;
        
        if (clicked)
        {
            bounds.height *= (count + 1);
            dropActive = true;
        }

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = PRESSED;
            else state = FOCUSED;
        }
        else state = NORMAL;
        
        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
        {
            for (int i = 1; i < (count + 1); i++)
            {
                if (CheckCollisionPointRec(mousePoint, (Rectangle){ bounds.x, bounds.y + i*bounds.height, bounds.width, bounds.height })) active = i - 1;
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
            if (dropActive) GuiListElement((Rectangle){ bounds.x, bounds.y, bounds.width, bounds.height/(count + 1) }, text[active], true);
            else GuiListElement((Rectangle){ bounds.x, bounds.y, bounds.width, bounds.height }, text[active], true);
            
            DrawTriangle((Vector2){ bounds.x + bounds.width - DROPDOWNBOX_ARROW_RIGHT_PADDING, bounds.y + boundsHeight0/2 - 2 }, 
                         (Vector2){ bounds.x + bounds.width - DROPDOWNBOX_ARROW_RIGHT_PADDING + 5, bounds.y + boundsHeight0/2 - 2 + 5 }, 
                         (Vector2){ bounds.x + bounds.width - DROPDOWNBOX_ARROW_RIGHT_PADDING + 10, bounds.y + boundsHeight0/2 - 2 }, GetColor(style[DEFAULT_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            if (dropActive) GuiListElement((Rectangle){ bounds.x, bounds.y, bounds.width, bounds.height/(count + 1) }, text[active], true);
            else GuiListElement((Rectangle){ bounds.x, bounds.y, bounds.width, bounds.height }, text[active], true);
            
            DrawTriangle((Vector2){ bounds.x + bounds.width - DROPDOWNBOX_ARROW_RIGHT_PADDING, bounds.y + boundsHeight0/2 - 2 }, 
                         (Vector2){ bounds.x + bounds.width - DROPDOWNBOX_ARROW_RIGHT_PADDING + 5, bounds.y + boundsHeight0/2 - 2 + 5 }, 
                         (Vector2){ bounds.x + bounds.width - DROPDOWNBOX_ARROW_RIGHT_PADDING + 10, bounds.y + boundsHeight0/2 - 2 }, GetColor(style[DEFAULT_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleLinesEx(bounds, 1, GetColor(style[LISTVIEW_BORDER_COLOR_PRESSED]));
            if (dropActive) GuiListElement((Rectangle){ bounds.x, bounds.y, bounds.width, bounds.height/(count + 1) }, text[active], true);
            else GuiListElement((Rectangle){ bounds.x, bounds.y, bounds.width, bounds.height }, text[active], true);

            if (clicked)
            {
                for(int i = 0; i < count; i++)
                {
                    GuiListElement((Rectangle){ bounds.x, bounds.y + bounds.height/(count + 1)*(i+1) + DROPDOWNBOX_PADDING, bounds.width, bounds.height/(count + 1) - DROPDOWNBOX_PADDING }, text[i], false);
                }
            }
            
            
            DrawTriangle((Vector2){ bounds.x + bounds.width - DROPDOWNBOX_ARROW_RIGHT_PADDING, bounds.y + boundsHeight0/2 - 2 }, 
                         (Vector2){ bounds.x + bounds.width - DROPDOWNBOX_ARROW_RIGHT_PADDING + 5, bounds.y + boundsHeight0/2 - 2 + 5 }, 
                         (Vector2){ bounds.x + bounds.width - DROPDOWNBOX_ARROW_RIGHT_PADDING + 10, bounds.y + boundsHeight0/2 - 2 }, GetColor(style[DEFAULT_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            GuiListElement((Rectangle){ bounds.x, bounds.y, bounds.width, bounds.height }, text[active], false);
            
            DrawTriangle((Vector2){ bounds.x + bounds.width - DROPDOWNBOX_ARROW_RIGHT_PADDING, bounds.y + boundsHeight0/2 - 2 }, 
                         (Vector2){ bounds.x + bounds.width - DROPDOWNBOX_ARROW_RIGHT_PADDING + 5, bounds.y + boundsHeight0/2 - 2 + 5 }, 
                         (Vector2){ bounds.x + bounds.width - DROPDOWNBOX_ARROW_RIGHT_PADDING + 10, bounds.y + boundsHeight0/2 - 2 }, GetColor(style[DEFAULT_TEXT_COLOR_DISABLED]));
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
    #define GROUPBOX_PADDING       2
    
    GuiControlState state = guiState;
    
    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case NORMAL:
        {
            
            DrawRectangle(bounds.x, bounds.y, GROUPBOX_LINE_THICK, bounds.height, GuiLinesColor());
            DrawRectangle(bounds.x, bounds.y + bounds.height, bounds.width + GROUPBOX_LINE_THICK, GROUPBOX_LINE_THICK, GuiLinesColor());
            DrawRectangle(bounds.x + bounds.width, bounds.y, GROUPBOX_LINE_THICK, bounds.height, GuiLinesColor());
            if (text[0] == '\0') DrawRectangle(bounds.x, bounds.y, bounds.width, GROUPBOX_LINE_THICK, GuiLinesColor());
            else 
            {
                DrawRectangle(bounds.x, bounds.y, GROUPBOX_TEXT_PADDING, GROUPBOX_LINE_THICK, GuiLinesColor());
                DrawRectangle(bounds.x + 2*GROUPBOX_TEXT_PADDING + MeasureText(text, style[DEFAULT_TEXT_SIZE]), bounds.y, bounds.width - 2*GROUPBOX_TEXT_PADDING - MeasureText(text, style[DEFAULT_TEXT_SIZE]), GROUPBOX_LINE_THICK, GuiLinesColor());
                DrawText(text, bounds.x + GROUPBOX_TEXT_PADDING + 2*GROUPBOX_PADDING, bounds.y - 2*GROUPBOX_PADDING - GROUPBOX_LINE_THICK, style[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_NORMAL]));
            }
        } break;
        case FOCUSED: break; // NOTE: State not used on this control
        case PRESSED: break; // NOTE: State not used on this control
        case DISABLED:
        {
            DrawRectangle(bounds.x, bounds.y, GROUPBOX_LINE_THICK, bounds.height, GetColor(style[DEFAULT_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x, bounds.y + bounds.height, bounds.width + GROUPBOX_LINE_THICK, GROUPBOX_LINE_THICK, GetColor(style[DEFAULT_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + bounds.width, bounds.y, GROUPBOX_LINE_THICK, bounds.height, GetColor(style[DEFAULT_BORDER_COLOR_DISABLED]));
            if (text[0] == '\0') DrawRectangle(bounds.x, bounds.y, bounds.width, GROUPBOX_LINE_THICK, GuiLinesColor());
            else 
            {
                DrawRectangle(bounds.x, bounds.y, GROUPBOX_TEXT_PADDING, GROUPBOX_LINE_THICK, GetColor(style[DEFAULT_BORDER_COLOR_DISABLED]));
                DrawRectangle(bounds.x + 2*GROUPBOX_TEXT_PADDING + MeasureText(text, style[DEFAULT_TEXT_SIZE]), bounds.y, bounds.width - 2*GROUPBOX_TEXT_PADDING - MeasureText(text, style[DEFAULT_TEXT_SIZE]), GROUPBOX_LINE_THICK, GetColor(style[DEFAULT_BORDER_COLOR_DISABLED]));
                DrawText(text, bounds.x + GROUPBOX_TEXT_PADDING + 2*GROUPBOX_PADDING, bounds.y - 2*GROUPBOX_PADDING - GROUPBOX_LINE_THICK, style[DEFAULT_TEXT_SIZE], GetColor(style[LABEL_TEXT_COLOR_DISABLED]));
            }
        } break;
        default: break;
    }
    //--------------------------------------------------------------------
    
    
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
            DrawText(text, bounds.x + GUITEXTBOX_PADDING, bounds.y + style[TEXTBOX_BORDER_WIDTH] + bounds.height/2 - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE], GetColor(style[TEXTBOX_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(bounds, GetColor(style[TEXTBOX_BORDER_COLOR_FOCUSED]));
            DrawRectangle(bounds.x + style[TEXTBOX_BORDER_WIDTH], bounds.y + style[TEXTBOX_BORDER_WIDTH], bounds.width - 2*style[TEXTBOX_BORDER_WIDTH], bounds.height - 2*style[TEXTBOX_BORDER_WIDTH], GetColor(style[TEXTBOX_BASE_COLOR_FOCUSED]));
            DrawText(text, bounds.x + GUITEXTBOX_PADDING, bounds.y + style[TEXTBOX_BORDER_WIDTH] + bounds.height/2 - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE], GetColor(style[TEXTBOX_TEXT_COLOR_PRESSED]));
            
            if ((framesCounter/20)%2 == 0) DrawRectangle(bounds.x + GUITEXTBOX_LINE_PADDING + MeasureText(text, style[DEFAULT_TEXT_SIZE]), bounds.y + GUITEXTBOX_LINE_PADDING/2, 1, bounds.height - GUITEXTBOX_LINE_PADDING, GetColor(style[TEXTBOX_BORDER_COLOR_FOCUSED]));
        } break;
        case PRESSED: break; // NOTE: State not used on this control
        case DISABLED:
        {
            DrawRectangleRec(bounds, GetColor(style[TEXTBOX_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[TEXTBOX_BORDER_WIDTH], bounds.y + style[TEXTBOX_BORDER_WIDTH], bounds.width - 2*style[TEXTBOX_BORDER_WIDTH], bounds.height - 2*style[TEXTBOX_BORDER_WIDTH], GetColor(style[TEXTBOX_BASE_COLOR_DISABLED]));
            DrawText(text, bounds.x + GUITEXTBOX_PADDING, bounds.y + style[TEXTBOX_BORDER_WIDTH] + bounds.height/2 - style[DEFAULT_TEXT_SIZE]/2, style[DEFAULT_TEXT_SIZE], GetColor(style[TEXTBOX_TEXT_COLOR_DISABLED]));
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
            DrawRectangleRec(slider, GetColor(style[DEFAULT_TEXT_COLOR_DISABLED]));
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
            DrawRectangleRec(slider, GetColor(style[DEFAULT_TEXT_COLOR_DISABLED]));
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
            DrawRectangle(bounds.x + style[PROGRESSBAR_BORDER_WIDTH], bounds.y + style[PROGRESSBAR_BORDER_WIDTH], bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH], bounds.height - 2*style[PROGRESSBAR_BORDER_WIDTH], GetColor(style[DEFAULT_BACKGROUND_COLOR]));
            DrawRectangleRec(progress, GetColor(style[PROGRESSBAR_BASE_COLOR_NORMAL]));
        } break;
        case FOCUSED: break;    // NOTE: State not used on this control
        case PRESSED: break;    // NOTE: State not used on this control
        case DISABLED: 
        {
            DrawRectangleRec(bounds, GetColor(style[PROGRESSBAR_BORDER_COLOR_DISABLED]));
            DrawRectangle(bounds.x + style[PROGRESSBAR_BORDER_WIDTH], bounds.y + style[PROGRESSBAR_BORDER_WIDTH], bounds.width - 2*style[PROGRESSBAR_BORDER_WIDTH], bounds.height - 2*style[PROGRESSBAR_BORDER_WIDTH], GetColor(style[DEFAULT_BACKGROUND_COLOR]));
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

    int textWidth = MeasureText(FormatText("%i", value), style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];

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
            DrawText(FormatText("%i", value), spinner.x + (spinner.width/2 - textWidth/2), spinner.y + (spinner.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_TEXT_COLOR_NORMAL]));
        } break;
        case FOCUSED:
        {
            DrawRectangleRec(spinner, GetColor(style[SPINNER_BORDER_COLOR_FOCUSED]));
            DrawRectangle(spinner.x + style[SPINNER_BUTTON_PADDING], spinner.y + style[SPINNER_BUTTON_PADDING], spinner.width - style[SPINNER_BUTTON_PADDING]*2, spinner.height - style[SPINNER_BUTTON_PADDING]*2, GetColor(style[SPINNER_BASE_COLOR_FOCUSED]));
            DrawText(FormatText("%i", value), spinner.x + (spinner.width/2 - textWidth/2), spinner.y + (spinner.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangleRec(spinner, GetColor(style[SPINNER_BORDER_COLOR_PRESSED]));
            DrawRectangle(spinner.x + style[SPINNER_BUTTON_PADDING], spinner.y + style[SPINNER_BUTTON_PADDING], spinner.width - style[SPINNER_BUTTON_PADDING]*2, spinner.height - style[SPINNER_BUTTON_PADDING]*2, GetColor(style[SPINNER_BASE_COLOR_PRESSED]));
            DrawText(FormatText("%i", value), spinner.x + (spinner.width/2 - textWidth/2), spinner.y + (spinner.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawRectangleRec(spinner, GetColor(style[SPINNER_BORDER_COLOR_DISABLED]));
            DrawRectangle(spinner.x + style[SPINNER_BUTTON_PADDING], spinner.y + style[SPINNER_BUTTON_PADDING], spinner.width - style[SPINNER_BUTTON_PADDING]*2, spinner.height - style[SPINNER_BUTTON_PADDING]*2, GetColor(style[SPINNER_BASE_COLOR_DISABLED]));
            DrawText(FormatText("%i", value), spinner.x + (spinner.width/2 - textWidth/2), spinner.y + (spinner.height/2 - (style[DEFAULT_TEXT_SIZE]/2)), style[DEFAULT_TEXT_SIZE], GetColor(style[SPINNER_TEXT_COLOR_DISABLED]));
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
    
    int textWidth = MeasureText(text, style[DEFAULT_TEXT_SIZE]);
    int textHeight = style[DEFAULT_TEXT_SIZE];
    
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
                DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[LISTVIEW_TEXT_COLOR_PRESSED]));
            }
            else
            {
                DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[LISTVIEW_TEXT_COLOR_NORMAL]));
            }
        } break;
        case FOCUSED:
        {
            DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BASE_COLOR_FOCUSED]));
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BORDER_COLOR_FOCUSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[LISTVIEW_TEXT_COLOR_FOCUSED]));
        } break;
        case PRESSED:
        {
            DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BASE_COLOR_PRESSED]));
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[LISTVIEW_BORDER_COLOR_PRESSED]));
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[LISTVIEW_TEXT_COLOR_PRESSED]));
        } break;
        case DISABLED:
        {
            DrawText(text, bounds.x + bounds.width/2 - textWidth/2, bounds.y + bounds.height/2 - textHeight/2, style[DEFAULT_TEXT_SIZE], GetColor(style[LISTVIEW_TEXT_COLOR_DISABLED]));
        } break;
        default: break;
    }
    //--------------------------------------------------------------------
    
    return active;
}

// List View control, returns selected list element index  // int *startIndex)
RAYGUIDEF int GuiListView(Rectangle bounds, const char **text, int count, int active)
{
    #define LISTVIEW_LINE_THICK       1
    
    GuiControlState state = guiState;
    
    static int startIndex = 0;
    int endIndex = count;

    // Update control
    //--------------------------------------------------------------------
    if (state != DISABLED)
    {
        for(int i = 0; i < count; i++)
        {
            int textWidth = MeasureText(text[i], style[DEFAULT_TEXT_SIZE]);
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
    else 
    {
        DrawRectangleGradientEx(bounds, Fade(GetColor(style[COLORPICKER_BASE_COLOR_DISABLED]), 0.1f), Fade(BLACK, 0.6f), Fade(BLACK, 0.6f), Fade(GetColor(style[COLORPICKER_BORDER_COLOR_DISABLED]), 0.6f));
        //DrawRectangleGradientEx(bounds, Fade(BLACK, 0), BLACK, BLACK, Fade(BLACK, 0));
    }
 
    switch (state)
    {
        case NORMAL: DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_NORMAL])); break;
        case FOCUSED: DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_FOCUSED])); break;
        case PRESSED: DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_PRESSED])); break;
        case DISABLED: DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_DISABLED])); break;
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
    if (state != DISABLED)
    {
        for (int i = 0; i < bounds.width/CHECKER_SIZE; i++) DrawRectangle(bounds.x + CHECKER_SIZE*(i%(bounds.width/CHECKER_SIZE)), bounds.y, bounds.width/(bounds.width/CHECKER_SIZE), CHECKER_SIZE, (i%2) ? Fade(GRAY, 0.4f) : Fade(RAYWHITE, 0.4f));
        for (int i = 0; i < bounds.width/CHECKER_SIZE; i++) DrawRectangle(bounds.x + CHECKER_SIZE*(i%(bounds.width/CHECKER_SIZE)), bounds.y + CHECKER_SIZE, bounds.width/(bounds.width/CHECKER_SIZE), CHECKER_SIZE, (i%2) ? Fade(RAYWHITE, 0.4f) : Fade(GRAY, 0.4f));
        DrawRectangleGradientH(bounds.x, bounds.y, bounds.width, bounds.height, (Color){ 255,255,255,0 }, (Color){ 0,0,0,255 });
    }
    else DrawRectangleGradientH(bounds.x, bounds.y, bounds.width, bounds.height, Fade(GetColor(style[COLORPICKER_BASE_COLOR_DISABLED]), 0.1f), GetColor(style[COLORPICKER_BORDER_COLOR_DISABLED])); 
    
    
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
            DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(style[COLORPICKER_BORDER_COLOR_DISABLED]));
            DrawRectangleRec(selector, GetColor(style[COLORPICKER_BORDER_COLOR_DISABLED]));
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
        DrawRectangleGradientV(bounds.x, bounds.y, bounds.width, bounds.height, Fade(GetColor(style[COLORPICKER_BASE_COLOR_DISABLED]), 0.1f), GetColor(style[COLORPICKER_BORDER_COLOR_DISABLED]));
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
            DrawRectangle(selector.x, selector.y, selector.width, selector.height, GetColor(style[COLORPICKER_BORDER_COLOR_DISABLED]));
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
    GuiControlState state = guiState;
    
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
    if (state != DISABLED)
    {
        for (int i = 0; i < 2; i++) DrawRectangle(bounds.x + style[COLORPICKER_BARS_PADDING]*(i%(bounds.width/(style[COLORPICKER_BARS_THICK]/2))) + bounds.width + style[COLORPICKER_BARS_PADDING], bounds.y + bounds.height + style[COLORPICKER_BARS_PADDING], bounds.width/(bounds.width/(style[COLORPICKER_BARS_THICK]/2)), style[COLORPICKER_BARS_THICK]/2, (i%2) ? Fade(GRAY, 0.4f) : Fade(RAYWHITE, 0.4f));
        for (int i = 0; i < 2; i++) DrawRectangle(bounds.x + style[COLORPICKER_BARS_PADDING]*(i%(bounds.width/(style[COLORPICKER_BARS_THICK]/2))) + bounds.width + style[COLORPICKER_BARS_PADDING], bounds.y + style[COLORPICKER_BARS_PADDING] + bounds.height + style[COLORPICKER_BARS_PADDING], bounds.width/(bounds.width/(style[COLORPICKER_BARS_THICK]/2)), style[COLORPICKER_BARS_THICK]/2, (i%2) ? Fade(RAYWHITE, 0.4f) : Fade(GRAY, 0.4f));
        DrawRectangle(bounds.x + bounds.width + style[COLORPICKER_BARS_PADDING], bounds.y + bounds.height + style[COLORPICKER_BARS_PADDING], style[COLORPICKER_BARS_THICK], style[COLORPICKER_BARS_THICK], color);
        DrawRectangleLines(bounds.x + bounds.width + style[COLORPICKER_BARS_PADDING], bounds.y + bounds.height + style[COLORPICKER_BARS_PADDING], style[COLORPICKER_BARS_THICK], style[COLORPICKER_BARS_THICK], GetColor(style[COLORPICKER_BORDER_COLOR_NORMAL]));
    }
    else 
    {
        //DrawRectangleGradientEx((Rectangle){ bounds.x + bounds.width + style[COLORPICKER_BARS_PADDING], bounds.y + bounds.height + style[COLORPICKER_BARS_PADDING], style[COLORPICKER_BARS_THICK], style[COLORPICKER_BARS_THICK] }, WHITE, GRAY, BLACK, GRAY);
        DrawRectangle(bounds.x + bounds.width + style[COLORPICKER_BARS_PADDING], bounds.y + bounds.height + style[COLORPICKER_BARS_PADDING], style[COLORPICKER_BARS_THICK], style[COLORPICKER_BARS_THICK], GetColor(style[COLORPICKER_BASE_COLOR_DISABLED]));
        DrawRectangleLines(bounds.x + bounds.width + style[COLORPICKER_BARS_PADDING], bounds.y + bounds.height + style[COLORPICKER_BARS_PADDING], style[COLORPICKER_BARS_THICK], style[COLORPICKER_BARS_THICK], GetColor(style[COLORPICKER_BORDER_COLOR_DISABLED]));
    }
    //--------------------------------------------------------------------

    return color;
}

#if defined(RAYGUI_STYLE_SAVE_LOAD)
// Save raygui style file (.rgs), text or binary
RAYGUIDEF void GuiSaveStyle(const char *fileName, bool binary)
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
                id = (short)i;

                fwrite(&id, 1, 2, rgsFile);
                fwrite(&style[i], 1, sizeof(int), rgsFile);
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
            // Write some description comments
            fprintf(rgsFile, "#\n# rgst file (v%s) - raygui style text file saved from raygui\n#\n", RGS_FILE_VERSION_TEXT);
            fprintf(rgsFile, "# Total number of properties:     %i\n", NUM_PROPERTIES);

            for (int i = 0; i < NUM_PROPERTIES; i++)
            {
                fprintf(rgsFile, "%03i 0x%08x\n", i, style[i]);      
            }

            fclose(rgsFile);
        }
    }
}

// Load raygui style file (.rgs), text or binary
// NOTE: File is tried to be loaded as text first
RAYGUIDEF void GuiLoadStyle(const char *fileName)
{
    short id = 0;
    int value = 0;
    int counter = 0;
    char buffer[256];
    bool tryBinary = false;
    
    FILE *rgsFile = fopen(fileName, "rt");
    
    if (rgsFile != NULL)
    {
        fgets(buffer, 256, rgsFile);

        if (buffer[0] != 'R')   // Text file!
        {
            while (!feof(rgsFile))
            {
                if ((buffer[0] != '\n') && (buffer[0] != '#'))
                {
                    sscanf(buffer, "%d 0x%x", &id, &value);
                    if (id < NUM_PROPERTIES) style[id] = value;     // Update style property
                    counter++;
                }
                
                fgets(buffer, 256, rgsFile);
            }
        }
        else tryBinary = true;
        
        fclose(rgsFile);
    }
    
    if (tryBinary)
    {
        FILE *rgsFile = fopen(fileName, "rb");
    
        if (rgsFile != NULL)
        {
            char signature[5] = "";
            short version = 0;
            short numProperties = 0;
            short changedProperties = 0;
            short reserved = 0;
            int editionTime = 0;

            fread(signature, 1, 4, rgsFile);
            fread(&version, 1, sizeof(short), rgsFile);
            fread(&reserved, 1, sizeof(short), rgsFile);
            fread(&numProperties, 1, sizeof(short), rgsFile);
            fread(&changedProperties, 1, sizeof(short), rgsFile);

            if ((signature[0] == 'R') &&
                (signature[1] == 'G') &&
                (signature[2] == 'S') &&
                (signature[3] == ' '))
            {
                for (int i = 0; i < numProperties; i++)
                {
                    fread(&id, 1, sizeof(short), rgsFile);
                    fread(&value, 1, sizeof(int), rgsFile);
                    if (id < NUM_PROPERTIES) style[id] = value;
                    counter++;
                }
            }
            else TraceLog(LOG_WARNING, "[raygui] Invalid style properties file");
            
            fclose(rgsFile);
        }
    }

    TraceLog(LOG_INFO, "[raygui] Style properties loaded: %i", counter);
}

// Load GUI style from an image style file
RAYGUIDEF void GuiLoadStyleImage(const char *fileName)
{
    // NOTE: Image data only defines color properties
    Image imStyle = LoadImage(fileName);
    Color *pixels = GetImageData(imStyle);

    // Load generic style color palette
    style[DEFAULT_BACKGROUND_COLOR] = ColorToInt(pixels[1 + imStyle.width*1]);
    style[DEFAULT_LINES_COLOR] = ColorToInt(pixels[0 + imStyle.width*0]);
    style[DEFAULT_BORDER_COLOR_NORMAL] = ColorToInt(pixels[2 + imStyle.width*2]);
    style[DEFAULT_BASE_COLOR_NORMAL] = ColorToInt(pixels[3 + imStyle.width*3]);
    style[DEFAULT_TEXT_COLOR_NORMAL] = ColorToInt(pixels[9 + imStyle.width*4]);
    style[DEFAULT_BORDER_COLOR_FOCUSED] = ColorToInt(pixels[17 + imStyle.width*2]);
    style[DEFAULT_BASE_COLOR_FOCUSED] = ColorToInt(pixels[18 + imStyle.width*3]);
    style[DEFAULT_TEXT_COLOR_FOCUSED] = ColorToInt(pixels[24 + imStyle.width*4]);
    style[DEFAULT_BORDER_COLOR_PRESSED] = ColorToInt(pixels[32 + imStyle.width*2]);
    style[DEFAULT_BASE_COLOR_PRESSED] = ColorToInt(pixels[33 + imStyle.width*3]);
    style[DEFAULT_TEXT_COLOR_PRESSED] = ColorToInt(pixels[39 + imStyle.width*4]);
    style[DEFAULT_BORDER_COLOR_DISABLED] = ColorToInt(pixels[47 + imStyle.width*2]);
    style[DEFAULT_BASE_COLOR_DISABLED] = ColorToInt(pixels[48 + imStyle.width*3]);
    style[DEFAULT_TEXT_COLOR_DISABLED] = ColorToInt(pixels[54 + imStyle.width*4]);
    
    // Update full style with generic values
    GuiUpdateStyleComplete();

    UnloadImage(imStyle);
}

// Load style from a color palette array (14 values required)
RAYGUIDEF void GuiLoadStylePalette(Color *palette)
{
    // Load generic style color palette
    style[DEFAULT_BACKGROUND_COLOR] = ColorToInt(palette[0]);
    style[DEFAULT_LINES_COLOR] = ColorToInt(palette[1]);
    style[DEFAULT_BORDER_COLOR_NORMAL] = ColorToInt(palette[2]);
    style[DEFAULT_BASE_COLOR_NORMAL] = ColorToInt(palette[3]);
    style[DEFAULT_TEXT_COLOR_NORMAL] = ColorToInt(palette[4]);
    style[DEFAULT_BORDER_COLOR_FOCUSED] = ColorToInt(palette[5]);
    style[DEFAULT_BASE_COLOR_FOCUSED] = ColorToInt(palette[6]);
    style[DEFAULT_TEXT_COLOR_FOCUSED] = ColorToInt(palette[7]);
    style[DEFAULT_BORDER_COLOR_PRESSED] = ColorToInt(palette[8]);
    style[DEFAULT_BASE_COLOR_PRESSED] = ColorToInt(palette[9]);
    style[DEFAULT_TEXT_COLOR_PRESSED] = ColorToInt(palette[10]);
    style[DEFAULT_BORDER_COLOR_DISABLED] = ColorToInt(palette[11]);
    style[DEFAULT_BASE_COLOR_DISABLED] = ColorToInt(palette[12]);
    style[DEFAULT_TEXT_COLOR_DISABLED] = ColorToInt(palette[13]);
    
    // Update full style with generic values
    GuiUpdateStyleComplete();
}

// Updates full style property set with generic values
// NOTE: Requires globals style[] and style[]
RAYGUIDEF void GuiUpdateStyleComplete(void)
{
    // Label
    style[LABEL_TEXT_COLOR_NORMAL] = style[DEFAULT_TEXT_COLOR_NORMAL];
    style[LABEL_TEXT_COLOR_FOCUSED] = style[DEFAULT_TEXT_COLOR_FOCUSED];
    style[LABEL_TEXT_COLOR_PRESSED] = style[DEFAULT_TEXT_COLOR_PRESSED];
    style[LABEL_TEXT_COLOR_DISABLED] = style[DEFAULT_TEXT_COLOR_DISABLED];
    // Button
    style[BUTTON_BORDER_WIDTH] = style[DEFAULT_BORDER_WIDTH]*2;
    style[BUTTON_BORDER_COLOR_NORMAL] = style[DEFAULT_BORDER_COLOR_NORMAL];
    style[BUTTON_BASE_COLOR_NORMAL] = style[DEFAULT_BASE_COLOR_NORMAL];
    style[BUTTON_TEXT_COLOR_NORMAL] = style[DEFAULT_TEXT_COLOR_NORMAL];
    style[BUTTON_BORDER_COLOR_FOCUSED] = style[DEFAULT_BORDER_COLOR_FOCUSED];
    style[BUTTON_BASE_COLOR_FOCUSED] = style[DEFAULT_BASE_COLOR_FOCUSED];
    style[BUTTON_TEXT_COLOR_FOCUSED] = style[DEFAULT_TEXT_COLOR_FOCUSED];
    style[BUTTON_BORDER_COLOR_PRESSED] = style[DEFAULT_BORDER_COLOR_PRESSED];
    style[BUTTON_BASE_COLOR_PRESSED] = style[DEFAULT_BASE_COLOR_PRESSED];
    style[BUTTON_TEXT_COLOR_PRESSED] = style[DEFAULT_TEXT_COLOR_PRESSED];
    style[BUTTON_BORDER_COLOR_DISABLED] = style[DEFAULT_BORDER_COLOR_DISABLED];
    style[BUTTON_BASE_COLOR_DISABLED] = style[DEFAULT_BASE_COLOR_DISABLED];
    style[BUTTON_TEXT_COLOR_DISABLED] = style[DEFAULT_TEXT_COLOR_DISABLED];
    // ToggleButton / ToggleGroup
    style[TOGGLE_BORDER_WIDTH] = style[DEFAULT_BORDER_WIDTH];
    style[TOGGLE_BORDER_COLOR_NORMAL] = style[DEFAULT_BORDER_COLOR_NORMAL];
    style[TOGGLE_BASE_COLOR_NORMAL] = style[DEFAULT_BASE_COLOR_NORMAL];
    style[TOGGLE_TEXT_COLOR_NORMAL] = style[DEFAULT_TEXT_COLOR_NORMAL];
    style[TOGGLE_BORDER_COLOR_FOCUSED] = style[DEFAULT_BORDER_COLOR_FOCUSED];
    style[TOGGLE_BASE_COLOR_FOCUSED] = style[DEFAULT_BASE_COLOR_FOCUSED];
    style[TOGGLE_TEXT_COLOR_FOCUSED] = style[DEFAULT_TEXT_COLOR_FOCUSED];
    style[TOGGLE_BORDER_COLOR_PRESSED] = style[DEFAULT_BORDER_COLOR_PRESSED];
    style[TOGGLE_BASE_COLOR_PRESSED] = style[DEFAULT_BASE_COLOR_PRESSED];
    style[TOGGLE_TEXT_COLOR_PRESSED] = style[DEFAULT_TEXT_COLOR_PRESSED];
    style[TOGGLE_BORDER_COLOR_DISABLED] = style[DEFAULT_BORDER_COLOR_DISABLED];
    style[TOGGLE_BASE_COLOR_DISABLED] = style[DEFAULT_BASE_COLOR_DISABLED];
    style[TOGGLE_TEXT_COLOR_DISABLED] = style[DEFAULT_TEXT_COLOR_DISABLED];
    style[TOGGLEGROUP_PADDING] = 2;
    // Slider
    style[SLIDER_BORDER_WIDTH] = style[DEFAULT_BORDER_WIDTH];
    style[SLIDER_SLIDER_WIDTH] = 20;
    style[SLIDER_BORDER_COLOR_NORMAL] = style[DEFAULT_BORDER_COLOR_NORMAL];
    style[SLIDER_BASE_COLOR_NORMAL] = style[DEFAULT_BASE_COLOR_NORMAL];
    style[SLIDER_BORDER_COLOR_FOCUSED] = style[DEFAULT_BORDER_COLOR_FOCUSED];
    style[SLIDER_BASE_COLOR_FOCUSED] = style[DEFAULT_BASE_COLOR_FOCUSED];
    style[SLIDER_BORDER_COLOR_PRESSED] = style[DEFAULT_BORDER_COLOR_PRESSED];
    style[SLIDER_BASE_COLOR_PRESSED] = style[DEFAULT_BASE_COLOR_PRESSED];
    style[SLIDER_BORDER_COLOR_DISABLED] = style[DEFAULT_BORDER_COLOR_DISABLED];
    style[SLIDER_BASE_COLOR_DISABLED] = style[DEFAULT_BASE_COLOR_DISABLED];
    // SliderBar
    style[SLIDERBAR_INNER_PADDING] = 1;
    style[SLIDERBAR_BORDER_WIDTH] = style[DEFAULT_BORDER_WIDTH];
    style[SLIDERBAR_BORDER_COLOR_NORMAL] = style[DEFAULT_BORDER_COLOR_NORMAL];
    style[SLIDERBAR_BASE_COLOR_NORMAL] = style[DEFAULT_BASE_COLOR_NORMAL];
    style[SLIDERBAR_BORDER_COLOR_FOCUSED] = style[DEFAULT_BORDER_COLOR_FOCUSED];
    style[SLIDERBAR_BASE_COLOR_FOCUSED] = style[DEFAULT_BASE_COLOR_FOCUSED];
    style[SLIDERBAR_BORDER_COLOR_PRESSED] = style[DEFAULT_BORDER_COLOR_PRESSED];
    style[SLIDERBAR_BASE_COLOR_PRESSED] = style[DEFAULT_BASE_COLOR_PRESSED];
    style[SLIDERBAR_BORDER_COLOR_DISABLED] = style[DEFAULT_BORDER_COLOR_DISABLED];
    style[SLIDERBAR_BASE_COLOR_DISABLED] = style[DEFAULT_BASE_COLOR_DISABLED];
    // ProgressBar
    style[PROGRESSBAR_INNER_PADDING] = 1;
    style[PROGRESSBAR_BORDER_WIDTH] = style[DEFAULT_BORDER_WIDTH];
    style[PROGRESSBAR_BORDER_COLOR_NORMAL] = style[DEFAULT_BORDER_COLOR_NORMAL];
    style[PROGRESSBAR_BASE_COLOR_NORMAL] = style[DEFAULT_BASE_COLOR_PRESSED];
    style[PROGRESSBAR_BORDER_COLOR_FOCUSED] = style[DEFAULT_BORDER_COLOR_FOCUSED];
    style[PROGRESSBAR_BASE_COLOR_FOCUSED] = style[DEFAULT_BASE_COLOR_FOCUSED];
    style[PROGRESSBAR_BORDER_COLOR_PRESSED] = style[DEFAULT_BORDER_COLOR_PRESSED];
    style[PROGRESSBAR_BASE_COLOR_PRESSED] = style[DEFAULT_BASE_COLOR_PRESSED];
    style[PROGRESSBAR_BORDER_COLOR_DISABLED] = style[DEFAULT_BORDER_COLOR_DISABLED];
    style[PROGRESSBAR_BASE_COLOR_DISABLED] = style[DEFAULT_BASE_COLOR_DISABLED];
    // Spinner
    style[SPINNER_BUTTON_PADDING] = 2;
    style[SPINNER_BUTTONS_WIDTH] = 35;
    style[SPINNER_BORDER_COLOR_NORMAL] = style[DEFAULT_BORDER_COLOR_NORMAL];
    style[SPINNER_BASE_COLOR_NORMAL] = style[DEFAULT_BASE_COLOR_NORMAL];
    style[SPINNER_TEXT_COLOR_NORMAL] = style[DEFAULT_TEXT_COLOR_NORMAL];
    style[SPINNER_BORDER_COLOR_FOCUSED] = style[DEFAULT_BORDER_COLOR_FOCUSED];
    style[SPINNER_BASE_COLOR_FOCUSED] = style[DEFAULT_BASE_COLOR_FOCUSED];
    style[SPINNER_TEXT_COLOR_FOCUSED] = style[DEFAULT_TEXT_COLOR_FOCUSED];
    style[SPINNER_BORDER_COLOR_PRESSED] = style[DEFAULT_BORDER_COLOR_PRESSED];
    style[SPINNER_BASE_COLOR_PRESSED] = style[DEFAULT_BASE_COLOR_PRESSED];
    style[SPINNER_TEXT_COLOR_PRESSED] = style[DEFAULT_TEXT_COLOR_PRESSED];
    style[SPINNER_BORDER_COLOR_DISABLED] = style[DEFAULT_BORDER_COLOR_DISABLED];
    style[SPINNER_BASE_COLOR_DISABLED] = style[DEFAULT_BASE_COLOR_DISABLED];
    style[SPINNER_TEXT_COLOR_DISABLED] = style[DEFAULT_TEXT_COLOR_DISABLED];
    // ComboBox
    style[COMBOBOX_BORDER_WIDTH] = style[DEFAULT_BORDER_WIDTH];
    style[COMBOBOX_BUTTON_PADDING] = 2;              
    style[COMBOBOX_SELECTOR_WIDTH] = 35;              
    style[COMBOBOX_BORDER_COLOR_NORMAL] = style[DEFAULT_BORDER_COLOR_NORMAL];
    style[COMBOBOX_BASE_COLOR_NORMAL] = style[DEFAULT_BASE_COLOR_NORMAL];
    style[COMBOBOX_TEXT_COLOR_NORMAL] = style[DEFAULT_TEXT_COLOR_NORMAL];
    style[COMBOBOX_BORDER_COLOR_FOCUSED] = style[DEFAULT_BORDER_COLOR_FOCUSED];
    style[COMBOBOX_BASE_COLOR_FOCUSED] = style[DEFAULT_BASE_COLOR_FOCUSED];
    style[COMBOBOX_TEXT_COLOR_FOCUSED] = style[DEFAULT_TEXT_COLOR_FOCUSED];
    style[COMBOBOX_BORDER_COLOR_PRESSED] = style[DEFAULT_BORDER_COLOR_PRESSED];
    style[COMBOBOX_BASE_COLOR_PRESSED] = style[DEFAULT_BASE_COLOR_PRESSED];
    style[COMBOBOX_TEXT_COLOR_PRESSED] = style[DEFAULT_TEXT_COLOR_PRESSED];
    style[COMBOBOX_BORDER_COLOR_DISABLED] = style[DEFAULT_BORDER_COLOR_DISABLED];
    style[COMBOBOX_BASE_COLOR_DISABLED] = style[DEFAULT_BASE_COLOR_DISABLED];
    style[COMBOBOX_TEXT_COLOR_DISABLED] = style[DEFAULT_TEXT_COLOR_DISABLED];
    // CheckBox
    style[CHECKBOX_BORDER_WIDTH] = style[DEFAULT_BORDER_WIDTH];
    style[CHECKBOX_INNER_PADDING] = 1;
    style[CHECKBOX_BORDER_COLOR_NORMAL] = style[DEFAULT_BORDER_COLOR_NORMAL];
    style[CHECKBOX_BASE_COLOR_NORMAL] = style[DEFAULT_BACKGROUND_COLOR];
    style[CHECKBOX_BORDER_COLOR_FOCUSED] = style[DEFAULT_BORDER_COLOR_FOCUSED];
    style[CHECKBOX_BASE_COLOR_FOCUSED] = style[DEFAULT_TEXT_COLOR_FOCUSED];
    style[CHECKBOX_BORDER_COLOR_PRESSED] = style[DEFAULT_BORDER_COLOR_PRESSED];
    style[CHECKBOX_BASE_COLOR_PRESSED] = style[DEFAULT_TEXT_COLOR_PRESSED];
    style[CHECKBOX_BORDER_COLOR_DISABLED] = style[DEFAULT_BORDER_COLOR_DISABLED];
    style[CHECKBOX_BASE_COLOR_DISABLED] = style[DEFAULT_BASE_COLOR_DISABLED];
    // TextBox
    style[TEXTBOX_BORDER_WIDTH] = style[DEFAULT_BORDER_WIDTH];
    style[TEXTBOX_BORDER_COLOR_NORMAL] = style[DEFAULT_BORDER_COLOR_NORMAL];
    style[TEXTBOX_BASE_COLOR_NORMAL] = style[DEFAULT_BACKGROUND_COLOR];
    style[TEXTBOX_TEXT_COLOR_NORMAL] = style[DEFAULT_TEXT_COLOR_NORMAL];
    style[TEXTBOX_BORDER_COLOR_FOCUSED] = style[DEFAULT_BORDER_COLOR_FOCUSED];
    style[TEXTBOX_BASE_COLOR_FOCUSED] = style[DEFAULT_BASE_COLOR_FOCUSED];
    style[TEXTBOX_TEXT_COLOR_FOCUSED] = style[DEFAULT_TEXT_COLOR_FOCUSED];
    style[TEXTBOX_BORDER_COLOR_PRESSED] = style[DEFAULT_BORDER_COLOR_PRESSED];
    style[TEXTBOX_BASE_COLOR_PRESSED] = style[DEFAULT_BASE_COLOR_PRESSED];
    style[TEXTBOX_TEXT_COLOR_PRESSED] = style[DEFAULT_TEXT_COLOR_PRESSED];
    style[TEXTBOX_BORDER_COLOR_DISABLED] = style[DEFAULT_BORDER_COLOR_DISABLED];
    style[TEXTBOX_BASE_COLOR_DISABLED] = style[DEFAULT_BASE_COLOR_DISABLED];
    style[TEXTBOX_TEXT_COLOR_DISABLED] = style[DEFAULT_TEXT_COLOR_DISABLED];
    // ColorPicker
    style[COLORPICKER_BARS_THICK] = 20;
    style[COLORPICKER_BARS_PADDING] = 10;
    style[COLORPICKER_BORDER_COLOR_NORMAL] = style[DEFAULT_BORDER_COLOR_NORMAL];
    style[COLORPICKER_BASE_COLOR_NORMAL] = style[DEFAULT_BACKGROUND_COLOR];
    style[COLORPICKER_BORDER_COLOR_FOCUSED] = style[DEFAULT_BORDER_COLOR_FOCUSED];
    style[COLORPICKER_BASE_COLOR_FOCUSED] = style[DEFAULT_TEXT_COLOR_FOCUSED];
    style[COLORPICKER_BORDER_COLOR_PRESSED] = style[DEFAULT_BORDER_COLOR_PRESSED];
    style[COLORPICKER_BASE_COLOR_PRESSED] = style[DEFAULT_TEXT_COLOR_PRESSED];
    style[COLORPICKER_BORDER_COLOR_DISABLED] = style[DEFAULT_BORDER_COLOR_DISABLED];
    style[COLORPICKER_BASE_COLOR_DISABLED] = style[DEFAULT_BASE_COLOR_DISABLED];
    // ListView
    style[LISTVIEW_ELEMENTS_HEIGHT] = 30;
    style[LISTVIEW_ELEMENTS_PADDING] = 2;
    style[LISTVIEW_BAR_WIDTH] = 10;
    style[LISTVIEW_BORDER_COLOR_NORMAL] = style[DEFAULT_BORDER_COLOR_NORMAL];
    style[LISTVIEW_BASE_COLOR_NORMAL] = style[DEFAULT_BACKGROUND_COLOR];
    style[LISTVIEW_TEXT_COLOR_NORMAL] = style[DEFAULT_TEXT_COLOR_NORMAL];
    style[LISTVIEW_BORDER_COLOR_FOCUSED] = style[DEFAULT_BORDER_COLOR_FOCUSED];
    style[LISTVIEW_BASE_COLOR_FOCUSED] = style[DEFAULT_BASE_COLOR_FOCUSED];
    style[LISTVIEW_TEXT_COLOR_FOCUSED] = style[DEFAULT_TEXT_COLOR_FOCUSED];
    style[LISTVIEW_BORDER_COLOR_PRESSED] = style[DEFAULT_BORDER_COLOR_PRESSED];
    style[LISTVIEW_BASE_COLOR_PRESSED] = style[DEFAULT_BASE_COLOR_PRESSED];
    style[LISTVIEW_TEXT_COLOR_PRESSED] = style[DEFAULT_TEXT_COLOR_PRESSED];
    style[LISTVIEW_BORDER_COLOR_DISABLED] = style[DEFAULT_BORDER_COLOR_DISABLED];
    style[LISTVIEW_BASE_COLOR_DISABLED] = style[DEFAULT_BASE_COLOR_DISABLED];
    style[LISTVIEW_TEXT_COLOR_DISABLED] = style[DEFAULT_TEXT_COLOR_DISABLED];
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
static int ColorToInt(Color color)
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
