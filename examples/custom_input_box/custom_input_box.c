/*******************************************************************************************
*
*   raygui - basic calculator app with custom input box for float values
*
*   DEPENDENCIES:
*       raylib 4.5  - Windowing/input management and drawing.
*       raygui 3.5  - Immediate-mode GUI controls.
*
*   COMPILATION (Windows - MinGW):
*       gcc -o $(NAME_PART).exe $(FILE_NAME) -I../../src -lraylib -lopengl32 -lgdi32 -std=c99
*
**********************************************************************************************/

#include <raylib.h>

#define RAYGUI_IMPLEMENTATION
#include <raygui.h>

int guiFloatingPointIndex = 0;                                                                                // Global variable shared by all GuiFLoatBox()

float TextToFloat(const char* text);                                                                          // Helper function that converts text to float
int GuiFloatBox(Rectangle bounds, const char* text, float* value, int minValue, int maxValue, bool editMode); // Custom input box that works with float values. Basicly GuiValueBox(), but with some changes

int main()
{
	InitWindow(250, 100, "Basic calculator");

	// General variables
	SetTargetFPS(60);

	float variableA = 0.0f;
	float variableB = 0.0f;
	float result = 0.0f;
	char operation[2];
	operation[0] = '+';
	operation[1] = '\0';

	bool variableAMode = false;
	bool variableBMode = false;
	//--------------------------------------------------------------------------------------

	// Main game loop
	while (!WindowShouldClose())
	{
		// Draw 
		//----------------------------------------------------------------------------------
		BeginDrawing();

		ClearBackground(RAYWHITE);

		if (GuiFloatBox((Rectangle){ 10, 10, 100, 20 }, NULL, &variableA, -1000000.0, 1000000.0, variableAMode)) variableAMode = !variableAMode;
		if (GuiFloatBox((Rectangle){ 140, 10, 100, 20 }, NULL, &variableB, -1000000.0, 1000000.0, variableBMode)) variableBMode = !variableBMode;
		
		if (GuiButton((Rectangle){ 10, 70, 50, 20 }, "+"))
		{
			result = variableA + variableB;
			operation[0] = '+';
		}
		if (GuiButton((Rectangle){ 70, 70, 50, 20 }, "-")) 
		{
			result = variableA - variableB;
			operation[0] = '-';
		}
		if (GuiButton((Rectangle){ 130, 70, 50, 20 }, "*")) 
		{
			result = variableA * variableB;
			operation[0] = '*';
		}
		if (GuiButton((Rectangle){ 190, 70, 50, 20 }, "/")) 
		{
			result = variableA / variableB;
			operation[0] = '/';
		}

		DrawText(operation, 123, 15, 10, DARKGRAY);
		
		GuiFloatBox((Rectangle){ 55, 40, 135, 20 }, "= ", &result, -2000000.0, 2000000.0, false);
		
		EndDrawing();
        //----------------------------------------------------------------------------------
	}

	CloseWindow();
}

// Get float value from text
float TextToFloat(const char* text)
{
	float value = 0.0f;
	float floatingPoint = 0.0f;
	int sign = 1;

    // deal with the sign
	if ((text[0] == '+') || (text[0] == '-'))
	{
		if (text[0] == '-') sign = -1;
		text++;
	}

    // convert text to float
	for (int i = 0; (((text[i] >= '0') && (text[i] <= '9')) || text[i] == '.'); i++)
	{
		if (text[i] == '.')
		{
			if (floatingPoint > 0.0f) break;

			floatingPoint = 10.0f;
			continue;
		}
		if (floatingPoint > 0.0f) // after encountering decimal separator
		{
			value += (float)(text[i] - '0') / floatingPoint;
			floatingPoint *= 10.0f;
		}
		else                      // before decimal separator
			value = value * 10.0f + (float)(text[i] - '0');
	}

	return value * sign;
}

