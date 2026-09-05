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
