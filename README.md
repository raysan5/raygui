# raygui
raygui is simple and easy-to-use IMGUI header-only library.

raygui was inspired by Unity OnGUI immediate mode GUI API.

Initially raygui was conceived as an additional raylib module to create simple GUIs using a raygui graphic style (simple colors, plain rectangular shapes, wide borders, raylib standard font).

## history
raygui was originally developed to create simple GUIs for some raylib-based tools (without external dependencies, only raylib).

raygui development started on December 2014 by two students (Kevin Gato and Daniel Nicolas), guided by me (@raysan5) to create a simple raylib GUI module. On June 2015, library was mostly complete (including all planned controls) and work started into styling possibilities for the library. Development on [rGuiStyler](https://github.com/raysan5/raygui/tree/master/tools/rGuiStyler) (project named `raygui_styler`) started at that point but resources invested on the tool were quite limited and project didn't move much between for the following year, most of the time was invested in [raylib](https://github.com/raysan5/raylib).

Finally, on the end of May 2016, project was picked up again and continued to release raylib 1.0 and use it on some tools development.
During August 2016, raygui was used to develop rFXGen and rGuiLayout tools and rGuiStyler was also updated an improved.

## notes on raygui 1.0
Officially, raygui 1.0 was published on June 2016, about one year and a half after initializing its development.

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

[Kevin Gato](@Gatonevermind) for working on initial development.
Daniel Nicolas  for working on initial development.
Sergio Martinez for improving raygui and working on [rGuiStyler](https://github.com/raysan5/raygui/tree/master/tools/rGuiStyler) tool.
