# Native lifecycle compatibility audit

This branch treats `AHZHudInfo.swf` and its ActionScript 2 member names as a compatibility ABI. The runtime movie may be the current moreHUD movie, a 5.4.0-era movie, or a third-party HUD replacement based on an older moreHUD release.

## Post-5.4.0 classification

| Commit | Classification | Branch treatment |
| --- | --- | --- |
| `9389fcf` CommonLibSSE-NG v6.6.3 | Unrelated dependency/build update | Preserved. |
| `aabe597` repository development guidance | Unrelated documentation | Preserved. |
| `3e9b493` bottom-bar reskin layout | Compatibility-sensitive AS2 fix | Replaced with native authored-right-edge detection and the legacy `PLAYER_CARD_WIDTH` member. The AS2 field and behavior are restored to 5.4.0. Negative local coordinates are accepted because translated replacement movies such as Edge UI legitimately author the player card outside the parent origin. |
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
| `a60ee63` 5.4.4.0 release and native lifecycle hardening | Native hardening | Added event/menu/movie/relocation guards without adding any target lookup, retention, or `SetTarget` caller. The existing SE/AE crosshair event and VR lookup hook remain the only target-update contexts. |
| `40001fa` / `a146005` 5.4.4.1 gold crash fix and merge | Unrelated native fix | Replaced the unsafe CommonLib gold helper with an `IsGold()` inventory filter. No crosshair target lifecycle changes. |
| `186f26e` 5.4.5.0 book rollover refresh | **Target lifecycle violation** | Cached `HUDData::crosshairRef`, resolved it after BookMenu close, called `CAHZTarget::SetTarget`, and replayed a target-bearing HUD message outside the SKSE crosshair hook. This path is removed by the pending correction and must not be restored. |
| Pending 5.4.5.0 correction | Required target lifecycle fix | Uses `PlayerCharacter::UpdateCrosshairs()` only to clear vanilla's `shouldUpdateCrosshair` flag. Skyrim resolves the live reference on its next update, SKSE dispatches the normal crosshair event, and moreHUD rebuilds its pointer-free `TargetData` snapshot inside that event. |

## Crosshair target lifetime audit

The audit covers every commit and release snapshot from `5.4.0.0` through the
current 5.4.5.0 worktree.

- Releases `5.4.0.0` through `5.4.4.1` have exactly two `SetTarget` contexts:
  `CrosshairHandler::ProcessEvent` for SE/AE and the hooked
  `LookupReferenceByHandle` call for VR. Both consume an owning pointer supplied
  by the active lookup and complete target derivation synchronously.
- `5.4.5.0` added the only out-of-hook call. The BookMenu restore code retained
  an `ObjectRefHandle`, resolved it later, and replayed its stale rollover
  payload. The pending correction deletes the payload cache, handle resolution,
  fabricated `HUDData`, and direct `SetTarget` call.
- CommonLib updates in `9389fcf` and `960539c` changed dependencies and VM data
  accessors, not target ownership. The HUD/BookMenu/Scaleform lifecycle commits
  from `9bb9098` through `d66f7a8` operate on GFx values and pointer-free state;
  they did not add a crosshair target access path.
- `CAHZTarget::UpdateTarget` dereferences its private reference/form/helper
  pointers only during the synchronous `SetTarget` call. Scaleform reads copies
  of `TargetData`, which contains IDs, enums, numbers, booleans, strings, and
  vectors but no engine pointers. The transient internal pointers are now
  cleared with a scope guard before `SetTarget` returns, including exceptional
  exits. `SetTarget` is private, and C++ friendship restricts callers to the
  SE/AE event handler and VR lookup hook so an accidental third caller fails to
  compile.
- The enemy-health lookup in `AHZScaleformHook.cpp` is a distinct engine hook:
  it derives and stores only `CAHZActorData` values while its owning `NiPointer`
  is in scope. Its saved `lastRefHandle` is compared as an opaque value and is
  never resolved later. It does not feed `CAHZTarget` or the rollover target.

Mandatory rule: no code may resolve, retain for later use, replay, or
dereference the crosshair rollover target outside the SE/AE SKSE crosshair event
or VR's equivalent hooked lookup. A refresh must invalidate vanilla crosshair
state and allow Skyrim/SKSE to produce a new event from the live reference.

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
- Native `BookMode` push/pop messages are tracked alongside BookMenu open/close events.
- While either book state is active, HUD messages and movie advances re-hide both the root container and the legacy static `IconContainer`; this suppresses rollover icons such as `ahzEye`, whose clips are parented beside the vanilla rollover text rather than under the moreHUD container.
- The bottom-bar authored right edge is applied only when the legacy objects and `PLAYER_CARD_WIDTH` member exist. Any finite local coordinate is valid, including Edge UI's negative right edge.

