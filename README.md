## The World of Repose - A calm world of constant change.
*Template project for [The Amalgam Engine](https://github.com/Net5F/AmalgamEngine). Visit [worlds.place](https://worlds.place/) to learn more.*

![ReposeMedium](https://user-images.githubusercontent.com/17211746/218555633-37cf85bc-1692-4c42-a33e-559187d43bac.png)

Repose is a cozy world, built around a large hedge maze. It serves three purposes:
1. Acts as a template project that anyone can base their own world off of.
1. Showcases that, with The Amalgam Engine, users and systems can easily make live changes to the world.
1. Serves as an example of an Amalgam Engine focus: making worlds that are dynamic and alive.

Modern virtual worlds should inspire curiosity and wonder in players. They should provide truly fulfilling social experiences. They should also be easy and free to create. These are core philosophies behind the development of The Amalgam Engine, and Repose is our first milestone towards realizing these goals.

## Joining the demo world
To join the demo world and play with other people, you can [download the latest release](https://github.com/Net5F/Repose/releases/latest) and follow the instructions in the README.

## Building
### Windows
#### Visual Studio (MSVC)
1. Open CMakeLists.txt in Visual Studio (`Open` -> `CMake`).
1. (Optional) Open CMakeSettings.json (in this repo) and enable flags for extra build targets:
   1. `AM_BUILD_RESOURCE_IMPORTER` to build the Resource Importer.
   1. `BUILD_TOOLS` to build our extra tools, such as the Map Generator.
1. `Project` -> `Generate CMake cache` (or just let it run if you have auto-config on).
1. `Build` -> `Build All`

#### MinGW
For MSYS2/MinGW, we don't have a dependency install script. Here's the list:

    pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-make mingw-w64-x86_64-cmake mingw-w64-x86_64-ninja mingw-w64-x86_64-gdb mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_image mingw-w64-x86_64-SDL2_mixer mingw-w64-x86_64-SDL2_ttf mingw-w64-x86_64-SDL2_gfx mingw-w64-x86_64-SDL2_net mingw-w64-x86_64-catch
    
Then, build through the Eclipse project or follow the Linux instructions for a command line build.

### Linux
Note: This is only tested on Ubuntu 20.04. If you have experience in multi-distro builds, please get involved!

1. Run `Scripts/Linux/InstallDependencies.sh`, then build through the Eclipse project, or:
1. (From the base of the repo) `mkdir -p Build/Linux/Release`
1. `cd Build/Linux/Release`
1. `cmake -DCMAKE_BUILD_TYPE=Release -G Ninja ../../../`
   1. (Optional) Add `-DAM_BUILD_RESOURCE_IMPORTER=ON` to build the Resource Importer.
   1. (Optional) Add `-DBUILD_TOOLS` to build our extra tools, such as the Map Generator.
1. `ninja all`

### macOS

Note: AmalgamEngine requires GCC to build on macOS. Clang / Apple Clang does not yet support C++ features required by this project (as of Apple Clang 14 / Clang 15).

1. Use the Homebrew package manager to install dependencies: `brew install gcc make cmake ninja sdl2 sdl2_image sdl2_mixer sdl2_ttf sdl2_gfx sdl2_net`
1. (From the base of the repo) `mkdir -p Build/macOS/Release`
1. `cd Build/macOS/Release`
1. `CC=gcc-NN CXX=g++-NN cmake -DCMAKE_BUILD_TYPE=Release -G Ninja ../../../`. Replace the `NN` in `gcc-NN` and `g++-NN` with the version of GCC that you installed with Homebrew. This is important, as using `gcc` without a version number will alias to `clang`.
   1. ~~(Optional) Add `-DAM_BUILD_RESOURCE_IMPORTER=ON` to build the Resource Importer.~~ Resource Importer doesn't currently build on macOS due to GCC not being able to build with Apple SDK headers which use certain Objective C extensions.
   1. (Optional) Add `-DBUILD_TOOLS` to build our extra tools, such as the Map Generator.
1. `ninja all`

## Packaging
To package the applications in a way that can be shared, first run the desired build. Then, run:
```
// Assuming you're at the base of the repo.
cmake --install Build/Windows/Release --prefix Packages/Windows
```
where 'Build/Windows/Release' is your desired build to package, and 'Packages/Windows' is your desired output directory.

On Windows, you can use Visual Studio's developer terminal (`Tools` -> `Command Line` -> `Developer Command Prompt`) for easy access to CMake.

macOS has extra steps due to app signing:
```
// Assuming you're at the base of the repo.
// $PWD is used because the directory needs to be absolute.
cmake --install Build/macOS/Release --prefix "$PWD/Packages/macOS"
// Re-sign the client app bundle with an add-hoc signature,
// because CMake invalidates the signatures of bundled libraries.
// If you have an Apple developer ID, you can use that instead.
codesign --force --deep -s - ./Packages/macOS/Repose/Client/Repose.app
```

## Contributing
Contributions are very welcome! Feel free to work on features or bugfixes and submit PRs, they'll all be promptly reviewed. If you're looking for ways to contribute, check the [Task Board](https://trello.com/b/8Z8VoAiX/amalgam-engine-tasks).

There's a fairly solid vision for the near-future of this engine. If you would like to work on a larger feature, please make sure it's on [the roadmap](https://worlds.place/roadmap.html), or [join the discord](https://discord.gg/EA2Sg3ar74) and discuss it before getting started.
