
# Description

This Repositiory contains the source for the SKSE64 plugin used by the [moreHUD SE](https://www.nexusmods.com/skyrimspecialedition/mods/12688) mod for Skyrim Special Edition.  
The plugin works in conjunction with the [ahzaab/moreHUDSEScaleform](https://github.com/ahzaab/moreHUDSEScaleform) Scaleform Elements.  

## How it Works

* The SKSE64 plugin is loaded by [SKSE64](http://skse.silverlock.org/) using the skse64_loader.exe
* The plugin dynamically loads the Scaleform .swf movie clip into the Hud Menu when the menu loads.
* The plugin registers Scaleform functions used by the ActionScript 2.0 code associated with the [moreHUD SE swf file](https://github.com/ahzaab/moreHUDSEScaleform) 
* The plugin provides data to the ActionScript such as Known Ingredients, Magic Effects, Enemy Level, Known Echantments, etc.

## Installation
The compiled .dll is installed in the Skyrim Data Folder to `Data/SKSE/Plugins`

## Does it need papyrus?
Not by the Plugin. But only for the .swf file as described [here](https://github.com/ahzaab/moreHUDSEScaleform) 

## Configuration
HUD mod authors, you can reskin or modify the additional enemy meters by including [these](https://github.com/ahzaab/moreHUDSE/tree/master/dist/AE/Data/Interface/exported/moreHUD) files with your mod as a fmod or standalone patch, and modity to meet your needs.  These files must be placed in the `Data/interface/exported/moreHUD` folder.

---

## Build Dependencies
* [cmake](https://cmake.org)
* [vcpkg](https://github.com/microsoft/vcpkg)
* [CommonLibSSE](https://github.com/Ryan-rsm-McKenzie/CommonLibSSE)
* [spdlog](https://github.com/gabime/spdlog) (Installed by vcpkg)
* [Xbyak](https://github.com/herumi/xbyak) (Installed by vcpkg)
* [Boost](https://www.boost.org/)
	* Stl_interfaces  (Installed by vcpkg)
* [binary_io](https://github.com/Ryan-rsm-McKenzie/binary_io) (Installed by vcpkg)


## End User Dependencies
* [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
* [Microsoft Visual C++ Redistributable for Visual Studio 2019](https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads)
* [SKSE64](https://skse.silverlock.org/)
* [SkyUI](https://www.nexusmods.com/skyrimspecialedition/mods/12604)


## Build Instructions
I use [Visual Studio Code](https://code.visualstudio.com/) for Compilation and Debugging.
The following environmental variable is required: `SkyrimAEPath` which points to your skyrim installation directory

Run the following commands for the debug build
```
rm -R .\build
mkdir build
cd .\build
cmake ..
cmake --build .
```

For release build:
```
rm -R .\build
mkdir build
cd .\build
cmake ..
cmake --build . --config Release
```
