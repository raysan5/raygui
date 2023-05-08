<img align="left" src="logo/raygui_256x256.png" width=256>

**raygui is a simple and easy-to-use immediate-mode-gui library.**

`raygui` was originally inspired by [Unity IMGUI](https://docs.unity3d.com/Manual/GUIScriptingGuide.html) (immediate mode GUI API).

`raygui` was designed as an auxiliar module for [raylib](https://github.com/raysan5/raylib) to create simple GUI interfaces using raylib graphic style (simple colors, plain rectangular shapes, wide borders...) but it can be adapted to other engines/frameworks.

`raygui` is intended for **tools development**; it has already been used to develop [multiple published tools](https://raylibtech.itch.io).

<br>

**WARNING: Latest `raygui` from master branch is always aligned with latest `raylib` from master branch.**

**Make sure to use the appropiate versions.**

## features

 - **Immediate-mode gui, no retained data**
 - **+25** controls provided (basic and advanced)
 - Powerful **styling system** for colors, font and metrics
 - Standalone usage mode supported (for other graphic libs)
 - **Icons support**, embedding a complete 1-bit icons pack
 - Multiple **tools** provided for raygui development

## raygui controls

### basic controls
```
Label       |  Button      |  LabelButton |  Toggle       |  ToggleGroup |  CheckBox
ComboBox    |  DropdownBox |  TextBox     |  ValueBox     |  Spinner
Slider      |  SliderBar   |  ProgressBar |  StatusBar    |  DummyRec    |  Grid
```
### container/separator controls
```
WindowBox   |  GroupBox    |  Line        |  Panel        |  ScrollPanel  | TabBar
```
### advanced controls
```
ListView    |  ColorPicker |  MessageBox  |  TextInputBox
```


## raygui styles

`raygui` comes with a [default](styles/default) style automatically loaded at runtime:

![raygui default style](styles/default/style_table.png)

Some additional styles are also provided for convenience, just check [styles directory](styles) for details:

![raygui additional styles](images/raygui_style_table_multi.png)

Custom styles can also be created very easily using [rGuiStyler](https://raylibtech.itch.io/rguistyler) tool.

Styles can be loaded at runtime using raygui `GuiLoadStyle()` function. Simple and easy-to-use.

## raygui icons

`raygui` supports custom icons, by default, a predefined set of icons is provided inside `raygui` as an array of binary data; it contains **256 possible icons** defined as **16x16 pixels** each; each pixel is codified using **1-bit**. The total size of the array is `2048 bytes`.

<img align="right" src="images/raygui_ricons.png">

To use any of those icons just prefix the *#iconId#* number to **any text** written within `raygui` controls:
```c
if (GuiButton(rec, "#05#Open Image")) { /* ACTION */ }
```
It's also possible to use the provided `GuiIconText()` function to prefix it automatically, using a clearer identifier (defined in `raygui.h`).
```c
if (GuiButton(rec, GuiIconText(RICON_FILE_OPEN, "Open Image"))) { /* ACTION */ }
```
Provided set of icons can be reviewed and customized using [rGuiIcons](https://raylibtech.itch.io/rguiicons) tool.

## raygui support tools

 - [**rGuiStyler**](https://raylibtech.itch.io/rguistyler) - A simple and easy-to-use raygui styles editor.

   ![rGuiStyler v3.1](images/rguistyler_v300.png)

 - [**rGuiIcons**](https://raylibtech.itch.io/rguiicons) - A simple and easy-to-use raygui icons editor.

   ![rGuiIcons v1.0](images/rguiicons_v100.png)

 - [**rGuiLayout**](https://raylibtech.itch.io/rguilayout) - A simple and easy-to-use raygui layouts editor.

   ![rGuiLayout v2.2](images/rguilayout_v220.png)

## building

`raygui` is intended to be used as a portable single-file header-only library, to be directly integrated into any C/C++ codebase but some users could require a shared/dynamic version of the library, for example, to create bindings:

 - **Windows (MinGW, GCC)**
```
copy src/raygui.h src/raygui.c
gcc -o src/raygui.dll src/raygui.c -shared -DRAYGUI_IMPLEMENTATION -DBUILD_LIBTYPE_SHARED -static-libgcc -lopengl32 -lgdi32 -lwinmm -Wl,--out-implib,src/librayguidll.a
```

 - **Windows (MSVC)**
```
copy src\raylib.h src\raylib.c
cl /O2 /I../raylib/src/ /D_USRDLL /D_WINDLL /DRAYGUI_IMPLEMENTATION /DBUILD_LIBTYPE_SHARED src/raygui.c /LD /Feraygui.dll /link /LIBPATH ../raylib/build/raylib/Release/raylib.lib /subsystem:windows /machine:x64
```

 - **Linux (GCC)**
```
mv src/raygui.h src/raygui.c
gcc -o raygui.so src/raygui.c -shared -fpic -DRAYGUI_IMPLEMENTATION -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
```

- **Mac (clang, homebrew installed raylib)**
```
cp src/raygui.h src/raygui.c
brew install raylib
gcc -o raygui.dynlib src/raygui.c -shared -fpic -DRAYGUI_IMPLEMENTATION -framework OpenGL -lm -lpthread -ldl $(pkg-config --libs --cflags raylib)
```


## license

raygui is licensed under an unmodified zlib/libpng license, which is an OSI-certified, BSD-like license that allows static linking with closed source software. Check [LICENSE](LICENSE) for further details.
