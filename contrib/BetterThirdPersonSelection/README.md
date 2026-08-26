# Better Third Person Selection integration

moreHUD owns the native rollover presentation. This includes the top and bottom
text fields, image-substitution placeholders, separately loaded `baseIcons.swf`
clips, the activation prompt, divider, and the moreHUD information panel.

Better Third Person Selection (BTPS) may position that presentation through the
public ActionScript API on `AHZWidgetContainer.AHZWidget`:

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

Apply `morehud-rollover-layout-v1.patch` to BTPS commit
`93e05fb5b1a4caddb24728f3d0aaaacdef7a40f0` or rebase the adapter changes onto
a later BTPS revision. The patch also includes the two collision-filter API
migrations needed to build against CommonLibVR `ng` commit
`3d81614617910e7f34b33d8750881811b5e36445`. Older moreHUD releases continue
to use the existing BTPS compatibility path.
