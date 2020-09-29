/*******************************************************************************************
*
*   A skin editor for raygui.
*
*   UPDATES: last updated - 29 september 2020 (v1.0)
*       v1.0 - Initial release
*    
*   HELP: 
*       - To create a new skin drag'n drop a `.png` file into the editor or if a skin is already
*         opened click [New] first to reset everything. Note that a single skin file can hold 
*         multiple skin styles. A new skin style can be added/removed or moved up/down by clicking 
*         on the button below the editor panel.
*       - To drag the view around just click and move the mouse [WHEEL] inside the editor panel.
*       - To select part of the texture left click inside the editor panel and move the mouse.
*         Note that selecting/resizing only works in [NORMAL] state, in other states the selection 
*         can only be moved
*       - Selecting the active control can be done with the combobox at the top of the editor panel
*         or with the arrows below the preview window.
*       - Right clicking inside the editor panel when a texture is available will bring a menu for
*         easily working with styles (copy/cut/paste/reset).
*       - Some controls can have multiple components (another combobox besides the active control one
*         will appear) so the process of selecting/resizing/moving the edges must be repeated for each.
*       - To change the default style for the preview just drag'n drop a `.rgs` file.
*       - To load a skin file just drag'n drop a `.skin` file, please note that if a skin is already
*         opened it will not be saved when the new skin is loaded.
*       - To save a skin click the [Save] button, when the save dialog opens input the path to the
*         skin file in the input text box. Note that an absolute path is required.
*
*   COMPILATION (Windows - MinGW):
*       gcc -o editor.exe editor.c -I../../src -lraylib -lopengl32 -lgdi32 -std=c99
*
*   COMPILATION (Linux - gcc):
*	    gcc -o editor editor.c -I../../src -lraylib -lm -std=c99
*       
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2020 Vlad Adrian (@Demizdor - https://github.com/Demizdor).
*
**********************************************************************************************/



#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
#include "../../src/raygui.h"

#define GUI_PROPERTY_LIST_IMPLEMENTATION
#include "../property_list/dm_property_list.h"

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again

#ifdef __unix__
#include <unistd.h>  // for chdir()
#endif

#define SIZEOF(A) (sizeof(A)/sizeof(A[0]))
#define PREVIEW_WIDTH 650
#define PREVIEW_HEIGHT 513



//---------------------------------------------------------------------------------------
// GLOBAL VARIABLES
//---------------------------------------------------------------------------------------
enum {
    EVENT_NONE = 0,
    EVENT_DRAG_VIEW,
    EVENT_SELECT,
    EVENT_SEL_MOVE,
    EVENT_SEL_RESIZE_TOPLEFT,
    EVENT_SEL_RESIZE_TOPRIGHT,
    EVENT_SEL_RESIZE_BOTTOMLEFT,
    EVENT_SEL_RESIZE_BOTTOMRIGHT,
    EVENT_MOVE_TOP_EDGE,
    EVENT_MOVE_BOTTOM_EDGE,
    EVENT_MOVE_LEFT_EDGE,
    EVENT_MOVE_RIGHT_EDGE,
} Event = EVENT_NONE; // enum of all possible events

int ActiveControl = 0;
int ActiveStyle = 0;
int ActiveState = 0;
int ActiveSkin = 0;
float Zoom = 1.0f;
Rectangle EditAreaBounds, PanelRec;
Vector2 Scroll = {0.0f, 0.0f},
        LastMouse = {0.0f, 0.0f}, // holds the last mouse position when an event was triggered
        InitialPos = {0.0f, 0.0f}, // holds the initial position of whatever is to be moved when an event was triggered
        InitialDim = {0.0f, 0.0f};
bool IsPanelActive = true;
bool IsCustomStyleLoaded = false;
bool ShowFullPreview = false;

struct {
    GuiSkinStyle data;
    int type;
    bool full;
} Clipboard = {0}; // holds the clipboard data

Texture CheckerTexture = {0};
Texture SkinTexture = {0};
char SkinTextureName[128] = {0};
Font StyleFont = {0}; // backup of the custom font if set
unsigned int GuiStyle[NUM_CONTROLS*(NUM_PROPS_DEFAULT + NUM_PROPS_EXTENDED)] = { 0 }; // backup of a custom style if set
int ActiveProp = 0;

// style properties shown for normal state
GuiDMProperty SkinStyleProp[] = {
    PRECT("Bounds", 0, 0, 0, 0, 0),
    PINT_RANGE("Margins", 0, 0, 1, 0, 256),
    PSELECT("Type", 0, "9 Slices;1 Slice", 0),
    PSELECT("Draw", 0, "Streched;Tiled;Centered", 0),
    PSECTION("Edges", 0, 4),
    PINT_RANGE("Top", 0, 0, 1, 0, 32767),
    PINT_RANGE("Bottom", 0, 0, 1, 0, 32767),
    PINT_RANGE("Left", 0, 0, 1, 0, 32767),
    PINT_RANGE("Right", 0, 0, 1, 0, 32767),
};

// style properties shown when state is FOCUS/PRESSED/DISABLED
GuiDMProperty SkinStyleProp2[] = {
    PSECTION("Position", 0, 2),
    PINT_RANGE("X", 0, 0, 1, 0, 32767),
    PINT_RANGE("Y", 0, 0, 1, 0, 32767),
};

// editor properties
GuiDMProperty EditorProp[] = {
    PCOLOR("Select", 1, 255, 0, 0, 200),
    PCOLOR("Edge", 1, 255, 0, 255, 200),
    PCOLOR("Margin", 1, 0, 0, 255, 200),
    PFLOAT_RANGE("Alpha", 0, 0.95f, 0.1f, 2, 0.0f, 1.0f),
    PBOOL("Grid", 0, true),
    PBOOL("Use Style", 0, false),
    PINT_RANGE("Resizers", 0, 5, 1, 2, 10),
    PINT_RANGE("Preview Margin", 0, 5, 1, 4, 20),
};

#define MAX_SKIN_SLOTS 32   // max number of skin slots
GuiSkin* Slots;     // slot array each holding a skin. Allocated once and only once at the start of the app.
int SlotsCount;     // total number of slots that are occupied
char* SkinNames[MAX_SKIN_SLOTS] = {0};   // array holding the skin names
//---------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------
// FORWARD FUNCTION DECLARATIONS
//---------------------------------------------------------------------------------------
void EditorUpdate(void);
void EditorDraw(void);
void DrawPreviewAll(Rectangle bounds);

void SkinSlotsCreate();
void SkinSlotsDestroy();
void SkinSlotsPush(GuiSkin* s);         // Create a new slot at end of slot array and put skin `s` into it
void SkinSlotsPop(int idx);             // Remove slot at index `idx`
int SkinSlotsMoveUp(int idx);           // Move slot up by one
int SkinSlotsMoveDown(int idx);         // Move slot down by one
GuiSkinStyle* GetActiveSkinStyle();

// Custom widgets
bool DropdownWidget(Rectangle bounds, int listHeight, const char** text, int count, int* active, bool editMode);   // A more `advanced` dropdown box
bool ZoomWidget(Rectangle bounds, const char** text, int count, int* active);
void SkinSelectWidget(Rectangle bounds, int listHeight, int* active, bool* toggle);  // Returns true when the skin list is shown, false otherwise.
void PreviewWidget(Rectangle bounds, GuiSkinStyle* style);
void StylePropWidget(Rectangle bounds, GuiSkinStyle* style);
//---------------------------------------------------------------------------------------




int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    SetConfigFlags(FLAG_WINDOW_RESIZABLE|FLAG_VSYNC_HINT);
    InitWindow(890, 680, "raygui - skin editor (v1.0)");
    SetWindowMinSize(700, 615);
    
    // Load default style
    GuiLoadStyleDefault();
    
    SkinSlotsCreate(); // allocate memory for the skins slots
    
    // Generate checker texture
    Image tmp = GenImageChecked(16, 16, 4, 4, GRAY, LIGHTGRAY);
    CheckerTexture = LoadTextureFromImage(tmp);
    UnloadImage(tmp);
    
#ifdef __unix__
    chdir("/"); // FIXME: Hack to make texture/font files load sucessfully on linux in GuiLoadStyle() / GuiLoadSkin()
#endif
    //---------------------------------------------------------------------------------------
    
    // Main loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        EditorUpdate();
        //----------------------------------------------------------------------------------
        
        
        // Draw
        //----------------------------------------------------------------------------------
		BeginDrawing();
            ClearBackground(RAYWHITE);
            EditorDraw();
        EndDrawing();
        //----------------------------------------------------------------------------------
    }
    
    // De-Initialization
    //--------------------------------------------------------------------------------------
    SkinSlotsDestroy();
    UnloadTexture(CheckerTexture);
    CloseWindow();          // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}



//---------------------------------------------------------------------------------------
// PRIMARY FUNCTIONS FOR DRAWING/UPDATING THE EDITOR
//---------------------------------------------------------------------------------------
// Helper function: Get mouse coordinates inside the scroll panel area
static inline Vector2 MouseCoords(Vector2 mouse) {
    return RAYGUI_CLITERAL(Vector2){floorf((mouse.x - Scroll.x - PanelRec.x)/Zoom), floorf((mouse.y - Scroll.y - PanelRec.y)/Zoom)};
}

