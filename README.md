
# Description

This repository contains the source for the SKSE64 plugin used by the [moreHUD SE](https://www.nexusmods.com/skyrimspecialedition/mods/12688) mod for Skyrim Special Edition.  
The plugin works in conjunction with the [ahzaab/moreHUDSEScaleform](https://github.com/ahzaab/moreHUDSEScaleform) Scaleform Elements.  

## How it Works

* The SKSE64 plugin is loaded by [SKSE64](http://skse.silverlock.org/) using the skse64_loader.exe
* The plugin dynamically loads the Scaleform `.swf` movie clip into the HUD Menu when the menu loads.
* The plugin registers Scaleform functions used by the ActionScript 2.0 code associated with the [moreHUD SE SWF file](https://github.com/ahzaab/moreHUDSEScaleform) 
* The plugin provides data to ActionScript such as known ingredients, magic effects, enemy level, known enchantments, etc.

## Installation
The compiled .dll is installed in the Skyrim Data Folder to `Data/SKSE/Plugins`

## Does it need Papyrus?
Not for the plugin itself, but Papyrus is used by the `.swf` file as described [here](https://github.com/ahzaab/moreHUDSEScaleform). 

## Configuration

### HUD compatibility and appearance patches

HUD authors can change the appearance and placement of moreHUD's enemy resource meters, numeric values, and icons without replacing `AHZHudInfo.swf`. The supported configuration file and default external movies are in [`dist/NG/Data/Interface/exported/moreHUD`](https://github.com/ahzaab/moreHUDSE/tree/master/dist/NG/Data/Interface/exported/moreHUD).

Copy the files being customized into the following layout in the patch mod:

```text
Data/
└── Interface/
    └── exported/
        └── moreHUD/
            ├── config.txt
            ├── baseIcons.swf
            ├── enemyMagickaMeter.swf
            └── enemyStaminaMeter.swf
```

Only include the SWFs that the patch actually replaces. If a custom movie uses another filename, include that file and set the corresponding `sSWFPath` in `config.txt`. The paths are relative to `Data/Interface`; when the configuration is loaded from the `exported` directory, moreHUD automatically resolves the standard `moreHUD/...` paths beneath `Data/Interface/exported`.

The loader checks `Data/Interface/moreHUD/config.txt` first and then `Data/Interface/exported/moreHUD/config.txt`. The exported location is recommended for compatibility patches. A loose file will override the copy packaged in moreHUD's BSA. If multiple mods distribute `config.txt`, normal mod-manager file priority applies and only the winning file is used, so authors should clearly document the required priority.

Lines beginning with `;` are comments. Setting names and section names are case-insensitive. Boolean values are `true` or `false`, and SWF paths may be enclosed in single quotes. A missing or blank optional setting uses moreHUD's built-in behavior where available.

#### Enemy meter stacking

| Setting | Description |
| --- | --- |
| `[EnemyMeter] bUseStacking` | When `true`, moreHUD dynamically stacks health, magicka, and stamina and moves the bracket as meters appear or disappear. Set this to `false` when a HUD places the magicka and stamina meters independently. |
| `[EnemyMeter] fHeight` | Visible height of one resource meter before HUD scaling. Stacking uses this value as its vertical step. The default is `11.5`. Match it to the visible height of a replacement meter, not the full SWF canvas. |

#### Magicka and stamina meters

The following settings exist in both `[EnemyMagickaMeter]` and `[EnemyStaminaMeter]`:

| Setting | Description |
| --- | --- |
| `sSWFPath` | Resource movie to load, such as `'moreHUD/enemyMagickaMeter.swf'`. Leave it blank to use the meter clip built into `AHZHudInfo.swf`. |
| `fXOffset` | Horizontal offset from the vanilla enemy-health-meter position. Positive values move right; negative values move left. |
| `fYOffset` | Vertical offset from the calculated meter position. Positive values move down; negative values move up. |
| `fXScale` | Horizontal scale multiplier. `1.0` preserves the HUD's current scale; `0.5` is half width and `2.0` is double width. |
| `fYScale` | Vertical scale multiplier, using the same convention as `fXScale`. |
| `fAlpha` | Meter opacity on Scaleform's `0`–`100` alpha scale. |
| `fNumbersXOffset` | Horizontal offset for that resource's current/maximum numeric display. |
| `fNumbersYOffset` | Vertical offset for that resource's numeric display. |
| `fNumbersXScale` | Horizontal scale multiplier for the numeric display. |
| `fNumbersYScale` | Vertical scale multiplier for the numeric display. |
| `fNumbersAlpha` | Numeric-display opacity on the `0`–`100` scale. |

With stacking enabled, magicka and stamina number offsets are relative to their associated meter. With stacking disabled, their number offsets are relative to the base enemy-health-meter position, allowing each element to be placed manually.

#### Health numbers

`[EnemyHealthMeter]` controls the numeric health display. It supports `fNumbersXOffset`, `fNumbersYOffset`, `fNumbersXScale`, `fNumbersYScale`, and `fNumbersAlpha` with the same meanings described above. The vanilla health meter artwork itself is not replaced through this section.

#### Icons

| Setting | Description |
| --- | --- |
| `[Icons] sSWFPath` | Icon resource movie to load, normally `'moreHUD/baseIcons.swf'`. |
| `[Icons] fScale` | Scale multiplier applied to displayed icons. The default is `1.0`. |
| `[Icons] iSpacing` | Horizontal spacing added between icons. |
| `[Icons] fYOffset` | Vertical adjustment applied to the icons relative to the item text. Positive values move down; negative values move up. |

### Creating replacement SWFs

The safest approach is to reskin the supplied FLA files in `dist/NG/AS2` and publish them as ActionScript 2/Scaleform-compatible SWFs. Replacement meter movies must retain the timeline and instance contract expected by Scaleform's `Meter` component, and replacement icon movies must retain the linkage names requested by moreHUD. Changing only artwork, colors, fonts, and transforms while preserving those names avoids breaking runtime updates.

Test the patch with both the legacy ESP and light ESL editions. They use the same `dist/NG` interface files, so one compatibility patch can support both Skyrim SE and AE. VR remains separate until its SWF and Papyrus assets are incorporated into the shared distribution.

---

## Build Dependencies
* [CMake](https://cmake.org)
* [vcpkg](https://github.com/microsoft/vcpkg)
* [CommonLibSSE-NG v6.7.0](https://github.com/alandtse/CommonLibSSE-NG) (included as a pinned submodule)
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
