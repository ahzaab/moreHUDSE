# Repository agent notes

## Scaleform build dependencies

`dist/NG/AS2/*.fla` uses the authored ActionScript 2 classpath
`../CLIK;../greensock-as2;../Common;...`. Before publishing the SWFs, stage
the external SkyUI compiler sources at these exact relative paths:

- Copy `J:/dev/Projects/ScaleForm/src/CLIK/*` (or the equivalent
  `J:/dev/SKYUIORG/skyrimui/src/CLIK/*`) to `dist/NG/CLIK/`.
- Copy `J:/dev/SKYUIORG/skyrimui/src/common/*` to `dist/NG/Common/`.

These are temporary compiler dependencies. Do not commit or package them.
Remove `dist/NG/CLIK/` and `dist/NG/Common/` after the SWFs have been
published and verified. Both directories are ignored by Git as a safeguard.

## Native reverse-engineering hardening

CommonLibSSE-NG models Skyrim from reverse-engineered layouts and functions;
do not treat engine-facing pointers or runtime objects as inherently safe.

- Before dereferencing, validate engine singletons, SKSE interfaces, event
  payloads, cast results, menu instances, GFx movie views, and optional members.
- Treat relocations, vtables, captured original hook functions, and runtime
  edition offsets as fallible. Verify that resolved addresses are nonzero before
  installing or invoking hooks, and preserve the original function call order.
- Check GFx lookup/invocation return values and object/value types before using
  returned members. A replacement SWF may omit or reshape legacy objects.
- Prefer an early return with a diagnostic log when required runtime state is
  unavailable. Do not continue into a dereference merely because the state is
  normally present in an observed game build.
- Include a null/access-violation hardening pass in the review of every native
  change, especially code shared across SE, AE, and VR.

## Crosshair target lifetime invariant (mandatory)

The crosshair rollover target is valid only while Skyrim/SKSE is resolving it.
This rule is mandatory and must not be weakened or bypassed:

- `CAHZTarget::SetTarget` may be called only synchronously from
  `Events::CrosshairHandler::ProcessEvent` while it is handling an
  `SKSE::CrosshairRefEvent`. For VR, where moreHUD supplies SKSE's equivalent
  hook, it may be called only inside `Hook_WandLookupREFRByHandle_Impl`, while
  the successful `LookupReferenceByHandle` result and its owning `NiPointer`
  are still in scope.
- Never call `CAHZTarget::SetTarget`, resolve, dereference, cache for later use,
  or replay the crosshair target from a menu event, HUD message, Scaleform or
  Papyrus callback, task, timer, saved `TESObjectREFR*`, saved `NiPointer`, saved
  `ObjectRefHandle`, or `HUDData::crosshairRef`.
- Derive all moreHUD rollover information synchronously inside the authorized
  crosshair hook and publish only pointer-free `TargetData` snapshots. Any raw
  reference/form/helper pointers used during derivation must be treated as
  transient and cleared before the hook returns.
- When the current target must be refreshed without changing the crosshair,
  request a vanilla refresh with a validated
  `PlayerCharacter::UpdateCrosshairs()` call. Let Skyrim resolve the live
  reference and re-enter the normal SKSE crosshair hook; never fabricate or
  replay a target-bearing HUD message.
- Every native review must search all `SetTarget` call sites and all storage of
  crosshair target pointers/handles. Any new path outside the two authorized
  hook contexts above must be rejected and redesigned before merge or release.
- Keep `CAHZTarget::SetTarget` private and grant access only to those two hook
  contexts. Do not broaden its visibility or add another friend/capability as a
  shortcut; a new caller is an invariant violation even if it appears safe.
