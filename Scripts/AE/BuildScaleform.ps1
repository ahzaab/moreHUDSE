[CmdletBinding()]
param(
    [string]$FlashExe,
    [string]$NpmExe,
    [string]$OutputDirectory
)

$ErrorActionPreference = 'Stop'
$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$sourceDirectory = Join-Path $repositoryRoot 'dist\AE\AS2'

# Keep generated SWFs under build/ so publishing never writes temporary files beside the FLA sources.
if (-not $OutputDirectory)
{
    $OutputDirectory = Join-Path $repositoryRoot 'build\scaleform\AE'
}

# Resolve tools without embedding machine-specific installation paths in the repository.
if (-not $FlashExe)
{
    $flashCandidates = @($env:FLASH_EXE)
    if ($env:FlashPath)
    {
        $flashCandidates += Join-Path $env:FlashPath 'Flash.exe'
    }

    foreach ($candidate in $flashCandidates)
    {
        if ($candidate -and (Test-Path -LiteralPath $candidate -PathType Leaf))
        {
            $FlashExe = $candidate
            break
        }
    }

    if (-not $FlashExe)
    {
        $flashCommand = Get-Command Flash.exe, Animate.exe -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($flashCommand)
        {
            $FlashExe = $flashCommand.Source
        }
    }
}

if (-not $FlashExe)
{
    throw 'Adobe Flash/Animate was not found. Supply -FlashExe or set FLASH_EXE.'
}

if (-not $NpmExe)
{
    $npmCommand = Get-Command npm.cmd -ErrorAction SilentlyContinue
    if ($npmCommand)
    {
        $NpmExe = $npmCommand.Source
    }
}

if (-not $NpmExe)
{
    throw 'npm.cmd was not found. Install Node.js or supply -NpmExe.'
}

New-Item -ItemType Directory -Path $OutputDirectory -Force | Out-Null
Push-Location $PSScriptRoot
try
{
    # flc 3.1.0 is pinned in package.json to match the established SkywindUI workflow.
    & $NpmExe install --ignore-scripts
    if ($LASTEXITCODE -ne 0)
    {
        throw "npm install failed with exit code $LASTEXITCODE."
    }

    $flc = Join-Path $PSScriptRoot 'node_modules\.bin\flc.cmd'
    & $flc --interactive-compiler $FlashExe --input-directory $sourceDirectory --output-directory $OutputDirectory --include-pattern '*.fla' --debug false
    if ($LASTEXITCODE -ne 0)
    {
        throw "flc failed with exit code $LASTEXITCODE."
    }
}
finally
{
    Pop-Location
}

# AHZHudInfo.swf is deliberately installed in both locations for vanilla UI and SkyUI compatibility.
$destinations = @{
    'AHZhudInfo.swf' = @('Interface\AHZHudInfo.swf', 'Interface\exported\AHZHudInfo.swf')
    'baseIcons.swf' = @('Interface\exported\moreHUD\baseIcons.swf')
    'enemyMagickaMeter.swf' = @('Interface\exported\moreHUD\enemyMagickaMeter.swf')
    'enemyStaminaMeter.swf' = @('Interface\exported\moreHUD\enemyStaminaMeter.swf')
}

$sourceDataDirectory = Join-Path $repositoryRoot 'dist\AE\Data'
foreach ($publishedName in $destinations.Keys)
{
    $publishedFile = Get-ChildItem -LiteralPath $OutputDirectory -Filter $publishedName -File -Recurse | Select-Object -First 1
    if (-not $publishedFile)
    {
        throw "flc did not publish $publishedName."
    }

    foreach ($relativeDestination in $destinations[$publishedName])
    {
        $destination = Join-Path $sourceDataDirectory $relativeDestination
        New-Item -ItemType Directory -Path (Split-Path -Parent $destination) -Force | Out-Null
        Copy-Item -LiteralPath $publishedFile.FullName -Destination $destination -Force
    }
}

Write-Host "Published AE Scaleform files to $sourceDataDirectory"