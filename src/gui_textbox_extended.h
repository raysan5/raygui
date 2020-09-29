/*******************************************************************************************
*
*   Text box extended (cursor positioning and editing)
*
*   MODULE USAGE:
*       #define GUI_TEXTBOX_EXTENDED_IMPLEMENTATION
*       #include "gui_textbox_extended.h"
*
*   On game draw call:  GuiTextBoxEx(...);
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2019 Vlad Adrian (@Demizdor) and Ramon Santamaria (@raysan5)
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

#ifndef GUI_TEXTBOX_EXTENDED_H
#define GUI_TEXTBOX_EXTENDED_H

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------

// Text box state data
typedef struct GuiTextBoxState {
    int cursor;      // Cursor position in text
    int start;       // Text start position (from where we begin drawing the text)
    int index;       // Text start index (index inside the text of `start` always in sync)
    int select;      // Marks position of cursor when selection has started
} GuiTextBoxState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
RAYGUIDEF void GuiTextBoxSetActive(Rectangle bounds);                   // Sets the active textbox
RAYGUIDEF Rectangle GuiTextBoxGetActive(void);                          // Get bounds of active textbox

RAYGUIDEF void GuiTextBoxSetCursor(int cursor);                         // Set cursor position of active textbox
RAYGUIDEF int GuiTextBoxGetCursor(void);                                // Get cursor position of active textbox

RAYGUIDEF void GuiTextBoxSetSelection(int start, int length);           // Set selection of active textbox
RAYGUIDEF Vector2 GuiTextBoxGetSelection(void);                         // Get selection of active textbox (x - selection start  y - selection length)

RAYGUIDEF bool GuiTextBoxIsActive(Rectangle bounds);                    // Returns true if a textbox control with specified `bounds` is the active textbox
RAYGUIDEF GuiTextBoxState GuiTextBoxGetState(void);                     // Get state for the active textbox
RAYGUIDEF void GuiTextBoxSetState(GuiTextBoxState state);               // Set state for the active textbox (state must be valid else things will break)

RAYGUIDEF void GuiTextBoxSelectAll(const char *text);                   // Select all characters in the active textbox (same as pressing `CTRL` + `A`)
RAYGUIDEF void GuiTextBoxCopy(const char *text);                        // Copy selected text to clipboard from the active textbox (same as pressing `CTRL` + `C`)
RAYGUIDEF void GuiTextBoxPaste(char *text, int textSize);               // Paste text from clipboard into the textbox (same as pressing `CTRL` + `V`)
RAYGUIDEF void GuiTextBoxCut(char *text);                               // Cut selected text in the active textbox and copy it to clipboard (same as pressing `CTRL` + `X`)
RAYGUIDEF int GuiTextBoxDelete(char *text, int length, bool before);    // Deletes a character or selection before from the active textbox (depending on `before`). Returns bytes deleted.
RAYGUIDEF int GuiTextBoxGetByteIndex(const char *text, int start, int from, int to); // Get the byte index for a character starting at position `from` with index `start` until position `to`.

RAYGUIDEF bool GuiTextBoxEx(Rectangle bounds, char *text, int textSize, bool editMode);

#ifdef __cplusplus
}
#endif

#endif // GUI_TEXTBOX_EXTENDED_H

/***********************************************************************************
*
*   GUI TEXTBOX EXTENDED IMPLEMENTATION
*
************************************************************************************/

#if defined(GUI_TEXTBOX_EXTENDED_IMPLEMENTATION)

#ifndef RAYGUI_H
#include "raygui.h"
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// Cursor measure mode
typedef enum {
    GUI_MEASURE_MODE_CURSOR_END = 0xA,
    GUI_MEASURE_MODE_CURSOR_POS,
    GUI_MEASURE_MODE_CURSOR_COORDS
} GuiMeasureMode;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
static Rectangle guiTextBoxActive = { 0 };      // Area of the currently active textbox

static GuiTextBoxState guiTextBoxState = {      // Keeps state of the active textbox
    .cursor = -1,
    .start = 0,
    .index = 0,
    .select = -1 
};

//----------------------------------------------------------------------------------
// Module Internal Functions Declaration
//----------------------------------------------------------------------------------
static int GetPrevCodepoint(const char *text, const char *start, int *prev);
static int GuiMeasureTextBox(const char *text, int length, Rectangle rec, int *pos, int mode);
static int GuiMeasureTextBoxRev(const char *text, int length, Rectangle rec, int *pos);                 // Highly synchronized with calculations in DrawTextRecEx()

static inline int GuiTextBoxGetCursorCoordinates(const char *text, int length, Rectangle rec, int pos); // Calculate cursor coordinates based on the cursor position `pos` inside the `text`.
static inline int GuiTextBoxGetCursorFromMouse(const char *text, int length, Rectangle rec, int *pos);  // Calculate cursor position in textbox based on mouse coordinates.
static inline int GuiTextBoxMaxCharacters(const char *text, int length, Rectangle rec);                 // Calculates how many characters is the textbox able to draw inside rec
static inline unsigned int GuiCountCodepointsUntilNewline(const char *text);    // Returns total number of characters(codepoints) in a UTF8 encoded `text` until `\0` or a `\n` is found.

