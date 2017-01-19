/*******************************************************************************************
*
*   rFXGen 1.0 - raylib FX sounds generator (based on Tomas Petterson sfxr)
*
*   DEPENDENCIES:
*
*   raylib 1.7 - This program uses latest raylib audio module functionality.
*   raygui 1.0 - Simple IMGUI library (based on raylib)
*   tinyfiledialogs 2.7.2 - Open/save file dialogs, it requires linkage with comdlg32 and ole32 libs.
*
*   VERSIONS HISTORY:
*
*   1.0  (19-Jan-2017) First stable version
*   0.XX (XX-Jan-2017) Review complete file...
*   0.95 (14-Sep-2016) Reviewed comments and .rfx format
*   0.9  (12-Sep-2016) Defined WaveParams struct and command line functionality
*   0.8  (09-Sep-2016) Added open/save file dialogs using tinyfiledialogs library
*   0.7  (04-Sep-2016) Program variables renaming for consistency, code reorganized
*   0.6  (30-Aug-2016) Interface redesigned (reduced size) and new features added (wave drawing)
*   0.5  (27-Aug-2016) Completed port and adaptation from sfxr (only sound generation and playing)
*
*   COMPILATION (MinGW 5.3):
*
*   gcc -o rfxgen.exe rfxen.c external/tinyfiledialogs.c -s -I../.. -lraylib -lglfw3 -lopengl32 -lgdi32 / 
*       -lopenal32 -lwinmm -lcomdlg32 -lole32 -std=c99 -Wl,--subsystem,windows -Wl,-allow-multiple-definition
*
*
*   LICENSE: zlib/libpng
*
*   Copyright (c) 2014-2017 Ramon Santamaria (@raysan5)
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

//#define RAYGUI_STYLE_DEFAULT_DARK
#define RAYGUI_NO_STYLE_SAVE_LOAD       // Avoid compiling style load/save code
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "external/tinyfiledialogs.h"   // Required for native open/save file dialogs

#include <math.h>                       // Required for: sinf(), pow()
#include <stdlib.h>                     // Required for: malloc(), free()
#include <string.h>                     // Required for: strcmp()
#include <stdio.h>                      // Required for: FILE, fopen(), fread(), fwrite(), ftell(), fseek() fclose()
                                        // NOTE: Used on functions: LoadSound(), SaveSound(), WriteWAV()
                                    
#include "twitter.h"                    // Twitter icon embedded

#if defined(_WIN32)
    #include <direct.h>
    #define GetCurrentDir _getcwd
#else
    #include <unistd.h>
    #define GetCurrentDir getcwd
#endif

//----------------------------------------------------------------------------------
// Defines and Macros
//----------------------------------------------------------------------------------
#define rnd(n) GetRandomValue(0, n)
#define frnd(range) ((float)rnd(10000)/10000.0f*range)

// Wave parameters type (92 bytes)
typedef struct WaveParams {
    
    // Wave type (square, sawtooth, sine, noise)
    int waveTypeValue;

    // Wave envelope parameters
    float attackTimeValue;
    float sustainTimeValue;
    float sustainPunchValue;
    float decayTimeValue;

    // Frequency parameters
    float startFrequencyValue;
    float minFrequencyValue;
    float slideValue;
    float deltaSlideValue;
    float vibratoDepthValue;
    float vibratoSpeedValue;
    //float vibratoPhaseDelayValue;

    // Tone change parameters
    float changeAmountValue;
    float changeSpeedValue;

    // Square wave parameters
    float squareDutyValue;
    float dutySweepValue;

    // Repeat parameters
    float repeatSpeedValue;

    // Phaser parameters
    float phaserOffsetValue;
    float phaserSweepValue;

    // Filter parameters
    float lpfCutoffValue;
    float lpfCutoffSweepValue;
    float lpfResonanceValue;
    float hpfCutoffValue;
    float hpfCutoffSweepValue;

} WaveParams;

//----------------------------------------------------------------------------------
// Global Variables Definition
//----------------------------------------------------------------------------------

// Volume parameters
static float volumeValue = 0.6f;    // Volume

// Export WAV variables
static int wavSampleSize = 16;      // Wave sample size in bits (bitrate)
static int wavSampleRate = 44100;   // Wave sample rate (frequency)

// Wave parameters
static WaveParams params;           // Stores wave parameters for generation
static bool regenerate = false;     // Wave regeneration required

static char currentPath[256];       // Path to current working folder

//----------------------------------------------------------------------------------
// Module Functions Declaration
//----------------------------------------------------------------------------------
static void ResetParams(WaveParams *params);        // Reset wave parameters
static Wave GenerateWave(WaveParams params);        // Generate wave data from parameters

static WaveParams LoadSoundParams(const char *fileName);                // Load sound parameters from file
static void SaveSoundParams(const char *fileName, WaveParams params);   // Save sound parameters to file

static void SaveWAV(const char *fileName, Wave wave);                   // Export sound to .wav file
static void DrawWave(Wave *wave, Rectangle bounds, Color color);        // Draw wave data using lines
static const char *GetExtension(const char *fileName);                  // Get extension from filename

// Buttons functions
static void BtnPickupCoin(void);    // Generate sound: Pickup/Coin
static void BtnLaserShoot(void);    // Generate sound: Laser shoot 
static void BtnExplosion(void);     // Generate sound: Explosion
static void BtnPowerup(void);       // Generate sound: Powerup
static void BtnHitHurt(void);       // Generate sound: Hit/Hurt
static void BtnJump(void);          // Generate sound: Jump
static void BtnBlipSelect(void);    // Generate sound: Blip/Select
static void BtnRandomize(void);     // Generate random sound
static void BtnMutate(void);;       // Mutate current sound

static void BtnLoadSound(void);     // Load sound parameters file
static void BtnSaveSound(void);     // Save sound parameters file
static void BtnExportWav(Wave wave); // Export current sound as .wav

//------------------------------------------------------------------------------------
// Program main entry point
//------------------------------------------------------------------------------------
int main(int argc, char *argv[])
{
    // Command line utility to generate wav files directly from .sfs and .rfx
    // NOTE: .sfs uses defaults: sampleRate = 22050, sampleSize = 8, channels = 1;
    // .rfx files contain sampleRate, sampleSize and channels information
    if (argc > 1)
    {
        for (int i = 1; i < argc; i++)
        {
            if ((strcmp(GetExtension(argv[i]), "rfx") == 0) ||
                (strcmp(GetExtension(argv[i]), "sfs") == 0))
            {
                params = LoadSoundParams(argv[i]);
                Wave wave = GenerateWave(params);
                
                // Format wave data to desired sampleSize and channels
                // NOTE: wavSampleRate and wavSampleSize are loaded from rfx or default values are used
                WaveFormat(&wave, wavSampleRate, wavSampleSize, 1);
                
                argv[i][strlen(argv[i]) - 3] = 'w';
                argv[i][strlen(argv[i]) - 2] = 'a';
                argv[i][strlen(argv[i]) - 1] = 'v';
                
                SaveWAV(argv[i], wave);
                UnloadWave(wave);
            }
        }
        
        return 0;
    }

    // Initialization
    //----------------------------------------------------------------------------------------
    int screenWidth = 500;
    int screenHeight = 500;

    //SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(screenWidth, screenHeight, "rFXGen - raylib FX Sound Generator");

    InitAudioDevice();

    Rectangle paramsRec = { 117, 43, 265, 373 };
    
    // Twitter logo image (grayscale)
    Image mask;
    mask.width = 16;
    mask.height = 16;
    mask.mipmaps = 1;
    mask.format = UNCOMPRESSED_GRAYSCALE;
    mask.data = imTwitter;
    
    Color *pixels = (Color *)malloc(16*16*sizeof(Color));
    for (int i = 0; i < 16*16; i++) pixels[i] = WHITE;
    Image twitter = LoadImageEx(pixels, 16, 16);
    free(pixels);
    
    ImageAlphaMask(&twitter, mask);      // Add alpha mask to image
    ImageFormat(&twitter, UNCOMPRESSED_GRAY_ALPHA);
    
    Texture2D texTwitter = LoadTextureFromImage(twitter);
    UnloadImage(twitter);
    
    // Label data
    //----------------------------------------------------------------------------------------
    Rectangle lblAttackTimeRec = { paramsRec.x + 33, paramsRec.y + 5, 100, 10 };
    Rectangle lblSustainTimeRec = { paramsRec.x + 31, paramsRec.y + 20, 100, 10 };
    Rectangle lblSustainPunchRec = { paramsRec.x + 27, paramsRec.y + 35, 100, 10 };
    Rectangle lblDecayTimeRec = { paramsRec.x + 37, paramsRec.y + 50, 100, 10 };

    Rectangle lblStartFrequencyRec = { paramsRec.x + 7, paramsRec.y + 66 + 5, 100, 10 };
    Rectangle lblMinFrequencyRec = { paramsRec.x + 27, paramsRec.y + 66 + 20, 100, 10 };
    Rectangle lblSlideRec = { paramsRec.x + 55, paramsRec.y + 66 + 35, 100, 10 };
    Rectangle lblDeltaSlideRec = { paramsRec.x + 35, paramsRec.y + 66 + 50, 100, 10 };
    Rectangle lblVibratoDepthRec = { paramsRec.x + 26, paramsRec.y + 66 + 65, 100, 10 };
    Rectangle lblVibratoSpeedRec = { paramsRec.x + 27, paramsRec.y + 66 + 80, 100, 10 };

    Rectangle lblChangeAmountRec = { paramsRec.x + 25, paramsRec.y + 162 + 5, 100, 10 };
    Rectangle lblChangeSpeedRec = { paramsRec.x + 30, paramsRec.y + 162 + 20, 100, 10 };

    Rectangle lblSquareDutyRec = { paramsRec.x + 33, paramsRec.y + 198 + 5, 100, 10 };
    Rectangle lblDutySweepRec = { paramsRec.x + 36, paramsRec.y + 198 + 20, 100, 10 };

    Rectangle lblRepeatSpeedRec = { paramsRec.x + 29, paramsRec.y + 234 + 5, 100, 10 };

    Rectangle lblPhaserOffsetRec = { paramsRec.x + 26, paramsRec.y + 255 + 5, 100, 10 };
    Rectangle lblPhaserSweepRec = { paramsRec.x + 29, paramsRec.y + 255 + 20, 100, 10 };

    Rectangle lblLpfCutoffRec = { paramsRec.x + 37, paramsRec.y + 291 + 5, 100, 10 };
    Rectangle lblLpfCutoffSweepRec = { paramsRec.x + 4, paramsRec.y + 291 + 20, 100, 10 };
    Rectangle lblLpfResonanceRec = { paramsRec.x + 27, paramsRec.y + 291 + 35, 100, 10 };
    Rectangle lblHpfCutoffRec = { paramsRec.x + 36, paramsRec.y + 291 + 50, 100, 10 };
    Rectangle lblHpfCutoffSweepRec = { paramsRec.x + 3, paramsRec.y + 291 + 65, 100, 10 };
    //----------------------------------------------------------------------------------------

    // SliderBar data
    //----------------------------------------------------------------------------------------
    Rectangle sldrAttackTimeRec = { paramsRec.x + 127, paramsRec.y + 5, 100, 10 };
    Rectangle sldrSustainTimeRec = { paramsRec.x + 127, paramsRec.y + 20, 100, 10 };
    Rectangle sldrSustainPunchRec = { paramsRec.x + 127, paramsRec.y + 35, 100, 10 };
    Rectangle sldrDecayTimeRec = { paramsRec.x + 127, paramsRec.y + 50, 100, 10 };

    Rectangle sldrStartFrequencyRec = { paramsRec.x + 127, paramsRec.y + 66 + 5, 100, 10 };
    Rectangle sldrMinFrequencyRec = { paramsRec.x + 127, paramsRec.y + 66 + 20, 100, 10 };
    Rectangle sldrSlideRec = { paramsRec.x + 127, paramsRec.y + 66 + 35, 100, 10 };
    Rectangle sldrDeltaSlideRec = { paramsRec.x + 127, paramsRec.y + 66 + 50, 100, 10 };
    Rectangle sldrVibratoDepthRec = { paramsRec.x + 127, paramsRec.y + 66 + 65, 100, 10 };
    Rectangle sldrVibratoSpeedRec = { paramsRec.x + 127, paramsRec.y + 66 + 80, 100, 10 };

    Rectangle sldrChangeAmountRec = { paramsRec.x + 127, paramsRec.y + 162 + 5, 100, 10 };
    Rectangle sldrChangeSpeedRec = { paramsRec.x + 127, paramsRec.y + 162 + 20, 100, 10 };

    Rectangle sldrSquareDutyRec = { paramsRec.x + 127, paramsRec.y + 198 + 5, 100, 10 };
    Rectangle sldrDutySweepRec = { paramsRec.x + 127, paramsRec.y + 198 + 20, 100, 10 };

    Rectangle sldrRepeatSpeedRec = { paramsRec.x + 127, paramsRec.y + 234 + 5, 100, 10 };

    Rectangle sldrPhaserOffsetRec = { paramsRec.x + 127, paramsRec.y + 255 + 5, 100, 10 };
    Rectangle sldrPhaserSweepRec = { paramsRec.x + 127, paramsRec.y + 255 + 20, 100, 10 };

    Rectangle sldrLpfCutoffRec = { paramsRec.x + 127, paramsRec.y + 291 + 5, 100, 10 };
    Rectangle sldrLpfCutoffSweepRec = { paramsRec.x + 127, paramsRec.y + 291 + 20, 100, 10 };
    Rectangle sldrLpfResonanceRec = { paramsRec.x + 127, paramsRec.y + 291 + 35, 100, 10 };
    Rectangle sldrHpfCutoffRec = { paramsRec.x + 127, paramsRec.y + 291 + 50, 100, 10 };
    Rectangle sldrHpfCutoffSweepRec = { paramsRec.x + 127, paramsRec.y + 291 + 65, 100, 10 };

    Rectangle sldrVolumeRec = { 394, 65, 92, 10 };
    //----------------------------------------------------------------------------------------

    // Button data
    //----------------------------------------------------------------------------------------
    Rectangle btnPickupCoinRec = { 13, 48, 92, 20 };
    Rectangle btnLaserShootRec = { 13, 73, 92, 20 };
    Rectangle btnExplosionRec = { 13, 98, 92, 20 };
    Rectangle btnPowerupRec = { 13, 123, 92, 20 };
    Rectangle btnHitHurtRec = { 13, 148, 92, 20 };
    Rectangle btnJumpRec = { 13, 173, 92, 20 };
    Rectangle btnBlipSelectRec = { 13, 198, 92, 20 };
    Rectangle btnMutateRec = { 13, 364, 92, 20 };
    Rectangle btnRandomizeRec = { 13, 389, 92, 20 };
    
    Rectangle btnPlaySoundRec = { 394, 81, 92, 20 };
    Rectangle btnLoadSoundRec = { 394, 283, 92, 20 };
    Rectangle btnSaveSoundRec = { 394, 307, 92, 20 };
    Rectangle btnExportWavRec = { 394, 389, 92, 20 };
    //----------------------------------------------------------------------------------------
    
    // CheckBox data
    //----------------------------------------------------------------------------------------
    Rectangle chkboxPlayOnChangeRec = { 394, 115, 10, 10 };
    bool playOnChangeValue = true;
    //----------------------------------------------------------------------------------------

    // ComboBox data
    //----------------------------------------------------------------------------------------
    Rectangle comboxSampleRateRec = { 394, 340, 61, 20 };
    Rectangle comboxSampleSizeRec = { 394, 364, 61, 20 };
    char *comboxSampleRateText[2] = { "22050 Hz", "44100 Hz" };
    char *comboxSampleSizeText[3] = { "8 bit", "16 bit", "32 bit" };
    int comboxSampleRateValue = 1;
    int comboxSampleSizeValue = 1;
    //----------------------------------------------------------------------------------------

    // ToggleGroup data
    //----------------------------------------------------------------------------------------
    Rectangle tgroupWaveTypeRec = { 117, 15, 64, 20 };
    char *tgroupWaveTypeText[4] = { "Square", "Sawtooth", "Sinewave", "Noise" };
    //----------------------------------------------------------------------------------------

    Wave wave;
    
    // Default wave values
    wave.sampleRate = 44100;
    wave.sampleSize = 32;       // 32 bit -> float
    wave.channels = 1;
    wave.sampleCount = 10*wave.sampleRate*wave.channels;    // Max sampleCount for 10 seconds
    wave.data = (float *)malloc(wave.sampleCount*sizeof(float));
    
    Sound sound; 
    sound = LoadSoundFromWave(wave);
    SetSoundVolume(sound, volumeValue);

    ResetParams(&params);
    
    Rectangle waveRec = { 13, 421, 473, 50 };
    
#define RENDER_WAVE_TO_TEXTURE
#if defined(RENDER_WAVE_TO_TEXTURE)
    // To avoid enabling MSXAAx4, we will render wave to a texture x2
    RenderTexture2D waveTarget = LoadRenderTexture(waveRec.width*2, waveRec.height*2);
#endif

    // Get current directory
    // NOTE: Current working directory could not match current executable directory
    GetCurrentDir(currentPath, sizeof(currentPath));
    currentPath[strlen(currentPath)] = '\\';
    currentPath[strlen(currentPath) + 1] = '\0';      // Not really required

    SetTargetFPS(60);
    //----------------------------------------------------------------------------------------

    // Main game loop
    while (!WindowShouldClose())    // Detect window close button or ESC key
    {
        // Update & Draw
        //------------------------------------------------------------------------------------
        if (IsKeyPressed(KEY_SPACE)) PlaySound(sound);
        
        // Consider two possible cases to regenerate wave and update sound:
        // CASE1: regenerate flag is true (set by sound buttons functions)
        // CASE2: Mouse is moving sliders and mouse is released (checks against all sliders box - a bit crappy solution...)
        if (regenerate || ((CheckCollisionPointRec(GetMousePosition(), (Rectangle){ 243, 48, 102, 362 })) && (IsMouseButtonReleased(MOUSE_LEFT_BUTTON))))
        {
            // Generate new wave and update sound
            free(wave.data);
            wave = GenerateWave(params);                        // Generate wave from parameters
            UpdateSound(sound, wave.data, wave.sampleCount);    // Update sound buffer with new data
            
            if (regenerate || playOnChangeValue) PlaySound(sound);
            regenerate = false;
        }

        BeginDrawing();

            ClearBackground(GuiBackgroundColor());

            DrawRectangleLines(paramsRec.x, paramsRec.y - 1, paramsRec.width, paramsRec.height + 1, GuiLinesColor());
            
            DrawLine(paramsRec.x, paramsRec.y + 66, paramsRec.x + paramsRec.width, paramsRec.y + 66, GuiLinesColor());
            DrawLine(paramsRec.x, paramsRec.y + 66 + 96, paramsRec.x + paramsRec.width, paramsRec.y + 66 + 96, GuiLinesColor());
            DrawLine(paramsRec.x, paramsRec.y + 162 + 36, paramsRec.x + paramsRec.width, paramsRec.y + 162 + 36, GuiLinesColor());
            DrawLine(paramsRec.x, paramsRec.y + 198 + 36, paramsRec.x + paramsRec.width, paramsRec.y + 198 + 36, GuiLinesColor());
            DrawLine(paramsRec.x, paramsRec.y + 234 + 21, paramsRec.x + paramsRec.width, paramsRec.y + 234 + 21, GuiLinesColor());
            DrawLine(paramsRec.x, paramsRec.y + 291, paramsRec.x + paramsRec.width, paramsRec.y + 291, GuiLinesColor());
            
            DrawLine(13, 225, 105, 224, GuiLinesColor());
            DrawLine(13, 358, 105, 358, GuiLinesColor());
            DrawLine(394, 108, 486, 108, GuiLinesColor());
            DrawLine(394, 277, 486, 277, GuiLinesColor());
            DrawLine(394, 334, 486, 334, GuiLinesColor());
            
            // Labels
            //--------------------------------------------------------------------------------
            DrawText("rFXGen", 28, 19, 20, DARKGRAY);
            
            GuiLabel(lblAttackTimeRec, "ATTACK TIME");
            GuiLabel(lblSustainTimeRec, "SUSTAIN TIME");
            GuiLabel(lblSustainPunchRec, "SUSTAIN PUNCH");
            GuiLabel(lblDecayTimeRec, "DECAY TIME");
            //      ---------
            GuiLabel(lblStartFrequencyRec, "START FREQUENCY");
            GuiLabel(lblMinFrequencyRec, "MIN FREQUENCY");
            GuiLabel(lblSlideRec, "SLIDE");
            GuiLabel(lblDeltaSlideRec, "DELTA SLIDE");
            GuiLabel(lblVibratoDepthRec, "VIBRATO DEPTH");
            GuiLabel(lblVibratoSpeedRec, "VIBRATO SPEED");
            //      ---------
            GuiLabel(lblChangeAmountRec, "CHANGE AMOUNT");
            GuiLabel(lblChangeSpeedRec, "CHANGE SPEED");
            //      ---------
            GuiLabel(lblSquareDutyRec, "SQUARE DUTY");
            GuiLabel(lblDutySweepRec, "DUTY SWEEP");
            //      ---------
            GuiLabel(lblRepeatSpeedRec, "REPEAT SPEED");
            //      ---------
            GuiLabel(lblPhaserOffsetRec, "PHASER OFFSET");
            GuiLabel(lblPhaserSweepRec, "PHASER SWEEP");
            //      ---------
            GuiLabel(lblLpfCutoffRec, "LPF CUTOFF");
            GuiLabel(lblLpfCutoffSweepRec, "LPF CUTOFF SWEEP");
            GuiLabel(lblLpfResonanceRec, "LPF RESONANCE");
            GuiLabel(lblHpfCutoffRec, "HPF CUTOFF");
            GuiLabel(lblHpfCutoffSweepRec, "HPF CUTOFF SWEEP");
            //--------------------------------------------------------------------------------

            // Sliders
            //--------------------------------------------------------------------------------
            params.attackTimeValue = GuiSliderBar(sldrAttackTimeRec, params.attackTimeValue, 0, 1);
            params.sustainTimeValue = GuiSliderBar(sldrSustainTimeRec, params.sustainTimeValue, 0, 1);
            params.sustainPunchValue = GuiSliderBar(sldrSustainPunchRec, params.sustainPunchValue, 0, 1);
            params.decayTimeValue = GuiSliderBar(sldrDecayTimeRec, params.decayTimeValue, 0, 1);
            //      --------------
            params.startFrequencyValue = GuiSliderBar(sldrStartFrequencyRec, params.startFrequencyValue, 0, 1);
            params.minFrequencyValue = GuiSliderBar(sldrMinFrequencyRec, params.minFrequencyValue, 0, 1);
            params.slideValue = GuiSliderBar(sldrSlideRec, params.slideValue, -1, 1);
            params.deltaSlideValue = GuiSliderBar(sldrDeltaSlideRec, params.deltaSlideValue, -1, 1);
            params.vibratoDepthValue = GuiSliderBar(sldrVibratoDepthRec, params.vibratoDepthValue, 0, 1);
            params.vibratoSpeedValue = GuiSliderBar(sldrVibratoSpeedRec, params.vibratoSpeedValue, 0, 1);
            //      --------------
            params.changeAmountValue = GuiSliderBar(sldrChangeAmountRec, params.changeAmountValue, -1, 1);
            params.changeSpeedValue = GuiSliderBar(sldrChangeSpeedRec, params.changeSpeedValue, 0, 1);
            //      --------------
            params.squareDutyValue = GuiSliderBar(sldrSquareDutyRec, params.squareDutyValue, 0, 1);
            params.dutySweepValue = GuiSliderBar(sldrDutySweepRec, params.dutySweepValue, -1, 1);
            //      --------------
            params.repeatSpeedValue = GuiSliderBar(sldrRepeatSpeedRec, params.repeatSpeedValue, 0, 1);
            //     ---------------
            params.phaserOffsetValue = GuiSliderBar(sldrPhaserOffsetRec, params.phaserOffsetValue, -1, 1);
            params.phaserSweepValue = GuiSliderBar(sldrPhaserSweepRec, params.phaserSweepValue, -1, 1);
            //     ---------------
            params.lpfCutoffValue = GuiSliderBar(sldrLpfCutoffRec, params.lpfCutoffValue, 0, 1);
            params.lpfCutoffSweepValue = GuiSliderBar(sldrLpfCutoffSweepRec, params.lpfCutoffSweepValue, -1, 1);
            params.lpfResonanceValue = GuiSliderBar(sldrLpfResonanceRec, params.lpfResonanceValue, 0, 1);
            params.hpfCutoffValue = GuiSliderBar(sldrHpfCutoffRec, params.hpfCutoffValue, 0, 1);
            params.hpfCutoffSweepValue = GuiSliderBar(sldrHpfCutoffSweepRec, params.hpfCutoffSweepValue, -1, 1);
            //     ----------------
            float previousVolumeValue = volumeValue;
            volumeValue = GuiSliderBar(sldrVolumeRec, volumeValue, 0, 1);
            if (volumeValue != previousVolumeValue) SetSoundVolume(sound, volumeValue);
            //--------------------------------------------------------------------------------
            
            // Slider values
            //--------------------------------------------------------------------------------
            DrawText(FormatText("%.02f", params.attackTimeValue), sldrAttackTimeRec.x + sldrAttackTimeRec.width + 7, sldrAttackTimeRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.sustainTimeValue), sldrSustainTimeRec.x + sldrSustainTimeRec.width + 7, sldrSustainTimeRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.sustainPunchValue), sldrSustainPunchRec.x + sldrSustainPunchRec.width + 7, sldrSustainPunchRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.decayTimeValue), sldrDecayTimeRec.x + sldrDecayTimeRec.width + 7, sldrDecayTimeRec.y + 1, 10, DARKGRAY);
            //     ---------------
            DrawText(FormatText("%.02f", params.startFrequencyValue), sldrStartFrequencyRec.x + sldrStartFrequencyRec.width + 7, sldrStartFrequencyRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.minFrequencyValue), sldrMinFrequencyRec.x + sldrMinFrequencyRec.width + 7, sldrMinFrequencyRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.slideValue), sldrSlideRec.x + sldrSlideRec.width + 7, sldrSlideRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.deltaSlideValue), sldrDeltaSlideRec.x + sldrDeltaSlideRec.width + 7, sldrDeltaSlideRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.vibratoDepthValue), sldrVibratoDepthRec.x + sldrVibratoDepthRec.width + 7, sldrVibratoDepthRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.vibratoSpeedValue), sldrVibratoSpeedRec.x + sldrVibratoSpeedRec.width + 7, sldrVibratoSpeedRec.y + 1, 10, DARKGRAY);
            //     ---------------
            DrawText(FormatText("%.02f", params.changeAmountValue), sldrChangeAmountRec.x + sldrChangeAmountRec.width + 7, sldrChangeAmountRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.changeSpeedValue), sldrChangeSpeedRec.x + sldrChangeSpeedRec.width + 7, sldrChangeSpeedRec.y + 1, 10, DARKGRAY);
            //     ---------------
            DrawText(FormatText("%.02f", params.squareDutyValue), sldrSquareDutyRec.x + sldrSquareDutyRec.width + 7, sldrSquareDutyRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.dutySweepValue), sldrDutySweepRec.x + sldrDutySweepRec.width + 7, sldrDutySweepRec.y + 1, 10, DARKGRAY);
            //     ---------------
            DrawText(FormatText("%.02f", params.repeatSpeedValue), sldrRepeatSpeedRec.x + sldrRepeatSpeedRec.width + 7, sldrRepeatSpeedRec.y + 1, 10, DARKGRAY);
            //     ---------------
            DrawText(FormatText("%.02f", params.phaserOffsetValue), sldrPhaserOffsetRec.x + sldrPhaserOffsetRec.width + 7, sldrPhaserOffsetRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.phaserSweepValue), sldrPhaserSweepRec.x + sldrPhaserSweepRec.width + 7, sldrPhaserSweepRec.y + 1, 10, DARKGRAY);
            //     ---------------
            DrawText(FormatText("%.02f", params.lpfCutoffValue), sldrLpfCutoffRec.x + sldrLpfCutoffRec.width + 7, sldrLpfCutoffRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.lpfCutoffSweepValue), sldrLpfCutoffSweepRec.x + sldrLpfCutoffSweepRec.width + 7, sldrLpfCutoffSweepRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.lpfResonanceValue), sldrLpfResonanceRec.x + sldrLpfResonanceRec.width + 7, sldrLpfResonanceRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.hpfCutoffValue), sldrHpfCutoffRec.x + sldrHpfCutoffRec.width + 7, sldrHpfCutoffRec.y + 1, 10, DARKGRAY);
            DrawText(FormatText("%.02f", params.hpfCutoffSweepValue), sldrHpfCutoffSweepRec.x + sldrHpfCutoffSweepRec.width + 7, sldrHpfCutoffSweepRec.y + 1, 10, DARKGRAY);
            //--------------------------------------------------------------------------------

            // Buttons
            //--------------------------------------------------------------------------------
            if (GuiButton(btnPickupCoinRec, "Pickup/Coin")) BtnPickupCoin();
            if (GuiButton(btnLaserShootRec, "Laser/Shoot")) BtnLaserShoot();
            if (GuiButton(btnExplosionRec, "Explosion")) BtnExplosion();
            if (GuiButton(btnPowerupRec, "Powerup")) BtnPowerup();
            if (GuiButton(btnHitHurtRec, "Hit/Hurt")) BtnHitHurt();
            if (GuiButton(btnJumpRec, "Jump")) BtnJump();
            if (GuiButton(btnBlipSelectRec, "Blip/Select")) BtnBlipSelect();
            if (GuiButton(btnMutateRec, "Mutate")) BtnMutate();
            if (GuiButton(btnRandomizeRec, "Randomize")) BtnRandomize();
            if (GuiButton(btnPlaySoundRec, "Play Sound")) PlaySound(sound);
            if (GuiButton(btnLoadSoundRec, "Load Sound")) BtnLoadSound();
            if (GuiButton(btnSaveSoundRec, "Save Sound")) BtnSaveSound();
            if (GuiButton(btnExportWavRec, "Export .Wav")) BtnExportWav(wave);
            //--------------------------------------------------------------------------------
            
            // CheckBox
            //--------------------------------------------------------------------------------
            playOnChangeValue = GuiCheckBox(chkboxPlayOnChangeRec, " Play on change", playOnChangeValue);
            //--------------------------------------------------------------------------------

            // ComboBox
            //--------------------------------------------------------------------------------
            comboxSampleRateValue = GuiComboBox(comboxSampleRateRec, 2, comboxSampleRateText, comboxSampleRateValue);
            comboxSampleSizeValue = GuiComboBox(comboxSampleSizeRec, 3, comboxSampleSizeText, comboxSampleSizeValue);

            if (comboxSampleRateValue == 0) wavSampleRate = 22050;
            else if (comboxSampleRateValue == 1) wavSampleRate = 44100;

            if (comboxSampleSizeValue == 0) wavSampleSize = 8;
            else if (comboxSampleSizeValue == 1) wavSampleSize = 16;
            else if (comboxSampleSizeValue == 2) wavSampleSize = 32;
            //--------------------------------------------------------------------------------

            // ToggleGroup
            //--------------------------------------------------------------------------------
            int previousWaveTypeValue = params.waveTypeValue;
            params.waveTypeValue = GuiToggleGroup(tgroupWaveTypeRec, 4, tgroupWaveTypeText, params.waveTypeValue);
            if (params.waveTypeValue != previousWaveTypeValue) regenerate = true;
            //--------------------------------------------------------------------------------
            
            if (volumeValue < 1.0f) DrawText(FormatText("VOLUME:      %02i %%", (int)(volumeValue*100.0f)), 394, 49, 10, DARKGRAY);
            else DrawText(FormatText("VOLUME:     %02i %%", (int)(volumeValue*100.0f)), 394, 49, 10, DARKGRAY);

#if defined(RENDER_WAVE_TO_TEXTURE)
            BeginTextureMode(waveTarget);
                DrawWave(&wave, (Rectangle){ 0, 0, waveTarget.texture.width, waveTarget.texture.height }, MAROON);
            EndTextureMode();

            DrawTextureEx(waveTarget.texture, (Vector2){ waveRec.x, waveRec.y }, 0.0f, 0.5f, WHITE);
#else
            DrawWave(&wave, waveRec, MAROON);
#endif    
            DrawRectangleLines(waveRec.x, waveRec.y, waveRec.width, waveRec.height, GuiLinesColor());
            DrawRectangle(waveRec.x, waveRec.y + waveRec.height/2, waveRec.width, 1, LIGHTGRAY);
            
            // Draw status bar
            DrawRectangle(0, screenHeight - 20, screenWidth, 20, Fade(LIGHTGRAY, 0.5f));
            DrawLine(0, screenHeight - 20, screenWidth, screenHeight - 20, LIGHTGRAY);
            DrawText("SOUND INFO:", 28, 486, 10, DARKGRAY);
            DrawText(FormatText("num samples: %i", wave.sampleCount), 117, 486, 10, DARKGRAY);
            DrawText(FormatText("|    duration: %i ms", wave.sampleCount*1000/(wave.sampleRate*wave.channels)), 234, 486, 10, DARKGRAY);
            DrawText(FormatText("|   Wave size: %i bytes", wave.sampleCount*wavSampleSize/8), 355, 486, 10, DARKGRAY);
            
            // Adverts
            DrawText("based on sfxr by", 16, 235, 10, GRAY);
            DrawText("Tomas Pettersson", 13, 248, 10, GRAY);
            
            DrawLine(13, 268, 105, 268, GuiLinesColor());
            
            DrawText("www/github.com/\nraysan5/raygui", 18, 280, 10, GRAY);
            DrawText("www/github.com/\nraysan5/raylib", 18, 318, 10, GRAY);
            DrawText("powered by", 394, 149, 10, DARKGRAY);
            DrawRectangle(394, 162, 92, 92, BLACK);
            DrawRectangle(400, 168, 80, 80, RAYWHITE);
            DrawText("raylib", 419, 223, 20, BLACK);

            DrawText("@raysan5", 421, 21, 10, GRAY);
            DrawTexture(texTwitter, 400, 18, Fade(BLACK, 0.4f));
            
            //DrawRectangleRec((Rectangle){ 243, 48, 102, 362 }, Fade(RED, 0.2f));

        EndDrawing();
        //------------------------------------------------------------------------------------
    }

    // De-Initialization
    //----------------------------------------------------------------------------------------
    UnloadSound(sound);
    UnloadWave(wave);
    
#if defined(RENDER_WAVE_TO_TEXTURE)
    UnloadRenderTexture(waveTarget);
#endif
    UnloadTexture(texTwitter);

    CloseAudioDevice();

    CloseWindow();          // Close window and OpenGL context
    //----------------------------------------------------------------------------------------

    return 0;
}

//--------------------------------------------------------------------------------------------
// Module Functions Definitions (local)
//--------------------------------------------------------------------------------------------
static void ResetParams(WaveParams *params)
{
    // Wave type
	params->waveTypeValue = 0;
    
    // Wave envelope params
	params->attackTimeValue = 0.0f;
	params->sustainTimeValue = 0.3f;
	params->sustainPunchValue = 0.0f;
	params->decayTimeValue = 0.4f;

    // Frequency params
	params->startFrequencyValue = 0.3f;
	params->minFrequencyValue = 0.0f;
	params->slideValue = 0.0f;
	params->deltaSlideValue = 0.0f;
	params->vibratoDepthValue = 0.0f;
	params->vibratoSpeedValue = 0.0f;
	//params->vibratoPhaseDelay = 0.0f;
    
    // Tone change params
    params->changeAmountValue = 0.0f;
	params->changeSpeedValue = 0.0f;
    
    // Square wave params
    params->squareDutyValue = 0.0f;
	params->dutySweepValue = 0.0f;

    // Repeat params
	params->repeatSpeedValue = 0.0f;
    
    // Phaser params
	params->phaserOffsetValue = 0.0f;
	params->phaserSweepValue = 0.0f;
    
    // Filter params
	params->lpfCutoffValue = 1.0f;
	params->lpfCutoffSweepValue = 0.0f;
	params->lpfResonanceValue = 0.0f;
	params->hpfCutoffValue = 0.0f;
	params->hpfCutoffSweepValue = 0.0f;
}

// Generates new wave from wave parameters
// NOTE: By default wave is generated as 44100Hz, 32bit float, mono
static Wave GenerateWave(WaveParams params)
{
    #define MAX_WAVE_LENGTH_SECONDS  10     // Max length for wave: 10 seconds
    #define WAVE_SAMPLE_RATE      44100     // Default sample rate
    
    // NOTE: GetRandomValue() is provided by raylib and seed is initialized at InitWindow()
    #define GetRandomFloat(range) ((float)GetRandomValue(0, 10000)/10000.0f*range)
    
    // Configuration parameters for generation
    // NOTE: Those parameters are calculated from selected values
    int phase = 0;
    double fperiod = 0.0;
    double fmaxperiod = 0.0;
    double fslide = 0.0;
    double fdslide = 0.0;
    int period = 0;
    float squareDuty = 0.0f;
    float squareSlide = 0.0f;
    int envelopeStage = 0;
    int envelopeTime = 0;
    int envelopeLength[3] = { 0 };
    float envelopeVolume = 0.0f;
    float fphase = 0.0f;
    float fdphase = 0.0f;
    int iphase = 0;
    float phaserBuffer[1024] = { 0.0f };
    int ipp = 0;
    float noiseBuffer[32] = { 0.0f };       // Required for noise wave, depends on random seed!
    float fltp = 0.0f;
    float fltdp = 0.0f;
    float fltw = 0.0f;
    float fltwd = 0.0f;
    float fltdmp = 0.0f;
    float fltphp = 0.0f;
    float flthp = 0.0f;
    float flthpd = 0.0f;
    float vibratoPhase = 0.0f;
    float vibratoSpeed = 0.0f;
    float vibratoAmplitude = 0.0f;
    int repeatTime = 0;
    int repeatLimit = 0;
    int arpeggioTime = 0;
    int arpeggioLimit = 0;
    double arpeggioModulation = 0.0;
    
    // Reset sample parameters
    //----------------------------------------------------------------------------------------
	fperiod = 100.0/(params.startFrequencyValue*params.startFrequencyValue + 0.001);
	period = (int)fperiod;
	fmaxperiod = 100.0/(params.minFrequencyValue*params.minFrequencyValue + 0.001);
	fslide = 1.0 - pow((double)params.slideValue, 3.0)*0.01;
	fdslide = -pow((double)params.deltaSlideValue, 3.0)*0.000001;
	squareDuty = 0.5f - params.squareDutyValue*0.5f;
	squareSlide = -params.dutySweepValue*0.00005f;

	if (params.changeAmountValue >= 0.0f) arpeggioModulation = 1.0 - pow((double)params.changeAmountValue, 2.0)*0.9;
	else arpeggioModulation = 1.0 + pow((double)params.changeAmountValue, 2.0)*10.0;

	arpeggioLimit = (int)(pow(1.0f - params.changeSpeedValue, 2.0f)*20000 + 32);

	if (params.changeSpeedValue == 1.0f) arpeggioLimit = 0;

    // Reset filter parameters
    fltw = pow(params.lpfCutoffValue, 3.0f)*0.1f;
    fltwd = 1.0f + params.lpfCutoffSweepValue*0.0001f;
    fltdmp = 5.0f/(1.0f + pow(params.lpfResonanceValue, 2.0f)*20.0f)*(0.01f + fltw);
    if (fltdmp > 0.8f) fltdmp = 0.8f;
    flthp = pow(params.hpfCutoffValue, 2.0f)*0.1f;
    flthpd = 1.0 + params.hpfCutoffSweepValue*0.0003f;

    // Reset vibrato
    vibratoSpeed = pow(params.vibratoSpeedValue, 2.0f)*0.01f;
    vibratoAmplitude = params.vibratoDepthValue*0.5f;

    // Reset envelope
    envelopeLength[0] = (int)(params.attackTimeValue*params.attackTimeValue*100000.0f);
    envelopeLength[1] = (int)(params.sustainTimeValue*params.sustainTimeValue*100000.0f);
    envelopeLength[2] = (int)(params.decayTimeValue*params.decayTimeValue*100000.0f);

    fphase = pow(params.phaserOffsetValue, 2.0f)*1020.0f;
    if (params.phaserOffsetValue < 0.0f) fphase = -fphase;
    
    fdphase = pow(params.phaserSweepValue, 2.0f)*1.0f;
    if (params.phaserSweepValue < 0.0f) fdphase = -fdphase;
    
    iphase = abs((int)fphase);

    for (int i = 0; i < 32; i++) noiseBuffer[i] = GetRandomFloat(2.0f) - 1.0f;

    repeatLimit = (int)(pow(1.0f - params.repeatSpeedValue, 2.0f)*20000 + 32);

    if (params.repeatSpeedValue == 0.0f) repeatLimit = 0;
    //----------------------------------------------------------------------------------------
    
    // NOTE: We reserve enough space for up to 10 seconds of wave audio at given sample rate
    // By default we use float size samples, they are converted to desired sample size at the end
    float *buffer = (float *)malloc(MAX_WAVE_LENGTH_SECONDS*WAVE_SAMPLE_RATE*sizeof(float));
    bool generatingSample = true;
    int sampleCount = 0;

	for (int i = 0; i < MAX_WAVE_LENGTH_SECONDS*WAVE_SAMPLE_RATE; i++)
	{
		if (!generatingSample)
        {
            sampleCount = i;
            break;
        }

        // Generate sample using selected parameters
        //------------------------------------------------------------------------------------
		repeatTime++;

		if (repeatLimit != 0 && repeatTime >= repeatLimit)
		{
            // Reset sample parameters (only some of them)
			repeatTime = 0;
            
            fperiod = 100.0/(params.startFrequencyValue*params.startFrequencyValue + 0.001);
            period = (int)fperiod;
            fmaxperiod = 100.0/(params.minFrequencyValue*params.minFrequencyValue + 0.001);
            fslide = 1.0 - pow((double)params.slideValue, 3.0)*0.01;
            fdslide = -pow((double)params.deltaSlideValue, 3.0)*0.000001;
            squareDuty = 0.5f - params.squareDutyValue*0.5f;
            squareSlide = -params.dutySweepValue*0.00005f;

            if (params.changeAmountValue >= 0.0f) arpeggioModulation = 1.0 - pow((double)params.changeAmountValue, 2.0)*0.9;
            else arpeggioModulation = 1.0 + pow((double)params.changeAmountValue, 2.0)*10.0;

            arpeggioTime = 0;
            arpeggioLimit = (int)(pow(1.0f - params.changeSpeedValue, 2.0f)*20000 + 32);

            if (params.changeSpeedValue == 1.0f) arpeggioLimit = 0;
		}

		// Frequency envelopes/arpeggios
		arpeggioTime++;

		if (arpeggioLimit !=0 && arpeggioTime >= arpeggioLimit)
		{
			arpeggioLimit = 0;
			fperiod *= arpeggioModulation;
		}

		fslide += fdslide;
		fperiod *= fslide;

		if (fperiod > fmaxperiod)
		{
			fperiod = fmaxperiod;

			if (params.minFrequencyValue > 0.0f) generatingSample = false;
		}

		float rfperiod = fperiod;

		if (vibratoAmplitude > 0.0f)
		{
			vibratoPhase += vibratoSpeed;
			rfperiod = fperiod*(1.0 + sinf(vibratoPhase)*vibratoAmplitude);
		}

		period = (int)rfperiod;

		if (period < 8) period=8;

		squareDuty += squareSlide;

		if (squareDuty < 0.0f) squareDuty = 0.0f;
		if (squareDuty > 0.5f) squareDuty = 0.5f;	

		// Volume envelope
		envelopeTime++;

		if (envelopeTime>envelopeLength[envelopeStage])
		{
			envelopeTime = 0;
			envelopeStage++;

			if (envelopeStage == 3) generatingSample = false;
		}

		if (envelopeStage == 0) envelopeVolume = (float)envelopeTime/envelopeLength[0];
		if (envelopeStage == 1) envelopeVolume = 1.0f + pow(1.0f - (float)envelopeTime/envelopeLength[1], 1.0f)*2.0f*params.sustainPunchValue;
		if (envelopeStage == 2) envelopeVolume = 1.0f - (float)envelopeTime/envelopeLength[2];

		// Phaser step
		fphase += fdphase;
		iphase = abs((int)fphase);

		if (iphase > 1023) iphase = 1023;

		if (flthpd != 0.0f)
		{
			flthp *= flthpd;
			if (flthp < 0.00001f) flthp = 0.00001f;
			if (flthp > 0.1f) flthp = 0.1f;
		}

		float ssample = 0.0f;

        #define MAX_SUPERSAMPLING   8
        
        // Supersampling x8
		for (int si = 0; si < MAX_SUPERSAMPLING; si++)
		{
			float sample = 0.0f;
			phase++;

			if (phase >= period)
			{
                //phase = 0;
				phase %= period;
                
				if (params.waveTypeValue == 3)
                {
					for (int i = 0;i < 32; i++) noiseBuffer[i] = GetRandomFloat(2.0f) - 1.0f;
                }
			}

			// base waveform
			float fp = (float)phase/period;

			switch (params.waveTypeValue)
			{
                case 0: // Square wave
                {
                    if (fp < squareDuty) sample = 0.5f;
                    else sample = -0.5f;
                } break;
                case 1: sample = 1.0f - fp*2; break;    // Sawtooth wave
                case 2: sample = sinf(fp*2*PI); break;  // Sine wave
                case 3: sample = noiseBuffer[phase*32/period]; break; // Noise wave
                default: break;
			}

			// LP filter
			float pp = fltp;
			fltw *= fltwd;

			if (fltw < 0.0f) fltw = 0.0f;
			if (fltw > 0.1f) fltw = 0.1f;
			if (params.lpfCutoffValue != 1.0f)
			{
				fltdp += (sample-fltp)*fltw;
				fltdp -= fltdp*fltdmp;
			}
			else
			{
				fltp = sample;
				fltdp = 0.0f;
			}

			fltp += fltdp;

			// HP filter
			fltphp += fltp - pp;
			fltphp -= fltphp*flthp;
			sample = fltphp;

			// Phaser
			phaserBuffer[ipp&1023] = sample;
			sample += phaserBuffer[(ipp - iphase + 1024) & 1023];
			ipp = (ipp + 1) & 1023;

			// Final accumulation and envelope application
			ssample += sample*envelopeVolume;
		}

        #define SAMPLE_SCALE_COEFICIENT 0.2f    // NOTE: Used to scale sample value to [-1..1]
        
		ssample = (ssample/MAX_SUPERSAMPLING)*SAMPLE_SCALE_COEFICIENT;
        //------------------------------------------------------------------------------------

        // Accumulate samples in the buffer
		if (ssample > 1.0f) ssample = 1.0f;
		if (ssample < -1.0f) ssample = -1.0f;

        buffer[i] = ssample;
	}
    
    Wave wave;
    wave.sampleCount = sampleCount;
    wave.sampleRate = WAVE_SAMPLE_RATE; // By default 44100 Hz
    wave.sampleSize = 32;               // By default 32 bit float samples
    wave.channels = 1;                  // By default 1 channel (mono)
    
    // NOTE: Wave can be converted to desired format after generation
    
    wave.data = malloc(wave.sampleCount*wave.sampleSize/8);
    memcpy(wave.data, buffer, wave.sampleCount*wave.sampleSize/8);
    
    free(buffer);
    
    return wave;
}

// Load .rfx (rFXGen) or .sfs (sfxr) sound parameters file
static WaveParams LoadSoundParams(const char *fileName)
{
    WaveParams params = { 0 };
    
    if (strcmp(GetExtension(fileName),"sfs") == 0)
    {
        FILE *file = fopen(fileName, "rb");

        // Load .sfs sound parameters 
        int version = 0;
        fread(&version, 1, sizeof(int), file);

        if ((version == 100) || (version == 101) || (version == 102))
        {
            fread(&params.waveTypeValue, 1, sizeof(int), file);

            volumeValue = 0.5f;

            if (version == 102) fread(&volumeValue, 1, sizeof(float), file);

            fread(&params.startFrequencyValue, 1, sizeof(float), file);
            fread(&params.minFrequencyValue, 1, sizeof(float), file);
            fread(&params.slideValue, 1, sizeof(float), file);

            if (version >= 101) fread(&params.deltaSlideValue, 1, sizeof(float), file);

            fread(&params.squareDutyValue, 1, sizeof(float), file);
            fread(&params.dutySweepValue, 1, sizeof(float), file);

            fread(&params.vibratoDepthValue, 1, sizeof(float), file);
            fread(&params.vibratoSpeedValue, 1, sizeof(float), file);
            
            float vibratoPhaseDelay = 0.0f;
            fread(&vibratoPhaseDelay, 1, sizeof(float), file); // Not used

            fread(&params.attackTimeValue, 1, sizeof(float), file);
            fread(&params.sustainTimeValue, 1, sizeof(float), file);
            fread(&params.decayTimeValue, 1, sizeof(float), file);
            fread(&params.sustainPunchValue, 1, sizeof(float), file);

            bool filterOn = false;
            fread(&filterOn, 1, sizeof(bool), file); // Not used
            
            fread(&params.lpfResonanceValue, 1, sizeof(float), file);
            fread(&params.lpfCutoffValue, 1, sizeof(float), file);
            fread(&params.lpfCutoffSweepValue, 1, sizeof(float), file);
            fread(&params.hpfCutoffValue, 1, sizeof(float), file);
            fread(&params.hpfCutoffSweepValue, 1, sizeof(float), file);
            
            fread(&params.phaserOffsetValue, 1, sizeof(float), file);
            fread(&params.phaserSweepValue, 1, sizeof(float), file);
            fread(&params.repeatSpeedValue, 1, sizeof(float), file);

            if (version >= 101)
            {
                fread(&params.changeSpeedValue, 1, sizeof(float), file);
                fread(&params.changeAmountValue, 1, sizeof(float), file);
            }
        }
        else printf("[%s] SFS file version not supported\n", fileName);
        
        fclose(file);
    }
    else if (strcmp(GetExtension(fileName),"rfx") == 0)
    {
        FILE *rfxFile = fopen(fileName, "rb");

        // Load .rfx sound parameters
        unsigned char signature[4];
        fread(signature, 4, sizeof(unsigned char), rfxFile);
        
        if ((signature[0] == 'r') &&
            (signature[1] == 'F') &&
            (signature[2] == 'X') &&
            (signature[3] == ' '))
        {
            int version;
            fread(&version, 1, sizeof(int), rfxFile);

            fread(&wavSampleRate, 1, sizeof(int), rfxFile);
            fread(&wavSampleSize, 1, sizeof(int), rfxFile);
 
            int channels;
            fread(&channels, 1, sizeof(int), rfxFile);
            
            // Read wave parameters struct
            fread(&params, 1, sizeof(WaveParams), rfxFile);
        }
        else printf("[%s] Not a valid rFX file\n", fileName);
        
        fclose(rfxFile);
    }

    return params;
}

// Save .rfx (rFXGen) or .sfs (sfxr) sound parameters file
static void SaveSoundParams(const char *fileName, WaveParams params)
{
    if (strcmp(GetExtension(fileName),"sfs") == 0)
    {
        FILE *sfsFile = fopen(fileName, "wb");

        // Save .sfs sound parameters 
        int version = 102;
        fwrite(&version, 1, sizeof(int), sfsFile);

        fwrite(&params.waveTypeValue, 1, sizeof(int), sfsFile);

        fwrite(&volumeValue, 1, sizeof(float), sfsFile);

        fwrite(&params.startFrequencyValue, 1, sizeof(float), sfsFile);
        fwrite(&params.minFrequencyValue, 1, sizeof(float), sfsFile);
        fwrite(&params.slideValue, 1, sizeof(float), sfsFile);
        fwrite(&params.deltaSlideValue, 1, sizeof(float), sfsFile);
        fwrite(&params.squareDutyValue, 1, sizeof(float), sfsFile);
        fwrite(&params.dutySweepValue, 1, sizeof(float), sfsFile);

        fwrite(&params.vibratoDepthValue, 1, sizeof(float), sfsFile);
        fwrite(&params.vibratoSpeedValue, 1, sizeof(float), sfsFile);
        
        float vibratoPhaseDelay = 0.0f;
        fwrite(&vibratoPhaseDelay, 1, sizeof(float), sfsFile); // Not used

        fwrite(&params.attackTimeValue, 1, sizeof(float), sfsFile);
        fwrite(&params.sustainTimeValue, 1, sizeof(float), sfsFile);
        fwrite(&params.decayTimeValue, 1, sizeof(float), sfsFile);
        fwrite(&params.sustainPunchValue, 1, sizeof(float), sfsFile);

        bool filterOn = false;
        fwrite(&filterOn, 1, sizeof(bool), sfsFile); // Not used
        
        fwrite(&params.lpfResonanceValue, 1, sizeof(float), sfsFile);
        fwrite(&params.lpfCutoffValue, 1, sizeof(float), sfsFile);
        fwrite(&params.lpfCutoffSweepValue, 1, sizeof(float), sfsFile);
        fwrite(&params.hpfCutoffValue, 1, sizeof(float), sfsFile);
        fwrite(&params.hpfCutoffSweepValue, 1, sizeof(float), sfsFile);
        
        fwrite(&params.phaserOffsetValue, 1, sizeof(float), sfsFile);
        fwrite(&params.phaserSweepValue, 1, sizeof(float), sfsFile);

        fwrite(&params.repeatSpeedValue, 1, sizeof(float), sfsFile);

        fwrite(&params.changeSpeedValue, 1, sizeof(float), sfsFile);
        fwrite(&params.changeAmountValue, 1, sizeof(float), sfsFile);
        
        fclose(sfsFile);
    }
    else if (strcmp(GetExtension(fileName),"rfx") == 0)
    {
        FILE *rfxFile = fopen(fileName, "wb");

        // Save .rfx sound parameters
        unsigned char signature[4] = "rFX ";
        fwrite(signature, 4, sizeof(unsigned char), rfxFile);
        
        int version = 100;      // File version
        fwrite(&version, 1, sizeof(int), rfxFile);

        // NOTE: Sound config shouldn't depend on Wave parameters
        // Wave params are used to generate a 44100Hz - 32bit float data
        
        // TODO: review this info
        fwrite(&wavSampleRate, 1, sizeof(int), rfxFile);
        fwrite(&wavSampleSize, 1, sizeof(int), rfxFile);
        
        int channels = 1;
        fwrite(&channels, 1, sizeof(int), rfxFile);
        
        fwrite(&params, 1, sizeof(WaveParams), rfxFile);
        
        fclose(rfxFile);
    }
}

// Draw wave data
// NOTE: For proper visualization, MSAA x4 is recommended, alternatively
// it should be rendered to a bigger texture and then scaled down with 
// bilinear/trilinear texture filtering
static void DrawWave(Wave *wave, Rectangle bounds, Color color)
{
    float sample, sampleNext;
    float currentSample = 0.0f;
    float sampleIncrement = (float)wave->sampleCount/(float)(bounds.width*2);
    float sampleScale = (float)bounds.height;
    
    for (int i = 1; i < bounds.width*2 - 1; i++)
    {
        sample = ((float *)wave->data)[(int)currentSample]*sampleScale;
        sampleNext = ((float *)wave->data)[(int)(currentSample + sampleIncrement)]*sampleScale;
        
        if (sample > bounds.height/2) sample = bounds.height/2;
        else if (sample < -bounds.height/2) sample = -bounds.height/2;
        
        if (sampleNext > bounds.height/2) sampleNext = bounds.height/2;
        else if (sampleNext < -bounds.height/2) sampleNext = -bounds.height/2;
        
        DrawLineV((Vector2){ (float)bounds.x + (float)i/2.0f, (float)(bounds.y + bounds.height/2) + sample }, 
                  (Vector2){ (float)bounds.x + (float)i/2.0f, (float)(bounds.y  + bounds.height/2) + sampleNext }, color);
        
        currentSample += sampleIncrement;
    }
}

// Save wave data to WAV file
static void SaveWAV(const char *fileName, Wave wave)
{
    // Basic WAV headers structs
    typedef struct {
        char chunkID[4];
        int chunkSize;
        char format[4];
    } RiffHeader;

    typedef struct {
        char subChunkID[4];
        int subChunkSize;
        short audioFormat;
        short numChannels;
        int sampleRate;
        int byteRate;
        short blockAlign;
        short bitsPerSample;
    } WaveFormat;

    typedef struct {
        char subChunkID[4];
        int subChunkSize;
    } WaveData;

    RiffHeader riffHeader;
    WaveFormat waveFormat;
    WaveData waveData;

    // Fill structs with data
    riffHeader.chunkID[0] = 'R';
    riffHeader.chunkID[1] = 'I';
    riffHeader.chunkID[2] = 'F';
    riffHeader.chunkID[3] = 'F';
    riffHeader.chunkSize = 44 - 4 + wave.sampleCount*wave.sampleSize/8;
    riffHeader.format[0] = 'W';
    riffHeader.format[1] = 'A';
    riffHeader.format[2] = 'V';
    riffHeader.format[3] = 'E';

    waveFormat.subChunkID[0] = 'f';
    waveFormat.subChunkID[1] = 'm';
    waveFormat.subChunkID[2] = 't';
    waveFormat.subChunkID[3] = ' ';
    waveFormat.subChunkSize = 16;
    waveFormat.audioFormat = 1;
    waveFormat.numChannels = wave.channels;
    waveFormat.sampleRate = wave.sampleRate;
    waveFormat.byteRate = wave.sampleRate*wave.sampleSize/8;   
    waveFormat.blockAlign = wave.sampleSize/8;
    waveFormat.bitsPerSample = wave.sampleSize;
    
    waveData.subChunkID[0] = 'd';
    waveData.subChunkID[1] = 'a';
    waveData.subChunkID[2] = 't';
    waveData.subChunkID[3] = 'a';
    waveData.subChunkSize = wave.sampleCount*wave.channels*wave.sampleSize/8;
    
    FILE *wavFile = fopen(fileName, "wb");
    
    fwrite(&riffHeader, 1, sizeof(RiffHeader), wavFile);
    fwrite(&waveFormat, 1, sizeof(WaveFormat), wavFile);
    fwrite(&waveData, 1, sizeof(WaveData), wavFile);

    fwrite(wave.data, 1, wave.sampleCount*wave.channels*wave.sampleSize/8, wavFile);

    fclose(wavFile);
}

//--------------------------------------------------------------------------------------------
// Buttons functions: sound generation
//--------------------------------------------------------------------------------------------

// Generate sound: Pickup/Coin
static void BtnPickupCoin(void)
{
    ResetParams(&params);

    params.startFrequencyValue = 0.4f + frnd(0.5f);
    params.attackTimeValue = 0.0f;
    params.sustainTimeValue = frnd(0.1f);
    params.decayTimeValue = 0.1f + frnd(0.4f);
    params.sustainPunchValue = 0.3f + frnd(0.3f);

    if (rnd(1))
    {
        params.changeSpeedValue = 0.5f + frnd(0.2f);
        params.changeAmountValue = 0.2f + frnd(0.4f);
    }

    regenerate = true;
}

// Generate sound: Laser shoot 
static void BtnLaserShoot(void)
{
    ResetParams(&params);

    params.waveTypeValue = rnd(2);

    if ((params.waveTypeValue == 2) && rnd(1)) params.waveTypeValue = rnd(1);

    params.startFrequencyValue = 0.5f + frnd(0.5f);
    params.minFrequencyValue = params.startFrequencyValue - 0.2f - frnd(0.6f);

    if (params.minFrequencyValue < 0.2f) params.minFrequencyValue = 0.2f;

    params.slideValue = -0.15f - frnd(0.2f);

    if (rnd(2) == 0)
    {
        params.startFrequencyValue = 0.3f + frnd(0.6f);
        params.minFrequencyValue = frnd(0.1f);
        params.slideValue = -0.35f - frnd(0.3f);
    }

    if (rnd(1))
    {
        params.squareDutyValue = frnd(0.5f);
        params.dutySweepValue = frnd(0.2f);
    }
    else
    {
        params.squareDutyValue = 0.4f + frnd(0.5f);
        params.dutySweepValue = -frnd(0.7f);
    }

    params.attackTimeValue = 0.0f;
    params.sustainTimeValue = 0.1f + frnd(0.2f);
    params.decayTimeValue = frnd(0.4f);

    if (rnd(1)) params.sustainPunchValue = frnd(0.3f);

    if (rnd(2) == 0)
    {
        params.phaserOffsetValue = frnd(0.2f);
        params.phaserSweepValue = -frnd(0.2f);
    }

    if (rnd(1)) params.hpfCutoffValue = frnd(0.3f);

    regenerate = true;
}

// Generate sound: Explosion
static void BtnExplosion(void)
{
    ResetParams(&params);

    params.waveTypeValue = 3;

    if (rnd(1))
    {
        params.startFrequencyValue = 0.1f + frnd(0.4f);
        params.slideValue = -0.1f + frnd(0.4f);
    }
    else
    {
        params.startFrequencyValue = 0.2f + frnd(0.7f);
        params.slideValue = -0.2f - frnd(0.2f);
    }

    params.startFrequencyValue *= params.startFrequencyValue;

    if (rnd(4) == 0) params.slideValue = 0.0f;
    if (rnd(2) == 0) params.repeatSpeedValue = 0.3f + frnd(0.5f);

    params.attackTimeValue = 0.0f;
    params.sustainTimeValue = 0.1f + frnd(0.3f);
    params.decayTimeValue = frnd(0.5f);

    if (rnd(1) == 0)
    {
        params.phaserOffsetValue = -0.3f + frnd(0.9f);
        params.phaserSweepValue = -frnd(0.3f);
    }

    params.sustainPunchValue = 0.2f + frnd(0.6f);

    if (rnd(1))
    {
        params.vibratoDepthValue = frnd(0.7f);
        params.vibratoSpeedValue = frnd(0.6f);
    }

    if (rnd(2) == 0)
    {
        params.changeSpeedValue = 0.6f + frnd(0.3f);
        params.changeAmountValue = 0.8f - frnd(1.6f);
    }

    regenerate = true;
}

// Generate sound: Powerup
static void BtnPowerup(void)
{
    ResetParams(&params);

    if (rnd(1)) params.waveTypeValue = 1;
    else params.squareDutyValue = frnd(0.6f);

    if (rnd(1))
    {
        params.startFrequencyValue = 0.2f + frnd(0.3f);
        params.slideValue = 0.1f + frnd(0.4f);
        params.repeatSpeedValue = 0.4f + frnd(0.4f);
    }
    else
    {
        params.startFrequencyValue = 0.2f + frnd(0.3f);
        params.slideValue = 0.05f + frnd(0.2f);
        
        if (rnd(1))
        {
            params.vibratoDepthValue = frnd(0.7f);
            params.vibratoSpeedValue = frnd(0.6f);
        }
    }

    params.attackTimeValue = 0.0f;
    params.sustainTimeValue = frnd(0.4f);
    params.decayTimeValue = 0.1f + frnd(0.4f);

    regenerate = true;
}

// Generate sound: Hit/Hurt
static void BtnHitHurt(void)
{
    ResetParams(&params);

    params.waveTypeValue = rnd(2);
    if (params.waveTypeValue == 2) params.waveTypeValue = 3;
    if (params.waveTypeValue == 0) params.squareDutyValue = frnd(0.6f);

    params.startFrequencyValue = 0.2f + frnd(0.6f);
    params.slideValue = -0.3f - frnd(0.4f);
    params.attackTimeValue = 0.0f;
    params.sustainTimeValue = frnd(0.1f);
    params.decayTimeValue = 0.1f + frnd(0.2f);

    if (rnd(1)) params.hpfCutoffValue = frnd(0.3f);

    regenerate = true;
}

// Generate sound: Jump
static void BtnJump(void)
{
    ResetParams(&params);

    params.waveTypeValue = 0;
    params.squareDutyValue = frnd(0.6f);
    params.startFrequencyValue = 0.3f + frnd(0.3f);
    params.slideValue = 0.1f + frnd(0.2f);
    params.attackTimeValue = 0.0f;
    params.sustainTimeValue = 0.1f + frnd(0.3f);
    params.decayTimeValue = 0.1f + frnd(0.2f);

    if (rnd(1)) params.hpfCutoffValue = frnd(0.3f);
    if (rnd(1)) params.lpfCutoffValue = 1.0f - frnd(0.6f);

    regenerate = true;
}

// Generate sound: Blip/Select
static void BtnBlipSelect(void)
{
    ResetParams(&params);

    params.waveTypeValue = rnd(1);
    if (params.waveTypeValue == 0) params.squareDutyValue = frnd(0.6f);
    params.startFrequencyValue = 0.2f + frnd(0.4f);
    params.attackTimeValue = 0.0f;
    params.sustainTimeValue = 0.1f + frnd(0.1f);
    params.decayTimeValue = frnd(0.2f);
    params.hpfCutoffValue = 0.1f;

    regenerate = true;
}

// Generate random sound
static void BtnRandomize(void)
{
    params.startFrequencyValue = pow(frnd(2.0f) - 1.0f, 2.0f);

    if (rnd(1)) params.startFrequencyValue = pow(frnd(2.0f) - 1.0f, 3.0f)+0.5f;

    params.minFrequencyValue = 0.0f;
    params.slideValue = pow(frnd(2.0f) - 1.0f, 5.0f);

    if ((params.startFrequencyValue > 0.7f) && (params.slideValue > 0.2f)) params.slideValue = -params.slideValue;
    if ((params.startFrequencyValue < 0.2f) && (params.slideValue < -0.05f)) params.slideValue = -params.slideValue;

    params.deltaSlideValue = pow(frnd(2.0f) - 1.0f, 3.0f);
    params.squareDutyValue = frnd(2.0f) - 1.0f;
    params.dutySweepValue = pow(frnd(2.0f) - 1.0f, 3.0f);
    params.vibratoDepthValue = pow(frnd(2.0f) - 1.0f, 3.0f);
    params.vibratoSpeedValue = frnd(2.0f) - 1.0f;
    //params.vibratoPhaseDelay = frnd(2.0f) - 1.0f;
    params.attackTimeValue = pow(frnd(2.0f) - 1.0f, 3.0f);
    params.sustainTimeValue = pow(frnd(2.0f) - 1.0f, 2.0f);
    params.decayTimeValue = frnd(2.0f)-1.0f;
    params.sustainPunchValue = pow(frnd(0.8f), 2.0f);

    if (params.attackTimeValue + params.sustainTimeValue + params.decayTimeValue < 0.2f)
    {
        params.sustainTimeValue += 0.2f + frnd(0.3f);
        params.decayTimeValue += 0.2f + frnd(0.3f);
    }

    params.lpfResonanceValue = frnd(2.0f) - 1.0f;
    params.lpfCutoffValue = 1.0f - pow(frnd(1.0f), 3.0f);
    params.lpfCutoffSweepValue = pow(frnd(2.0f) - 1.0f, 3.0f);

    if (params.lpfCutoffValue < 0.1f && params.lpfCutoffSweepValue < -0.05f) params.lpfCutoffSweepValue = -params.lpfCutoffSweepValue;

    params.hpfCutoffValue = pow(frnd(1.0f), 5.0f);
    params.hpfCutoffSweepValue = pow(frnd(2.0f) - 1.0f, 5.0f);
    params.phaserOffsetValue = pow(frnd(2.0f) - 1.0f, 3.0f);
    params.phaserSweepValue = pow(frnd(2.0f) - 1.0f, 3.0f);
    params.repeatSpeedValue = frnd(2.0f) - 1.0f;
    params.changeSpeedValue = frnd(2.0f) - 1.0f;
    params.changeAmountValue = frnd(2.0f) - 1.0f;

    regenerate = true;
}

// Mutate current sound
static void BtnMutate(void)
{
    if (rnd(1)) params.startFrequencyValue += frnd(0.1f) - 0.05f;
    //if (rnd(1)) params.minFrequencyValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.slideValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.deltaSlideValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.squareDutyValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.dutySweepValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.vibratoDepthValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.vibratoSpeedValue += frnd(0.1f) - 0.05f;
    //if (rnd(1)) params.vibratoPhaseDelay += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.attackTimeValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.sustainTimeValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.decayTimeValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.sustainPunchValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.lpfResonanceValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.lpfCutoffValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.lpfCutoffSweepValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.hpfCutoffValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.hpfCutoffSweepValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.phaserOffsetValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.phaserSweepValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.repeatSpeedValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.changeSpeedValue += frnd(0.1f) - 0.05f;
    if (rnd(1)) params.changeAmountValue += frnd(0.1f) - 0.05f;

    regenerate = true;
}

//--------------------------------------------------------------------------------------------
// Buttons functions: sound playing and export functions
//--------------------------------------------------------------------------------------------

// Load sound parameters file
static void BtnLoadSound(void)
{   
    // Open file dialog
    const char *filters[] = { "*.rfx", "*.sfs" };
    const char *fileName = tinyfd_openFileDialog("Load sound parameters file", currentPath, 2, filters, "Sound Param Files (*.rfx, *.sfs)", 0);

    if (fileName != NULL)
    {
        params = LoadSoundParams(fileName);
        regenerate = true;
    }
}

// Save sound parameters file
static void BtnSaveSound(void)
{
    char currrentPathFile[256];

    // Add sample file name to currentPath
    strcpy(currrentPathFile, currentPath);
    strcat(currrentPathFile, "sound.rfx\0");

    // Save file dialog
    const char *filters[] = { "*.rfx", "*.sfs" };
    const char *fileName = tinyfd_saveFileDialog("Save sound parameters file", currrentPathFile, 2, filters, "Sound Param Files (*.rfx, *.sfs)");

    if (fileName != NULL) SaveSoundParams(fileName, params);
}

// Export current sound as .wav
static void BtnExportWav(Wave wave)
{
    char currrentPathFile[256];

    // Add sample file name to currentPath
    strcpy(currrentPathFile, currentPath);
    strcat(currrentPathFile, "sound.wav\0");

    // Save file dialog
    const char *filters[] = { "*.wav" };
    const char *fileName = tinyfd_saveFileDialog("Save wave file", currrentPathFile, 1, filters, "Wave File (*.wav)");

    Wave cwave = WaveCopy(wave);
    
    // Before exporting wave data, we format it as desired
    WaveFormat(&cwave, wavSampleRate, wavSampleSize, 1);

    SaveWAV(fileName, cwave);
    
    UnloadWave(cwave);
}

//--------------------------------------------------------------------------------------------
// Helper functions
//--------------------------------------------------------------------------------------------

// Get the extension for a filename
static const char *GetExtension(const char *fileName)
{
    const char *dot = strrchr(fileName, '.');
    if (!dot || dot == fileName) return "";
    return (dot + 1);
}
