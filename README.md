# Skyrim Alchemy Helper

Utility for the Skyrim video game: analyses the user's mods and game save, then computes the list of craftable potions. There are various ways to filter and sort the potions list, and it helps discover the ingredients effects.

More information on [its page on nexusmods](http://www.nexusmods.com/skyrim/mods/70171/?).

## Features

- Automatically detects the Skyrim installation
- Parse the users mods list and extract information about the ingredients and magical effects
- Extract the names from compressed BSA files, in many languages
- Supports the ModOrganizer utility (without having to run it)
- Reads game saves, extracts known ingredient effects
- Tries to find ingredients in all containers
- Reparse the mods to get the containers names and location of containers
- Filters containers in the same cell as the player, or other parameters
- Really fast potions list widget
- Simple filters by ingredients, effects, purity, etc.
- Tool to create a list of the best potions to discover every effect

## Screenshots

![Main window](https://raw.githubusercontent.com/cguebert/SkyrimAlchemyHelper/master/doc/MainWindow.jpg)
![Game save information](https://raw.githubusercontent.com/cguebert/SkyrimAlchemyHelper/master/doc/GameSave.jpg)
![Known ingredients](https://raw.githubusercontent.com/cguebert/SkyrimAlchemyHelper/master/doc/KnownIngredients.jpg)
![Configuration](https://raw.githubusercontent.com/cguebert/SkyrimAlchemyHelper/master/doc/Configuration.jpg)

## Code

C++11 for parsing mods and game saves.  
Qt C++ for the GUI.  
Uses zlib for decompressing part of the files.

## Compilation

I use VCPG to build zlib and lz4, and have Qt5 installed using their installer. Using CMake to build the project:
(replace with the correct paths for vcpkg & qt)
```
cmake .. -G "Visual Studio 15 2017 Win64" -DCMAKE_INSTALL_PREFIX="../install" -DCMAKE_TOOLCHAIN_FILE="{path-to-VCPKG}\scripts\buildsystems\vcpkg.cmake" -DCMAKE_PREFIX_PATH={path-to-qt}\msvc2017_64 -DVCPKG_TARGET_TRIPLET="x64-windows"
cmake --build . --config Release --target Install
```