static inline void MoveTextBoxCursorRight(const char *text, int length, Rectangle textRec);
static inline void MoveTextBoxCursorLeft(const char *text);

static int EncodeCodepoint(unsigned int c, char out[5]);

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

// Sets the active textbox (reseting state of the previous active textbox)
RAYGUIDEF void GuiTextBoxSetActive(Rectangle bounds)
{
    guiTextBoxActive = bounds;
    guiTextBoxState = (GuiTextBoxState){ .cursor = -1, .start = 0, .index = 0, .select = -1 };
}

// Gets bounds of active textbox
RAYGUIDEF Rectangle GuiTextBoxGetActive(void) { return guiTextBoxActive; }

// Set cursor position of active textbox
RAYGUIDEF void GuiTextBoxSetCursor(int cursor)
{
    guiTextBoxState.cursor = (cursor < 0) ? -1 : cursor;
    guiTextBoxState.start = -1; // Mark this to be recalculated
}

// Get cursor position of active textbox
RAYGUIDEF int GuiTextBoxGetCursor(void) { return guiTextBoxState.cursor; }

// Set selection of active textbox
RAYGUIDEF void GuiTextBoxSetSelection(int start, int length)
{
    if (start < 0) start = 0;
    if (length < 0) length = 0;

    GuiTextBoxSetCursor(start + length);
    guiTextBoxState.select = start;
}

// Get selection of active textbox
RAYGUIDEF Vector2 GuiTextBoxGetSelection(void)
{
    if (guiTextBoxState.select == -1 || guiTextBoxState.select == guiTextBoxState.cursor) return RAYGUI_CLITERAL(Vector2){ 0 };
    else if (guiTextBoxState.cursor > guiTextBoxState.select) return RAYGUI_CLITERAL(Vector2){ guiTextBoxState.select, guiTextBoxState.cursor - guiTextBoxState.select };

    return RAYGUI_CLITERAL(Vector2){ guiTextBoxState.cursor, guiTextBoxState.select - guiTextBoxState.cursor };
}

// Returns true if a textbox control with specified `bounds` is the active textbox
RAYGUIDEF bool GuiTextBoxIsActive(Rectangle bounds)
{
    return (bounds.x == guiTextBoxActive.x && bounds.y == guiTextBoxActive.y &&
            bounds.width == guiTextBoxActive.width && bounds.height == guiTextBoxActive.height);
}

RAYGUIDEF GuiTextBoxState GuiTextBoxGetState(void) { return guiTextBoxState; }
RAYGUIDEF void GuiTextBoxSetState(GuiTextBoxState state)
{
    // NOTE: should we check if state values are valid ?!?
    guiTextBoxState = state;
}

RAYGUIDEF int GuiTextBoxGetByteIndex(const char *text, int start, int from, int to)
{
    int i = start, k = from;

    while ((text[i] != '\0') && (k < to))
    {
        int j = 0;
        int letter = GetNextCodepoint(&text[i], &j);

        if (letter == 0x3f) j = 1;
        i += j;
        ++k;
    }

    return i;
}

RAYGUIDEF int GuiTextBoxDelete(char *text, int length, bool before)
{
    if ((guiTextBoxState.cursor != -1) && (text != NULL))
    {
        int startIdx = 0, endIdx = 0;
        if ((guiTextBoxState.select != -1) && (guiTextBoxState.select != guiTextBoxState.cursor))
        {
            // Delete selection
            int start = guiTextBoxState.cursor;
            int end = guiTextBoxState.select;

            if (guiTextBoxState.cursor > guiTextBoxState.select)
            {
                start = guiTextBoxState.select;
                end = guiTextBoxState.cursor;
            }

            // Convert to byte indexes
            startIdx = GuiTextBoxGetByteIndex(text, 0, 0, start);
            endIdx = GuiTextBoxGetByteIndex(text, 0, 0, end);

            // Adjust text box state
            guiTextBoxState.cursor = start; // Always set cursor to start of selection
            if (guiTextBoxState.select < guiTextBoxState.start) guiTextBoxState.start = -1; // Force to recalculate on the next frame
        }
        else
        {
            if (before)
            {
                // Delete character before cursor
                if (guiTextBoxState.cursor != 0)
                {
                    endIdx = GuiTextBoxGetByteIndex(text, 0, 0, guiTextBoxState.cursor);
                    guiTextBoxState.cursor--;
                    startIdx = GuiTextBoxGetByteIndex(text, 0, 0, guiTextBoxState.cursor);

                    if (guiTextBoxState.cursor < guiTextBoxState.start) guiTextBoxState.start = -1; // Force to recalculate on the next frame
                }
            }
            else
            {
                // Delete character after cursor
                if (guiTextBoxState.cursor + 1 <= GuiCountCodepointsUntilNewline(text))
                {
                    startIdx = GuiTextBoxGetByteIndex(text, 0, 0, guiTextBoxState.cursor);
                    endIdx = GuiTextBoxGetByteIndex(text, 0, 0, guiTextBoxState.cursor+1);
                }
            }
        }

        memmove(&text[startIdx], &text[endIdx], length - endIdx);
        text[length - (endIdx - startIdx)] = '\0';
        guiTextBoxState.select = -1; // Always deselect

        return (endIdx - startIdx);
    }

    return 0;
}

