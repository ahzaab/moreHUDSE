---
name: morehud-development
description: Modify, diagnose, build, and verify moreHUD SE/AE/VR across its C++ SKSE plugin, Papyrus scripts, ActionScript 2, FLA/SWF assets, and Skyrim Scaleform integration. Use for work in this repository that touches any of those runtime or authoring boundaries.
---

# moreHUD SE development

Treat the repository as a multi-runtime feature spanning native SKSE code, Papyrus/MCM, and Scaleform/GFx. Before editing, identify the supported target (AE, SE, or VR) and trace the relevant contract through native registration, Papyrus or ActionScript producers/consumers, authored FLA timelines and symbols, and the installed movie or script path.

Read [references/repository-map.md](references/repository-map.md) before changing code or generated assets. For AS2, FLA, SWF, or visual UI work, also read [references/scaleform.md](references/scaleform.md). For Papyrus work, read the Papyrus section of the repository map.

## Working rules

- Preserve the repository's CMake/MSVC wrappers, release scripts, pinned dependencies, and AE/SE/VR separation. Use existing scripts rather than inventing parallel build commands.
- Edit authoritative sources, not generated PEX/SWF/package artifacts, unless the request specifically concerns binary inspection or patching. Regenerate tracked outputs from their matching sources and report the compiler/publisher used.
- Keep native, Papyrus, and AS2 contracts synchronized: registered function names, object paths, value types, sentinel values, configuration globals, event timing, and movie paths must agree across layers.
- ActionScript is AS2 targeting Skyrim's Scaleform/GFx runtime, not browser JavaScript or AS3. Preserve AS2 syntax, `_root`/`_parent` behavior, timeline assumptions, linkage identifiers, frame labels, depths, and CLIK conventions.
- FLA files are binary authoring documents. Never byte-edit them. Use Adobe Flash/Animate or narrowly scoped JSFL automation to modify authored documents; use FFDec for inspection, export, comparison, or an explicitly selected SWF/XML/script injection pipeline.
- Do not silently replace an authoritative FLA workflow with decompiler output. SWF-to-FLA reconstruction can lose authoring fidelity even if the result executes.
- Validate the narrowest changed layer first, then its adjacent contract, then the integrated target build when practical. Distinguish build success from an in-game Skyrim test.

## Tool routing

- C++/MSVC, CMake, SKSE, or CommonLibSSE-NG: use `build.ps1`, `build-debug.ps1`, or the applicable release wrapper documented in the repository map.
- `.psc` Papyrus sources: use the AE compiler wrapper when applicable and supply the target game's source dependencies. Never claim a `.pex` changed without successful compiler output.
- AE `.as` beside a tracked `.fla`: edit the AS2 source, then publish the owning FLAs with `Scripts/AE/BuildScaleform.ps1`.
- Timeline, symbol, library, linkage, instance-name, font, or layout changes: edit the owning FLA through Flash/Animate or explicit JSFL, save it, publish it, and inspect the resulting SWF.
- Existing SWF investigation: use the vendored FFDec CLI to export or dump into `build/` or a temporary directory without overwriting source assets.
- SkyUI movie reconstruction or AS2 injection: follow the vendored SkyUI Community XML/`-importScript` model only where that pipeline is deliberately used.

## Completion evidence

Report the target edition, source files changed, outputs regenerated, exact compile/publish/build checks run, and anything that still requires Skyrim. For UI changes, include the menu/movie, FLA symbol or frame/instance involved, installed path, and native or Papyrus data producer when relevant.
