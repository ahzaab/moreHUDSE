
# Description

This Repositiory is for the Scaleform elements used by the [moreHUD SE](https://www.nexusmods.com/skyrimspecialedition/mods/12688) mod for Skyrim Special Edition.  The Scaleform elements work in conjunction with the [ahzaab/moreHUDSE](https://github.com/ahzaab/moreHUDSE) SKSE64 plugin.  

## How it Works

* The SKSE64 plugin loads this scaleform movie dynamically into the HudMenu when the HudMenu first loads.
* Once associated ActionScript 2.0 code is running listens to the cross hair changes and updates the Hud Elements by getting information back from the [SKSE64 Plugin](https://github.com/ahzaab/moreHUDSE) registered functions.
* In addition the associated ActionScript listens to the compass changes to update the enemy level value the is next to the enemies name.

## Installation
The compiled .swf is installed in the Skyrim Data Folder (through a bsa file) to `Data/Interfaces/exported` as well as `Data/Interfaces/` for maximum compatibility.

## Does it need papyrus?
Yes and no.  The swf movie is dynamically loaded by the SKSE64 plugin and does not technically need papypus.  
However, for convenience and for MCM menu support, the settings are stored in Global variables in the esp file and the settings are then
transferred to the ActionScript using the UI Papyrus script.
No polling or process is done in papyrus with the exception of MCM menu configuration and KeyPress events for the toggling functions although technically that could be handled in the plugin.