RAYGUIDEF void GuiTextBoxSelectAll(const char *text)
{
    guiTextBoxState.cursor = GuiCountCodepointsUntilNewline(text);

    if (guiTextBoxState.cursor > 0)
    {
        guiTextBoxState.select = 0;
        guiTextBoxState.start = -1; // Force recalculate on the next frame
    }
    else guiTextBoxState.select = -1;
}

RAYGUIDEF void GuiTextBoxCopy(const char *text)
{
    if ((text != NULL) &&
        (guiTextBoxState.select != -1) &&
        (guiTextBoxState.cursor != -1) &&
        (guiTextBoxState.select != guiTextBoxState.cursor))
    {
        int start = guiTextBoxState.cursor;
        int end = guiTextBoxState.select;

        if (guiTextBoxState.cursor > guiTextBoxState.select)
        {
            start = guiTextBoxState.select;
            end = guiTextBoxState.cursor;
        }

        // Convert to byte indexes
        start = GuiTextBoxGetByteIndex(text, 0, 0, start);
        end = GuiTextBoxGetByteIndex(text, 0, 0, end);

        // FIXME: `TextSubtext()` only lets use copy TEXTSPLIT_MAX_TEXT_LENGTH (1024) bytes
        // maybe modify `SetClipboardText()` so we can use it only on part of a string
        const char *clipText = TextSubtext(text, start, end - start);

        SetClipboardText(clipText);
    }
}

// Paste text from clipboard into the active textbox.
// `text` is the pointer to the buffer used by the textbox while `textSize` is the text buffer max size
RAYGUIDEF void GuiTextBoxPaste(char *text, int textSize)
{
    const char *clipText = GetClipboardText(); // GLFW guaratees this should be UTF8 encoded!
    int length = strlen(text);

    if ((text != NULL) && (clipText != NULL) && (guiTextBoxState.cursor != -1))
    {
        if ((guiTextBoxState.select != -1) && (guiTextBoxState.select != guiTextBoxState.cursor))
        {
            // If there's a selection we'll have to delete it first
            length -= GuiTextBoxDelete(text, length, true);
        }

        int clipLen = strlen(clipText); // We want the length in bytes

        // Calculate how many bytes can we copy from clipboard text before we run out of space
        int size = ((length + clipLen) <= textSize) ? clipLen : textSize - length;

        // Make room by shifting to right the bytes after cursor
        int startIdx = GuiTextBoxGetByteIndex(text, 0, 0, guiTextBoxState.cursor);
        int endIdx = startIdx + size;
        memmove(&text[endIdx], &text[startIdx], length - startIdx);
        text[length + size] = '\0'; // Set the NULL char

        // At long last copy the clipboard text
        memcpy(&text[startIdx], clipText, size);

        // Set cursor position at the end of the pasted text
        guiTextBoxState.cursor = 0;

        for (int i = 0; i < (startIdx + size); guiTextBoxState.cursor++)
        {
            int next = 0;
            int letter = GetNextCodepoint(&text[i], &next);
            if (letter != 0x3f) i += next;
            else i += 1;
        }

        guiTextBoxState.start = -1; // Force to recalculate on the next frame
    }
}

RAYGUIDEF void GuiTextBoxCut(char* text)
{
    if ((text != NULL) &&
        (guiTextBoxState.select != -1) &&
        (guiTextBoxState.cursor != -1) &&
        (guiTextBoxState.select != guiTextBoxState.cursor))
    {
        // First copy selection to clipboard;
        int start = guiTextBoxState.cursor, end = guiTextBoxState.select;

        if (guiTextBoxState.cursor > guiTextBoxState.select)
        {
            start = guiTextBoxState.select;
            end = guiTextBoxState.cursor;
        }

        // Convert to byte indexes
        int startIdx = GuiTextBoxGetByteIndex(text, 0, 0, start);
        int endIdx = GuiTextBoxGetByteIndex(text, 0, 0, end);

        // FIXME: `TextSubtext()` only lets use copy TEXTSPLIT_MAX_TEXT_LENGTH (1024) bytes
        // maybe modify `SetClipboardText()` so we can use it only on parts of a string
        const char *clipText = TextSubtext(text, startIdx, endIdx - startIdx);
        SetClipboardText(clipText);

        // Now delete selection (copy data over it)
        int len = strlen(text);
        memmove(&text[startIdx], &text[endIdx], len - endIdx);
        text[len - (endIdx - startIdx)] = '\0';

        // Adjust text box state
        guiTextBoxState.cursor = start; // Always set cursor to start of selection
        if (guiTextBoxState.select < guiTextBoxState.start) guiTextBoxState.start = -1; // Force to recalculate
        guiTextBoxState.select = -1;    // Deselect
    }
}

