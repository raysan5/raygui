/*******************************************************************************************
*
*   PropertyListControl v1.0.1 - A custom control that displays a set of properties as a list
*
*   UPDATES: last updated - 10 march 2020 (v1.0.1)
*       v1.0.1 - Made it work with latest raygui version
*              - Added `GuiDMSaveProperties()` for saving properties to a text file
*              - A `GuiDMLoadProperties()` is planed but not implemented yet
*              - Added a section property that can work as a way to group multiple properties
*              - Fixed issue with section not having the correct height
*       v1.0.0 - Initial release
*
*
*   MODULE USAGE:
*       #define GUI_PROPERTY_LIST_IMPLEMENTATION
*       #include "dm_property_list.h"
*
*       INIT: GuiDMProperty props[] = { // initialize a set of properties first
                   PCOLOR(),
                   PINT(),
                   PFLOAT(),
                   ...
              };
*       DRAW: GuiDMPropertyList(bounds, props, sizeof(props)/sizeof(props[0]), ...);
*   
*       
*   NOTE: This module also contains 2 extra controls used internally by the property list
*       - GuiDMValueBox() - a value box that supports displaying float values
*       - GuiDMSpinner() - a `better` GuiSpinner()
*       
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2020 Vlad Adrian (@Demizdor - https://github.com/Demizdor).
*
**********************************************************************************************/

#include "raylib.h"

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "../../src/raygui.h"


#ifndef GUI_PROPERTY_LIST_H
#define GUI_PROPERTY_LIST_H

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------

// A bunch of usefull macros for modifying the flags of each property 

// Set flag `F` of property `P`. `P` must be a pointer!
#define PROP_SET_FLAG(P, F) ((P)->flags |= (F)) 
// Clear flag `F` of property `P`. `P` must be a pointer!
#define PROP_CLEAR_FLAG(P, F) ((P)->flags &= ~(F)) 
// Toggles flag `F` of property `P`. `P` must be a pointer!
#define PROP_TOGGLE_FLAG(P, F) ((P)->flags ^= (F)) 
// Checks if flag `F` of property `P` is set . `P` must be a pointer!
#define PROP_CHECK_FLAG(P, F) ((P)->flags & (F))

// Some usefull macros for creating properties

// Create a bool property with name `N`, flags `F` and value `V`
#define PBOOL(N, F, V) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_BOOL, F, .value.vbool = V}
// Create a int property with name `N`, flags `F` and value `V`
#define PINT(N, F, V) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_INT, F, .value.vint = {V,0,0,1}}
// Create a ranged int property within `MIN` and `MAX` with name `N`, flags `F` value `V`. 
// Pressing the spinner buttons will increase/decrease the value by `S`.
#define PINT_RANGE(N, F, V, S, MIN, MAX) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_INT, F, .value.vint = {V,MIN,MAX,S}}
// Create a float property with name `N`, flags `F` and value `V`
#define PFLOAT(N, F, V) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_FLOAT, F, .value.vfloat = {V,0.f,0.f,1.0f,3}}
// Create a ranged float property within `MIN` and `MAX` with name `N`, flags `F` value `V` with `P` decimal digits to show. 
// Pressing the spinner buttons will increase/decrease the value by `S`.
#define PFLOAT_RANGE(N, F, V, S, P, MIN, MAX) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_FLOAT, F, .value.vfloat = {V,MIN,MAX,S,P}}
// Create a text property with name `N`, flags `F` value `V` and max text size `S`
#define PTEXT(N, F, V, S) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_TEXT, F, .value.vtext = {V, S} }
// Create a text property with name `N`, flags `F` value `V` and max text size `S`
#define PSELECT(N, F, V, A) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_SELECT, F, .value.vselect = {V, A} }
// Create a 2D vector property with name `N`, flags `F` and the `X`, `Y` coordinates
#define PVEC2(N, F, X, Y) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_VECTOR2, F, .value.v2 = {X, Y} }
// Create a 3D vector property with name `N`, flags `F` and the `X`, `Y`, `Z` coordinates
#define PVEC3(N, F, X, Y, Z) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_VECTOR3, F, .value.v3 = {X, Y, Z} }
// Create a 4D vector property with name `N`, flags `F` and the `X`, `Y`, `Z`, `W` coordinates
#define PVEC4(N, F, X, Y, Z, W) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_VECTOR4, F, .value.v4 = {X, Y, Z, W} }
// Create a rectangle property with name `N`, flags `F`, `X`, `Y` coordinates and `W` and `H` size
#define PRECT(N, F, X, Y, W, H) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_RECT, F, .value.vrect = {X, Y, W, H} }
// Create a 3D vector property with name `N`, flags `F` and the `R`, `G`, `B`, `A` channel values
#define PCOLOR(N, F, R, G, B, A) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_COLOR, F, .value.vcolor = {R, G, B, A} }
// Create a collapsable section named `N` with `F` flags and the next `C` properties as children.
// !! A section cannot hold another section as a child !!
#define PSECTION(N, F, C) RAYGUI_CLITERAL(GuiDMProperty){N, GUI_PROP_SECTION, F, .value.vsection = (C)}

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
enum GuiDMPropertyTypes {
    GUI_PROP_BOOL = 0,
    GUI_PROP_INT,
    GUI_PROP_FLOAT,
    GUI_PROP_TEXT,
    GUI_PROP_SELECT,
    GUI_PROP_VECTOR2,
    GUI_PROP_VECTOR3,
    GUI_PROP_VECTOR4,
    GUI_PROP_RECT,
    GUI_PROP_COLOR,
    GUI_PROP_SECTION,
};

enum GuiDMPropertyFlags {
    GUI_PFLAG_COLLAPSED = 1 << 0, // is the property expanded or collapsed?
    GUI_PFLAG_DISABLED = 1 << 1, // is this property disabled or enabled?
};

// Data structure for each property
typedef struct {
    char* name;
    short type;
    short flags;
    union {
        bool vbool;
        struct { int val; int min; int max; int step; } vint;
        struct { float val; float min; float max; float step; int precision; } vfloat;
        struct { char* val; int size; } vtext;
        struct { char* val; int active; } vselect;
        int vsection;
        Vector2 v2;
        Vector3 v3;
        Vector4 v4;
        Rectangle vrect;
        Color vcolor;
    } value;
} GuiDMProperty;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

