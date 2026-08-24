[CmdletBinding()]
param(
    [string]$Version,
    [string]$CMakeExe,
    [string]$VsDevCmd,
    [string]$GameDataDirectory,
    [string]$BaseBsaPath,
    [string]$FlashExe,
    [string]$NpmExe,
    [string]$ArchiveExe,
    [string]$SevenZipExe,
    [switch]$BuildScaleform,
    [switch]$SkipBuild,
    [switch]$SkipPapyrus
)

$ErrorActionPreference = 'Stop'
$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path

# Use the CMake project version by default so package names and DLL metadata cannot drift apart.
if (-not $Version)
{
    $cmakeText = Get-Content -LiteralPath (Join-Path $repositoryRoot 'CMakeLists.txt') -Raw
    $match = [regex]::Match($cmakeText, '(?ms)project\s*\(\s*AHZmoreHUDPlugin\s+VERSION\s+(\d+\.\d+\.\d+\.\d+)')
    if (-not $match.Success)
    {
        throw 'Could not read the project version from CMakeLists.txt.'
    }

    $Version = $match.Groups[1].Value
}

if ($Version -notmatch '^\d+\.\d+\.\d+\.\d+$')
{
    throw "Invalid four-part version: $Version"
}

function Expand-BsaFile
{
    param(
        [string]$ArchivePath,
        [string]$RelativePath,
        [string]$Destination
    )

    # This minimal reader hydrates an unchanged loose SWF from a Skyrim SE BSA v105.
    # It intentionally rejects compressed records instead of attempting an unsafe partial extraction.
    $stream = [IO.File]::OpenRead($ArchivePath)
    $reader = [IO.BinaryReader]::new($stream)

    try
    {
        $magic = [Text.Encoding]::ASCII.GetString($reader.ReadBytes(4))
        if ($magic -ne "BSA`0")
        {
            throw "$ArchivePath is not a BSA archive."
        }

        $version = $reader.ReadUInt32()
        if ($version -ne 105)
        {
            throw "Only Skyrim SE BSA version 105 is supported; found $version."
        }

        $reader.ReadUInt32() | Out-Null
        $archiveFlags = $reader.ReadUInt32()
        $folderCount = $reader.ReadUInt32()
        $reader.ReadUInt32() | Out-Null
        $reader.ReadUInt32() | Out-Null
        $reader.ReadUInt32() | Out-Null
        $reader.ReadUInt32() | Out-Null

        $folderFileCounts = @()
        for ($index = 0; $index -lt $folderCount; $index++)
        {
            $reader.ReadUInt64() | Out-Null
            $folderFileCounts += $reader.ReadUInt32()
            $reader.ReadUInt32() | Out-Null
            $reader.ReadUInt64() | Out-Null
        }

        $entries = @()
        foreach ($fileCount in $folderFileCounts)
        {
            $folderNameLength = $reader.ReadByte()
            $folderName = [Text.Encoding]::ASCII.GetString($reader.ReadBytes($folderNameLength)).TrimEnd([char]0)

            for ($fileIndex = 0; $fileIndex -lt $fileCount; $fileIndex++)
            {
                $reader.ReadUInt64() | Out-Null
                $entries += [pscustomobject]@{
                    Folder = $folderName
                    SizeField = $reader.ReadUInt32()
                    Offset = $reader.ReadUInt32()
                }
            }
        }

        foreach ($entry in $entries)
        {
            $nameBytes = [Collections.Generic.List[byte]]::new()
            while (($value = $reader.ReadByte()) -ne 0)
            {
                $nameBytes.Add($value)
            }

            $entry | Add-Member NoteProperty Name ([Text.Encoding]::ASCII.GetString($nameBytes.ToArray()))
        }

        $normalizedTarget = $RelativePath.Replace('/', '\').ToLowerInvariant()
        $matchingEntry = $null
        foreach ($candidate in $entries)
        {
            $candidatePath = (Join-Path $candidate.Folder $candidate.Name).ToLowerInvariant()
            if ($candidatePath -eq $normalizedTarget)
            {
                $matchingEntry = $candidate
                break
            }
        }

        if (-not $matchingEntry)
        {
            throw "$RelativePath was not found in $ArchivePath."
        }

        $sizeField = [uint32]$matchingEntry.SizeField
        $isCompressed = (($archiveFlags -band 4) -ne 0) -xor (($sizeField -band [uint32]0x40000000) -ne 0)
        if ($isCompressed)
        {
            throw "Cannot extract compressed fallback file $RelativePath."
        }

        $size = [int]($sizeField -band [uint32]0x3fffffff)
        $stream.Position = [int64]$matchingEntry.Offset
        if (($archiveFlags -band 0x100) -ne 0)
        {
            $embeddedNameLength = $reader.ReadByte()
            $reader.ReadBytes($embeddedNameLength) | Out-Null
            $size -= ($embeddedNameLength + 1)
        }

        $bytes = $reader.ReadBytes($size)
        New-Item -ItemType Directory -Path (Split-Path -Parent $Destination) -Force | Out-Null
        [IO.File]::WriteAllBytes($Destination, $bytes)
    }
    finally
    {
        $reader.Dispose()
        $stream.Dispose()
    }
}

$sourceDataDirectory = Join-Path $repositoryRoot 'dist\NG\Data'

if ($BuildScaleform)
{
    # Publish every FLA with flc when Flash/Animate and npm are available.
    $scaleformArguments = @{}
    if ($FlashExe)
    {
        $scaleformArguments.FlashExe = $FlashExe
    }

    if ($NpmExe)
    {
        $scaleformArguments.NpmExe = $NpmExe
    }

    & (Join-Path $PSScriptRoot 'BuildScaleform.ps1') @scaleformArguments
}
else
{
    # A clean checkout ignores generated SWFs. Hydrate only the unchanged main movie when FLA publishing is skipped.
    $mainMovies = @(
        'Interface\AHZHudInfo.swf',
        'Interface\exported\AHZHudInfo.swf'
    )
    $missingMovies = @()

    foreach ($mainMovie in $mainMovies)
    {
        if (-not (Test-Path -LiteralPath (Join-Path $sourceDataDirectory $mainMovie) -PathType Leaf))
        {
            $missingMovies += $mainMovie
        }
    }

    if ($missingMovies)
    {
        if (-not $BaseBsaPath)
        {
            $BaseBsaPath = $env:MOREHUD_BASE_BSA
        }

        if (-not (Test-Path -LiteralPath $BaseBsaPath -PathType Leaf))
        {
            throw 'The main SWF is missing. Run with -BuildScaleform, or supply -BaseBsaPath to hydrate the unchanged production movie.'
        }

        foreach ($relativeMovie in $missingMovies)
        {
            $destination = Join-Path $sourceDataDirectory $relativeMovie
            Expand-BsaFile -ArchivePath $BaseBsaPath -RelativePath $relativeMovie -Destination $destination
        }

        Write-Host "Hydrated unchanged main SWF from $BaseBsaPath"
    }
}

if (-not $SkipPapyrus)
{
    # Compile the version-bearing quest script. Other unchanged PEX files remain byte-for-byte stable.
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

    & (Join-Path $PSScriptRoot 'CompilePapyrus.ps1') -GameDataDirectory $GameDataDirectory -ScriptNames @('ahzmainquest.psc')
}

if (-not $SkipBuild)
{
    # Resolve the compiler tools without storing this workstation's installation paths in source control.
    if (-not $CMakeExe)
    {
        $CMakeExe = $env:CMAKE_EXE
        if (-not $CMakeExe)
        {
            $cmakeCommand = Get-Command cmake.exe -ErrorAction SilentlyContinue
            if ($cmakeCommand)
            {
                $CMakeExe = $cmakeCommand.Source
            }
        }
    }

    if (-not (Test-Path -LiteralPath $CMakeExe -PathType Leaf))
    {
        throw 'cmake.exe was not found. Supply -CMakeExe or set CMAKE_EXE.'
    }

    if (-not $VsDevCmd)
    {
        $VsDevCmd = $env:VCVARS64
        if (-not $VsDevCmd)
        {
            $vcvarsCommand = Get-Command vcvars64.bat -ErrorAction SilentlyContinue
            if ($vcvarsCommand)
            {
                $VsDevCmd = $vcvarsCommand.Source
            }
        }
    }

    if (-not (Test-Path -LiteralPath $VsDevCmd -PathType Leaf))
    {
        throw 'Visual Studio x64 environment script was not found. Supply -VsDevCmd or set VCVARS64.'
    }

    # Import vcvars64 into this PowerShell process only. Preserve the caller's selected vcpkg root.
    $callerVcpkgRoot = $env:VCPKG_ROOT
    $developerEnvironment = & $env:ComSpec /d /s /c "`"$VsDevCmd`" >nul && set"
    if ($LASTEXITCODE -ne 0)
    {
        throw "Visual Studio developer environment initialization failed with exit code $LASTEXITCODE."
    }

    foreach ($line in $developerEnvironment)
    {
        $separator = $line.IndexOf('=')
        if ($separator -gt 0)
        {
            $name = $line.Substring(0, $separator)
            $value = $line.Substring($separator + 1)
            [Environment]::SetEnvironmentVariable($name, $value, 'Process')
        }
    }

    if ($callerVcpkgRoot)
    {
        $env:VCPKG_ROOT = $callerVcpkgRoot
    }

    # Disable automatic game deployment during packaging so a running Skyrim process cannot lock the DLL.
    $previousTargets = $env:SkyrimPluginTargets
    try
    {
        $env:SkyrimPluginTargets = ''
        & $CMakeExe --preset build-release-msvc -S $repositoryRoot
        if ($LASTEXITCODE -ne 0)
        {
            throw "CMake configure failed with exit code $LASTEXITCODE."
        }

        & $CMakeExe --build --preset release-msvc
        if ($LASTEXITCODE -ne 0)
        {
            throw "CMake build failed with exit code $LASTEXITCODE."
        }
    }
    finally
    {
        $env:SkyrimPluginTargets = $previousTargets
    }
}

# Refuse to package a missing DLL/PDB pair or a stale DLL from a different version.
$pluginDll = Join-Path $repositoryRoot 'contrib\Distribution\PluginRelease\AHZmoreHUDPlugin.dll'
$pluginPdb = Join-Path $repositoryRoot 'contrib\Distribution\PluginRelease\AHZmoreHUDPlugin.pdb'
if (-not (Test-Path -LiteralPath $pluginDll -PathType Leaf))
{
    throw "Release DLL was not found: $pluginDll"
}
if (-not (Test-Path -LiteralPath $pluginPdb -PathType Leaf))
{
    throw "Release PDB was not found: $pluginPdb"
}


$dllVersion = (Get-Item -LiteralPath $pluginDll).VersionInfo.FileVersion
if ($dllVersion -ne $Version)
{
    throw "Release DLL version is $dllVersion, but package version is $Version."
}

$packageArguments = @{
    Version = $Version
    SourceDataDirectory = $sourceDataDirectory
    PluginDll = $pluginDll
    PluginPdb = $pluginPdb
}

if ($ArchiveExe)
{
    $packageArguments.ArchiveExe = $ArchiveExe
}

if ($SevenZipExe)
{
    $packageArguments.SevenZipExe = $SevenZipExe
}

$packages = & (Join-Path $PSScriptRoot 'PackageRelease.ps1') @packageArguments
$packages | Format-Table -AutoSize
