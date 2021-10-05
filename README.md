<img align="left" src="logo/raygui_256x256.png" width=256>

**raygui is a simple and easy-to-use immediate-mode-gui library.**

`raygui` was initially inspired by [Unity IMGUI](https://docs.unity3d.com/Manual/GUIScriptingGuide.html) (immediate mode GUI API).

`raygui` was originated as an auxiliar module for [raylib](https://github.com/raysan5/raylib) to create simple GUI interfaces using raylib graphic style (simple colors, plain rectangular shapes, wide borders...).

`raygui` is intended for **tools development**; it has already been used to develop [multiple publised tools](https://raylibtech.itch.io/rguiicons).

<br>
<br>

## features

 - Immediate-mode gui, no retained data
 - +28 basic and advance controls provided
 - Powerful styling system for colors, font and metrics
 - Standalone usage mode supported (for other graphic libs)
 - Icons support, embedding a complete 1-bit icons pack
 - Multiple tools provided for raygui development

## raygui controls

### basic controls
```
Label       |  Button      |  LabelButton |  ImageButton  |  Toggle      |  ToggleGroup |  CheckBox
ComboBox    |  DropdownBox |  TextBox     |  TextBoxMulti |  ValueBox    |  Spinner     |  Slider
SliderBar   |  ProgressBar |  StatusBar   |  ScrollBar    |  ScrollPanel |  DummyRec    |  Grid
```
### advance controls
```
ListView    |  ColorPicker |  MessageBox  |  TextInputBox
```
### container/separator controls
```
WindowBox   |  GroupBox    |  Line        |  Panel
```

## raygui styles

`raygui` comes with a [default](styles/default) style automatically loaded at runtime:

![raygui default style](styles/default/style_table.png)

Some additional styles are also provided for convenience, just check [styles directory](styles) for details:

![raygui additional styles](images/raygui_style_table_multi.png)

Custom styles can also be created very easily using [rGuiStyler](https://raylibtech.itch.io/rguistyler) tool.

Styles can be loaded at runtime using raygui `GuiLoadStyle()` function. Simple and easy-to-use.

## raygui icons

`raygui` supports custom icons provided as an external array of data. To support icons just define `RAYGUI_SUPPORT_RICONS` before including `raygui`.

A set of custom handcrafted icons is provided in [`ricons`](src/ricons.h). This set of icons can be created and customized using [rGuiIcons](https://raylibtech.itch.io/rguiicons) tool.

<img align="right" src="images/raygui_ricons.png">

```c
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_RICONS
#include "raygui.h"
```
To use any of those icons in your gui, just preprend *iconId* to any text written within `raygui` controls:
```c
if (GuiButton(rec, "#05#Open Image")) { /* ACTION */ }
```
or use the provided `GuiIconText()` function to prepend it automatically, using a clearer identifier.
```c
if (GuiButton(rec, GuiIconText(RICON_FILE_OPEN, "Open Image"))) { /* ACTION */ }
```

## raygui support tools

### [rGuiStyler](https://raylibtech.itch.io/rguistyler)

A simple and easy-to-use raygui styles editor.

![rGuiStyler v3.1](images/rguistyler_v300.png)

### [rGuiIcons](https://raylibtech.itch.io/rguiicons)

A simple and easy-to-use raygui icons editor.

![rGuiIcons v1.0](images/rguiicons_v100.png)

### [rGuiLayout](https://raylibtech.itch.io/rguilayout)

A simple and easy-to-use raygui layouts editor.

![rGuiLayout v2.2](images/rguilayout_v220.png)

## building

`raygui` is intended to be used as a portable single-file header-only library, to be directly integrated into any C/C++ codebas but some users could require a shared/dynamic version of the library, for example, to create bindings. In that case, `raygui` can be built as a (Linux) shared library using:
```
mv src/raygui.h src/raygui.c && gcc -shared -fpic -DRAYGUI_SUPPORT_RICONS -DRAYGUI_IMPLEMENTATION -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 src/raygui.c -o raygui.so
```

## license

raygui is licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.
