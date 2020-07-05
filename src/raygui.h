/*******************************************************************************************
*
*   raygui v2.8 - A simple and easy-to-use immediate-mode gui library
*
*   DESCRIPTION:
*
*   raygui is a tools-dev-focused immediate-mode-gui library based on raylib but also
*   available as a standalone library, as long as input and drawing functions are provided.
*
*   Controls provided:
*
*   # Container/separators Controls
*       - WindowBox
*       - GroupBox
*       - Line
*       - Panel
*
*   # Basic Controls
*       - Label
*       - Button
*       - LabelButton   --> Label
*       - ImageButton   --> Button
*       - ImageButtonEx --> Button
*       - Toggle
*       - ToggleGroup   --> Toggle
*       - CheckBox
*       - ComboBox
*       - DropdownBox
*       - TextBox
*       - TextBoxMulti
*       - ValueBox      --> TextBox
*       - Spinner       --> Button, ValueBox
*       - Slider
*       - SliderBar     --> Slider
*       - ProgressBar
*       - StatusBar
*       - ScrollBar
*       - ScrollPanel
*       - DummyRec
*       - Grid
*
*   # Advance Controls
*       - ListView
*       - ColorPicker   --> ColorPanel, ColorBarHue
*       - MessageBox    --> Window, Label, Button
*       - TextInputBox  --> Window, Label, TextBox, Button
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
*   #define RAYGUI_SUPPORT_ICONS
*       Includes riconsdata.h header defining a set of 128 icons (binary format) to be used on
*       multiple controls and following raygui styles
*
*
*   VERSIONS HISTORY:
*       2.8 (03-May-2020) Centralized rectangles drawing to GuiDrawRectangle()
*       2.7 (20-Feb-2020) Added possible tooltips API
*       2.6 (09-Sep-2019) ADDED: GuiTextInputBox()
*                         REDESIGNED: GuiListView*(), GuiDropdownBox(), GuiSlider*(), GuiProgressBar(), GuiMessageBox()
*                         REVIEWED: GuiTextBox(), GuiSpinner(), GuiValueBox(), GuiLoadStyle()
*                         Replaced property INNER_PADDING by TEXT_PADDING, renamed some properties
*                         Added 8 new custom styles ready to use
*                         Multiple minor tweaks and bugs corrected
*       2.5 (28-May-2019) Implemented extended GuiTextBox(), GuiValueBox(), GuiSpinner()
*       2.3 (29-Apr-2019) Added rIcons auxiliar library and support for it, multiple controls reviewed
*                         Refactor all controls drawing mechanism to use control state
*       2.2 (05-Feb-2019) Added GuiScrollBar(), GuiScrollPanel(), reviewed GuiListView(), removed Gui*Ex() controls
*       2.1 (26-Dec-2018) Redesign of GuiCheckBox(), GuiComboBox(), GuiDropdownBox(), GuiToggleGroup() > Use combined text string
*                         Complete redesign of style system (breaking change)
*       2.0 (08-Nov-2018) Support controls guiLock and custom fonts, reviewed GuiComboBox(), GuiListView()...
*       1.9 (09-Oct-2018) Controls review: GuiGrid(), GuiTextBox(), GuiTextBoxMulti(), GuiValueBox()...
*       1.8 (01-May-2018) Lot of rework and redesign to align with rGuiStyler and rGuiLayout
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
*       Vlad Adrian:        Complete rewrite of GuiTextBox() to support extended features (2019)
*       Sergio Martinez:    Review, testing (2015) and redesign of multiple controls (2018)
*       Adria Arranz:       Testing and Implementation of additional controls (2018)
*       Jordi Jorba:        Testing and Implementation of additional controls (2018)
*       Albert Martos:      Review and testing of the library (2015)
*       Ian Eito:           Review and testing of the library (2015)
*       Kevin Gato:         Initial implementation of basic components (2014)
*       Daniel Nicolas:     Initial implementation of basic components (2014)
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2020 Ramon Santamaria (@raysan5)
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

#define RAYGUI_VERSION  "2.6-dev"

#if !defined(RAYGUI_STANDALONE)
    #include "raylib.h"
#endif

// Define functions scope to be used internally (static) or externally (extern) to the module including this file
#if defined(RAYGUI_STATIC)
    #define RAYGUIDEF static            // Functions just visible to module including this file
#else
    #ifdef __cplusplus
        #define RAYGUIDEF extern "C"    // Functions visible from other files (no name mangling of functions in C++)
    #else
        // NOTE: By default any function declared in a C file is extern
        #define RAYGUIDEF extern        // Functions visible from other files
    #endif
#endif

#if defined(_WIN32)
    #if defined(BUILD_LIBTYPE_SHARED)
        #define RAYGUIDEF __declspec(dllexport)     // We are building raygui as a Win32 shared library (.dll).
    #elif defined(USE_LIBTYPE_SHARED)
        #define RAYGUIDEF __declspec(dllimport)     // We are using raygui as a Win32 shared library (.dll)
    #endif
#endif


#if !defined(RAYGUI_MALLOC) && !defined(RAYGUI_CALLOC) && !defined(RAYGUI_FREE)
    #include <stdlib.h>                 // Required for: malloc(), calloc(), free()
#endif

// Allow custom memory allocators
#ifndef RAYGUI_MALLOC
    #define RAYGUI_MALLOC(sz)       malloc(sz)
#endif
#ifndef RAYGUI_CALLOC
    #define RAYGUI_CALLOC(n,sz)     calloc(n,sz)
#endif
#ifndef RAYGUI_FREE
    #define RAYGUI_FREE(p)          free(p)
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define NUM_CONTROLS                    16      // Number of standard controls
#define NUM_PROPS_DEFAULT               16      // Number of standard properties
#define NUM_PROPS_EXTENDED               8      // Number of extended properties

#define TEXTEDIT_CURSOR_BLINK_FRAMES    20      // Text edit controls cursor blink timming

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
        float x;
        float y;
        float width;
        float height;
    } Rectangle;

    // TODO: Texture2D type is very coupled to raylib, mostly required by GuiImageButton()
    // It should be redesigned to be provided by user
    typedef struct Texture2D {
        unsigned int id;        // OpenGL texture id
        int width;              // Texture base width
        int height;             // Texture base height
        int mipmaps;            // Mipmap levels, 1 by default
        int format;             // Data format (PixelFormat type)
    } Texture2D;

    // Font character info
    typedef struct CharInfo CharInfo;

    // TODO: Font type is very coupled to raylib, mostly required by GuiLoadStyle()
    // It should be redesigned to be provided by user
    typedef struct Font {
        int baseSize;           // Base size (default chars height)
        int charsCount;         // Number of characters
        Texture2D texture;      // Characters texture atlas
        Rectangle *recs;        // Characters rectangles in texture
        CharInfo *chars;        // Characters info data
    } Font;
#endif

// Style property
typedef struct GuiStyleProp {
    unsigned short controlId;
    unsigned short propertyId;
    int propertyValue;
} GuiStyleProp;

typedef enum {
    GUI_SKIN_TYPE_SLICE1 = 0,    // 1 part
    GUI_SKIN_TYPE_SLICE9,        // 9 parts
    
    // TODO: are these needed ?!?
    // GUI_SKIN_TYPE_SLICE3H
    // GUI_SKIN_TYPE_SLICE3V
} GuiSkinType;

typedef enum {
    GUI_SKIN_DRAW_STRECHED = 0,
    GUI_SKIN_DRAW_TILED,    // WARN: don't use this mode too much (performance is bad especially with small tiles)
    GUI_SKIN_DRAW_CENTERED, // NOTE: Only available when patch type is GUI_SKIN_TYPE_SLICE1
    
    // TODO: currently only one draw mode can be selected for both the borders and the center (with 9-slices), maybe use different drawing modes
    // for example the borders could be tiled while the center will be stretched
} GuiSkinDraw;

typedef struct {
    unsigned short width, height;                   // (width,height) of the source rect in texture
    unsigned short nx, ny, fx, fy, px, py, dx, dy;  // (x,y) coordinates of the source rect inside the texture for all states
    unsigned short top, bottom, left, right;        // edges for the 9 slices (set only when type is GUI_SKIN_TYPE_SLICE9)
    unsigned short margins;                         // margins around the control (inflates the bounds, very usefull, used for fake shadows our when we want to draw outside the control bounds)
    unsigned char type;                             // style type (GUI_SKIN_TYPE_SLICE1/GUI_SKIN_TYPE_SLICE9)
    unsigned char mode;                             // style draw mode (GUI_SKIN_DRAW_STRECHED,GUI_SKIN_DRAW_TILED,GUI_SKIN_DRAW_CENTERED)
} GuiSkinStyle; // Main style struct used by all controls (32 bytes)

typedef GuiSkinStyle GuiSkinButton;
typedef GuiSkinStyle GuiSkinPanel;
typedef GuiSkinStyle GuiSkinStatusbar;
typedef GuiSkinStyle GuiSkinValueBox;
typedef GuiSkinStyle GuiSkinTextBox;        // TODO: maybe style the cursor too
typedef GuiSkinStyle GuiSkinTextBoxMulti;   // TODO: maybe style the cursor too
typedef GuiSkinStyle GuiSkinImageButton;
typedef GuiSkinStyle GuiSkinTooltip;

typedef struct {
	GuiSkinPanel panel;
	GuiSkinStatusbar titlebar;
	GuiSkinButton button;
} GuiSkinWindowBox;

typedef struct {
	GuiSkinStyle off;
	GuiSkinStyle on;
} GuiSkinToggle;

typedef struct {
	GuiSkinToggle item;
	GuiSkinToggle first;
	GuiSkinToggle last;
} GuiSkinToggleGroup;

typedef struct {
	GuiSkinStyle body;
	GuiSkinStyle checked;
} GuiSkinCheckBox;

typedef struct {
	GuiSkinStyle body;
	GuiSkinStyle button;
} GuiSkinComboBox;

typedef struct {
	GuiSkinStyle body;
    GuiSkinPanel listpanel;
	GuiSkinStyle item;
} GuiSkinDropBox;

typedef struct {
	GuiSkinValueBox valuebox;
	GuiSkinButton left, right;
} GuiSkinSpinner;

typedef struct {
	GuiSkinStyle body;
	GuiSkinStyle slider;    // NOTE: only active when control is a slider
    GuiSkinStyle bar;       // NOTE: only active when control is a sliderbar
} GuiSkinSlider; // used by slider and sliderbar

typedef struct {
    GuiSkinStyle body;
    GuiSkinStyle progress;
} GuiSkinProgressbar;

typedef struct {
	GuiSkinStyle body;
	GuiSkinStyle selector;
} GuiSkinColorPanel;

typedef struct {
    GuiSkinStyle body;
    GuiSkinStyle rail;      // NOTE: style for the active area which holds the slider
    GuiSkinStyle slider;    // NOTE: style for the slider bar
} GuiSkinScrollbar;

typedef struct {
    GuiSkinStyle body;
    GuiSkinStyle item;
    GuiSkinStyle first;
    GuiSkinStyle last;
    GuiSkinScrollbar scrollbar;
} GuiSkinList;

typedef struct {
	GuiSkinStyle body;
	GuiSkinStyle corner;
	GuiSkinScrollbar hscroll, vscroll;
} GuiSkinScrollPanel;

typedef struct { 
    Texture tex;
    bool useColor;
    
    GuiSkinButton button;
    GuiSkinImageButton imagebutton;
    GuiSkinPanel panel;
    GuiSkinStatusbar status;
    GuiSkinWindowBox window;
    GuiSkinScrollPanel scrollpanel;
    GuiSkinList list;
    GuiSkinScrollbar scrollbar;
    GuiSkinToggle toggle;
    GuiSkinToggleGroup togglegroup;
    GuiSkinCheckBox checkbox;
    GuiSkinComboBox combobox;
    GuiSkinDropBox dropbox;
    GuiSkinSpinner spinner;
    GuiSkinSlider slider;
    GuiSkinProgressbar progressbar;
    GuiSkinValueBox valuebox;
    GuiSkinTextBox textbox;
    GuiSkinTextBoxMulti textboxmulti;
    GuiSkinColorPanel colorpanel;
    GuiSkinColorPanel colorbaralpha;
    GuiSkinColorPanel colorbarhue;
    GuiSkinTooltip tooltip;
} GuiSkin;

typedef enum {
    GUI_SKIN_BUTTON_SID = 0,
    GUI_SKIN_IMGBUTTON_SID,
    GUI_SKIN_PANEL_SID,
    GUI_SKIN_STATUS_SID,
    GUI_SKIN_WINDOW_SID,
    GUI_SKIN_SCROLLPANEL_SID,
    GUI_SKIN_LIST_SID,
    GUI_SKIN_SCROLLBAR_SID,
    GUI_SKIN_TOGGLE_SID,
    GUI_SKIN_TOGGLEGROUP_SID,
    GUI_SKIN_CHECKBOX_SID,
    GUI_SKIN_COMBOBOX_SID,
    GUI_SKIN_DROPBOX_SID,
    GUI_SKIN_SPINNER_SID,
    GUI_SKIN_SLIDER_SID,
    GUI_SKIN_PROGRESSBAR_SID,
    GUI_SKIN_VALUEBOX_SID,
    GUI_SKIN_TEXTBOX_SID,
    GUI_SKIN_TEXTBOXMULTI_SID,
    GUI_SKIN_COLORPANEL_SID,
    GUI_SKIN_COLORBARALPHA_SID,
    GUI_SKIN_COLORBARHUE_SID,
    GUI_SKIN_TOOLTIP_SID,
} GuiSkinStyleId;


// Gui control state
typedef enum {
    GUI_STATE_NORMAL = 0,
    GUI_STATE_FOCUSED,
    GUI_STATE_PRESSED,
    GUI_STATE_DISABLED,
} GuiControlState;

// Gui control text alignment
typedef enum {
    GUI_TEXT_ALIGN_LEFT = 0,
    GUI_TEXT_ALIGN_CENTER,
    GUI_TEXT_ALIGN_RIGHT,
} GuiTextAlignment;

// Gui controls
typedef enum {
    DEFAULT = 0,
    LABEL,          // LABELBUTTON
    BUTTON,         // IMAGEBUTTON
    TOGGLE,         // TOGGLEGROUP
    SLIDER,         // SLIDERBAR
    PROGRESSBAR,
    CHECKBOX,
    COMBOBOX,
    DROPDOWNBOX,
    TEXTBOX,        // TEXTBOXMULTI
    VALUEBOX,
    SPINNER,
    LISTVIEW,
    COLORPICKER,
    SCROLLBAR,
    STATUSBAR
} GuiControl;

// Gui base properties for every control
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
    TEXT_COLOR_DISABLED,
    BORDER_WIDTH,
    TEXT_PADDING,
    TEXT_ALIGNMENT,
    RESERVED
} GuiControlProperty;

// Gui extended properties depend on control
// NOTE: We reserve a fixed size of additional properties per control

// DEFAULT properties
typedef enum {
    TEXT_SIZE = 16,
    TEXT_SPACING,
    LINE_COLOR,
    BACKGROUND_COLOR,
} GuiDefaultProperty;

// Label
//typedef enum { } GuiLabelProperty;

// Button
//typedef enum { } GuiButtonProperty;

// Toggle / ToggleGroup
typedef enum {
    GROUP_PADDING = 16,
} GuiToggleProperty;

// Slider / SliderBar
typedef enum {
    SLIDER_WIDTH = 16,
    SLIDER_PADDING
} GuiSliderProperty;

// ProgressBar
typedef enum {
    PROGRESS_PADDING = 16,
} GuiProgressBarProperty;

// CheckBox
typedef enum {
    CHECK_PADDING = 16
} GuiCheckBoxProperty;

// ComboBox
typedef enum {
    COMBO_BUTTON_WIDTH = 16,
    COMBO_BUTTON_PADDING
} GuiComboBoxProperty;

// DropdownBox
typedef enum {
    ARROW_PADDING = 16,
    DROPDOWN_ITEMS_PADDING
} GuiDropdownBoxProperty;

// TextBox / TextBoxMulti / ValueBox / Spinner
typedef enum {
    TEXT_INNER_PADDING = 16,
    TEXT_LINES_PADDING,
    COLOR_SELECTED_FG,
    COLOR_SELECTED_BG
} GuiTextBoxProperty;

// Spinner
typedef enum {
    SPIN_BUTTON_WIDTH = 16,
    SPIN_BUTTON_PADDING,
} GuiSpinnerProperty;

// ScrollBar
typedef enum {
    ARROWS_SIZE = 16,
    ARROWS_VISIBLE,
    SCROLL_SLIDER_PADDING,
    SCROLL_SLIDER_SIZE,
    SCROLL_PADDING,
    SCROLL_SPEED,
} GuiScrollBarProperty;

// ScrollBar side
typedef enum {
    SCROLLBAR_LEFT_SIDE = 0,
    SCROLLBAR_RIGHT_SIDE
} GuiScrollBarSide;

// ListView
typedef enum {
    LIST_ITEMS_HEIGHT = 16,
    LIST_ITEMS_PADDING,
    SCROLLBAR_WIDTH,
    SCROLLBAR_SIDE,
} GuiListViewProperty;

// ColorPicker
typedef enum {
    COLOR_SELECTOR_SIZE = 16,
    HUEBAR_WIDTH,                  // Right hue bar width
    HUEBAR_PADDING,                // Right hue bar separation from panel
    HUEBAR_SELECTOR_HEIGHT,        // Right hue bar selector height
    HUEBAR_SELECTOR_OVERFLOW       // Right hue bar selector overflow
} GuiColorPickerProperty;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

// State modification functions
RAYGUIDEF void GuiEnable(void);                                         // Enable gui controls (global state)
RAYGUIDEF void GuiDisable(void);                                        // Disable gui controls (global state)
RAYGUIDEF void GuiLock(void);                                           // Lock gui controls (global state)
RAYGUIDEF void GuiUnlock(void);                                         // Unlock gui controls (global state)
RAYGUIDEF void GuiFade(float alpha);                                    // Set gui controls alpha (global state), alpha goes from 0.0f to 1.0f
RAYGUIDEF void GuiSetState(int state);                                  // Set gui state (global state)
RAYGUIDEF int GuiGetState(void);                                        // Get gui state (global state)

// Font set/get functions
RAYGUIDEF void GuiSetFont(Font font);                                   // Set gui custom font (global state)
RAYGUIDEF Font GuiGetFont(void);                                        // Get gui custom font (global state)

// Style set/get functions
RAYGUIDEF void GuiSetStyle(int control, int property, int value);       // Set one style property
RAYGUIDEF int GuiGetStyle(int control, int property);                   // Get one style property

// Skin set/get functions
RAYGUIDEF void GuiSetSkin(GuiSkin* skin);       // Sets the global skin
RAYGUIDEF GuiSkin* GuiGetSkin(void);            // Gets the global skin

// Tooltips set functions
RAYGUIDEF void GuiEnableTooltip(void);                                  // Enable gui tooltips
RAYGUIDEF void GuiDisableTooltip(void);                                 // Disable gui tooltips
RAYGUIDEF void GuiSetTooltip(const char *tooltip);                      // Set current tooltip for display
RAYGUIDEF void GuiClearTooltip(void);                                   // Clear any tooltip registered

// Container/separator controls, useful for controls organization
RAYGUIDEF bool GuiWindowBox(Rectangle bounds, const char *title);                                       // Window Box control, shows a window that can be closed
RAYGUIDEF void GuiGroupBox(Rectangle bounds, const char *text);                                         // Group Box control with text name
RAYGUIDEF void GuiLine(Rectangle bounds, const char *text);                                             // Line separator control, could contain text
RAYGUIDEF void GuiPanel(Rectangle bounds);                                                              // Panel control, useful to group controls
RAYGUIDEF Rectangle GuiScrollPanel(Rectangle bounds, Rectangle content, Vector2 *scroll);               // Scroll Panel control

// Basic controls set
RAYGUIDEF void GuiLabel(Rectangle bounds, const char *text);                                            // Label control, shows text
RAYGUIDEF bool GuiButton(Rectangle bounds, const char *text);                                           // Button control, returns true when clicked
RAYGUIDEF bool GuiLabelButton(Rectangle bounds, const char *text);                                      // Label button control, show true when clicked
RAYGUIDEF bool GuiImageButton(Rectangle bounds, const char *text, Texture2D texture);                   // Image button control, returns true when clicked
RAYGUIDEF bool GuiImageButtonEx(Rectangle bounds, const char *text, Texture2D texture, Rectangle texSource);    // Image button extended control, returns true when clicked
RAYGUIDEF bool GuiToggle(Rectangle bounds, const char *text, bool active);                              // Toggle Button control, returns true when active
RAYGUIDEF int GuiToggleGroup(Rectangle bounds, const char *text, int active);                           // Toggle Group control, returns active toggle index
RAYGUIDEF bool GuiCheckBox(Rectangle bounds, const char *text, bool checked);                           // Check Box control, returns true when active
RAYGUIDEF int GuiComboBox(Rectangle bounds, const char *text, int active);                              // Combo Box control, returns selected item index
RAYGUIDEF bool GuiDropdownBox(Rectangle bounds, const char *text, int *active, bool editMode);          // Dropdown Box control, returns selected item
RAYGUIDEF bool GuiSpinner(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode);     // Spinner control, returns selected value
RAYGUIDEF bool GuiValueBox(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode);    // Value Box control, updates input text with numbers
RAYGUIDEF bool GuiTextBox(Rectangle bounds, char *text, int textSize, bool editMode);                   // Text Box control, updates input text
RAYGUIDEF bool GuiTextBoxMulti(Rectangle bounds, char *text, int textSize, bool editMode);              // Text Box control with multiple lines
RAYGUIDEF float GuiSlider(Rectangle bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue);       // Slider control, returns selected value
RAYGUIDEF float GuiSliderBar(Rectangle bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue);    // Slider Bar control, returns selected value
RAYGUIDEF float GuiProgressBar(Rectangle bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue);  // Progress Bar control, shows current progress value
RAYGUIDEF void GuiStatusBar(Rectangle bounds, const char *text);                                        // Status Bar control, shows info text
RAYGUIDEF void GuiDummyRec(Rectangle bounds, const char *text);                                         // Dummy control for placeholders
RAYGUIDEF int GuiScrollBar(Rectangle bounds, int value, int minValue, int maxValue);                    // Scroll Bar control
RAYGUIDEF Vector2 GuiGrid(Rectangle bounds, float spacing, int subdivs);                                // Grid control

// Advance controls set
RAYGUIDEF int GuiListView(Rectangle bounds, const char *text, int *scrollIndex, int active);            // List View control, returns selected list item index
RAYGUIDEF int GuiListViewEx(Rectangle bounds, const char **text, int count, int *focus, int *scrollIndex, int active);      // List View with extended parameters
RAYGUIDEF int GuiMessageBox(Rectangle bounds, const char *title, const char *message, const char *buttons);                 // Message Box control, displays a message
RAYGUIDEF int GuiTextInputBox(Rectangle bounds, const char *title, const char *message, const char *buttons, char *text);   // Text Input Box control, ask for text
RAYGUIDEF Color GuiColorPicker(Rectangle bounds, Color color);                                          // Color Picker control (multiple color controls)
RAYGUIDEF Color GuiColorPanel(Rectangle bounds, Color color);                                           // Color Panel control
RAYGUIDEF float GuiColorBarAlpha(Rectangle bounds, float alpha);                                        // Color Bar Alpha control
RAYGUIDEF float GuiColorBarHue(Rectangle bounds, float value);                                          // Color Bar Hue control

// Styles loading functions
RAYGUIDEF void GuiLoadStyle(const char *fileName);              // Load style file (.rgs)
RAYGUIDEF void GuiLoadStyleDefault(void);                       // Load style default over global style

// Skin loading/saving functions

// Saves a skin style to file. If file exists it will be overwritten. Returns `0` on failure.
RAYGUIDEF int GuiSaveSkin(const char *fileName, const char* texPath, GuiSkin* skin);
// Save multiple skin styles to file. If file exists it will be overwritten. Returns `0` on failure.
RAYGUIDEF int GuiSaveSkinEx(const char *fileName, const char* texPath, GuiSkin** skins, int count);
// Load skin style with specified `id` form a skin file. If skin texture is not set it will be loaded with `LoadTexture()`. Returns `0` on failure.
RAYGUIDEF int GuiLoadSkin(const char *fileName, GuiSkin* skin, int id);


/*
typedef GuiStyle (unsigned int *)
RAYGUIDEF GuiStyle LoadGuiStyle(const char *fileName);          // Load style from file (.rgs)
RAYGUIDEF void UnloadGuiStyle(GuiStyle style);                  // Unload style
*/

RAYGUIDEF const char *GuiIconText(int iconId, const char *text); // Get text with icon id prepended (if supported)

#if defined(RAYGUI_SUPPORT_ICONS)
// Gui icons functionality
RAYGUIDEF void GuiDrawIcon(int iconId, Vector2 position, int pixelSize, Color color);

RAYGUIDEF unsigned int *GuiGetIcons(void);                      // Get full icons data pointer
RAYGUIDEF unsigned int *GuiGetIconData(int iconId);             // Get icon bit data
RAYGUIDEF void GuiSetIconData(int iconId, unsigned int *data);  // Set icon bit data