// A more advanced `GuiValueBox()` supporting float/int values with specified `precision`, cursor movements, cut/copy/paste and
// other keybord shortcuts. Needed by `GuiDMSpinner()` !!
// `precision` should be between 1-7 for float values and 0 for int values (maybe 15 for doubles but that was not tested)
// WARNING: The bounds should be set big enough else the text will overflow and things will break
// WARNING: Sometimes the last decimal value could differ, this is probably due to rounding
double GuiDMValueBox(Rectangle bounds, double value, double minValue, double maxValue, int precision, bool editMode);

// A more advanced `GuiSpinner()` using `GuiDMValueBox()` for displaying the values.
// This was needed because `GuiSpinner()` can't display float values and editing values is somewhat hard. 
// This is by no means perfect but should be more user friendly than the default control provided by raygui.
double GuiDMSpinner(Rectangle bounds, double value, double minValue, double maxValue, double step, int precision, bool editMode);

// Works just like `GuiListViewEx()` but with an array of properties instead of text.
void GuiDMPropertyList(Rectangle bounds, GuiDMProperty* props, int count, int* focus, int* scrollIndex);

// Handy function to save properties to a file. Returns false on failure or true otherwise.
bool GuiDMSaveProperties(const char* file, GuiDMProperty* props, int count);

#ifdef __cplusplus
}
#endif

#endif // GUI_PROPERTY_LIST_H



