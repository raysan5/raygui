# raygui styles

`raygui` comes with **8 custom styles** carefully designed for the best visual experience. Those styles have been created using [rGuiStyler](https://raylibtech.itch.io/rguistyler) tool and they complement internal [default style](default), always available by `raygui`.

## styles usage 

To use those styles with your `raygui` development, you need to call `GuiLoadStyle()` function at initialization, passing the `.rgs` file to load. Note that `.rgs` is by default a binary file containing the style required font data (glyphs data + glyph atlas image data).

Styles can also be embedded in the code if desired, `.h` files are provided with every style containing all the required style data, including the font data. To embed those fonts just add the `.h` to your project and call the required function as specified in the header info.

Here it is a quick overview of those styles, you can navigate to each directory for additional information.

#### style: [default](default)
![default style](default/style_table.png)

#### style: [ashes](ashes)
![ashes style](ashes/style_table.png)

#### style: [bluish](bluish)
![bluish style](bluish/style_table.png)

#### style: [candy](candy)
![candy style](candy/style_table.png)

#### style: [cherry](cherry)
![cherry style](cherry/style_table.png)

#### style: [cyber](cyber)
![cyber style](cyber/style_table.png)

#### style: [jungle](jungle)
![jungle style](jungle/style_table.png)

#### style: [lavanda](lavanda)
![lavanda style](lavanda/style_table.png)

#### style: [terminal](terminal)
![terminal style](terminal/style_table.png)

#### style: [sunny](sunny)
![sunny style](sunny/style_table.png)


*NOTE: Those styles require raylib 4.0 and raygui 3.1.*