// A text box control supporting text selection, cursor positioning and commonly used keyboard shortcuts.
// NOTE 1: Requires static variables: framesCounter
// NOTE 2: Returns if KEY_ENTER pressed (useful for data validation)
RAYGUIDEF bool GuiTextBoxEx(Rectangle bounds, char *text, int textSize, bool editMode)
{
    // Define the cursor movement/selection speed when movement keys are held/pressed
    #define TEXTBOX_CURSOR_COOLDOWN   5

    static int framesCounter = 0;           // Required for blinking cursor

    GuiControlState state = guiState;
    bool pressed = false;

    // Make sure length doesn't exceed `textSize`. `textSize` is actually the max amount of characters the textbox can handle.
    int length = strlen(text);
    if (length > textSize)
    {
        text[textSize] = '\0';
        length = textSize;
    }

    // Make sure we have enough room to draw at least 1 character
    if ((bounds.width - 2*GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING)) < GuiGetStyle(DEFAULT, TEXT_SIZE))
    {
        bounds.width = GuiGetStyle(DEFAULT, TEXT_SIZE) + 2*GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING);
    }

    // Center the text vertically
    int verticalPadding = (bounds.height - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH) - GuiGetStyle(DEFAULT, TEXT_SIZE))/2;

    if (verticalPadding < 0)
    {
        // Make sure the height is sufficient
        bounds.height = 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH) + GuiGetStyle(DEFAULT, TEXT_SIZE);
        verticalPadding = 0;
    }

    // Calculate the drawing area for the text inside the control `bounds`
    Rectangle textRec = { bounds.x + GuiGetStyle(TEXTBOX, BORDER_WIDTH) + GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING),
                          bounds.y + verticalPadding + GuiGetStyle(TEXTBOX, BORDER_WIDTH),
                          bounds.width - 2*(GuiGetStyle(TEXTBOX, TEXT_INNER_PADDING) + GuiGetStyle(TEXTBOX, BORDER_WIDTH)),
                          GuiGetStyle(DEFAULT, TEXT_SIZE) };

    Vector2 cursorPos = { textRec.x, textRec.y };   // This holds the coordinates inside textRec of the cursor at current position and will be recalculated later
    bool active = GuiTextBoxIsActive(bounds);       // Check if this textbox is the global active textbox

    int selStart = 0, selLength = 0, textStartIndex = 0;

    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked)
    {
        Vector2 mousePoint = GetMousePosition();

        if (editMode)
        {
            // Check if we are the global active textbox
            // A textbox becomes active when the user clicks it :)
            if (!active)
            {
                if (CheckCollisionPointRec(mousePoint, bounds) && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)))
                {
                    // Hurray!!! we just became the active textbox
                    active = true;
                    GuiTextBoxSetActive(bounds);
                }
            }
            else if (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON))
            {
                // When active and the right mouse is clicked outside the textbox we should deactivate it
                // NOTE: We set a dummy rect as the active textbox bounds
                GuiTextBoxSetActive(RAYGUI_CLITERAL(Rectangle){ 0, 0, -1, -1 });
                active = false;
            }

            if (active)
            {
                state = GUI_STATE_PRESSED;
                framesCounter++;

                // Make sure state doesn't have invalid values
                if (guiTextBoxState.cursor > length) guiTextBoxState.cursor = -1;
                if (guiTextBoxState.select > length) guiTextBoxState.select = -1;
                if (guiTextBoxState.start > length) guiTextBoxState.start = -1;


                // Check textbox state for changes and recalculate if necesary
                if (guiTextBoxState.cursor == -1)
                {
                    // Set cursor to last visible character in textbox
                    guiTextBoxState.cursor = GuiTextBoxMaxCharacters(text, length, textRec);
                }

                if (guiTextBoxState.start == -1)
                {
                    // Force recalculate text start position and text start index

                    // NOTE: start and index are always in sync
                    // start will hold the starting character position from where the text will be drawn
                    // while index will hold the byte index inside the text for that character

                    if (guiTextBoxState.cursor == 0)
                    {
                        guiTextBoxState.start = guiTextBoxState.index = 0; // No need to recalculate
                    }
                    else
                    {
                        int pos = 0;
                        int len =  GuiTextBoxGetByteIndex(text, 0, 0, guiTextBoxState.cursor);
                        guiTextBoxState.index = GuiMeasureTextBoxRev(text, len, textRec, &pos);
                        guiTextBoxState.start = guiTextBoxState.cursor - pos + 1;
                    }
                }

                // -----------------
                // HANDLE KEY INPUT
                // -----------------
                // * -> | LSHIFT + -> move cursor to the right | increase selection by one
                // * <- | LSHIFT + <- move cursor to the left | decrease selection by one
                // * HOME | LSHIFT + HOME moves cursor to start of text | selects text from cursor to start of text
                // * END | LSHIFT + END move cursor to end of text | selects text from cursor until end of text
                // * CTRL + A select all characters in text
                // * CTRL + C copy selected text
                // * CTRL + X cut selected text
                // * CTRL + V remove selected text, if any, then paste clipboard data
                // * DEL delete character or selection after cursor
                // * BACKSPACE delete character or selection before cursor
                // TODO: Add more shortcuts (insert mode, select word, moveto/select prev/next word ...)
                if (IsKeyPressed(KEY_RIGHT) || (IsKeyDown(KEY_RIGHT) && (framesCounter%TEXTBOX_CURSOR_COOLDOWN == 0)))
                {
                    if (IsKeyDown(KEY_LEFT_SHIFT))
                    {
                        // Selecting
                        if (guiTextBoxState.select == -1) guiTextBoxState.select = guiTextBoxState.cursor; // Mark selection start

                        MoveTextBoxCursorRight(text, length, textRec);
                    }
                    else
                    {
                        if (guiTextBoxState.select != -1 && guiTextBoxState.select != guiTextBoxState.cursor)
                        {
                            // Deselect and move cursor to end of selection
                            if (guiTextBoxState.cursor < guiTextBoxState.select)
                            {
                                guiTextBoxState.cursor = guiTextBoxState.select - 1;
                                MoveTextBoxCursorRight(text, length, textRec);
                            }
                        }
                        else
                        {
                            // Move cursor to the right
                            MoveTextBoxCursorRight(text, length, textRec);
                        }

                        guiTextBoxState.select = -1;
                    }

                    framesCounter = 0;
                }
                else if (IsKeyPressed(KEY_LEFT) || (IsKeyDown(KEY_LEFT) && (framesCounter%TEXTBOX_CURSOR_COOLDOWN == 0)))
                {
                    if (IsKeyDown(KEY_LEFT_SHIFT))
                    {
                        // Selecting
                        if (guiTextBoxState.select == -1) guiTextBoxState.select = guiTextBoxState.cursor; // Mark selection start

                        MoveTextBoxCursorLeft(text);
                    }
                    else
                    {
                        if ((guiTextBoxState.select != -1) && (guiTextBoxState.select != guiTextBoxState.cursor))
                        {
                            // Deselect and move cursor to start of selection
                            if (guiTextBoxState.cursor > guiTextBoxState.select)
                            {
                                guiTextBoxState.cursor = guiTextBoxState.select;

                                if (guiTextBoxState.start > guiTextBoxState.cursor)
                                {
                                    guiTextBoxState.start = guiTextBoxState.cursor;
                                    guiTextBoxState.index = GuiTextBoxGetByteIndex(text, 0, 0, guiTextBoxState.start); // Recalculate byte index
                                }
                            }
                        }
                        else
                        {
                            // Move cursor to the left
                            MoveTextBoxCursorLeft(text);
                        }

                        guiTextBoxState.select = -1;
                    }

                    framesCounter = 0;
                }
                else if (IsKeyPressed(KEY_BACKSPACE) || (IsKeyDown(KEY_BACKSPACE) && (framesCounter%TEXTBOX_CURSOR_COOLDOWN) == 0))
                {
                    GuiTextBoxDelete(text, length, true);
                }
                else if (IsKeyPressed(KEY_DELETE) || (IsKeyDown(KEY_DELETE) && (framesCounter%TEXTBOX_CURSOR_COOLDOWN) == 0))
                {
                    GuiTextBoxDelete(text, length, false);
                }
                else if (IsKeyPressed(KEY_HOME))
                {
                    if (IsKeyDown(KEY_LEFT_SHIFT))
                    {
                        // Select from start of text to cursor
                        if ((guiTextBoxState.select > guiTextBoxState.cursor) || ((guiTextBoxState.select == -1) && (guiTextBoxState.cursor != 0)))
                        {
                            guiTextBoxState.select = guiTextBoxState.cursor;
                        }
                    }
                    else guiTextBoxState.select = -1; // Deselect everything

                    // Move cursor to start of text
                    guiTextBoxState.cursor = guiTextBoxState.start = guiTextBoxState.index = 0;
                    framesCounter = 0;
                }
                else if (IsKeyPressed(KEY_END))
                {
                    int max = GuiCountCodepointsUntilNewline(text);

                    if (IsKeyDown(KEY_LEFT_SHIFT))
                    {
                        if ((guiTextBoxState.select == -1) && (guiTextBoxState.cursor != max))
                        {
                            guiTextBoxState.select = guiTextBoxState.cursor;
                        }
                    }
                    else guiTextBoxState.select = -1; // Deselect everything

                    int pos = 0;
                    guiTextBoxState.cursor = max;
                    int len = GuiTextBoxGetByteIndex(text, 0, 0, guiTextBoxState.cursor);
                    guiTextBoxState.index = GuiMeasureTextBoxRev(text, len, textRec, &pos);
                    guiTextBoxState.start = guiTextBoxState.cursor - pos + 1;
                }
                else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_A)) GuiTextBoxSelectAll(text); // CTRL + A > Select all
                else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C)) GuiTextBoxCopy(text);      // CTRL + C > Copy selected text to clipboard
                else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_X)) GuiTextBoxCut(text);       // CTRL + X > Cut selected text
                else if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_V)) GuiTextBoxPaste(text, textSize); // CTRL + V > Paste clipboard text
                else if (IsKeyPressed(KEY_ENTER)) pressed = true;
                else
                {
                    int key = GetKeyPressed();
                    if ((key >= 32) && ((guiTextBoxState.cursor + 1) < textSize))
                    {
                        if ((guiTextBoxState.select != -1) && (guiTextBoxState.select != guiTextBoxState.cursor))
                        {
                            // Delete selection
                            GuiTextBoxDelete(text, length, true);
                        }

                        // Decode codepoint
                        char out[5] = {0};
                        int sz = EncodeCodepoint(key, &out[0]);

                        if (sz != 0)
                        {
                            int startIdx = GuiTextBoxGetByteIndex(text, 0, 0, guiTextBoxState.cursor);
                            int endIdx = startIdx + sz;

                            if (endIdx <= textSize && length < textSize - 1)
                            {
                                guiTextBoxState.cursor++;
                                guiTextBoxState.select = -1;
                                memmove(&text[endIdx], &text[startIdx], length - startIdx);
                                memcpy(&text[startIdx], &out[0], sz);
                                length += sz;
                                text[length] = '\0';

                                if (guiTextBoxState.start != -1)
                                {
                                    const int max = GuiTextBoxMaxCharacters(&text[guiTextBoxState.index], length - guiTextBoxState.index, textRec);

                                    if ((guiTextBoxState.cursor - guiTextBoxState.start) > max) guiTextBoxState.start = -1;
                                }
                            }
                        }
                    }
                }

                // -------------
                // HANDLE MOUSE
                // -------------
                if (CheckCollisionPointRec(mousePoint, bounds))
                {
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
                    {
                        if (CheckCollisionPointRec(mousePoint, textRec))
                        {
                            GuiTextBoxGetCursorFromMouse(&text[guiTextBoxState.index], length - guiTextBoxState.index, textRec, &guiTextBoxState.cursor);
                            guiTextBoxState.cursor += guiTextBoxState.start;
                            guiTextBoxState.select = -1;
                        }
                        else
                        {
                            // Clicked outside the `textRec` but still inside bounds
                            if (mousePoint.x <= bounds.x+bounds.width/2) guiTextBoxState.cursor = 0 + guiTextBoxState.start;
                            else guiTextBoxState.cursor = guiTextBoxState.start + GuiTextBoxMaxCharacters(&text[guiTextBoxState.index], length - guiTextBoxState.index, textRec);
                            guiTextBoxState.select = -1;
                        }
                    }
                    else if (IsMouseButtonDown(MOUSE_LEFT_BUTTON))
                    {
                        int cursor = guiTextBoxState.cursor - guiTextBoxState.start;
                        bool move = false;
                        if (CheckCollisionPointRec(mousePoint, textRec))
                        {
                            GuiTextBoxGetCursorFromMouse(&text[guiTextBoxState.index], length - guiTextBoxState.index, textRec, &cursor);
                        }
                        else
                        {
                            // Clicked outside the `textRec` but still inside bounds, this means that we must move the text
                            move = true;
                            if (mousePoint.x > bounds.x+bounds.width/2)
                            {
                                cursor = GuiTextBoxMaxCharacters(&text[guiTextBoxState.index], length - guiTextBoxState.index, textRec);
                            }
                        }

                        guiTextBoxState.cursor = cursor + guiTextBoxState.start;

                        if (guiTextBoxState.select == -1)
                        {
                            // Mark start of selection
                            guiTextBoxState.select = guiTextBoxState.cursor;
                        }

                        // Move the text when cursor is positioned before or after the text
                        if ((framesCounter%TEXTBOX_CURSOR_COOLDOWN) == 0 && move)
                        {
                            if (cursor == 0) MoveTextBoxCursorLeft(text);
                            else if (cursor == GuiTextBoxMaxCharacters(&text[guiTextBoxState.index], length - guiTextBoxState.index, textRec))
                            {
                                MoveTextBoxCursorRight(text, length, textRec);
                            }
                        }
                    }
                }

                // Calculate X coordinate of the blinking cursor
                cursorPos.x = GuiTextBoxGetCursorCoordinates(&text[guiTextBoxState.index], length - guiTextBoxState.index, textRec, guiTextBoxState.cursor - guiTextBoxState.start);

                // Update variables
                textStartIndex = guiTextBoxState.index;

                if (guiTextBoxState.select == -1)
                {
                    selStart = guiTextBoxState.cursor;
                    selLength = 0;
                }
                else if (guiTextBoxState.cursor > guiTextBoxState.select)
                {
                    selStart = guiTextBoxState.select;
                    selLength = guiTextBoxState.cursor - guiTextBoxState.select;
                }
                else
                {
                    selStart = guiTextBoxState.cursor;
                    selLength = guiTextBoxState.select - guiTextBoxState.cursor;
                }

                // We aren't drawing all of the text so make sure `DrawTextRecEx()` is selecting things correctly
                if (guiTextBoxState.start > selStart)
                {
                    selLength -= guiTextBoxState.start - selStart;
                    selStart = 0;
                }
                else selStart = selStart - guiTextBoxState.start;
            }
            else state = GUI_STATE_FOCUSED;
            
            if (IsKeyPressed(KEY_ENTER) || (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(0))) pressed = true;
        }
        else
        {
            if (active && IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_C))
            {
                // If active copy all text to clipboard even when disabled

                // Backup textbox state
                int select = guiTextBoxState.select;
                int cursor = guiTextBoxState.cursor;
                int start = guiTextBoxState.start;

                if ((guiTextBoxState.select == -1) || (guiTextBoxState.select == guiTextBoxState.cursor))
                {
                    // If no selection then mark all text to be copied to clipboard
                    GuiTextBoxSelectAll(text);
                }

                GuiTextBoxCopy(text);

                // Restore textbox state
                guiTextBoxState.select = select;
                guiTextBoxState.cursor = cursor;
                guiTextBoxState.start = start;
            }
            
            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                state = GUI_STATE_FOCUSED;
                if (IsMouseButtonPressed(0)) pressed = true;
            }

        }
        
        if (pressed) framesCounter = 0;
    }

    // Draw control
    //--------------------------------------------------------------------
    DrawRectangleLinesEx(bounds, GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER + (state*3))), guiAlpha));

    if (state == GUI_STATE_PRESSED)
    {
        DrawRectangle(bounds.x + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_FOCUSED)), guiAlpha));

        // Draw blinking cursor
        if (editMode && active && ((framesCounter/TEXTEDIT_CURSOR_BLINK_FRAMES)%2 == 0) && selLength == 0)
        {
            DrawRectangle(cursorPos.x, cursorPos.y, 1, GuiGetStyle(DEFAULT, TEXT_SIZE)*2, Fade(GetColor(GuiGetStyle(TEXTBOX, BORDER_COLOR_PRESSED)), guiAlpha));
        }
    }
    else if (state == GUI_STATE_DISABLED)
    {
        DrawRectangle(bounds.x + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.y + GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.width - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), bounds.height - 2*GuiGetStyle(TEXTBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(TEXTBOX, BASE_COLOR_DISABLED)), guiAlpha));
    }

    // Finally draw the text and selection
    DrawTextRecEx(guiFont, &text[textStartIndex], textRec, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING), false, Fade(GetColor(GuiGetStyle(TEXTBOX, TEXT + (state*3))), guiAlpha), selStart, selLength, GetColor(GuiGetStyle(TEXTBOX, COLOR_SELECTED_FG)), GetColor(GuiGetStyle(TEXTBOX, COLOR_SELECTED_BG)));

    return pressed;
}