// Most events are handled here
void EditorUpdate() 
{
    // HANDLE DROPPED FILES
    if(IsFileDropped()) {
        int count = 0;
        char** files = GetDroppedFiles(&count);
        for(int i=0; i<count; ++i)
        {
            if(strncmp(GetExtension(files[i]), "png", 3) == 0)
            {
                // Load a texture
                Texture tmp = LoadTexture(files[i]);
                if(tmp.id > 0)
                {
                    if(SkinTexture.id > 0) UnloadTexture(SkinTexture); // Unload old texture
                    SkinTexture = tmp;
                    strncpy(SkinTextureName, GetFileNameWithoutExt(files[i]), 127);
                }
                break;
            }
            else if(strncmp(GetExtension(files[i]), "rgs", 3) == 0)
            {
                if(IsCustomStyleLoaded)
                    UnloadFont(StyleFont);
                // Load custom style for the preview
                GuiLoadStyle(files[i]); // load new style
                EditorProp[5].value.vbool = true; // use a custom style
                IsCustomStyleLoaded = true;
                memcpy(GuiStyle, guiStyle, SIZEOF(guiStyle)*sizeof(guiStyle[0])); // save new style data
                StyleFont = GuiGetFont(); // save the new font
                GuiLoadStyleDefault(); // load the default style again
                break;
            }
            else if(strncmp(GetExtension(files[i]), "skin", 4) == 0)
            {
                UnloadTexture(SkinTexture);
                
                // Load the skin file
                SlotsCount = GuiLoadSkin(files[i], Slots, MAX_SKIN_SLOTS);
                
                // Get the new skin texture
                SkinTexture = GuiGetSkinTexture();
                
                if(SlotsCount == 0) {
                    TraceLog(LOG_WARNING, TextFormat("Failed to load skin file `%s`", files[i]));
                    
                    // Set the default skin
                    GuiSkin style0 = {0};
                    style0.useColor = true;
                    strcpy(style0.name, "default");
                    SkinSlotsPush(&style0);
                } else {
                    // Fill the skin names array
                    for(int i=0; i<SlotsCount; ++i) {
                        SkinNames[i] = (char*)&Slots[i].name;
                    }
                    
                    TraceLog(LOG_INFO, TextFormat("Sucessfully loaded `%i` skin style(s) from file `%s`.", SlotsCount, files[i]));
                }
                break;
            }
        }
        ClearDroppedFiles();
    }
    
    // HANDLE EVENTS
    if(IsPanelActive && !ShowFullPreview)
    {
        Vector2 mouse = GetMousePosition();
        GuiSkinStyle* style = GetActiveSkinStyle();
        
        switch(Event)
        {
            case EVENT_NONE:
            if(CheckCollisionPointRec(mouse, PanelRec)) // are we inside the panel?
            {
                if(IsMouseButtonPressed(MOUSE_MIDDLE_BUTTON)) 
                { 
                    Event = EVENT_DRAG_VIEW; 
                    LastMouse = mouse;
                    InitialPos = Scroll; 
                }
                else if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && SkinTexture.id > 0 && SlotsCount > 0) 
                {
                    if(style != NULL) 
                    {
                        int x[4] = { style->nx, style->fx, style->px, style->dx };
                        int y[4] = { style->ny, style->fy, style->py, style->dy };
                        LastMouse = MouseCoords(mouse);
                        Rectangle r = {Scroll.x + PanelRec.x + x[ActiveState]*Zoom, Scroll.y + PanelRec.y + y[ActiveState]*Zoom, style->width*Zoom, style->height*Zoom};
                        
                        // Same calculations are done when drawing the selection lines
                        const int lineScale = 1 + Zoom/2;
                        Rectangle linesRec = {r.x - lineScale, r.y - lineScale, r.width + 2*lineScale, r.height+2*lineScale};
                        const int helper = EditorProp[6].value.vint.val*lineScale;
                        
                        // Calculate bounds for the resizers
                        Rectangle tl = {linesRec.x - helper, linesRec.y - helper, helper, helper}; // Top Left
                        Rectangle tr = {linesRec.x + linesRec.width, linesRec.y - helper, helper, helper}; //Top Right
                        Rectangle bl = {linesRec.x - helper, linesRec.y + linesRec.height, helper, helper}; // Bottom Left
                        Rectangle br = {linesRec.x + linesRec.width, linesRec.y + linesRec.height, helper, helper}; // Bottom Right
                        
                        // Calculate bounds for the edges
                        Rectangle te = { linesRec.x + lineScale, tl.y + style->top*Zoom + helper/2, style->width*Zoom, helper }; // Top Edge
                        Rectangle be = { linesRec.x + lineScale, bl.y - style->bottom*Zoom - helper/2, style->width*Zoom, helper }; // Bottom Edge
                        Rectangle le = { bl.x + style->left*Zoom + helper/2, linesRec.y + lineScale, helper, style->height*Zoom }; // Left Edge
                        Rectangle re = { br.x - style->right*Zoom - helper/2, linesRec.y + lineScale, helper, style->height*Zoom }; // Right Edge
                        
                        if(ActiveState == GUI_STATE_NORMAL) {
                            if(CheckCollisionPointRec(mouse, tl)) {
                                Event = EVENT_SEL_RESIZE_TOPLEFT;
                                InitialPos.x = x[ActiveState];
                                InitialPos.y = y[ActiveState];
                                InitialDim = (Vector2){style->width, style->height};
                                LastMouse = mouse;
                            }
                            else if(CheckCollisionPointRec(mouse, tr)) {
                                Event = EVENT_SEL_RESIZE_TOPRIGHT;
                                InitialPos.y = y[ActiveState];
                                InitialDim = (Vector2){style->width, style->height};
                                LastMouse = mouse;
                            }
                            else if(CheckCollisionPointRec(mouse, bl)) {
                                Event = EVENT_SEL_RESIZE_BOTTOMLEFT;
                                InitialPos.x = x[ActiveState];
                                InitialDim = (Vector2){style->width, style->height};
                                LastMouse = mouse;
                            }
                            else if(CheckCollisionPointRec(mouse, br)) {
                                Event = EVENT_SEL_RESIZE_BOTTOMRIGHT;
                                InitialDim = (Vector2){style->width, style->height};
                                LastMouse = mouse;
                            }
                            else if(CheckCollisionPointRec(mouse, te)) { // move the top edge
                                Event = EVENT_MOVE_TOP_EDGE;
                                InitialPos.x = style->top;
                                LastMouse = mouse;
                            }
                            else if(CheckCollisionPointRec(mouse, be)) { // move the bottom edge
                                Event = EVENT_MOVE_BOTTOM_EDGE;
                                InitialPos.x = style->bottom;
                                LastMouse = mouse;
                            }
                            else if(CheckCollisionPointRec(mouse, le)) { // move the left edge
                                Event = EVENT_MOVE_LEFT_EDGE;
                                InitialPos.x = style->left;
                                LastMouse = mouse;
                            }
                            else if(CheckCollisionPointRec(mouse, re)) { // move the right edge
                                Event = EVENT_MOVE_RIGHT_EDGE;
                                InitialPos.x = style->right;
                                LastMouse = mouse;
                            }
                            else if(CheckCollisionPointRec(mouse, r)) { // move selection when mouse is clicked inside it
                                Event = EVENT_SEL_MOVE;
                                InitialPos.x = x[ActiveState];
                                InitialPos.y = y[ActiveState];
                                LastMouse = mouse;
                            }
                            else if(style->width == 0 || style->height == 0) { 
                                Event = EVENT_SELECT;
                                InitialPos.x = -1.0f;
                                style->top = style->bottom = style->left = style->right = 0;
                            }
                        }
                        else if(CheckCollisionPointRec(mouse, r)) { // move selection when mouse is clicked inside it
                                Event = EVENT_SEL_MOVE;
                                InitialPos.x = x[ActiveState];
                                InitialPos.y = y[ActiveState];
                                LastMouse = mouse;
                        }
                    }
                }
            }
            break;
            
            case EVENT_DRAG_VIEW: 
                if(IsMouseButtonDown(MOUSE_MIDDLE_BUTTON)) {
                    Scroll.x = InitialPos.x + (LastMouse.x - mouse.x)*Zoom;
                    Scroll.y = InitialPos.y + (LastMouse.y - mouse.y)*Zoom;
                } else Event = EVENT_NONE;
            break;
            
            case EVENT_SELECT:
                if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    unsigned short* x[4] = { &style->nx, &style->fx, &style->px, &style->dx };
                    unsigned short* y[4] = { &style->ny, &style->fy, &style->py, &style->dy };
                    Vector2 m = MouseCoords(mouse);
                    if(m.x < LastMouse.x) { *x[ActiveState] = m.x; style->width = LastMouse.x - m.x; }
                    else { *x[ActiveState] = LastMouse.x; style->width = m.x - LastMouse.x; }
                    
                    if(m.y < LastMouse.y) { *y[ActiveState] = m.y; style->height = LastMouse.y - m.y; }
                    else { *y[ActiveState] = LastMouse.y; style->height = m.y - LastMouse.y; }
                    
                    if(InitialPos.x < 0.0f && ActiveState == GUI_STATE_NORMAL) {
                        *x[1] = *x[2] = *x[3] = *x[0];
                        *y[1] = *y[2] = *y[3] = *y[0];
                    }
                } else Event = EVENT_NONE;
            break;
            
            case EVENT_SEL_MOVE:
                if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    unsigned short* x[4] = { &style->nx, &style->fx, &style->px, &style->dx };
                    unsigned short* y[4] = { &style->ny, &style->fy, &style->py, &style->dy };
                    int tmp = InitialPos.x + (mouse.x - LastMouse.x)/Zoom;
                    *x[ActiveState] = tmp < 0 ? 0 : tmp;
                    tmp = InitialPos.y + (mouse.y - LastMouse.y)/Zoom;
                    *y[ActiveState] = tmp < 0 ? 0 : tmp;
                } else Event = EVENT_NONE;
            break;
            
            case EVENT_SEL_RESIZE_TOPLEFT:
                if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    unsigned short* x[4] = { &style->nx, &style->fx, &style->px, &style->dx };
                    unsigned short* y[4] = { &style->ny, &style->fy, &style->py, &style->dy };
                    // adjust x coord and width
                    int diff = (LastMouse.x - mouse.x)/Zoom;
                    int px = InitialPos.x - diff;
                    if(px >= 0) {
                        int w = InitialDim.x + diff;
                        if(w > 0) {
                            style->width = w;
                            *x[ActiveState] = px;
                            // check if the left/right edges overflow
                            if(style->left + style->right > style->width) style->left = 0;
                            if(style->right > style->width) style->right = 0;
                        }
                    }
                    // adjust y coord and height
                    diff = (LastMouse.y - mouse.y)/Zoom;
                    int py = InitialPos.y - diff;
                    if(py >= 0) { 
                        int h = InitialDim.y + diff;
                        if(h > 0) {
                            style->height = h;
                            *y[ActiveState] = py;
                            // check if the top/bottom edges overflow
                            if(style->top + style->bottom > style->height) style->top = 0;
                            if(style->bottom > style->height) style->bottom = 0;
                        }
                    }
                    // check for margin overflow
                    if(2*style->margins >= style->width-1) style->margins = style->width/2;
                    if(2*style->margins >= style->height-1) style->margins = style->height/2;
                } else Event = EVENT_NONE;
            break;
            
            case EVENT_SEL_RESIZE_TOPRIGHT:
                if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    unsigned short* y[4] = { &style->ny, &style->fy, &style->py, &style->dy };
                    // adjust width
                    int diff = (LastMouse.x - mouse.x)/Zoom;
                    int w = InitialDim.x - diff;
                    if(w > 0) { 
                        style->width = w;
                        // check if the right/left edges overflow
                        if(style->left + style->right > style->width) style->right = 0;
                        if(style->left > style->width) style->left = 0;
                    }
                    // adjust y coord and height
                    diff = (LastMouse.y - mouse.y)/Zoom;
                    int py = InitialPos.y - diff;
                    if(py >= 0) { 
                        int h = InitialDim.y + diff;
                        if(h > 0) {
                            style->height = h;
                            *y[ActiveState] = py;
                            // check if the top/bottom edges overflow
                            if(style->top + style->bottom > style->height) style->top = 0;
                            if(style->bottom > style->height) style->bottom = 0;
                        }
                    }
                    // check for margin overflow
                    if(2*style->margins >= style->width-1) style->margins = style->width/2;
                    if(2*style->margins >= style->height-1) style->margins = style->height/2;
                } else Event = EVENT_NONE;
            break;
            
            case EVENT_SEL_RESIZE_BOTTOMLEFT:
                if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    unsigned short* x[4] = { &style->nx, &style->fx, &style->px, &style->dx };
                    // adjust x coord and width
                    int diff = (LastMouse.x - mouse.x)/Zoom;
                    int px = InitialPos.x - diff;
                    if(px >= 0) {
                        int w = InitialDim.x + diff;
                        if(w > 0) { 
                            style->width = w;
                            *x[ActiveState] = px;
                            // check if the left/right edges overflow
                            if(style->left + style->right > style->width) style->left = 0;
                            if(style->right > style->width) style->right = 0;
                        }
                    }
                    // adjust height
                    diff = (LastMouse.y - mouse.y)/Zoom;
                    int h = InitialDim.y - diff;
                    if(h > 0) {
                        style->height = h;
                            // check if the bottom/top edges overflow
                            if(style->top + style->bottom > style->height) style->bottom = 0;
                            if(style->top > style->height) style->top = 0;
                    }
                    // check for margin overflow
                    if(2*style->margins >= style->width-1) style->margins = style->width/2;
                    if(2*style->margins >= style->height-1) style->margins = style->height/2;
                } else Event = EVENT_NONE;
            break;
            
            case EVENT_SEL_RESIZE_BOTTOMRIGHT:
                if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    // adjust width
                    int diff = (LastMouse.x - mouse.x)/Zoom;
                    int w = InitialDim.x - diff;
                    if(w > 0) { 
                        style->width = w; 
                        // check if the right/left edges overflow
                        if(style->left + style->right > style->width) style->right = 0;
                        if(style->left > style->width) style->left = 0;
                    }
                    // adjust height
                    diff = (LastMouse.y - mouse.y)/Zoom;
                    int h = InitialDim.y - diff;
                    if(h > 0) { 
                        style->height = h; 
                        // check if the bottom/top edges overflow
                        if(style->top + style->bottom > style->height) style->bottom = 0;
                        if(style->top > style->height) style->top = 0;
                    }
                    // check for margin overflow
                    if(2*style->margins >= style->width-1) style->margins = style->width/2;
                    if(2*style->margins >= style->height-1) style->margins = style->height/2;
                } else Event = EVENT_NONE;
            break;
            
            case EVENT_MOVE_TOP_EDGE:
                if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    int diff = (LastMouse.y - mouse.y)/Zoom;
                    int t = InitialPos.x - diff;
                    if( t >= 0 && t + style->bottom < style->height) style->top = t;
                } else Event = EVENT_NONE;
            break;
            
            case EVENT_MOVE_BOTTOM_EDGE:
                if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    int diff = (LastMouse.y - mouse.y)/Zoom;
                    int b = InitialPos.x + diff;
                    if( b >= 0 && b + style->top < style->height) style->bottom = b;
                } else Event = EVENT_NONE;
            break;
            
            case EVENT_MOVE_LEFT_EDGE:
                if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    int diff = (LastMouse.x - mouse.x)/Zoom;
                    int l = InitialPos.x - diff;
                    if( l >= 0 && l + style->right < style->width) style->left = l;
                } else Event = EVENT_NONE;
            break;
            
            case EVENT_MOVE_RIGHT_EDGE:
                if(IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
                    int diff = (LastMouse.x - mouse.x)/Zoom;
                    int r = InitialPos.x + diff;
                    if( r >= 0 && r + style->left < style->width) style->right = r;
                } else Event = EVENT_NONE;
            break;
        }
    }
}

