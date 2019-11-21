
# Description

This Repositiory contains the source for the SKSE64 plugin used by the [moreHUD SE](https://www.nexusmods.com/skyrimspecialedition/mods/12688) mod for Skyrim Special Edition.  
The plugin works in conjunction with the [ahzaab/moreHUDSEScaleform](https://github.com/ahzaab/moreHUDSEScaleform) Scaleform Elements.  

## How it Works

* The SKSE64 plugin is loaded by [SKSE64](http://skse.silverlock.org/) using the skse64_loader.exe
* The plugin dynammically loads the Scaleform .swf movie clip into the Hud Menu when the menu loads.
* The plugin registers Scaleform functions used by the ActionScript 2.0 code associated with the [moreHUD SE swf file](https://github.com/ahzaab/moreHUDSEScaleform) 
* The plugin provides data to the ActionScript such as Known Ingredients, Magic Effects, Enemy Level, Known Echantments, etc.

## Installation
The compiled .dll is installed in the Skyrim Data Folder to `Data/SKSE/Plugins`

## Does it need papyrus?
Not by the Plugin. But only for the .swf file as described [here](https://github.com/ahzaab/moreHUDSEScaleform) 