RAYGUIDEF void GuiSetIconPixel(int iconId, int x, int y);       // Set icon pixel value
RAYGUIDEF void GuiClearIconPixel(int iconId, int x, int y);     // Clear icon pixel value
RAYGUIDEF bool GuiCheckIconPixel(int iconId, int x, int y);     // Check icon pixel value
#endif

#endif // RAYGUI_H


/***********************************************************************************
*
*   RAYGUI IMPLEMENTATION
*
************************************************************************************/

#if defined(RAYGUI_IMPLEMENTATION)

#if defined(RAYGUI_SUPPORT_ICONS)
    #define RICONS_IMPLEMENTATION
    #include "ricons.h"         // Required for: raygui icons data
#endif

#include <stdio.h>              // Required for: FILE, fopen(), fclose(), fprintf(), feof(), fscanf(), vsprintf()
#include <string.h>             // Required for: strlen() on GuiTextBox()
#include <math.h>               // Required for: roundf() on GuiColorPicker()

#if defined(RAYGUI_STANDALONE)
    #include <stdarg.h>         // Required for: va_list, va_start(), vfprintf(), va_end()
#endif

#ifdef __cplusplus
    #define RAYGUI_CLITERAL(name) name
#else
    #define RAYGUI_CLITERAL(name) (name)
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// Gui control property style color element
typedef enum { BORDER = 0, BASE, TEXT, OTHER } GuiPropertyElement;



//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static GuiControlState guiState = GUI_STATE_NORMAL;
static GuiSkin* guiSkin = NULL;

static Font guiFont = { 0 };            // Gui current font (WARNING: highly coupled to raylib)
static bool guiLocked = false;          // Gui lock state (no inputs processed)
static float guiAlpha = 1.0f;           // Gui element transpacency on drawing

// Global gui style array (allocated on data segment by default)
// NOTE: In raygui we manage a single int array with all the possible style properties.
// When a new style is loaded, it loads over the global style... but default gui style
// could always be recovered with GuiLoadStyleDefault()
static unsigned int guiStyle[NUM_CONTROLS*(NUM_PROPS_DEFAULT + NUM_PROPS_EXTENDED)] = { 0 };
static bool guiStyleLoaded = false;     // Style loaded flag for lazy style initialization

// Tooltips required variables
static const char *guiTooltip = NULL;   // Gui tooltip currently active (user provided)
static bool guiTooltipEnabled = true;   // Gui tooltips enabled

//----------------------------------------------------------------------------------
// Standalone Mode Functions Declaration
//
// NOTE: raygui depend on some raylib input and drawing functions
// To use raygui as standalone library, below functions must be defined by the user
//----------------------------------------------------------------------------------
#if defined(RAYGUI_STANDALONE)

#define KEY_RIGHT           262
#define KEY_LEFT            263
#define KEY_DOWN            264
#define KEY_UP              265
#define KEY_BACKSPACE       259
#define KEY_ENTER           257

#define MOUSE_LEFT_BUTTON     0

// Input required functions
//-------------------------------------------------------------------------------
static Vector2 GetMousePosition(void);
static int GetMouseWheelMove(void);
static bool IsMouseButtonDown(int button);
static bool IsMouseButtonPressed(int button);
static bool IsMouseButtonReleased(int button);

static bool IsKeyDown(int key);
static bool IsKeyPressed(int key);
static int GetKeyPressed(void);         // -- GuiTextBox(), GuiTextBoxMulti(), GuiValueBox()
//-------------------------------------------------------------------------------

// Drawing required functions
//-------------------------------------------------------------------------------
static void DrawRectangle(int x, int y, int width, int height, Color color);        // -- GuiDrawRectangle(), GuiDrawIcon()

static void DrawRectangleGradientEx(Rectangle rec, Color col1, Color col2, Color col3, Color col4);     // -- GuiColorPicker()
static void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color);                              // -- GuiDropdownBox(), GuiScrollBar()
static void DrawTextureRec(Texture2D texture, Rectangle sourceRec, Vector2 position, Color tint);       // -- GuiImageButtonEx()

static void DrawTextRec(Font font, const char *text, Rectangle rec, float fontSize, float spacing, bool wordWrap, Color tint); // -- GuiTextBoxMulti()
//-------------------------------------------------------------------------------

// Text required functions
//-------------------------------------------------------------------------------
static Font GetFontDefault(void);                           // -- GuiLoadStyleDefault()
static Vector2 MeasureTextEx(Font font, const char *text, float fontSize, float spacing);                          // -- GetTextWidth(), GuiTextBoxMulti()
static void DrawTextEx(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint);  // -- GuiDrawText()

static Font LoadFontEx(const char *fileName, int fontSize, int *fontChars, int charsCount);  // -- GuiLoadStyle()
static char *LoadText(const char *fileName);                // -- GuiLoadStyle()
static const char *GetDirectoryPath(const char *filePath);  // -- GuiLoadStyle()
//-------------------------------------------------------------------------------

// raylib functions already implemented in raygui
//-------------------------------------------------------------------------------
static Color GetColor(int hexValue);                // Returns a Color struct from hexadecimal value
static int ColorToInt(Color color);                 // Returns hexadecimal value for a Color
static Color Fade(Color color, float alpha);        // Color fade-in or fade-out, alpha goes from 0.0f to 1.0f
static bool CheckCollisionPointRec(Vector2 point, Rectangle rec);   // Check if point is inside rectangle
static const char *TextFormat(const char *text, ...);               // Formatting of text with variables to 'embed'
static const char **TextSplit(const char *text, char delimiter, int *count);    // Split text into multiple strings
static int TextToInteger(const char *text);         // Get integer value from text

static void DrawRectangleGradientV(int posX, int posY, int width, int height, Color color1, Color color2);  // Draw rectangle vertical gradient
//-------------------------------------------------------------------------------

#endif      // RAYGUI_STANDALONE

//----------------------------------------------------------------------------------
// Module specific Functions Declaration
//----------------------------------------------------------------------------------
static int GetTextWidth(const char *text);                      // Gui get text width using default font
static Rectangle GetTextBounds(int control, Rectangle bounds);  // Get text bounds considering control bounds
static const char *GetTextIcon(const char *text, int *iconId);  // Get text icon if provided and move text cursor

static void GuiDrawText(const char *text, Rectangle bounds, int alignment, Color tint);         // Gui draw text using default font
static void GuiDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color color, GuiSkinStyle* prop, int state);   // Gui draw rectangle using default raygui style
static void GuiDrawTooltip(Rectangle bounds);                   // Draw tooltip relatively to bounds

static const char **GuiTextSplit(const char *text, int *count, int *textRow);   // Split controls text into multiple strings
static Vector3 ConvertHSVtoRGB(Vector3 hsv);                    // Convert color data from HSV to RGB
static Vector3 ConvertRGBtoHSV(Vector3 rgb);                    // Convert color data from RGB to HSV

//----------------------------------------------------------------------------------
// Gui Setup Functions Definition
//----------------------------------------------------------------------------------
// Enable gui global state
void GuiEnable(void) { guiState = GUI_STATE_NORMAL; }

// Disable gui global state
void GuiDisable(void) { guiState = GUI_STATE_DISABLED; }

// Lock gui global state
void GuiLock(void) { guiLocked = true; }

// Unlock gui global state
void GuiUnlock(void) { guiLocked = false; }

// Set gui controls alpha global state
void GuiFade(float alpha)
{
    if (alpha < 0.0f) alpha = 0.0f;
    else if (alpha > 1.0f) alpha = 1.0f;

    guiAlpha = alpha;
}

// Set gui state (global state)
void GuiSetState(int state) { guiState = (GuiControlState)state; }

// Get gui state (global state)
int GuiGetState(void) { return guiState; }

// Set custom gui font
// NOTE: Font loading/unloading is external to raygui
void GuiSetFont(Font font)
{
    if (font.texture.id > 0)
    {
        // NOTE: If we try to setup a font but default style has not been
        // lazily loaded before, it will be overwritten, so we need to force
        // default style loading first
        if (!guiStyleLoaded) GuiLoadStyleDefault();

        guiFont = font;
        GuiSetStyle(DEFAULT, TEXT_SIZE, font.baseSize);
    }
}

// Get custom gui font
Font GuiGetFont(void)
{
    return guiFont;
}

// Set control style property value
void GuiSetStyle(int control, int property, int value)
{
    if (!guiStyleLoaded) GuiLoadStyleDefault();
    guiStyle[control*(NUM_PROPS_DEFAULT + NUM_PROPS_EXTENDED) + property] = value;

    // Default properties are propagated to all controls
    if ((control == 0) && (property < NUM_PROPS_DEFAULT))
    {
        for (int i = 1; i < NUM_CONTROLS; i++) guiStyle[i*(NUM_PROPS_DEFAULT + NUM_PROPS_EXTENDED) + property] = value;
    }
}

// Get control style property value
int GuiGetStyle(int control, int property)
{
    if (!guiStyleLoaded) GuiLoadStyleDefault();
    return guiStyle[control*(NUM_PROPS_DEFAULT + NUM_PROPS_EXTENDED) + property];
}

// Sets the global skin
void GuiSetSkin(GuiSkin* skin)
{
    guiSkin = skin;
}

// Gets the global skin
GuiSkin* GuiGetSkin(void)
{
    return guiSkin;
}

// Enable gui tooltips
void GuiEnableTooltip(void) { guiTooltipEnabled = true; }

// Disable gui tooltips
void GuiDisableTooltip(void) { guiTooltipEnabled = false; }

// Set current tooltip for display
void GuiSetTooltip(const char *tooltip) { guiTooltip = tooltip; }

// Clear any tooltip registered
void GuiClearTooltip(void) { guiTooltip = NULL; }


//----------------------------------------------------------------------------------
// Gui Controls Functions Definition
//----------------------------------------------------------------------------------

// Window Box control
bool GuiWindowBox(Rectangle bounds, const char *title)
{
    // NOTE: This define is also used by GuiMessageBox() and GuiTextInputBox()
    #define WINDOW_STATUSBAR_HEIGHT        22

    //GuiControlState state = guiState;
    bool clicked = false;

    int statusBarHeight = WINDOW_STATUSBAR_HEIGHT + 2*GuiGetStyle(STATUSBAR, BORDER_WIDTH);
    statusBarHeight += (statusBarHeight%2);

    Rectangle statusBar = { bounds.x, bounds.y, bounds.width, statusBarHeight };
    if (bounds.height < statusBarHeight*2) bounds.height = statusBarHeight*2;
    
    Rectangle windowPanel = { bounds.x, bounds.y + statusBarHeight - 1, bounds.width, bounds.height - statusBarHeight };
    Rectangle closeButtonRec = { statusBar.x + statusBar.width - GuiGetStyle(STATUSBAR, BORDER_WIDTH) - 20,
                                 statusBar.y + statusBarHeight/2 - 18/2, 18, 18 };

    // Update control
    //--------------------------------------------------------------------
    // NOTE: Logic is directly managed by button
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiSkinStyle style = {0}; // temporary style
    if(guiSkin != NULL) {
        style = guiSkin->status; // Save status style
        guiSkin->status = guiSkin->window.titlebar; // Override with the window status style
    }
    GuiStatusBar(statusBar, title); // Draw window header as status bar
    if(guiSkin != NULL) {
        guiSkin->status = style; // Restore status style
        style = guiSkin->panel;
        guiSkin->panel = guiSkin->window.panel;
    }
    GuiPanel(windowPanel);          // Draw window base
    if(guiSkin != NULL) {
        guiSkin->panel = style;  // Restore panel style
        style = guiSkin->button;
        guiSkin->button = guiSkin->window.button; // Override with the window button style
    }
    
    // Draw window close button
    int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
    int tempTextAlignment = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
#if defined(RAYGUI_SUPPORT_ICONS)
    clicked = GuiButton(closeButtonRec, GuiIconText(RICON_CROSS_SMALL, NULL));
#else
    clicked = GuiButton(closeButtonRec, "x");
#endif
    GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlignment);
    if(guiSkin != NULL) guiSkin->button = style; // Restore the button style
    //--------------------------------------------------------------------

    return clicked;
}

// Group Box control with text name
void GuiGroupBox(Rectangle bounds, const char *text)
{
    #define GROUPBOX_LINE_THICK     1
    #define GROUPBOX_TEXT_PADDING  10

    GuiControlState state = guiState;

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(RAYGUI_CLITERAL(Rectangle){ bounds.x, bounds.y, GROUPBOX_LINE_THICK, bounds.height }, 0, BLANK, Fade(GetColor(GuiGetStyle(DEFAULT, (state == GUI_STATE_DISABLED)? BORDER_COLOR_DISABLED : LINE_COLOR)), guiAlpha), NULL, state);
    GuiDrawRectangle(RAYGUI_CLITERAL(Rectangle){ bounds.x, bounds.y + bounds.height - 1, bounds.width, GROUPBOX_LINE_THICK }, 0, BLANK, Fade(GetColor(GuiGetStyle(DEFAULT, (state == GUI_STATE_DISABLED)? BORDER_COLOR_DISABLED : LINE_COLOR)), guiAlpha), NULL, state);
    GuiDrawRectangle(RAYGUI_CLITERAL(Rectangle){ bounds.x + bounds.width - 1, bounds.y, GROUPBOX_LINE_THICK, bounds.height }, 0, BLANK, Fade(GetColor(GuiGetStyle(DEFAULT, (state == GUI_STATE_DISABLED)? BORDER_COLOR_DISABLED : LINE_COLOR)), guiAlpha), NULL, state);

    GuiLine(RAYGUI_CLITERAL(Rectangle){ bounds.x, bounds.y, bounds.width, 1 }, text);
    //--------------------------------------------------------------------
}

// Line control
void GuiLine(Rectangle bounds, const char *text)
{
    #define LINE_TEXT_PADDING  10

    GuiControlState state = guiState;

    Color color = Fade(GetColor(GuiGetStyle(DEFAULT, (state == GUI_STATE_DISABLED)? BORDER_COLOR_DISABLED : LINE_COLOR)), guiAlpha);

    // Draw control
    //--------------------------------------------------------------------
    if (text == NULL) GuiDrawRectangle(RAYGUI_CLITERAL(Rectangle){ bounds.x, bounds.y + bounds.height/2, bounds.width, 1 }, 0, BLANK, color, NULL, state);
    else
    {
        Rectangle textBounds = { 0 };
        textBounds.width = GetTextWidth(text);      // TODO: Consider text icon
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + LINE_TEXT_PADDING;
        textBounds.y = bounds.y - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        // Draw line with embedded text label: "--- text --------------"
        GuiDrawRectangle(RAYGUI_CLITERAL(Rectangle){ bounds.x, bounds.y, LINE_TEXT_PADDING - 2, 1 }, 0, BLANK, color, NULL, state);
        GuiLabel(textBounds, text);
        GuiDrawRectangle(RAYGUI_CLITERAL(Rectangle){ bounds.x + LINE_TEXT_PADDING + textBounds.width + 4, bounds.y, bounds.width - textBounds.width - LINE_TEXT_PADDING - 4, 1 }, 0, BLANK, color, NULL, state);
    }
    //--------------------------------------------------------------------
}

// Panel control
void GuiPanel(Rectangle bounds)
{
    #define PANEL_BORDER_WIDTH   1

    GuiControlState state = guiState;

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, PANEL_BORDER_WIDTH, Fade(GetColor(GuiGetStyle(DEFAULT, (state == GUI_STATE_DISABLED)? BORDER_COLOR_DISABLED: LINE_COLOR)), guiAlpha),
                     Fade(GetColor(GuiGetStyle(DEFAULT, (state == GUI_STATE_DISABLED)? BASE_COLOR_DISABLED : BACKGROUND_COLOR)), guiAlpha), guiSkin != NULL ? &guiSkin->panel : NULL, state);
    //--------------------------------------------------------------------
}

// Scroll Panel control
Rectangle GuiScrollPanel(Rectangle bounds, Rectangle content, Vector2 *scroll)
{
    GuiControlState state = guiState;

    Vector2 scrollPos = { 0.0f, 0.0f };
    if (scroll != NULL) scrollPos = *scroll;

    bool hasHorizontalScrollBar = (content.width > bounds.width - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH))? true : false;
    bool hasVerticalScrollBar = (content.height > bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH))? true : false;

    // Recheck to account for the other scrollbar being visible
    if (!hasHorizontalScrollBar) hasHorizontalScrollBar = (hasVerticalScrollBar && (content.width > (bounds.width - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH))))? true : false;
    if (!hasVerticalScrollBar) hasVerticalScrollBar = (hasHorizontalScrollBar && (content.height > (bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH))))? true : false;

    const int horizontalScrollBarWidth = hasHorizontalScrollBar? GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH) : 0;
    const int verticalScrollBarWidth =  hasVerticalScrollBar? GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH) : 0;
    const Rectangle horizontalScrollBar = { (float)((GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)? (float)bounds.x + verticalScrollBarWidth : (float)bounds.x) + GuiGetStyle(DEFAULT, BORDER_WIDTH), (float)bounds.y + bounds.height - horizontalScrollBarWidth - GuiGetStyle(DEFAULT, BORDER_WIDTH), (float)bounds.width - verticalScrollBarWidth - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH), (float)horizontalScrollBarWidth };
    const Rectangle verticalScrollBar = { (float)((GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)? (float)bounds.x + GuiGetStyle(DEFAULT, BORDER_WIDTH) : (float)bounds.x + bounds.width - verticalScrollBarWidth - GuiGetStyle(DEFAULT, BORDER_WIDTH)), (float)bounds.y + GuiGetStyle(DEFAULT, BORDER_WIDTH), (float)verticalScrollBarWidth, (float)bounds.height - horizontalScrollBarWidth - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) };

    // Calculate view area (area without the scrollbars)
    Rectangle view = (GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)?
                RAYGUI_CLITERAL(Rectangle){ bounds.x + verticalScrollBarWidth + GuiGetStyle(DEFAULT, BORDER_WIDTH), bounds.y + GuiGetStyle(DEFAULT, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - verticalScrollBarWidth, bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - horizontalScrollBarWidth } :
                RAYGUI_CLITERAL(Rectangle){ bounds.x + GuiGetStyle(DEFAULT, BORDER_WIDTH), bounds.y + GuiGetStyle(DEFAULT, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - verticalScrollBarWidth, bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - horizontalScrollBarWidth };

    // Clip view area to the actual content size
    if (view.width > content.width) view.width = content.width;
    if (view.height > content.height) view.height = content.height;

    // TODO: Review!
    const int horizontalMin = hasHorizontalScrollBar? ((GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)? -verticalScrollBarWidth : 0) - GuiGetStyle(DEFAULT, BORDER_WIDTH) : ((GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)? -verticalScrollBarWidth : 0) - GuiGetStyle(DEFAULT, BORDER_WIDTH);
    const int horizontalMax = hasHorizontalScrollBar? content.width - bounds.width + verticalScrollBarWidth + GuiGetStyle(DEFAULT, BORDER_WIDTH) - ((GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)? verticalScrollBarWidth : 0) : -GuiGetStyle(DEFAULT, BORDER_WIDTH);
    const int verticalMin = hasVerticalScrollBar? -GuiGetStyle(DEFAULT, BORDER_WIDTH) : -GuiGetStyle(DEFAULT, BORDER_WIDTH);
    const int verticalMax = hasVerticalScrollBar? content.height - bounds.height + horizontalScrollBarWidth + GuiGetStyle(DEFAULT, BORDER_WIDTH) : -GuiGetStyle(DEFAULT, BORDER_WIDTH);

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check button state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;

            if (hasHorizontalScrollBar)
            {
                if (IsKeyDown(KEY_RIGHT)) scrollPos.x -= GuiGetStyle(SCROLLBAR, SCROLL_SPEED);
                if (IsKeyDown(KEY_LEFT)) scrollPos.x += GuiGetStyle(SCROLLBAR, SCROLL_SPEED);
            }

            if (hasVerticalScrollBar)
            {
                if (IsKeyDown(KEY_DOWN)) scrollPos.y -= GuiGetStyle(SCROLLBAR, SCROLL_SPEED);
                if (IsKeyDown(KEY_UP)) scrollPos.y += GuiGetStyle(SCROLLBAR, SCROLL_SPEED);
            }

            scrollPos.y += GetMouseWheelMove()*20;
        }
    }

    // Normalize scroll values
    if (scrollPos.x > -horizontalMin) scrollPos.x = -horizontalMin;
    if (scrollPos.x < -horizontalMax) scrollPos.x = -horizontalMax;
    if (scrollPos.y > -verticalMin) scrollPos.y = -verticalMin;
    if (scrollPos.y < -verticalMax) scrollPos.y = -verticalMax;
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, GuiGetStyle(DEFAULT, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, (float)BORDER + (state*3))), guiAlpha), GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), guiSkin != NULL ? &guiSkin->scrollpanel.body : NULL, state);        // Draw background

    // Save size of the scrollbar slider
    const int slider = GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE);
    
    GuiSkinScrollbar style = {0}; // Temporary scrollbar style
    // Draw horizontal scrollbar if visible
    if (hasHorizontalScrollBar)
    {
        if(guiSkin != NULL) { style =  guiSkin->scrollbar; guiSkin->scrollbar = guiSkin->scrollpanel.hscroll; } // Save the scrollbar style and overwrite it with the windowbox scrollbar style
        // Change scrollbar slider size to show the diff in size between the content width and the widget width
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, ((bounds.width - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - verticalScrollBarWidth)/content.width)*(bounds.width - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - verticalScrollBarWidth));
        scrollPos.x = -GuiScrollBar(horizontalScrollBar, -scrollPos.x, horizontalMin, horizontalMax);
    }

    // Draw vertical scrollbar if visible
    if (hasVerticalScrollBar)
    {
        if(guiSkin != NULL) guiSkin->scrollbar = guiSkin->scrollpanel.vscroll;  // Overwrite with windowbox scrollbar style
        // Change scrollbar slider size to show the diff in size between the content height and the widget height
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, ((bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - horizontalScrollBarWidth)/content.height)* (bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) - horizontalScrollBarWidth));
        scrollPos.y = -GuiScrollBar(verticalScrollBar, -scrollPos.y, verticalMin, verticalMax);
    }

    // Draw detail corner rectangle if both scroll bars are visible
    if (hasHorizontalScrollBar && hasVerticalScrollBar)
    {
        // TODO: Consider scroll bars side
        Rectangle corner = { horizontalScrollBar.x + horizontalScrollBar.width + 2, verticalScrollBar.y + verticalScrollBar.height + 2, horizontalScrollBarWidth - 4, verticalScrollBarWidth - 4 };
        GuiDrawRectangle(corner, 0, BLANK, Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT + (state*3))), guiAlpha), guiSkin != NULL ? &guiSkin->scrollpanel.corner : NULL, state);
    }
    if(guiSkin != NULL) guiSkin->scrollbar = style; // Restore scrollbar style
    
    // Set scrollbar slider size back to the way it was before
    GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, slider);
    //--------------------------------------------------------------------

    if (scroll != NULL) *scroll = scrollPos;

    return view;
}

// Label control
void GuiLabel(Rectangle bounds, const char *text)
{
    GuiControlState state = guiState;

    // Update control
    //--------------------------------------------------------------------
    // ...
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawText(text, GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LABEL, (state == GUI_STATE_DISABLED)? TEXT_COLOR_DISABLED : TEXT_COLOR_NORMAL)), guiAlpha));
    //--------------------------------------------------------------------
}

// Button control, returns true when clicked
bool GuiButton(Rectangle bounds, const char *text)
{
    GuiControlState state = guiState;
    bool pressed = false;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check button state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) pressed = true;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiSkinStyle* style = guiSkin != NULL ? &guiSkin->button : NULL;
    GuiDrawRectangle(bounds, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha), style, state);
    GuiDrawText(text, GetTextBounds(BUTTON, bounds), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
    //------------------------------------------------------------------

    return pressed;
}