// Draw the whole GUI, used once and only once
static inline void DrawGUI()
{
    int width = GetScreenWidth(), height = GetScreenHeight();
    Vector2 mouse = GetMousePosition();
    
    static bool showInputBox = false;
    static bool menu = false;
    
    if(showInputBox) GuiLock();
    
    ActiveProp = GuiToggleGroup(RAYGUI_CLITERAL(Rectangle){width - 10 - 138, height - 58 - 8, 70, 30}, "#198# Style;#198# Editor", ActiveProp);
    ShowFullPreview = GuiToggle(RAYGUI_CLITERAL(Rectangle){width - 10 - 195, height - 58 - 8, 30, 30}, "#25#", ShowFullPreview);
    
    
    // control names...in sync with GuiSkinStyleId
    const char* controls[] = {
        "BUTTON",
        "IMGBUTTON",
        "PANEL",
        "STATUS",
        "WINDOW",
        "SCROLLPANEL",
        "LIST",
        "SCROLLBAR",
        "TOGGLE",
        "TOGGLEGROUP",
        "CHECKBOX",
        "COMBOBOX",
        "DROPBOX",
        "SPINNER",
        "SLIDER",
        "PROGRESSBAR",
        "VALUEBOX",
        "TEXTBOX",
        "TEXTBOXMULTI",
        "COLORPANEL",
        "COLORBARALPHA",
        "COLORBARHUE",
        "TOOLTIP",
    };
    
    // CONTROL LIST
    GuiLabel( RAYGUI_CLITERAL(Rectangle){10, 0, 170, 30}, "Widget");
    static bool editWidget = false;
    const int listHeight = height*0.7;
    if(DropdownWidget(RAYGUI_CLITERAL(Rectangle){10, 23, 170, 30}, (listHeight/GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT))*GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT)+GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING), controls, SIZEOF(controls), &ActiveControl, editWidget)) {
        editWidget = !editWidget;
        ActiveStyle = 0;
        if(ActiveControl < 0) ActiveControl = 0;
    }
    
    bool locked = CheckCollisionPointRec(mouse, EditAreaBounds);
    if(locked || menu) GuiLock();
    if(GuiLabelButton(RAYGUI_CLITERAL(Rectangle){width-205, 262, 20, 20}, "#114#")) ActiveControl -= 1; // `<` arrow
    if(GuiLabelButton(RAYGUI_CLITERAL(Rectangle){width-25, 262, 20, 20}, "#115#")) ActiveControl += 1;  // `>` arrow
    int lblStyle = GuiGetStyle(LABEL, TEXT_ALIGNMENT);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    GuiLabel(RAYGUI_CLITERAL(Rectangle){width-205, 262, 200, 20}, controls[ActiveControl]);
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, lblStyle);
    if(ActiveControl < 0) ActiveControl = GUI_SKIN_TOOLTIP_SID;
    if(ActiveControl > GUI_SKIN_TOOLTIP_SID) ActiveControl = 0;
    
    GuiSkinStyle* st = GetActiveSkinStyle();
    // PREVIEW WIDGET
    PreviewWidget(RAYGUI_CLITERAL(Rectangle){width-205, 60, 200, 200}, st);
    
    // SKIN STYLE PROPERTIES
    if(!locked) {
        locked = !CheckCollisionPointRec(mouse, RAYGUI_CLITERAL(Rectangle){width-205, 285, 200, height - 285-2*30-15});
        if(locked) GuiLock();
    }
    StylePropWidget(RAYGUI_CLITERAL(Rectangle){width-205, 285, 200, height - 285-2*30-15}, st);
    if(locked || menu) GuiUnlock();
    
    struct {
        int count;
        const char* name[8];
    }  const style[SIZEOF(controls)] = { {0, {0}}, {0, {0}}, {0, {0}}, {0, {0}}, // BUTTON, IMGBUTTON, PANEL, STATUS
        {3, {"BODY", "TITLEBAR", "BUTTON"}}, // WINDOW
        {8, {"BODY", "CORNER", "HSCROLL BODY", "HSCROLL RAIL", "HSCROLL SLIDER", "VSCROLL BODY", "VSCROLL RAIL", "VSCROLL SLIDER"}}, // SCROLLPANEL
        {7, {"BODY", "ITEM", "FIRST ITEM", "LAST ITEM", "SCROLLBAR BODY", "SCROLLBAR RAIL", "SCROLLBAR SLIDER"}}, // LIST
        {3, {"BODY", "RAIL", "SLIDER"}}, // SCROLLBAR
        {2, {"BODY", "ON"}}, // TOGGLE
        {6, {"BODY", "ON", "FIRST", "FIRST ON", "LAST", "LAST ON"}}, // TOGGLEGROUP
        {2, {"BODY", "CHECKED"}}, // CHECKBOX
        {2, {"BODY", "BUTTON"}}, // COMBOBOX
        {3, {"BODY", "LIST", "ITEM"}}, // DROPBOX
        {3, {"VALUEBOX", "LEFT BUTTON", "RIGHT BUTTON"}}, // SPINNER
        {3, {"BODY", "SLIDER", "BAR"}}, // SLIDER
        {2, {"BODY", "BAR"}}, // PROGRESSBAR
        {0, {0}}, {0, {0}}, {0, {0}}, // VALUEBOX, TEXTBOX, TEXTBOXMULTI
        {2, {"BODY", "SELECTOR"}}, // COLORPANEL
        {2, {"BODY", "SELECTOR"}}, // COLORBARALPHA
        {2, {"BODY", "SELECTOR"}}, // COLORBARHUE
        {0, {0}} // TOOLTIP
    };
    
    // STYLE LIST
    static bool styleWidget = false;
    if(style[ActiveControl].count) {
        GuiLabel(RAYGUI_CLITERAL(Rectangle){198, 0, 170, 30}, "Style");
        GuiLabel(RAYGUI_CLITERAL(Rectangle){180, 23, 30, 30}, "#115# "); // Draw a `>` arrow
        if(DropdownWidget(RAYGUI_CLITERAL(Rectangle){198, 23, 170, 30}, 2*GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING) + style[ActiveControl].count*(GuiGetStyle(LISTVIEW, LIST_ITEMS_HEIGHT) + GuiGetStyle(LISTVIEW, LIST_ITEMS_PADDING)), (const char**)style[ActiveControl].name, style[ActiveControl].count, &ActiveStyle, styleWidget))
            styleWidget = !styleWidget;
    }
    
    // ZOOM
    const char* zoomText[] = {"#43#50%", "#43#100%", "#43#200%", "#43#400%", "#43#800%"};
    float zoomLevel[] = {0.5f, 1.0f, 2.0f, 4.0f, 8.0f}; // in sync with text values above
    static int zoomActive = 1;
    int rightX = width - 100; // align to right of screen
    GuiLabel(RAYGUI_CLITERAL(Rectangle){rightX, 0, 90, 30}, "Zoom");
    bool pressed = ZoomWidget(RAYGUI_CLITERAL(Rectangle){rightX, 23, 90, 30}, zoomText, SIZEOF(zoomText), &zoomActive);
    if(pressed) 
    {
        int w =  SkinTexture.width;
        int h = SkinTexture.height;
        if(ShowFullPreview) {
            w = PREVIEW_WIDTH;
            h = PREVIEW_HEIGHT;
        }
        
        // when zoomed in make sure the scroll doesn't change
        float diff = (Scroll.x)/(w*Zoom);
        Scroll.x = (diff*w*zoomLevel[zoomActive]);
        diff = (Scroll.y)/(h*Zoom);
        Scroll.y = (diff*h*zoomLevel[zoomActive]);
    }
    Zoom = zoomLevel[zoomActive];
    
    // STATE TOGGLE
    rightX -= 20 + 100;
    static bool stateWidget = false;
    GuiLabel(RAYGUI_CLITERAL(Rectangle){rightX, 0, 100, 30}, "State");
    if(GuiDropdownBox(RAYGUI_CLITERAL(Rectangle){rightX, 23, 100, 30}, "NORMAL;FOCUSED;PRESSED;DISABLED", &ActiveState, stateWidget))
        stateWidget = !stateWidget;
        
    
    // SKIN LIST WIDGET
    static bool skinselWidget = false;
    SkinSelectWidget(RAYGUI_CLITERAL(Rectangle){ (width - 200)/2, height - 58 - 8, 200, 30}, 200, &ActiveSkin, &skinselWidget);
    
    if(GuiToggle(RAYGUI_CLITERAL(Rectangle){ 10, height - 58 - 8, 70, 30}, "#8# New", false)) {
        // reset skin slots
        SlotsCount = 0;
        // add a default skin
        GuiSkin style0 = {0};
        style0.useColor = true;
        strcpy(style0.name, "default");
        SkinSlotsPush(&style0);
        // reset skin texture
        SkinTextureName[0] = '\0';
        UnloadTexture(SkinTexture);
        SkinTexture = RAYGUI_CLITERAL(Texture){0};
    }
    if (GuiToggle(RAYGUI_CLITERAL(Rectangle){ 10+70+3, height - 58 - 8, 70, 30}, "#6# Save", false) && SkinTexture.id > 0 && !ShowFullPreview) showInputBox = true;
    
    
    // STATUSBAR
    const char* eventNames[] = {"EVENT_NONE", "EVENT_DRAG_VIEW", "EVENT_SELECT", "EVENT_SEL_MOVE", 
        "EVENT_SEL_RESIZE_TOPLEFT", "EVENT_SEL_RESIZE_TOPRIGHT", "EVENT_SEL_RESIZE_BOTTOMLEFT", "EVENT_SEL_RESIZE_BOTTOMRIGHT", 
        "EVENT_MOVE_TOP_EDGE", "EVENT_MOVE_BOTTOM_EDGE", "EVENT_MOVE_LEFT_EDGE", "EVENT_MOVE_RIGHT_EDGE"};
    GuiStatusBar(RAYGUI_CLITERAL(Rectangle){-1, height - 26, width + 2, 26}, TextFormat("%s | %i/%i styles | FPS:%3i", eventNames[Event], SlotsCount, MAX_SKIN_SLOTS, GetFPS()));
    
    IsPanelActive = !(editWidget || stateWidget || styleWidget || skinselWidget || showInputBox); // check if any of the widgets are active and deactivate the panel
    
    // RIGHT CLICK MENU
    static Rectangle menuRec = {0, 0, 120, 140};
    if(SkinTexture.id > 0 && !menu && IsPanelActive && !ShowFullPreview && locked && IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
        // Show menu at mouse location
        menu = true;
        IsPanelActive = false;
        menuRec.x = mouse.x;
        menuRec.y = mouse.y;
    }
    
    if(menu) 
    {
        IsPanelActive = false;
        // Check if menu should still be shown
        if((IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) && !CheckCollisionPointRec(mouse, menuRec)) menu = false;
        
        if(menu)
        {
            // Draw the menu
            int tmp = GuiListView(menuRec, "#17#Cut;#16#Copy;#18#Paste;Reset", NULL, -1);
            if(tmp != -1) {
                menu = false;
                
                // Handle menu press (NOTE: we don't touch the OS clipboard)
                switch(tmp) 
                {
                    case 0: // CUT
                    {
                        Clipboard.full = true;
                        Clipboard.type = ActiveState;
                        Clipboard.data = *st;
                        if(ActiveState == GUI_STATE_NORMAL) *st = RAYGUI_CLITERAL(GuiSkinStyle){0};
                        else if(ActiveState == GUI_STATE_FOCUSED) st->fx = st->fy = 0;
                        else if(ActiveState == GUI_STATE_PRESSED) st->px = st->py = 0;
                        else st->dx = st->dy = 0;
                    }   
                    break;
                    
                    case 1: // COPY
                    {
                        Clipboard.full = true;
                        Clipboard.type = ActiveState;
                        Clipboard.data = *st;
                    }
                    break;
                    
                    case 2: // PASTE
                    {
                        if(Clipboard.full) {
                            unsigned short* x[4] = {&st->nx, &st->fx, &st->px, &st->dx};
                            unsigned short* y[4] = {&st->ny, &st->fy, &st->py, &st->dy};
                            if(Clipboard.type == GUI_STATE_NORMAL) { 
                                if(ActiveState == GUI_STATE_NORMAL) *st = Clipboard.data;
                                else if(ActiveState == GUI_STATE_FOCUSED) { st->fx = Clipboard.data.nx; st->fy = Clipboard.data.ny; }
                                else if(ActiveState == GUI_STATE_PRESSED) { st->px = Clipboard.data.nx; st->py = Clipboard.data.ny; }
                                else { st->dx = Clipboard.data.nx; st->dy = Clipboard.data.ny; }
                            } else if (Clipboard.type == GUI_STATE_FOCUSED) {
                                *x[ActiveState] = Clipboard.data.fx;
                                *y[ActiveState] = Clipboard.data.fy;
                            } else if(Clipboard.type == GUI_STATE_PRESSED) {
                                *x[ActiveState] = Clipboard.data.px;
                                *y[ActiveState] = Clipboard.data.py;
                            } else {
                                *x[ActiveState] = Clipboard.data.dx;
                                *y[ActiveState] = Clipboard.data.dy;
                            }
                            
                            if(st->width == 0 || st->height == 0) {
                                st->width = Clipboard.data.width;
                                st->height = Clipboard.data.height;
                            }
                        }
                    }
                    break;
                    
                    case 3: // RESET
                        *st = RAYGUI_CLITERAL(GuiSkinStyle){0};
                    break;
                }
            }
        }
    }
         
    if(showInputBox) 
    {
        GuiUnlock();
        int btn = GuiTextInputBox(RAYGUI_CLITERAL(Rectangle){(width-200)/2, (height-140)/2, 200, 140}, "Save Skin", "Input skin path", "OK;Cancel", SkinTextureName);
        if( btn == 1) {
            // Save texture
            const char* tPath = TextFormat("%s/%s.png", GetDirectoryPath(SkinTextureName), GetFileNameWithoutExt(SkinTextureName));
            ExportImage(GetTextureData(SkinTexture), tPath);
            
            // Save skin to file
            const char* sPath = TextFormat("%s/%s.skin", GetDirectoryPath(SkinTextureName), GetFileNameWithoutExt(SkinTextureName));
            if(!GuiSaveSkin(sPath, GetFileName(tPath), Slots, SlotsCount)) {
                TraceLog(LOG_ERROR, TextFormat("Failed to save skin to `%s`", sPath));
            } else {
                TraceLog(LOG_INFO, "Skin saved successfully!");
            }
        } 
        
        if(btn != -1) showInputBox = false;
    }
}

