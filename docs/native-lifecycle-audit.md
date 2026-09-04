# Native lifecycle compatibility audit

This branch treats `AHZHudInfo.swf` and its ActionScript 2 member names as a compatibility ABI. The runtime movie may be the current moreHUD movie, a 5.4.0-era movie, or a third-party HUD replacement based on an older moreHUD release.

## Post-5.4.0 classification

| Commit | Classification | Branch treatment |
| --- | --- | --- |
| `9389fcf` CommonLibSSE-NG v6.6.3 | Unrelated dependency/build update | Preserved. |
| `aabe597` repository development guidance | Unrelated documentation | Preserved. |
| `3e9b493` bottom-bar reskin layout | Compatibility-sensitive AS2 fix | Replaced with native authored-right-edge detection and the legacy `PLAYER_CARD_WIDTH` member. The AS2 field and behavior are restored to 5.4.0. |
| `e09fcb9` 5.4.1 release and shared NG distribution | Mixed | Preserved the shared build/distribution and Papyrus readiness work. Replaced HUD close/reset/reinjection with native object-existence checks and persistent ownership. |
| `960539c` CommonLibSSE-NG v6.7.0 | Unrelated dependency update | Preserved. |
| `9bb9098` direct BookMenu visibility fix | Compatibility-sensitive/lifecycle | Replaced with native container visibility handling. Preserved the fully qualified AS2 event dispatcher calls. |
| `271379c` BookMenu rollover suppression | Compatibility-sensitive/lifecycle | Removed the added AS2 state and guards; native code hides the whole moreHUD container while BookMenu is open. |
| `dced03c` BookMode suppression | Compatibility-sensitive/lifecycle | Removed the added AS2 behavior; native BookMenu events are authoritative. |
| `64102b7` Papyrus release-script rebuild | Unrelated generated output | Preserved. |
| `932db9e` repeated widget-generation lifecycle | Mixed | Removed per-generation ownership, teardown, readiness flags, and bracket restoration state. Preserved independent meter-load ordering/error handling, null guards, timeline stopping, and corrected bracket duplication depth. |
| `be5103f` 5.4.2 release metadata | Unrelated release metadata | Preserved. |
| `63046b8` BTPS compatibility and release cleanup | Mixed | Removed the per-widget `IconContainer` alias workaround because the original static `IconContainer` contract is restored. Preserved `kContinue` event-sink behavior and release cleanup. |
| `3ffe11a` 5.4.3 release metadata | Unrelated release metadata | Preserved. |

## Restored AS2 contract

Declaration comparison against tag `5.4.0.0` confirms that no original field or method name is missing from the changed classes.

Restored names and storage semantics include:

- `AHZBottomBar.playerCardWith`, including the original spelling.
- `AHZBottomBar.PLAYER_CARD_WIDTH` as the internal alignment property.
- `AHZIconContainer.iconLoader`.
- Static `AHZIconContainer.eventObject`.
- Static `AHZIconContainer.managerSetup`.
- Static `AHZIconContainer.loadErrorCalled`.
- Static `AHZHudInfoWidget.IconContainer`, initialized once with `new AHZIconContainer()`.
- Original `hookFunction` routing through its captured `a_hookScope`.

Removed lifecycle-only names include `activeWidget`, `instanceIconContainer`, `Dispose`, `iconLoaders`, `meterLoadingComplete`, `clipsAreReady`, `bookModeActive`, and the original-bracket teardown fields.

The remaining post-5.4.0 AS2 symbols are additive local variables or helper methods used by independent enemy-meter load/error handling. They do not replace a 5.4.0 field or method.

## Runtime ownership rules

- Every non-VR HUD open probes `_root.AHZWidgetContainer`.
- An existing container is reused regardless of the native request flag.
- HUD close does not unload or remove the container.
- A missing container causes one injection and arms the native readiness probe.
- `HUDMenu::AdvanceMovie` probes only while armed, after the original advance function.
- Readiness requires `_root.AHZWidgetContainer.AHZWidget` and its legacy `updateSettings` member.
- The `AHZmoreHUD_MovieLoaded` Papyrus event is emitted once for the current HUD generation.
- A queued event from an obsolete HUD generation is discarded.
- BookMenu visibility is applied to the root moreHUD container and restored to its prior value.
- The bottom-bar authored right edge is applied only when the legacy objects and `PLAYER_CARD_WIDTH` member exist.

## In-game validation matrix

1. Start a game and confirm the first HUD creates one moreHUD container.
2. Load two or more saves during the same process and confirm the retained container is reused without another injection.
3. Force a genuine HUD movie recreation and confirm one new injection and one readiness event.
4. Open BookMenu normally and through a direct opener; confirm all moreHUD elements remain hidden until close.
5. Test left, right, and center bottom-bar alignment with the stock HUD and a reskin with a different authored gold-field right edge.
6. Test BTPS 0.8.x against the restored static `AHZHudInfoWidget.IconContainer` member.
7. Repeat the save-load tests with at least one 5.4.0-era or reskinned `AHZHudInfo.swf` that has no `MovieLoaded()` callback.
