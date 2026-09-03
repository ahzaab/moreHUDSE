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
