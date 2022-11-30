$skip = Read-Host "Building AE? (y/n)"

if ($skip.ToUpper() -eq "Y")
{
    $version = Read-Host "Enter AE Version"
    if (!$version)
    {
        Write-Error "You need to specify a version"
        return -1
    }
    & "$PSScriptRoot\AE\ArchiveFiles.ps1"
    & "$PSScriptRoot\AE\ReleaseFiles.ps1" -Version $version -AsLightPlugin 0
    & "$PSScriptRoot\AE\ReleaseFiles.ps1" -Version $($version + "esl") -AsLightPlugin 1
}

$skip = Read-Host "Building SE? (y/n)"

if ($skip.ToUpper() -eq "Y")
{
    $version = Read-Host "Enter SE Version"
    if (!$version)
    {
        Write-Error "You need to specify a version"
        return -1
    }
    & "$PSScriptRoot\SE\ArchiveFiles.ps1" 
    & "$PSScriptRoot\SE\ReleaseFiles.ps1" -Version $version -AsLightPlugin 0
    & "$PSScriptRoot\SE\ReleaseFiles.ps1" -Version $($version + "esl") -AsLightPlugin 1
}

$skip = Read-Host "Building VR? (y/n)"

if ($skip.ToUpper() -eq "Y")
{
    $version = Read-Host "Enter VR Version"
    if (!$version)
    {
        Write-Error "You need to specify a version"
        return -1
    }
    & "$PSScriptRoot\VR\ArchiveFiles.ps1"
    & "$PSScriptRoot\VR\ReleaseFiles.ps1" -Version $version
}