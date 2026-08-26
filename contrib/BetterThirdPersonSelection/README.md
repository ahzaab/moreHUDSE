# Better Third Person Selection integration — Option 2

moreHUD owns the native rollover presentation. This includes the top and bottom
text fields, image-substitution placeholders, separately loaded `baseIcons.swf`
clips, the activation prompt, divider, and the moreHUD information panel.

moreHUD now owns the BTPS adapter as well. When `BTPS Menu` opens, the native
plugin discovers its `AdvanceMovie` vtable entry, verifies that the function is
owned by `BetterThirdPersonSelection.dll`, and installs a post-update hook. The
hook reads `mc_SelectionWidget`'s position, scale, alpha, and visibility after
BTPS finishes its update, then sends those values to the public ActionScript API
on `AHZWidgetContainer.AHZWidget`:

```actionscript
GetExternalRolloverLayoutVersion():Number
SetExternalRolloverLayout(centerX:Number, centerY:Number,
                          scale:Number, alpha:Number,
                          visible:Boolean):Boolean
ClearExternalRolloverLayout():Void
```

`centerX` and `centerY` are normalized viewport coordinates. `scale` and
`alpha` use multipliers where `1.0` means the native value. The call moves the
entire moreHUD-authored rollover as a unit without changing its internal layout.

The implementation restores native transforms before Skyrim updates the target,
then reapplies the submitted layout after moreHUD performs image substitution.
This prevents cumulative transform drift and allows substituted images and
reskinnable icon clips to continue sizing naturally with the rollover text.

No BTPS source, DLL, SWF, or setting override is required. The adapter
feature-detects BTPS's menu/display-object contract and moreHUD's layout API. It
restores moreHUD's native baseline after BTPS's legacy compatibility update, so
the old icon and content transforms cannot compete with the new layout. It hides
BTPS's duplicate text, divider, information, and activation prompt only after
moreHUD accepts the layout; otherwise BTPS continues rendering normally. BTPS
retains ownership of its progress circle.

If another plugin has already replaced BTPS's `AdvanceMovie` entry, ownership
validation fails closed and the hook is not installed rather than chaining an
unknown patch.