//----------------------------------------------------------------------------------
// Module Internal Functions Definition
//----------------------------------------------------------------------------------

static int GetPrevCodepoint(const char *text, const char *start, int *prev)
{
    int c = 0x3f;
    char *p = (char *)text;
    *prev = 1;

    for (int i = 0; (p >= start) && (i < 4); p--, i++)
    {
        if ((((unsigned char)*p) >> 6) != 2)
        {
            c = GetNextCodepoint(p, prev);
            break;
        }
    }

    return c;
}

// Returns total number of characters(codepoints) in a UTF8 encoded `text` until `\0` or a `\n` is found.
// NOTE: If a invalid UTF8 sequence is encountered a `?`(0x3f) codepoint is counted instead.
static inline unsigned int GuiCountCodepointsUntilNewline(const char *text)
{
    unsigned int len = 0;
    char *ptr = (char*)&text[0];

    while ((*ptr != '\0') && (*ptr != '\n'))
    {
        int next = 0;
        int letter = GetNextCodepoint(ptr, &next);

        if (letter == 0x3f) ptr += 1;
        else ptr += next;
        ++len;
    }

    return len;
}

// Highly synchronized with calculations in DrawTextRecEx()
static int GuiMeasureTextBox(const char *text, int length, Rectangle rec, int *pos, int mode)
{
    // Get gui font properties
    const Font font = guiFont;
    const float fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    const float spacing = GuiGetStyle(DEFAULT, TEXT_SPACING);

    int textOffsetX = 0;        // Offset between characters
    float scaleFactor = 0.0f;

    int letter = 0;   // Current character
    int index = 0;    // Index position in sprite font

    scaleFactor = fontSize/font.baseSize;

    int i = 0, k = 0;
    int glyphWidth = 0;

    for (i = 0; i < length; i++, k++)
    {
        glyphWidth = 0;
        int next = 1;
        letter = GetNextCodepoint(&text[i], &next);
        if (letter == 0x3f) next = 1;
        index = GetGlyphIndex(font, letter);
        i += next - 1;

        if (letter != '\n')
        {
            glyphWidth = (font.chars[index].advanceX == 0)?
                         (int)(font.recs[index].width*scaleFactor + spacing):
                         (int)(font.chars[index].advanceX*scaleFactor + spacing);

            if ((textOffsetX + glyphWidth + 1) >= rec.width) break;

            if ((mode == GUI_MEASURE_MODE_CURSOR_POS) && (*pos == k)) break;
            else if (mode == GUI_MEASURE_MODE_CURSOR_COORDS)
            {
                // Check if the mouse pointer is inside the glyph rect
                Rectangle grec = {rec.x + textOffsetX - 1, rec.y, glyphWidth, (font.baseSize + font.baseSize/2)*scaleFactor - 1 };
                Vector2 mouse = GetMousePosition();

                if (CheckCollisionPointRec(mouse, grec))
                {
                    // Smooth selection by dividing the glyph rectangle into 2 equal parts and checking where the mouse resides
                    if (mouse.x > (grec.x + glyphWidth/2))
                    {
                        textOffsetX += glyphWidth;
                        k++;
                    }

                    break;
                }
            }
        }
        else break;

        textOffsetX += glyphWidth;
    }

    *pos = k;

    return (rec.x + textOffsetX - 1);
}

