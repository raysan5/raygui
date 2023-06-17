/*******************************************************************************************
*
*   Animation curves - An example demo for animation curves
*
*   DEPENDENCIES:
*       raylib 4.0  - Windowing/input management and drawing.
*       raygui 3.0  - Immediate-mode GUI controls.
*
*   COMPILATION (Windows - MinGW):
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -I../../src -lraylib -lopengl32 -lgdi32 -std=c99
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2023 Pierre Jaffuer (@smallcluster)
*
**********************************************************************************************/

#include "raylib.h"

#define RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT 24
#define RAYGUI_IMPLEMENTATION
#include "../../src/raygui.h"

// raygui embedded styles
#include "../style_selector/styles/style_cyber.h"             // raygui style: cyber
#include "../style_selector/styles/style_jungle.h"            // raygui style: jungle
#include "../style_selector/styles/style_lavanda.h"           // raygui style: lavanda
#include "../style_selector/styles/style_dark.h"              // raygui style: dark
#include "../style_selector/styles/style_bluish.h"            // raygui style: bluish
#include "../style_selector/styles/style_terminal.h"          // raygui style: terminal

#undef RAYGUI_IMPLEMENTATION            // Avoid including raygui implementation again

#define GUI_CURVE_EDITOR_IMPLEMENTATION
#include "gui_curve_editor.h"