// Draws all controls with currently selected skin when preview all is enabled
void DrawPreviewAll(Rectangle bounds) 
{
    // Switch to skin
    GuiSetSkinTexture(SkinTexture);
    GuiSetSkin(&Slots[ActiveSkin]);
    
    // Switch the style
    if(IsCustomStyleLoaded && EditorProp[5].value.vbool) { // use a custom style or the default one
        memcpy(guiStyle, GuiStyle, SIZEOF(guiStyle)*sizeof(guiStyle[0]));
        GuiSetFont(StyleFont);
    }
    
    ClearBackground( GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)) );
    if(EditorProp[4].value.vbool) GuiGrid(RAYGUI_CLITERAL(Rectangle){0,0,bounds.width+bounds.x, bounds.height+bounds.y}, 42.0f, 3);  // Draw grid
    
    const int px = Scroll.x + PanelRec.x;
    const int py = Scroll.y + PanelRec.y;
    
    GuiSetState(ActiveState);
    Vector2 mouse = GetMousePosition();
    
    const int m = EditorProp[7].value.vint.val; // control margin padding
    const int d = 5; // min distance between controls
    
    // DRAW FULL PREVIEW CONTROLS HERE
    //--------------------------------------------------------------------
    GuiButton(RAYGUI_CLITERAL(Rectangle){px+m*Zoom, py+m*Zoom, 100*Zoom, 30*Zoom}, "Button");
    GuiImageButton(RAYGUI_CLITERAL(Rectangle){px+(100+2*m+d)*Zoom, py+m*Zoom, 100*Zoom, 30*Zoom}, " ", CheckerTexture);
    static bool active = false;
    active = GuiToggle(RAYGUI_CLITERAL(Rectangle){px+(200+3*m+2*d)*Zoom, py+m*Zoom, 100*Zoom, 30*Zoom}, "Toggle", active);
    static int act = -1;
    act = GuiComboBox(RAYGUI_CLITERAL(Rectangle){px+(300+4*m+3*d)*Zoom, py+m*Zoom, 100*Zoom, 30*Zoom}, "One;Two;Three", act);
    GuiStatusBar(RAYGUI_CLITERAL(Rectangle){px+(400+5*m+4*d)*Zoom, py+m*Zoom, 120*Zoom, 30*Zoom}, "Status bar");
    
    GuiWindowBox(RAYGUI_CLITERAL(Rectangle){px+m*Zoom, py+(30+2*m)*Zoom, 180*Zoom, 162*Zoom}, "Window");
    static int scroll = 0; static int selected = 0;
    selected = GuiListView(RAYGUI_CLITERAL(Rectangle){px+(180+2*m+d)*Zoom, py+(30+2*m)*Zoom, 180*Zoom, 162*Zoom}, "One;Two;Three;Four;Five;Six", &scroll, selected);
    static Vector2 scrl = {0.0f, 0.0f};
    GuiScrollPanel(RAYGUI_CLITERAL(Rectangle){px+(360+3*m+2*d)*Zoom, py+(30+2*m)*Zoom, 180*Zoom, 162*Zoom}, RAYGUI_CLITERAL(Rectangle){0, 0, 190*Zoom, 170*Zoom}, &scrl);
    
    static char text[32] = {0};
    GuiTextBox(RAYGUI_CLITERAL(Rectangle){px+m*Zoom, py+(192+3*m)*Zoom, 100*Zoom, 30*Zoom}, (char*)&text, 31, CheckCollisionPointRec(mouse, RAYGUI_CLITERAL(Rectangle){px+m*Zoom, py+(192+3*m)*Zoom, 100*Zoom, 30*Zoom}));
    static int val = 10;
    GuiValueBox(RAYGUI_CLITERAL(Rectangle){px+(100+2*m+d)*Zoom, py+(192+3*m)*Zoom, 100*Zoom, 30*Zoom}, NULL, &val, 0, 100, CheckCollisionPointRec(mouse, RAYGUI_CLITERAL(Rectangle){px+(100+2*m+d)*Zoom, py+(192+3*m)*Zoom, 100*Zoom, 30*Zoom}));
    GuiSpinner(RAYGUI_CLITERAL(Rectangle){px+(200+3*m+2*d)*Zoom, py+(192+3*m)*Zoom, 100*Zoom, 30*Zoom}, NULL, &val, 0, 100, CheckCollisionPointRec(mouse, RAYGUI_CLITERAL(Rectangle){px+(200+3*m+2*d)*Zoom, py+(192+3*m)*Zoom, 100*Zoom, 30*Zoom}));
    act = GuiToggleGroup(RAYGUI_CLITERAL(Rectangle){px+(300+4*m+3*d)*Zoom, py+(192+3*m)*Zoom, 60*Zoom, 30*Zoom}, "One;Two;Three", act);
    
    GuiTextBoxMulti(RAYGUI_CLITERAL(Rectangle){px+m*Zoom, py+(222+4*m)*Zoom, 160*Zoom, 120*Zoom}, (char*)&text, 31, CheckCollisionPointRec(mouse, RAYGUI_CLITERAL(Rectangle){px+m*Zoom, py+(222+4*m)*Zoom, 160*Zoom, 120*Zoom}));
    static int scr2 = 0;
    scr2 = GuiScrollBar(RAYGUI_CLITERAL(Rectangle){px+(160+2*m+d)*Zoom, py+(222+4*m)*Zoom, 20*Zoom, 120*Zoom}, scr2, 0, 100);
    scr2 = GuiScrollBar(RAYGUI_CLITERAL(Rectangle){px+(180+3*m+2*d)*Zoom, py+(222+4*m)*Zoom, 160*Zoom, 20*Zoom}, scr2, 0, 100);
    GuiPanel(RAYGUI_CLITERAL(Rectangle){px+(180+3*m+2*d)*Zoom, py+(242+4*m+d)*Zoom, 160*Zoom, 95*Zoom});
    static Color color = BLUE;
    color = GuiColorPicker(RAYGUI_CLITERAL(Rectangle){px+(340+4*m+3*d)*Zoom, py+(222+4*m)*Zoom, 200*Zoom, 120*Zoom}, color);
    
    static bool check = false;
    check = GuiCheckBox(RAYGUI_CLITERAL(Rectangle){px+m*Zoom, py+5+(342+5*m)*Zoom, 20*Zoom, 20*Zoom}, "Checkbox", check);

    static float slider = 50.0f;
    slider = GuiSlider(RAYGUI_CLITERAL(Rectangle){px+(200+3*m+2*d)*Zoom, py+(342+5*m)*Zoom, 100*Zoom, 30*Zoom}, NULL, NULL, slider, 0.0f, 100.0f);
    slider = GuiSliderBar(RAYGUI_CLITERAL(Rectangle){px+(300+4*m+3*d)*Zoom, py+(342+5*m)*Zoom, 100*Zoom, 30*Zoom}, NULL, NULL, slider, 0.0f, 100.0f);
    slider = GuiProgressBar(RAYGUI_CLITERAL(Rectangle){px+(400+5*m+4*d)*Zoom, py+(342+5*m)*Zoom, 120*Zoom, 30*Zoom}, NULL, TextFormat(" %3d%%", (int)(slider)), slider, 0.0f, 100.0f);
    
    GuiMessageBox(RAYGUI_CLITERAL(Rectangle){px+m*Zoom, py+(372+6*m)*Zoom, 180*Zoom, 140*Zoom}, "Message Box", "Could be important!", "Oh!...;Hmmm!..." );
    GuiTextInputBox(RAYGUI_CLITERAL(Rectangle){px+(180+2*m+d)*Zoom, py+(372+6*m)*Zoom, 240*Zoom, 140*Zoom}, "Text Input Box", "Write something!", "Ok, sure;Nah!", (char*)&text);
    static float alpha = 0.5f;
    alpha = GuiColorBarAlpha(RAYGUI_CLITERAL(Rectangle){px+(420+3*m+2*d)*Zoom, py+(382+6*m)*Zoom, 140*Zoom, 40*Zoom}, alpha);
    
    static bool cact = false;
    if(GuiDropdownBox(RAYGUI_CLITERAL(Rectangle){px+(100+2*m+d)*Zoom, py+(342+5*m)*Zoom, 100*Zoom, 30*Zoom}, "One;Two;Three", &act, cact)) cact = !cact;
    //--------------------------------------------------------------------
    
    GuiSetState(GUI_STATE_NORMAL);
    
    // Disable skin & load default style
    GuiSetSkin(NULL);
    GuiLoadStyleDefault();
}