// Required by GuiTextBoxEx()
// Highly synchronized with calculations in DrawTextRecEx()
static int GuiMeasureTextBoxRev(const char *text, int length, Rectangle rec, int *pos)
{
    // Get gui font properties
    const Font font = guiFont;
    const float fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    const float spacing = GuiGetStyle(DEFAULT, TEXT_SPACING);

    int textOffsetX = 0;        // Offset between characters
    float scaleFactor = 0.0f;

    int letter = 0;   // Current character
    int index = 0;    // Index position in sprite font

    scaleFactor = fontSize/font.baseSize;

    int i = 0, k = 0;
    int glyphWidth = 0, prev = 1;
    for (i = length; i >= 0; i--, k++)
    {
        glyphWidth = 0;
        letter = GetPrevCodepoint(&text[i], &text[0], &prev);

        if (letter == 0x3f) prev = 1;
        index = GetGlyphIndex(font, letter);
        i -= prev - 1;

        if (letter != '\n')
        {
            glyphWidth = (font.chars[index].advanceX == 0)?
                         (int)(font.recs[index].width*scaleFactor + spacing):
                         (int)(font.chars[index].advanceX*scaleFactor + spacing);

            if ((textOffsetX + glyphWidth + 1) >= rec.width) break;
        }
        else break;

        textOffsetX += glyphWidth;
    }

    *pos = k;

    return (i + prev);
}

