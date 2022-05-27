$sourcePath = "$($Env:SkyrimVRPath)\Data"
$destinationDataPath = "$($Env:ModDevPath)\MODS\SkyrimSE\moreHUD\Plugin\dist\VR\Data"
$destinationAS2Path = "$($Env:ModDevPath)\MODS\SkyrimSE\moreHUD\Plugin\dist\VR\AS2"

if (!$(Test-Path "$destinationDataPath\Interface\vr"))
{
    New-Item -ItemType Directory "$destinationDataPath\Interface\vr"
    New-Item -ItemType Directory "$destinationDataPath\meshes"
    New-Item -ItemType Directory "$destinationDataPath\Scripts"
    New-Item -ItemType Directory "$destinationDataPath\Source\Scripts"   
}

if (!$(Test-Path "$destinationDataPath\Interface\translations"))
{
    New-Item -ItemType Directory "$destinationDataPath\Interface\translations"
}

if (!$(Test-Path "$destinationDataPath\Interface\exported"))
{
    New-Item -ItemType Directory "$destinationDataPath\Interface\exported"
}

Copy-Item "$sourcePath\Interface\translations\ahzmorehud*.txt" -Destination "$destinationDataPath\Interface\translations"
Copy-Item "$sourcePath\Interface\vr\activaterollover.swf" -Destination "$destinationDataPath\Interface\vr"
Copy-Item "$sourcePath\Interface\vr\AHZEnemyLevel.swf" -Destination "$destinationDataPath\Interface\vr"
Copy-Item "$sourcePath\meshes\vr_enemyhealthbar.nif" -Destination "$destinationDataPath\meshes"
Copy-Item "$sourcePath\Scripts\ahz*.pex" -Exclude "AhzMoreHudIE.pex" -Destination "$destinationDataPath\Scripts"
Copy-Item "$sourcePath\Source\Scripts\ahz*.psc" -Exclude "AhzMoreHudIE.psc" -Destination "$destinationDataPath\Source\Scripts"
Copy-Item "$sourcePath\AHZmoreHUD.esp" -Destination "$destinationDataPath"
Copy-Item "$sourcePath\Interface\exported\ahz*.*" -Exclude "Ahz*Inventory.swf" -Destination "$destinationDataPath\Interface\exported"

#AS2


Copy-Item "$($Env:ModDevPath)\MODS\SkyrimVR\moreHUD\ScaleForm\src\HUDWidgets\*" -Destination $destinationAS2Path -Exclude .git* -Recurse -Force