// Performs drawing of the editor (calls all other drawing functions)
void EditorDraw()
{
    bool locked = false;
    if(!IsPanelActive || Event != EVENT_NONE) { GuiLock(); locked = true; }
    EditAreaBounds = (Rectangle){10, 60, GetScreenWidth()-20-200, GetScreenHeight()-60-2*30-15};
    if(!ShowFullPreview)
        PanelRec = GuiScrollPanel(EditAreaBounds, (Rectangle){0, 0, SkinTexture.width*Zoom, SkinTexture.height*Zoom}, &Scroll);
    else
        PanelRec = GuiScrollPanel(EditAreaBounds, (Rectangle){0, 0, (PREVIEW_WIDTH+EditorProp[7].value.vint.val)*Zoom, (PREVIEW_HEIGHT+6*EditorProp[7].value.vint.val)*Zoom}, &Scroll);
    
    BeginScissorMode(PanelRec.x, PanelRec.y, PanelRec.width, PanelRec.height);
        if(!ShowFullPreview)
        {
             ClearBackground(LIGHTGRAY);
             
            // Draw grid and texture
            if(EditorProp[4].value.vbool) {
                DrawTexturePro(CheckerTexture, (Rectangle){0,0,SkinTexture.width/Zoom/2,SkinTexture.height/Zoom/2}, 
                    (Rectangle){Scroll.x + PanelRec.x, Scroll.y+PanelRec.y, SkinTexture.width*Zoom,SkinTexture.height*Zoom}, (Vector2){0,0}, 0.f, Fade(WHITE, 0.45f));
            }
            DrawTextureEx(SkinTexture, (Vector2){Scroll.x + PanelRec.x, Scroll.y + PanelRec.y}, 0.0f, Zoom, Fade(WHITE, EditorProp[3].value.vfloat.val));
            
            GuiSkinStyle* style = GetActiveSkinStyle();
            if(style != NULL && style->width != 0 && style->height != 0) {
                // Draw selection
                int x[4] = { style->nx, style->fx, style->px, style->dx };
                int y[4] = { style->ny, style->fy, style->py, style->dy };
                int lineScale = 1 + Zoom/2;
                Rectangle linesRec = {Scroll.x + PanelRec.x + x[ActiveState]*Zoom - lineScale, Scroll.y + PanelRec.y + y[ActiveState]*Zoom - lineScale, 
                        style->width*Zoom+2*lineScale, style->height*Zoom+2*lineScale};
                DrawRectangleLinesEx(linesRec,lineScale, EditorProp[0].value.vcolor);
                const int helper = EditorProp[6].value.vint.val*lineScale;
                
                // Calculate bounds for the resizers
                Rectangle tl = {linesRec.x - helper, linesRec.y - helper, helper, helper}; // Top Left
                Rectangle tr = {linesRec.x + linesRec.width, linesRec.y - helper, helper, helper}; //Top Right
                Rectangle bl = {linesRec.x - helper, linesRec.y + linesRec.height, helper, helper}; // Bottom Left
                Rectangle br = {linesRec.x + linesRec.width, linesRec.y + linesRec.height, helper, helper}; // Bottom Right
                
                if(ActiveState == GUI_STATE_NORMAL) {
                    // Draw resizers
                    DrawRectangleRec(tl, EditorProp[0].value.vcolor); // Top Left
                    DrawRectangleRec(tr, EditorProp[0].value.vcolor); // Top Right
                    DrawRectangleRec(bl, EditorProp[0].value.vcolor); // Bottom Left
                    DrawRectangleRec(br, EditorProp[0].value.vcolor); // Bottom Right
                }
                
                Rectangle te = { linesRec.x + lineScale, tl.y + style->top*Zoom + helper/2, style->width*Zoom, helper }; // Top Edge
                Rectangle be = { linesRec.x + lineScale, bl.y - style->bottom*Zoom - helper/2, style->width*Zoom, helper }; // Bottom Edge
                Rectangle le = { bl.x + style->left*Zoom + helper/2, linesRec.y + lineScale, helper, style->height*Zoom }; // Left Edge
                Rectangle re = { br.x - style->right*Zoom - helper/2, linesRec.y + lineScale, helper, style->height*Zoom }; // Right Edge
                
                // Draw margins
                if(style->margins > 0 && 2*style->margins < style->height && 2*style->margins < style->width) {
                    const float m = style->margins*Zoom;
                    DrawRectangleLines(linesRec.x + lineScale + m , linesRec.y + lineScale + m, linesRec.width - 2*(lineScale + m), linesRec.height - 2*(lineScale + m), EditorProp[2].value.vcolor);
                }
                
                Vector2 mouse = GetMousePosition();
                // Draw top edge
                if(Event == EVENT_MOVE_TOP_EDGE || (Event == EVENT_NONE && CheckCollisionPointRec(mouse, te)))
                    DrawLine(Scroll.x + PanelRec.x, linesRec.y + lineScale + style->top*Zoom, Scroll.x + PanelRec.x + PanelRec.width*Zoom, linesRec.y + lineScale + style->top*Zoom, EditorProp[1].value.vcolor);
                else if(style->top != 0)
                    DrawLine(linesRec.x + lineScale, linesRec.y + lineScale + style->top*Zoom, linesRec.x + lineScale + style->width*Zoom, linesRec.y + lineScale + style->top*Zoom, EditorProp[1].value.vcolor);
                // Draw bottom edge
                if(Event == EVENT_MOVE_BOTTOM_EDGE || (Event == EVENT_NONE && CheckCollisionPointRec(mouse, be)))
                    DrawLine(Scroll.x + PanelRec.x, linesRec.y + lineScale + style->height*Zoom - style->bottom*Zoom, Scroll.x + PanelRec.x + PanelRec.width*Zoom, linesRec.y + lineScale + style->height*Zoom - style->bottom*Zoom, EditorProp[1].value.vcolor);
                else if(style->bottom != 0)
                    DrawLine(linesRec.x + lineScale, linesRec.y + lineScale + style->height*Zoom - style->bottom*Zoom, linesRec.x + lineScale + style->width*Zoom, linesRec.y + lineScale + style->height*Zoom - style->bottom*Zoom, EditorProp[1].value.vcolor);
                // Draw left edge
                if(Event == EVENT_MOVE_LEFT_EDGE || (Event == EVENT_NONE && CheckCollisionPointRec(mouse, le)))
                    DrawLine(linesRec.x + lineScale + style->left*Zoom, Scroll.y + PanelRec.y,linesRec.x + lineScale + style->left*Zoom, Scroll.y + PanelRec.y + PanelRec.height*Zoom, EditorProp[1].value.vcolor);
                else if(style->left != 0)
                    DrawLine(linesRec.x + lineScale + style->left*Zoom, linesRec.y + lineScale, linesRec.x + lineScale + style->left*Zoom, linesRec.y + lineScale + style->height*Zoom, EditorProp[1].value.vcolor);
                // Draw right edge
                if(Event == EVENT_MOVE_RIGHT_EDGE || (Event == EVENT_NONE && CheckCollisionPointRec(mouse, re)))
                    DrawLine(linesRec.x + lineScale + (style->width - style->right)*Zoom, Scroll.y + PanelRec.y, linesRec.x + lineScale + (style->width - style->right)*Zoom, Scroll.y + PanelRec.y + PanelRec.height*Zoom, EditorProp[1].value.vcolor);
                else if(style->right != 0)
                    DrawLine(linesRec.x + lineScale + (style->width - style->right)*Zoom, linesRec.y + lineScale, linesRec.x + lineScale + (style->width - style->right)*Zoom, linesRec.y + lineScale + style->height*Zoom, EditorProp[1].value.vcolor);
                
            }
        } 
        else 
            DrawPreviewAll(EditAreaBounds);
    EndScissorMode();
    
    if(locked) GuiUnlock();
    
    // Draw the GUI on top of everything
    if(Event != EVENT_NONE) GuiLock(); // lock the UI when moving/selecting/resizing
    DrawGUI();
    GuiUnlock(); 
}