// Calculate cursor coordinates based on the cursor position `pos` inside the `text`.
static inline int GuiTextBoxGetCursorCoordinates(const char *text, int length, Rectangle rec, int pos)
{
    return GuiMeasureTextBox(text, length, rec, &pos, GUI_MEASURE_MODE_CURSOR_POS);
}

// Calculate cursor position in textbox based on mouse coordinates.
static inline int GuiTextBoxGetCursorFromMouse(const char *text, int length, Rectangle rec, int* pos)
{
    return GuiMeasureTextBox(text, length, rec, pos, GUI_MEASURE_MODE_CURSOR_COORDS);
}

// Calculates how many characters is the textbox able to draw inside rec
static inline int GuiTextBoxMaxCharacters(const char *text, int length, Rectangle rec)
{
    int pos = -1;
    GuiMeasureTextBox(text, length, rec, &pos, GUI_MEASURE_MODE_CURSOR_END);
    return pos;
}

static inline void MoveTextBoxCursorRight(const char* text, int length, Rectangle textRec)
{
    // FIXME: Counting codepoints each time we press the key is expensive, find another way
    int count = GuiCountCodepointsUntilNewline(text);
    if (guiTextBoxState.cursor < count ) guiTextBoxState.cursor++;

    const int max = GuiTextBoxMaxCharacters(&text[guiTextBoxState.index], length - guiTextBoxState.index, textRec);

    if ((guiTextBoxState.cursor - guiTextBoxState.start) > max)
    {
        const int cidx = GuiTextBoxGetByteIndex(text, guiTextBoxState.index, guiTextBoxState.start, guiTextBoxState.cursor);
        int pos = 0;
        guiTextBoxState.index = GuiMeasureTextBoxRev(text, cidx - 1, textRec, &pos);
        guiTextBoxState.start = guiTextBoxState.cursor - pos;
    }
}