//------------------------------------------------------------------------------------
// Helper function
//------------------------------------------------------------------------------------
void LoadCurveDefaults(GuiCurveEditorState curves[]);

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main()
{
    // Initialization
    //---------------------------------------------------------------------------------------
    const int screenWidth = 800;
    const int screenHeight = 540;

    InitWindow(screenWidth, screenHeight, "raygui - animation curves");
    SetTargetFPS(60);

    // Gui style
    GuiLoadStyleDefault();
    int visualStyleActive = 0;
    int prevVisualStyleActive = 0;

    float fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    const float margin = 8;

    // Gui states
    Vector2 scrollOffset = (Vector2){ 0, 0 };
    Rectangle contentRect = (Rectangle){ 0, 0, 0, 0 };
    bool moveSlider = false;
    bool sectionActive[5] = { 0 };
    sectionActive[0] = true;
    const char *sectionNames[5] = { "X Position", "Y Position", "Width", "Height", "Rotation" };
    bool editValueBox[5][4] = { 0 };
    char *valTextBox[5][4][20] = { 0 };
    bool playAnimation = true;
    bool showHelp = true;

    Rectangle settingsRect = (Rectangle){ screenWidth - screenWidth/3, 0, screenWidth/3, screenHeight };

    // Animation curves
    // 0 -> Ball X position
    // 1 -> Ball Y position
    // 2 -> Ball Width
    // 3 -> Ball Height
    // 4 -> Ball rotation
    GuiCurveEditorState curves[5] = { 0 };
    LoadCurveDefaults(curves);

    // Animation time
    float time = 0.0f;
    float animationTime = 4.0f;

    //SetTargetFPS(60);
    //--------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update
        //----------------------------------------------------------------------------------
        if (playAnimation) time += GetFrameTime();

        // Reset timer
        if (time > animationTime) time = 0;

        // Ball animation
        const float t = time/animationTime;
        Vector2 ballPos = (Vector2){ GuiCurveEval(&curves[0], t), GuiCurveEval(&curves[1], t) };
        Vector2 ballSize = (Vector2){ GuiCurveEval(&curves[2], t), GuiCurveEval(&curves[3], t) };
        float ballRotation = GuiCurveEval(&curves[4], t);

        // Update style
        if (visualStyleActive != prevVisualStyleActive)
        {
            switch (visualStyleActive)
            {
                case 0: GuiLoadStyleDefault(); break;
                case 1: GuiLoadStyleJungle(); break;
                case 2: GuiLoadStyleLavanda(); break;
                case 3: GuiLoadStyleDark(); break;
                case 4: GuiLoadStyleBluish(); break;
                case 5: GuiLoadStyleCyber(); break;
                case 6: GuiLoadStyleTerminal(); break;
                default: break;
            }

            fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
            prevVisualStyleActive = visualStyleActive;
        }

        // Update settings panel rect
        Rectangle sliderRect = (Rectangle){ settingsRect.x - 4, settingsRect.y, 4, settingsRect.height };
        if (CheckCollisionPointRec(GetMousePosition(), sliderRect) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) moveSlider = true;
        if (IsMouseButtonUp(MOUSE_BUTTON_LEFT)) moveSlider = false;

        if (moveSlider)
        {
            settingsRect.x = GetMouseX();

            // Minimum-Maximum size
            if (settingsRect.x > (screenWidth - 4)) settingsRect.x = screenWidth - 4;
            else if (settingsRect.x < 4) settingsRect.x = 4;

            settingsRect.width = screenWidth - settingsRect.x;
        }


        //----------------------------------------------------------------------------------

        // Draw
        //----------------------------------------------------------------------------------
        BeginDrawing();
            ClearBackground(GetColor( GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

            // Scene
            //----------------------------------------------------------------------------------
            DrawRectangle(curves[0].start, curves[1].end, curves[0].end-curves[0].start, curves[1].start-curves[1].end, BLUE);  // Sky

            DrawRectangle(curves[0].start, curves[1].start, curves[0].end-curves[0].start, 32, DARKGREEN);  // Ground

            BeginScissorMode(curves[0].start, curves[1].end, curves[0].end-curves[0].start, curves[1].start-curves[1].end+32);

                DrawRectanglePro((Rectangle){ballPos.x, ballPos.y, ballSize.x, ballSize.y}, (Vector2){ballSize.x/2.f,ballSize.y/2.f}, ballRotation, PINK);  // Ball

                DrawLine(ballPos.x, ballPos.y, ballPos.x + cosf(ballRotation*DEG2RAD)*ballSize.x, ballPos.y +sinf(ballRotation*DEG2RAD)*ballSize.y, RED);
                DrawLine(ballPos.x, ballPos.y, ballPos.x + cosf((ballRotation+90)*DEG2RAD)*ballSize.x, ballPos.y +sinf((ballRotation+90)*DEG2RAD)*ballSize.y, GREEN);

            EndScissorMode();

            // Bounds
            DrawRectangleLines(curves[0].start, curves[1].end, curves[0].end-curves[0].start, curves[1].start-curves[1].end+32, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));
            //----------------------------------------------------------------------------------

            // GUI
            //----------------------------------------------------------------------------------
            if (showHelp)
            {
                if (GuiWindowBox((Rectangle) {margin, margin, settingsRect.x-2*margin, curves[1].end-2*margin}, "help")) showHelp = false;

                Rectangle helpTextRect = (Rectangle) { 2*margin, 2*margin+RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, settingsRect.x - 4 - 4*margin, 0 };
                GuiLabel((Rectangle) {helpTextRect.x, helpTextRect.y+helpTextRect.height, helpTextRect.width, fontSize}, "Curve widget controls:");
                helpTextRect.height += fontSize+margin;
                GuiLabel((Rectangle) {helpTextRect.x, helpTextRect.y+helpTextRect.height, helpTextRect.width, fontSize}, "- Left click to move/add point or move tangents");
                helpTextRect.height += fontSize+margin/2;
                GuiLabel((Rectangle) {helpTextRect.x, helpTextRect.y+helpTextRect.height, helpTextRect.width, fontSize}, "- While moving a tangent, hold SHIFT to disable tangent symetry");
                helpTextRect.height += fontSize+margin/2;
                GuiLabel((Rectangle) {helpTextRect.x, helpTextRect.y+helpTextRect.height, helpTextRect.width, fontSize}, "- Right click to remove a point");
                helpTextRect.height += fontSize+margin/2;
                DrawRectangleGradientV(margin, margin+curves[1].end - 2*margin, settingsRect.x - 2*margin, 12, (Color){ 0,0,0,100 }, BLANK);
            }

            // Settings panel
            GuiScrollPanel(settingsRect, "Settings", contentRect, &scrollOffset, NULL);

            BeginScissorMode(settingsRect.x, settingsRect.y+RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, settingsRect.width, settingsRect.height);

            // Rebuild the content Rect
            contentRect = (Rectangle){ settingsRect.x + margin, RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT+margin, settingsRect.width - 2*margin - GuiGetStyle(LISTVIEW, SCROLLBAR_WIDTH), 0 };

            // Help button
            if (GuiButton((Rectangle){ contentRect.x, contentRect.y + contentRect.height + scrollOffset.y, contentRect.width, 1.5*fontSize }, GuiIconText(showHelp? ICON_EYE_ON : ICON_EYE_OFF, "Curve controls help"))) showHelp = !showHelp;

            contentRect.height += 1.5*fontSize + margin;

            // Animation Time slider
            GuiSlider((Rectangle){ contentRect.x, contentRect.y+contentRect.height+scrollOffset.y, contentRect.width/2, fontSize }, NULL, TextFormat("Animation Time: %.2fs", animationTime), &animationTime, 1, 8);
            contentRect.height += fontSize + margin;

            // Load default curves
            if (GuiButton((Rectangle){ contentRect.x, contentRect.y+contentRect.height+scrollOffset.y, contentRect.width, 1.5*fontSize }, "Load default"))
            {
                LoadCurveDefaults(curves);
                animationTime = 4.0f;
                time = 0.0f;
            }
            contentRect.height += 1.5f*fontSize + margin;

            // Styles
            GuiLabel((Rectangle){ contentRect.x, contentRect.y + contentRect.height + scrollOffset.y, contentRect.width, fontSize }, "Style:");
            contentRect.height += fontSize;
            GuiComboBox((Rectangle){contentRect.x, contentRect.y+contentRect.height+scrollOffset.y, contentRect.width, 1.5*fontSize }, "default;Jungle;Lavanda;Dark;Bluish;Cyber;Terminal", &visualStyleActive);
            contentRect.height += 1.5f*fontSize + margin;

            // Draw curves with their controls
            //----------------------------------------------------------------------------------
            for (int i = 0; i < 5; i++)
            {
                // Collapsing section
                Rectangle headerRect = (Rectangle){ contentRect.x, contentRect.y + contentRect.height+scrollOffset.y, contentRect.width, 1.5f*fontSize };
                GuiStatusBar(headerRect, NULL);

                if (GuiLabelButton(headerRect, GuiIconText(sectionActive[i] ? ICON_ARROW_DOWN_FILL : ICON_ARROW_RIGHT_FILL, sectionNames[i]))) sectionActive[i] = !sectionActive[i];

                contentRect.height += 1.5f*fontSize + margin;

                // Skip this section
                if (!sectionActive[i]) continue;

                // Draw curve control
                Rectangle curveRect = (Rectangle){ contentRect.x, contentRect.y+contentRect.height + scrollOffset.y, contentRect.width, fontSize*12 };
                EndScissorMode(); // Stop clipping from setting rect

                // Curves can leaks from control boundary... scissor it !
                BeginScissorMode(curveRect.x, curveRect.y, curveRect.width, curveRect.height);
                    GuiCurveEditor(&curves[i], curveRect);
                EndScissorMode();

                // Resume clipping from setting rect
                BeginScissorMode(settingsRect.x, settingsRect.y + RAYGUI_WINDOWBOX_STATUSBAR_HEIGHT, settingsRect.width, settingsRect.height);
                contentRect.height += fontSize*12 + margin;

                // Draw selected point controls
                GuiCurveEditorPoint *p = &(curves[i].points[curves[i].selectedIndex]);
                GuiCheckBox((Rectangle){ contentRect.x, contentRect.y + contentRect.height + scrollOffset.y, 1.5f*fontSize, 1.5f*fontSize }, "Left Linear", &p->leftLinear);
                GuiCheckBox((Rectangle){ contentRect.x+contentRect.width/2, contentRect.y + contentRect.height + scrollOffset.y, 1.5f*fontSize, 1.5f*fontSize }, "Right Linear", &p->rightLinear);
                contentRect.height += 1.5f*fontSize + margin;

                // Positions
                GuiLabel((Rectangle){ contentRect.x, contentRect.y + contentRect.height + scrollOffset.y, contentRect.width, fontSize }, "Position");
                contentRect.height += fontSize;

                if (!editValueBox[i][0]) gcvt(p->position.x, 6, valTextBox[i][0]);  // Transform x position to string

                if (!editValueBox[i][1]) gcvt(curves[i].start + (curves[i].end-curves[i].start)*p->position.y, 6, valTextBox[i][1]); // Transform y position to string

                // X pos
                if (GuiTextBox((Rectangle){ contentRect.x, contentRect.y + contentRect.height + scrollOffset.y, contentRect.width/2-margin, 1.5f*fontSize }, valTextBox[i][0], 20, editValueBox[i][0]))
                {
                    editValueBox[i][0] = !editValueBox[i][0];

                    // Input ended
                    if (!editValueBox[i][0])
                    {
                        // Try to convert text to float and assign it to the point
                        char *endPtr = NULL;
                        double value = strtod((char *)valTextBox[i][0], &endPtr);

                        if (endPtr != (char *)valTextBox[i][0]) p->position.x = (value < 0)? 0 : (value > 1)? 1 : value;
                    }
                }

                // Y pos
                if (GuiTextBox((Rectangle){ contentRect.x + contentRect.width/2, contentRect.y + contentRect.height + scrollOffset.y, contentRect.width/2.0f, 1.5f*fontSize }, valTextBox[i][1], 20, editValueBox[i][1]))
                {
                    editValueBox[i][1] = !editValueBox[i][1];

                    // Input ended
                    if (!editValueBox[i][1])
                    {
                        // Try to convert text to float and assign it to the point
                        char *endPtr = NULL;
                        double value = strtod((char *)valTextBox[i][1], &endPtr);

                        if (endPtr != (char *)valTextBox[i][1])
                        {
                            float normalizedVal = (value - curves[i].start)/(curves[i].end - curves[i].start);
                            p->position.y = (normalizedVal < 0)? 0 : (normalizedVal > 1)? 1 : normalizedVal;
                        }
                    }

                }

                contentRect.height += 1.5f*fontSize + margin;

                // Tangents
                GuiLabel((Rectangle){ contentRect.x, contentRect.y + contentRect.height + scrollOffset.y, contentRect.width, fontSize }, "Tangents");
                contentRect.height += fontSize;

                if (!editValueBox[i][2]) gcvt(p->tangents.x, 6, valTextBox[i][2]); // Transform left tangent to string

                if (!editValueBox[i][3]) gcvt(p->tangents.y, 6, valTextBox[i][3]);   // Transform right tangent to string

                // Left tan
                if (GuiTextBox((Rectangle){ contentRect.x, contentRect.y + contentRect.height + scrollOffset.y, contentRect.width/2 - margin, 1.5f*fontSize }, valTextBox[i][2], 20, editValueBox[i][2]))
                {
                    editValueBox[i][2] = !editValueBox[i][2];

                    // Input ended
                    if (!editValueBox[i][2])
                    {
                        // Try to convert text to float and assign it to the point
                        char *endPtr = NULL;
                        double value = strtod((char *)valTextBox[i][2], &endPtr);
                        if (endPtr != (char *)valTextBox[i][2]) p->tangents.x = value;
                    }
                }

                // Right tan
                if (GuiTextBox((Rectangle){ contentRect.x + contentRect.width/2.0f, contentRect.y + contentRect.height + scrollOffset.y, contentRect.width/2.0f, 1.5f*fontSize }, valTextBox[i][3], 20, editValueBox[i][3]))
                {
                    editValueBox[i][3] = !editValueBox[i][3];

                    // Input ended
                    if (!editValueBox[i][3])
                    {
                        // Try to convert text to float and assign it to the point
                        char *endPtr = NULL;
                        double value = strtod((char *)valTextBox[i][3], &endPtr);
                        if (endPtr != (char *)valTextBox[i][3]) p->tangents.y = value;
                    }
                }

                contentRect.height += 1.5*fontSize + margin;
            }

            contentRect.height += margin;

            EndScissorMode();

            // Settings panel shadow
            DrawRectangleGradientH(settingsRect.x - 12, 0, 12, settingsRect.height, BLANK, (Color){ 0, 0, 0, 100 });

            // Slider
            if (moveSlider) DrawRectangle(sliderRect.x, sliderRect.y, sliderRect.width, sliderRect.height, GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED)));
            else if(CheckCollisionPointRec(GetMousePosition(), sliderRect)) DrawRectangle(sliderRect.x, sliderRect.y, sliderRect.width, sliderRect.height, GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED)));

            // Draw Time controls
            //----------------------------------------------------------------------------------
            Rectangle timeLineRect = (Rectangle){ 0, screenHeight-4*fontSize, settingsRect.x, 4*fontSize };
            GuiPanel((Rectangle){ timeLineRect.x, timeLineRect.y, timeLineRect.width, 2*fontSize }, NULL);
            GuiLabel((Rectangle){ timeLineRect.x, timeLineRect.y, timeLineRect.width, 2*fontSize }, TextFormat("Normalized Time: %.3f", time/animationTime));
            if (GuiButton((Rectangle){ timeLineRect.x+timeLineRect.width/2 - 2*fontSize - margin/4, timeLineRect.y, 2*fontSize, 2*fontSize }, GuiIconText((playAnimation? ICON_PLAYER_PAUSE : ICON_PLAYER_PLAY), ""))) playAnimation = !playAnimation;

            if (GuiButton((Rectangle){ timeLineRect.x+timeLineRect.width/2 + margin/4, timeLineRect.y, 2*fontSize, 2*fontSize }, GuiIconText(ICON_PLAYER_STOP, "")))
            {
                playAnimation = false;
                time = 0;
            }

            float animTime = time/animationTime;
            GuiSlider((Rectangle){timeLineRect.x, timeLineRect.y + 2*fontSize, timeLineRect.width, timeLineRect.height - 2*fontSize }, NULL, NULL, &animTime, 0, 1);
            time = animationTime*animTime;

            // Time panel shadow
            DrawRectangleGradientV(timeLineRect.x, timeLineRect.y - 12, timeLineRect.width, 12, BLANK, (Color){ 0, 0, 0, 100 });

        EndDrawing();
        //----------------------------------------------------------------------------------
    }

    CloseWindow();              // Close window and OpenGL context
    //--------------------------------------------------------------------------------------

    return 0;
}