// Helper function: Gets the active style of currenly selected control/skin
GuiSkinStyle* GetActiveSkinStyle() {
    if(SkinTexture.id > 0 && SlotsCount > 0)
    {
        GuiSkin* skin = &Slots[ActiveSkin];
        
        switch(ActiveControl)
        {
            case GUI_SKIN_BUTTON_SID: return &skin->button;
            case GUI_SKIN_IMGBUTTON_SID: return &skin->imagebutton;
            case GUI_SKIN_STATUS_SID: return &skin->status;
            case GUI_SKIN_PANEL_SID: return &skin->panel;
            
            case GUI_SKIN_WINDOW_SID:
                if(ActiveStyle == 0) return &skin->window.panel;
                else if(ActiveStyle == 1) return &skin->window.titlebar;
                else if(ActiveStyle == 2) return &skin->window.button;
            break;
            
            case GUI_SKIN_SCROLLPANEL_SID:
            {
                GuiSkinStyle* arr[8] = {&skin->scrollpanel.body, &skin->scrollpanel.corner, &skin->scrollpanel.hscroll.body, &skin->scrollpanel.hscroll.rail, 
                    &skin->scrollpanel.hscroll.slider, &skin->scrollpanel.vscroll.body, &skin->scrollpanel.vscroll.rail, &skin->scrollpanel.vscroll.slider};
                return arr[ActiveStyle];
            }
            
            case GUI_SKIN_LIST_SID:
            {
                GuiSkinStyle* arr[7] = {&skin->list.body, &skin->list.item, &skin->list.first, &skin->list.last, &skin->list.scrollbar.body, &skin->list.scrollbar.rail, 
                                &skin->list.scrollbar.slider};
                return arr[ActiveStyle];
            }
            
            case GUI_SKIN_SCROLLBAR_SID:
                if(ActiveStyle == 0) return &skin->scrollbar.body;
                else if(ActiveStyle == 1) return &skin->scrollbar.rail;
                else if(ActiveStyle == 2) return &skin->scrollbar.slider;
            break;
            
                                
            case GUI_SKIN_TOGGLE_SID:
                if(ActiveStyle == 0) return &skin->toggle.off;
                else if(ActiveStyle == 1) return &skin->toggle.on;
            break;
                    
            case GUI_SKIN_TOGGLEGROUP_SID:
            {
                GuiSkinStyle* arr[6] = {&skin->togglegroup.item.off, &skin->togglegroup.item.on, &skin->togglegroup.first.off, &skin->togglegroup.first.on, 
                    &skin->togglegroup.last.off, &skin->togglegroup.last.on};
                return arr[ActiveStyle];
            }
                    
            case GUI_SKIN_CHECKBOX_SID:
                if(ActiveStyle == 0) return &skin->checkbox.body;
                else if(ActiveStyle == 1) return &skin->checkbox.checked;
            break;
                    
            case GUI_SKIN_COMBOBOX_SID:
                if(ActiveStyle == 0) return &skin->combobox.body;
                else if(ActiveStyle == 1) return &skin->combobox.button;
            break;
                    
            case GUI_SKIN_DROPBOX_SID:
                if(ActiveStyle == 0) return &skin->dropbox.body;
                else if(ActiveStyle == 1) return &skin->dropbox.listpanel;
                else if(ActiveStyle == 2) return &skin->dropbox.item;
            break;
                    
            case GUI_SKIN_SPINNER_SID:
                if(ActiveStyle == 0) return &skin->spinner.valuebox;
                else if(ActiveStyle == 1) return &skin->spinner.left;
                else if(ActiveStyle == 2) return &skin->spinner.right;
            break;
                    
            case GUI_SKIN_SLIDER_SID:
                if(ActiveStyle == 0) return &skin->slider.body;
                else if(ActiveStyle == 1) return &skin->slider.slider;
                else if(ActiveStyle == 2) return &skin->slider.bar;
            break;
 
            case GUI_SKIN_PROGRESSBAR_SID:
                if(ActiveStyle == 0) return &skin->progressbar.body;
                else if(ActiveStyle == 1) return &skin->progressbar.progress;
            break;
                    
            case GUI_SKIN_VALUEBOX_SID: return &skin->valuebox;
            case GUI_SKIN_TEXTBOX_SID: return &skin->textbox;
            case GUI_SKIN_TEXTBOXMULTI_SID: return &skin->textboxmulti;
                    
            case GUI_SKIN_COLORPANEL_SID:
                if(ActiveStyle == 0) return &skin->colorpanel.body;
                else if(ActiveStyle == 1) return &skin->colorpanel.selector;
            break;
                    
            case GUI_SKIN_COLORBARALPHA_SID:
                if(ActiveStyle == 0) return &skin->colorbaralpha.body;
                else if(ActiveStyle == 1) return &skin->colorbaralpha.selector;
            break;
                    
            case GUI_SKIN_COLORBARHUE_SID:
                if(ActiveStyle == 0) return &skin->colorbarhue.body;
                else if(ActiveStyle == 1) return &skin->colorbarhue.selector;
            break;
                    
            case GUI_SKIN_TOOLTIP_SID: return &skin->tooltip;
        }
    }
    
    return NULL;
}
//---------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------
// CUSTOM WIDGETS USED BY THE EDITOR
//---------------------------------------------------------------------------------------
// A dropdownbox with a adjustable list height.
bool DropdownWidget(Rectangle bounds, int listHeight, const char** text, int count, int* active, bool editMode) 
{
    static int scroll = 0;
    static int focused = 0;
    bool pressed = false;
    
    if(GuiDropdownBox(bounds, TextFormat("%s; ", text[*active == -1 ? 0 : *active]), &(int){0}, false)) pressed = true;
    
    if(editMode) {
        Rectangle listBounds = { bounds.x, bounds.y + bounds.height + GuiGetStyle(DROPDOWNBOX, DROPDOWN_ITEMS_PADDING), bounds.width, listHeight};
        int tmp = *active;
        *active = GuiListViewEx(listBounds, text, count, &focused, &scroll, tmp);
        if(tmp != *active) pressed = true;  // active item changed
        
        if(!pressed && (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) || IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) && !CheckCollisionPointRec(GetMousePosition(), listBounds))
            pressed = true; // clicked outside the list
    }
    
    return pressed;
}

