/*******************************************************************************************
*
*   CurveEdit v1.0 - A cubic Hermite editor for making animation curves
*
*   MODULE USAGE:
*       #define GUI_CURVE_EDIT_IMPLEMENTATION
*       #include "gui_curve_edit.h"
*
*       INIT: GuiCurveEditState state = InitCurveEdit();
*       EVALUATE: float y = EvalGuiCurve(&state, t); // 0 <= t <= 1
*       DRAW: BeginScissorMode(bounds.x,bounds.y,bounds.width,bounds.height); 
*               GuiCurveEdit(&state, bounds, pointSize);
*             EndScissorMode(); 
*
*   NOTE: See 'Module Structures Declaration' section for more informations.
*
*   NOTE: This module uses functions of the stdlib:
*       - qsort
*
*   NOTE: Built-in interactions:
*       - Left click to move/add point or move tangents
*       - While moving a tangent, hold (left/right) SHIFT to disable tangent symetry 
*       - Right click to remove a point
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2023 Pierre Jaffuer (@smallcluster)
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

#ifndef GUI_CURVE_EDIT_H
#define GUI_CURVE_EDIT_H


#ifndef GUI_CURVE_EDIT_MAX_POINTS
    #define GUI_CURVE_EDIT_MAX_POINTS 30
#endif 

//----------------------------------------------------------------------------------
// Module Structures Declaration
//----------------------------------------------------------------------------------

typedef struct {
    Vector2 position = (Vector2) {0,0}; // In normalized space [0.f, 1.f]
    Vector2 tangents = (Vector2) {0,0}; // The derivatives (left/right) of the 1D curve
    // Let the curve editor calculate tangents to linearize part of the curve
    bool leftLinear = false;
    bool rightLinear = false;
} GuiCurveEditPoint;

typedef struct {
    float start = 0; // Value at y = 0
    float end = 1; // Value at y = 1
    // Always valid (unless you manualy change state's point array)
    int selectedIndex = -1; // -1 before Init
    // Unsorted array with at least one point (constant curve)
    GuiCurveEditPoint points[GUI_CURVE_EDIT_MAX_POINTS];
    int numPoints = 0;

    // private part
    bool editLeftTangent = false;
    bool editRightTangent = false;
    Vector2 mouseOffset = (Vector2) {0,0};
} GuiCurveEditState;


#ifdef __cplusplus
extern "C" {            // Prevents name mangling of functions
#endif

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------

// Initialize state 
GuiCurveEditState InitGuiCurveEdit();
// Draw and update curve control
void GuiCurveEdit(GuiCurveEditState *state, Rectangle bounds);

// 1D Interpolation
// Returns the y value (in [start, end]) of the curve at x = t
// t must be normalized [0.f, 1.f]
float EvalGuiCurve(GuiCurveEditState *state, float t);

#ifdef __cplusplus
}
#endif

#endif // GUI_CURVE_EDIT_H

/***********************************************************************************
*
*   GUI_CURVE_EDIT IMPLEMENTATION
*
************************************************************************************/
#if defined(GUI_CURVE_EDIT_IMPLEMENTATION)

#include "../../src/raygui.h" // Change this to fit your project

#include "stdlib.h" // Required for qsort

//----------------------------------------------------------------------------------
// Module Functions Definition
//----------------------------------------------------------------------------------

GuiCurveEditState InitGuiCurveEdit()
{
    GuiCurveEditState state;

    state.start = 0;
    state.end = 1;
    state.selectedIndex = 0;
    state.editLeftTangent = false;
    state.editRightTangent = false;
    state.mouseOffset = (Vector2) {0.f,0.f};

    // At least one point (AVG by default)
    state.numPoints = 1;
    state.points[0].position = (Vector2) {0.5f,0.5f};
    state.points[0].tangents = (Vector2) {0.f,0.f};
    state.points[0].leftLinear = false;
    state.points[0].rightLinear = false;

    return state;
}

int CompareGuiCurveEditPointPtr (const void * a, const void * b)
{
  float fa = (*(GuiCurveEditPoint**)a)->position.x;
  float fb = (*(GuiCurveEditPoint**)b)->position.x;
  return (fa > fb) - (fa < fb);
}

