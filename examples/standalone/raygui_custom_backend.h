/*******************************************************************************************
*
*   raygui - Standalone mode custom backend
*
*   Just edit this file to include your custom implementation to your graphic API
*
*   LICENSE: <your_license>
*
*   Copyright (c) <year> <developer_name>
*
**********************************************************************************************/

//#include "my_cool_graphic_api.h"

//----------------------------------------------------------------------------------
// Defines and Macros
// TODO: Define input keys required by raygui
//----------------------------------------------------------------------------------
#define KEY_RIGHT           262
#define KEY_LEFT            263
#define KEY_DOWN            264
#define KEY_UP              265
#define KEY_BACKSPACE       259
#define KEY_ENTER           257
#define MOUSE_LEFT_BUTTON     0

//----------------------------------------------------------------------------------
// Types and Structures Definition
// TODO: Define required structures, maybe Font/Texture2D should be defined here?
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Module Functions Definition
// TODO: Define all raygui required functions (previously provided by raylib)
//----------------------------------------------------------------------------------

//-------------------------------------------------------------------------------
// Input required functions
//-------------------------------------------------------------------------------
static Vector2 GetMousePosition(void)
{
    Vector2 position = { 0 };
    
    // TODO: Mouse position
    
    return position;
}

static int GetMouseWheelMove(void)
{
    // TODO: Mouse wheel movement variation, reseted every frame
    
    return 0;
}

static bool IsMouseButtonDown(int button)
{
    // TODO: Return true while mouse button [0..2] is being down
    
    return false;
}

static bool IsMouseButtonPressed(int button)
{
    // TODO: Return true when mouse button [0..2] has been pressed: up->down
    
    return false;
}

static bool IsMouseButtonReleased(int button)
{
    // TODO: Return true when mouse button [0..2] has been released: down->up
    
    return false;
}

static bool IsKeyDown(int key)
{
    // TODO: Return true while key is being down
    
    return false;
}

static bool IsKeyPressed(int key)
{
    // TODO: Return true when key has been pressed: up->down
    
    return false;
}

// USED IN: GuiTextBox(), GuiValueBox()
static int GetKeyPressed(void)
{
    // TODO: Return last key pressed (up->down) in the frame
    
    return 0;
}

//-------------------------------------------------------------------------------
// Drawing required functions
//-------------------------------------------------------------------------------
static void DrawRectangle(int x, int y, int width, int height, Color color)
{ 
    // TODO: Draw rectangle on the screen
}

// USED IN: GuiColorPicker()
static void DrawRectangleGradientEx(Rectangle rec, Color col1, Color col2, Color col3, Color col4)
{
    // TODO: Draw rectangle with gradients (4 vertex colors) on the screen
}

// USED IN: GuiDropdownBox(), GuiScrollBar()
static void DrawTriangle(Vector2 v1, Vector2 v2, Vector2 v3, Color color)
{ 
    // TODO: Draw triangle on the screen, required for arrows
}

//-------------------------------------------------------------------------------
// Text required functions
//-------------------------------------------------------------------------------
// USED IN: GuiLoadStyleDefault()
static Font GetFontDefault(void)
{
    Font font = { 0 };
    
    // TODO: Return default rendering Font for the UI
    
    return font; 
}

// USED IN: GetTextWidth()
static Vector2 MeasureTextEx(Font font, const char *text, float fontSize, float spacing) 
{ 
    Vector2 size = { 0 };
    
    // TODO: Return text size (width, height) on screen depending on the Font, text, fontSize and spacing
    
    return size;
}

// USED IN: GuiDrawText()
static void DrawTextEx(Font font, const char *text, Vector2 position, float fontSize, float spacing, Color tint)
{
    // TODO: Draw text on the screen
}

//-------------------------------------------------------------------------------
// GuiLoadStyle() required functions
//-------------------------------------------------------------------------------
static Font LoadFontEx(const char *fileName, int fontSize, int *fontChars, int glyphCount)
{
    Font font = { 0 };
    
    // TODO: Load a new font from a file
    
    return font; 
}

static char *LoadText(const char *fileName)
{
    // TODO: Load text file data, used by GuiLoadStyle() to load characters list required on Font generation,
    // this is a .rgs feature, probably this function is not required in most cases

    return NULL;
}

static const char *GetDirectoryPath(const char *filePath)
{
    // TODO: Get directory path for .rgs file, required to look for a possible .ttf/.otf font file referenced,
    // this is a .rgs feature, probably this function is not required in most cases
    
    return NULL;
}

