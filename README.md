<img align="left" src="logo/raygui_256x256.png" width=256>

**raygui** is a simple and easy-to-use immediate-mode-gui library.

raygui was initially inspired by [Unity IMGUI](https://docs.unity3d.com/Manual/GUIScriptingGuide.html) (immediate mode GUI API).

raygui was originated as an auxiliar module for [raylib](https://github.com/raysan5/raylib) to create simple GUI interfaces using raylib graphic style (simple colors, plain rectangular shapes, wide borders...).

raygui is intended for **tools development**; it has already been used to develop the following tools: [rFXGen](https://github.com/raysan5/rfxgen), [rTexViewer](https://raylibtech.itch.io/rtexviewer), [rTexPacker](https://raylibtech.itch.io/rtexpacker) [rGuiStyler](https://raylibtech.itch.io/rguistyler), [rGuiLayout](https://raylibtech.itch.io/rguilayout) and [rGuiIcons](https://raylibtech.itch.io/rguiicons)

<br>

## raygui provided controls

#### Container/separator controls
```
WindowBox   |  GroupBox    |  Line        |  Panel
```
#### Basic controls
```
Label       |  Button      |  LabelButton |  ImageButton  |  Toggle      |  ToggleGroup |  CheckBox
ComboBox    |  DropdownBox |  TextBox     |  TextBoxMulti |  ValueBox    |  Spinner     |  Slider   
SliderBar   |  ProgressBar |  StatusBar   |  ScrollBar    |  ScrollPanel |  DummyRec    |  Grid
```
#### Advance controls
```
ListView    |  ColorPicker |  MessageBox  |  TextInputBox
```

## raygui styles

raygui comes with a [default](styles/default) style automatically loaded at runtime:

![raygui default style](styles/default/style_table.png)

Some additional styles are also provided for convenience, just check [styles directory](styles) for details:

![raygui additional styles](images/raygui_style_table_multi.png)

Custom styles can also be created very easily using [rGuiStyler](https://raylibtech.itch.io/rguistyler) tool. 

Styles can be loaded at runtime using raygui `GuiLoadStyle()` function. Simple and easy-to-use.

![rGuiStyler v3.1](images/rguistyler_v300.png)

*rGuiStyler v3.1 - raygui styles editor, useful to create custom styles*
 
## raygui icons

`raygui` supports custom icons provided as an external array of data. To support icons just define `RAYGUI_SUPPORT_ICONS` before including `raygui`. 

A set of custom handcrafted icons is provided in [`ricons`](src/ricons.h). This set of icons can be created and customized using [rGuiIcons](https://raylibtech.itch.io/rguiicons) tool.  

<img align="right" src="images/raygui_ricons.png">

```c
#define RAYGUI_IMPLEMENTATION
#define RAYGUI_SUPPORT_ICONS
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

license
-------

raygui is licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.
