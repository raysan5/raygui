#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "../../src/raygui.h"

#include "../../styles/dark/style_dark.h"

static Vector2 window_position = { 10, 10 };
static Vector2 window_size = { 200, 400 };
static bool minimized = false;
static bool moving = false;
static bool resizing = false;
static Vector2 scroll;

static Vector2 window2_position = { 250, 10 };
static Vector2 window2_size = { 200, 400 };
static bool minimized2 = false;
static bool moving2 = false;
static bool resizing2 = false;
static Vector2 scroll2;

void GuiWindowFloating(Vector2 *position, Vector2 *size, bool *minimized, bool *moving, bool *resizing, void (*draw_content)(Vector2, Vector2), Vector2 content_size, Vector2 *scroll, const char* title) {
    #if !defined(RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT)
        #define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24
    #endif

    #if !defined(RAYGUI_WINDOW_CLOSEBUTTON_SIZE)
        #define RAYGUI_WINDOW_CLOSEBUTTON_SIZE 18
    #endif

    int close_title_size_delta_half = (RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT - RAYGUI_WINDOW_CLOSEBUTTON_SIZE) / 2;

    // window movement and resize input and collision check
    if(IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && !*moving && !*resizing) {
        Vector2 mouse_position = GetMousePosition();

        Rectangle title_collision_rect = { position->x, position->y, size->x - (RAYGUI_WINDOW_CLOSEBUTTON_SIZE + close_title_size_delta_half), RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT };
        Rectangle resize_collision_rect = { position->x + size->x - 20, position->y + size->y - 20, 20, 20 };

        if(CheckCollisionPointRec(mouse_position, title_collision_rect)) {
            *moving = true;
        } else if(!*minimized && CheckCollisionPointRec(mouse_position, resize_collision_rect)) {
            *resizing = true;
        }
    }

    // window movement and resize update
    if(*moving) {
        Vector2 mouse_delta = GetMouseDelta();
        position->x += mouse_delta.x;
        position->y += mouse_delta.y;

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            *moving = false;

            // clamp window position keep it inside the application area
            if(position->x < 0) position->x = 0;
            else if(position->x > GetScreenWidth() - size->x) position->x = GetScreenWidth() - size->x;
            if(position->y < 0) position->y = 0;
            else if(position->y > GetScreenHeight()) position->y = GetScreenHeight() - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT;
        }

    } else if(*resizing) {
        Vector2 mouse = GetMousePosition();
        if (mouse.x > position->x)
            size->x = mouse.x - position->x;
        if (mouse.y > position->y)
            size->y = mouse.y - position->y;

        // clamp window size to an arbitrary minimum value and the window size as the maximum
        if(size->x < 100) size->x = 100;
        else if(size->x > GetScreenWidth()) size->x = GetScreenWidth();
        if(size->y < 100) size->y = 100;
        else if(size->y > GetScreenHeight()) size->y = GetScreenHeight();

        if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
            *resizing = false;
        }
    }

    // window and content drawing with scissor and scroll area
    if(*minimized) {
        GuiStatusBar((Rectangle){ position->x, position->y, size->x, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT }, title);

        if (GuiButton((Rectangle){ position->x + size->x - RAYGUI_WINDOW_CLOSEBUTTON_SIZE - close_title_size_delta_half,
                                   position->y + close_title_size_delta_half,
                                   RAYGUI_WINDOW_CLOSEBUTTON_SIZE,
                                   RAYGUI_WINDOW_CLOSEBUTTON_SIZE },
                                   "#120#")) {
            *minimized = false;
        }

    } else {
        *minimized = GuiWindowBox((Rectangle) { position->x, position->y, size->x, size->y }, title);

        // scissor and draw content within a scroll panel
        if(draw_content != NULL) {
            Rectangle scissor = { 0 };
            GuiScrollPanel((Rectangle) { position->x, position->y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, size->x, size->y - RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT },
                                         NULL,
                                         (Rectangle) { position->x, position->y, content_size.x, content_size.y },
                                         scroll,
                                         &scissor);

            bool require_scissor = size->x < content_size.x || size->y < content_size.y;

            if(require_scissor) {
                BeginScissorMode(scissor.x, scissor.y, scissor.width, scissor.height);
            }

            draw_content(*position, *scroll);

            if(require_scissor) {
                EndScissorMode();
            }
        }

        // draw the resize button/icon
        GuiDrawIcon(71, position->x + size->x - 20, position->y + size->y - 20, 1, WHITE);
    }
}

static void DrawContent(Vector2 position, Vector2 scroll) {
    GuiButton((Rectangle) { position.x + 20 + scroll.x, position.y + 50  + scroll.y, 100, 25 }, "Button 1");
    GuiButton((Rectangle) { position.x + 20 + scroll.x, position.y + 100 + scroll.y, 100, 25 }, "Button 2");
    GuiButton((Rectangle) { position.x + 20 + scroll.x, position.y + 150 + scroll.y, 100, 25 }, "Button 3");
    GuiLabel((Rectangle) { position.x + 20 + scroll.x, position.y + 200 + scroll.y, 250, 25 }, "A Label");
    GuiLabel((Rectangle) { position.x + 20 + scroll.x, position.y + 250 + scroll.y, 250, 25 }, "Another Label");
    GuiLabel((Rectangle) { position.x + 20 + scroll.x, position.y + 300 + scroll.y, 250, 25 }, "Yet Another Label");
}

int main(void) {
    InitWindow(960, 560, "raygui - floating window example");
    SetTargetFPS(60);
    GuiLoadStyleDark();

    while(!WindowShouldClose()) {
        BeginDrawing();
            ClearBackground(DARKGREEN);
            GuiWindowFloating(&window_position, &window_size, &minimized, &moving, &resizing, &DrawContent, (Vector2) { 140, 320 }, &scroll, "Movable & Scalable Window");
            GuiWindowFloating(&window2_position, &window2_size, &minimized2, &moving2, &resizing2, &DrawContent, (Vector2) { 140, 320 }, &scroll2, "Another window");
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