// Label button control
bool GuiLabelButton(Rectangle bounds, const char *text)
{
    GuiControlState state = guiState;
    bool pressed = false;

    // NOTE: We force bounds.width to be all text
    int textWidth = MeasureTextEx(guiFont, text, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING)).x;
    if (bounds.width < textWidth) bounds.width = textWidth;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check checkbox state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) pressed = true;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawText(text, GetTextBounds(LABEL, bounds), GuiGetStyle(LABEL, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    return pressed;
}

// Image button control, returns true when clicked
bool GuiImageButton(Rectangle bounds, const char *text, Texture2D texture)
{
    return GuiImageButtonEx(bounds, text, texture, RAYGUI_CLITERAL(Rectangle){ 0, 0, (float)texture.width, (float)texture.height });
}

// Image button control, returns true when clicked
bool GuiImageButtonEx(Rectangle bounds, const char *text, Texture2D texture, Rectangle texSource)
{
    GuiControlState state = guiState;
    bool clicked = false;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check button state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) clicked = true;
            else state = GUI_STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, GuiGetStyle(BUTTON, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(BUTTON, BASE + (state*3))), guiAlpha), NULL, state);

    if (text != NULL) GuiDrawText(text, GetTextBounds(BUTTON, bounds), GuiGetStyle(BUTTON, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
    if (texture.id > 0) DrawTextureRec(texture, texSource, RAYGUI_CLITERAL(Vector2){ bounds.x + bounds.width/2 - texSource.width/2, bounds.y + bounds.height/2 - texSource.height/2 }, Fade(GetColor(GuiGetStyle(BUTTON, TEXT + (state*3))), guiAlpha));
    //------------------------------------------------------------------

    return clicked;
}

// Toggle Button control, returns true when active
bool GuiToggle(Rectangle bounds, const char *text, bool active)
{
    GuiControlState state = guiState;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check toggle button state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
            {
                state = GUI_STATE_NORMAL;
                active = !active;
            }
            else state = GUI_STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (state == GUI_STATE_NORMAL)
    {
        GuiDrawRectangle(bounds, GuiGetStyle(TOGGLE, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TOGGLE, (active? BORDER_COLOR_PRESSED : (BORDER + state*3)))), guiAlpha), Fade(GetColor(GuiGetStyle(TOGGLE, (active? BASE_COLOR_PRESSED : (BASE + state*3)))), guiAlpha), guiSkin != NULL ? (!active ? &guiSkin->toggle.off : &guiSkin->toggle.on ) : NULL, GUI_STATE_NORMAL);
        GuiDrawText(text, GetTextBounds(TOGGLE, bounds), GuiGetStyle(TOGGLE, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(TOGGLE, (active? TEXT_COLOR_PRESSED : (TEXT + state*3)))), guiAlpha));
    }
    else
    {
        GuiDrawRectangle(bounds, GuiGetStyle(TOGGLE, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TOGGLE, BORDER + state*3)), guiAlpha), Fade(GetColor(GuiGetStyle(TOGGLE, BASE + state*3)), guiAlpha), guiSkin != NULL ? &guiSkin->toggle.off : NULL, state);
        GuiDrawText(text, GetTextBounds(TOGGLE, bounds), GuiGetStyle(TOGGLE, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(TOGGLE, TEXT + state*3)), guiAlpha));
    }
    //--------------------------------------------------------------------

    return active;
}

// Toggle Group control, returns toggled button index
int GuiToggleGroup(Rectangle bounds, const char *text, int active)
{
    #if !defined(TOGGLEGROUP_MAX_ELEMENTS)
        #define TOGGLEGROUP_MAX_ELEMENTS    32
    #endif

    float initBoundsX = bounds.x;

    // Get substrings items from text (items pointers)
    int rows[TOGGLEGROUP_MAX_ELEMENTS] = { 0 };
    int itemsCount = 0;
    const char **items = GuiTextSplit(text, &itemsCount, rows);

    int prevRow = rows[0];
    
    GuiSkinToggle style = {0};
    if(guiSkin != NULL) style = guiSkin->toggle;  // Save toggle style
    
    for (int i = 0; i < itemsCount; i++)
    {
        if (prevRow != rows[i])
        {
            bounds.x = initBoundsX;
            bounds.y += (bounds.height + GuiGetStyle(TOGGLE, GROUP_PADDING));
            prevRow = rows[i];
        }
        
        if(guiSkin != NULL) {  // Set correct style for the toggled item
            if(i == 0) guiSkin->toggle = guiSkin->togglegroup.first;
            else if(i + 1 == itemsCount) guiSkin->toggle = guiSkin->togglegroup.last;
            else guiSkin->toggle = guiSkin->togglegroup.item;
        }
        
        if (i == active) GuiToggle(bounds, items[i], true);
        else if (GuiToggle(bounds, items[i], false) == true) active = i;

        bounds.x += (bounds.width + GuiGetStyle(TOGGLE, GROUP_PADDING));
    }
    
    if(guiSkin != NULL) guiSkin->toggle = style; // Restore toggle style

    return active;
}

// Check Box control, returns true when active
bool GuiCheckBox(Rectangle bounds, const char *text, bool checked)
{
    GuiControlState state = guiState;

    Rectangle textBounds = { 0 };

    if (text != NULL)
    {
        textBounds.width = GetTextWidth(text);
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(CHECKBOX, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;
        if (GuiGetStyle(CHECKBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_LEFT) textBounds.x = bounds.x - textBounds.width - GuiGetStyle(CHECKBOX, TEXT_PADDING);
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        Rectangle totalBounds = {
            (GuiGetStyle(CHECKBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_LEFT)? textBounds.x : bounds.x,
            bounds.y,
            bounds.width + textBounds.width + GuiGetStyle(CHECKBOX, TEXT_PADDING),
            bounds.height,
        };

        // Check checkbox state
        if (CheckCollisionPointRec(mousePoint, totalBounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;

            if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) checked = !checked;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, GuiGetStyle(CHECKBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(CHECKBOX, BORDER + (state*3))), guiAlpha), guiSkin != NULL ? Fade(GetColor(GuiGetStyle(CHECKBOX, BORDER + (state*3))), guiAlpha) : BLANK, guiSkin != NULL ? &guiSkin->checkbox.body : NULL, state);
    
    if (checked)
    {
        Rectangle check = { bounds.x + GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING),
                            bounds.y + GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING),
                            bounds.width - 2*(GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING)),
                            bounds.height - 2*(GuiGetStyle(CHECKBOX, BORDER_WIDTH) + GuiGetStyle(CHECKBOX, CHECK_PADDING)) };
        GuiDrawRectangle(check, 0, BLANK, Fade(GetColor(GuiGetStyle(CHECKBOX, TEXT + state*3)), guiAlpha), guiSkin != NULL ? &guiSkin->checkbox.checked : NULL, state);
    }

    if (text != NULL) GuiDrawText(text, textBounds, (GuiGetStyle(CHECKBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_RIGHT)? GUI_TEXT_ALIGN_LEFT : GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    return checked;
}

// Combo Box control, returns selected item index
int GuiComboBox(Rectangle bounds, const char *text, int active)
{
    GuiControlState state = guiState;

    bounds.width -= (GuiGetStyle(COMBOBOX, COMBO_BUTTON_WIDTH) + GuiGetStyle(COMBOBOX, COMBO_BUTTON_PADDING));

    Rectangle selector = { (float)bounds.x + bounds.width + GuiGetStyle(COMBOBOX, COMBO_BUTTON_PADDING),
                           (float)bounds.y, (float)GuiGetStyle(COMBOBOX, COMBO_BUTTON_WIDTH), (float)bounds.height };

    // Get substrings items from text (items pointers, lengths and count)
    int itemsCount = 0;
    const char **items = GuiTextSplit(text, &itemsCount, NULL);

    if (active < 0) active = 0;
    else if (active > itemsCount - 1) active = itemsCount - 1;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked && (itemsCount > 1))
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds) ||
            CheckCollisionPointRec(mousePoint, selector))
        {
            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                active += 1;
                if (active >= itemsCount) active = 0;
            }

            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    // Draw combo box main
    GuiDrawRectangle(bounds, GuiGetStyle(COMBOBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(COMBOBOX, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(COMBOBOX, BASE + (state*3))), guiAlpha), guiSkin != NULL ? &guiSkin->combobox.body : NULL, state);
    GuiDrawText(items[active], GetTextBounds(COMBOBOX, bounds), GuiGetStyle(COMBOBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(COMBOBOX, TEXT + (state*3))), guiAlpha));

    // Draw selector using a custom button
    // NOTE: BORDER_WIDTH and TEXT_ALIGNMENT forced values
    int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
    int tempTextAlign = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 1);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    
    GuiSkinStyle style = {0};
    if(guiSkin != NULL) { // Set the combobox style for the button
        style = guiSkin->button;
        guiSkin->button = guiSkin->combobox.button;
    }
    GuiButton(selector, TextFormat("%i/%i", active + 1, itemsCount));
    
    if(guiSkin != NULL) guiSkin->button = style; // Reset the button style

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlign);
    GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);
    //--------------------------------------------------------------------

    return active;
}

// Dropdown Box control
// NOTE: Returns mouse click
bool GuiDropdownBox(Rectangle bounds, const char *text, int *active, bool editMode)
{
    GuiControlState state = guiState;
    int itemSelected = *active;
    int itemFocused = -1;

    // Get substrings items from text (items pointers, lengths and count)
    int itemsCount = 0;
    const char **items = GuiTextSplit(text, &itemsCount, NULL);

    Rectangle boundsOpen = bounds;
    boundsOpen.height = (itemsCount + 1)*(bounds.height + GuiGetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING));

    Rectangle itemBounds = bounds;

    bool pressed = false;       // Check mouse button pressed

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked && (itemsCount > 1))
    {
        Vector2 mousePoint = GetMousePosition();

        if (editMode)
        {
            state = GUI_STATE_PRESSED;

            // Check if mouse has been pressed or released outside limits
            if (!CheckCollisionPointRec(mousePoint, boundsOpen))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) pressed = true;
            }

            // Check if already selected item has been pressed again
            if (CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pressed = true;

            // Check focused and selected item
            for (int i = 0; i < itemsCount; i++)
            {
                // Update item rectangle y position for next item
                itemBounds.y += (bounds.height + GuiGetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING));

                if (CheckCollisionPointRec(mousePoint, itemBounds))
                {
                    itemFocused = i;
                    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))
                    {
                        itemSelected = i;
                        pressed = true;     // Item selected, change to editMode = false
                    }
                    break;
                }
            }

            itemBounds = bounds;
        }
        else
        {
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                {
                    pressed = true;
                    state = GUI_STATE_PRESSED;
                }
                else state = GUI_STATE_FOCUSED;
            }
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (editMode) 
    { 
        GuiSkinPanel panel = {0};
        
        if(guiSkin != NULL) 
        { 
            // Draw dropdownbox panel using the correct style
            panel = guiSkin->panel; // Save the panel style
            guiSkin->panel = guiSkin->dropbox.listpanel; // Replace with the dropdown panel style
        }
        
        GuiPanel(boundsOpen);
        
        if(guiSkin != NULL) guiSkin->panel = panel; // Reset panel style
    }

    GuiDrawRectangle(bounds, GuiGetStyle(DROPDOWNBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BORDER + state*3)), guiAlpha), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BASE + state*3)), guiAlpha), guiSkin != NULL ? &guiSkin->dropbox.body : NULL, state);
    GuiDrawText(items[itemSelected], GetTextBounds(DEFAULT, bounds), GuiGetStyle(DROPDOWNBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT + state*3)), guiAlpha));

    GuiSkinStyle* itemStyle = guiSkin != NULL ? &guiSkin->dropbox.item : NULL;
    if (editMode)
    {
        // Draw visible items
        for (int i = 0; i < itemsCount; i++)
        {
            // Update item rectangle y position for next item
            itemBounds.y += (bounds.height + GuiGetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING));

            if (i == itemSelected)
            {
                GuiDrawRectangle(itemBounds, GuiGetStyle(DROPDOWNBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BORDER_COLOR_PRESSED)), guiAlpha), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BASE_COLOR_PRESSED)), guiAlpha), itemStyle, GUI_STATE_PRESSED);
                GuiDrawText(items[i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(DROPDOWNBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT_COLOR_PRESSED)), guiAlpha));
            }
            else if (i == itemFocused)
            {
                GuiDrawRectangle(itemBounds, GuiGetStyle(DROPDOWNBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BORDER_COLOR_FOCUSED)), guiAlpha), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, BASE_COLOR_FOCUSED)), guiAlpha), itemStyle, GUI_STATE_FOCUSED);
                GuiDrawText(items[i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(DROPDOWNBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT_COLOR_FOCUSED)), guiAlpha));
            }
            else GuiDrawText(items[i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(DROPDOWNBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT_COLOR_NORMAL)), guiAlpha));
        }
    }

    // TODO: Avoid this function, use icon instead or 'v'
    DrawTriangle(RAYGUI_CLITERAL(Vector2){ bounds.x + bounds.width - GuiGetStyle(DROPDOWNBOX, ARROW_PADDING), bounds.y + bounds.height/2 - 2 },
                 RAYGUI_CLITERAL(Vector2){ bounds.x + bounds.width - GuiGetStyle(DROPDOWNBOX, ARROW_PADDING) + 5, bounds.y + bounds.height/2 - 2 + 5 },
                 RAYGUI_CLITERAL(Vector2){ bounds.x + bounds.width - GuiGetStyle(DROPDOWNBOX, ARROW_PADDING) + 10, bounds.y + bounds.height/2 - 2 },
                 Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT + (state*3))), guiAlpha));

    //GuiDrawText("v", RAYGUI_CLITERAL(Rectangle){ bounds.x + bounds.width - GuiGetStyle(DROPDOWNBOX, ARROW_PADDING), bounds.y + bounds.height/2 - 2, 10, 10 },
    //            GUI_TEXT_ALIGN_CENTER, Fade(GetColor(GuiGetStyle(DROPDOWNBOX, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    *active = itemSelected;
    return pressed;
}

// Text Box control, updates input text
// NOTE 1: Requires static variables: framesCounter
// NOTE 2: Returns if KEY_ENTER pressed (useful for data validation)
bool GuiTextBox(Rectangle bounds, char *text, int textSize, bool editMode)
{
    static int framesCounter = 0;           // Required for blinking cursor

    GuiControlState state = guiState;
    bool pressed = false;

    Rectangle cursor = {
        bounds.x + GuiGetStyle(TEXTBOX, TEXT_PADDING) + GetTextWidth(text) + 2,
        bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE),
        1,
        GuiGetStyle(DEFAULT, TEXT_SIZE)*2
    };

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        if (editMode)
        {
            state = GUI_STATE_PRESSED;
            framesCounter++;

            int key = GetKeyPressed();
            int keyCount = strlen(text);

            // Only allow keys in range [32..125]
            if (keyCount < (textSize - 1))
            {
                int maxWidth = (bounds.width - (GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)*2));

                if (GetTextWidth(text) < (maxWidth - GuiGetStyle(DEFAULT, TEXT_SIZE)))
                {
                    if (((key >= 32) && (key <= 125)) ||
                        ((key >= 128) && (key < 255)))
                    {
                        text[keyCount] = (char)key;
                        keyCount++;
                        text[keyCount] = '\0';
                    }
                }
            }

            // Delete text
            if (keyCount > 0)
            {
                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    keyCount--;
                    text[keyCount] = '\0';
                    framesCounter = 0;
                    if (keyCount < 0) keyCount = 0;
                }
                else if (IsKeyDown(KEY_BACKSPACE))
                {
                    if ((framesCounter > TEXTEDIT_CURSOR_BLINK_FRAMES) && (framesCounter%2) == 0) keyCount--;
                    text[keyCount] = '\0';
                    if (keyCount < 0) keyCount = 0;
                }
            }

            if (IsKeyPressed(KEY_ENTER) || (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) pressed = true;

            // Check text alignment to position cursor properly
            int textAlignment = GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT);
            if (textAlignment == GUI_TEXT_ALIGN_CENTER) cursor.x = bounds.x + GetTextWidth(text)/2 + bounds.width/2 + 1;
            else if (textAlignment == GUI_TEXT_ALIGN_RIGHT) cursor.x = bounds.x + bounds.width - GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING);
        }
        else
        {
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                state = GUI_STATE_FOCUSED;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pressed = true;
            }
        }

        if (pressed) framesCounter = 0;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (state == GUI_STATE_PRESSED)
    {
        GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)), guiAlpha), guiSkin != NULL ? &guiSkin->textbox : NULL, GUI_STATE_PRESSED);

        // Draw blinking cursor
        if (editMode && ((framesCounter/20)%2 == 0)) GuiDrawRectangle(cursor, 0, BLANK, Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)), guiAlpha), NULL, state);
    }
    else if (state == GUI_STATE_DISABLED)
    {
        GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)), guiAlpha), guiSkin != NULL ? &guiSkin->textbox : NULL, GUI_STATE_DISABLED);
    }
    else GuiDrawRectangle(bounds, 1, Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), guiSkin != NULL ? Fade(GetColor(GuiGetStyle(TEXTBOX, BASE + (state*3))), guiAlpha) : BLANK, guiSkin != NULL ? &guiSkin->textbox : NULL, state);

    GuiDrawText(text, GetTextBounds(TEXTBOX, bounds), GuiGetStyle(TEXTBOX, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(TEXTBOX, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    return pressed;
}

// Spinner control, returns selected value
bool GuiSpinner(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode)
{
    GuiControlState state = guiState;

    bool pressed = false;
    int tempValue = *value;

    Rectangle spinner = { bounds.x + GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH) + GuiGetStyle(SPINNER, SPIN_BUTTON_PADDING), bounds.y,
                          bounds.width - 2*(GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH) + GuiGetStyle(SPINNER, SPIN_BUTTON_PADDING)), bounds.height };
    Rectangle leftButtonBound = { (float)bounds.x, (float)bounds.y, (float)GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.height };
    Rectangle rightButtonBound = { (float)bounds.x + bounds.width - GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.y, (float)GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.height };

    Rectangle textBounds = { 0 };
    if (text != NULL)
    {
        textBounds.width = GetTextWidth(text);
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(SPINNER, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;
        if (GuiGetStyle(SPINNER, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_LEFT) textBounds.x = bounds.x - textBounds.width - GuiGetStyle(SPINNER, TEXT_PADDING);
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check spinner state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;
        }
    }

    if (!editMode)
    {
        if (tempValue < minValue) tempValue = minValue;
        if (tempValue > maxValue) tempValue = maxValue;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiSkinStyle style = {0};
    if(guiSkin != NULL) { // Change to the correct value box style
        style = guiSkin->valuebox;
        guiSkin->valuebox = guiSkin->spinner.valuebox;
    }
    // TODO: Set Spinner properties for ValueBox
    pressed = GuiValueBox(spinner, NULL, &tempValue, minValue, maxValue, editMode);
    if(guiSkin != NULL) { 
        guiSkin->valuebox = style; // Reset style for valuebox
        style = guiSkin->button; // Save the button style
    }

    // Draw value selector custom buttons
    // NOTE: BORDER_WIDTH and TEXT_ALIGNMENT forced values
    int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
    int tempTextAlign = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    GuiSetStyle(BUTTON, BORDER_WIDTH, GuiGetStyle(SPINNER, BORDER_WIDTH));
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

    if(guiSkin != NULL) guiSkin->button = guiSkin->spinner.left; // Change style for the left button
#if defined(RAYGUI_SUPPORT_ICONS)
    if (GuiButton(leftButtonBound, GuiIconText(RICON_ARROW_LEFT_FILL, NULL))) tempValue--;
    if(guiSkin != NULL) guiSkin->button = guiSkin->spinner.right; // Change style for the right button
    if (GuiButton(rightButtonBound, GuiIconText(RICON_ARROW_RIGHT_FILL, NULL))) tempValue++;
#else
    if (GuiButton(leftButtonBound, "<")) tempValue--;
    if(guiSkin != NULL) guiSkin->button = guiSkin->spinner.right; // Change style for the right button
    if (GuiButton(rightButtonBound, ">")) tempValue++;
#endif
    if(guiSkin != NULL) guiSkin->button = style; // Reset button style

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlign);
    GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);

    // Draw text label if provided
    if (text != NULL) GuiDrawText(text, textBounds, (GuiGetStyle(SPINNER, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_RIGHT)? GUI_TEXT_ALIGN_LEFT : GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    *value = tempValue;
    return pressed;
}

// Value Box control, updates input text with numbers
// NOTE: Requires static variables: framesCounter
bool GuiValueBox(Rectangle bounds, const char *text, int *value, int minValue, int maxValue, bool editMode)
{
    #if !defined(VALUEBOX_MAX_CHARS)
        #define VALUEBOX_MAX_CHARS  32
    #endif

    static int framesCounter = 0;           // Required for blinking cursor

    GuiControlState state = guiState;
    bool pressed = false;

    char textValue[VALUEBOX_MAX_CHARS + 1] = "\0";
    sprintf(textValue, "%i", *value);

    Rectangle textBounds = { 0 };
    if (text != NULL)
    {
        textBounds.width = GetTextWidth(text);
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(VALUEBOX, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;
        if (GuiGetStyle(VALUEBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_LEFT) textBounds.x = bounds.x - textBounds.width - GuiGetStyle(VALUEBOX, TEXT_PADDING);
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        bool valueHasChanged = false;

        if (editMode)
        {
            state = GUI_STATE_PRESSED;

            framesCounter++;

            int keyCount = strlen(textValue);

            // Only allow keys in range [48..57]
            if (keyCount < VALUEBOX_MAX_CHARS)
            {
                int maxWidth = bounds.width;
                if (GetTextWidth(textValue) < maxWidth)
                {
                    int key = GetKeyPressed();
                    if ((key >= 48) && (key <= 57))
                    {
                        textValue[keyCount] = (char)key;
                        keyCount++;
                        valueHasChanged = true;
                    }
                }
            }

            // Delete text
            if (keyCount > 0)
            {
                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    keyCount--;
                    textValue[keyCount] = '\0';
                    framesCounter = 0;
                    if (keyCount < 0) keyCount = 0;
                    valueHasChanged = true;
                }
                else if (IsKeyDown(KEY_BACKSPACE))
                {
                    if ((framesCounter > TEXTEDIT_CURSOR_BLINK_FRAMES) && (framesCounter%2) == 0) keyCount--;
                    textValue[keyCount] = '\0';
                    if (keyCount < 0) keyCount = 0;
                    valueHasChanged = true;
                }
            }

            if (valueHasChanged) *value = TextToInteger(textValue);

            if (IsKeyPressed(KEY_ENTER) || (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON))) pressed = true;
        }
        else
        {
            if (*value > maxValue) *value = maxValue;
            else if (*value < minValue) *value = minValue;

            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                state = GUI_STATE_FOCUSED;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pressed = true;
            }
        }

        if (pressed) framesCounter = 0;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    Color baseColor = guiSkin != NULL ? GetColor(GuiGetStyle(VALUEBOX, BASE + (state*3))) : BLANK;
    if (state == GUI_STATE_PRESSED) baseColor = GetColor(GuiGetStyle(VALUEBOX, BASE_COLOR_PRESSED));
    else if (state == GUI_STATE_DISABLED) baseColor = GetColor(GuiGetStyle(VALUEBOX, BASE_COLOR_DISABLED));
    
    // WARNING: BLANK color does not work properly with Fade()
    GuiDrawRectangle(bounds, GuiGetStyle(VALUEBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(VALUEBOX, BORDER + (state*3))), guiAlpha), baseColor, guiSkin != NULL ? &guiSkin->valuebox : NULL, state);
    GuiDrawText(textValue, GetTextBounds(VALUEBOX, bounds), GUI_TEXT_ALIGN_CENTER, Fade(GetColor(GuiGetStyle(VALUEBOX, TEXT + (state*3))), guiAlpha));

    // Draw blinking cursor
    if ((state == GUI_STATE_PRESSED) && (editMode && ((framesCounter/20)%2 == 0))) 
    {
        // NOTE: ValueBox internal text is always centered
        Rectangle cursor = { bounds.x + GetTextWidth(textValue)/2 + bounds.width/2 + 2, bounds.y + 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), 1, bounds.height - 4*GuiGetStyle(VALUEBOX, BORDER_WIDTH) };
        GuiDrawRectangle(cursor, 0, BLANK, Fade(GetColor(GuiGetStyle(VALUEBOX, BORDER_COLOR_PRESSED)), guiAlpha), NULL, state);
    }
    
    // Draw text label if provided
    if (text != NULL) GuiDrawText(text, textBounds, (GuiGetStyle(VALUEBOX, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_RIGHT)? GUI_TEXT_ALIGN_LEFT : GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    return pressed;
}

// Text Box control with multiple lines
bool GuiTextBoxMulti(Rectangle bounds, char *text, int textSize, bool editMode)
{
    static int framesCounter = 0;           // Required for blinking cursor

    GuiControlState state = guiState;
    bool pressed = false;

    Rectangle textAreaBounds = {
        bounds.x + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING),
        bounds.y + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING),
        bounds.width - 2*GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING),
        bounds.height - 2*GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)
    };

    // Cursor position, [x, y] values should be updated
    Rectangle cursor = { 0, 0, 1, GuiGetStyle(DEFAULT, TEXT_SIZE) + 2 };

    int textWidth = 0;
    int currentLine = 0;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        if (editMode)
        {
            state = GUI_STATE_PRESSED;
            framesCounter++;

            int key = GetKeyPressed();
            int keyCount = strlen(text);

            // Introduce characters
            if (keyCount < (textSize - 1))
            {
                Vector2 textSize = MeasureTextEx(guiFont, text, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING));

                if (textSize.y < (textAreaBounds.height - GuiGetStyle(DEFAULT, TEXT_SIZE)))
                {
                    if (IsKeyPressed(KEY_ENTER))
                    {
                        text[keyCount] = '\n';
                        keyCount++;
                    }
                    else if (((key >= 32) && (key < 255)))  // TODO: Support Unicode inputs
                    {
                        text[keyCount] = (char)key;
                        keyCount++;
                    }
                }
            }

            // Delete characters
            if (keyCount > 0)
            {
                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    keyCount--;
                    text[keyCount] = '\0';
                    framesCounter = 0;

                    if (keyCount < 0) keyCount = 0;
                }
                else if (IsKeyDown(KEY_BACKSPACE))
                {
                    if ((framesCounter > TEXTEDIT_CURSOR_BLINK_FRAMES) && (framesCounter%2) == 0) keyCount--;
                    text[keyCount] = '\0';

                    if (keyCount < 0) keyCount = 0;
                }
            }

            // Calculate cursor position considering text
            char oneCharText[2] = { 0 };
            int lastSpacePos = -1;

            for (int i = 0; i < keyCount && currentLine < keyCount; i++)
            {
                oneCharText[0] = text[i];
                textWidth += (GetTextWidth(oneCharText) + GuiGetStyle(DEFAULT, TEXT_SPACING));

                if (text[i] == ' ') lastSpacePos = i;

                if (textWidth >= textAreaBounds.width)
                {
                    currentLine++;
                    textWidth = 0;

                    if(lastSpacePos > 0)
                        i = lastSpacePos;
                    else
                        textWidth += (GetTextWidth(oneCharText) + GuiGetStyle(DEFAULT, TEXT_SPACING));

                    lastSpacePos = -1;
                }
            }

            cursor.x = bounds.x + GuiGetStyle(TEXTBOX, BORDER_WIDTH) + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING) + textWidth - GuiGetStyle(DEFAULT, TEXT_SPACING);
            cursor.y = bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH) + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)/2 + ((GuiGetStyle(DEFAULT, TEXT_SIZE) + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING))*currentLine);

            // Exit edit mode
            if (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pressed = true;
        }
        else
        {
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                state = GUI_STATE_FOCUSED;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) pressed = true;
            }
        }

        if (pressed) framesCounter = 0;     // Reset blinking cursor
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (state == GUI_STATE_PRESSED)
    {
        GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_PRESSED)), guiAlpha), guiSkin != NULL ? &guiSkin->textboxmulti : NULL, GUI_STATE_PRESSED);

        // Draw blinking cursor
        if (editMode && ((framesCounter/20)%2 == 0)) GuiDrawRectangle(cursor, 0, BLANK, Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)), guiAlpha), NULL, state);
    }
    else if (state == GUI_STATE_DISABLED)
    {
        GuiDrawRectangle(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)), guiAlpha), guiSkin != NULL ? &guiSkin->textboxmulti : NULL, GUI_STATE_DISABLED);
    }
    else GuiDrawRectangle(bounds, 1, Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha), guiSkin != NULL ? Fade(GetColor(GuiGetStyle(TEXTBOX, BASE + (state*3))), guiAlpha) : BLANK, guiSkin != NULL ? &guiSkin->textboxmulti : NULL, state);

    DrawTextRec(guiFont, text, textAreaBounds, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING), true, Fade(GetColor(GuiGetStyle(TEXTBOX, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------

    return pressed;
}

// Slider control with pro parameters
// NOTE: Other GuiSlider*() controls use this one
float GuiSliderPro(Rectangle bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue, int sliderWidth)
{
    GuiControlState state = guiState;

    int sliderValue = (int)(((value - minValue)/(maxValue - minValue))*(bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH)));

    Rectangle slider = { bounds.x, bounds.y + GuiGetStyle(SLIDER, BORDER_WIDTH) + GuiGetStyle(SLIDER, SLIDER_PADDING),
                         0, bounds.height - 2*GuiGetStyle(SLIDER, BORDER_WIDTH) - 2*GuiGetStyle(SLIDER, SLIDER_PADDING) };

    if (sliderWidth > 0)        // Slider
    {
        slider.x += (sliderValue - sliderWidth/2);
        slider.width = sliderWidth;
    }
    else if (sliderWidth == 0)  // SliderBar
    {
        slider.x += GuiGetStyle(SLIDER, BORDER_WIDTH);
        slider.width = sliderValue;
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                state = GUI_STATE_PRESSED;

                // Get equivalent value and slider position from mousePoint.x
                value = ((maxValue - minValue)*(mousePoint.x - (float)(bounds.x + sliderWidth/2)))/(float)(bounds.width - sliderWidth) + minValue;

                if (sliderWidth > 0) slider.x = mousePoint.x - slider.width/2;  // Slider
                else if (sliderWidth == 0) slider.width = sliderValue;          // SliderBar
            }
            else state = GUI_STATE_FOCUSED;
        }

        if (value > maxValue) value = maxValue;
        else if (value < minValue) value = minValue;
    }

    // Bar limits check
    if (sliderWidth > 0)        // Slider
    {
        if (slider.x <= (bounds.x + GuiGetStyle(SLIDER, BORDER_WIDTH))) slider.x = bounds.x + GuiGetStyle(SLIDER, BORDER_WIDTH);
        else if ((slider.x + slider.width) >= (bounds.x + bounds.width)) slider.x = bounds.x + bounds.width - slider.width - GuiGetStyle(SLIDER, BORDER_WIDTH);
    }
    else if (sliderWidth == 0)  // SliderBar
    {
        if (slider.width > bounds.width) slider.width = bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH);
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, GuiGetStyle(SLIDER, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(SLIDER, BORDER + (state*3))), guiAlpha), Fade(GetColor(GuiGetStyle(SLIDER, (state != GUI_STATE_DISABLED)?  BASE_COLOR_NORMAL : BASE_COLOR_DISABLED)), guiAlpha), guiSkin != NULL ? &guiSkin->slider.body : NULL, state);
    
    GuiSkinStyle* style = guiSkin != NULL ? (sliderWidth == 0 ? &guiSkin->slider.bar : &guiSkin->slider.slider) : NULL;
    // Draw slider internal bar (depends on state)
    if ((state == GUI_STATE_NORMAL) || (state == GUI_STATE_PRESSED)) 
        GuiDrawRectangle(slider, 0, BLANK, Fade(GetColor(GuiGetStyle(SLIDER, BASE_COLOR_PRESSED)), guiAlpha), style, state);
    else if (state == GUI_STATE_FOCUSED) 
        GuiDrawRectangle(slider, 0, BLANK, Fade(GetColor(GuiGetStyle(SLIDER, TEXT_COLOR_FOCUSED)), guiAlpha), style, state);

    // Draw left/right text if provided
    if (textLeft != NULL)
    {
        Rectangle textBounds = { 0 };
        textBounds.width = GetTextWidth(textLeft);  // TODO: Consider text icon
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x - textBounds.width - GuiGetStyle(SLIDER, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        GuiDrawText(textLeft, textBounds, GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(SLIDER, TEXT + (state*3))), guiAlpha));
    }

    if (textRight != NULL)
    {
        Rectangle textBounds = { 0 };
        textBounds.width = GetTextWidth(textRight);  // TODO: Consider text icon
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(SLIDER, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        GuiDrawText(textRight, textBounds, GUI_TEXT_ALIGN_LEFT, Fade(GetColor(GuiGetStyle(SLIDER, TEXT + (state*3))), guiAlpha));
    }
    //--------------------------------------------------------------------

    return value;
}