/***********************************************************************************
*
*   GUI_PROPERTY_LIST_IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_PROPERTY_LIST_IMPLEMENTATION)

#include "../../src/raygui.h"

#include <stdlib.h> // for calloc()
#include <string.h> // for memmove(), strlen()
#include <stdio.h> // for sscanf(), snprintf()

#ifndef __cplusplus
#if __STDC_VERSION__ >= 199901L
#include <stdbool.h>    // for bool if >= C99
#endif
#endif

double GuiDMValueBox(Rectangle bounds, double value, double minValue, double maxValue, int precision, bool editMode) {
    // FIXME: Hope all those `memmove()` functions are correctly used so we won't leak memory or overflow the buffer !!!
    static int framesCounter = 0;           // Required for blinking cursor
    static int cursor = 0;                  // Required for tracking the cursor position (only for a single active valuebox)
    
    enum {cursorTimer = 6, maxChars = 31, textPadding = 2};
    
    GuiState state = GuiGetState();
    
    // Make sure value is in range
    if(maxValue != minValue){
        if(value < minValue) value = minValue;
        if(value > maxValue) value = maxValue;
    }
    
    char textValue[maxChars + 1] = "\0"; 
    snprintf(textValue, maxChars, "%.*f", precision, value); // NOTE: use `snprintf` here so we don't overflow the buffer
    int len = strlen(textValue);
    
    bool valueHasChanged = false;
    
    // Update control
    //--------------------------------------------------------------------
    if ((state != STATE_DISABLED) && !guiLocked)
    {
        if (editMode)
        {
            // Make sure cursor position is correct
            if(cursor > len) cursor = len;
            if(cursor < 0) cursor = 0;
            
            state = STATE_PRESSED;
            framesCounter++;
            
            if(IsKeyPressed(KEY_RIGHT) || (IsKeyDown(KEY_RIGHT) && (framesCounter%cursorTimer == 0))) {
                // MOVE CURSOR TO RIGHT
                ++cursor;
                framesCounter = 0;
            } else if(IsKeyPressed(KEY_LEFT) || (IsKeyDown(KEY_LEFT) && (framesCounter%cursorTimer == 0))) {
                // MOVE CURSOR TO LEFT
                --cursor;
                framesCounter = 0;
            } else if (IsKeyPressed(KEY_BACKSPACE) || (IsKeyDown(KEY_BACKSPACE) && (framesCounter%cursorTimer) == 0)) {
                // HANDLE BACKSPACE
                if(cursor > 0) {
                    if(textValue[cursor-1] != '.') {
                        if(cursor < len ) 
                            memmove(&textValue[cursor-1], &textValue[cursor], len-cursor);
                        textValue[len - 1] = '\0';
                        valueHasChanged = true;
                    }
                    --cursor;
                }
                framesCounter = 0;
            } else if (IsKeyPressed(KEY_DELETE) || (IsKeyDown(KEY_DELETE) && (framesCounter%cursorTimer) == 0)) {
                // HANDLE DEL
                if(len > 0 && cursor < len && textValue[cursor] != '.') {
                    memmove(&textValue[cursor], &textValue[cursor+1], len-cursor);
                    textValue[len] = '\0';
                    len -= 1;
                    valueHasChanged = true;
                }
            } else if (IsKeyPressed(KEY_HOME)) {
                // MOVE CURSOR TO START
                cursor = 0;
            } else if (IsKeyPressed(KEY_END)) {
                // MOVE CURSOR TO END
                cursor = len;
            }  else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) {
                // COPY
                SetClipboardText(textValue);
            } else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_X)) {
                // CUT
                SetClipboardText(textValue);
                textValue[0] = '\0';
                cursor = len = 0;
                value = 0.0; // set it to 0 and pretend the value didn't change
            } else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) {
                // PASTE
                const char* clip = GetClipboardText();
                int clipLen = strlen(clip);
                clipLen = clipLen > maxChars ? maxChars : clipLen;
                memcpy(textValue, clip, clipLen);
                len = clipLen;
                textValue[len] = '\0';
                valueHasChanged = true;
            }
            else {
                // HANDLE KEY PRESS
                int key = GetKeyPressed();
                if( ((len < maxChars) && (key >= 48) && (key <= 57)) || (key == 46) || (key == 45)  ) // only allow 0..9, minus(-) and dot(.)
                {
                    if(precision != 0 && cursor < len) { // when we have decimals we can't insert at the end
                        memmove(&textValue[cursor], &textValue[cursor-1], len+1-cursor);
                        textValue[len+1] = '\0';
                        textValue[cursor] = (char)key;
                        cursor++;
                        valueHasChanged = true;
                    }
                    else if(precision == 0) {
                        if(cursor < len) memmove(&textValue[cursor], &textValue[cursor-1], len+1-cursor);
                        len += 1;
                        textValue[len+1] = '\0';
                        textValue[cursor] = (char)key;
                        cursor++;
                        valueHasChanged = true;
                    }
                }
            }
            
            // Make sure cursor position is correct
            if(cursor > len) cursor = len;
            if(cursor < 0) cursor = 0;
        }
        else
        {
            if (CheckCollisionPointRec(GetMousePosition(), bounds))
            {
                state = STATE_FOCUSED;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) framesCounter = 0;
            }
        }
    }
    //--------------------------------------------------------------------
    
    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleLinesEx(bounds, GuiGetStyle(VALUEBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(VALUEBOX, BORDER + (state*3))), guiAlpha));
    
    Rectangle textBounds = {bounds.x + GuiGetStyle(VALUEBOX, BORDER_WIDTH) + textPadding, bounds.y + GuiGetStyle(VALUEBOX, BORDER_WIDTH), 
        bounds.width - 2*(GuiGetStyle(VALUEBOX, BORDER_WIDTH) + textPadding), bounds.height - 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH)};
    
    int textWidth = GetTextWidth(textValue);
    if(textWidth > textBounds.width) textBounds.width = textWidth;
    
    if (state == STATE_PRESSED)
    {
        DrawRectangle(bounds.x + GuiGetStyle(VALUEBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(VALUEBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(VALUEBOX, BASE_COLOR_PRESSED)), guiAlpha));

        // Draw blinking cursor
        // NOTE: ValueBox internal text is always centered
        if (editMode && ((framesCounter/20)%2 == 0)) {
            // Measure text until the cursor
            int textWidthCursor = -2;
            if(cursor > 0) {
                char c = textValue[cursor];
                textValue[cursor] = '\0';
                textWidthCursor = GetTextWidth(textValue);
                textValue[cursor] = c;
            }
            //DrawRectangle(bounds.x + textWidthCursor + textPadding + 2, bounds.y + 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), 1, bounds.height - 4*GuiGetStyle(VALUEBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(VALUEBOX, BORDER_COLOR_PRESSED)), guiAlpha));
            DrawRectangle(bounds.x + textWidthCursor + (int)((bounds.width - textWidth - textPadding)/2.0f) + 2, bounds.y + 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), 1, bounds.height - 4*GuiGetStyle(VALUEBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(VALUEBOX, BORDER_COLOR_PRESSED)), guiAlpha));
        }
    }
    else if (state == STATE_DISABLED)
    {
        DrawRectangle(bounds.x + GuiGetStyle(VALUEBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(VALUEBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(VALUEBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(VALUEBOX, BASE_COLOR_DISABLED)), guiAlpha));
    }

    GuiDrawText(textValue, textBounds, TEXT_ALIGN_CENTER, Fade(GetColor(GuiGetStyle(VALUEBOX, TEXT + (state*3))), guiAlpha));
    
    value = valueHasChanged ? strtod(textValue, NULL) : value;
    
    // Make sure value is in range
    if(maxValue != minValue){
        if(value < minValue) value = minValue;
        if(value > maxValue) value = maxValue;
    }
    
    return value;
}



double GuiDMSpinner(Rectangle bounds, double value, double minValue, double maxValue, double step, int precision, bool editMode) {
    GuiState state = GuiGetState();

    Rectangle spinner = { bounds.x + GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH) + GuiGetStyle(SPINNER, SPIN_BUTTON_SPACING), bounds.y,
                          bounds.width - 2*(GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH) + GuiGetStyle(SPINNER, SPIN_BUTTON_SPACING)), bounds.height };
    Rectangle leftButtonBound = { (float)bounds.x, (float)bounds.y, (float)GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.height };
    Rectangle rightButtonBound = { (float)bounds.x + bounds.width - GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.y, (float)GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.height };
    
    // Update control
    //--------------------------------------------------------------------
    if ((state != STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        // Check spinner state
        if (CheckCollisionPointRec(mousePoint, bounds))
        {
            if (IsMouseButtonDown(MOUSE_LEFT_BUTTON)) state = STATE_PRESSED;
            else state = STATE_FOCUSED;
        }
    }
    //--------------------------------------------------------------------
    
    
    // Draw control
    //--------------------------------------------------------------------
    // Draw value selector custom buttons
    // NOTE: BORDER_WIDTH and TEXT_ALIGNMENT forced values
    int tempBorderWidth = GuiGetStyle(BUTTON, BORDER_WIDTH);
    int tempTextAlign = GuiGetStyle(BUTTON, TEXT_ALIGNMENT);
    GuiSetStyle(BUTTON, BORDER_WIDTH, GuiGetStyle(SPINNER, BORDER_WIDTH));
    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, TEXT_ALIGN_CENTER);

#if defined(RAYGUI_SUPPORT_RICONS)
    if (GuiButton(leftButtonBound, GuiIconText(RICON_ARROW_LEFT_FILL, NULL))) value -= step;
    if (GuiButton(rightButtonBound, GuiIconText(RICON_ARROW_RIGHT_FILL, NULL))) value += step;
#else
    if (GuiButton(leftButtonBound, "<")) value -= step;
    if (GuiButton(rightButtonBound, ">")) value += step;
#endif

    GuiSetStyle(BUTTON, TEXT_ALIGNMENT, tempTextAlign);
    GuiSetStyle(BUTTON, BORDER_WIDTH, tempBorderWidth);
    
    value = GuiDMValueBox(spinner, value, minValue, maxValue, precision, editMode);

    return value;
}



void GuiDMPropertyList(Rectangle bounds, GuiDMProperty* props, int count, int* focus, int* scrollIndex) {
    #ifdef RAYGUI_SUPPORT_RICONS
    #define PROPERTY_COLLAPSED_ICON "#120#"
    #define PROPERTY_EXPANDED_ICON "#121#"
    #else
    #define PROPERTY_COLLAPSED_ICON "+"
    #define PROPERTY_EXPANDED_ICON "-"
    #endif
    
    #define PROPERTY_PADDING 6
    #define PROPERTY_ICON_SIZE 16
    #define PROPERTY_DECIMAL_DIGITS 3  //how many digits to show (used only for the vector properties)
    
    // NOTE: Using ListView style for everything !!
    GuiState state = GuiGetState();
    int propFocused = (focus == NULL)? -1 : *focus;
    int scroll = *scrollIndex > 0 ? 0 : *scrollIndex; // NOTE: scroll should always be negative or 0
    
    // Each property occupies a certain number of slots, highly synchronized with the properties enum (GUI_PROP_BOOL ... GUI_PROP_SECTION)
    // NOTE: If you add a custom property type make sure to add the number of slots it occupies here !!
    const int propSlots[] = {1,1,1,2,1,3,4,5,5,5,1};

    Rectangle absoluteBounds = {0}; // total bounds for all of the properties (unclipped)
    // We need to loop over all the properties to get total height so we can see if we need a scrollbar or not
    for(int p=0; p<count; ++p) {
        // Calculate height of this property (properties can occupy 1 or more slots)
        int height = GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
        if(props[p].type < (sizeof(propSlots)/sizeof(propSlots[0]))) {
            if(!PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED)) height = propSlots[props[p].type]*GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
            if(props[p].type == GUI_PROP_SECTION && (PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED))) p += props[p].value.vsection; // skip slots for collapsed section
        }
        absoluteBounds.height += height+1;
    }
    
    // Check if we need a scrollbar and adjust bounds when necesary
    bool useScrollBar = absoluteBounds.height > bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH) ? true : false;
    if(!useScrollBar && scroll != 0) scroll = 0; // make sure scroll is 0 when there's no scrollbar 

    Rectangle scrollBarBounds = {bounds.x + GuiGetStyle(LISTVIEW, BORDER_WIDTH), bounds.y + GuiGetStyle(LISTVIEW, BORDER_WIDTH),
                GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH),  bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH)};
                
    absoluteBounds.x = bounds.x + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) + GuiGetStyle(DEFAULT, BORDER_WIDTH);
    absoluteBounds.y = bounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) + GuiGetStyle(DEFAULT, BORDER_WIDTH) + scroll;
    absoluteBounds.width = bounds.width - 2*(GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) + GuiGetStyle(DEFAULT, BORDER_WIDTH));
    
    if(useScrollBar) {
        if(GuiGetStyle(LISTVIEW, SCROLLBAR_SIDE) == SCROLLBAR_LEFT_SIDE)
            absoluteBounds.x += GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH); // scrollbar is on the LEFT, adjust bounds
        else
            scrollBarBounds.x = bounds.x + bounds.width - GuiGetStyle(LISTVIEW, BORDER_WIDTH) - GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH); // scrollbar is on the RIGHT
        absoluteBounds.width -= GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH); // adjust width to fit the scrollbar
    }
    
    int maxScroll = absoluteBounds.height + 2*(GuiGetStyle(LISTVIEW, LIST_ITEMS_SPACING) + GuiGetStyle(DEFAULT, BORDER_WIDTH))-bounds.height;
    
    // Update control
    //--------------------------------------------------------------------
    Vector2 mousePos = GetMousePosition();
    // NOTE: most of the update code is actually done in the draw control section
    if ((state != STATE_DISABLED) && !guiLocked) {
        if(!CheckCollisionPointRec(mousePos, bounds)) {
            propFocused = -1;
        }
        
        if (useScrollBar)
        {
            int wheelMove = GetMouseWheelMove();
            scroll += wheelMove*count;
            if(-scroll > maxScroll) scroll = -maxScroll;
        }
    }
    //--------------------------------------------------------------------
    
    
    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleRec(bounds, Fade(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), guiAlpha) );     // Draw background
    DrawRectangleLinesEx(bounds, GuiGetStyle(DEFAULT, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER + state*3)), guiAlpha)); // Draw border
    
    BeginScissorMode(absoluteBounds.x, bounds.y + GuiGetStyle(DEFAULT, BORDER_WIDTH), absoluteBounds.width, bounds.height - 2*GuiGetStyle(DEFAULT, BORDER_WIDTH));
        int currentHeight = 0;
        for(int p=0; p<count; ++p) 
        {
            int height = GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
            if(props[p].type < (sizeof(propSlots)/sizeof(propSlots[0])) && !PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED) )
                height = propSlots[props[p].type]*GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT); // get property height based on how many slots it occupies
            
            // Even with scissor mode on, draw only properties that we can see (comment out both BeginScissorMode() / EndScissorMode() to see this)
            if(absoluteBounds.y + currentHeight + height >= bounds.y && absoluteBounds.y + currentHeight <= bounds.y + bounds.height) 
            {
                Rectangle propBounds = {absoluteBounds.x, absoluteBounds.y + currentHeight, absoluteBounds.width, height};
                Color textColor = Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_NORMAL)), guiAlpha);
                int propState = STATE_NORMAL;
                
                // Get the state of this property and do some initial drawing
                if(PROP_CHECK_FLAG(&props[p], GUI_PFLAG_DISABLED)) { 
                    propState = STATE_DISABLED;
                    propBounds.height += 1; 
                    DrawRectangleRec(propBounds, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_DISABLED)), guiAlpha));
                    propBounds.height -= 1;
                    textColor = Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_DISABLED)), guiAlpha);
                } else {
                    if(CheckCollisionPointRec(mousePos, propBounds) && !guiLocked) {
                        if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                            propState = STATE_PRESSED;
                            //DrawRectangleRec(propRect, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_PRESSED)), guiAlpha));
                            textColor = Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_PRESSED)), guiAlpha);
                        } else { 
                            propState = STATE_FOCUSED;
                            propFocused = p;
                            //DrawRectangleRec(propRect, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_FOCUSED)), guiAlpha));
                            textColor = Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_FOCUSED)), guiAlpha);
                        }
                    } else propState = STATE_NORMAL; 
                }
                
                if(propState == STATE_DISABLED) GuiSetState(propState);
                switch(props[p].type) 
                {
                    case GUI_PROP_BOOL: {
                        // draw property name
                        GuiDrawText(props[p].name, (Rectangle){propBounds.x + PROPERTY_PADDING, propBounds.y, propBounds.width/2-PROPERTY_PADDING, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)}, TEXT_ALIGN_LEFT, textColor);
                        if(propState == STATE_PRESSED) props[p].value.vbool = !props[p].value.vbool; // toggle the property value when clicked
                        
                        // draw property value
                        const bool locked = guiLocked;
                        GuiLock(); // lock the checkbox since we changed the value manually
                        GuiCheckBox((Rectangle){propBounds.x+propBounds.width/2, propBounds.y + height/4, height/2, height/2}, props[p].value.vbool ? "Yes" : "No", props[p].value.vbool);
                        if(!locked) GuiUnlock(); // only unlock when needed
                    } break;
                    
                    case GUI_PROP_INT:
                        // draw property name
                        GuiDrawText(props[p].name, (Rectangle){propBounds.x + PROPERTY_PADDING, propBounds.y, propBounds.width/2-PROPERTY_PADDING, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)}, TEXT_ALIGN_LEFT, textColor);
                        // draw property value
                        props[p].value.vint.val = GuiDMSpinner((Rectangle){propBounds.x+propBounds.width/2, propBounds.y + 1, propBounds.width/2, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) - 2}, 
                                    props[p].value.vint.val, props[p].value.vint.min, props[p].value.vint.max, props[p].value.vint.step, 0, (propState == STATE_FOCUSED) );
                    break;
                    
                    case GUI_PROP_FLOAT:
                        // draw property name
                        GuiDrawText(props[p].name, (Rectangle){propBounds.x + PROPERTY_PADDING, propBounds.y, propBounds.width/2-PROPERTY_PADDING, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)}, TEXT_ALIGN_LEFT, textColor);
                        // draw property value
                        props[p].value.vfloat.val = GuiDMSpinner((Rectangle){propBounds.x+propBounds.width/2, propBounds.y + 1, propBounds.width/2, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) - 2},
                                    props[p].value.vfloat.val, props[p].value.vfloat.min, props[p].value.vfloat.max, props[p].value.vfloat.step, props[p].value.vfloat.precision, (propState == STATE_FOCUSED) );
                    break;
                    
                    case GUI_PROP_TEXT: {
                        Rectangle titleBounds = { propBounds.x, propBounds.y, propBounds.width, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) };
                        // Collapse/Expand property on click
                        if((propState == STATE_PRESSED) && CheckCollisionPointRec(mousePos, titleBounds))
                            PROP_TOGGLE_FLAG(&props[p], GUI_PFLAG_COLLAPSED);
                        
                        // draw property name
                        GuiDrawText(PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED) ? PROPERTY_COLLAPSED_ICON : PROPERTY_EXPANDED_ICON, titleBounds, TEXT_ALIGN_LEFT, textColor);
                        GuiDrawText(props[p].name, (Rectangle){propBounds.x+PROPERTY_ICON_SIZE+PROPERTY_PADDING, propBounds.y, propBounds.width-PROPERTY_ICON_SIZE-PROPERTY_PADDING, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)}, TEXT_ALIGN_LEFT, textColor);
                        GuiDrawText(TextFormat("%i/%i", strlen(props[p].value.vtext.val), props[p].value.vtext.size), (Rectangle){propBounds.x+propBounds.width/2, propBounds.y + 1, propBounds.width/2, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) - 2}, TEXT_ALIGN_LEFT, textColor);
                        
                        // draw property value
                        if(!PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED))
                            GuiTextBox((Rectangle){propBounds.x, propBounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)+1, propBounds.width, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)-2}, props[p].value.vtext.val, props[p].value.vtext.size, (propState == STATE_FOCUSED));
                    } break;
                    
                    case GUI_PROP_SELECT: {
                        // TODO: Create a custom dropdownbox control instead of using the raygui combobox
                        // draw property name
                        GuiDrawText(props[p].name, (Rectangle){propBounds.x + PROPERTY_PADDING, propBounds.y, propBounds.width/2-PROPERTY_PADDING, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)}, TEXT_ALIGN_LEFT, textColor);
                        // draw property value
                        props[p].value.vselect.active = GuiComboBox((Rectangle){propBounds.x+propBounds.width/2, propBounds.y + 1, propBounds.width/2, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) - 2}, 
                                props[p].value.vselect.val, props[p].value.vselect.active);
                    } break;
                    
                    case GUI_PROP_VECTOR2: case GUI_PROP_VECTOR3: case GUI_PROP_VECTOR4: {
                        Rectangle titleBounds = { propBounds.x, propBounds.y, propBounds.width, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) };
                        // Collapse/Expand property on click
                        if((propState == STATE_PRESSED) && CheckCollisionPointRec(mousePos, titleBounds))
                            PROP_TOGGLE_FLAG(&props[p], GUI_PFLAG_COLLAPSED);
                        
                        const char* fmt = "";
                        if(props[p].type == GUI_PROP_VECTOR2) fmt = TextFormat("[%.0f, %.0f]", props[p].value.v2.x, props[p].value.v2.y);
                        else if(props[p].type == GUI_PROP_VECTOR3) fmt = TextFormat("[%.0f, %.0f, %.0f]", props[p].value.v3.x, props[p].value.v3.y, props[p].value.v3.z);
                        else fmt = TextFormat("[%.0f, %.0f, %.0f, %.0f]", props[p].value.v4.x, props[p].value.v4.y, props[p].value.v4.z, props[p].value.v4.w);
                        
                        // draw property name
                        GuiDrawText(PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED) ? PROPERTY_COLLAPSED_ICON : PROPERTY_EXPANDED_ICON, titleBounds, TEXT_ALIGN_LEFT, textColor);
                        GuiDrawText(props[p].name, (Rectangle){propBounds.x+PROPERTY_ICON_SIZE+PROPERTY_PADDING, propBounds.y, propBounds.width-PROPERTY_ICON_SIZE-PROPERTY_PADDING, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)}, TEXT_ALIGN_LEFT, textColor);
                        GuiDrawText(fmt, (Rectangle){propBounds.x+propBounds.width/2, propBounds.y + 1, propBounds.width/2, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) - 2}, TEXT_ALIGN_LEFT, textColor);
                        
                        // draw X, Y, Z, W values (only when expanded)
                        if(!PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED)) {
                            Rectangle slotBounds = { propBounds.x, propBounds.y+GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)+1, propBounds.width, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)-2};
                            Rectangle lblBounds = { propBounds.x+PROPERTY_PADDING, slotBounds.y, GetTextWidth("A"), slotBounds.height};
                            Rectangle valBounds = { lblBounds.x+lblBounds.width+PROPERTY_PADDING, slotBounds.y, propBounds.width-lblBounds.width-2*PROPERTY_PADDING, slotBounds.height};
                            GuiDrawText("X", lblBounds, TEXT_ALIGN_LEFT, textColor);
                            props[p].value.v2.x = GuiDMSpinner(valBounds, props[p].value.v2.x, 0.0, 0.0, 1.0, PROPERTY_DECIMAL_DIGITS, (propState == STATE_FOCUSED) && CheckCollisionPointRec(mousePos, slotBounds) );
                            slotBounds.y += GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
                            lblBounds.y = valBounds.y = slotBounds.y;
                            GuiDrawText("Y", lblBounds, TEXT_ALIGN_LEFT, textColor);
                            props[p].value.v2.y = GuiDMSpinner(valBounds, props[p].value.v2.y, 0.0, 0.0, 1.0, PROPERTY_DECIMAL_DIGITS, (propState == STATE_FOCUSED) && CheckCollisionPointRec(mousePos, slotBounds) );
                            slotBounds.y += GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
                            lblBounds.y = valBounds.y = slotBounds.y;
                            if(props[p].type >= GUI_PROP_VECTOR3) {
                                GuiDrawText("Z", lblBounds, TEXT_ALIGN_LEFT, textColor);
                                props[p].value.v3.z = GuiDMSpinner(valBounds, props[p].value.v3.z, 0.0, 0.0, 1.0, PROPERTY_DECIMAL_DIGITS, (propState == STATE_FOCUSED) && CheckCollisionPointRec(mousePos, slotBounds) );
                                slotBounds.y += GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
                                lblBounds.y = valBounds.y = slotBounds.y;
                            }
                            
                            if(props[p].type >= GUI_PROP_VECTOR4) {
                                GuiDrawText("W", lblBounds, TEXT_ALIGN_LEFT, textColor);
                                props[p].value.v4.w = GuiDMSpinner(valBounds, props[p].value.v4.w, 0.0, 0.0, 1.0, PROPERTY_DECIMAL_DIGITS, (propState == STATE_FOCUSED) && CheckCollisionPointRec(mousePos, slotBounds) );
                            }
                        }
                    } break;
                    
                    case GUI_PROP_RECT:{
                        Rectangle titleBounds = { propBounds.x, propBounds.y, propBounds.width, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) };
                        // Collapse/Expand property on click
                        if((propState == STATE_PRESSED) && CheckCollisionPointRec(mousePos, titleBounds))
                            PROP_TOGGLE_FLAG(&props[p], GUI_PFLAG_COLLAPSED);
                        
                        // draw property name
                        GuiDrawText(PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED) ? PROPERTY_COLLAPSED_ICON : PROPERTY_EXPANDED_ICON, titleBounds, TEXT_ALIGN_LEFT, textColor);
                        GuiDrawText(props[p].name, (Rectangle){propBounds.x+PROPERTY_ICON_SIZE+PROPERTY_PADDING, propBounds.y, propBounds.width-PROPERTY_ICON_SIZE-PROPERTY_PADDING, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)}, TEXT_ALIGN_LEFT, textColor);
                        GuiDrawText(TextFormat("[%.0f, %.0f, %.0f, %.0f]", props[p].value.vrect.x, props[p].value.vrect.y, props[p].value.vrect.width, props[p].value.vrect.height), 
                                (Rectangle){propBounds.x+propBounds.width/2, propBounds.y + 1, propBounds.width/2, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) - 2}, TEXT_ALIGN_LEFT, textColor);
                        
                        // draw X, Y, Width, Height values (only when expanded)
                        if(!PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED)) {
                            Rectangle slotBounds = { propBounds.x, propBounds.y+GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)+1, propBounds.width, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)-2};
                            Rectangle lblBounds = { propBounds.x+PROPERTY_PADDING, slotBounds.y, GetTextWidth("Height"), slotBounds.height};
                            Rectangle valBounds = { lblBounds.x+lblBounds.width+PROPERTY_PADDING, slotBounds.y, propBounds.width-lblBounds.width-2*PROPERTY_PADDING, slotBounds.height};
                            GuiDrawText("X", lblBounds, TEXT_ALIGN_LEFT, textColor);
                            props[p].value.vrect.x = GuiDMSpinner(valBounds, props[p].value.vrect.x, 0.0, 0.0, 1.0, 0, (propState == STATE_FOCUSED) && CheckCollisionPointRec(mousePos, slotBounds) );
                            slotBounds.y += GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
                            lblBounds.y = valBounds.y = slotBounds.y;
                            GuiDrawText("Y", lblBounds, TEXT_ALIGN_LEFT, textColor);
                            props[p].value.vrect.y = GuiDMSpinner(valBounds, props[p].value.vrect.y, 0.0, 0.0, 1.0, 0, (propState == STATE_FOCUSED) && CheckCollisionPointRec(mousePos, slotBounds) );
                            slotBounds.y += GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
                            lblBounds.y = valBounds.y = slotBounds.y;
                            GuiDrawText("Width", lblBounds, TEXT_ALIGN_LEFT, textColor);
                            props[p].value.vrect.width = GuiDMSpinner(valBounds, props[p].value.vrect.width, 0.0, 0.0, 1.0, 0, (propState == STATE_FOCUSED) && CheckCollisionPointRec(mousePos, slotBounds) );
                            slotBounds.y += GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
                            lblBounds.y = valBounds.y = slotBounds.y;
                            GuiDrawText("Height", lblBounds, TEXT_ALIGN_LEFT, textColor);
                            props[p].value.vrect.height = GuiDMSpinner(valBounds, props[p].value.vrect.height, 0.0, 0.0, 1.0, 0, (propState == STATE_FOCUSED) && CheckCollisionPointRec(mousePos, slotBounds) );
                        }
                    } break;
                    
                    
                    case GUI_PROP_COLOR: {
                        Rectangle titleBounds = { propBounds.x, propBounds.y, propBounds.width, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) };
                        // Collapse/Expand property on click
                        if((propState == STATE_PRESSED) && CheckCollisionPointRec(mousePos, titleBounds))
                            PROP_TOGGLE_FLAG(&props[p], GUI_PFLAG_COLLAPSED);
                        
                        // draw property name
                        GuiDrawText(PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED) ? PROPERTY_COLLAPSED_ICON : PROPERTY_EXPANDED_ICON, titleBounds, TEXT_ALIGN_LEFT, textColor);
                        GuiDrawText(props[p].name, (Rectangle){propBounds.x+PROPERTY_ICON_SIZE+PROPERTY_PADDING, propBounds.y+1, propBounds.width-PROPERTY_ICON_SIZE-PROPERTY_PADDING, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)-2}, TEXT_ALIGN_LEFT, textColor);
                        DrawLineEx( (Vector2){propBounds.x+propBounds.width/2, propBounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) - 5}, (Vector2){propBounds.x+propBounds.width, propBounds.y + GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) - 5}, 6.0f, props[p].value.vcolor);
                        const char* fmt = TextFormat("#%02X%02X%02X%02X", props[p].value.vcolor.r, props[p].value.vcolor.g, props[p].value.vcolor.b, props[p].value.vcolor.a);
                        char clip[10] = "\0";
                        memcpy(clip, fmt, 10*sizeof(char)); // copy to temporary buffer since we can't be sure when TextFormat() will be called again and our text will be overwritten
                        GuiDrawText(fmt, (Rectangle){propBounds.x+propBounds.width/2, propBounds.y + 1, propBounds.width/2, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) - 2}, TEXT_ALIGN_LEFT, textColor);
                                
                        // draw R, G, B, A values (only when expanded)
                        if(!PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED)) {
                            Rectangle slotBounds = { propBounds.x, propBounds.y+GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)+1, propBounds.width, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)-2};
                            Rectangle lblBounds = { propBounds.x+PROPERTY_PADDING, slotBounds.y, GetTextWidth("A"), slotBounds.height};
                            Rectangle valBounds = { lblBounds.x+lblBounds.width+PROPERTY_PADDING, slotBounds.y, GetTextWidth("000000"), slotBounds.height};
                            Rectangle sbarBounds = { valBounds.x + valBounds.width + PROPERTY_PADDING, slotBounds.y, slotBounds.width - 3*PROPERTY_PADDING - lblBounds.width - valBounds.width, slotBounds.height };
                            
                            if(sbarBounds.width <= GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)-2) valBounds.width = propBounds.width-lblBounds.width-2*PROPERTY_PADDING; // hide slider when no space
                            // save current scrollbar style
                            int tmpSliderPadding = GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING);
                            int tmpPadding = GuiGetStyle(SCROLLBAR, SCROLL_PADDING);
                            int tmpBorder = GuiGetStyle(SCROLLBAR, BORDER_WIDTH);
                            int tmpSliderSize = GuiGetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE);
                            int tmpArrows =  GuiGetStyle(SCROLLBAR, ARROWS_VISIBLE);
                            Color tmpBG1 = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_DISABLED));
                            // set a custom scrollbar style
                            GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING, 3);
                            GuiSetStyle(SCROLLBAR, SCROLL_PADDING, 10);
                            GuiSetStyle(SCROLLBAR, BORDER_WIDTH, 0);
                            GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, 6);
                            GuiSetStyle(SCROLLBAR, ARROWS_VISIBLE, 0);
                            GuiSetStyle(DEFAULT, BORDER_COLOR_DISABLED, GuiGetStyle(DEFAULT, BACKGROUND_COLOR)); // disable scrollbar background
                            
                            GuiDrawText("R", lblBounds, TEXT_ALIGN_LEFT, textColor);
                            props[p].value.vcolor.r = GuiDMValueBox(valBounds, props[p].value.vcolor.r, 0.0, 255.0, 0, (propState == STATE_FOCUSED) && CheckCollisionPointRec(mousePos, slotBounds) );
                            if(sbarBounds.width > GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)-2) 
                                props[p].value.vcolor.r = GuiScrollBar(sbarBounds, props[p].value.vcolor.r, 0, 255);
                            slotBounds.y += GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
                            lblBounds.y = valBounds.y = sbarBounds.y = slotBounds.y;
                            
                            GuiDrawText("G", lblBounds, TEXT_ALIGN_LEFT, textColor);
                            props[p].value.vcolor.g = GuiDMValueBox(valBounds, props[p].value.vcolor.g, 0.0, 255.0, 0, (propState == STATE_FOCUSED) && CheckCollisionPointRec(mousePos, slotBounds) );
                            if(sbarBounds.width > GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)-2) 
                                props[p].value.vcolor.g = GuiScrollBar(sbarBounds, props[p].value.vcolor.g, 0, 255);
                            slotBounds.y += GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
                            lblBounds.y = valBounds.y = sbarBounds.y = slotBounds.y;
                            
                            GuiDrawText("B", lblBounds, TEXT_ALIGN_LEFT, textColor);
                            props[p].value.vcolor.b = GuiDMValueBox(valBounds, props[p].value.vcolor.b, 0.0, 255.0, 0, (propState == STATE_FOCUSED) && CheckCollisionPointRec(mousePos, slotBounds) );
                            if(sbarBounds.width > GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)-2) 
                                props[p].value.vcolor.b = GuiScrollBar(sbarBounds, props[p].value.vcolor.b, 0, 255);
                            slotBounds.y += GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
                            lblBounds.y = valBounds.y = sbarBounds.y = slotBounds.y;
                            
                            GuiDrawText("A", lblBounds, TEXT_ALIGN_LEFT, textColor);
                            props[p].value.vcolor.a = GuiDMValueBox(valBounds, props[p].value.vcolor.a, 0.0, 255.0, 0, (propState == STATE_FOCUSED) && CheckCollisionPointRec(mousePos, slotBounds) );
                            if(sbarBounds.width > GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)-2) 
                                props[p].value.vcolor.a = GuiScrollBar(sbarBounds, props[p].value.vcolor.a, 0, 255);
                            
                            // load saved scrollbar style
                            GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_PADDING, tmpSliderPadding);
                            GuiSetStyle(SCROLLBAR, SCROLL_PADDING, tmpPadding);
                            GuiSetStyle(SCROLLBAR, BORDER_WIDTH, tmpBorder);
                            GuiSetStyle(SCROLLBAR, SCROLL_SLIDER_SIZE, tmpSliderSize);
                            GuiSetStyle(SCROLLBAR, ARROWS_VISIBLE, tmpArrows);
                            GuiSetStyle(DEFAULT, BORDER_COLOR_DISABLED, ColorToInt(tmpBG1));
                        }
                        
                        // support COPY/PASTE (need to do this here since GuiDMValueBox() also has COPY/PASTE so we need to overwrite it)
                        if((propState == STATE_FOCUSED)) { 
                            if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) 
                                SetClipboardText(clip);
                            else if(IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)){
                                unsigned int a = props[p].value.vcolor.a, r = props[p].value.vcolor.r, g=props[p].value.vcolor.g, b=props[p].value.vcolor.b;
                                sscanf(GetClipboardText(), "#%02X%02X%02X%02X", &r, &g, &b, &a);
                                props[p].value.vcolor.r=r; props[p].value.vcolor.g=g; props[p].value.vcolor.b=b; props[p].value.vcolor.a=a;
                            }
                        }
                    } break;
                    
                    case GUI_PROP_SECTION: {
                        Rectangle titleBounds = { propBounds.x, propBounds.y, propBounds.width, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) };
                        // Collapse/Expand section on click
                        if( (propState == STATE_PRESSED) && CheckCollisionPointRec(mousePos, titleBounds) )
                            PROP_TOGGLE_FLAG(&props[p], GUI_PFLAG_COLLAPSED);
                        
                        if(!PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED)) {
                            GuiDrawText(PROPERTY_EXPANDED_ICON, titleBounds, TEXT_ALIGN_LEFT, textColor);
                            GuiDrawText(props[p].name, (Rectangle){propBounds.x+PROPERTY_ICON_SIZE+PROPERTY_PADDING, propBounds.y, propBounds.width-PROPERTY_ICON_SIZE-PROPERTY_PADDING, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)}, TEXT_ALIGN_CENTER, textColor);
                        } else {
                            GuiDrawText(PROPERTY_COLLAPSED_ICON, titleBounds, TEXT_ALIGN_LEFT, textColor);
                            GuiDrawText(TextFormat("%s [%i]", props[p].name, props[p].value.vsection), (Rectangle){propBounds.x+PROPERTY_ICON_SIZE+PROPERTY_PADDING, propBounds.y, propBounds.width-PROPERTY_ICON_SIZE-PROPERTY_PADDING, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)}, TEXT_ALIGN_CENTER, textColor);
                        }
                    } break;
                    
                    
                    // NOTE: Add your custom property here !!
                    default: {
                        // draw property name
                        GuiDrawText(props[p].name, (Rectangle){propBounds.x + PROPERTY_PADDING, propBounds.y, propBounds.width/2-PROPERTY_PADDING, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)}, TEXT_ALIGN_LEFT, textColor); 
                        // draw property type
                        GuiDrawText(TextFormat("TYPE %i", props[p].type), (Rectangle){propBounds.x+propBounds.width/2, propBounds.y + 1, propBounds.width/2, GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) - 2}, TEXT_ALIGN_LEFT, textColor);
                    } break;
                    
                } // end of switch()
                
                 GuiSetState(state);
            }
            
            currentHeight += height + 1;
            
            // Skip collapsed section. Don't put this code inside the switch !!
            if(props[p].type == GUI_PROP_SECTION && (PROP_CHECK_FLAG(&props[p], GUI_PFLAG_COLLAPSED))) p += props[p].value.vsection;
        } // end for
    EndScissorMode();
    
    if(useScrollBar) {
        scroll = -GuiScrollBar(scrollBarBounds, -scroll, 0, maxScroll);
        *scrollIndex = scroll;
    }
    //--------------------------------------------------------------------
    
    if(focus != NULL) *focus = propFocused;
}

bool GuiDMSaveProperties(const char* file, GuiDMProperty* props, int count) {
    if(file == NULL || props == NULL) return false;
    if(count == 0) return true;
    
    FILE* f = fopen(file, "w");
    if(f == NULL) return false;
    
    // write header
    fprintf(f, "#\n# Property types:\n"
            "#   b  <name> <flags> <value>                                    // Bool\n"
            "#   i  <name> <flags> <value> <min> <max> <step>                 // Int\n"
            "#   f  <name> <flags> <value> <min> <max> <step> <precision>     // Float\n"
            "#   t  <name> <flags> <value> <edit_length>                      // Text\n"
            "#   l  <name> <flags> <value> <active>                           // Select\n"
            "#   g  <name> <flags> <value>                                    // Section (Group)\n"
            "#   v2 <name> <flags> <x> <y>                                    // Vector 2D\n"
            "#   v3 <name> <flags> <x> <y> <z>                                // Vector 3D\n"
            "#   v4 <name> <flags> <x> <y> <z> <w>                            // Vector 4D\n"
            "#   r  <name> <flags> <x> <y> <width> <height>                   // Rectangle\n"
            "#   c  <name> <flags> <r> <g> <b> <a>                            // Color\n"
            "#\n\n");
    for(int p=0; p<count; ++p) 
    {
        switch(props[p].type) {
            case GUI_PROP_BOOL: fprintf(f, "b  %s %i %i\n", props[p].name, (int)props[p].flags, (int)props[p].value.vbool);
            break;
            
            case GUI_PROP_INT: fprintf(f, "i  %s %i %i %i %i %i\n", props[p].name, (int)props[p].flags, props[p].value.vint.val, props[p].value.vint.min, 
                        props[p].value.vint.max, props[p].value.vint.step); 
            break;
            
            case GUI_PROP_FLOAT: fprintf(f, "f  %s %i %f %f %f %f %i\n", props[p].name, (int)props[p].flags, props[p].value.vfloat.val, props[p].value.vfloat.min, 
                        props[p].value.vfloat.max, props[p].value.vfloat.step, props[p].value.vfloat.precision);
            break;
            
            case GUI_PROP_TEXT: fprintf(f, "t  %s %i %s %i\n", props[p].name, (int)props[p].flags, props[p].value.vtext.val, props[p].value.vtext.size);
            break;
            
            case GUI_PROP_SELECT: fprintf(f, "l  %s %i %s %i\n", props[p].name, (int)props[p].flags, props[p].value.vselect.val, props[p].value.vselect.active);
            break;
            
            case GUI_PROP_SECTION: fprintf(f, "g  %s %i %i\n", props[p].name, (int)props[p].flags, props[p].value.vsection);
            break;
            
            case GUI_PROP_VECTOR2: fprintf(f, "v2 %s %i %f %f\n", props[p].name, (int)props[p].flags, props[p].value.v2.x, props[p].value.v2.y);
            break;
            
            case GUI_PROP_VECTOR3: fprintf(f, "v3 %s %i %f %f %f\n", props[p].name, (int)props[p].flags, props[p].value.v3.x, props[p].value.v3.y, props[p].value.v3.z);
            break;
            
            case GUI_PROP_VECTOR4: fprintf(f, "v4 %s %i %f %f %f %f\n", props[p].name, (int)props[p].flags, props[p].value.v4.x, props[p].value.v4.y, 
                        props[p].value.v4.z, props[p].value.v4.w);
            break;
            
            case GUI_PROP_RECT: fprintf(f, "r  %s %i %i %i %i %i\n", props[p].name, (int)props[p].flags, (int)props[p].value.vrect.x, (int)props[p].value.vrect.y, 
                        (int)props[p].value.vrect.width, (int)props[p].value.vrect.height);
            break;
            
            case GUI_PROP_COLOR: fprintf(f, "c  %s %i %i %i %i %i\n", props[p].name, (int)props[p].flags, props[p].value.vcolor.r, props[p].value.vcolor.g, 
                        props[p].value.vcolor.b, props[p].value.vcolor.a);
            break;
        }
    }
    
    fclose(f);
    return true;
}

#endif // GUI_PROPERTY_LIST_IMPLEMENTATION