bool ZoomWidget(Rectangle bounds, const char** text, int count, int* active)
{
    GuiControlState state = GuiGetState();
    
    Rectangle spinner = { bounds.x + GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH) + GuiGetStyle(SPINNER, SPIN_BUTTON_PADDING), bounds.y,
                          bounds.width - 2*(GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH) + GuiGetStyle(SPINNER, SPIN_BUTTON_PADDING)), bounds.height };
    Rectangle leftButtonBound = { (float)bounds.x, (float)bounds.y, (float)GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.height };
    Rectangle rightButtonBound = { (float)bounds.x + bounds.width - GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.y, (float)GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.height };
    
    bool pressed = false;
    
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
    //--------------------------------------------------------------------
    
    // Draw control
    //--------------------------------------------------------------------
    int tempBorderWidth = GuiGetStyle(LABEL, BORDER_WIDTH);
    int tempTextAlign = GuiGetStyle(LABEL, TEXT_ALIGNMENT);
    GuiSetStyle(LABEL, BORDER_WIDTH, GuiGetStyle(SPINNER, BORDER_WIDTH));
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    
    if (GuiLabelButton(leftButtonBound, "-")) { --*active; pressed = true; }
    if (GuiLabelButton(rightButtonBound, "+")) { ++*active; pressed = true; }
    
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, tempTextAlign);
    GuiSetStyle(LABEL, BORDER_WIDTH, tempBorderWidth);
    
    if(*active < 0) *active = count - 1;
    else if(*active >= count) *active = 0;
    
    GuiDrawRectangle(bounds, 1, Fade(GetColor(GuiGetStyle(BUTTON, BORDER + (state*3))), guiAlpha), BLANK, NULL, state);
    if (text != NULL && text[*active] != NULL) GuiDrawText(text[*active], spinner, GUI_TEXT_ALIGN_LEFT, Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state*3))), guiAlpha));
    //--------------------------------------------------------------------
    
    return pressed;
}

void SkinSelectWidget(Rectangle bounds, int listHeight, int* active, bool* toggle) {
    GuiControlState state = GuiGetState();
    
    Rectangle centerBound = { bounds.x + GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH) + GuiGetStyle(SPINNER, SPIN_BUTTON_PADDING), bounds.y,
                          bounds.width - 2*(GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH) + GuiGetStyle(SPINNER, SPIN_BUTTON_PADDING)), bounds.height };
    Rectangle leftButtonBound = { (float)bounds.x, (float)bounds.y, (float)GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.height };
    Rectangle rightButtonBound = { (float)bounds.x + bounds.width - GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.y, (float)GuiGetStyle(SPINNER, SPIN_BUTTON_WIDTH), (float)bounds.height };
    
    Rectangle listBound = { bounds.x, bounds.y - listHeight - GuiGetStyle(SPINNER, SPIN_BUTTON_PADDING), bounds.width, listHeight };
    
    
    // Update control
    //--------------------------------------------------------------------
    if ((state != GUI_STATE_DISABLED) && !guiLocked) {
        Vector2 mouse = GetMousePosition();
        if(*toggle && IsMouseButtonDown(MOUSE_LEFT_BUTTON)) {
            if(!CheckCollisionPointRec(mouse, listBound)) *toggle = false;
        }
        
        if(IsMouseButtonDown(MOUSE_LEFT_BUTTON) && CheckCollisionPointRec(mouse, centerBound)) *toggle = !*toggle;
    }
    //--------------------------------------------------------------------
    
    
    // Draw control
    //--------------------------------------------------------------------
    
    // Draw the left/right buttons
    int tempBorderWidth = GuiGetStyle(LABEL, BORDER_WIDTH);
    int tempTextAlign = GuiGetStyle(LABEL, TEXT_ALIGNMENT);
    GuiSetStyle(LABEL, BORDER_WIDTH, GuiGetStyle(SPINNER, BORDER_WIDTH));
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, GUI_TEXT_ALIGN_CENTER);
    
    if (GuiLabelButton(leftButtonBound, "<")) --*active;
    if (GuiLabelButton(rightButtonBound, ">")) ++*active;
    
    GuiSetStyle(LABEL, TEXT_ALIGNMENT, tempTextAlign);
    GuiSetStyle(LABEL, BORDER_WIDTH, tempBorderWidth);
    
    if(*active < 0) *active = SlotsCount - 1;
    else if(*active >= SlotsCount) *active = 0;
    
    
    if(*toggle) {
        // Draw the center piece as a textbox pointing to the active skin
        GuiTextBox(centerBound, SkinNames[*active], 31, true);
        
        // Draw skin list bg
        GuiPanel(listBound);
        
        Rectangle leftRec = { listBound.x + 2, listBound.y + 2, listBound.width - 33, listBound.height - 4};
        
        // Draw the list
        static int listFocus = 0; static int listScroll = 0;
        listFocus = *active;
        *active = GuiListViewEx(leftRec, (const char**)SkinNames, SlotsCount, &listFocus, &listScroll, *active);
        
        // Draw the right side buttons
        Rectangle rightRec = { leftRec.x + leftRec.width  + 2 , leftRec.y, listBound.width - leftRec.width - 2, leftRec.height};
        if(GuiButton(RAYGUI_CLITERAL(Rectangle){rightRec.x, rightRec.y, 26, 26}, "#121#")) { *active = SkinSlotsMoveUp(*active); } // MOVE UP
        if(GuiButton(RAYGUI_CLITERAL(Rectangle){rightRec.x, rightRec.y + 26 + 2, 26, 26}, "#120#")) { *active = SkinSlotsMoveDown(*active); } //MOVE DOWN
        if(GuiButton(RAYGUI_CLITERAL(Rectangle){rightRec.x, rightRec.y + rightRec.height - 2*26 - 2, 26, 26}, "#143#")) { // REMOVE
            SkinSlotsPop(*active);
        }
        if(GuiButton(RAYGUI_CLITERAL(Rectangle){rightRec.x, rightRec.y + rightRec.height - 26, 26, 26}, "#23#")) { // CREATE
            if(SlotsCount < MAX_SKIN_SLOTS)
            {
                GuiSkin skin = {0};
                strcpy(skin.name, TextFormat("skin_%i", SlotsCount));
                SkinSlotsPush(&skin);
                *active = SlotsCount - 1;
            }
        }
    } else {
        // Draw center piece as a button, hide the skin list
        GuiButton(centerBound, SlotsCount>0 ? SkinNames[*active] : "");
    }
    //--------------------------------------------------------------------
}