// Slider control extended, returns selected value and has text
float GuiSlider(Rectangle bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue)
{
    return GuiSliderPro(bounds, textLeft, textRight, value, minValue, maxValue, GuiGetStyle(SLIDER, SLIDER_WIDTH));
}

// Slider Bar control extended, returns selected value
float GuiSliderBar(Rectangle bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue)
{
    return GuiSliderPro(bounds, textLeft, textRight, value, minValue, maxValue, 0);
}

// Progress Bar control extended, shows current progress value
float GuiProgressBar(Rectangle bounds, const char *textLeft, const char *textRight, float value, float minValue, float maxValue)
{
    GuiControlState state = guiState;

    Rectangle progress = { bounds.x + GuiGetStyle(PROGRESSBAR, BORDER_WIDTH),
                           bounds.y + GuiGetStyle(PROGRESSBAR, BORDER_WIDTH) + GuiGetStyle(PROGRESSBAR, PROGRESS_PADDING), 0,
                           bounds.height - 2*GuiGetStyle(PROGRESSBAR, BORDER_WIDTH) - 2*GuiGetStyle(PROGRESSBAR, PROGRESS_PADDING) };

    // Update control
    //--------------------------------------------------------------------
    if (state != GUI_STATE_DISABLED) progress.width = (int)(value/(maxValue - minValue)*(float)(bounds.width - 2*GuiGetStyle(PROGRESSBAR, BORDER_WIDTH)));
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, GuiGetStyle(PROGRESSBAR, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(PROGRESSBAR, BORDER + (state*3))), guiAlpha), guiSkin != NULL ? Fade(GetColor(GuiGetStyle(PROGRESSBAR, BASE + (state*3))), guiAlpha) : BLANK, guiSkin != NULL ? &guiSkin->progressbar.body : NULL, state);

    // Draw slider internal progress bar (depends on state)
    if ((state == GUI_STATE_NORMAL) || (state == GUI_STATE_PRESSED)) GuiDrawRectangle(progress, 0, BLANK, Fade(GetColor(GuiGetStyle(PROGRESSBAR, BASE_COLOR_PRESSED)), guiAlpha), guiSkin != NULL ? &guiSkin->progressbar.progress : NULL, state);
    else if (state == GUI_STATE_FOCUSED) GuiDrawRectangle(progress, 0, BLANK, Fade(GetColor(GuiGetStyle(PROGRESSBAR, TEXT_COLOR_FOCUSED)), guiAlpha), guiSkin != NULL ? &guiSkin->progressbar.progress : NULL, state);

    // Draw left/right text if provided
    if (textLeft != NULL)
    {
        Rectangle textBounds = { 0 };
        textBounds.width = GetTextWidth(textLeft);  // TODO: Consider text icon
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x - textBounds.width - GuiGetStyle(PROGRESSBAR, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        GuiDrawText(textLeft, textBounds, GUI_TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(PROGRESSBAR, TEXT + (state*3))), guiAlpha));
    }

    if (textRight != NULL)
    {
        Rectangle textBounds = { 0 };
        textBounds.width = GetTextWidth(textRight);  // TODO: Consider text icon
        textBounds.height = GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(PROGRESSBAR, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height/2 - GuiGetStyle(DEFAULT, TEXT_SIZE)/2;

        GuiDrawText(textRight, textBounds, GUI_TEXT_ALIGN_LEFT, Fade(GetColor(GuiGetStyle(PROGRESSBAR, TEXT + (state*3))), guiAlpha));
    }
    //--------------------------------------------------------------------

    return value;
}

// Status Bar control
void GuiStatusBar(Rectangle bounds, const char *text)
{
    GuiControlState state = guiState;

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, GuiGetStyle(STATUSBAR, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(STATUSBAR, (state != GUI_STATE_DISABLED)? BORDER_COLOR_NORMAL : BORDER_COLOR_DISABLED)), guiAlpha),
                     Fade(GetColor(GuiGetStyle(STATUSBAR, (state != GUI_STATE_DISABLED)? BASE_COLOR_NORMAL : BASE_COLOR_DISABLED)), guiAlpha), guiSkin != NULL ? &guiSkin->status : NULL, state);
    GuiDrawText(text, GetTextBounds(STATUSBAR, bounds), GuiGetStyle(STATUSBAR, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(STATUSBAR, (state != GUI_STATE_DISABLED)? TEXT_COLOR_NORMAL : TEXT_COLOR_DISABLED)), guiAlpha));
    //--------------------------------------------------------------------
}

// Dummy rectangle control, intended for placeholding
void GuiDummyRec(Rectangle bounds, const char *text)
{
    GuiControlState state = guiState;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check button state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = GUI_STATE_PRESSED;
            else state = GUI_STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiDrawRectangle(bounds, 0, BLANK, Fade(GetColor(GuiGetStyle(DEFAULT, (state != GUI_STATE_DISABLED)? BASE_COLOR_NORMAL : BASE_COLOR_DISABLED)), guiAlpha), NULL, state);
    GuiDrawText(text, GetTextBounds(DEFAULT, bounds), GUI_TEXT_ALIGN_CENTER, Fade(GetColor(GuiGetStyle(BUTTON, (state != GUI_STATE_DISABLED)? TEXT_COLOR_NORMAL : TEXT_COLOR_DISABLED)), guiAlpha));
    //------------------------------------------------------------------
}

// Scroll Bar control
// TODO: I feel GuiScrollBar could be simplified...
int GuiScrollBar(Rectangle bounds, int value, int minValue, int maxValue)
{
    GuiControlState state = guiState;

    // Is the scrollbar horizontal or vertical?
    bool isVertical = (bounds.width > bounds.height)? false : true;

    // The size (width or height depending on scrollbar type) of the spinner buttons
    const int spinnerSize = GuiGetStyle(SCROLLBAR, ARROWS_VISIBLE)? (isVertical? bounds.width - 2*GuiGetStyle(SCROLLBAR, BORDER_WIDTH) : bounds.height - 2*GuiGetStyle(SCROLLBAR, BORDER_WIDTH)) : 0;

    // Arrow buttons [<] [>] [∧] [∨]
    Rectangle arrowUpLeft = { 0 };
    Rectangle arrowDownRight = { 0 };

    // Actual area of the scrollbar excluding the arrow buttons
    Rectangle scrollbar = { 0 };

    // Slider bar that moves     --[///]-----
    Rectangle slider = { 0 };

    // Normalize value
    if (value > maxValue) value = maxValue;
    if (value < minValue) value = minValue;

    const int range = maxValue - minValue;
    int sliderSize = GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE);

    // Calculate rectangles for all of the components
    arrowUpLeft = RAYGUI_CLITERAL(Rectangle){ (float)bounds.x + GuiGetStyle(SCROLLBAR, BORDER_WIDTH), (float)bounds.y + GuiGetStyle(SCROLLBAR, BORDER_WIDTH), (float)spinnerSize, (float)spinnerSize };

    if (isVertical)
    {
        arrowDownRight = RAYGUI_CLITERAL(Rectangle){ (float)bounds.x + GuiGetStyle(SCROLLBAR, BORDER_WIDTH), (float)bounds.y + bounds.height - spinnerSize - GuiGetStyle(SCROLLBAR, BORDER_WIDTH), (float)spinnerSize, (float)spinnerSize};
        scrollbar = RAYGUI_CLITERAL(Rectangle){ bounds.x + GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_PADDING), arrowUpLeft.y + arrowUpLeft.height, bounds.width - 2*(GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_PADDING)), bounds.height - arrowUpLeft.height - arrowDownRight.height - 2*GuiGetStyle(SCROLLBAR, BORDER_WIDTH) };
        sliderSize = (sliderSize >= scrollbar.height)? (scrollbar.height - 2) : sliderSize;     // Make sure the slider won't get outside of the scrollbar
        slider = RAYGUI_CLITERAL(Rectangle){ (float)bounds.x + GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING), (float)scrollbar.y + (int)(((float)(value - minValue)/range)*(scrollbar.height - sliderSize)), (float)bounds.width - 2*(GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING)), (float)sliderSize };
    }
    else
    {
        arrowDownRight = RAYGUI_CLITERAL(Rectangle){ (float)bounds.x + bounds.width - spinnerSize - GuiGetStyle(SCROLLBAR, BORDER_WIDTH), (float)bounds.y + GuiGetStyle(SCROLLBAR, BORDER_WIDTH), (float)spinnerSize, (float)spinnerSize};
        scrollbar = RAYGUI_CLITERAL(Rectangle){ arrowUpLeft.x + arrowUpLeft.width, bounds.y + GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_PADDING), bounds.width - arrowUpLeft.width - arrowDownRight.width - 2*GuiGetStyle(SCROLLBAR, BORDER_WIDTH), bounds.height - 2*(GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_PADDING))};
        sliderSize = (sliderSize >= scrollbar.width)? (scrollbar.width - 2) : sliderSize;       // Make sure the slider won't get outside of the scrollbar
        slider = RAYGUI_CLITERAL(Rectangle){ (float)scrollbar.x + (int)(((float)(value - minValue)/range)*(scrollbar.width - sliderSize)), (float)bounds.y + GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING), (float)sliderSize, (float)bounds.height - 2*(GuiGetStyle(SCROLLBAR, BORDER_WIDTH) + GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING)) };
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            state = GUI_STATE_FOCUSED;

            // Handle mouse wheel
            int wheel = GetMouseWheelMove();
            if (wheel != 0) value += wheel;

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                if (CheckCollisionPointRec(mousePoint, arrowUpLeft)) value -= range/GuiGetStyle(SCROLLBAR, SCROLL_SPEED);
                else if (CheckCollisionPointRec(mousePoint, arrowDownRight)) value += range/GuiGetStyle(SCROLLBAR, SCROLL_SPEED);

                state = GUI_STATE_PRESSED;
            }
            else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                if (!isVertical)
                {
                    Rectangle scrollArea = { arrowUpLeft.x + arrowUpLeft.width, arrowUpLeft.y, scrollbar.width, bounds.height - 2*GuiGetStyle(SCROLLBAR, BORDER_WIDTH)};
                    if (CheckCollisionPointRec(mousePoint, scrollArea)) value = ((float)(mousePoint.x - scrollArea.x - slider.width/2)*range)/(scrollArea.width - slider.width) + minValue;
                }
                else
                {
                    Rectangle scrollArea = { arrowUpLeft.x, arrowUpLeft.y+arrowUpLeft.height, bounds.width - 2*GuiGetStyle(SCROLLBAR, BORDER_WIDTH),  scrollbar.height};
                    if (CheckCollisionPointRec(mousePoint, scrollArea)) value = ((float)(mousePoint.y - scrollArea.y - slider.height/2)*range)/(scrollArea.height - slider.height) + minValue;
                }
            }
        }

        // Normalize value
        if (value > maxValue) value = maxValue;
        if (value < minValue) value = minValue;
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiSkinScrollbar* skin = guiSkin != NULL ? &guiSkin->scrollbar : NULL;
    GuiDrawRectangle(bounds, GuiGetStyle(SCROLLBAR, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER + state*3)), guiAlpha), Fade(GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_DISABLED)), guiAlpha), skin != NULL ? &skin->body : NULL, state);   // Draw the background
    
    GuiDrawRectangle(scrollbar, 0, BLANK, Fade(GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL)), guiAlpha), skin != NULL ? &skin->rail : NULL, state);     // Draw the scrollbar active area background
    GuiDrawRectangle(slider, 0, BLANK, Fade(GetColor(GuiGetStyle(SLIDER, BORDER + state*3)), guiAlpha), skin != NULL ? &skin->slider : NULL, state);         // Draw the slider bar

    // Draw arrows
    const int padding = (spinnerSize - GuiGetStyle(SCROLLBAR, ARROWS_SIZE))/2;
    const Vector2 lineCoords[] =
    {
        // Coordinates for <     0,1,2
        { arrowUpLeft.x + padding, arrowUpLeft.y + spinnerSize/2 },
        { arrowUpLeft.x + spinnerSize - padding, arrowUpLeft.y + padding },
        { arrowUpLeft.x + spinnerSize - padding, arrowUpLeft.y + spinnerSize - padding },

        // Coordinates for >     3,4,5
        { arrowDownRight.x + padding, arrowDownRight.y + padding },
        { arrowDownRight.x + spinnerSize - padding, arrowDownRight.y + spinnerSize/2 },
        { arrowDownRight.x + padding, arrowDownRight.y + spinnerSize - padding },

        // Coordinates for ∧     6,7,8
        { arrowUpLeft.x + spinnerSize/2, arrowUpLeft.y + padding },
        { arrowUpLeft.x + padding, arrowUpLeft.y + spinnerSize - padding },
        { arrowUpLeft.x + spinnerSize - padding, arrowUpLeft.y + spinnerSize - padding },

        // Coordinates for ∨     9,10,11
        { arrowDownRight.x + padding, arrowDownRight.y + padding },
        { arrowDownRight.x + spinnerSize/2, arrowDownRight.y + spinnerSize - padding },
        { arrowDownRight.x + spinnerSize - padding, arrowDownRight.y + padding }
    };

    Color lineColor = Fade(GetColor(GuiGetStyle(BUTTON, TEXT + state*3)), guiAlpha);

    if (GuiGetStyle(SCROLLBAR, ARROWS_VISIBLE))
    {
        if (isVertical)
        {
            DrawTriangle(lineCoords[6], lineCoords[7], lineCoords[8], lineColor);
            DrawTriangle(lineCoords[9], lineCoords[10], lineCoords[11], lineColor);
        }
        else
        {
            DrawTriangle(lineCoords[2], lineCoords[1], lineCoords[0], lineColor);
            DrawTriangle(lineCoords[5], lineCoords[4], lineCoords[3], lineColor);
        }
    }
    //--------------------------------------------------------------------

    return value;
}

// List View control
int GuiListView(Rectangle bounds, const char *text, int *scrollIndex, int active)
{
    int itemsCount = 0;
    const char **items = NULL;

    if (text != NULL) items = GuiTextSplit(text, &itemsCount, NULL);

    return GuiListViewEx(bounds, items, itemsCount, NULL, scrollIndex, active);
}

// List View control with extended parameters
int GuiListViewEx(Rectangle bounds, const char **text, int count, int *focus, int *scrollIndex, int active)
{
    GuiControlState state = guiState;
    int itemFocused = (focus == NULL)? -1 : *focus;
    int itemSelected = active;

    // Check if we need a scroll bar
    bool useScrollBar = false;
    if ((GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING))*count > bounds.height) useScrollBar = true;

    // Define base item rectangle [0]
    Rectangle itemBounds = { 0 };
    itemBounds.x = bounds.x + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING);
    itemBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING) + GuiGetStyle(DEFAULT, BORDER_WIDTH);
    itemBounds.width = bounds.width - 2*GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING) - GuiGetStyle(DEFAULT, BORDER_WIDTH);
    itemBounds.height = GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
    if (useScrollBar) itemBounds.width -= GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH);

    // Get items on the list
    int visibleItems = bounds.height/(GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING));
    if (visibleItems > count) visibleItems = count;

    int startIndex = (scrollIndex == NULL)? 0 : *scrollIndex;
    if ((startIndex < 0) || (startIndex > (count - visibleItems))) startIndex = 0;
    int endIndex = startIndex + visibleItems;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check mouse inside list view
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            state = GUI_STATE_FOCUSED;

            // Check focused and selected item
            for (int i = 0; i < visibleItems; i++)
            {
                if (CheckCollisionPointRec(mousePoint, itemBounds))
                {
                    itemFocused = startIndex + i;
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        if (itemSelected == (startIndex + i)) itemSelected = -1;
                        else itemSelected = startIndex + i;
                    }
                    break;
                }

                // Update item rectangle y position for next item
                itemBounds.y += (GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING));
            }

            if (useScrollBar)
            {
                int wheelMove = GetMouseWheelMove();
                startIndex -= wheelMove;

                if (startIndex < 0) startIndex = 0;
                else if (startIndex > (count - visibleItems)) startIndex = count - visibleItems;

                endIndex = startIndex + visibleItems;
                if (endIndex > count) endIndex = count;
            }
        }
        else itemFocused = -1;

        // Reset item rectangle y to [0]
        itemBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING) + GuiGetStyle(DEFAULT, BORDER_WIDTH);
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    GuiSkinList* skin = guiSkin != NULL ? &guiSkin->list : NULL;
    GuiDrawRectangle(bounds, GuiGetStyle(DEFAULT, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER + state*3)), guiAlpha), GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), skin != NULL ? &skin->body : NULL, state);     // Draw background

    // Draw visible items
    for (int i = 0; ((i < visibleItems) && (text != NULL)); i++)
    {
        if (state == GUI_STATE_DISABLED)
        {
            if ((startIndex + i) == itemSelected) GuiDrawRectangle(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_DISABLED)), guiAlpha), Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_DISABLED)), guiAlpha), skin != NULL ? &skin->item : NULL, state);

            GuiDrawText(text[startIndex + i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_DISABLED)), guiAlpha));
        }
        else
        {
            if ((startIndex + i) == itemSelected)
            {
                // Draw item selected
                GuiDrawRectangle(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_PRESSED)), guiAlpha), Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_PRESSED)), guiAlpha), skin != NULL ? &skin->item : NULL, GUI_STATE_PRESSED);
                GuiDrawText(text[startIndex + i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_PRESSED)), guiAlpha));
            }
            else if ((startIndex + i) == itemFocused)
            {
                // Draw item focused
                GuiDrawRectangle(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_FOCUSED)), guiAlpha), Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_FOCUSED)), guiAlpha), skin != NULL ? &skin->item : NULL, GUI_STATE_FOCUSED);
                GuiDrawText(text[startIndex + i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_FOCUSED)), guiAlpha));
            }
            else
            {
                // Draw item normal
                if(skin != NULL) GuiDrawRectangle(itemBounds, 0, BLANK, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_NORMAL)), guiAlpha), &skin->item, GUI_STATE_NORMAL);
                GuiDrawText(text[startIndex + i], GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_NORMAL)), guiAlpha));
            }
        }

        // Update item rectangle y position for next item
        itemBounds.y += (GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING));
    }

    if (useScrollBar)
    {
        Rectangle scrollBarBounds = {
            bounds.x + bounds.width - GuiGetStyle(LISTVIEW, BORDER_WIDTH) - GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH),
            bounds.y + GuiGetStyle(LISTVIEW, BORDER_WIDTH), (float)GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH),
            bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH)
        };

        // Calculate percentage of visible items and apply same percentage to scrollbar
        float percentVisible = (float)(endIndex - startIndex)/count;
        float sliderSize = bounds.height*percentVisible;

        int prevSliderSize = GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE);   // Save default slider size
        int prevScrollSpeed = GuiGetStyle(SCROLLBAR, SCROLL_SPEED); // Save default scroll speed
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, sliderSize);            // Change slider size
        GuiSetStyle(SCROLLBAR, SCROLL_SPEED, count - visibleItems); // Change scroll speed
        
        GuiSkinScrollbar scroll = {0}; // Temporary skin style for the scrollbar
        if(skin != NULL) { 
            scroll = guiSkin->scrollbar;  // Copy scrollbar skin style
            guiSkin->scrollbar = skin->scrollbar; // Override the default scrollbar skin with our list skin style
        }
        startIndex = GuiScrollBar(scrollBarBounds, startIndex, 0, count - visibleItems);
        if(skin != NULL) guiSkin->scrollbar = scroll; // Reset the scrollbar skin style

        GuiSetStyle(SCROLLBAR, SCROLL_SPEED, prevScrollSpeed); // Reset scroll speed to default
        GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, prevSliderSize); // Reset slider size to default
    }
    //--------------------------------------------------------------------

    if (focus != NULL) *focus = itemFocused;
    if (scrollIndex != NULL) *scrollIndex = startIndex;

    return itemSelected;
}

