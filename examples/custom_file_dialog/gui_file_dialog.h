/*******************************************************************************************
*
*   FileDialog v1.2 - Modal file dialog to open/save files
*
*   MODULE USAGE:
*       #define GUI_FILE_DIALOG_IMPLEMENTATION
*       #include "gui_file_dialog.h"
*
*       INIT: GuiFileDialogState state = InitGuiFileDialog();
*       DRAW: GuiFileDialog(&state);
*
*   NOTE: This module depends on some raylib file system functions:
*       - LoadDirectoryFiles()
*       - UnloadDirectoryFiles()
*       - GetWorkingDirectory()
*       - DirectoryExists()
*       - FileExists()
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2019-2022 Ramon Santamaria (@raysan5)
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

#ifndef GUI_FILE_DIALOG_H
#define GUI_FILE_DIALOG_H

// Gui file dialog context data
typedef struct {

    // Window management variables
    bool windowActive;
    Rectangle windowBounds;
    Vector2 panOffset;
    bool dragMode;
    bool supportDrag;

    // UI variables
    bool dirPathEditMode;
    char dirPathText[1024];

    int filesListScrollIndex;
    bool filesListEditMode;
    int filesListActive;

    bool fileNameEditMode;
    char fileNameText[1024];
    bool SelectFilePressed;
    bool CancelFilePressed;
    int fileTypeActive;
    int itemFocused;

    // Custom state variables
    FilePathList dirFiles;
    char filterExt[256];
    char dirPathTextCopy[1024];
    char fileNameTextCopy[1024];

    int prevFilesListActive;

    bool saveFileMode;

} GuiFileDialogState;

#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
// ...

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
//...

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
GuiFileDialogState InitGuiFileDialog(const char *initPath);
void GuiFileDialog(GuiFileDialogState *state);

#ifdef __cplusplus
}
#endif

#endif // GUI_FILE_DIALOG_H

/***********************************************************************************
*
*   GUI_FILE_DIALOG IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_FILE_DIALOG_IMPLEMENTATION)

#include "../../src/raygui.h"

#include <string.h>     // Required for: strcpy()

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define MAX_DIRECTORY_FILES    2048
#define MAX_ICON_PATH_LENGTH    512

//----------------------------------------------------------------------------------
// Types and Structures Definition
//----------------------------------------------------------------------------------
#if defined(USE_CUSTOM_LISTVIEW_FILEINFO)
// Detailed file info type
typedef struct FileInfo {
    const char *name;
    int size;
    int modTime;
    int type;
    int icon;
} FileInfo;
#else
// Filename only
typedef char *FileInfo;             // Files are just a path string
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
FileInfo *dirFilesIcon = NULL;      // Path string + icon (for fancy drawing)

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
// Read files in new path
static void ReloadDirectoryFiles(GuiFileDialogState *state);

#if defined(USE_CUSTOM_LISTVIEW_FILEINFO)
// List View control for files info with extended parameters
static int GuiListViewFiles(Rectangle bounds, FileInfo *files, int count, int *focus, int *scrollIndex, int active);
#endif

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiFileDialogState InitGuiFileDialog(const char *initPath)
{
    GuiFileDialogState state = { 0 };

    // Init window data
    state.windowBounds = (Rectangle){ GetScreenWidth()/2 - 440/2, GetScreenHeight()/2 - 310/2, 440, 310 };
    state.windowActive = false;
    state.supportDrag = true;
    state.dragMode = false;
    state.panOffset = (Vector2){ 0, 0 };

    // Init path data
    state.dirPathEditMode = false;
    state.filesListActive = -1;
    state.prevFilesListActive = state.filesListActive;
    state.filesListScrollIndex = 0;

    state.fileNameEditMode = false;

    state.SelectFilePressed = false;
    state.CancelFilePressed = false;

    state.fileTypeActive = 0;

    strcpy(state.fileNameText, "\0");

    // Custom variables initialization
    if (initPath && DirectoryExists(initPath))
    {
        strcpy(state.dirPathText, initPath);
    }
    else if (initPath && FileExists(initPath))
    {
        strcpy(state.dirPathText, GetDirectoryPath(initPath));
        strcpy(state.fileNameText, GetFileName(initPath));
    }
    else strcpy(state.dirPathText, GetWorkingDirectory());

    // TODO: Why we keep a copy?
    strcpy(state.dirPathTextCopy, state.dirPathText);
    strcpy(state.fileNameTextCopy, state.fileNameText);

    state.filterExt[0] = '\0';
    //strcpy(state.filterExt, "all");

    state.dirFiles.count = 0;

    return state;
}

// Update and draw file dialog
void GuiFileDialog(GuiFileDialogState *state)
{
    if (state->windowActive)
    {
        // Update window dragging
        //----------------------------------------------------------------------------------------
        if (state->supportDrag)
        {
            Vector2 mousePosition = GetMousePosition();

            if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
            {
                // Window can be dragged from the top window bar
                if (CheckCollisionPointRec(mousePosition, (Rectangle){ state->windowBounds.x, state->windowBounds.y, (float)state->windowBounds.width, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }))
                {
                    state->dragMode = true;
                    state->panOffset.x = mousePosition.x - state->windowBounds.x;
                    state->panOffset.y = mousePosition.y - state->windowBounds.y;
                }
            }

            if (state->dragMode)
            {
                state->windowBounds.x = (mousePosition.x - state->panOffset.x);
                state->windowBounds.y = (mousePosition.y - state->panOffset.y);

                // Check screen limits to avoid moving out of screen
                if (state->windowBounds.x < 0) state->windowBounds.x = 0;
                else if (state->windowBounds.x > (GetScreenWidth() - state->windowBounds.width)) state->windowBounds.x = GetScreenWidth() - state->windowBounds.width;

                if (state->windowBounds.y < 0) state->windowBounds.y = 0;
                else if (state->windowBounds.y > (GetScreenHeight() - state->windowBounds.height)) state->windowBounds.y = GetScreenHeight() - state->windowBounds.height;

                if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) state->dragMode = false;
            }
        }
        //----------------------------------------------------------------------------------------

        // Load dirFilesIcon and state->dirFiles lazily on windows open
        // NOTE: They are automatically unloaded at fileDialog closing
        //----------------------------------------------------------------------------------------
        if (dirFilesIcon == NULL)
        {
            dirFilesIcon = (FileInfo *)RL_CALLOC(MAX_DIRECTORY_FILES, sizeof(FileInfo));    // Max files to read
            for (int i = 0; i < MAX_DIRECTORY_FILES; i++) dirFilesIcon[i] = (char *)RL_CALLOC(MAX_ICON_PATH_LENGTH, 1);    // Max file name length
        }

        // Load current directory files
        if (state->dirFiles.paths == NULL) ReloadDirectoryFiles(state);
        //----------------------------------------------------------------------------------------

        // Draw window and controls
        //----------------------------------------------------------------------------------------
        state->windowActive = !GuiWindowBox(state->windowBounds, "#198# Select File Dialog");

        // Draw previous directory button + logic
        if (GuiButton((Rectangle){ state->windowBounds.x + state->windowBounds.width - 48, state->windowBounds.y + 24 + 12, 40, 24 }, "< .."))
        {
            // Move dir path one level up
            strcpy(state->dirPathText, GetPrevDirectoryPath(state->dirPathText));

            // Reload directory files (frees previous list)
            ReloadDirectoryFiles(state);

            state->filesListActive = -1;
            memset(state->fileNameText, 0, 1024);
            memset(state->fileNameTextCopy, 0, 1024);
        }

        // Draw current directory text box info + path editing logic
        if (GuiTextBox((Rectangle){ state->windowBounds.x + 8, state->windowBounds.y + 24 + 12, state->windowBounds.width - 48 - 16, 24 }, state->dirPathText, 1024, state->dirPathEditMode))
        {
            if (state->dirPathEditMode)
            {
                // Verify if a valid path has been introduced
                if (DirectoryExists(state->dirPathText))
                {
                    // Reload directory files (frees previous list)
                    ReloadDirectoryFiles(state);

                    strcpy(state->dirPathTextCopy, state->dirPathText);
                }
                else strcpy(state->dirPathText, state->dirPathTextCopy);
            }

            state->dirPathEditMode = !state->dirPathEditMode;
        }

        // List view elements are aligned left
        int prevTextAlignment = GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT);
        int prevElementsHeight = GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
        GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, TEXT_ALIGN_LEFT);
        GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 24);
# if defined(USE_CUSTOM_LISTVIEW_FILEINFO)
        state->filesListActive = GuiListViewFiles((Rectangle){ state->position.x + 8, state->position.y + 48 + 20, state->windowBounds.width - 16, state->windowBounds.height - 60 - 16 - 68 }, fileInfo, state->dirFiles.count, &state->itemFocused, &state->filesListScrollIndex, state->filesListActive);
# else
        state->filesListActive = GuiListViewEx((Rectangle){ state->windowBounds.x + 8, state->windowBounds.y + 48 + 20, state->windowBounds.width - 16, state->windowBounds.height - 60 - 16 - 68 }, (const char**)dirFilesIcon, state->dirFiles.count, &state->itemFocused, &state->filesListScrollIndex, state->filesListActive);
# endif
        GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, prevTextAlignment);
        GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, prevElementsHeight);

        // Check if a path has been selected, if it is a directory, move to that directory (and reload paths)
        if ((state->filesListActive >= 0) && (state->filesListActive != state->prevFilesListActive))
            //&& (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_DPAD_A)))
        {
            strcpy(state->fileNameText, GetFileName(state->dirFiles.paths[state->filesListActive]));

            if (DirectoryExists(TextFormat("%s/%s", state->dirPathText, state->fileNameText)))
            {
                if (TextIsEqual(state->fileNameText, "..")) strcpy(state->dirPathText, GetPrevDirectoryPath(state->dirPathText));
                else strcpy(state->dirPathText, TextFormat("%s/%s", (strcmp(state->dirPathText, "/") == 0)? "" : state->dirPathText, state->fileNameText));

                strcpy(state->dirPathTextCopy, state->dirPathText);

                // Reload directory files (frees previous list)
                ReloadDirectoryFiles(state);

                strcpy(state->dirPathTextCopy, state->dirPathText);

                state->filesListActive = -1;
                strcpy(state->fileNameText, "\0");
                strcpy(state->fileNameTextCopy, state->fileNameText);
            }

            state->prevFilesListActive = state->filesListActive;
        }

        // Draw bottom controls
        //--------------------------------------------------------------------------------------
        GuiLabel((Rectangle){ state->windowBounds.x + 8, state->windowBounds.y + state->windowBounds.height - 68, 60, 24 }, "File name:");
        if (GuiTextBox((Rectangle){ state->windowBounds.x + 72, state->windowBounds.y + state->windowBounds.height - 68, state->windowBounds.width - 184, 24 }, state->fileNameText, 128, state->fileNameEditMode))
        {
            if (*state->fileNameText)
            {
                // Verify if a valid filename has been introduced
                if (FileExists(TextFormat("%s/%s", state->dirPathText, state->fileNameText)))
                {
                    // Select filename from list view
                    for (int i = 0; i < state->dirFiles.count; i++)
                    {
                        if (TextIsEqual(state->fileNameText, state->dirFiles.paths[i]))
                        {
                            state->filesListActive = i;
                            strcpy(state->fileNameTextCopy, state->fileNameText);
                            break;
                        }
                    }
                }
                else if (!state->saveFileMode)
                {
                    strcpy(state->fileNameText, state->fileNameTextCopy);
                }
            }

            state->fileNameEditMode = !state->fileNameEditMode;
        }

        GuiLabel((Rectangle){ state->windowBounds.x + 8, state->windowBounds.y + state->windowBounds.height - 24 - 12, 68, 24 }, "File filter:");
        state->fileTypeActive = GuiComboBox((Rectangle){ state->windowBounds.x + 72, state->windowBounds.y + state->windowBounds.height - 24 - 12, state->windowBounds.width - 184, 24 }, "All files", state->fileTypeActive);

        state->SelectFilePressed = GuiButton((Rectangle){ state->windowBounds.x + state->windowBounds.width - 96 - 8, state->windowBounds.y + state->windowBounds.height - 68, 96, 24 }, "Select");

        if (GuiButton((Rectangle){ state->windowBounds.x + state->windowBounds.width - 96 - 8, state->windowBounds.y + state->windowBounds.height - 24 - 12, 96, 24 }, "Cancel")) state->windowActive = false;
        //--------------------------------------------------------------------------------------

        // Exit on file selected
        if (state->SelectFilePressed) state->windowActive = false;

        // File dialog has been closed, free all memory before exit
        if (!state->windowActive)
        {
            // Free dirFilesIcon memory
            for (int i = 0; i < MAX_DIRECTORY_FILES; i++) RL_FREE(dirFilesIcon[i]);

            RL_FREE(dirFilesIcon);
            dirFilesIcon = NULL;

            // Unload directory file paths
            UnloadDirectoryFiles(state->dirFiles);

            // Reset state variables
            state->dirFiles.count = 0;
            state->dirFiles.capacity = 0;
            state->dirFiles.paths = NULL;
        }
    }
}

// Compare two files from a directory
static inline int FileCompare(const char *d1, const char *d2, const char *dir)
{
    const bool b1 = DirectoryExists(TextFormat("%s/%s", dir, d1));
    const bool b2 = DirectoryExists(TextFormat("%s/%s", dir, d2));

    if (b1 && !b2) return -1;
    if (!b1 && b2) return 1;

    if (!FileExists(TextFormat("%s/%s", dir, d1))) return 1;
    if (!FileExists(TextFormat("%s/%s", dir, d2))) return -1;

    return strcmp(d1, d2);
}

// Read files in new path
static void ReloadDirectoryFiles(GuiFileDialogState *state)
{
    UnloadDirectoryFiles(state->dirFiles);

    state->dirFiles = LoadDirectoryFilesEx(state->dirPathText, (state->filterExt[0] == '\0')? NULL : state->filterExt, false);
    state->itemFocused = 0;

    // Reset dirFilesIcon memory
    for (int i = 0; i < MAX_DIRECTORY_FILES; i++) memset(dirFilesIcon[i], 0, MAX_ICON_PATH_LENGTH);

    // Copy paths as icon + fileNames into dirFilesIcon
    for (int i = 0; i < state->dirFiles.count; i++)
    {
        if (IsPathFile(state->dirFiles.paths[i]))
        {
            // Path is a file, a file icon for convenience (for some recognized extensions)
            if (IsFileExtension(state->dirFiles.paths[i], ".png;.bmp;.tga;.gif;.jpg;.jpeg;.psd;.hdr;.qoi;.dds;.pkm;.ktx;.pvr;.astc"))
            {
                strcpy(dirFilesIcon[i], TextFormat("#12#%s", GetFileName(state->dirFiles.paths[i])));
            }
            else if (IsFileExtension(state->dirFiles.paths[i], ".wav;.mp3;.ogg;.flac;.xm;.mod;.it;.wma;.aiff"))
            {
                strcpy(dirFilesIcon[i], TextFormat("#11#%s", GetFileName(state->dirFiles.paths[i])));
            }
            else if (IsFileExtension(state->dirFiles.paths[i], ".txt;.info;.md;.nfo;.xml;.json;.c;.cpp;.cs;.lua;.py;.glsl;.vs;.fs"))
            {
                strcpy(dirFilesIcon[i], TextFormat("#10#%s", GetFileName(state->dirFiles.paths[i])));
            }
            else if (IsFileExtension(state->dirFiles.paths[i], ".exe;.bin;.raw;.msi"))
            {
                strcpy(dirFilesIcon[i], TextFormat("#200#%s", GetFileName(state->dirFiles.paths[i])));
            }
            else strcpy(dirFilesIcon[i], TextFormat("#218#%s", GetFileName(state->dirFiles.paths[i])));
        }
        else
        {
            // Path is a directory, add a directory icon
            strcpy(dirFilesIcon[i], TextFormat("#1#%s", GetFileName(state->dirFiles.paths[i])));
        }
    }
}

#if defined(USE_CUSTOM_LISTVIEW_FILEINFO)
// List View control for files info with extended parameters
static int GuiListViewFiles(Rectangle bounds, FileInfo *files, int count, int *focus, int *scrollIndex, int active)
{
    GuiState state = guiState;
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
                    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) itemSelected = startIndex + i;
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
    DrawRectangleRec(bounds, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));     // Draw background
    DrawRectangleLinesEx(bounds, GuiGetStyle(DEFAULT, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER + state*3)), guiAlpha));

    // TODO: Draw list view header with file sections: icon+name | size | type | modTime

    // Draw visible items
    for (int i = 0; i < visibleItems; i++)
    {
        if (state == GUI_STATE_DISABLED)
        {
            if ((startIndex + i) == itemSelected)
            {
                DrawRectangleRec(itemBounds, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_DISABLED)), guiAlpha));
                DrawRectangleLinesEx(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_DISABLED)), guiAlpha));
            }

            // TODO: Draw full file info line: icon+name | size | type | modTime

            GuiDrawText(files[startIndex + i].name, GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_DISABLED)), guiAlpha));
        }
        else
        {
            if ((startIndex + i) == itemSelected)
            {
                // Draw item selected
                DrawRectangleRec(itemBounds, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_PRESSED)), guiAlpha));
                DrawRectangleLinesEx(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_PRESSED)), guiAlpha));

                GuiDrawText(files[startIndex + i].name, GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_PRESSED)), guiAlpha));
            }
            else if ((startIndex + i) == itemFocused)
            {
                // Draw item focused
                DrawRectangleRec(itemBounds, Fade(GetColor(GuiGetStyle(LISTVIEW, BASE_COLOR_FOCUSED)), guiAlpha));
                DrawRectangleLinesEx(itemBounds, GuiGetStyle(LISTVIEW, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(LISTVIEW, BORDER_COLOR_FOCUSED)), guiAlpha));

                GuiDrawText(files[startIndex + i].name, GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_FOCUSED)), guiAlpha));
            }
            else
            {
                // Draw item normal
                GuiDrawText(files[startIndex + i].name, GetTextBounds(DEFAULT, itemBounds), GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT), Fade(GetColor(GuiGetStyle(LISTVIEW, TEXT_COLOR_NORMAL)), guiAlpha));
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

        int prevSliderSize = GuiGetStyle(SCROLLBAR, SLIDER_WIDTH);  // Save default slider size
        int prevScrollSpeed = GuiGetStyle(SCROLLBAR, SCROLL_SPEED); // Save default scroll speed
        GuiSetStyle(SCROLLBAR, SLIDER_WIDTH, sliderSize);           // Change slider size
        GuiSetStyle(SCROLLBAR, SCROLL_SPEED, count - visibleItems); // Change scroll speed

        startIndex = GuiScrollBar(scrollBarBounds, startIndex, 0, count - visibleItems);

        GuiSetStyle(SCROLLBAR, SCROLL_SPEED, prevScrollSpeed); // Reset scroll speed to default
        GuiSetStyle(SCROLLBAR, SLIDER_WIDTH, prevSliderSize);  // Reset slider size to default
    }
    //--------------------------------------------------------------------

    if (focus != NULL) *focus = itemFocused;
    if (scrollIndex != NULL) *scrollIndex = startIndex;

    return itemSelected;
}
#endif // USE_CUSTOM_LISTVIEW_FILEINFO

#endif // GUI_FILE_DIALOG_IMPLEMENTATION