// Float Box control, updates input text with numbers
int GuiFloatBox(Rectangle bounds, const char* text, float* value, int minValue, int maxValue, bool editMode)
{
#if !defined(RAYGUI_VALUEBOX_MAX_CHARS)
#define RAYGUI_VALUEBOX_MAX_CHARS  32
#endif

    int result = 0;
    GuiState state = guiState;

    char textValue[RAYGUI_VALUEBOX_MAX_CHARS + 1] = "\0";

    Rectangle textBounds = { 0 };
    if (text != NULL)
    {
        textBounds.width = (float)GetTextWidth(text) + 2;
        textBounds.height = (float)GuiGetStyle(DEFAULT, TEXT_SIZE);
        textBounds.x = bounds.x + bounds.width + GuiGetStyle(VALUEBOX, TEXT_PADDING);
        textBounds.y = bounds.y + bounds.height / 2 - GuiGetStyle(DEFAULT, TEXT_SIZE) / 2;
        if (GuiGetStyle(VALUEBOX, TEXT_ALIGNMENT) == TEXT_ALIGN_LEFT) textBounds.x = bounds.x - textBounds.width - GuiGetStyle(VALUEBOX, TEXT_PADDING);
    }

    // Update control
    //--------------------------------------------------------------------
    if ((state != STATE_DISABLED) && !guiLocked && !guiSliderDragging)
    {
        Vector2 mousePoint = GetMousePosition();

        if (*value >= 0) sprintf(textValue, "+%.3f", *value);
        else sprintf(textValue, "%.3f", *value);

        bool valueHasChanged = false;

        int keyCount = (int)strlen(textValue) - guiFloatingPointIndex;

        if (editMode)
        {
            state = STATE_PRESSED;

            // Only allow keys in range [48..57]
            if (keyCount < RAYGUI_VALUEBOX_MAX_CHARS)
            {
                if (GetTextWidth(textValue) < bounds.width)
                {
                    int key = GetCharPressed();
                    if ((key >= 48) && (key <= 57) && guiFloatingPointIndex)
                    {
                        if (guiFloatingPointIndex && guiFloatingPointIndex != 4) guiFloatingPointIndex--;

                        textValue[keyCount] = (char)key;
                        textValue[++keyCount] = '\0';
                        valueHasChanged = true;
                    }
                }
            }

            // Delete text
            if (keyCount > 0)
            {
                if (IsKeyPressed(KEY_BACKSPACE))
                {
                    if (guiFloatingPointIndex < 4) guiFloatingPointIndex++;

                    keyCount--;
                    textValue[keyCount] = '\0';
                    valueHasChanged = true;
                }
            }

            // Change sign
            if (IsKeyPressed(KEY_MINUS))
            {
                if (textValue[0] == '+') textValue[0] = '-';
                else if (textValue[0] == '-') textValue[0] = '+';
                valueHasChanged = true;
            }

            // Add decimal separator
            if ((IsKeyPressed(KEY_COMMA) || IsKeyPressed(KEY_PERIOD)) && guiFloatingPointIndex == 4)
            {
                guiFloatingPointIndex--;
                valueHasChanged = true;
            }

            if (valueHasChanged)
            {
                *value = TextToFloat(textValue);
            }

            if (IsKeyPressed(KEY_ENTER) || (!CheckCollisionPointRec(mousePoint, bounds) && IsMouseButtonPressed(MOUSE_LEFT_BUTTON)))
            {
                guiFloatingPointIndex = 0;
                result = 1;
            }
        }
        else
        {
            if (*value > maxValue) *value = maxValue;
            else if (*value < minValue) *value = minValue;

            if (CheckCollisionPointRec(mousePoint, bounds))
            {
                state = STATE_FOCUSED;
                if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) result = 1;
            }
        }
    }
    //--------------------------------------------------------------------

    // Draw control
    //--------------------------------------------------------------------
    Color baseColor = BLANK;
    sprintf(textValue, "%.3f", *value);

    if (state == STATE_PRESSED)
    {
        baseColor = GetColor(GuiGetStyle(VALUEBOX, BASE_COLOR_PRESSED));
        textValue[(int)strlen(textValue) - guiFloatingPointIndex] = '\0';
    }
    else if (state == STATE_DISABLED) baseColor = GetColor(GuiGetStyle(VALUEBOX, BASE_COLOR_DISABLED));

    // WARNING: BLANK color does not work properly with Fade()
    GuiDrawRectangle(bounds, GuiGetStyle(VALUEBOX, BORDER_WIDTH), Fade(GetColor(GuiGetStyle(VALUEBOX, BORDER + (state * 3))), guiAlpha), baseColor);
    GuiDrawText(textValue, GetTextBounds(VALUEBOX, bounds), TEXT_ALIGN_CENTER, Fade(GetColor(GuiGetStyle(VALUEBOX, TEXT + (state * 3))), guiAlpha));

    // Draw cursor
    if (editMode)
    {
        // NOTE: ValueBox internal text is always centered
        Rectangle cursor = { bounds.x + GetTextWidth(textValue) / 2 + bounds.width / 2 + 1, bounds.y + 2 * GuiGetStyle(VALUEBOX, BORDER_WIDTH), 4, bounds.height - 4 * GuiGetStyle(VALUEBOX, BORDER_WIDTH) };
        GuiDrawRectangle(cursor, 0, BLANK, Fade(GetColor(GuiGetStyle(VALUEBOX, BORDER_COLOR_PRESSED)), guiAlpha));
    }

    // Draw text label if provided
    GuiDrawText(text, textBounds, (GuiGetStyle(VALUEBOX, TEXT_ALIGNMENT) == TEXT_ALIGN_RIGHT) ? TEXT_ALIGN_LEFT : TEXT_ALIGN_RIGHT, Fade(GetColor(GuiGetStyle(LABEL, TEXT + (state * 3))), guiAlpha));
    //--------------------------------------------------------------------

    return result;
}