// Color Panel control
Color GuiColorPanelEx(Rectangle bounds, Color color, float hue)
{
    GuiControlState state = guiState;
    Vector2 pickerSelector = { 0 };

    Vector3 vcolor = { (float)color.r/255.0f, (float)color.g/255.0f, (float)color.b/255.0f };
    Vector3 hsv = ConvertRGBtoHSV(vcolor);

    pickerSelector.x = bounds.x + (float)hsv.y*bounds.width;            // HSV: Saturation
    pickerSelector.y = bounds.y + (1.0f - (float)hsv.z)*bounds.height;  // HSV: Value

    Vector3 maxHue = { hue >= 0.0f ? hue : hsv.x, 1.0f, 1.0f };
    Vector3 rgbHue = ConvertHSVtoRGB(maxHue);
    Color maxHueCol = { (unsigned char)(255.0f*rgbHue.x),
                      (unsigned char)(255.0f*rgbHue.y),
                      (unsigned char)(255.0f*rgbHue.z), 255 };

    const Color colWhite = { 255, 255, 255, 255 };
    const Color colBlack = { 0, 0, 0, 255 };

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                state = GUI_STATE_PRESSED;
                pickerSelector = mousePoint;

                // Calculate color from picker
                Vector2 colorPick = { pickerSelector.x - bounds.x, pickerSelector.y - bounds.y };

                colorPick.x /= (float)bounds.width;     // Get normalized value on x
                colorPick.y /= (float)bounds.height;    // Get normalized value on y

                hsv.y = colorPick.x;
                hsv.z = 1.0f - colorPick.y;

                Vector3 rgb = ConvertHSVtoRGB(hsv);

                // NOTE: Vector3ToColor() only available on raylib 1.8.1
                color = RAYGUI_CLITERAL(Color){ (unsigned char)(255.0f*rgb.x),
                                 (unsigned char)(255.0f*rgb.y),
                                 (unsigned char)(255.0f*rgb.z),
                                 (unsigned char)(255.0f*(float)color.a/255.0f) };

            }
            else state = GUI_STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    if (state != GUI_STATE_DISABLED)
    {
        DrawRectangleGradientEx(bounds, Fade(colWhite, guiAlpha), Fade(colWhite, guiAlpha), Fade(maxHueCol, guiAlpha), Fade(maxHueCol, guiAlpha));
        DrawRectangleGradientEx(bounds, Fade(colBlack, 0), Fade(colBlack, guiAlpha), Fade(colBlack, guiAlpha), Fade(colBlack, 0));

        // Draw color picker: selector
        Rectangle selector = { pickerSelector.x - GuiGetStyle(COLORPICKER, COLOR_SELECTOR_SIZE)/2, pickerSelector.y - GuiGetStyle(COLORPICKER, COLOR_SELECTOR_SIZE)/2, GuiGetStyle(COLORPICKER, COLOR_SELECTOR_SIZE), GuiGetStyle(COLORPICKER, COLOR_SELECTOR_SIZE) };
        GuiDrawRectangle(selector, 0, BLANK, Fade(colWhite, guiAlpha), guiSkin != NULL ? &guiSkin->colorpanel.selector : NULL, state);
    }
    else
    {
        DrawRectangleGradientEx(bounds, Fade(Fade(GetColor(GuiGetStyle(COLORPICKER, BASE_COLOR_DISABLED)), 0.1f), guiAlpha), Fade(Fade(colBlack, 0.6f), guiAlpha), Fade(Fade(colBlack, 0.6f), guiAlpha), Fade(Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER_COLOR_DISABLED)), 0.6f), guiAlpha));
    }

    GuiDrawRectangle(bounds, 1, Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER + state*3)), guiAlpha), guiSkin != NULL ? Fade(GetColor(GuiGetStyle(COLORPICKER, BASE + state*3)), guiAlpha) : BLANK,  guiSkin != NULL ? &guiSkin->colorpanel.body : NULL, state);
    //--------------------------------------------------------------------

    return color;
}

Color GuiColorPanel(Rectangle bounds, Color color)
{
    return GuiColorPanelEx(bounds, color, -1.0f);
}

// Color Bar Alpha control
// NOTE: Returns alpha value normalized [0..1]
float GuiColorBarAlpha(Rectangle bounds, float alpha)
{
    #define COLORBARALPHA_CHECKED_SIZE   10

    GuiControlState state = guiState;
    Rectangle selector = { (float)bounds.x + alpha*bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (float)bounds.y - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (float)GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_HEIGHT), (float)bounds.height + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)*2 };

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds) ||
            CheckCollisionPointRec(mousePoint, selector))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                state = GUI_STATE_PRESSED;
                selector.x = mousePoint.x - selector.width/2;

                alpha = (mousePoint.x - bounds.x)/bounds.width;
                if (alpha <= 0.0f) alpha = 0.0f;
                if (alpha >= 1.0f) alpha = 1.0f;
                //selector.x = bounds.x + (int)(((alpha - 0)/(100 - 0))*(bounds.width - 2*GuiGetStyle(SLIDER, BORDER_WIDTH))) - selector.width/2;
            }
            else state = GUI_STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------

    // Draw alpha bar: checked background
    if (state != GUI_STATE_DISABLED)
    {
        int checksX = bounds.width/COLORBARALPHA_CHECKED_SIZE;
        int checksY = bounds.height/COLORBARALPHA_CHECKED_SIZE;

        for (int x = 0; x < checksX; x++)
        {
            for (int y = 0; y < checksY; y++)
            {
                Rectangle check = { bounds.x + x*COLORBARALPHA_CHECKED_SIZE, bounds.y + y*COLORBARALPHA_CHECKED_SIZE, COLORBARALPHA_CHECKED_SIZE, COLORBARALPHA_CHECKED_SIZE };
                GuiDrawRectangle(check, 0, BLANK, ((x + y)%2)? Fade(Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER_COLOR_DISABLED)), 0.4f), guiAlpha) : Fade(Fade(GetColor(GuiGetStyle(COLORPICKER, BASE_COLOR_DISABLED)), 0.4f), guiAlpha), NULL, state);
            }
        }

        DrawRectangleGradientEx(bounds, RAYGUI_CLITERAL(Color){ 255, 255, 255, 0 }, RAYGUI_CLITERAL(Color){ 255, 255, 255, 0 }, Fade(RAYGUI_CLITERAL(Color){ 0, 0, 0, 255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 0, 0, 0, 255 }, guiAlpha));
    }
    else DrawRectangleGradientEx(bounds, Fade(GetColor(GuiGetStyle(COLORPICKER, BASE_COLOR_DISABLED)), 0.1f), Fade(GetColor(GuiGetStyle(COLORPICKER, BASE_COLOR_DISABLED)), 0.1f), Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER_COLOR_DISABLED)), guiAlpha), Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER_COLOR_DISABLED)), guiAlpha));

    GuiDrawRectangle(bounds, 1, Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER + state*3)), guiAlpha), guiSkin != NULL ? Fade(GetColor(GuiGetStyle(COLORPICKER, BASE + state*3)), guiAlpha) : BLANK,  guiSkin != NULL ? &guiSkin->colorbaralpha.body : NULL, state);

    // Draw alpha bar: selector
    GuiDrawRectangle(selector, 0, BLANK, Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER + state*3)), guiAlpha), guiSkin != NULL ? &guiSkin->colorbaralpha.selector : NULL, state);
    //--------------------------------------------------------------------

    return alpha;
}

// Color Bar Hue control
// NOTE: Returns hue value normalized [0..1]
float GuiColorBarHue(Rectangle bounds, float hue)
{
    GuiControlState state = guiState;
    Rectangle selector = { (float)bounds.x - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (float)bounds.y + hue/360.0f*bounds.height - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (float)bounds.width + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)*2, (float)GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_HEIGHT) };

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        if (CheckCollisionPointRec(mousePoint, bounds) ||
            CheckCollisionPointRec(mousePoint, selector))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
            {
                state = GUI_STATE_PRESSED;
                selector.y = mousePoint.y - selector.height/2;

                hue = (mousePoint.y - bounds.y)*360/bounds.height;
                if (hue <= 0.0f) hue = 0.0f;
                if (hue >= 359.0f) hue = 359.0f;

            }
            else state = GUI_STATE_FOCUSED;

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
    if (state != GUI_STATE_DISABLED)
    {
        // Draw hue bar:color bars
        DrawRectangleGradientV(bounds.x + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.y + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (int)bounds.height/6, Fade(RAYGUI_CLITERAL(Color){ 255,0,0,255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 255,255,0,255 }, guiAlpha));
        DrawRectangleGradientV(bounds.x + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.y + (int)bounds.height/6 + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (int)bounds.height/6, Fade(RAYGUI_CLITERAL(Color){ 255,255,0,255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 0,255,0,255 }, guiAlpha));
        DrawRectangleGradientV(bounds.x + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.y + 2*((int)bounds.height/6) + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (int)bounds.height/6, Fade(RAYGUI_CLITERAL(Color){ 0,255,0,255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 0,255,255,255 }, guiAlpha));
        DrawRectangleGradientV(bounds.x + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.y + 3*((int)bounds.height/6) + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (int)bounds.height/6, Fade(RAYGUI_CLITERAL(Color){ 0,255,255,255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 0,0,255,255 }, guiAlpha));
        DrawRectangleGradientV(bounds.x + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.y + 4*((int)bounds.height/6) + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (int)bounds.height/6, Fade(RAYGUI_CLITERAL(Color){ 0,0,255,255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 255,0,255,255 }, guiAlpha));
        DrawRectangleGradientV(bounds.x + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.y + 5*((int)bounds.height/6) + GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW)/2, bounds.width - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), (int)bounds.height/6 - GuiGetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW), Fade(RAYGUI_CLITERAL(Color){ 255,0,255,255 }, guiAlpha), Fade(RAYGUI_CLITERAL(Color){ 255,0,0,255 }, guiAlpha));
    }
    else DrawRectangleGradientV(bounds.x, bounds.y, bounds.width, bounds.height, Fade(Fade(GetColor(GuiGetStyle(COLORPICKER, BASE_COLOR_DISABLED)), 0.1f), guiAlpha), Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER_COLOR_DISABLED)), guiAlpha));

    GuiDrawRectangle(bounds, 1, Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER + state*3)), guiAlpha),  guiSkin != NULL ? Fade(GetColor(GuiGetStyle(COLORPICKER, BASE + state*3)), guiAlpha) : BLANK,  guiSkin != NULL ? &guiSkin->colorbarhue.body : NULL, state);

    // Draw hue bar: selector
    GuiDrawRectangle(selector, 0, BLANK, Fade(GetColor(GuiGetStyle(COLORPICKER, BORDER + state*3)), guiAlpha), guiSkin != NULL ? &guiSkin->colorbarhue.selector : NULL, state);
    //--------------------------------------------------------------------

    return hue;
}

// TODO: Color GuiColorBarSat() [WHITE->color]
// TODO: Color GuiColorBarValue() [BLACK->color], HSV / HSL
// TODO: float GuiColorBarLuminance() [BLACK->WHITE]

// Color Picker control
// NOTE: It's divided in multiple controls:
//      Color GuiColorPanel(Rectangle bounds, Color color)
//      float GuiColorBarAlpha(Rectangle bounds, float alpha)
//      float GuiColorBarHue(Rectangle bounds, float value)
// NOTE: bounds define GuiColorPanel() size
Color GuiColorPicker(Rectangle bounds, Color color)
{
    color = GuiColorPanel(bounds, color);

    Rectangle boundsHue = { (float)bounds.x + bounds.width + GuiGetStyle(COLORPICKER, HUEBAR_PADDING), (float)bounds.y, (float)GuiGetStyle(COLORPICKER, HUEBAR_WIDTH), (float)bounds.height };
    //Rectangle boundsAlpha = { bounds.x, bounds.y + bounds.height + GuiGetStyle(COLORPICKER, BARS_PADDING), bounds.width, GuiGetStyle(COLORPICKER, BARS_THICK) };

    Vector3 hsv = ConvertRGBtoHSV(RAYGUI_CLITERAL(Vector3){ color.r/255.0f, color.g/255.0f, color.b/255.0f });
    hsv.x = GuiColorBarHue(boundsHue, hsv.x);
    //color.a = (unsigned char)(GuiColorBarAlpha(boundsAlpha, (float)color.a/255.0f)*255.0f);
    Vector3 rgb = ConvertHSVtoRGB(hsv);
    color = RAYGUI_CLITERAL(Color){ (unsigned char)roundf(rgb.x*255.0f), (unsigned char)roundf(rgb.y*255.0f), (unsigned char)roundf(rgb.z*255.0f), color.a };

    return color;
}

// Message Box control
int GuiMessageBox(Rectangle bounds, const char *title, const char *message, const char *buttons)
{
    #define MESSAGEBOX_BUTTON_HEIGHT    24
    #define MESSAGEBOX_BUTTON_PADDING   10

    int clicked = -1;    // Returns clicked button from buttons list, 0 refers to closed window button

    int buttonsCount = 0;
    const char **buttonsText = GuiTextSplit(buttons, &buttonsCount, NULL);
    Rectangle buttonBounds = { 0 };
    buttonBounds.x = bounds.x + MESSAGEBOX_BUTTON_PADDING;
    buttonBounds.y = bounds.y + bounds.height - MESSAGEBOX_BUTTON_HEIGHT - MESSAGEBOX_BUTTON_PADDING;
    buttonBounds.width = (bounds.width - MESSAGEBOX_BUTTON_PADDING*(buttonsCount + 1))/buttonsCount;
    buttonBounds.height = MESSAGEBOX_BUTTON_HEIGHT;

    Vector2 textSize = MeasureTextEx(guiFont, message, GuiGetStyle(DEFAULT, TEXT_SIZE), 1);

    Rectangle textBounds = { 0 };
    textBounds.x = bounds.x + bounds.width/2 - textSize.x/2;
    textBounds.y = bounds.y + WINDOW_STATUSBAR_HEIGHT + (bounds.height - WINDOW_STATUSBAR_HEIGHT - MESSAGEBOX_BUTTON_HEIGHT - MESSAGEBOX_BUTTON_PADDING)/2 - textSize.y/2;
    textBounds.width = textSize.x;
    textBounds.height = textSize.y;

    // Draw control
    //--------------------------------------------------------------------
    if (GuiWindowBox(bounds, title)) clicked = 0;

    int prevTextAlignment = GuiGetStyle(LABEL, TEXT_ALIGNMENT);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    GuiLabel(textBounds, message);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, prevTextAlignment);

    prevTextAlignment = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

    for (int i = 0; i < buttonsCount; i++)
    {
        if (GuiButton(buttonBounds, buttonsText[i])) clicked = i + 1;
        buttonBounds.x += (buttonBounds.width + MESSAGEBOX_BUTTON_PADDING);
    }

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, prevTextAlignment);
    //--------------------------------------------------------------------

    return clicked;
}

// Text Input Box control, ask for text
int GuiTextInputBox(Rectangle bounds, const char *title, const char *message, const char *buttons, char *text)
{
    #define TEXTINPUTBOX_BUTTON_HEIGHT      24
    #define TEXTINPUTBOX_BUTTON_PADDING     10
    #define TEXTINPUTBOX_HEIGHT             30

    #define TEXTINPUTBOX_MAX_TEXT_LENGTH   256

    // Used to enable text edit mode
    // WARNING: No more than one GuiTextInputBox() should be open at the same time
    static bool textEditMode = false;

    int btnIndex = -1;

    int buttonsCount = 0;
    const char **buttonsText = GuiTextSplit(buttons, &buttonsCount, NULL);
    Rectangle buttonBounds = { 0 };
    buttonBounds.x = bounds.x + TEXTINPUTBOX_BUTTON_PADDING;
    buttonBounds.y = bounds.y + bounds.height - TEXTINPUTBOX_BUTTON_HEIGHT - TEXTINPUTBOX_BUTTON_PADDING;
    buttonBounds.width = (bounds.width - TEXTINPUTBOX_BUTTON_PADDING*(buttonsCount + 1))/buttonsCount;
    buttonBounds.height = TEXTINPUTBOX_BUTTON_HEIGHT;

    int messageInputHeight = bounds.height - WINDOW_STATUSBAR_HEIGHT - GuiGetStyle(STATUSBAR, BORDER_WIDTH) - TEXTINPUTBOX_BUTTON_HEIGHT - 2*TEXTINPUTBOX_BUTTON_PADDING;

    Rectangle textBounds = { 0 };
    if (message != NULL)
    {
        Vector2 textSize = MeasureTextEx(guiFont, message, GuiGetStyle(DEFAULT, TEXT_SIZE), 1);

        textBounds.x = bounds.x + bounds.width/2 - textSize.x/2;
        textBounds.y = bounds.y + WINDOW_STATUSBAR_HEIGHT + messageInputHeight/4 - textSize.y/2;
        textBounds.width = textSize.x;
        textBounds.height = textSize.y;
    }

    Rectangle textBoxBounds = { 0 };
    textBoxBounds.x = bounds.x + TEXTINPUTBOX_BUTTON_PADDING;
    textBoxBounds.y = bounds.y + WINDOW_STATUSBAR_HEIGHT - TEXTINPUTBOX_HEIGHT/2;
    if (message == NULL) textBoxBounds.y += messageInputHeight/2;
    else textBoxBounds.y += (messageInputHeight/2 + messageInputHeight/4);
    textBoxBounds.width = bounds.width - TEXTINPUTBOX_BUTTON_PADDING*2;
    textBoxBounds.height = TEXTINPUTBOX_HEIGHT;

    // Draw control
    //--------------------------------------------------------------------
    if (GuiWindowBox(bounds, title)) btnIndex = 0;

    // Draw message if available
    if (message != NULL)
    {
        int prevTextAlignment = GuiGetStyle(LABEL, TEXT_ALIGNMENT);
        GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
        GuiLabel(textBounds, message);
        GuiSetStyle(LABEL, TEXT_ALIGNMENT, prevTextAlignment);
    }

    if (GuiTextBox(textBoxBounds, text, TEXTINPUTBOX_MAX_TEXT_LENGTH, textEditMode)) textEditMode = !textEditMode;

    int prevBtnTextAlignment = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);

    for (int i = 0; i < buttonsCount; i++)
    {
        if (GuiButton(buttonBounds, buttonsText[i])) btnIndex = i + 1;
        buttonBounds.x += (buttonBounds.width + MESSAGEBOX_BUTTON_PADDING);
    }

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, prevBtnTextAlignment);
    //--------------------------------------------------------------------

    return btnIndex;
}

// Grid control
// NOTE: Returns grid mouse-hover selected cell
// About drawing lines at subpixel spacing, simple put, not easy solution:
// https://stackoverflow.com/questions/4435450/2d-opengl-drawing-lines-that-dont-exactly-fit-pixel-raster
Vector2 GuiGrid(Rectangle bounds, float spacing, int subdivs)
{
    #if !defined(GRID_COLOR_ALPHA)
        #define GRID_COLOR_ALPHA    0.15f           // Grid lines alpha amount
    #endif

    GuiControlState state = guiState;
    Vector2 mousePoint = GetMousePosition();
    Vector2 currentCell = { -1, -1 };

    int linesV = ((int)(bounds.width/spacing))*subdivs + 1;
    int linesH = ((int)(bounds.height/spacing))*subdivs + 1;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            currentCell.x = (int)((mousePoint.x - bounds.x)/spacing);
            currentCell.y = (int)((mousePoint.y - bounds.y)/spacing);
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    switch (state)
    {
        case GUI_STATE_NORMAL:
        {
            if (subdivs > 0)
            {
                // Draw vertical grid lines
                for (int i = 0; i < linesV; i++)
                {
                    Rectangle lineV = { bounds.x + spacing * i / subdivs, bounds.y, 1, bounds.height };
                    GuiDrawRectangle(lineV, 0, BLANK, ((i%subdivs) == 0) ? Fade(GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)), GRID_COLOR_ALPHA * 4) : Fade(GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)), GRID_COLOR_ALPHA), NULL, state);
                }

                // Draw horizontal grid lines
                for (int i = 0; i < linesH; i++)
                {
                    Rectangle lineH = { bounds.x, bounds.y + spacing * i / subdivs, bounds.width, 1 };
                    GuiDrawRectangle(lineH, 0, BLANK, ((i%subdivs) == 0) ? Fade(GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)), GRID_COLOR_ALPHA * 4) : Fade(GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)), GRID_COLOR_ALPHA), NULL, state);
                }
            }
        } break;
        default: break;
    }

    return currentCell;
}

//----------------------------------------------------------------------------------
// Styles loading functions
//----------------------------------------------------------------------------------