void LoadCurveDefaults(GuiCurveEditorState curves[])
{
    // X pos
    curves[0].start = 28;
    curves[0].end = 506;
    curves[0].numPoints = 4;
    curves[0].selectedIndex = 0;
    curves[0].editLeftTangent = false;
    curves[0].editRightTangent = false;
    curves[0].points[0].position =(Vector2) {0.000000, 0.000000}; curves[0].points[0].tangents = (Vector2) {0.000000, 1.515101};  curves[0].points[0].leftLinear =   1;curves[0].points[0].rightLinear =  1;
    curves[0].points[1].position =(Vector2) {0.422414, 0.640000}; curves[0].points[1].tangents = (Vector2) {-2.824348, -4.494999};curves[0].points[1].leftLinear =   0;curves[0].points[1].rightLinear =  0;
    curves[0].points[2].position =(Vector2) {0.732759, 0.210000}; curves[0].points[2].tangents = (Vector2) {0.000000, 2.956133};  curves[0].points[2].leftLinear =   0;curves[0].points[2].rightLinear =  1;
    curves[0].points[3].position =(Vector2) {1.000000, 1.000000}; curves[0].points[3].tangents = (Vector2) {2.956133, 0.000000};  curves[0].points[3].leftLinear =   1;curves[0].points[3].rightLinear =  1;

    // Y pos
    curves[1].start = 405;
    curves[1].end = 135;
    curves[1].numPoints = 7;
    curves[1].selectedIndex = 0;
    curves[1].editLeftTangent = false;
    curves[1].editRightTangent = false;
    curves[1].points[0].position = (Vector2) {0.000000, 1.000000};curves[1].points[0].tangents = (Vector2) { 0.000000  , 0.000000};curves[1].points[0].leftLinear =  0;curves[1].points[0].rightLinear =  0;
    curves[1].points[1].position = (Vector2) {0.140000, 0.000000};curves[1].points[1].tangents = (Vector2) {-10.000000 ,10.000000};curves[1].points[1].leftLinear =  0;curves[1].points[1].rightLinear =  0;
    curves[1].points[2].position = (Vector2) {0.450000, 0.000000};curves[1].points[2].tangents = (Vector2) {-10.000000 ,10.000000};curves[1].points[2].leftLinear =  0;curves[1].points[2].rightLinear =  0;
    curves[1].points[3].position = (Vector2) {0.670000, 0.000000};curves[1].points[3].tangents = (Vector2) {-10.000000 ,10.000000};curves[1].points[3].leftLinear =  0;curves[1].points[3].rightLinear =  0;
    curves[1].points[4].position = (Vector2) {0.830000, 0.000000};curves[1].points[4].tangents = (Vector2) {-10.000000 ,10.000000};curves[1].points[4].leftLinear =  0;curves[1].points[4].rightLinear =  0;
    curves[1].points[5].position = (Vector2) {0.940000, 0.000000};curves[1].points[5].tangents = (Vector2) {-10.000000 ,10.000000};curves[1].points[5].leftLinear =  0;curves[1].points[5].rightLinear =  0;
    curves[1].points[6].position = (Vector2) {1.000000, 0.000000};curves[1].points[6].tangents = (Vector2) {-10.000000 , 0.000000};curves[1].points[6].leftLinear =  0;curves[1].points[6].rightLinear =  0;

    // X size
    curves[2].start = 1;
    curves[2].end = 64;
    curves[2].numPoints = 16;
    curves[2].selectedIndex = 0;
    curves[2].editLeftTangent = false;
    curves[2].editRightTangent = false;
    curves[2].points[0].position =  (Vector2) {0.000000, 0.492063}; curves[2].points[0].tangents =  (Vector2) {0,0}; curves[2].points[0].leftLinear =  0; curves[2].points[0].rightLinear =  0;
    curves[2].points[1].position =  (Vector2) {0.130000, 0.492063}; curves[2].points[1].tangents =  (Vector2) {0,0}; curves[2].points[1].leftLinear =  0; curves[2].points[1].rightLinear =  0;
    curves[2].points[2].position =  (Vector2) {0.140000, 0.746032}; curves[2].points[2].tangents =  (Vector2) {0,0}; curves[2].points[2].leftLinear =  0; curves[2].points[2].rightLinear =  0;
    curves[2].points[3].position =  (Vector2) {0.150000, 0.492063}; curves[2].points[3].tangents =  (Vector2) {0,0}; curves[2].points[3].leftLinear =  0; curves[2].points[3].rightLinear =  0;
    curves[2].points[4].position =  (Vector2) {0.440000, 0.490000}; curves[2].points[4].tangents =  (Vector2) {0,0}; curves[2].points[4].leftLinear =  0; curves[2].points[4].rightLinear =  0;
    curves[2].points[5].position =  (Vector2) {0.450000, 0.682540}; curves[2].points[5].tangents =  (Vector2) {0,0}; curves[2].points[5].leftLinear =  0; curves[2].points[5].rightLinear =  0;
    curves[2].points[6].position =  (Vector2) {0.460000, 0.480000}; curves[2].points[6].tangents =  (Vector2) {0,0}; curves[2].points[6].leftLinear =  0; curves[2].points[6].rightLinear =  0;
    curves[2].points[7].position =  (Vector2) {0.660000, 0.492063}; curves[2].points[7].tangents =  (Vector2) {0,0}; curves[2].points[7].leftLinear =  0; curves[2].points[7].rightLinear =  0;
    curves[2].points[8].position =  (Vector2) {0.670000, 0.619048}; curves[2].points[8].tangents =  (Vector2) {0,0}; curves[2].points[8].leftLinear =  0; curves[2].points[8].rightLinear =  0;
    curves[2].points[9].position =  (Vector2) {0.680000, 0.492063}; curves[2].points[9].tangents =  (Vector2) {0,0}; curves[2].points[9].leftLinear =  0; curves[2].points[9].rightLinear =  0;
    curves[2].points[10].position = (Vector2) {0.820000, 0.492063}; curves[2].points[10].tangents = (Vector2) {0,0}; curves[2].points[10].leftLinear = 0; curves[2].points[10].rightLinear = 0;
    curves[2].points[11].position = (Vector2) {0.830000, 0.619048}; curves[2].points[11].tangents = (Vector2) {0,0}; curves[2].points[11].leftLinear = 0; curves[2].points[11].rightLinear = 0;
    curves[2].points[12].position = (Vector2) {0.840000, 0.492063}; curves[2].points[12].tangents = (Vector2) {0,0}; curves[2].points[12].leftLinear = 0; curves[2].points[12].rightLinear = 0;
    curves[2].points[13].position = (Vector2) {0.930000, 0.492063}; curves[2].points[13].tangents = (Vector2) {0,0}; curves[2].points[13].leftLinear = 0; curves[2].points[13].rightLinear = 0;
    curves[2].points[14].position = (Vector2) {0.940000, 0.619048}; curves[2].points[14].tangents = (Vector2) {0,0}; curves[2].points[14].leftLinear = 0; curves[2].points[14].rightLinear = 0;
    curves[2].points[15].position = (Vector2) {0.950000, 0.492063}; curves[2].points[15].tangents = (Vector2) {0,0}; curves[2].points[15].leftLinear = 0; curves[2].points[15].rightLinear = 0;

    // Y Size
    curves[3].start = 1;
    curves[3].end = 64;
    curves[3].numPoints = 16;
    curves[3].selectedIndex = 0;
    curves[3].editLeftTangent = false;
    curves[3].editRightTangent = false;
    curves[3].points[0].position =  (Vector2) {0.000000, 0.492063};curves[3].points[0].tangents =  (Vector2) {0,0};curves[3].points[0].leftLinear =  0;curves[3].points[0].rightLinear =  0;
    curves[3].points[1].position =  (Vector2) {0.130000, 0.492063};curves[3].points[1].tangents =  (Vector2) {0,0};curves[3].points[1].leftLinear =  0;curves[3].points[1].rightLinear =  0;
    curves[3].points[2].position =  (Vector2) {0.140000, 0.238095};curves[3].points[2].tangents =  (Vector2) {0,0};curves[3].points[2].leftLinear =  0;curves[3].points[2].rightLinear =  0;
    curves[3].points[3].position =  (Vector2) {0.150000, 0.492063};curves[3].points[3].tangents =  (Vector2) {0,0};curves[3].points[3].leftLinear =  0;curves[3].points[3].rightLinear =  0;
    curves[3].points[4].position =  (Vector2) {0.440000, 0.492063};curves[3].points[4].tangents =  (Vector2) {0,0};curves[3].points[4].leftLinear =  0;curves[3].points[4].rightLinear =  0;
    curves[3].points[5].position =  (Vector2) {0.450000, 0.301587};curves[3].points[5].tangents =  (Vector2) {0,0};curves[3].points[5].leftLinear =  0;curves[3].points[5].rightLinear =  0;
    curves[3].points[6].position =  (Vector2) {0.460000, 0.492063};curves[3].points[6].tangents =  (Vector2) {0,0};curves[3].points[6].leftLinear =  0;curves[3].points[6].rightLinear =  0;
    curves[3].points[7].position =  (Vector2) {0.660000, 0.492063};curves[3].points[7].tangents =  (Vector2) {0,0};curves[3].points[7].leftLinear =  0;curves[3].points[7].rightLinear =  0;
    curves[3].points[8].position =  (Vector2) {0.670000, 0.365079};curves[3].points[8].tangents =  (Vector2) {0,0};curves[3].points[8].leftLinear =  0;curves[3].points[8].rightLinear =  0;
    curves[3].points[9].position =  (Vector2) {0.680000, 0.492063};curves[3].points[9].tangents =  (Vector2) {0,0};curves[3].points[9].leftLinear =  0;curves[3].points[9].rightLinear =  0;
    curves[3].points[10].position = (Vector2) {0.820000, 0.492063};curves[3].points[10].tangents = (Vector2) {0,0};curves[3].points[10].leftLinear = 0;curves[3].points[10].rightLinear = 0;
    curves[3].points[11].position = (Vector2) {0.830000, 0.365079};curves[3].points[11].tangents = (Vector2) {0,0};curves[3].points[11].leftLinear = 0;curves[3].points[11].rightLinear = 0;
    curves[3].points[12].position = (Vector2) {0.840000, 0.492063};curves[3].points[12].tangents = (Vector2) {0,0};curves[3].points[12].leftLinear = 0;curves[3].points[12].rightLinear = 0;
    curves[3].points[13].position = (Vector2) {0.930000, 0.492063};curves[3].points[13].tangents = (Vector2) {0,0};curves[3].points[13].leftLinear = 0;curves[3].points[13].rightLinear = 0;
    curves[3].points[14].position = (Vector2) {0.940000, 0.365079};curves[3].points[14].tangents = (Vector2) {0,0};curves[3].points[14].leftLinear = 0;curves[3].points[14].rightLinear = 0;
    curves[3].points[15].position = (Vector2) {0.950000, 0.507937};curves[3].points[15].tangents = (Vector2) {0,0};curves[3].points[15].leftLinear = 0;curves[3].points[15].rightLinear = 0;

    // Rotation
    curves[4].start = -360;
    curves[4].end = 360;
    curves[4].numPoints = 9;
    curves[4].selectedIndex = 0;
    curves[4].editLeftTangent = false;
    curves[4].editRightTangent = false;
    curves[4].points[0].position =  (Vector2) {0.140000, 0.500000};curves[4].points[0].tangents =  (Vector2) {0,0};curves[4].points[0].leftLinear =  0;curves[4].points[0].rightLinear =  0;
    curves[4].points[1].position =  (Vector2) {0.450000, 0.500000};curves[4].points[1].tangents =  (Vector2) {0,0};curves[4].points[1].leftLinear =  0;curves[4].points[1].rightLinear =  0;
    curves[4].points[2].position =  (Vector2) {0.670000, 0.500000};curves[4].points[2].tangents =  (Vector2) {0,0};curves[4].points[2].leftLinear =  0;curves[4].points[2].rightLinear =  0;
    curves[4].points[3].position =  (Vector2) {0.830000, 0.500000};curves[4].points[3].tangents =  (Vector2) {0,0};curves[4].points[3].leftLinear =  0;curves[4].points[3].rightLinear =  0;
    curves[4].points[4].position =  (Vector2) {0.940000, 0.500000};curves[4].points[4].tangents =  (Vector2) {0,0};curves[4].points[4].leftLinear =  0;curves[4].points[4].rightLinear =  0;
    curves[4].points[5].position =  (Vector2) {1.000000, 0.500000};curves[4].points[5].tangents =  (Vector2) {0,0};curves[4].points[5].leftLinear =  0;curves[4].points[5].rightLinear =  0;
    curves[4].points[6].position =  (Vector2) {0.000000, 0.472222};curves[4].points[6].tangents =  (Vector2) {0,0};curves[4].points[6].leftLinear =  0;curves[4].points[6].rightLinear =  0;
    curves[4].points[7].position =  (Vector2) {0.302752, 0.527778};curves[4].points[7].tangents =  (Vector2) {0,0};curves[4].points[7].leftLinear =  0;curves[4].points[7].rightLinear =  0;
    curves[4].points[8].position =  (Vector2) {0.577982, 0.472222};curves[4].points[8].tangents =  (Vector2) {0,0};curves[4].points[8].leftLinear =  0;curves[4].points[8].rightLinear =  0;
}