## In-game validation matrix

The 5.4.6.0 package accidentally reused a generated main HUD movie containing
`ProcessInteriorCellStatus()` and a call to the unregistered `GetInteriorCellStatus()`
native function. FFDec comparison against the 5.4.5.0 BSA shows that extra function and
call as the only widget-script difference. The local distribution inputs have been
restored byte-for-byte to the 5.4.5.0 main movie (SHA-256
`CBD14647151F932D78305797F289CF31DD9BBEA77B7DDB1F52E28B700F0F7649`).
The release wrapper now requires that verified hash unless an intentional movie update
supplies a different expected hash. Authored AS2/FLA files are unchanged by this repair.

Separately, the 5.4.6.0 native read-book query gated its snapshot fact on transient HUD
visibility/alpha. The repair returns the snapshot's book-read fact independently of
those measurements, retaining explicit BookMenu/BookMode suppression. The unchanged
movie controls whether to draw the eye beside its rollover text. A callback regression
test verifies that alternating movie visibility cannot change the fact returned to AS2.
These findings identify a broken movie/native contract and a timing-sensitive gate;
the reported in-game flashing still requires confirmation with the repaired package.

The post-5.4.6.0 snapshot refactor moves target inventory counts,
equipment comparisons, localized armor/book skill names, Completionist icon lookup,
registered form-list membership queries, and location-status translation/marker reads
into `CAHZTarget::SetTarget` derivation.
Only the SE/AE crosshair event and VR wand lookup hook can enter that private path.
The published `TargetData` contains owning strings and scalar values, is copied under
a mutex, and contains no engine pointers or handles. Target helper pointers are cleared
before the hook returns. Scaleform callbacks read snapshots and perform GFx operations;
HUD message decoration appends the previously captured location-status string.
Each invocation uses its own transient derivation context, so concurrent or reentrant
hooks cannot share borrowed target pointers or partially constructed snapshot strings.
Publication uses invocation sequence numbers so an older derivation finishing later
cannot replace a newer published snapshot.
Hook placement alone does not establish arbitrary engine-member lifetime or thread
safety; the snapshot boundary prevents those pointers from escaping to later callbacks.

Enemy meters retain their distinct owning enemy-reference lookup hook, including player
level and localized soul-name capture. Their UI callback also consumes a pointer-free
snapshot. This preserves continuous meter updates when the crosshair points elsewhere.
UI movie operations, hook installation and validated vanilla crosshair refresh requests
remain in their required UI/lifecycle contexts; they do not derive target game data.
The restriction applies to game objects directly related to the current target.
Papyrus APIs may still access their own object arguments, and independent player-only
queries (gold, carry weight and combat state) retain their live reads. GFx/UI objects,
startup form-table initialization and SKSE setup interfaces remain available in their
normal contexts. Vanilla refresh resolves the live
target through the normal engine publisher and re-enters the authorized crosshair hook;
moreHUD never looks up or replays a saved target from the refresh request.

Run `python Scripts/VerifySnapshotBoundary.py` alongside the native build to check the
producer/consumer boundary. In-game testing must also confirm updates while keeping the
crosshair still after inventory/equipment changes, and enemy meter updates while looking
away. A successful build and boundary audit do not demonstrate that the reported
Scaleform object corruption has been eliminated.

For a controlled Completionist API comparison, set the following in
`Data/SKSE/Plugins/AHZmoreHUDPlugin.ini` and restart Skyrim:

```ini
[Compatibility]
EnableCompletionistAPI=0
```

This disables moreHUD's API initialization and icon queries while leaving Completionist
installed. The default remains enabled when the setting is absent; set it to `1` and
restart to re-enable. Check `moreHUDSE.log` for the disabled diagnostic. This comparison
isolates the new API-driven integration and its icons; it does not disable Completionist's
own hooks or prove which component caused the original corruption.

1. Start a game and confirm the first HUD creates one moreHUD container.
2. Load two or more saves during the same process and confirm the retained container is reused without another injection.
3. Force a genuine HUD movie recreation and confirm one new injection and one readiness event.
4. Open BookMenu normally and through a direct opener; confirm all moreHUD elements remain hidden until close.
5. Test left, right, and center bottom-bar alignment with the stock HUD and a reskin with a different authored gold-field right edge.
6. Test BTPS 0.8.x against the restored static `AHZHudInfoWidget.IconContainer` member.
7. Repeat the save-load tests with at least one 5.4.0-era or reskinned `AHZHudInfo.swf` that has no `MovieLoaded()` callback.
8. With Read Or Take active, open a book while holding its modifier, release the modifier while BookMenu remains open, and confirm `ahzEye` stays hidden until both BookMenu and `BookMode` have ended.