// Load raygui style file (.rgs)
void GuiLoadStyle(const char *fileName)
{
    bool tryBinary = false;

    // Try reading the files as text file first
    FILE *rgsFile = fopen(fileName, "rt");

    if (rgsFile != NULL)
    {
        char buffer[256] = { 0 };
        fgets(buffer, 256, rgsFile);

        if (buffer[0] == '#')
        {
            int controlId = 0;
            int propertyId = 0;
            unsigned int propertyValue = 0;

            while (!feof(rgsFile))
            {
                switch (buffer[0])
                {
                    case 'p':
                    {
                        // Style property: p <control_id> <property_id> <property_value> <property_name>

                        sscanf(buffer, "p %d %d 0x%x", &controlId, &propertyId, &propertyValue);

                        GuiSetStyle(controlId, propertyId, (int)propertyValue);

                    } break;
                    case 'f':
                    {
                        // Style font: f <gen_font_size> <charmap_file> <font_file>

                        int fontSize = 0;
                        char charmapFileName[256] = { 0 };
                        char fontFileName[256] = { 0 };
                        sscanf(buffer, "f %d %s %[^\n]s", &fontSize, charmapFileName, fontFileName);

                        Font font = { 0 };

                        if (charmapFileName[0] != '0')
                        {
                            // Load characters from charmap file,
                            // expected '\n' separated list of integer values
                            char *charValues = LoadText(charmapFileName);
                            if (charValues != NULL)
                            {
                                int charsCount = 0;
                                const char **chars = TextSplit(charValues, '\n', &charsCount);

                                int *values = (int *)RAYGUI_MALLOC(charsCount*sizeof(int));
                                for (int i = 0; i < charsCount; i++) values[i] = TextToInteger(chars[i]);

                                font = LoadFontEx(TextFormat("%s/%s", GetDirectoryPath(fileName), fontFileName), fontSize, values, charsCount);

                                RAYGUI_FREE(values);
                            }
                        }
                        else font = LoadFontEx(TextFormat("%s/%s", GetDirectoryPath(fileName), fontFileName), fontSize, NULL, 0);

                        if ((font.texture.id > 0) && (font.charsCount > 0)) GuiSetFont(font);

                    } break;
                    default: break;
                }

                fgets(buffer, 256, rgsFile);
            }
        }
        else tryBinary = true;

        fclose(rgsFile);
    }

    if (tryBinary)
    {
        rgsFile = fopen(fileName, "rb");

        if (rgsFile == NULL) return;

        char signature[5] = "";
        short version = 0;
        short reserved = 0;
        int propertiesCount = 0;

        fread(signature, 1, 4, rgsFile);
        fread(&version, 1, sizeof(short), rgsFile);
        fread(&reserved, 1, sizeof(short), rgsFile);
        fread(&propertiesCount, 1, sizeof(int), rgsFile);

        if ((signature[0] == 'r') &&
            (signature[1] == 'G') &&
            (signature[2] == 'S') &&
            (signature[3] == ' '))
        {
            short controlId = 0;
            short propertyId = 0;
            int propertyValue = 0;

            for (int i = 0; i < propertiesCount; i++)
            {
                fread(&controlId, 1, sizeof(short), rgsFile);
                fread(&propertyId, 1, sizeof(short), rgsFile);
                fread(&propertyValue, 1, sizeof(int), rgsFile);

                if (controlId == 0) // DEFAULT control
                {
                    // If a DEFAULT property is loaded, it is propagated to all controls
                    // NOTE: All DEFAULT properties should be defined first in the file
                    GuiSetStyle(0, (int)propertyId, propertyValue);

                    if (propertyId < NUM_PROPS_DEFAULT) for (int i = 1; i < NUM_CONTROLS; i++) GuiSetStyle(i, (int)propertyId, propertyValue);
                }
                else GuiSetStyle((int)controlId, (int)propertyId, propertyValue);
            }

            // Font loading is highly dependant on raylib API to load font data and image
            // TODO: Find some mechanism to support it in standalone mode
#if !defined(RAYGUI_STANDALONE)
            // Load custom font if available
            int fontDataSize = 0;
            fread(&fontDataSize, 1, sizeof(int), rgsFile);

            if (fontDataSize > 0)
            {
                Font font = { 0 };
                int fontType = 0;   // 0-Normal, 1-SDF
                Rectangle whiteRec = { 0 };

                fread(&font.baseSize, 1, sizeof(int), rgsFile);
                fread(&font.charsCount, 1, sizeof(int), rgsFile);
                fread(&fontType, 1, sizeof(int), rgsFile);

                // Load font white rectangle
                fread(&whiteRec, 1, sizeof(Rectangle), rgsFile);

                // Load font image parameters
                int fontImageSize = 0;
                fread(&fontImageSize, 1, sizeof(int), rgsFile);

                if (fontImageSize > 0)
                {
                    Image imFont = { 0 };
                    imFont.mipmaps = 1;
                    fread(&imFont.width, 1, sizeof(int), rgsFile);
                    fread(&imFont.height, 1, sizeof(int), rgsFile);
                    fread(&imFont.format, 1, sizeof(int), rgsFile);

                    imFont.data = (unsigned char *)RAYGUI_MALLOC(fontImageSize);
                    fread(imFont.data, 1, fontImageSize, rgsFile);

                    font.texture = LoadTextureFromImage(imFont);

                    UnloadImage(imFont);
                }

                // Load font recs data
                font.recs = (Rectangle *)RAYGUI_CALLOC(font.charsCount, sizeof(Rectangle));
                for (int i = 0; i < font.charsCount; i++) fread(&font.recs[i], 1, sizeof(Rectangle), rgsFile);

                // Load font chars info data
                font.chars = (CharInfo *)RAYGUI_CALLOC(font.charsCount, sizeof(CharInfo));
                for (int i = 0; i < font.charsCount; i++)
                {
                    fread(&font.chars[i].value, 1, sizeof(int), rgsFile);
                    fread(&font.chars[i].offsetX, 1, sizeof(int), rgsFile);
                    fread(&font.chars[i].offsetY, 1, sizeof(int), rgsFile);
                    fread(&font.chars[i].advanceX, 1, sizeof(int), rgsFile);
                }

                GuiSetFont(font);

                // Set font texture source rectangle to be used as white texture to draw shapes
                // NOTE: This way, all gui can be draw using a single draw call
                if ((whiteRec.width != 0) && (whiteRec.height != 0)) SetShapesTexture(font.texture, whiteRec);
            }
#endif
        }

        fclose(rgsFile);
    }
}

// Load style default over global style
void GuiLoadStyleDefault(void)
{
    // We set this variable first to avoid cyclic function calls
    // when calling GuiSetStyle() and GuiGetStyle()
    guiStyleLoaded = true;

    // Initialize default LIGHT style property values
    GuiSetStyle(DEFAULT, BORDER_COLOR_NORMAL, 0x838383ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_NORMAL, 0xc9c9c9ff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_NORMAL, 0x686868ff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_FOCUSED, 0x5bb2d9ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_FOCUSED, 0xc9effeff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_FOCUSED, 0x6c9bbcff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_PRESSED, 0x0492c7ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_PRESSED, 0x97e8ffff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_PRESSED, 0x368bafff);
    GuiSetStyle(DEFAULT, BORDER_COLOR_DISABLED, 0xb5c1c2ff);
    GuiSetStyle(DEFAULT, BASE_COLOR_DISABLED, 0xe6e9e9ff);
    GuiSetStyle(DEFAULT, TEXT_COLOR_DISABLED, 0xaeb7b8ff);
    GuiSetStyle(DEFAULT, BORDER_WIDTH, 1);                       // WARNING: Some controls use other values
    GuiSetStyle(DEFAULT, TEXT_PADDING, 0);                       // WARNING: Some controls use other values
    GuiSetStyle(DEFAULT, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER); // WARNING: Some controls use other values

    // Initialize control-specific property values
    // NOTE: Those properties are in default list but require specific values by control type
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
    GuiSetStyle(BUTTON, BORDER_WIDTH, 2);
    GuiSetStyle(SLIDER, TEXT_PADDING, 5);
    GuiSetStyle(CHECKBOX, TEXT_PADDING, 5);
    GuiSetStyle(CHECKBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_RIGHT);
    GuiSetStyle(TEXTBOX, TEXT_PADDING, 5);
    GuiSetStyle(TEXTBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
    GuiSetStyle(VALUEBOX, TEXT_PADDING, 4);
    GuiSetStyle(VALUEBOX, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
    GuiSetStyle(SPINNER, TEXT_PADDING, 4);
    GuiSetStyle(SPINNER, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
    GuiSetStyle(STATUSBAR, TEXT_PADDING, 6);
    GuiSetStyle(STATUSBAR, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);

    // Initialize extended property values
    // NOTE: By default, extended property values are initialized to 0
    GuiSetStyle(DEFAULT, TEXT_SIZE, 10);                // DEFAULT, shared by all controls
    GuiSetStyle(DEFAULT, TEXT_SPACING, 1);              // DEFAULT, shared by all controls
    GuiSetStyle(DEFAULT, LINE_COLOR, 0x90abb5ff);       // DEFAULT specific property
    GuiSetStyle(DEFAULT, BACKGROUND_COLOR, 0xf5f5f5ff); // DEFAULT specific property
    GuiSetStyle(TOGGLE, GROUP_PADDING, 2);
    GuiSetStyle(SLIDER, SLIDER_WIDTH, 15);
    GuiSetStyle(SLIDER, SLIDER_PADDING, 1);
    GuiSetStyle(PROGRESSBAR, PROGRESS_PADDING, 1);
    GuiSetStyle(CHECKBOX, CHECK_PADDING, 1);
    GuiSetStyle(COMBOBOX, COMBO_BUTTON_WIDTH, 30);
    GuiSetStyle(COMBOBOX, COMBO_BUTTON_PADDING, 2);
    GuiSetStyle(DROPDOWNBOX, ARROW_PADDING, 16);
    GuiSetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING, 2);
    GuiSetStyle(TEXTBOX, TEXT_LINES_PADDING, 5);
    GuiSetStyle(TEXTBOX, TEXT_INNER_PADDING, 4);
    GuiSetStyle(TEXTBOX, COLOR_SELECTED_FG, 0xf0fffeff);
    GuiSetStyle(TEXTBOX, COLOR_SELECTED_BG, 0x839affe0);
    GuiSetStyle(SPINNER, SPIN_BUTTON_WIDTH, 20);
    GuiSetStyle(SPINNER, SPIN_BUTTON_PADDING, 2);
    GuiSetStyle(SCROLLBAR, BORDER_WIDTH, 0);
    GuiSetStyle(SCROLLBAR, ARROWS_VISIBLE, 0);
    GuiSetStyle(SCROLLBAR, ARROWS_SIZE, 6);
    GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING, 0);
    GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, 16);
    GuiSetStyle(SCROLLBAR, SCROLL_PADDING, 0);
    GuiSetStyle(SCROLLBAR, SCROLL_SPEED, 10);
    GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 0x1e);
    GuiSetStyle(LISTVIEW, LIST_ITEMS_PADDING, 2);
    GuiSetStyle(LISTVIEW, SCROLLBAR_WIDTH, 10);
    GuiSetStyle(LISTVIEW, SCROLLBAR_SIDE, SCROLLBAR_RIGHT_SIDE);
    GuiSetStyle(COLORPICKER, COLOR_SELECTOR_SIZE, 6);
    GuiSetStyle(COLORPICKER, HUEBAR_WIDTH, 0x14);
    GuiSetStyle(COLORPICKER, HUEBAR_PADDING, 0xa);
    GuiSetStyle(COLORPICKER, HUEBAR_SELECTOR_HEIGHT, 6);
    GuiSetStyle(COLORPICKER, HUEBAR_SELECTOR_OVERFLOW, 2);

    guiFont = GetFontDefault();     // Initialize default font
}


//----------------------------------------------------------------------------------
// Skin loading/saving functions
//----------------------------------------------------------------------------------
// Saves a skin style to file. If file exists it will be overwritten. Returns `0` on failure.
int GuiSaveSkin(const char *fileName, const char* texPath, GuiSkin* skin)
{
    GuiSkin* skins[1] = { skin };
    return GuiSaveSkinEx(fileName, texPath, skins, 1);
}

