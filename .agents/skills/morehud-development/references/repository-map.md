# moreHUD SE repository map

## Native plugin

- C++ sources: `src/`
- Headers: `include/`
- Project definition: `CMakeLists.txt`, `CMakePresets.json`
- Release/debug wrappers: `build.ps1`, `build-debug.ps1`
- Multi-edition/release tooling: `Scripts/`, especially `Scripts/AE/`, `Scripts/SE/`, and `Scripts/VR/`
- Distribution inputs and generated target trees: shared SE/AE `dist/NG` and edition-specific `dist/VR`

Use the root wrappers for ordinary native builds. They import an x64 Visual Studio environment when required, preserve a caller-selected `VCPKG_ROOT`, select the matching CMake/Ninja preset, and deploy only when explicitly given `-DeployTarget`. Do not commit workstation-specific Visual Studio, SDK, game, or Mod Organizer paths.

When tracing a native/UI boundary, search registration and attachment code in `src/Scaleform.cpp`, `src/AHZScaleform.cpp`, and `src/AHZScaleformHook.cpp` together with the matching AS2 plugin declaration. Papyrus native registration is represented by `src/Papyrus.cpp`, `src/AHZPapyrusMoreHud.cpp`, and their headers.

## Edition boundaries

- SE and AE share the CommonLibSSE-NG native plugin, Papyrus, Scaleform sources, and packaged assets under `dist/NG/`.
- The maintained automated Papyrus and FLA publishing pipeline remains under `Scripts/AE/` for historical naming, but its distribution input and output is `dist/NG/`.
- VR has a distinct AS2/FLA surface under `dist/VR/` and separate release tooling under `Scripts/VR/` until its Papyrus and SWF assets are brought into the shared distribution.

Confirm the intended edition before editing. Do not propagate an AE source change into SE or VR mechanically: compare the corresponding movie structure, AS2 class path, Papyrus source, and release layout first.

## Papyrus

- Shared SE/AE sources: `dist/NG/Data/Source/Scripts/`
- Shared SE/AE compiled output: `dist/NG/Data/Scripts/`
- VR sources/outputs: `dist/VR/Data/Source/Scripts/` and `dist/VR/Data/Scripts/`
- Historical/distribution source copies: `contrib/Distribution/PapyrusSources/`
- AE compiler wrapper: `Scripts/AE/CompilePapyrus.ps1`

Treat `dist/NG/Data/Source/Scripts` as the SE/AE build input and `dist/VR/Data/Source/Scripts` as the VR build input unless the task establishes another authoritative source. Check historical copies before editing and avoid silent source drift.

`CompilePapyrus.ps1` accepts `-PapyrusCompiler`, `-GameDataDirectory`, `-SkyUiSourceDirectory`, `-ScriptNames`, and `-AdditionalImportDirectories`. It resolves game data from `SKYRIM_AE_DATA` or legacy `Skyrim64AEPath`, uses the pinned SkyUI Community script sources by default, puts SKSE-modified game sources in the import path, locates `TESV_Papyrus_Flags.flg`, and writes PEX files into the AE output tree. Its default script is only `ahzmainquest.psc`; pass every changed script explicitly.

Example from the repository root:

```powershell
./Scripts/AE/CompilePapyrus.ps1 -GameDataDirectory '<Skyrim AE Data>' -ScriptNames @('ahzmainquest.psc', 'ahzconfigmenu.psc')
```

Never infer that editing a PSC updates a PEX. Record compiler exit success and confirm the expected output exists. Papyrus primarily supports MCM/global configuration and key events; first establish whether a behavior belongs in Papyrus, native code, or AS2.

## SE/AE Scaleform source and output

- Authoritative source tree: `dist/NG/AS2/`
- Main FLA: `dist/NG/AS2/AHZhudInfo.fla`
- Resource FLAs: `baseIcons.fla`, `enemyMagickaMeter.fla`, `enemyStaminaMeter.fla`
- Main AS2 widget: `dist/NG/AS2/ahz/scripts/widgets/AHZHudInfoWidget.as`
- Native bridge declaration: `dist/NG/AS2/ahz/scripts/widgets/AHZCommon/skse/plugins/AHZmoreHUDPlugin.as`
- Publisher: `Scripts/AE/BuildScaleform.ps1`
- Pinned dependency: `Scripts/AE/package.json` (`flc` 3.1.0)
- Generated publish directory: `build/scaleform/AE/`
- Installed main movie: `dist/NG/Data/Interface/AHZHudInfo.swf` and `dist/NG/Data/Interface/exported/AHZHudInfo.swf`
- Installed resources: `dist/NG/Data/Interface/exported/moreHUD/`

Publish from the repository root:

```powershell
./Scripts/AE/BuildScaleform.ps1 -FlashExe '<Adobe Flash/Animate executable>'
```

The script resolves Flash from `-FlashExe`, `FLASH_EXE`, `FlashPath`, or `PATH`; resolves `npm.cmd`; installs the pinned `flc`; publishes every FLA; verifies the four expected SWFs; and copies them to their AE data destinations. `AHZHudInfo.swf` is intentionally installed in both the root Interface directory and `Interface/exported` for vanilla/SkyUI compatibility.

AE integrated release orchestration is in `Scripts/AE/BuildRelease.ps1`. Its `-BuildScaleform` and Papyrus steps are opt-in/configurable through its parameters; deployment and packaging paths must remain explicit.

## Cross-layer tracing

For HUD information or an enemy meter, check this sequence:

1. Native data extraction and GFx value construction.
2. Scaleform registration/callback and exposed plugin method/object name.
3. AS2 bridge declaration and call site.
4. Papyrus/MCM global or UI transfer when configuration participates.
5. AS2 transformation and frame/visibility/layout selection.
6. FLA instance names, linkage identifiers, text fields, and frame labels.
7. Published SWF destination and runtime load path.

Search exact identifiers across `src`, `include`, `dist/<edition>/AS2`, and `dist/<edition>/Data/Source/Scripts`. Contract drift commonly appears as spelling, casing, type, edition, or initialization-timing mismatches.
