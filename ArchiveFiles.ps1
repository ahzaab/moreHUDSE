$sourcePath = "$($Env:Skyrim64Path)\Data"
$destinationDataPath = "$($Env:ModDevPath)\MODS\SkyrimSE\moreHUD\SKSE64\skse64\moreHUD\Data"
$destinationAS2Path = "$($Env:ModDevPath)\MODS\SkyrimSE\moreHUD\SKSE64\skse64\moreHUD\AS2"

if (!$(Test-Path "$destinationDataPath\Interface"))
{
    New-Item -ItemType Directory "$destinationDataPath\Interface"
    New-Item -ItemType Directory "$destinationDataPath\Scripts"
    New-Item -ItemType Directory "$destinationDataPath\Source\Scripts"
}

if (!$(Test-Path "$destinationDataPath\Interface\exported"))
{
    New-Item -ItemType Directory "$destinationDataPath\Interface\exported"
}

if (!$(Test-Path "$destinationDataPath\Interface\translations"))
{
    New-Item -ItemType Directory "$destinationDataPath\Interface\translations"
}

if (!$(Test-Path "$destinationDataPath\Interface\moreHUD"))
{
    New-Item -ItemType Directory "$destinationDataPath\Interface\moreHUD"
}

if ($(Test-Path "$sourcePath\Interface\moreHUD"))
{
    Copy-Item "$sourcePath\Interface\moreHUD\*.*" -Destination "$destinationDataPath\Interface\moreHUD"
}

if ($(Test-Path "$sourcePath\Interface\moreHUD"))
{
    if (!$(Test-Path "$destinationDataPath\Interface\moreHUD"))
    {
        New-Item -ItemType Directory "$destinationDataPath\Interface\moreHUD"
    }
    Copy-Item "$sourcePath\Interface\moreHUD\*.*" -Destination "$destinationDataPath\Interface\moreHUD"
}

Copy-Item "$sourcePath\Scripts\ahz*.pex" -Destination "$destinationDataPath\Scripts"
Copy-Item "$sourcePath\Source\Scripts\ahz*.psc" -Exclude AhzMoreHudIE.psc -Destination "$destinationDataPath\Source\Scripts"
Copy-Item "$sourcePath\ahzmorehud.esp" -Destination "$destinationDataPath"
Copy-Item "$sourcePath\ahzmorehud.esl" -Destination "$destinationDataPath"
Copy-Item "$sourcePath\Interface\ahzhudinfo.swf" -Destination "$destinationDataPath\Interface"
Copy-Item "$sourcePath\Interface\exported\ahzmorehudlogo.dds" -Destination "$destinationDataPath\Interface\exported"

Copy-Item "$sourcePath\Interface\translations\ahz*.txt" -Destination "$destinationDataPath\Interface\translations"

#AS2


Copy-Item "$($Env:ModDevPath)\MODS\SkyrimSE\moreHUD\ScaleForm\src\HUDWidgets\*" -Destination $destinationAS2Path -Exclude .git* -Recurse -Force