// Save multiple skin styles to file. If file exists it will be overwritten. Returns `0` on failure.
int GuiSaveSkinEx(const char *fileName, const char* texPath, GuiSkin** skins, int count)
{
    if(skins == NULL || fileName == NULL || texPath == NULL || count == 0) return 0;
    
    FILE* f = fopen(fileName, "wb");
    if(f == NULL) return 0;
    
    int id = 0;
 
    // Write help
    fprintf(f, "#\n# raygui skin file (v1.0)\n#\n# NOTE: each skin file can hold many skins styles differentiated by the <skin_id>\n#\n# Skin properties:\n"
    "#   t <texture_file>\n#   s <skin_id> <style_id> <substyle_id> <width> <height> <nx> <ny> <px> <py> <fx> <fy> <dx> <dy> <margins> <type> <mode>\n#\n\n");
    
    // Write texture
    fprintf(f, "t %s\n\n", texPath);
    
    for(int i=0; i<count; ++i) {
        GuiSkin* skin = skins[i];
        if(skin == NULL) continue;
        
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    BUTTON\n", id, GUI_SKIN_BUTTON_SID, 0, skin->button.width, skin->button.height, skin->button.nx, skin->button.ny, 
            skin->button.px, skin->button.py, skin->button.fx, skin->button.fy, skin->button.dx, skin->button.dy, skin->button.margins, skin->button.type, skin->button.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    IMGBUTTON\n", id, GUI_SKIN_IMGBUTTON_SID, 0, skin->imagebutton.width, skin->imagebutton.height, skin->imagebutton.nx, skin->imagebutton.ny, 
            skin->imagebutton.px, skin->imagebutton.py, skin->imagebutton.fx, skin->imagebutton.fy, skin->imagebutton.dx, skin->imagebutton.dy, skin->imagebutton.margins, skin->imagebutton.type, skin->imagebutton.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    PANEL\n", id, GUI_SKIN_PANEL_SID, 0, skin->panel.width, skin->panel.height, skin->panel.nx, skin->panel.ny, 
            skin->panel.px, skin->panel.py, skin->panel.fx, skin->panel.fy, skin->panel.dx, skin->panel.dy, skin->panel.margins, skin->panel.type, skin->panel.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    STATUSBAR\n", id, GUI_SKIN_STATUS_SID, 0, skin->status.width, skin->status.height, skin->status.nx, skin->status.ny, 
            skin->status.px, skin->status.py, skin->status.fx, skin->status.fy, skin->status.dx, skin->status.dy, skin->status.margins, skin->status.type, skin->status.mode);
        
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    WINDOW_PANEL\n", id, GUI_SKIN_WINDOW_SID, 0, skin->window.panel.width, skin->window.panel.height, skin->window.panel.nx, skin->window.panel.ny, 
            skin->window.panel.px, skin->window.panel.py, skin->window.panel.fx, skin->window.panel.fy, skin->window.panel.dx, skin->window.panel.dy, skin->window.panel.margins, skin->window.panel.type, skin->window.panel.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    WINDOW_TITLEBAR\n", id, GUI_SKIN_WINDOW_SID, 1, skin->window.titlebar.width, skin->window.titlebar.height, skin->window.titlebar.nx, skin->window.titlebar.ny, 
            skin->window.titlebar.px, skin->window.titlebar.py, skin->window.titlebar.fx, skin->window.titlebar.fy, skin->window.titlebar.dx, skin->window.titlebar.dy, skin->window.titlebar.margins, skin->window.titlebar.type, skin->window.titlebar.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    WINDOW_BUTTON\n", id, GUI_SKIN_WINDOW_SID, 2, skin->window.button.width, skin->window.button.height, skin->window.button.nx, skin->window.button.ny, 
            skin->window.button.px, skin->window.button.py, skin->window.button.fx, skin->window.button.fy, skin->window.button.dx, skin->window.button.dy, skin->window.button.margins, skin->window.button.type, skin->window.button.mode);
            
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SCROLLPANEL\n", id, GUI_SKIN_SCROLLPANEL_SID, 0, skin->scrollpanel.body.width, skin->scrollpanel.body.height, skin->scrollpanel.body.nx, skin->scrollpanel.body.ny, 
            skin->scrollpanel.body.px, skin->scrollpanel.body.py, skin->scrollpanel.body.fx, skin->scrollpanel.body.fy, skin->scrollpanel.body.dx, skin->scrollpanel.body.dy, skin->scrollpanel.body.margins, skin->scrollpanel.body.type, skin->scrollpanel.body.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SCROLLPANEL_CORNER\n", id, GUI_SKIN_SCROLLPANEL_SID, 1, skin->scrollpanel.corner.width, skin->scrollpanel.corner.height, skin->scrollpanel.corner.nx, skin->scrollpanel.corner.ny, 
            skin->scrollpanel.corner.px, skin->scrollpanel.corner.py, skin->scrollpanel.corner.fx, skin->scrollpanel.corner.fy, skin->scrollpanel.corner.dx, skin->scrollpanel.corner.dy, skin->scrollpanel.corner.margins, skin->scrollpanel.corner.type, skin->scrollpanel.corner.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SCROLLPANEL_HSCROLL_BODY\n", id, GUI_SKIN_SCROLLPANEL_SID, 2, skin->scrollpanel.hscroll.body.width, skin->scrollpanel.hscroll.body.height, skin->scrollpanel.hscroll.body.nx, skin->scrollpanel.hscroll.body.ny, 
            skin->scrollpanel.hscroll.body.px, skin->scrollpanel.hscroll.body.py, skin->scrollpanel.hscroll.body.fx, skin->scrollpanel.hscroll.body.fy, skin->scrollpanel.hscroll.body.dx, skin->scrollpanel.hscroll.body.dy, skin->scrollpanel.hscroll.body.margins, skin->scrollpanel.hscroll.body.type, skin->scrollpanel.hscroll.body.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SCROLLPANEL_HSCROLL_RAIL\n", id, GUI_SKIN_SCROLLPANEL_SID, 3, skin->scrollpanel.hscroll.rail.width, skin->scrollpanel.hscroll.rail.height, skin->scrollpanel.hscroll.rail.nx, skin->scrollpanel.hscroll.rail.ny, 
            skin->scrollpanel.hscroll.rail.px, skin->scrollpanel.hscroll.rail.py, skin->scrollpanel.hscroll.rail.fx, skin->scrollpanel.hscroll.rail.fy, skin->scrollpanel.hscroll.rail.dx, skin->scrollpanel.hscroll.rail.dy, skin->scrollpanel.hscroll.rail.margins, skin->scrollpanel.hscroll.rail.type, skin->scrollpanel.hscroll.rail.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SCROLLPANEL_HSCROLL_SLIDER\n", id, GUI_SKIN_SCROLLPANEL_SID, 4, skin->scrollpanel.hscroll.slider.width, skin->scrollpanel.hscroll.slider.height, skin->scrollpanel.hscroll.slider.nx, skin->scrollpanel.hscroll.slider.ny, 
            skin->scrollpanel.hscroll.slider.px, skin->scrollpanel.hscroll.slider.py, skin->scrollpanel.hscroll.slider.fx, skin->scrollpanel.hscroll.slider.fy, skin->scrollpanel.hscroll.slider.dx, skin->scrollpanel.hscroll.slider.dy, skin->scrollpanel.hscroll.slider.margins, skin->scrollpanel.hscroll.slider.type, skin->scrollpanel.hscroll.slider.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SCROLLPANEL_VSCROLL_BODY\n", id, GUI_SKIN_SCROLLPANEL_SID, 5, skin->scrollpanel.vscroll.body.width, skin->scrollpanel.vscroll.body.height, skin->scrollpanel.vscroll.body.nx, skin->scrollpanel.vscroll.body.ny, 
            skin->scrollpanel.vscroll.body.px, skin->scrollpanel.vscroll.body.py, skin->scrollpanel.vscroll.body.fx, skin->scrollpanel.vscroll.body.fy, skin->scrollpanel.vscroll.body.dx, skin->scrollpanel.vscroll.body.dy, skin->scrollpanel.vscroll.body.margins, skin->scrollpanel.vscroll.body.type, skin->scrollpanel.vscroll.body.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SCROLLPANEL_VSCROLL_RAIL\n", id, GUI_SKIN_SCROLLPANEL_SID, 6, skin->scrollpanel.vscroll.rail.width, skin->scrollpanel.vscroll.rail.height, skin->scrollpanel.vscroll.rail.nx, skin->scrollpanel.vscroll.rail.ny, 
            skin->scrollpanel.vscroll.rail.px, skin->scrollpanel.vscroll.rail.py, skin->scrollpanel.vscroll.rail.fx, skin->scrollpanel.vscroll.rail.fy, skin->scrollpanel.vscroll.rail.dx, skin->scrollpanel.vscroll.rail.dy, skin->scrollpanel.vscroll.rail.margins, skin->scrollpanel.vscroll.rail.type, skin->scrollpanel.vscroll.rail.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SCROLLPANEL_VSCROLL_SLIDER\n", id, GUI_SKIN_SCROLLPANEL_SID, 7, skin->scrollpanel.vscroll.slider.width, skin->scrollpanel.vscroll.slider.height, skin->scrollpanel.vscroll.slider.nx, skin->scrollpanel.vscroll.slider.ny, 
            skin->scrollpanel.vscroll.slider.px, skin->scrollpanel.vscroll.slider.py, skin->scrollpanel.vscroll.slider.fx, skin->scrollpanel.vscroll.slider.fy, skin->scrollpanel.vscroll.slider.dx, skin->scrollpanel.vscroll.slider.dy, skin->scrollpanel.vscroll.slider.margins, skin->scrollpanel.vscroll.slider.type, skin->scrollpanel.vscroll.slider.mode);

        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    LIST\n", id, GUI_SKIN_LIST_SID, 0, skin->list.body.width, skin->list.body.height, skin->list.body.nx, skin->list.body.ny, 
            skin->list.body.px, skin->list.body.py, skin->list.body.fx, skin->list.body.fy, skin->list.body.dx, skin->list.body.dy, skin->list.body.margins, skin->list.body.type, skin->list.body.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    LIST_ITEM\n", id, GUI_SKIN_LIST_SID, 1, skin->list.item.width, skin->list.item.height, skin->list.item.nx, skin->list.item.ny, 
            skin->list.item.px, skin->list.item.py, skin->list.item.fx, skin->list.item.fy, skin->list.item.dx, skin->list.item.dy, skin->list.item.margins, skin->list.item.type, skin->list.item.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    LIST_ITEM_FIRST\n", id, GUI_SKIN_LIST_SID, 2, skin->list.first.width, skin->list.first.height, skin->list.first.nx, skin->list.first.ny, 
            skin->list.first.px, skin->list.first.py, skin->list.first.fx, skin->list.first.fy, skin->list.first.dx, skin->list.first.dy, skin->list.first.margins, skin->list.first.type, skin->list.first.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    LIST_ITEM_LAST\n", id, GUI_SKIN_LIST_SID, 3, skin->list.last.width, skin->list.last.height, skin->list.last.nx, skin->list.last.ny, 
            skin->list.last.px, skin->list.last.py, skin->list.last.fx, skin->list.last.fy, skin->list.last.dx, skin->list.last.dy, skin->list.last.margins, skin->list.last.type, skin->list.last.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    LIST_SCROLLBAR_BODY\n", id, GUI_SKIN_LIST_SID, 4, skin->list.scrollbar.body.width, skin->list.scrollbar.body.height, skin->list.scrollbar.body.nx, skin->list.scrollbar.body.ny, 
            skin->list.scrollbar.body.px, skin->list.scrollbar.body.py, skin->list.scrollbar.body.fx, skin->list.scrollbar.body.fy, skin->list.scrollbar.body.dx, skin->list.scrollbar.body.dy, skin->list.scrollbar.body.margins, skin->list.scrollbar.body.type, skin->list.scrollbar.body.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    LIST_SCROLLBAR_RAIL\n", id, GUI_SKIN_LIST_SID, 5, skin->list.scrollbar.rail.width, skin->list.scrollbar.rail.height, skin->list.scrollbar.rail.nx, skin->list.scrollbar.rail.ny, 
            skin->list.scrollbar.rail.px, skin->list.scrollbar.rail.py, skin->list.scrollbar.rail.fx, skin->list.scrollbar.rail.fy, skin->list.scrollbar.rail.dx, skin->list.scrollbar.rail.dy, skin->list.scrollbar.rail.margins, skin->list.scrollbar.rail.type, skin->list.scrollbar.rail.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    LIST_SCROLLBAR_SLIDER\n", id, GUI_SKIN_LIST_SID, 6, skin->list.scrollbar.slider.width, skin->list.scrollbar.slider.height, skin->list.scrollbar.slider.nx, skin->list.scrollbar.slider.ny, 
            skin->list.scrollbar.slider.px, skin->list.scrollbar.slider.py, skin->list.scrollbar.slider.fx, skin->list.scrollbar.slider.fy, skin->list.scrollbar.slider.dx, skin->list.scrollbar.slider.dy, skin->list.scrollbar.slider.margins, skin->list.scrollbar.slider.type, skin->list.scrollbar.slider.mode);

        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SCROLLBAR\n", id, GUI_SKIN_SCROLLBAR_SID, 0, skin->scrollbar.body.width, skin->scrollbar.body.height, skin->scrollbar.body.nx, skin->scrollbar.body.ny, 
            skin->scrollbar.body.px, skin->scrollbar.body.py, skin->scrollbar.body.fx, skin->scrollbar.body.fy, skin->scrollbar.body.dx, skin->scrollbar.body.dy, skin->scrollbar.body.margins, skin->scrollbar.body.type, skin->scrollbar.body.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SCROLLBAR_RAIL\n", id, GUI_SKIN_SCROLLBAR_SID, 1, skin->scrollbar.rail.width, skin->scrollbar.rail.height, skin->scrollbar.rail.nx, skin->scrollbar.rail.ny, 
            skin->scrollbar.rail.px, skin->scrollbar.rail.py, skin->scrollbar.rail.fx, skin->scrollbar.rail.fy, skin->scrollbar.rail.dx, skin->scrollbar.rail.dy, skin->scrollbar.rail.margins, skin->scrollbar.rail.type, skin->scrollbar.rail.mode);
        fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SCROLLBAR_SLIDER\n", id, GUI_SKIN_SCROLLBAR_SID, 2, skin->scrollbar.slider.width, skin->scrollbar.slider.height, skin->scrollbar.slider.nx, skin->scrollbar.slider.ny, 
            skin->scrollbar.slider.px, skin->scrollbar.slider.py, skin->scrollbar.slider.fx, skin->scrollbar.slider.fy, skin->scrollbar.slider.dx, skin->scrollbar.slider.dy, skin->scrollbar.slider.margins, skin->scrollbar.slider.type, skin->scrollbar.slider.mode);

		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    TOGGLE\n", id, GUI_SKIN_TOGGLE_SID, 0, skin->toggle.off.width, skin->toggle.off.height, skin->toggle.off.nx, skin->toggle.off.ny, 
            skin->toggle.off.px, skin->toggle.off.py, skin->toggle.off.fx, skin->toggle.off.fy, skin->toggle.off.dx, skin->toggle.off.dy, skin->toggle.off.margins, skin->toggle.off.type, skin->toggle.off.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    TOGGLE_ON\n", id, GUI_SKIN_TOGGLE_SID, 1, skin->toggle.on.width, skin->toggle.on.height, skin->toggle.on.nx, skin->toggle.on.ny, 
            skin->toggle.on.px, skin->toggle.on.py, skin->toggle.on.fx, skin->toggle.on.fy, skin->toggle.on.dx, skin->toggle.on.dy, skin->toggle.on.margins, skin->toggle.on.type, skin->toggle.on.mode);

		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    TOGGLEGROUP\n", id, GUI_SKIN_TOGGLEGROUP_SID, 0, skin->togglegroup.item.off.width, skin->togglegroup.item.off.height, skin->togglegroup.item.off.nx, skin->togglegroup.item.off.ny, 
            skin->togglegroup.item.off.px, skin->togglegroup.item.off.py, skin->togglegroup.item.off.fx, skin->togglegroup.item.off.fy, skin->togglegroup.item.off.dx, skin->togglegroup.item.off.dy, skin->togglegroup.item.off.margins, skin->togglegroup.item.off.type, skin->togglegroup.item.off.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    TOGGLEGROUP_ON\n", id, GUI_SKIN_TOGGLEGROUP_SID, 1, skin->togglegroup.item.on.width, skin->togglegroup.item.on.height, skin->togglegroup.item.on.nx, skin->togglegroup.item.on.ny, 
            skin->togglegroup.item.on.px, skin->togglegroup.item.on.py, skin->togglegroup.item.on.fx, skin->togglegroup.item.on.fy, skin->togglegroup.item.on.dx, skin->togglegroup.item.on.dy, skin->togglegroup.item.on.margins, skin->togglegroup.item.on.type, skin->togglegroup.item.on.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    TOGGLEGROUP_FIRST\n", id, GUI_SKIN_TOGGLEGROUP_SID, 2, skin->togglegroup.first.off.width, skin->togglegroup.first.off.height, skin->togglegroup.first.off.nx, skin->togglegroup.first.off.ny, 
            skin->togglegroup.first.off.px, skin->togglegroup.first.off.py, skin->togglegroup.first.off.fx, skin->togglegroup.first.off.fy, skin->togglegroup.first.off.dx, skin->togglegroup.first.off.dy, skin->togglegroup.first.off.margins, skin->togglegroup.first.off.type, skin->togglegroup.first.off.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    TOGGLEGROUP_FIRST_ON\n", id, GUI_SKIN_TOGGLEGROUP_SID, 3, skin->togglegroup.first.on.width, skin->togglegroup.first.on.height, skin->togglegroup.first.on.nx, skin->togglegroup.first.on.ny, 
            skin->togglegroup.first.on.px, skin->togglegroup.first.on.py, skin->togglegroup.first.on.fx, skin->togglegroup.first.on.fy, skin->togglegroup.first.on.dx, skin->togglegroup.first.on.dy, skin->togglegroup.first.on.margins, skin->togglegroup.first.on.type, skin->togglegroup.first.on.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    TOGGLEGROUP_LAST\n", id, GUI_SKIN_TOGGLEGROUP_SID, 4, skin->togglegroup.last.off.width, skin->togglegroup.last.off.height, skin->togglegroup.last.off.nx, skin->togglegroup.last.off.ny, 
            skin->togglegroup.last.off.px, skin->togglegroup.last.off.py, skin->togglegroup.last.off.fx, skin->togglegroup.last.off.fy, skin->togglegroup.last.off.dx, skin->togglegroup.last.off.dy, skin->togglegroup.last.off.margins, skin->togglegroup.last.off.type, skin->togglegroup.last.off.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    TOGGLEGROUP_LAST_ON\n", id, GUI_SKIN_TOGGLEGROUP_SID, 5, skin->togglegroup.last.on.width, skin->togglegroup.last.on.height, skin->togglegroup.last.on.nx, skin->togglegroup.last.on.ny, 
            skin->togglegroup.last.on.px, skin->togglegroup.last.on.py, skin->togglegroup.last.on.fx, skin->togglegroup.last.on.fy, skin->togglegroup.last.on.dx, skin->togglegroup.last.on.dy, skin->togglegroup.last.on.margins, skin->togglegroup.last.on.type, skin->togglegroup.last.on.mode);

		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    CHECKBOX\n", id, GUI_SKIN_CHECKBOX_SID, 0, skin->checkbox.body.width, skin->checkbox.body.height, skin->checkbox.body.nx, skin->checkbox.body.ny, 
            skin->checkbox.body.px, skin->checkbox.body.py, skin->checkbox.body.fx, skin->checkbox.body.fy, skin->checkbox.body.dx, skin->checkbox.body.dy, skin->checkbox.body.margins, skin->checkbox.body.type, skin->checkbox.body.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    CHECKBOX_CHECKED\n", id, GUI_SKIN_CHECKBOX_SID, 1, skin->checkbox.checked.width, skin->checkbox.checked.height, skin->checkbox.checked.nx, skin->checkbox.checked.ny, 
            skin->checkbox.checked.px, skin->checkbox.checked.py, skin->checkbox.checked.fx, skin->checkbox.checked.fy, skin->checkbox.checked.dx, skin->checkbox.checked.dy, skin->checkbox.checked.margins, skin->checkbox.checked.type, skin->checkbox.checked.mode);

		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    COMBOBOX\n", id, GUI_SKIN_COMBOBOX_SID, 0, skin->combobox.body.width, skin->combobox.body.height, skin->combobox.body.nx, skin->combobox.body.ny, 
            skin->combobox.body.px, skin->combobox.body.py, skin->combobox.body.fx, skin->combobox.body.fy, skin->combobox.body.dx, skin->combobox.body.dy, skin->combobox.body.margins, skin->combobox.body.type, skin->combobox.body.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    COMBOBOX_BUTTON\n", id, GUI_SKIN_COMBOBOX_SID, 1, skin->combobox.button.width, skin->combobox.button.height, skin->combobox.button.nx, skin->combobox.button.ny, 
            skin->combobox.button.px, skin->combobox.button.py, skin->combobox.button.fx, skin->combobox.button.fy, skin->combobox.button.dx, skin->combobox.button.dy, skin->combobox.button.margins, skin->combobox.button.type, skin->combobox.button.mode);

		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    DROPBOX\n", id, GUI_SKIN_DROPBOX_SID, 0, skin->dropbox.body.width, skin->dropbox.body.height, skin->dropbox.body.nx, skin->dropbox.body.ny, 
            skin->dropbox.body.px, skin->dropbox.body.py, skin->dropbox.body.fx, skin->dropbox.body.fy, skin->dropbox.body.dx, skin->dropbox.body.dy, skin->dropbox.body.margins, skin->dropbox.body.type, skin->dropbox.body.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    DROPBOX_LIST\n", id, GUI_SKIN_DROPBOX_SID, 1, skin->dropbox.listpanel.width, skin->dropbox.listpanel.height, skin->dropbox.listpanel.nx, skin->dropbox.listpanel.ny, 
            skin->dropbox.listpanel.px, skin->dropbox.listpanel.py, skin->dropbox.listpanel.fx, skin->dropbox.listpanel.fy, skin->dropbox.listpanel.dx, skin->dropbox.listpanel.dy, skin->dropbox.listpanel.margins, skin->dropbox.listpanel.type, skin->dropbox.listpanel.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    DROPBOX_ITEM\n", id, GUI_SKIN_DROPBOX_SID, 2, skin->dropbox.item.width, skin->dropbox.item.height, skin->dropbox.item.nx, skin->dropbox.item.ny, 
            skin->dropbox.item.px, skin->dropbox.item.py, skin->dropbox.item.fx, skin->dropbox.item.fy, skin->dropbox.item.dx, skin->dropbox.item.dy, skin->dropbox.item.margins, skin->dropbox.item.type, skin->dropbox.item.mode);
            
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SPINNER_VALUEBOX\n", id, GUI_SKIN_SPINNER_SID, 0, skin->spinner.valuebox.width, skin->spinner.valuebox.height, skin->spinner.valuebox.nx, skin->spinner.valuebox.ny, 
            skin->spinner.valuebox.px, skin->spinner.valuebox.py, skin->spinner.valuebox.fx, skin->spinner.valuebox.fy, skin->spinner.valuebox.dx, skin->spinner.valuebox.dy, skin->spinner.valuebox.margins, skin->spinner.valuebox.type, skin->spinner.valuebox.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SPINNER_LEFT_BUTTON\n", id, GUI_SKIN_SPINNER_SID, 1, skin->spinner.left.width, skin->spinner.left.height, skin->spinner.left.nx, skin->spinner.left.ny, 
            skin->spinner.left.px, skin->spinner.left.py, skin->spinner.left.fx, skin->spinner.left.fy, skin->spinner.left.dx, skin->spinner.left.dy, skin->spinner.left.margins, skin->spinner.left.type, skin->spinner.left.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SPINNER_RIGHT_BUTTON\n", id, GUI_SKIN_SPINNER_SID, 2, skin->spinner.right.width, skin->spinner.right.height, skin->spinner.right.nx, skin->spinner.right.ny, 
            skin->spinner.right.px, skin->spinner.right.py, skin->spinner.right.fx, skin->spinner.right.fy, skin->spinner.right.dx, skin->spinner.right.dy, skin->spinner.right.margins, skin->spinner.right.type, skin->spinner.right.mode);

		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SLIDER\n", id, GUI_SKIN_SLIDER_SID, 0, skin->slider.body.width, skin->slider.body.height, skin->slider.body.nx, skin->slider.body.ny, 
            skin->slider.body.px, skin->slider.body.py, skin->slider.body.fx, skin->slider.body.fy, skin->slider.body.dx, skin->slider.body.dy, skin->slider.body.margins, skin->slider.body.type, skin->slider.body.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SLIDER_SLIDER\n", id, GUI_SKIN_SLIDER_SID, 1, skin->slider.slider.width, skin->slider.slider.height, skin->slider.slider.nx, skin->slider.slider.ny, 
            skin->slider.slider.px, skin->slider.slider.py, skin->slider.slider.fx, skin->slider.slider.fy, skin->slider.slider.dx, skin->slider.slider.dy, skin->slider.slider.margins, skin->slider.slider.type, skin->slider.slider.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    SLIDER_BAR\n", id, GUI_SKIN_SLIDER_SID, 2, skin->slider.bar.width, skin->slider.bar.height, skin->slider.bar.nx, skin->slider.bar.ny, 
            skin->slider.bar.px, skin->slider.bar.py, skin->slider.bar.fx, skin->slider.bar.fy, skin->slider.bar.dx, skin->slider.bar.dy, skin->slider.bar.margins, skin->slider.bar.type, skin->slider.bar.mode);

		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    PROGRESSBAR\n", id, GUI_SKIN_PROGRESSBAR_SID, 0, skin->progressbar.body.width, skin->progressbar.body.height, skin->progressbar.body.nx, skin->progressbar.body.ny, 
            skin->progressbar.body.px, skin->progressbar.body.py, skin->progressbar.body.fx, skin->progressbar.body.fy, skin->progressbar.body.dx, skin->progressbar.body.dy, skin->progressbar.body.margins, skin->progressbar.body.type, skin->progressbar.body.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    PROGRESSBAR_PROGRESS\n", id, GUI_SKIN_PROGRESSBAR_SID, 1, skin->progressbar.progress.width, skin->progressbar.progress.height, skin->progressbar.progress.nx, skin->progressbar.progress.ny, 
            skin->progressbar.progress.px, skin->progressbar.progress.py, skin->progressbar.progress.fx, skin->progressbar.progress.fy, skin->progressbar.progress.dx, skin->progressbar.progress.dy, skin->progressbar.progress.margins, skin->progressbar.progress.type, skin->progressbar.progress.mode);

		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    VALUEBOX\n", id, GUI_SKIN_VALUEBOX_SID, 0, skin->valuebox.width, skin->valuebox.height, skin->valuebox.nx, skin->valuebox.ny, 
            skin->valuebox.px, skin->valuebox.py, skin->valuebox.fx, skin->valuebox.fy, skin->valuebox.dx, skin->valuebox.dy, skin->valuebox.margins, skin->valuebox.type, skin->valuebox.mode);

		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    TEXTBOX\n", id, GUI_SKIN_TEXTBOX_SID, 0, skin->textbox.width, skin->textbox.height, skin->textbox.nx, skin->textbox.ny, 
            skin->textbox.px, skin->textbox.py, skin->textbox.fx, skin->textbox.fy, skin->textbox.dx, skin->textbox.dy, skin->textbox.margins, skin->textbox.type, skin->textbox.mode);
            
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    TEXTBOXMULTI\n", id, GUI_SKIN_TEXTBOXMULTI_SID, 0, skin->textboxmulti.width, skin->textboxmulti.height, skin->textboxmulti.nx, skin->textboxmulti.ny, 
            skin->textboxmulti.px, skin->textboxmulti.py, skin->textboxmulti.fx, skin->textboxmulti.fy, skin->textboxmulti.dx, skin->textboxmulti.dy, skin->textboxmulti.margins, skin->textboxmulti.type, skin->textboxmulti.mode);

		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    COLORPANEL\n", id, GUI_SKIN_COLORPANEL_SID, 0, skin->colorpanel.body.width, skin->colorpanel.body.height, skin->colorpanel.body.nx, skin->colorpanel.body.ny, 
            skin->colorpanel.body.px, skin->colorpanel.body.py, skin->colorpanel.body.fx, skin->colorpanel.body.fy, skin->colorpanel.body.dx, skin->colorpanel.body.dy, skin->colorpanel.body.margins, skin->colorpanel.body.type, skin->colorpanel.body.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    COLORPANEL_SELECTOR\n", id, GUI_SKIN_COLORPANEL_SID, 1, skin->colorpanel.selector.width, skin->colorpanel.selector.height, skin->colorpanel.selector.nx, skin->colorpanel.selector.ny, 
            skin->colorpanel.selector.px, skin->colorpanel.selector.py, skin->colorpanel.selector.fx, skin->colorpanel.selector.fy, skin->colorpanel.selector.dx, skin->colorpanel.selector.dy, skin->colorpanel.selector.margins, skin->colorpanel.selector.type, skin->colorpanel.selector.mode);
            
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    COLORBARALPHA\n", id, GUI_SKIN_COLORBARALPHA_SID, 0, skin->colorbaralpha.body.width, skin->colorbaralpha.body.height, skin->colorbaralpha.body.nx, skin->colorbaralpha.body.ny, 
            skin->colorbaralpha.body.px, skin->colorbaralpha.body.py, skin->colorbaralpha.body.fx, skin->colorbaralpha.body.fy, skin->colorbaralpha.body.dx, skin->colorbaralpha.body.dy, skin->colorbaralpha.body.margins, skin->colorbaralpha.body.type, skin->colorbaralpha.body.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    COLORBARALPHA_SELECTOR\n", id, GUI_SKIN_COLORBARALPHA_SID, 1, skin->colorbaralpha.selector.width, skin->colorbaralpha.selector.height, skin->colorbaralpha.selector.nx, skin->colorbaralpha.selector.ny, 
            skin->colorbaralpha.selector.px, skin->colorbaralpha.selector.py, skin->colorbaralpha.selector.fx, skin->colorbaralpha.selector.fy, skin->colorbaralpha.selector.dx, skin->colorbaralpha.selector.dy, skin->colorbaralpha.selector.margins, skin->colorbaralpha.selector.type, skin->colorbaralpha.selector.mode);
           
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    COLORBARHUE\n", id, GUI_SKIN_COLORBARHUE_SID, 0, skin->colorbarhue.body.width, skin->colorbarhue.body.height, skin->colorbarhue.body.nx, skin->colorbarhue.body.ny, 
            skin->colorbarhue.body.px, skin->colorbarhue.body.py, skin->colorbarhue.body.fx, skin->colorbarhue.body.fy, skin->colorbarhue.body.dx, skin->colorbarhue.body.dy, skin->colorbarhue.body.margins, skin->colorbarhue.body.type, skin->colorbarhue.body.mode);
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    COLORBARHUE_SELECTOR\n", id, GUI_SKIN_COLORBARHUE_SID, 1, skin->colorbarhue.selector.width, skin->colorbarhue.selector.height, skin->colorbarhue.selector.nx, skin->colorbarhue.selector.ny, 
            skin->colorbarhue.selector.px, skin->colorbarhue.selector.py, skin->colorbarhue.selector.fx, skin->colorbarhue.selector.fy, skin->colorbarhue.selector.dx, skin->colorbarhue.selector.dy, skin->colorbarhue.selector.margins, skin->colorbarhue.selector.type, skin->colorbarhue.selector.mode);
           
		fprintf(f, "s  %02i %02i %02i %4i %4i %4i %4i %4i %4i %4i %4i %4i %4i %03i %02i %02i    TOOLTIP\n", id, GUI_SKIN_TOOLTIP_SID, 0, skin->tooltip.width, skin->tooltip.height, skin->tooltip.nx, skin->tooltip.ny, 
            skin->tooltip.px, skin->tooltip.py, skin->tooltip.fx, skin->tooltip.fy, skin->tooltip.dx, skin->tooltip.dy, skin->tooltip.margins, skin->tooltip.type, skin->tooltip.mode);

        ++id;
    }
    
    fclose(f);
    
    return id;
}


// Load skin style with specified `id` from a skin file. Returns `0` on failure.
int GuiLoadSkin(const char *fileName, GuiSkin* skin, int id) 
{
    FILE* f = fopen(fileName, "rb");
    if(f == NULL) return 0;
    
    char buffer[256] = { 0 };
    fgets(buffer, 256, f);
    if(buffer[0] != '#') { fclose(f); return 0; }
    
    int r=0;
    
    char textureName[256] = {0};
    while (!feof(f))
    {
        if(buffer[0] == 's')
        {
            int skin_id = 0, style_id = -1, substyle_id = -1;
            GuiSkinStyle style = {0};
            
            //s <skin_id> <style_id> <substyle_id> <width> <height> <nx> <ny> <px> <py> <fx> <fy> <dx> <dy> <margins> <type> <mode>
            sscanf(buffer, "s %02i %02i %02i %4hu %4hu %4hu %4hu %4hu %4hu %4hu %4hu %4hu %4hu %03hu %02hhu %02hhu", &skin_id, &style_id, &substyle_id, &style.width, &style.height, &style.nx, &style.ny, 
                &style.px, &style.py, &style.fx, &style.fy, &style.dx, &style.dy, &style.margins, &style.type, &style.mode);
            
            if( skin_id == id)
            {
                r = 1;
                switch(style_id) {
                    case GUI_SKIN_BUTTON_SID: skin->button = style; break;
                    case GUI_SKIN_IMGBUTTON_SID: skin->imagebutton = style; break;
                    case GUI_SKIN_STATUS_SID: skin->status = style; break;
                    case GUI_SKIN_PANEL_SID: skin->panel = style; break;
                    
                    case GUI_SKIN_WINDOW_SID:
                        if(substyle_id == 0) skin->window.panel = style;
                        else if(substyle_id == 1) skin->window.titlebar = style;
                        else if(substyle_id == 2) skin->window.button = style;
                    break;
                    
                    case GUI_SKIN_SCROLLPANEL_SID: 
                        if(substyle_id >= 0 && substyle_id <= 7)  {
                            // NOTE: this might not be faster than using a switch but it's way shorter ;)
                            GuiSkinStyle* arr[8] = {&skin->scrollpanel.body, &skin->scrollpanel.corner, &skin->scrollpanel.hscroll.body, &skin->scrollpanel.hscroll.rail, &skin->scrollpanel.hscroll.slider, 
                                &skin->scrollpanel.vscroll.body, &skin->scrollpanel.vscroll.rail, &skin->scrollpanel.vscroll.slider};
                            *arr[substyle_id] = style;
                        }
                    break;
                    
                    case GUI_SKIN_LIST_SID:
                        if(substyle_id >= 0 && substyle_id <= 6) {
                            // NOTE: this might not be faster than using a switch but it's way shorter ;)
                            GuiSkinStyle* arr[7] = {&skin->list.body, &skin->list.item, &skin->list.first, &skin->list.last, &skin->list.scrollbar.body, &skin->list.scrollbar.rail, 
                                &skin->list.scrollbar.slider};
                            *arr[substyle_id] = style;
                        }
                    break;
                    
                    case GUI_SKIN_SCROLLBAR_SID:
                        if(substyle_id == 0) skin->scrollbar.body = style;
                        else if(substyle_id == 1) skin->scrollbar.rail = style;
                        else if(substyle_id == 2) skin->scrollbar.slider = style;
                    break;
                    
                    case GUI_SKIN_TOGGLE_SID:
                        if(substyle_id == 0) skin->toggle.off = style;
                        else if(substyle_id == 1) skin->toggle.on = style;
                    break;
                    
                    case GUI_SKIN_TOGGLEGROUP_SID:
                        if(substyle_id >= 0 && substyle_id <= 5) {
                            // NOTE: this might not be faster than using a switch but it's way shorter ;)
                            GuiSkinStyle* arr[6] = {&skin->togglegroup.item.off, &skin->togglegroup.item.on, &skin->togglegroup.first.off, &skin->togglegroup.first.on, 
                            &skin->togglegroup.last.off, &skin->togglegroup.last.on};
                            *arr[substyle_id] = style;
                        }
                    break;
                    
                    case GUI_SKIN_CHECKBOX_SID:
                        if(substyle_id == 0) skin->checkbox.body = style;
                        else if(substyle_id == 1) skin->checkbox.checked = style;
                    break;
                    
                    case GUI_SKIN_COMBOBOX_SID:
                        if(substyle_id == 0) skin->combobox.body = style;
                        else if(substyle_id == 1) skin->combobox.button = style;
                    break;
                    
                    case GUI_SKIN_DROPBOX_SID:
                        if(substyle_id == 0) skin->dropbox.body = style;
                        else if(substyle_id == 1) skin->dropbox.listpanel = style;
                        else if(substyle_id == 2) skin->dropbox.item = style;
                    break;
                    
                    case GUI_SKIN_SPINNER_SID:
                        if(substyle_id == 0) skin->spinner.valuebox = style;
                        else if(substyle_id == 1) skin->spinner.left= style;
                        else if(substyle_id == 2) skin->spinner.right = style;
                    break;
                    
                    case GUI_SKIN_SLIDER_SID:
                        if(substyle_id == 0) skin->slider.body = style;
                        else if(substyle_id == 1) skin->slider.slider= style;
                        else if(substyle_id == 2) skin->slider.bar = style;
                    break;
 
                    case GUI_SKIN_PROGRESSBAR_SID:
                        if(substyle_id == 0) skin->progressbar.body = style;
                        else if(substyle_id == 1) skin->progressbar.progress = style;
                    break;
                    
                    case GUI_SKIN_VALUEBOX_SID: skin->valuebox = style; break;
                    case GUI_SKIN_TEXTBOX_SID: skin->textbox = style; break;
                    case GUI_SKIN_TEXTBOXMULTI_SID: skin->textboxmulti = style; break;
                    
                    
                    case GUI_SKIN_COLORPANEL_SID:
                        if(substyle_id == 0) skin->colorpanel.body = style;
                        else if(substyle_id == 1) skin->colorpanel.selector = style;
                    break;
                    
                    case GUI_SKIN_COLORBARALPHA_SID:
                        if(substyle_id == 0) skin->colorbaralpha.body = style;
                        else if(substyle_id == 1) skin->colorbaralpha.selector = style;
                    break;
                    
                    case GUI_SKIN_COLORBARHUE_SID:
                        if(substyle_id == 0) skin->colorbarhue.body = style;
                        else if(substyle_id == 1) skin->colorbarhue.selector = style;
                    break;
                    
                    case GUI_SKIN_TOOLTIP_SID: skin->tooltip = style; break;
                }
            }
        }
        else if(buffer[0] == 't') 
        {
            // Load texture only if the skin doesn't have any set
            if(skin->tex.id <= 0) 
            {
                sscanf(buffer, "t %[^\n]s", textureName);
                skin->tex = LoadTexture(TextFormat("%s/%s", GetDirectoryPath(fileName), textureName));
                if(skin->tex.id <= 0 ) {fclose(f); return 0;} // Couldn't load texture;
            }
        }
        fgets(buffer, 256, f);
    }
    
    fclose(f);
    return r;
}


// Get text with icon id prepended
// NOTE: Useful to add icons by name id (enum) instead of
// a number that can change between ricon versions
const char *GuiIconText(int iconId, const char *text)
{
#if defined(RAYGUI_SUPPORT_ICONS)
    static char buffer[1024] = { 0 };
    memset(buffer, 0, 1024);

    sprintf(buffer, "#%03i#", iconId);

    if (text != NULL)
    {
        for (int i = 5; i < 1024; i++)
        {
            buffer[i] = text[i - 5];
            if (text[i - 5] == '\0') break;
        }
    }

    return buffer;
#else
    return NULL;
#endif
}

#if defined(RAYGUI_SUPPORT_ICONS)

// Get full icons data pointer
unsigned int *GuiGetIcons(void) { return guiIcons; }