void PreviewWidget(Rectangle bounds, GuiSkinStyle* style) 
{
    GuiGroupBox(bounds, "Preview");
    
    // Switch to skin
    GuiSetSkinTexture(SkinTexture);
    GuiSetSkin(&Slots[ActiveSkin]);
    
    // Switch the style
    if(IsCustomStyleLoaded && EditorProp[5].value.vbool) { // use a custom style or the default one
        memcpy(guiStyle, GuiStyle, SIZEOF(guiStyle)*sizeof(guiStyle[0]));
        GuiSetFont(StyleFont);
    }
    
    if(EditorProp[4].value.vbool) GuiGrid(bounds, 16.0f, 2);  // Draw grid
    
    // Set selected state for the control
    int state = GuiGetState();
    GuiSetState(ActiveState);
    
    // Draw active control using selected skin
    //--------------------------------------------------------------------
    static Vector2 scroll = {0.0f, 0.0f};
    static int active = 0; static int scrl = 0;
    static bool toggle = false;
    static bool edit = false;
    static int value = 50;
    static float val = 50.0f;
    static Color color = GOLD;
    Vector2 mouse = GetMousePosition();
    static char text[36] = {0};
    if(ActiveState != GUI_STATE_NORMAL) edit = true;
    switch(ActiveControl) {
        case GUI_SKIN_BUTTON_SID: GuiButton(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-60)/2, bounds.width-20, 60}, "Button"); break;
        case GUI_SKIN_IMGBUTTON_SID: GuiImageButton(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-60)/2, bounds.width-20, 60}, "", CheckerTexture); break;
        case GUI_SKIN_STATUS_SID: GuiStatusBar(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-40)/2, bounds.width-20, 40}, "Status | Bar"); break;
        case GUI_SKIN_PANEL_SID: GuiPanel(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-80)/2, bounds.width-20, 80}); break;
        case GUI_SKIN_WINDOW_SID: GuiWindowBox(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-180)/2, bounds.width-20, 180}, "Window"); break;
        case GUI_SKIN_SCROLLPANEL_SID: GuiScrollPanel(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-180)/2, bounds.width-20, 180}, RAYGUI_CLITERAL(Rectangle){0,0,360,400}, &scroll); break;
        case GUI_SKIN_LIST_SID: active = GuiListView(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-180)/2, bounds.width-20, 180}, "ONE;TWO;THREE;FOUR;FIVE;SIX;SEVEN;EIGHT", &scrl, active);  break;
        case GUI_SKIN_SCROLLBAR_SID: 
            scrl = GuiScrollBar( RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+10, bounds.width-20, 20}, scrl, 0, 100);
            scrl = GuiScrollBar( RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+40, 20, bounds.height-50}, scrl, 0, 100);
        break;
        case GUI_SKIN_TOGGLE_SID: toggle = GuiToggle(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-60)/2, bounds.width-20, 60}, "Toggle", toggle); break;
        case GUI_SKIN_TOGGLEGROUP_SID: active = GuiToggleGroup(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-60)/2, 45, 30}, "ONE;TWO;THREE;FOUR", active); break;
        case GUI_SKIN_CHECKBOX_SID: toggle = GuiCheckBox(RAYGUI_CLITERAL(Rectangle){bounds.x+(bounds.width-60)/2, bounds.y+(bounds.height-40)/2, 20, 20}, "Checkbox", toggle); break;
        case GUI_SKIN_COMBOBOX_SID: active = GuiComboBox(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-60)/2, bounds.width-20, 60}, "ONE;TWO;THREE", active);  break;
        case GUI_SKIN_DROPBOX_SID: if(GuiDropdownBox(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+10, bounds.width-20, 30}, "ONE;TWO;THREE;FOUR", &active, edit)) edit = !edit; break;
        case GUI_SKIN_SPINNER_SID: GuiSpinner(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-40)/2, bounds.width-20, 40}, "", &value, 0, 100, CheckCollisionPointRec(mouse, bounds)); break;
        case GUI_SKIN_SLIDER_SID: 
            val = GuiSlider(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-30)/2 - 30, bounds.width-20, 30}, "", "", val, 0.0f, 100.0f); 
            val = GuiSliderBar(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-30)/2+10, bounds.width-20, 30}, "", "", val, 0.0f, 100.0f); 
        break;
        case GUI_SKIN_PROGRESSBAR_SID:
            if(CheckCollisionPointRec(mouse, bounds)) val += GetMouseWheelMove()*3.0f; // move progress bar with mouse wheel
            if(val > 100.0f) val = 100.0f;
            else if(val < 0.0f) val = 0.0f;
            GuiProgressBar(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-30)/2, bounds.width-20, 30}, "", "", val, 0.0f, 100.0f); 
        break;
        case GUI_SKIN_VALUEBOX_SID: GuiValueBox(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-40)/2, bounds.width-20, 40}, "", &value, 0, 100, CheckCollisionPointRec(mouse, bounds)); break;
        case GUI_SKIN_TEXTBOX_SID: GuiTextBox(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-40)/2, bounds.width-20, 40}, text, 35, CheckCollisionPointRec(mouse, bounds)); break;
        case GUI_SKIN_TEXTBOXMULTI_SID: GuiTextBoxMulti(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+10, bounds.width-20, 180}, text, 35, CheckCollisionPointRec(mouse, bounds)); break;
        case GUI_SKIN_COLORPANEL_SID: color = GuiColorPanel(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+10, bounds.width-20, 180}, color); break;
        case GUI_SKIN_COLORBARALPHA_SID: val = GuiColorBarAlpha(RAYGUI_CLITERAL(Rectangle){bounds.x+10, bounds.y+(bounds.height-60)/2, bounds.width-20, 60}, val); break;
        case GUI_SKIN_COLORBARHUE_SID: val = GuiColorBarHue(RAYGUI_CLITERAL(Rectangle){bounds.x+(bounds.width-40)/2+10, bounds.y+10, 20, 180}, val); break;
        case GUI_SKIN_TOOLTIP_SID:
        {
            const char* const tooltip = "Just a tooltip preview!";
            
            // code below copied from GuiDrawTooltip()
            Vector2 textSize = MeasureTextEx(guiFont, tooltip, GuiGetStyle(DEFAULT, TEXT_SIZE), GuiGetStyle(DEFAULT, TEXT_SPACING));
            Rectangle tooltipBounds = { bounds.x+(bounds.width-textSize.x-20)/2, bounds.y+(bounds.height-textSize.y*2)/2, textSize.x + 20, textSize.y*2 };

            GuiDrawRectangle(tooltipBounds, 1, Fade(GetColor(GuiGetStyle(DEFAULT, LINE_COLOR)), guiAlpha), Fade(GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)), guiAlpha), guiSkin != NULL ? &guiSkin->tooltip : NULL, GUI_STATE_NORMAL);

            tooltipBounds.x += 10;
            GuiLabel(tooltipBounds, tooltip);
        }
        break;
    }
    //--------------------------------------------------------------------
    GuiSetState(state);
    
    // Disable skin & load default style
    GuiSetSkin(NULL);
    GuiLoadStyleDefault();
}

void StylePropWidget(Rectangle bounds, GuiSkinStyle* style) {
    static int propFocus = 0;
    static int propScroll = 0;

    if(ActiveProp == 0)
    {
        // STYLE PROPS
        if(style != NULL) 
        {
            unsigned short* x[4] = {&style->nx, &style->fx, &style->px, &style->dx};
            unsigned short* y[4] = {&style->ny, &style->fy, &style->py, &style->dy};
            
            if(ActiveState != GUI_STATE_NORMAL) 
            {
                // Set props
                
                //can only set the X / Y coordinates here
                SkinStyleProp2[1].value.vint.val = *x[ActiveState];
                SkinStyleProp2[2].value.vint.val = *y[ActiveState];
                
                // Draw props
                GuiDMPropertyList(bounds, SkinStyleProp2, SIZEOF(SkinStyleProp2), &propFocus, &propScroll);
                
                // Update props
                *x[ActiveState] = SkinStyleProp2[1].value.vint.val;
                *y[ActiveState] = SkinStyleProp2[2].value.vint.val;
            } 
            else 
            {
                // Set props
                SkinStyleProp[0].value.vrect.x = *x[ActiveState];
                SkinStyleProp[0].value.vrect.y = *y[ActiveState]; 
                SkinStyleProp[0].value.vrect.width = style->width; 
                SkinStyleProp[0].value.vrect.height = style->height;
                
                SkinStyleProp[1].value.vint.val = style->margins;
                SkinStyleProp[2].value.vselect.active = style->type;
                SkinStyleProp[3].value.vselect.active = style->mode;
                
                SkinStyleProp[5].value.vint.val = style->top;
                SkinStyleProp[6].value.vint.val = style->bottom;
                SkinStyleProp[7].value.vint.val = style->left;
                SkinStyleProp[8].value.vint.val = style->right;
                
                // Draw prop
                GuiDMPropertyList(bounds, SkinStyleProp, SIZEOF(SkinStyleProp), &propFocus, &propScroll);
                
                // Update props
                unsigned short tmp = SkinStyleProp[0].value.vrect.x < 0.0f ? 0 : SkinStyleProp[0].value.vrect.x;
                *x[ActiveState] = tmp;
                tmp = SkinStyleProp[0].value.vrect.y < 0.0f ? 0 : SkinStyleProp[0].value.vrect.y;
                *y[ActiveState] = tmp;
                
                style->width = SkinStyleProp[0].value.vrect.width < 0.0f ? 0 : SkinStyleProp[0].value.vrect.width;
                style->height = SkinStyleProp[0].value.vrect.height < 0.0f ? 0 : SkinStyleProp[0].value.vrect.height;
                
                style->margins = SkinStyleProp[1].value.vint.val;
                if(2*style->margins >= style->width-1) style->margins = style->width/2;
                if(2*style->margins >= style->height-1) style->margins = style->height/2;
                
                style->type = SkinStyleProp[2].value.vselect.active;
                style->mode = SkinStyleProp[3].value.vselect.active;
                if(style->mode == GUI_SKIN_DRAW_CENTERED) style->type = GUI_SKIN_TYPE_SLICE1;
                
                style->top = SkinStyleProp[5].value.vint.val;
                style->bottom = SkinStyleProp[6].value.vint.val;
                style->left = SkinStyleProp[7].value.vint.val;
                style->right = SkinStyleProp[8].value.vint.val;
            }
        }
        else
        {
            // draw properties but in the disabled state
            int state = GuiGetState();
            GuiSetState(GUI_STATE_DISABLED);
            if(ActiveState != GUI_STATE_NORMAL) GuiDMPropertyList(bounds, SkinStyleProp2, SIZEOF(SkinStyleProp2), &propFocus, &propScroll);
            else GuiDMPropertyList(bounds, SkinStyleProp, SIZEOF(SkinStyleProp), &propFocus, &propScroll);
            GuiSetState(state);
        }
    }
    else 
    {
        // EDITOR PROPS
        GuiDMPropertyList(bounds, EditorProp, SIZEOF(EditorProp), &propFocus, &propScroll);
    }
    
}
//---------------------------------------------------------------------------------------




//---------------------------------------------------------------------------------------
// FUNCTIONS FOR CREATING/DESTROYING/MODIFYING THE SKIN SLOTS ARRAY
//---------------------------------------------------------------------------------------
void SkinSlotsCreate() {
    Slots = calloc(MAX_SKIN_SLOTS, sizeof(GuiSkin));
    if(Slots == NULL)
        TraceLog(LOG_FATAL, "Failed to allocate skin slots array. Out of memory!");
    SlotsCount = 0;
    
    // After creating the slot array we push a default skin style into it
    GuiSkin style0 = {0};
    style0.useColor = true;
    strcpy(style0.name, "default");
    SkinSlotsPush(&style0);
}

void SkinSlotsDestroy() {
    free(Slots);
}

// Add skin `s` into last free slot
void SkinSlotsPush(GuiSkin* s) {
    if(SlotsCount + 1 > MAX_SKIN_SLOTS) return;
    Slots[SlotsCount] = *s;
    
    // Modify the SkinNames to reflect the changes done to the Slots array
    SkinNames[SlotsCount] = &Slots[SlotsCount].name[0];
    
    SlotsCount++;
}

// Remove slot at index `idx`
void SkinSlotsPop(int idx) {
    if(idx < 0 || SlotsCount <= 1) return;
    if(idx != SlotsCount - 1)  {
        // Shift slots to the left
        memmove(&Slots[idx], &Slots[idx+1], (SlotsCount - idx - 1)*sizeof(GuiSkin) );
    }
    SlotsCount--;
}

// Move slot up by one
int SkinSlotsMoveUp(int idx) {
    if(idx > 0 && idx < SlotsCount) {
        // Swap slots
        GuiSkin tmp = Slots[idx-1];
        Slots[idx-1] = Slots[idx];
        Slots[idx] = tmp;
        
        return idx-1;
    }
    return idx;
}

// Move slot down by one
int SkinSlotsMoveDown(int idx) {
    if(idx >= 0 && idx + 1 < SlotsCount) {
        GuiSkin tmp = Slots[idx+1];
        Slots[idx+1] = Slots[idx];
        Slots[idx] = tmp;

        return idx+1;
    }
    return idx;
}

//---------------------------------------------------------------------------------------

