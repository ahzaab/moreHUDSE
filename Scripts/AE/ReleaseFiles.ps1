Param(
    [Parameter(Mandatory=$true)]
    [ValidateNotNullOrEmpty()]
    $Version,
    [Parameter(Mandatory=$true)]
    [ValidateNotNullOrEmpty()]
    [int]$AsLightPlugin
)

try{

if ($AsLightPlugin)
{
    $pluginExtesion = '.esl'  #The esl file must have already been compacted and flagged
}
else{
    $pluginExtesion = '.esp'
}

$executingPath = split-path $SCRIPT:MyInvocation.MyCommand.Path -parent

$sourceDir = "$($Env:Skyrim64AEPath)"
$archiveToolDir = "$sourceDir\Tools\Archive"
$sourceDataDir = "$sourceDir\Data"
$releaseDir = "$($Env:ModDevPath)\MODS\SkyrimSE\moreHUD\Release"
$versionDir = "$releaseDir\$Version"
$tempDir = "$versionDir\.tmp"
$destDataDir = "$versionDir\Data"
$destSksePlugin = "$destDataDir\SKSE\Plugins\AHZmoreHUDPlugin.dll"
$sourceSksePlugin = "$sourceDataDir\SKSE\Plugins\AHZmoreHUDPlugin.dll"

$requiredDataDirs = @("Interface","Scripts","Source\Scripts" , "Interface\exported", "Interface\translations", "SKSE\Plugins", "Interface\exported\moreHUD" )

$requiredDataDirs | ForEach-Object{
    if (!$(Test-Path "$destDataDir\$_"))
    {
        New-Item -ItemType Directory "$destDataDir\$_"
    }
}

if ($destSksePlugin -and $sourceSksePlugin){
    Copy-Item $sourceSksePlugin $destSksePlugin
}

$items = Get-ChildItem "$sourceDataDir\Scripts" -Filter ahz*.pex | Where-Object {$_.Name -ne 'AhzMoreHudIE.pex' -and $_.Name -notmatch 'ahz.*?test.*?\.pex'}
$items += Get-ChildItem "$sourceDataDir\Source\Scripts" -Filter ahz*.psc | Where-Object {$_.Name -ne 'AhzMoreHudIE.psc' -and $_.Name -notmatch 'ahz.*?test.*?\.psc'}
$items += @(Get-ChildItem "$sourceDataDir\Interface\exported\moreHUD")
$items += Get-ChildItem "$sourceDataDir" -Filter "ahzmorehud$pluginExtesion"
$items += Get-ChildItem "$sourceDataDir\Interface" -Include @('ahzhudinfo.swf', 'ahzmorehudlogo.dds', 'ahzmorehud_*.txt') -Recurse

$filesToCopy = $items | Select-ObjecT -ExpandProperty FullName

$filesToCopy | ForEach-Object {
    $dest = $_.Replace($sourceDataDir,$destDataDir); 
    Copy-Item $_ $dest
}

# Get te list of files to include in the bsa (Do not include the esp)
$bsaFileList = $filesToCopy | Where-object {$(Get-Item $_).Extension -ne $pluginExtesion -and $(Get-Item $_).Extension -ne '.dll'} | ForEach-Object { $_.Replace($sourceDataDir,"") } | ForEach-Object { $_.TrimStart("\") }

#Get the esp file name and create the name of the bsa file to match the esp file
$pluginFile = $filesToCopy | Where-object {$(Get-Item $_).Extension -eq $pluginExtesion} | Select-Object -First 1
$pluginFile = $pluginFile.Replace("$sourceDataDir\","");
$bsaFileName = $pluginFile.Replace($pluginExtesion, ".bsa")

#Update the bsa file list
Set-Content -Path "$executingPath\bsafilelist.txt" -Value $($bsaFileList -join "`r`n")

# Update bas script with the name of the bsa file
$bsaScript = Get-Content -Path "$executingPath\bsascript.txt"
$bsaScript = $bsaScript.Replace("_generated", "$bsaFileName")

#Create a staging folder because with using the Archive tool all the files need to be in just the right place or it will not work

New-Item -ItemType Directory "$tempDir\Data"
Copy-Item "$destDataDir\*" "$tempDir\Data" -Recurse
Copy-Item "$executingPath\bsafilelist.txt" $tempDir
Set-Content -Path "$tempDir\bsascript.txt" -Value $bsaScript
Copy-Item "$archiveToolDir\Archive.exe" $tempDir

Push-Location $tempDir
Start-Process "$tempDir\Archive" -ArgumentList "bsascript.txt" -wait -NoNewWindow -PassThru
Pop-Location

#Copy back the archive
Copy-Item "$tempDir\$bsaFileName" $versionDir
Copy-Item "$destDataDir\$pluginFile" $versionDir

# Give time for files to not be in use
Start-Sleep -Milliseconds 20

#Prepair for 7z archive
Remove-Item $tempDir\* -Force -Recurse
New-Item -ItemType Directory "$tempDir\Data"
New-Item -ItemType Directory "$tempDir\Data\SKSE\Plugins"
Copy-Item "$versionDir\$bsaFileName" "$tempDir\Data"
Copy-Item "$versionDir\$pluginFile" "$tempDir\Data"
Copy-Item $destSksePlugin "$tempDir\Data\SKSE\Plugins"

$fileVersionNane = $Version.Replace('.', '_')
$zipFileName = $pluginFile.Replace($pluginExtesion, "$fileVersionNane.7z")

Start-Process "C:\Program Files\7-Zip\7z" -ArgumentList "a `"$versionDir\$zipFileName`" `"$tempDir\Data`" -mx5 -t7z" -wait -NoNewWindow -PassThru

}
finally
{   
    if ($(Test-Path "$tempDir"))
    {
        Start-Sleep -Milliseconds 20
        # Cleaup the temp directory
        Remove-Item $tempDir -Force -Recurse
    }
}