static inline void MoveTextBoxCursorLeft(const char* text)
{
    if (guiTextBoxState.cursor > 0) guiTextBoxState.cursor--;

    if (guiTextBoxState.cursor < guiTextBoxState.start)
    {
        int prev = 0;
        int letter = GetPrevCodepoint(&text[guiTextBoxState.index - 1], text, &prev);
        if (letter == 0x3f) prev = 1;
        guiTextBoxState.start--;
        guiTextBoxState.index -= prev;
    }
}

static int EncodeCodepoint(unsigned int c, char out[5])
{
    int len = 0;
    if (c <= 0x7f)
    {
        out[0] = (char)c;
        len = 1;
    }
    else if (c <= 0x7ff)
    {
        out[0] = (char)(((c >> 6) & 0x1f) | 0xc0);
        out[1] = (char)((c & 0x3f) | 0x80);
        len = 2;
    }
    else if (c <= 0xffff)
    {
        out[0] = (char)(((c >> 12) & 0x0f) | 0xe0);
        out[1] = (char)(((c >>  6) & 0x3f) | 0x80);
        out[2] = (char)((c & 0x3f) | 0x80);
        len = 3;
    }
    else if (c <= 0x10ffff)
    {
        out[0] = (char)(((c >> 18) & 0x07) | 0xf0);
        out[1] = (char)(((c >> 12) & 0x3f) | 0x80);
        out[2] = (char)(((c >>  6) & 0x3f) | 0x80);
        out[3] = (char)((c & 0x3f) | 0x80);
        len = 4;
    }

    out[len] = 0;
    return len;
}

#endif // GUI_TEXTBOX_EXTENDED_IMPLEMENTATION
