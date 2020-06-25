/*******************************************************************************************
*
*   FileDialog v1.1 - Modal file dialog to open/save files
*
*   MODULE USAGE:
*       #define GUI_FILE_DIALOG_IMPLEMENTATION
*       #include "gui_file_dialog.h"
*
*       INIT: GuiFileDialogState state = InitGuiFileDialog();
*       DRAW: GuiFileDialog(&state);
*
*   NOTE: This module depends on some raylib file system functions:
*       - GetDirectoryFiles()
*       - ClearDirectoryFiles()
*       - GetWorkingDirectory()
*       - DirectoryExists()
*       - FileExists()
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2019-2020 Ramon Santamaria (@raysan5)
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

// WARNING: raygui implementation is expected to be defined before including this header
#undef RAYGUI_IMPLEMENTATION
#include "../../src/raygui.h"

#ifndef GUI_FILE_DIALOG_H
#define GUI_FILE_DIALOG_H

typedef struct {
    Vector2 position;
    Vector2 size;

    bool fileDialogActive;

    bool dirPathEditMode;
    char dirPathText[256];

    int filesListScrollIndex;
    bool filesListEditMode;
    int filesListActive;

    bool fileNameEditMode;
    char fileNameText[256];
    bool SelectFilePressed;
    bool CancelFilePressed;
    int fileTypeActive;
    int itemFocused;

    // Custom state variables (depend on development software)
    // NOTE: This variables should be added manually if required
    char **dirFiles;
    int dirFilesCount;

    char filterExt[256];

    char dirPathTextCopy[256];
    char fileNameTextCopy[256];

    int prevFilesListActive;

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
GuiFileDialogState InitGuiFileDialog(int width, int height, const char *initPath, bool active);
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
#define MAX_DIRECTORY_FILES    1024
#define MAX_DIR_PATH_LENGTH    1024

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
typedef char *FileInfo;
#endif

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------
FileInfo *dirFilesIcon = NULL;

//----------------------------------------------------------------------------------
// Internal Module Functions Definition
//----------------------------------------------------------------------------------
// Read all filenames from directory (supported file types)
static char **ReadDirectoryFiles(const char *dir, int *filesCount, char *filterExt);

#if defined(USE_CUSTOM_LISTVIEW_FILEINFO)
// List View control for files info with extended parameters
static int GuiListViewFiles(Rectangle bounds, FileInfo *files, int count, int *focus, int *scrollIndex, int active);
#endif

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------
GuiFileDialogState InitGuiFileDialog(int width, int height, const char *initPath, bool active)
{
    GuiFileDialogState state = { 0 };

    // Default dialog size is 440x310
    state.size.x = width == -1 ? 440 : width;
    state.size.y = height == -1 ? 310 : height;
    state.position = (Vector2){ GetScreenWidth()/2 - state.size.x/2, GetScreenHeight()/2 - state.size.y/2 };

    state.fileDialogActive = active;
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

    strcpy(state.dirPathTextCopy, state.dirPathText);
    strcpy(state.fileNameTextCopy, state.fileNameText);

    strcpy(state.filterExt, "all");

    state.dirFilesCount = 0;
    state.dirFiles = NULL;      // NOTE: Loaded lazily on window active

    return state;
}

// Read files in new path
static void FD_RELOAD_DIRPATH(GuiFileDialogState *state)
{
    for (int i = 0; i < state->dirFilesCount; i++) RL_FREE(state->dirFiles[i]);
    RL_FREE(state->dirFiles);

    state->dirFiles = ReadDirectoryFiles(state->dirPathText, &state->dirFilesCount, state->filterExt);
    state->itemFocused = 0;
}

// Update and draw file dialog
void GuiFileDialog(GuiFileDialogState *state)
{
    if (state->fileDialogActive)
    {
        const int winWidth = state->size.x;
        const int winHeight = state->size.y;

        // Load dirFilesIcon and state->dirFiles lazily on windows open
        // NOTE: they are automatically unloaded at fileDialog closing
        //------------------------------------------------------------------------------------
        if (dirFilesIcon == NULL)
        {
            dirFilesIcon = (FileInfo *)RL_MALLOC(MAX_DIRECTORY_FILES*sizeof(FileInfo));    // Max files to read
            for (int i = 0; i < MAX_DIRECTORY_FILES; i++) dirFilesIcon[i] = (char *)calloc(MAX_DIR_PATH_LENGTH, 1);    // Max file name length
        }

        if (state->dirFiles == NULL)
        {
            state->dirFiles = ReadDirectoryFiles(state->dirPathText, &state->dirFilesCount, state->filterExt);

            for(int f = 0; f < state->dirFilesCount; f++)
            {
                if (strcmp(state->fileNameText, state->dirFiles[f]) == 0)
                {
                    if (state->filesListActive != f) state->filesListScrollIndex = state->filesListActive = f; // make it active and visible only on first call

                    break;
                }
            }
        }
        //------------------------------------------------------------------------------------

        DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), 0.85f));
        state->fileDialogActive = !GuiWindowBox((Rectangle){ state->position.x + 0, state->position.y + 0, winWidth, winHeight }, "#198#LuaJIT | Select File Dialog");

        if (GuiButton((Rectangle){ state->position.x + winWidth - 50, state->position.y + 35, 40, 25 }, "< .."))// || IsKeyReleased(KEY_DPAD_Y))
        {
            // Move dir path one level up
            strcpy(state->dirPathText, GetPrevDirectoryPath(state->dirPathText));

            // RL_FREE previous dirFiles (reloaded by ReadDirectoryFiles())
            FD_RELOAD_DIRPATH(state);

            state->filesListActive = -1;
            strcpy(state->fileNameText, "\0");
            strcpy(state->fileNameTextCopy, state->fileNameText);
        }

        if (GuiTextBox((Rectangle){ state->position.x + 10, state->position.y + 35, winWidth - 65, 25 }, state->dirPathText, 256, state->dirPathEditMode))
        {
            if (state->dirPathEditMode)
            {
                // Verify if a valid path has been introduced
                if (DirectoryExists(state->dirPathText))
                {
                    // RL_FREE previous dirFiles (reloaded by ReadDirectoryFiles())
                    FD_RELOAD_DIRPATH(state);

                    strcpy(state->dirPathTextCopy, state->dirPathText);
                }
                else strcpy(state->dirPathText, state->dirPathTextCopy);
            }

            state->dirPathEditMode = !state->dirPathEditMode;
        }

        int prevTextAlignment = GuiGetStyle(LISTVIEW, TEXT_ALIGNMENT);
        int prevElementsHeight = GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT);
        GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_LEFT);
        GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, 24);

        // TODO: ListViewElements should be aligned left
# if defined(USE_CUSTOM_LISTVIEW_FILEINFO)
        FileInfo fileInfo;
        state->filesListActive = GuiListViewFiles((Rectangle){ state->position.x + 10, state->position.y + 70, winWidth - 20, winHeight - 135 }, fileInfo, state->dirFilesCount, &state->itemFocused, &state->filesListScrollIndex, state->filesListActive);
# else
        state->filesListActive = GuiListViewEx((Rectangle){ state->position.x + 10, state->position.y + 70, winWidth - 20, winHeight - 135 }, (const char**)dirFilesIcon, state->dirFilesCount, &state->itemFocused, &state->filesListScrollIndex, state->filesListActive);
# endif
        GuiSetStyle(LISTVIEW, TEXT_ALIGNMENT, prevTextAlignment);
        GuiSetStyle(LISTVIEW, LIST_ITEMS_HEIGHT, prevElementsHeight);

        if ((state->filesListActive >= 0) && (state->filesListActive != state->prevFilesListActive))
            //&& (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_DPAD_A)))
        {
            strcpy(state->fileNameText, state->dirFiles[state->filesListActive]);

            if (DirectoryExists(TextFormat("%s/%s", state->dirPathText, state->fileNameText)))
            {
                if (TextIsEqual(state->fileNameText, "..")) strcpy(state->dirPathText, GetPrevDirectoryPath(state->dirPathText));
                else strcpy(state->dirPathText, TextFormat("%s/%s", strcmp(state->dirPathText, "/")==0 ? "" : state->dirPathText, state->fileNameText));

                strcpy(state->dirPathTextCopy, state->dirPathText);

                // RL_FREE previous dirFiles (reloaded by ReadDirectoryFiles())
                FD_RELOAD_DIRPATH(state);

                strcpy(state->dirPathTextCopy, state->dirPathText);

                state->filesListActive = -1;
                strcpy(state->fileNameText, "\0");
                strcpy(state->fileNameTextCopy, state->fileNameText);
            }

            state->prevFilesListActive = state->filesListActive;
        }

        GuiLabel((Rectangle){ state->position.x + 10, state->position.y + winHeight - 60, 68, 25 }, "File name:");

        if (GuiTextBox((Rectangle){ state->position.x + 75, state->position.y + winHeight - 60, winWidth - 200, 25 }, state->fileNameText, 128, state->fileNameEditMode))
        {
            if (*state->fileNameText)
            {
                // Verify if a valid filename has been introduced
                if (FileExists(TextFormat("%s/%s", state->dirPathText, state->fileNameText)))
                {
                    // Select filename from list view
                    for (int i = 0; i < state->dirFilesCount; i++)
                    {
                        if (TextIsEqual(state->fileNameText, state->dirFiles[i]))
                        {
                            state->filesListActive = i;
                            strcpy(state->fileNameTextCopy, state->fileNameText);
                            break;
                        }
                    }
                }
                else
                {
                    strcpy(state->fileNameText, state->fileNameTextCopy);
                }
            }

            state->fileNameEditMode = !state->fileNameEditMode;
        }

        state->fileTypeActive = GuiComboBox((Rectangle){ state->position.x + 75, state->position.y  + winHeight - 30, winWidth - 200, 25 }, "All files", state->fileTypeActive);
        GuiLabel((Rectangle){ state->position.x + 10, state->position.y + winHeight - 30, 68, 25 }, "File filter:");

        state->SelectFilePressed = GuiButton((Rectangle){ state->position.x + winWidth - 120, state->position.y + winHeight - 60, 110,
#ifdef PLATFORM_DESKTOP
            25
#else
            25+30
#endif
        }, "Select");// || IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_DPAD_A);

        if (state->SelectFilePressed) state->fileDialogActive = false;

#ifdef PLATFORM_DESKTOP
        if (GuiButton((Rectangle){ state->position.x + winWidth - 120, state->position.y + winHeight - 30, 110, 25 }, "Quit")) state->fileDialogActive = false;
#endif

        // File dialog has been closed!
        if (!state->fileDialogActive)
        {
            // RL_FREE dirFiles memory
            for (int i = 0; i < state->dirFilesCount; i++)
            {
                RL_FREE(state->dirFiles[i]);
                RL_FREE(dirFilesIcon[i]);
            }

            RL_FREE(state->dirFiles);
            RL_FREE(dirFilesIcon);

            dirFilesIcon = NULL;
            state->dirFiles = NULL;
        }
    }
}

// Read all filenames from directory (supported file types)
static inline int _file_comp(const char *d1, const char *d2, const char *dir)
{
    const bool b1 = DirectoryExists(TextFormat("%s/%s", dir, d1));
    const bool b2 = DirectoryExists(TextFormat("%s/%s", dir, d2));

    if (b1 && !b2) return -1;
    if (!b1 && b2) return 1;

    if (!FileExists(TextFormat("%s/%s", dir, d1))) return 1;
    if (!FileExists(TextFormat("%s/%s", dir, d2))) return -1;

    return strcmp(d1, d2);
}
static char **ReadDirectoryFiles(const char *dir, int *filesCount, char *filterExt)
{
    int validFilesCount = 0;
    char **validFiles = (char **)RL_MALLOC(MAX_DIRECTORY_FILES*sizeof(char *));    // Max files to read
    for (int i = 0; i < MAX_DIRECTORY_FILES; i++) validFiles[i] = (char *)RL_MALLOC(MAX_DIR_PATH_LENGTH);    // Max file name length

    int filterExtCount = 0;
    const char **extensions = GuiTextSplit(filterExt, &filterExtCount, NULL);
    bool filterExtensions = true;

    int dirFilesCount = 0;
    char **files = GetDirectoryFiles(dir, &dirFilesCount);

    // Sort files and directories: dir by name + files by name
    // https://en.wikibooks.org/wiki/Algorithm_Implementation/Sorting/Quicksort#C
    if (dirFilesCount > 1)
    {
        const int MAX = 64;
        unsigned int left = 0, stack[64], pos = 0, seed = rand(), len = dirFilesCount;

        for ( ; ; )
        {
            for (; left+1 < len; len++)    /* sort left to len-1 */
            {
                if (pos == MAX) len = stack[pos = 0];           /* stack overflow, reset */
                char *pivot = files[left+seed%(len-left)];      /* pick random pivot */
                seed = seed*69069+1;                            /* next pseudorandom number */
                stack[pos++] = len;                             /* sort right part later */

                for (unsigned int right = left-1; ; )         /* inner loop: partitioning */
                {
                    while (_file_comp(files[++right], pivot, dir) < 0);/* look for greater element */
                    while (_file_comp(pivot, files[--len], dir) < 0);  /* look for smaller element */
                    if (right >= len) break;                    /* partition point found? */
                    char *temp = files[right];
                    files[right] = files[len];                  /* the only swap */
                    files[len] = temp;
                }                                               /* partitioned, continue left part */
            }

            if (pos == 0) break;                                            /* stack empty? */
            left = len;                                         /* left to right is sorted */
            len = stack[--pos];                                 /* get next range to sort */
        }
    }

    if (TextIsEqual(extensions[0], "all")) filterExtensions = false;

    for (int i = 0; (i < dirFilesCount) && (validFilesCount < MAX_DIRECTORY_FILES); i++)
    {
        if (TextIsEqual(files[i], ".")) continue;

        if (!filterExtensions)
        {
            strncpy(validFiles[validFilesCount], files[i], MAX_DIR_PATH_LENGTH);

            // Only filter files by extensions, directories should be available
            if (DirectoryExists(TextFormat("%s/%s", dir, files[i]))) strcpy(dirFilesIcon[validFilesCount], TextFormat("#%i#%s", 1, files[i]));
            else
            {
                // TODO: Assign custom filetype icons depending on file extension (image, audio, text, video, models...)

                if (IsFileExtension(files[i], ".png")) strcpy(dirFilesIcon[validFilesCount], TextFormat("#%i#%s", 12, files[i]));
                else strcpy(dirFilesIcon[validFilesCount], TextFormat("#%i#%s", 10, files[i]));
            }

            validFilesCount++;
        }
        else
        {
            for (int j = 0; j < filterExtCount; j++)
            {
                // Check file type extensions supported
                // NOTE: We just store valid files list
                if (IsFileExtension(files[i], extensions[j]))
                {
                    // TODO: Assign custom filetype icons depending on file extension (image, audio, text, video, models...)

                    if (IsFileExtension(files[i], ".png")) strcpy(dirFilesIcon[validFilesCount], TextFormat("#%i#%s", 12, files[i]));
                    else strcpy(dirFilesIcon[validFilesCount], TextFormat("#%i#%s", 10, files[i]));

                    validFilesCount++;
                }
            }
        }
    }

    ClearDirectoryFiles();

    *filesCount = validFilesCount;
    return validFiles;
}

#if defined(USE_CUSTOM_LISTVIEW_FILEINFO)
// List View control for files info with extended parameters
static int GuiListViewFiles(Rectangle bounds, FileInfo *files, int count, int *focus, int *scrollIndex, int active)
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
