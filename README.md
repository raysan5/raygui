# raygui
raygui is simple and easy-to-use IMGUI header-only library.

raygui was inspired by [Unity IMGUI](https://docs.unity3d.com/Manual/GUIScriptingGuide.html) (immediate mode GUI API).

Initially raygui was proposed as an extra module for [raylib](https://github.com/raysan5/raylib) to create simple GUIs using a raylib graphic style (simple colors, plain rectangular shapes, wide borders, raylib standard font...). raygui was developed to be used on some raylib-based tools (without external dependencies, only raylib).

## history
raygui development started on December 2014 by two intenship students (Kevin Gato and Daniel Nicolas), guided by me (@raysan5) to create a simple raylib IMGUI module. On June 2015, library was mostly complete (including all planned controls) and work started into the styling possibilities for the library. Development on [rGuiStyler](https://github.com/raysan5/raygui/tree/master/tools/rGuiStyler) (previously named `raygui_styler`) also started at that point but resources invested on the tool were quite limited and project didn't move much during the following year, most of the time was invested in [raylib](https://github.com/raysan5/raylib).

Finally, by the end of June 2016, project was picked up again and raygui 1.0 was released. During August 2016, raygui was used to develop [rFXGen](https://github.com/raysan5/raygui/tree/master/tools/rFXGen) and [rGuiLayout](https://github.com/raysan5/raygui/tree/master/tools/rGuiLayout) tools. Currently, [rGuiStyler](https://github.com/raysan5/raygui/tree/master/tools/rGuiStyler) is also being updated an improved.

## notes on raygui 1.0
Officially, raygui 1.0 was published on June 2016, about one year and a half after its developmentment started.

## controls
Currently raygui provides the following controls:
 - Label
 - Button
 - ToggleButton
 - ToggleGroup
 - ComboBox
 - CheckBox
 - Slider
 - SliderBar
 - ProgressBar
 - Spinner
 - TextBox

## acknowledgements
The following people has invested their time and effort into raygui project. Big thanks to them!

 - [Kevin Gato](https://github.com/Gatonevermind) for working on initial development.
 - Daniel Nicolas  for working on initial development.
 - [Sergio Martinez](https://github.com/anidealgift) for improving raygui and working on [rGuiStyler](https://github.com/raysan5/raygui/tree/master/tools/rGuiStyler) tool.