float EvalGuiCurve(GuiCurveEditState *state, float t){

    // Sort points
    GuiCurveEditPoint* sortedPoints[GUI_CURVE_EDIT_MAX_POINTS];
    for(int i=0; i < state->numPoints; i++){
        sortedPoints[i] = &state->points[i];
    }
    qsort(sortedPoints, state->numPoints, sizeof(GuiCurveEditPoint*), CompareGuiCurveEditPointPtr);

    if(state->numPoints == 0)
        return state->start;

    // Constants part on edges
    if(t <= sortedPoints[0]->position.x)
        return state->start + (state->end-state->start) * sortedPoints[0]->position.y;
    if(t >= sortedPoints[state->numPoints-1]->position.x)
        return state->start + (state->end-state->start) * sortedPoints[state->numPoints-1]->position.y;

    // Find curve portion
    for(int i=0; i < state->numPoints-1; i++){
        const GuiCurveEditPoint* p1 = sortedPoints[i];
        const GuiCurveEditPoint* p2 = sortedPoints[i+1];
        // Skip this range
        if(!(t >= p1->position.x && t < p2->position.x) || p1->position.x == p2->position.x)
            continue;
        float scale = (p2->position.x-p1->position.x);
        float T = (t-p1->position.x)/scale;
        float startTangent = scale * p1->tangents.y;
        float endTangent = scale * p2->tangents.x;
        float T2 = T*T;
        float T3 = T*T*T;
        return state->start + (state->end-state->start) * ((2*T3-3*T2+1)*p1->position.y+(T3-2*T2+T)*startTangent+(3*T2-2*T3)*p2->position.y+(T3-T2)*endTangent);
    }

    return state->start;
}

