## The World of Repose - A calm world of constant change.
Reference project for [The Amalgam Engine](https://github.com/Net5F/AmalgamEngine).

## Building
### Windows
#### Visual Studio (MSVC)
1. Run `Scripts/Windows/InstallDependencies.bat`, passing it the path you want to install the dependencies to.
1. Open CMakeLists.txt in Visual Studio (`Open` -> `CMake`).
1. Update CMakeSettings.json (in this repo) to point at the various SDL folders in your installation path.
   1. You'll leave this file dirty. Don't try to commit it back upstream with your personal paths.
   1. You can optionally enable `AM_BUILD_SPRITE_EDITOR` to build the sprite editor.
   1. You can optionally enable `BUILD_TOOLS` to build our extra tools, such as the Map Generator.
1. `Project` -> `Generate CMake cache` (or just let it run if you have auto-config on).
1. `Build` -> `Build All`

#### MinGW
For MSYS2/MinGW, we don't have a dependency install script. Here's the list:

    pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-gdb mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_gfx mingw-w64-x86_64-SDL2_net mingw-w64-x86_64-catch
    
Then, build through the Eclipse project or follow the linux instructions for a command line build.

### Linux
Had to update to Ubuntu 21.04 for g++ 10.3.0 (older versions didn't have support for some C++20 features I needed.)
I haven't yet locked down an SDL2 or CMake version, we'll see where it ends up.

1. Run `Scripts/Linux/InstallDependencies.sh`, then build through the Eclipse project, or:
1. (From the base of the repo) `mkdir -p Build/Linux/Release`
1. `cd Build/Linux/Release`
1. `cmake -DCMAKE_BUILD_TYPE=Release -G Ninja ../../../Source/`
   1. You can optionally add `-DAM_BUILD_SPRITE_EDITOR=ON` to build the sprite editor.
   1. You can optionally add `-DBUILD_TOOLS` to build our extra tools, such as the Map Generator.
1. `ninja all`

## Contributing
### Bugs
Bug reports and fixes are always welcome. Feel free to open an issue or submit a PR.

### Features
**Unsolicited feature PRs will not be reviewed. Please ask about the feature plan before working on a feature.**

Collaboration is very welcome! That being said, there is a fairly solid vision for this project, so please ask before beginning work.
