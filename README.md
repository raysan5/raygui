<img align="left" src="logo/raygui_256x256.png" width=256>

**raygui** is a simple and easy-to-use immediate-mode-gui single-file header-only library.

raygui was initially inspired by [Unity IMGUI](https://docs.unity3d.com/Manual/GUIScriptingGuide.html) (immediate mode GUI API).

raygui was originated as an auxiliar module for [raylib](https://github.com/raysan5/raylib) to create simple GUI interfaces using raylib graphic style (simple colors, plain rectangular shapes, wide borders, raylib default font...).

raygui is intended for **tools development**; it has already been used on tools like [rFXGen](https://github.com/raysan5/rfxgen), [rGuiStyler](https://github.com/raysan5/rguistyler) and [rGuiLayout](https://github.com/raysan5/rguilayout).

<br>

## raygui styles
raygui comes with two predefined styles, selectable at compile time just defining `RAYGUI_STYLE_DEFAULT_LIGHT` or `RAYGUI_STYLE_DEFAULT_DARK` but additional custom styles can be created using [rGuiStyler](https://github.com/raysan5/rguistyler) and loaded at runtime using `GuiLoadStyle()`, `GuiLoadStyleImage()` or `GuiLoadStylePalette()` functions.

![raygui light design](styles_design/raygui_style_table_light_REV10.png)

![raygui dark design](styles_design/raygui_style_table_dark_REV9.png)

![raygui candy design](styles_design/raygui_style_table_candy_REV1.png)

![raygui cherry design](styles_design/raygui_style_table_cherry_REV1.png)

## raygui history
raygui development started on December 2014 by two intenship students (Kevin and Daniel) guided by me, the objective was creating a simple and easy-to-use immediate-mode-gui module for raylib, intended for tools development. On June 2015, library was mostly functional (including all basic planned controls) and we started working in the styling options for the library, focusing on an easy way to style some controls properties. Consequently, development of [rGuiStyler](https://github.com/raysan5/rguistyler) also started at that point but resources were quite limited and project didn't move much during the following year, most of the time was invested in [raylib](https://github.com/raysan5/raylib) development.

On June 2016, project was picked up again and raygui 1.0 was released at the end of that month. During August 2016, raygui was used to develop [rFXGen](https://github.com/raysan5/rfxgen) and also an early protoype of [rGuiLayout](https://github.com/raysan5/rguilayout), mainly as an usage prove test. After that, raygui project was stopped again for the coming months.

On June 2017, a complete redesign of the library started, almost from scratch, all functions were reviewed and mostly rewritten and a brand new styling system was developed. The objective was using raygui professionally in several tools.

On January 2018, two students (Adria and Jordi) started working on raygui and related tools. Consequently, library and tools evolved A LOT in the following months. [rGuiStyler](https://github.com/raysan5/rguistyler) was [completely redesigned](https://github.com/raysan5/rguistyler/design) and rewritten from scratch. [rGuiLayout](https://github.com/raysan5/rguilayout) turned from a protoype into a professional software and raygui reached version 2.0 with plenty of new controls and features.

## raygui 2.0 provided controls

#### Container/separator controls, useful for controls organization

<img align="right" src="images/raygui_messagebox.png">

 - WindowBox
 - GroupBox
 - Line
 - Panel

#### Basic controls

<img align="right" src="images/rguistyler_v210_light_shot01.png">

 - Label
 - Button
 - LabelButton
 - ImageButton
 - ToggleButton
 - ToggleGroup
 - CheckBox
 - ComboBox
 - DropdownBox
 - Spinner
 - ValueBox
 - TextBox
 - Slider
 - SliderBar
 - ProgressBar
 - StatusBar
 - ScrollBar (WIP)
 - DummyRec

#### Advance controls
 - ListView
 - ColorPicker
 - GuiMessageBox
 - Grid

## Acknowledgements
The following people has invested their time and effort into raygui project. Big thanks to them!

 - [Adria Arranz](https://github.com/Adri102) and [Jordi Jorba](https://github.com/KoroBli) for improving raygui, adding new controls and working hard on [rGuiStyler](https://github.com/raysan5/rguistyler) and [rGuiLayout](https://github.com/raysan5/rguilayout) tools redesign and rewrite.
 - [Sergio Martinez](https://github.com/anidealgift) for improving raygui and working on original [rGuiStyler](https://github.com/raysan5/rguistyler) tool.
 - Daniel Nicolas and [Kevin Gato](https://github.com/Gatonevermind) for working on initial development.
