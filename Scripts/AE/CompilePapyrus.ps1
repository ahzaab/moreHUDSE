[CmdletBinding()]
param(
    [string]$PapyrusCompiler,
    [string]$GameDataDirectory,
    [string]$SkyUiSourceDirectory,
    [string[]]$ScriptNames = @('ahzmainquest.psc'),
    [string[]]$AdditionalImportDirectories = @()
)

$ErrorActionPreference = 'Stop'
$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path

# Prefer an explicit path, then a release-specific environment variable, then the legacy variable.
if (-not $GameDataDirectory)
{
    if ($env:SKYRIM_AE_DATA)
    {
        $GameDataDirectory = $env:SKYRIM_AE_DATA
    }
    elseif ($env:Skyrim64AEPath)
    {
        $GameDataDirectory = Join-Path $env:Skyrim64AEPath 'Data'
    }
}

if (-not $GameDataDirectory)
{
    throw 'Supply -GameDataDirectory or set SKYRIM_AE_DATA.'
}

$GameDataDirectory = (Resolve-Path -LiteralPath $GameDataDirectory).Path

# The compiler normally sits beside Data in a Creation Kit installation.
if (-not $PapyrusCompiler)
{
    $PapyrusCompiler = Join-Path (Split-Path -Parent $GameDataDirectory) 'Papyrus Compiler\PapyrusCompiler.exe'
}

if (-not (Test-Path -LiteralPath $PapyrusCompiler -PathType Leaf))
{
    throw "Papyrus compiler was not found: $PapyrusCompiler"
}

# SkyUI is pinned as a submodule so MCM scripts can be compiled from a known source revision.
if (-not $SkyUiSourceDirectory)
{
    $SkyUiSourceDirectory = Join-Path $repositoryRoot 'external\SkyUI-Community\source\scripts'
}

if (-not (Test-Path -LiteralPath $SkyUiSourceDirectory -PathType Container))
{
    throw "SkyUI source directory was not found: $SkyUiSourceDirectory. Initialize the SkyUI-Community submodule."
}

$sourceDirectory = Join-Path $repositoryRoot 'dist\AE\Data\Source\Scripts'
$outputDirectory = Join-Path $repositoryRoot 'dist\AE\Data\Scripts'

# SKSE's modified base scripts must precede vanilla sources in the import path.
$gameSourceCandidates = @(
    (Join-Path $GameDataDirectory 'Scripts\Source'),
    (Join-Path $GameDataDirectory 'Source\Scripts')
) | Where-Object { Test-Path -LiteralPath $_ -PathType Container }

$flagsFile = $gameSourceCandidates |
    ForEach-Object { Join-Path $_ 'TESV_Papyrus_Flags.flg' } |
    Where-Object { Test-Path -LiteralPath $_ -PathType Leaf } |
    Select-Object -First 1

if (-not $flagsFile)
{
    throw "TESV_Papyrus_Flags.flg was not found below $GameDataDirectory."
}

$importDirectories = @($sourceDirectory, $SkyUiSourceDirectory) + $gameSourceCandidates + $AdditionalImportDirectories
$importDirectories = $importDirectories |
    Where-Object { $_ -and (Test-Path -LiteralPath $_ -PathType Container) } |
    Select-Object -Unique
$importPath = $importDirectories -join ';'

New-Item -ItemType Directory -Path $outputDirectory -Force | Out-Null

foreach ($scriptName in $ScriptNames)
{
    $sourceFile = Join-Path $sourceDirectory $scriptName
    if (-not (Test-Path -LiteralPath $sourceFile -PathType Leaf))
    {
        throw "Papyrus source file was not found: $sourceFile"
    }

    Write-Host "Compiling $scriptName"
    & $PapyrusCompiler $sourceFile "-f=$flagsFile" "-i=$importPath" "-o=$outputDirectory"
    if ($LASTEXITCODE -ne 0)
    {
        throw "Papyrus compilation failed for $scriptName with exit code $LASTEXITCODE."
    }

    $compiledFile = Join-Path $outputDirectory ([IO.Path]::ChangeExtension($scriptName, '.pex'))
    if (-not (Test-Path -LiteralPath $compiledFile -PathType Leaf))
    {
        throw "Papyrus compiler did not create $compiledFile."
    }
}

Write-Host "Compiled $($ScriptNames.Count) Papyrus script(s) to $outputDirectory"