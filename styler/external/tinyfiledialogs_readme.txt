tiny file dialogs (cross-platform C C++)
InputBox PasswordBox MessageBox ColorPicker
OpenFileDialog SaveFileDialog SelectFolderDialog
Native dialog library for WINDOWS MAC OSX GTK+ QT CONSOLE & more
tested with C & C++ compilers
on Visual Studio 2013 MinGW OSX LINUX FREEBSD ILLUMOS SOLARIS
using Gnome Kde Enlightenment Mate Cinnamon Unity
Lxde Lxqt Xfce WindowMaker IceWm Cde Jds
v2.2 [July 9, 2015] zlib licence.

int tinyfd_messageBox (
    char const * const aTitle , // ""
    char const * const aMessage , // "" may contain \n and \t
    char const * const aDialogType , // "ok" "okcancel" "yesno"
    char const * const aIconType , // "info" "warning" "error" "question"
    int const aDefaultButton ) ; // 0 for cancel/no , 1 for ok/yes
        // returns 0 for cancel/no , 1 for ok/yes

char const * tinyfd_inputBox (
    char const * const aTitle , // ""
    char const * const aMessage , // "" may NOT contain \n nor \t on windows
    char const * const aDefaultInput ) ; // "" , if NULL it's a passwordBox
        // returns NULL on cancel

char const * tinyfd_saveFileDialog (
    char const * const aTitle , // ""
    char const * const aDefaultPathAndFile , // ""
    int const aNumOfFilterPatterns , // 0
    char const * const * const aFilterPatterns , // NULL or {"*.txt"}
    char const * const aSingleFilterDescription ) ; // NULL or "image files"
        // returns NULL on cancel

char const * tinyfd_openFileDialog (
    char const * const aTitle , // ""
    char const * const aDefaultPathAndFile , // ""
    int const aNumOfFilterPatterns , // 0
    char const * const * const aFilterPatterns , // NULL or {"*.jpg","*.png"}
    char const * const aSingleFilterDescription , // NULL or "image files"
    int const aAllowMultipleSelects ) ; // 0
        // in case of multiple files, the separator is |
        // returns NULL on cancel

char const * tinyfd_selectFolderDialog (
    char const * const aTitle , // ""
    char const * const aDefaultPath ) ; // ""
        // returns NULL on cancel

char const * tinyfd_colorChooser(
    char const * const aTitle , // ""
    char const * const aDefaultHexRGB , // NULL or "#FF0000”
    unsigned char const aDefaultRGB[3] , // { 0 , 255 , 255 }
    unsigned char aoResultRGB[3] ) ; // { 0 , 0 , 0 }
        // returns the hexcolor as a string "#FF0000"
        // aoResultRGB also contains the result
        // aDefaultRGB is used only if aDefaultHexRGB is NULL
        // aDefaultRGB and aoResultRGB can be the same array
        // returns NULL on cancel

- This is not for android nor ios.
- The code is pure C, perfectly compatible with C++.
- There's one file filter only, it may contain several patterns.
- If no filter description is provided,
  the list of patterns will become the description.
- char const * filterPatterns[3] = { "*.obj" , "*.stl" , "*.dxf" } ;
- On windows, inputbox and passwordbox are not as smooth as they should be:
  they open a console window for a few seconds.
- On windows link against Comdlg32.lib
  This linking is not compulsary for console mode (see header file).
- On unix: it tries command line calls, so no such need.
- On unix you need applescript, zenity, kdialog, Xdialog, python2/tkinter
  or dialog (will open a terminal if running without console);
- One of those is already included on most (if not all) desktops.
- In the absence of those it will use gdialog, gxmessage or whiptail
  with a textinputbox.
- If nothing is found, it switches to basic console input,
  it opens a console if needed.
- Avoid using " and ' in titles and messages.
- Use windows separators on windows and unix separators on unix.
- String memory is preallocated statically for all the returned values.
- File and path names are tested before return, they are valid.
- If you pass only a path instead of path + filename,
  make sure it ends with a separator.
- tinyfd_forceConsole=1; at run time, forces dialogs into console mode.
- On windows, console mode only make sense for console applications.
- Mutiple selects are not allowed in console mode.
- The package dialog must be installed to run in enhanced console mode.
  It is already installed on most unix systems.
- On osx, the package dialog can be installed via http://macports.org
- On windows, for enhanced console mode,
  dialog.exe should be copied somewhere on your executable path.
  It can be found at the bottom of the following page:
  http://andrear.altervista.org/home/cdialog.php
- If dialog is missing, it will switch to basic console input.

- Here is the Hello World:
    if a console is missing, it will use graphic dialogs
    if a graphical display is absent, it will use console dialogs

#include <stddef.h>
#include "tinyfiledialogs.h"
int main()
{
  tinyfd_forceConsole = tinyfd_messageBox("Hello World",
    "force dialogs into console mode?\
    \n\t(it's better if dialog is installed)",
    "yesno", "question", 0);

  char const * lThePassword =  tinyfd_inputBox(
    "a password box","your password will be revealed",NULL);

  if ( lThePassword )
    tinyfd_messageBox("your password is", lThePassword, "ok", "info", 1);
}

OSX :
$ gcc -o main.app main.c tinyfiledialogs.c

UNIX :
$ gcc -o main main.c tinyfiledialogs.c

MinGW :
> gcc -o main.exe main.c tinyfiledialogs.c -LC:/mingw/lib -lcomdlg32

VisualStudio :
  create a console application project
  (by default it links against Comdlg32.lib)