// Load raygui icons file (.rgi)
// NOTE: In case nameIds are required, they can be requested with loadIconsName,
// they are returned as a guiIconsName[iconsCount][RICON_MAX_NAME_LENGTH],
// guiIconsName[]][] memory should be manually freed!
char **GuiLoadIcons(const char *fileName, bool loadIconsName)
{
    // Style File Structure (.rgi)
    // ------------------------------------------------------
    // Offset  | Size    | Type       | Description
    // ------------------------------------------------------
    // 0       | 4       | char       | Signature: "rGI "
    // 4       | 2       | short      | Version: 100
    // 6       | 2       | short      | reserved

    // 8       | 2       | short      | Num icons (N)
    // 10      | 2       | short      | Icons size (Options: 16, 32, 64) (S)

    // Icons name id (32 bytes per name id)
    // foreach (icon)
    // {
    //   12+32*i  | 32   | char       | Icon NameId
    // }

    // Icons data: One bit per pixel, stored as unsigned int array (depends on icon size)
    // S*S pixels/32bit per unsigned int = K unsigned int per icon
    // foreach (icon)
    // {
    //   ...   | K       | unsigned int | Icon Data
    // }

    FILE *rgiFile = fopen(fileName, "rb");

    char **guiIconsName = NULL;

    if (rgiFile != NULL)
    {
        char signature[5] = "";
        short version = 0;
        short reserved = 0;
        short iconsCount = 0;
        short iconsSize = 0;

        fread(signature, 1, 4, rgiFile);
        fread(&version, 1, sizeof(short), rgiFile);
        fread(&reserved, 1, sizeof(short), rgiFile);
        fread(&iconsCount, 1, sizeof(short), rgiFile);
        fread(&iconsSize, 1, sizeof(short), rgiFile);

        if ((signature[0] == 'r') &&
            (signature[1] == 'G') &&
            (signature[2] == 'I') &&
            (signature[3] == ' '))
        {
            if (loadIconsName)
            {
                guiIconsName = (char **)RAYGUI_MALLOC(iconsCount*sizeof(char **));
                for (int i = 0; i < iconsCount; i++)
                {
                    guiIconsName[i] = (char *)RAYGUI_MALLOC(RICON_MAX_NAME_LENGTH);
                    fread(guiIconsName[i], 32, 1, rgiFile);
                }
            }

            // Read icons data directly over guiIcons data array
            fread(guiIcons, iconsCount*(iconsSize*iconsSize/32), sizeof(unsigned int), rgiFile);
        }

        fclose(rgiFile);
    }

    return guiIconsName;
}

// Draw selected icon using rectangles pixel-by-pixel
void GuiDrawIcon(int iconId, Vector2 position, int pixelSize, Color color)
{
    #define BIT_CHECK(a,b) ((a) & (1<<(b)))

    for (int i = 0, y = 0; i < RICON_SIZE*RICON_SIZE/32; i++)
    {
        for (int k = 0; k < 32; k++)
        {
            if (BIT_CHECK(guiIcons[iconId*RICON_DATA_ELEMENTS + i], k))
            {
            #if !defined(RAYGUI_STANDALONE)
                DrawRectangle(position.x + (k%RICON_SIZE)*pixelSize, position.y + y*pixelSize, pixelSize, pixelSize, color);
            #endif
            }

            if ((k == 15) || (k == 31)) y++;
        }
    }
}

// Get icon bit data
// NOTE: Bit data array grouped as unsigned int (ICON_SIZE*ICON_SIZE/32 elements)
unsigned int *GuiGetIconData(int iconId)
{
    static unsigned int iconData[RICON_DATA_ELEMENTS] = { 0 };
    memset(iconData, 0, RICON_DATA_ELEMENTS*sizeof(unsigned int));

    if (iconId < RICON_MAX_ICONS) memcpy(iconData, &guiIcons[iconId*RICON_DATA_ELEMENTS], RICON_DATA_ELEMENTS*sizeof(unsigned int));

    return iconData;
}

// Set icon bit data
// NOTE: Data must be provided as unsigned int array (ICON_SIZE*ICON_SIZE/32 elements)
void GuiSetIconData(int iconId, unsigned int *data)
{
    if (iconId < RICON_MAX_ICONS) memcpy(&guiIcons[iconId*RICON_DATA_ELEMENTS], data, RICON_DATA_ELEMENTS*sizeof(unsigned int));
}

// Set icon pixel value
void GuiSetIconPixel(int iconId, int x, int y)
{
    #define BIT_SET(a,b)   ((a) |= (1<<(b)))

    // This logic works for any RICON_SIZE pixels icons,
    // For example, in case of 16x16 pixels, every 2 lines fit in one unsigned int data element
    BIT_SET(guiIcons[iconId*RICON_DATA_ELEMENTS + y/(sizeof(unsigned int)*8/RICON_SIZE)], x + (y%(sizeof(unsigned int)*8/RICON_SIZE)*RICON_SIZE));
}

// Clear icon pixel value
void GuiClearIconPixel(int iconId, int x, int y)
{
    #define BIT_CLEAR(a,b) ((a) &= ~((1)<<(b)))

    // This logic works for any RICON_SIZE pixels icons,
    // For example, in case of 16x16 pixels, every 2 lines fit in one unsigned int data element
    BIT_CLEAR(guiIcons[iconId*RICON_DATA_ELEMENTS + y/(sizeof(unsigned int)*8/RICON_SIZE)], x + (y%(sizeof(unsigned int)*8/RICON_SIZE)*RICON_SIZE));
}

// Check icon pixel value
bool GuiCheckIconPixel(int iconId, int x, int y)
{
    #define BIT_CHECK(a,b) ((a) & (1<<(b)))

    return (BIT_CHECK(guiIcons[iconId*8 + y/2], x + (y%2*16)));
}
#endif      // RAYGUI_SUPPORT_ICONS

//----------------------------------------------------------------------------------
// Module specific Functions Definition
//----------------------------------------------------------------------------------
// Gui get text width using default font
static int GetTextWidth(const char *text)
{
    Vector2 size = { 0 };

    if ((text != NULL) && (text[0] != '\0')) size = MeasureTextEx(guiFont, text, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING));

    // TODO: Consider text icon width here???

    return (int)size.x;
}

// Get text bounds considering control bounds
static Rectangle GetTextBounds(int control, Rectangle bounds)
{
    Rectangle textBounds = bounds;

    textBounds.x = bounds.x + GuiGetStyle(control, BORDER_WIDTH);
    textBounds.y = bounds.y + GuiGetStyle(control, BORDER_WIDTH);
    textBounds.width = bounds.width - 2*GuiGetStyle(control, BORDER_WIDTH);
    textBounds.height = bounds.height - 2*GuiGetStyle(control, BORDER_WIDTH);

    // Consider TEXT_PADDING properly, depends on control type and TEXT_ALIGNMENT
    switch (control)
    {
        case COMBOBOX: bounds.width -= (GuiGetStyle(control, COMBO_BUTTON_WIDTH) + GuiGetStyle(control, COMBO_BUTTON_PADDING)); break;
        case VALUEBOX: break;   // NOTE: ValueBox text value always centered, text padding applies to label
        default:
        {
            if (GuiGetStyle(control, TEXT_ALIGNMENT) == GUI_TEXT_ALIGN_RIGHT) textBounds.x -= GuiGetStyle(control, TEXT_PADDING);
            else textBounds.x += GuiGetStyle(control, TEXT_PADDING);
        } break;
    }

    // TODO: Special cases (no label): COMBOBOX, DROPDOWNBOX, LISTVIEW (scrollbar?)
    // More special cases (label side): CHECKBOX, SLIDER, VALUEBOX, SPINNER

    return textBounds;
}

// Get text icon if provided and move text cursor
// NOTE: We support up to 999 values for iconId
static const char *GetTextIcon(const char *text, int *iconId)
{
#if defined(RAYGUI_SUPPORT_ICONS)
    *iconId = -1;
    if (text[0] == '#')     // Maybe we have an icon!
    {
        char iconValue[4] = { 0 };  // Maximum length for icon value: 3 digits + '\0'

        int pos = 1;
        while ((pos < 4) && (text[pos] >= '0') && (text[pos] <= '9'))
        {
            iconValue[pos - 1] = text[pos];
            pos++;
        }

        if (text[pos] == '#')
        {
            *iconId = TextToInteger(iconValue);

            // Move text pointer after icon
            // WARNING: If only icon provided, it could point to EOL character!
            if (*iconId >= 0) text += (pos + 1);
        }
    }
#endif

    return text;
}

// Gui draw text using default font
static void GuiDrawText(const char *text, Rectangle bounds, int alignment, Color tint)
{
    #define TEXT_VALIGN_PIXEL_OFFSET(h)  ((int)h%2)     // Vertical alignment for pixel perfect

    if ((text != NULL) && (text[0] != '\0'))
    {
        int iconId = 0;
        text = GetTextIcon(text, &iconId);  // Check text for icon and move cursor

        // Get text position depending on alignment and iconId
        //---------------------------------------------------------------------------------
        #define ICON_TEXT_PADDING   4

        Vector2 position = { bounds.x, bounds.y };

        // NOTE: We get text size after icon been processed
        int textWidth = GetTextWidth(text);
        int textHeight = GuiGetStyle(DEFAULT, TEXT_SIZE);

#if defined(RAYGUI_SUPPORT_ICONS)
        if (iconId >= 0)
        {
            textWidth += RICON_SIZE;

            // WARNING: If only icon provided, text could be pointing to eof character!
            if ((text != NULL) && (text[0] != '\0')) textWidth += ICON_TEXT_PADDING;
        }
#endif
        // Check guiTextAlign global variables
        switch (alignment)
        {
            case GUI_TEXT_ALIGN_LEFT:
            {
                position.x = bounds.x;
                position.y = bounds.y + bounds.height/2 - textHeight/2 + TEXT_VALIGN_PIXEL_OFFSET(bounds.height);
            } break;
            case GUI_TEXT_ALIGN_CENTER:
            {
                position.x = bounds.x + bounds.width/2 - textWidth/2;
                position.y = bounds.y + bounds.height/2 - textHeight/2 + TEXT_VALIGN_PIXEL_OFFSET(bounds.height);
            } break;
            case GUI_TEXT_ALIGN_RIGHT:
            {
                position.x = bounds.x + bounds.width - textWidth;
                position.y = bounds.y + bounds.height/2 - textHeight/2 + TEXT_VALIGN_PIXEL_OFFSET(bounds.height);
            } break;
            default: break;
        }

        // NOTE: Make sure we get pixel-perfect coordinates,
        // In case of decimals we got weird text positioning
        position.x = (float)((int)position.x);
        position.y = (float)((int)position.y);
        //---------------------------------------------------------------------------------

        // Draw text (with icon if available)
        //---------------------------------------------------------------------------------
#if defined(RAYGUI_SUPPORT_ICONS)
        if (iconId >= 0)
        {
            // NOTE: We consider icon height, probably different than text size
            GuiDrawIcon(iconId, RAYGUI_CLITERAL(Vector2){ position.x, bounds.y + bounds.height/2 - RICON_SIZE/2 + TEXT_VALIGN_PIXEL_OFFSET(bounds.height) }, 1, tint);
            position.x += (RICON_SIZE + ICON_TEXT_PADDING);
        }
#endif
        DrawTextEx(guiFont, text, position, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING), tint);
        //---------------------------------------------------------------------------------
    }
}

static void GuiDrawRectangle(Rectangle rec, int borderWidth, Color borderColor, Color color, GuiSkinStyle* style, int state)
{
    if(style != NULL && guiSkin->tex.id > 0 && style->width != 0 && style->height != 0)
    {
        // Use color overlay?
        const Color c = guiSkin->useColor ? color : Fade(WHITE, guiAlpha);
        
        // Calculate the source rectangle inside the texture based on state
        Rectangle srcRec = RAYGUI_CLITERAL(Rectangle){ style->nx, style->ny, style->width, style->height };
        if(state == GUI_STATE_FOCUSED) {srcRec.x = style->fx; srcRec.y = style->fy;}
        else if(state == GUI_STATE_PRESSED) {srcRec.x = style->px; srcRec.y = style->py;}
        else if(state == GUI_STATE_DISABLED) {srcRec.x = style->dx; srcRec.y = style->dy;}
        
        if(style->margins != 0)
        {
            // Inflate the bounds of the control when there are margins set
            rec.x -= style->margins;
            rec.y -= style->margins;
            rec.width += 2*style->margins;
            rec.height += 2*style->margins;
        }
        
        // Constant origin vector set here since it's used by every draw function
        const Vector2 origin = RAYGUI_CLITERAL(Vector2){0.0f,0.0f};
        
        if(style->mode == GUI_SKIN_DRAW_STRECHED) 
        {
            // DRAW STRETCHED
            if(style->type == GUI_SKIN_TYPE_SLICE9)  // Draw 9 slices
            {
                NPatchInfo npi = {srcRec,style->left,style->top,style->right,style->bottom,NPT_9PATCH};
                DrawTextureNPatch(guiSkin->tex, npi, rec, origin, 0.0f, c);
            }
            else // Draw single slice
            {
                DrawTexturePro(guiSkin->tex, srcRec, rec, origin, 0.0f, c);
            }
        }
        else if(style->mode == GUI_SKIN_DRAW_TILED)
        {
            // DRAW TILED
            if(style->type == GUI_SKIN_TYPE_SLICE9)  // Draw 9 slices
            {
                // NOTE: https://en.wikipedia.org/wiki/9-slice_scaling
                
                // WARN: `DrawTextureTiled()` performance is very bad especially with small tiles
                Rectangle topLeft = RAYGUI_CLITERAL(Rectangle){srcRec.x, srcRec.y, style->left, style->top};
                if(topLeft.width > 0 && topLeft.height > 0) DrawTextureRec(guiSkin->tex, topLeft, RAYGUI_CLITERAL(Vector2){rec.x, rec.y}, c);
                
                Rectangle topRight = RAYGUI_CLITERAL(Rectangle) {srcRec.x + srcRec.width - style->right, srcRec.y, style->right, style->top};
                if(topRight.width > 0 && topRight.height > 0) DrawTextureRec(guiSkin->tex, topRight, RAYGUI_CLITERAL(Vector2){rec.x + rec.width - style->right, rec.y}, c);
                
                Rectangle topMiddle = RAYGUI_CLITERAL(Rectangle){srcRec.x + style->left, srcRec.y, srcRec.width - style->left - style->right, style->top};
                if(topMiddle.width > 0 && topMiddle.height > 0) DrawTextureTiled(guiSkin->tex, topMiddle, RAYGUI_CLITERAL(Rectangle){rec.x+style->left, rec.y, rec.width - style->left - style->right, style->top}, origin, 0.0f, 1.0f, c);
                
                Rectangle bottomLeft = RAYGUI_CLITERAL(Rectangle){srcRec.x, srcRec.y + srcRec.height - style->bottom, style->left, style->bottom };
                if(bottomLeft.width > 0 && bottomLeft.height > 0) DrawTextureRec(guiSkin->tex, bottomLeft, RAYGUI_CLITERAL(Vector2){rec.x, rec.y + rec.height - style->bottom}, c);
                
                Rectangle bottomRight = RAYGUI_CLITERAL(Rectangle){srcRec.x + srcRec.width - style->right, srcRec.y + srcRec.height - style->bottom, style->right, style->bottom };
                if(bottomRight.width > 0 && bottomRight.height > 0) DrawTextureRec(guiSkin->tex, bottomRight, RAYGUI_CLITERAL(Vector2){rec.x + rec.width - style->right, rec.y + rec.height - style->bottom}, c);
                
                Rectangle bottomMiddle = RAYGUI_CLITERAL(Rectangle){srcRec.x + style->left, srcRec.y + srcRec.height - style->bottom, srcRec.width - style->left - style->right, style->bottom };
                if(bottomMiddle.width > 0 && bottomMiddle.height > 0) DrawTextureTiled(guiSkin->tex, bottomMiddle, RAYGUI_CLITERAL(Rectangle){rec.x + style->left, rec.y + rec.height - style->bottom, rec.width - style->left - style->right, style->bottom}, origin, 0.0f, 1.0f, c);
                
                Rectangle centerLeft = RAYGUI_CLITERAL(Rectangle){srcRec.x, srcRec.y + style->top, style->left, srcRec.height - style->bottom - style->top };
                if(centerLeft.width > 0 && centerLeft.height > 0) DrawTextureTiled(guiSkin->tex, centerLeft, RAYGUI_CLITERAL(Rectangle){rec.x, rec.y + style->top, style->left, rec.height - style->bottom - style->top}, origin, 0.0f, 1.0f, c);
                
                Rectangle centerRight = RAYGUI_CLITERAL(Rectangle){srcRec.x + srcRec.width - style->right, srcRec.y + style->top, style->right, srcRec.height - style->top - style->bottom};
                if(centerRight.width > 0 && centerRight.height > 0) DrawTextureTiled(guiSkin->tex, centerRight, RAYGUI_CLITERAL(Rectangle){rec.x + rec.width - style->right, rec.y + style->top, style->right, rec.height - style->top - style->bottom}, origin, 0.0f, 1.0f, c);
                
                Rectangle centerMiddle = RAYGUI_CLITERAL(Rectangle){srcRec.x + style->left, srcRec.y + style->top, srcRec.width - style->right - style->left, srcRec.height - style->top - style->bottom};
                if(centerMiddle.width > 0 && centerMiddle.height > 0) DrawTextureTiled(guiSkin->tex, centerMiddle, RAYGUI_CLITERAL(Rectangle){rec.x + style->left, rec.y + style->top, rec.width - style->right - style->left, rec.height - style->top - style->bottom}, origin, 0.0f, 1.0f, c);
            }
            else // Draw single slice
            {
                DrawTextureTiled(guiSkin->tex, srcRec, rec, origin, 0.0f, 1.0f, c);
            }
        }
        else 
        {
            // DRAW CENTERED (only available for single slice)
            if(style->type == GUI_SKIN_TYPE_SLICE1) {
                Rectangle destRec = rec;
                int dw = (rec.width - srcRec.width)/2;
                if(dw > 0) { destRec.x += dw; destRec.width = srcRec.width; }
                int dh = (rec.height - srcRec.height)/2;
                if(dh > 0) { destRec.y += dh; destRec.height = srcRec.height; }
                
                DrawTexturePro(guiSkin->tex, srcRec, destRec, origin, 0.0f, c);
            }
        }
    }
    else 
    {
        if (color.a > 0) 
        {
            // Draw rectangle filled with color
            DrawRectangle(rec.x, rec.y, rec.width, rec.height, color);
        }
        
        if (borderWidth > 0) 
        {
            // Draw rectangle border lines with color
            DrawRectangle(rec.x, rec.y, rec.width, borderWidth, borderColor);
            DrawRectangle(rec.x, rec.y + borderWidth, borderWidth, rec.height - 2*borderWidth, borderColor);
            DrawRectangle(rec.x + rec.width - borderWidth, rec.y + borderWidth, borderWidth, rec.height - 2*borderWidth, borderColor);
            DrawRectangle(rec.x, rec.y + rec.height - borderWidth, rec.width, borderWidth, borderColor);
        }
    }
}

// Draw tooltip relatively to bounds
static void GuiDrawTooltip(Rectangle bounds)
{
    //static int tooltipFramesCounter = 0;  // Not possible gets reseted at second function call!

    if (guiTooltipEnabled && (guiTooltip != NULL) && CheckCollisionPointRec(GetMousePosition(), bounds))
    {
        Vector2 mousePosition = GetMousePosition();
        Vector2 textSize = MeasureTextEx(guiFont, guiTooltip, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING));
        Rectangle tooltipBounds = { mousePosition.x, mousePosition.y, textSize.x + 20, textSize.y*2 };

        GuiDrawRectangle(tooltipBounds, 1, Fade(GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)), guiAlpha), Fade(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), guiAlpha), guiSkin != NULL ? &guiSkin->tooltip : NULL, GUI_STATE_NORMAL);

        tooltipBounds.x += 10;
        GuiLabel(tooltipBounds, guiTooltip);
    }
}

// Split controls text into multiple strings
// Also check for multiple columns (required by GuiToggleGroup())
static const char **GuiTextSplit(const char *text, int *count, int *textRow)
{
    // NOTE: Current implementation returns a copy of the provided string with '\0' (string end delimiter)
    // inserted between strings defined by "delimiter" parameter. No memory is dynamically allocated,
    // all used memory is static... it has some limitations:
    //      1. Maximum number of possible split strings is set by TEXTSPLIT_MAX_TEXT_ELEMENTS
    //      2. Maximum size of text to split is TEXTSPLIT_MAX_TEXT_LENGTH
    // NOTE: Those definitions could be externally provided if required

    #if !defined(TEXTSPLIT_MAX_TEXT_LENGTH)
        #define TEXTSPLIT_MAX_TEXT_LENGTH      1024
    #endif

    #if !defined(TEXTSPLIT_MAX_TEXT_ELEMENTS)
        #define TEXTSPLIT_MAX_TEXT_ELEMENTS     128
    #endif

    static const char *result[TEXTSPLIT_MAX_TEXT_ELEMENTS] = { NULL };
    static char buffer[TEXTSPLIT_MAX_TEXT_LENGTH] = { 0 };
    memset(buffer, 0, TEXTSPLIT_MAX_TEXT_LENGTH);

    result[0] = buffer;
    int counter = 1;

    if (textRow != NULL) textRow[0] = 0;

    // Count how many substrings we have on text and point to every one
    for (int i = 0; i < TEXTSPLIT_MAX_TEXT_LENGTH; i++)
    {
        buffer[i] = text[i];
        if (buffer[i] == '\0') break;
        else if ((buffer[i] == ';') || (buffer[i] == '\n'))
        {
            result[counter] = buffer + i + 1;

            if (textRow != NULL)
            {
                if (buffer[i] == '\n') textRow[counter] = textRow[counter - 1] + 1;
                else textRow[counter] = textRow[counter - 1];
            }

            buffer[i] = '\0';   // Set an end of string at this point

            counter++;
            if (counter == TEXTSPLIT_MAX_TEXT_ELEMENTS) break;
        }
    }

    *count = counter;

    return result;
}

// Convert color data from RGB to HSV
// NOTE: Color data should be passed normalized
static Vector3 ConvertRGBtoHSV(Vector3 rgb)
{
    Vector3 hsv = { 0 };
    float min = 0.0f;
    float max = 0.0f;
    float delta = 0.0f;

    min = (rgb.x < rgb.y)? rgb.x : rgb.y;
    min = (min < rgb.z)? min  : rgb.z;

    max = (rgb.x > rgb.y)? rgb.x : rgb.y;
    max = (max > rgb.z)? max  : rgb.z;

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
        hsv.x = 0.0f;        // Undefined, maybe NAN?
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
    Vector3 rgb = { 0 };
    float hh = 0.0f, p = 0.0f, q = 0.0f, t = 0.0f, ff = 0.0f;
    long i = 0;

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
static Color Fade(Color color, float alpha)
{
    if (alpha < 0.0f) alpha = 0.0f;
    else if (alpha > 1.0f) alpha = 1.0f;
    
    Color result = { color.r, color.g, color.b, (unsigned char)(255.0f*alpha) };

    return result;
}

// Formatting of text with variables to 'embed'
static const char *TextFormat(const char *text, ...)
{
    #define MAX_FORMATTEXT_LENGTH   64

    static char buffer[MAX_FORMATTEXT_LENGTH];

    va_list args;
    va_start(args, text);
    vsprintf(buffer, text, args);
    va_end(args);

    return buffer;
}

// Draw rectangle with vertical gradient fill color
// NOTE: This function is only used by GuiColorPicker()
static void DrawRectangleGradientV(int posX, int posY, int width, int height, Color color1, Color color2)
{
    Rectangle bounds = { (float)posX, (float)posY, (float)width, (float)height };
    DrawRectangleGradientEx(bounds, color1, color2, color2, color1);
}

#define TEXTSPLIT_MAX_TEXT_BUFFER_LENGTH    1024        // Size of static buffer: TextSplit()
#define TEXTSPLIT_MAX_SUBSTRINGS_COUNT       128        // Size of static pointers array: TextSplit()


// Split string into multiple strings
const char **TextSplit(const char *text, char delimiter, int *count)
{
    // NOTE: Current implementation returns a copy of the provided string with '\0' (string end delimiter)
    // inserted between strings defined by "delimiter" parameter. No memory is dynamically allocated,
    // all used memory is static... it has some limitations:
    //      1. Maximum number of possible split strings is set by TEXTSPLIT_MAX_SUBSTRINGS_COUNT
    //      2. Maximum size of text to split is TEXTSPLIT_MAX_TEXT_BUFFER_LENGTH

    static const char *result[TEXTSPLIT_MAX_SUBSTRINGS_COUNT] = { NULL };
    static char buffer[TEXTSPLIT_MAX_TEXT_BUFFER_LENGTH] = { 0 };
    memset(buffer, 0, TEXTSPLIT_MAX_TEXT_BUFFER_LENGTH);

    result[0] = buffer;
    int counter = 0;

    if (text != NULL)
    {
        counter = 1;

        // Count how many substrings we have on text and point to every one
        for (int i = 0; i < TEXTSPLIT_MAX_TEXT_BUFFER_LENGTH; i++)
        {
            buffer[i] = text[i];
            if (buffer[i] == '\0') break;
            else if (buffer[i] == delimiter)
            {
                buffer[i] = '\0';   // Set an end of string at this point
                result[counter] = buffer + i + 1;
                counter++;

                if (counter == TEXTSPLIT_MAX_SUBSTRINGS_COUNT) break;
            }
        }
    }

    *count = counter;
    return result;
}

// Get integer value from text
// NOTE: This function replaces atoi() [stdlib.h]
static int TextToInteger(const char *text)
{
    int value = 0;
    int sign = 1;

    if ((text[0] == '+') || (text[0] == '-'))
    {
        if (text[0] == '-') sign = -1;
        text++;
    }

    for (int i = 0; ((text[i] >= '0') && (text[i] <= '9')); ++i) value = value*10 + (int)(text[i] - '0');

    return value*sign;
}
#endif      // RAYGUI_STANDALONE

#endif      // RAYGUI_IMPLEMENTATION