void GuiCurveEdit(GuiCurveEditState *state, Rectangle bounds){

    //----------------------------------------------------------------------------------
    // CONST
    //----------------------------------------------------------------------------------
    const float pointSize = 10;
    const float fontSize = GuiGetStyle(DEFAULT, TEXT_SIZE);
    const float handleLength = pointSize*2.5;
    const float handleSize = pointSize/1.5f;

    const Rectangle innerBounds = (Rectangle){bounds.x+fontSize, bounds.y+fontSize, bounds.width-2*fontSize, bounds.height-2*fontSize};
    const Vector2 mouse = GetMousePosition();
    const Vector2 mouseLocal = (Vector2) {(mouse.x-innerBounds.x)/innerBounds.width, (innerBounds.y+innerBounds.height-mouse.y)/innerBounds.height};

    //----------------------------------------------------------------------------------
    // UPDATE STATE
    //----------------------------------------------------------------------------------

    // Find first point under mouse (-1 if not found)
    int hoveredPointIndex = -1;
    for(int i=0; i < state->numPoints; i++){
        const GuiCurveEditPoint* p = &state->points[i];
        const Vector2 screenPos = (Vector2){p->position.x*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-p->position.y*innerBounds.height};
        const Rectangle pointRect = (Rectangle) {screenPos.x-pointSize/2.f, screenPos.y-pointSize/2.f, pointSize, pointSize};
        if(CheckCollisionPointRec(mouse, pointRect)){
            hoveredPointIndex = i;
            break;
        }
    }

    // Unselect tangents
    if(IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
        state->editLeftTangent = false;
        state->editRightTangent = false;
    }

    // Select a tangent if possible
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && state->selectedIndex != -1 && CheckCollisionPointRec(mouse, bounds)){
        const GuiCurveEditPoint* p = &state->points[state->selectedIndex];
        const Vector2 screenPos = (Vector2){p->position.x*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-p->position.y*innerBounds.height};

        // Left control
        Vector2 target = (Vector2) {(p->position.x-1)*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-(p->position.y-p->tangents.x)*innerBounds.height};
        Vector2 dir = (Vector2) {target.x-screenPos.x, target.y-screenPos.y};
        float d = sqrt(dir.x*dir.x+dir.y*dir.y);
        Vector2 control = (Vector2) {screenPos.x+dir.x/d*handleLength, screenPos.y+dir.y/d*handleLength};
        Rectangle controlRect = (Rectangle) {control.x-handleSize/2.f, control.y-handleSize/2.f, handleSize, handleSize};

        // Edit left tangent
        if(CheckCollisionPointRec(mouse, controlRect)){
            state->editLeftTangent = true;
        }

        // Right control
        target = (Vector2) {(p->position.x+1)*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-(p->position.y+p->tangents.y)*innerBounds.height};
        dir = (Vector2) {target.x-screenPos.x, target.y-screenPos.y};
        d = sqrt(dir.x*dir.x+dir.y*dir.y);
        control = (Vector2) {screenPos.x+dir.x/d*handleLength, screenPos.y+dir.y/d*handleLength};
        controlRect = (Rectangle) {control.x-handleSize/2.f, control.y-handleSize/2.f, handleSize, handleSize};
        // Edit right tangent
        if(CheckCollisionPointRec(mouse, controlRect)){
            state->editRightTangent = true;
        }
    }

    
    // Move tangents
    if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && state->editRightTangent){
        // editRightTangent == true implies selectedIndex != -1
        GuiCurveEditPoint* p = &state->points[state->selectedIndex]; 
        const Vector2 screenPos = (Vector2){p->position.x*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-p->position.y*innerBounds.height};
        const Vector2 dir = (Vector2){mouseLocal.x-p->position.x, mouseLocal.y-p->position.y};
        // Calculate right tangent slope 
        p->tangents.y = dir.x < 0.001f ? dir.y/0.001f : dir.y/dir.x;
        p->rightLinear = false; // Stop right linearization update

        // Tangents are symetric by default unless SHIFT is pressed
        if(!(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))){
            p->tangents.x = p->tangents.y;
            p->leftLinear = false; // Stop left linearization update
        }

    } else if(IsMouseButtonDown(MOUSE_BUTTON_LEFT) && state->editLeftTangent){
        // editLeftTangent == true implies selectedIndex != -1
        GuiCurveEditPoint* p = &state->points[state->selectedIndex];
        const Vector2 screenPos = (Vector2){p->position.x*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-p->position.y*innerBounds.height};
        const Vector2 dir = (Vector2){mouseLocal.x-p->position.x, mouseLocal.y-p->position.y};
        // Calculate left tangent slope 
        p->tangents.x = dir.x > -0.001f ? dir.y/(-0.001f) : dir.y/dir.x;
        p->leftLinear = false; // Stop left linearization update

        // Tangents are symetric by default unless SHIFT is pressed
        if(!(IsKeyDown(KEY_LEFT_SHIFT) || IsKeyDown(KEY_RIGHT_SHIFT))){
            p->tangents.y = p->tangents.x;
            p->rightLinear = false; // Stop right linearization update
        }
    } 
    // Select a point
    else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && hoveredPointIndex != -1 CheckCollisionPointRec(mouse, bounds)){
        state->selectedIndex = hoveredPointIndex;
        const GuiCurveEditPoint* p = &state->points[state->selectedIndex];
        const Vector2 screenPos = (Vector2){p->position.x*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-p->position.y*innerBounds.height};
        state->mouseOffset = (Vector2) {p->position.x - mouseLocal.x, p->position.y - mouseLocal.y};
    }
    // Remove a point (check against bounds)
    else if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT) && hoveredPointIndex != -1 && CheckCollisionPointRec(mouse, bounds) && state->numPoints > 1){
        // Deselect everything
        state->selectedIndex = 0; // select first point by default
        state->editLeftTangent = false;
        state->editRightTangent = false;

        // Remove point
        state->numPoints -= 1;
        for(int i=hoveredPointIndex; i < state->numPoints; i++ ){
            state->points[i] = state->points[i+1];
        }
        
    // Add a point (check against innerBounds)
    } else if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, innerBounds) && state->numPoints < GUI_CURVE_EDIT_MAX_POINTS){
        state->editLeftTangent = false;
        state->editRightTangent = false;

        // Create new point
        GuiCurveEditPoint p;
        p.tangents = (Vector2) {0.f, 0.f};
        p.position = mouseLocal;
        p.leftLinear = false;
        p.rightLinear = false;
        // Append point
        state->points[state->numPoints] = p;
        state->selectedIndex = state->numPoints; // select new point
        state->numPoints += 1;
        // Point is add on mouse pos
        state->mouseOffset = (Vector2) {0,0};

    // Move selected point
    } else if(state->selectedIndex != -1 && IsMouseButtonDown(MOUSE_BUTTON_LEFT) && CheckCollisionPointRec(mouse, bounds) ){
        GuiCurveEditPoint* p = &state->points[state->selectedIndex];

        // use mouse offset on click to prevent point teleporting to mouse
        const Vector2 newLocalPos = (Vector2){mouseLocal.x + state->mouseOffset.x, mouseLocal.y + state->mouseOffset.y};

        // Clamp to innerbounds
        p->position.x = newLocalPos.x < 0 ? 0 : newLocalPos.x > 1 ? 1 : newLocalPos.x;
        p->position.y = newLocalPos.y < 0 ? 0 : newLocalPos.y > 1 ? 1 : newLocalPos.y;
    }

    // Sort points
    GuiCurveEditPoint* sortedPoints[GUI_CURVE_EDIT_MAX_POINTS];
    for(int i=0; i < state->numPoints; i++){
        sortedPoints[i] = &state->points[i];
    }
    qsort(sortedPoints, state->numPoints, sizeof(GuiCurveEditPoint*), CompareGuiCurveEditPointPtr);


    // Update linear tangents
    for(int i=0; i < state->numPoints; i++){
        GuiCurveEditPoint* p = sortedPoints[i];
        // Left tangent
        if(i > 0 && p->leftLinear){
            const GuiCurveEditPoint* p2 = sortedPoints[i-1];
            Vector2 dir = (Vector2) {p2->position.x-p->position.x, p2->position.y-p->position.y};
            p->tangents.x = dir.x == 0 ? 0 : dir.y/dir.x;
        }
        // Right tangent
        if(i < state->numPoints-1 && p->rightLinear){
            const GuiCurveEditPoint* p2 = sortedPoints[i+1];
            Vector2 dir = (Vector2) {p2->position.x-p->position.x, p2->position.y-p->position.y};
            p->tangents.y = dir.x == 0 ? 0 : dir.y/dir.x;
        }
    }

    //----------------------------------------------------------------------------------
    // DRAWING
    //----------------------------------------------------------------------------------

    // Draw bg
    DrawRectangle(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(GuiGetStyle(DEFAULT, BACKGROUND_COLOR)));

    // Draw grid
    // H lines
    const Color lineColor = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));
    DrawLine(bounds.x, innerBounds.y, bounds.x+bounds.width, innerBounds.y, lineColor); // end
    DrawLine(bounds.x, innerBounds.y+innerBounds.height/2, bounds.x+bounds.width, innerBounds.y+innerBounds.height/2, lineColor); // avg
    DrawLine(bounds.x, innerBounds.y+innerBounds.height, bounds.x+bounds.width, innerBounds.y+innerBounds.height, lineColor); // start

    // V lines
    DrawLine(innerBounds.x, bounds.y, innerBounds.x, bounds.y+bounds.height, lineColor); // 0
    DrawLine(innerBounds.x+innerBounds.width/4, bounds.y, innerBounds.x+innerBounds.width/4, bounds.y+bounds.height, lineColor); // 0.25
    DrawLine(innerBounds.x+innerBounds.width/2, bounds.y, innerBounds.x+innerBounds.width/2, bounds.y+bounds.height, lineColor); // 0.5
    DrawLine(innerBounds.x+3*innerBounds.width/4, bounds.y, innerBounds.x+3*innerBounds.width/4, bounds.y+bounds.height, lineColor); // 0.75
    DrawLine(innerBounds.x+innerBounds.width, bounds.y, innerBounds.x+innerBounds.width, bounds.y+bounds.height, lineColor); // 1

    Font font = GuiGetFont();
    // V labels
    DrawTextEx(font, "0",    (Vector2) {innerBounds.x, bounds.y+bounds.height-fontSize}, fontSize,GuiGetStyle(DEFAULT, TEXT_SPACING), lineColor);
    DrawTextEx(font, "0.25", (Vector2) {innerBounds.x+innerBounds.width/4.f, bounds.y+bounds.height-fontSize}, fontSize,GuiGetStyle(DEFAULT, TEXT_SPACING), lineColor);
    DrawTextEx(font, "0.5",  (Vector2) {innerBounds.x+innerBounds.width/2.f, bounds.y+bounds.height-fontSize}, fontSize,GuiGetStyle(DEFAULT, TEXT_SPACING), lineColor);
    DrawTextEx(font, "0.75", (Vector2) {innerBounds.x+3.f*innerBounds.width/4.f, bounds.y+bounds.height-fontSize}, fontSize,GuiGetStyle(DEFAULT, TEXT_SPACING), lineColor);
    DrawTextEx(font, "1",    (Vector2) {innerBounds.x+innerBounds.width, bounds.y+bounds.height-fontSize}, fontSize,GuiGetStyle(DEFAULT, TEXT_SPACING), lineColor);

    // H labels
    DrawTextEx(font, TextFormat("%.2f", state->start), (Vector2) {innerBounds.x, innerBounds.y-fontSize+innerBounds.height}, fontSize, GuiGetStyle(DEFAULT, TEXT_SPACING), lineColor);
    DrawTextEx(font, TextFormat("%.2f", state->start + (state->end-state->start)/2.f), (Vector2) {innerBounds.x, innerBounds.y-fontSize+innerBounds.height/2.f}, fontSize, GuiGetStyle(DEFAULT, TEXT_SPACING), lineColor);
    DrawTextEx(font, TextFormat("%.2f", state->end), (Vector2) {innerBounds.x, innerBounds.y}, fontSize, GuiGetStyle(DEFAULT, TEXT_SPACING), lineColor);

    // Draw contours
    if(CheckCollisionPointRec(mouse, bounds))
        DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_FOCUSED)));
    else
        DrawRectangleLines(bounds.x, bounds.y, bounds.width, bounds.height, GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL)));

    // Draw points
    for(int i=0; i < state->numPoints; i++){

        const GuiCurveEditPoint* p = sortedPoints[i];

        const Vector2 screenPos = (Vector2){p->position.x*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-p->position.y*innerBounds.height};
        const Rectangle pointRect = (Rectangle) {screenPos.x-pointSize/2.f, screenPos.y-pointSize/2.f, pointSize, pointSize};

        Color pointColor;
        Color pointBorderColor;

        // Draw point
        if(&state->points[state->selectedIndex] == p){

            // Draw left handle
            if(i > 0){
                const Vector2 target = (Vector2) {(p->position.x-1)*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-(p->position.y-p->tangents.x)*innerBounds.height};
                const Vector2 dir = (Vector2) {target.x-screenPos.x, target.y-screenPos.y};
                const float d = sqrt(dir.x*dir.x+dir.y*dir.y);
                const Vector2 control = (Vector2) {screenPos.x+dir.x/d*handleLength, screenPos.y+dir.y/d*handleLength};
                const Rectangle controlRect = (Rectangle) {control.x-handleSize/2.f, control.y-handleSize/2.f, handleSize, handleSize};

                Color controlColor;
                Color controlBorderColor;
                if(state->editLeftTangent){
                    controlColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
                    controlBorderColor =  GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));
                } else if(CheckCollisionPointRec(mouse, controlRect)){
                    controlColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED));
                    controlBorderColor = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));
                }else{
                    controlColor = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL));
                    controlBorderColor = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL));
                }
                DrawLine(screenPos.x,screenPos.y, control.x, control.y, controlColor);
                DrawRectangle(controlRect.x, controlRect.y, controlRect.width, controlRect.height, controlColor);
                DrawRectangleLines(controlRect.x, controlRect.y, controlRect.width, controlRect.height, controlColor);
            }
            // Draw right handle
            if(i < state->numPoints-1){
                const Vector2 target = (Vector2) {(p->position.x+1)*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-(p->position.y+p->tangents.y)*innerBounds.height};
                const Vector2 dir = (Vector2) {target.x-screenPos.x, target.y-screenPos.y};
                const float d = sqrt(dir.x*dir.x+dir.y*dir.y);
                const Vector2 control = (Vector2) {screenPos.x+dir.x/d*handleLength, screenPos.y+dir.y/d*handleLength};
                const Rectangle controlRect = (Rectangle) {control.x-handleSize/2.f, control.y-handleSize/2.f, handleSize, handleSize};

                Color controlColor;
                Color controlBorderColor;
                if(state->editRightTangent){
                    controlColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
                    controlBorderColor =  GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));
                } else if(CheckCollisionPointRec(mouse, controlRect)){
                    controlColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED));
                    controlBorderColor = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));
                }else{
                    controlColor = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL));
                    controlBorderColor = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL));
                }
                DrawLine(screenPos.x,screenPos.y, control.x, control.y, controlColor);
                DrawRectangle(controlRect.x, controlRect.y, controlRect.width, controlRect.height, controlColor);
                DrawRectangleLines(controlRect.x, controlRect.y, controlRect.width, controlRect.height, controlColor);
            }

            pointColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_PRESSED));
            pointBorderColor = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));

        } else if(&state->points[hoveredPointIndex] == p) {
            pointColor = GetColor(GuiGetStyle(DEFAULT, BASE_COLOR_FOCUSED));
            pointBorderColor = GetColor(GuiGetStyle(DEFAULT, BORDER_COLOR_NORMAL));
        } else {
            pointColor = GetColor(GuiGetStyle(BUTTON, BASE_COLOR_NORMAL));
            pointBorderColor = GetColor(GuiGetStyle(BUTTON, BORDER_COLOR_NORMAL));
        }

        DrawRectangle(pointRect.x, pointRect.y, pointRect.width, pointRect.height, pointColor);
        DrawRectangleLines(pointRect.x, pointRect.y, pointRect.width, pointRect.height, pointBorderColor);

    }

    // Draw curve
    Color curveColor = GetColor(GuiGetStyle(LABEL,  TEXT_COLOR_FOCUSED));
    if(state->numPoints == 1){
        const GuiCurveEditPoint* p = sortedPoints[0];
        const Vector2 screenPos = (Vector2){p->position.x*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-p->position.y*innerBounds.height};
        DrawLine(innerBounds.x, screenPos.y, innerBounds.x+innerBounds.width, screenPos.y, curveColor);
    }else {
        for(int i=0; i < state->numPoints-1; i++){
            const GuiCurveEditPoint* p1 = sortedPoints[i];
            const GuiCurveEditPoint* p2 = sortedPoints[i+1];
            const Vector2 screenPos1 = (Vector2){p1->position.x*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-p1->position.y*innerBounds.height};
            const Vector2 screenPos2 = (Vector2){p2->position.x*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-p2->position.y*innerBounds.height};
            // Constant on edge
            if(screenPos1.x > innerBounds.x && i == 0){
                DrawLine(innerBounds.x, screenPos1.y, screenPos1.x, screenPos1.y, curveColor);
            }
            if(screenPos2.x < innerBounds.x+innerBounds.width && i == (state->numPoints-2)){
                DrawLine(screenPos2.x, screenPos2.y, innerBounds.x+innerBounds.width, screenPos2.y, curveColor);
            }
            // Draw cubic Hermite curve
            const float scale = (p2->position.x-p1->position.x)/3.f;
            const Vector2 offset1 = (Vector2) {scale, scale*p1->tangents.y};
            // negative endTangent => top part => need to invert value to calculate offset
            const Vector2 offset2 = (Vector2) {-scale, -scale*p2->tangents.x};

            const Vector2 c1 = (Vector2) {p1->position.x+offset1.x, p1->position.y+offset1.y};
            const Vector2 c2 = (Vector2) {p2->position.x+offset2.x, p2->position.y+offset2.y};

            const Vector2 screenC1 = (Vector2) {c1.x*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-c1.y*innerBounds.height};
            const Vector2 screenC2 = (Vector2) {c2.x*innerBounds.width+innerBounds.x, innerBounds.y+innerBounds.height-c2.y*innerBounds.height};

            DrawLineBezierCubic(screenPos1, screenPos2, screenC1, screenC2, 1, curveColor);
        }
    }
}

#endif // GUI_CURVE_EDIT_IMPLEMENTATION

