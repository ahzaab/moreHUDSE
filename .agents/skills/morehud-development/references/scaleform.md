# AS2, FLA, SWF, and Scaleform workflow

## Choose the source of truth

When a movie has a tracked FLA, use Adobe Flash/Animate plus the repository's publisher as the authoring path. This preserves its library, timelines, linkage, publish settings, and class paths. Use FFDec/JPEXS for compiled-movie inspection, comparison, exports, recovery, or a build deliberately based on SWF XML and script injection.

Do not assume FFDec can faithfully round-trip an authored FLA. A decompiled FLA is a reconstruction, even when the resulting SWF appears functional.

## Editing AS2

- Identify the edition, owning FLA, document class/classpath, and publish destination before changing a `.as` file.
- Preserve AS2 syntax and typing (`function ...:Void`, `var`, prototype/timeline semantics). Do not introduce AS3 packages, namespaces, E4X, or AVM2 APIs.
- Treat frame labels, instance names, linkage IDs, exported class names, depths, and loaded SWF paths as API contracts.
- Account for Skyrim's embedded Scaleform/GFx runtime and its extensions rather than relying on desktop Flash Player behavior.
- Keep the AS2 plugin stub synchronized with native Scaleform registration, including spelling, casing, parameter/return shapes, and initialization timing.
- Verify both the physical destination and loader string for resource movies. Keep path casing consistent even when Windows tolerates differences.

## Editing a FLA

FLA changes require Flash/Animate or JSFL executed by it. Before editing, record the target document, timeline, layer, frame/frame label, symbol/library path, linkage identifier, instance name, and publish target. Make the smallest authoring change, save the FLA, then publish.

For automation, create a narrowly scoped JSFL helper in a temporary or repository tooling location. It should fail visibly when an expected document, symbol, layer, or instance is absent. Avoid screen-coordinate automation and do not run concurrent publishers against the same FLA or output directory.

After publishing, verify all expected movies exist and that only intended files changed. When scripts, linkage, frames, symbols, transforms, or fonts changed, inspect the output with FFDec.

## AE publishing prerequisites

- `Scripts/AE/BuildScaleform.ps1` uses the pinned `flc` 3.1.0 package and publishes `dist/NG/AS2/*.fla` into `build/scaleform/AE` by default.
- Resolve Flash from explicit parameters, environment, PATH, or the Windows App Paths registry. A locally discovered Adobe path is machine state and must not be committed.
- The AS2 sources use CLIK/Scaleform classes. If Flash cannot resolve them, use the matching pinned classpath from the repository dependencies; do not duplicate or silently replace the dependency tree.
- If a synced or virtual drive causes `flc` status-file sharing failures, use a unique local temporary output directory, wait for a clean Flash shutdown/status, and copy only verified SWFs to repository destinations.
- Missing local authoring fonts can change previews or trigger modal mapping dialogs. Keep proprietary fonts outside Git and do not alter runtime linkage merely to silence authoring warnings.

The AE publisher expects:

- `AHZhudInfo.swf`
- `baseIcons.swf`
- `enemyMagickaMeter.swf`
- `enemyStaminaMeter.swf`

Treat zero publisher errors plus all expected output files as publish success. This does not establish in-game correctness.

## FFDec/JPEXS

Prefer the vendored CLI at `external/SkyUI-Community/tools/FFDec/ffdec-cli.exe`. Check its `-help` before composing a mutation because flags vary by version. Keep exports and experiments under `build/` or a temporary directory.

Always write mutations to a distinct output, inspect the result, and replace a tracked/generated destination only as part of the requested build. Useful modes include script export/dump, SWF XML export/import, `-xml2swf`, and `-importScript`.

For the vendored SkyUI Community injection model, consult:

- `external/SkyUI-Community/cmake/SWFXMLPatch.cmake`
- `external/SkyUI-Community/cmake/ImportToSWF.cmake`
- the matching SkyUI source/build declarations

That model rebuilds a base SWF from tracked XML, stages target-specific AS2 with preserved class paths, copies to a distinct output, and imports scripts. Do not apply it to moreHUD's authored FLAs unless the task deliberately changes the build system.

## Verification

For script-only changes, confirm the published SWF contains the expected AS2 class/script and that unrelated movies are unchanged. For timeline/library changes, inspect frame labels, linkage, instance names, transforms, text properties, imported fonts, and load paths. State the exact HUD/menu interaction and edition that still need an in-game Skyrim smoke test.
