
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
* [CMake](https://cmake.org)
* [vcpkg](https://github.com/microsoft/vcpkg)
* [CommonLibSSE-NG v6.0.0](https://github.com/alandtse/CommonLibSSE-NG) (included as a pinned submodule)
* [spdlog](https://github.com/gabime/spdlog) (Installed by vcpkg)
* [Xbyak](https://github.com/herumi/xbyak) (Installed by vcpkg)
* [Boost](https://www.boost.org/)
	* Stl_interfaces  (Installed by vcpkg)
* [binary_io](https://github.com/Ryan-rsm-McKenzie/binary_io) (Installed by vcpkg)


## End User Dependencies
* [Address Library for SKSE Plugins](https://www.nexusmods.com/skyrimspecialedition/mods/32444)
* [Microsoft Visual C++ Redistributable for Visual Studio 2019](https://support.microsoft.com/en-us/help/2977003/the-latest-supported-visual-c-downloads)
* [SKSE64](https://skse.silverlock.org/)
* [SkyUI](https://www.nexusmods.com/skyrimspecialedition/mods/12604) (Soft requirement.  Only needed for MCM)


## Build Instructions
I use [Visual Studio Code](https://code.visualstudio.com/) for Compilation and Debugging.
Clone with submodules, or initialize them after cloning:

```powershell
git submodule update --init --recursive
```

The build produces one DLL supporting Skyrim SE, AE, and VR.

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

## AE Nexus release

The AE release pipeline builds the DLL, compiles the changed Papyrus script, creates a fresh BSA, and emits separate legacy ESP and light ESL archives. Initialize submodules before packaging so the pinned SkyUI Community sources are available.

Tool locations are supplied through parameters, environment variables, or `PATH`; the scripts do not contain machine-specific installation paths. Supported environment variables are:

* `CMAKE_EXE` — CMake executable
* `VCVARS64` — Visual Studio x64 developer-environment batch file
* `SKYRIM_AE_DATA` — Skyrim AE Data directory containing compiler source dependencies
* `SKYRIM_ARCHIVE_EXE` — Bethesda `Archive.exe`
* `SEVEN_ZIP_EXE` — `7z.exe`
* `FLASH_EXE` — Adobe Flash/Animate executable for optional FLA publishing
* `MOREHUD_BASE_BSA` — existing production BSA used only to hydrate an unchanged main SWF when Flash is unavailable

Build both Nexus packages using the version from `CMakeLists.txt`:

```powershell
& .\Scripts\AE\BuildRelease.ps1
```

To republish all FLA files through the pinned `flc` package before creating the BSA, ensure `npm` is on `PATH` and run:

```powershell
& .\Scripts\AE\BuildRelease.ps1 -BuildScaleform
```

`AHZHudInfo.swf` is intentionally installed in both `Data\Interface` and `Data\Interface\exported` for compatibility with vanilla UI and SkyUI. The package builder validates both copies before creating the BSA.

Release archives are written beneath `release\AE`:

* `<version>\AHZmoreHUD<version>.7z` contains the legacy ESP.
* `<version>esl\AHZmoreHUD<version>esl.7z` contains the light ESL.
