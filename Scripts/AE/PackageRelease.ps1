[CmdletBinding()]
param(
    [Parameter(Mandatory = $true)]
    [ValidatePattern('^\d+\.\d+\.\d+\.\d+$')]
    [string]$Version,
    [string]$SourceDataDirectory,
    [string]$PluginDll,
    [string]$PluginPdb,
    [string]$LicenseFile,
    [string]$OutputDirectory,
    [string]$ArchiveExe,
    [string]$SevenZipExe
)

$ErrorActionPreference = 'Stop'
$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path

# Repository-owned inputs and outputs are safe to resolve relative to the checkout.
if (-not $SourceDataDirectory)
{
    $SourceDataDirectory = Join-Path $repositoryRoot 'dist\NG\Data'
}

if (-not $PluginDll)
{
    $PluginDll = Join-Path $repositoryRoot 'contrib\Distribution\PluginRelease\AHZmoreHUDPlugin.dll'
}
if (-not $PluginPdb)
{
    $PluginPdb = Join-Path $repositoryRoot 'contrib\Distribution\PluginRelease\AHZmoreHUDPlugin.pdb'
}
if (-not $LicenseFile)
{
    $LicenseFile = Join-Path $repositoryRoot 'LICENSE.txt'
}


if (-not $OutputDirectory)
{
    $OutputDirectory = Join-Path $repositoryRoot 'release\AE'
}

# Validate both SWF locations because each serves a different UI compatibility path.
$requiredFiles = @(
    $PluginDll,
    $PluginPdb,
    $LicenseFile,
    (Join-Path $SourceDataDirectory 'AHZmoreHUD.esp'),
    (Join-Path $SourceDataDirectory 'AHZmoreHUD.esl'),
    (Join-Path $SourceDataDirectory 'Interface\AHZHudInfo.swf'),
    (Join-Path $SourceDataDirectory 'Interface\exported\AHZHudInfo.swf')
)

foreach ($requiredFile in $requiredFiles)
{
    if (-not (Test-Path -LiteralPath $requiredFile -PathType Leaf))
    {
        throw "Required release input is missing: $requiredFile"
    }
}

# External tools come from parameters, environment variables, or PATH.
if (-not $ArchiveExe)
{
    $ArchiveExe = $env:SKYRIM_ARCHIVE_EXE
    if (-not $ArchiveExe)
    {
        $archiveCommand = Get-Command Archive.exe -ErrorAction SilentlyContinue
        if ($archiveCommand)
        {
            $ArchiveExe = $archiveCommand.Source
        }
    }
}

if (-not $ArchiveExe)
{
    throw 'Archive.exe was not found. Supply -ArchiveExe or set SKYRIM_ARCHIVE_EXE.'
}

if (-not $SevenZipExe)
{
    $SevenZipExe = $env:SEVEN_ZIP_EXE
    if (-not $SevenZipExe)
    {
        $sevenZipCommand = Get-Command 7z.exe -ErrorAction SilentlyContinue
        if ($sevenZipCommand)
        {
            $SevenZipExe = $sevenZipCommand.Source
        }
    }
}

if (-not $SevenZipExe)
{
    throw '7z.exe was not found. Supply -SevenZipExe or set SEVEN_ZIP_EXE.'
}

$tempDirectory = Join-Path ([IO.Path]::GetTempPath()) ("morehud-ae-release-" + [guid]::NewGuid().ToString('N'))
$archiveDataDirectory = Join-Path $tempDirectory 'archive\Data'
New-Item -ItemType Directory -Path $archiveDataDirectory -Force | Out-Null

try
{
    # The BSA contains UI, translations, Papyrus bytecode, and Papyrus sources, but not plugins or DLLs.
    $payloadFiles = Get-ChildItem -LiteralPath $SourceDataDirectory -File -Recurse | Where-Object {
        $_.Extension -notin @('.esp', '.esl', '.bsa', '.dll', '.pdb')
    }

    foreach ($file in $payloadFiles)
    {
        $relativePath = $file.FullName.Substring($SourceDataDirectory.Length).TrimStart('\')
        $destination = Join-Path $archiveDataDirectory $relativePath
        New-Item -ItemType Directory -Path (Split-Path -Parent $destination) -Force | Out-Null
        Copy-Item -LiteralPath $file.FullName -Destination $destination -Force
    }

    $archiveWorkingDirectory = Split-Path -Parent $archiveDataDirectory
    $payloadFiles |
        ForEach-Object { $_.FullName.Substring($SourceDataDirectory.Length).TrimStart('\') } |
        Set-Content -LiteralPath (Join-Path $archiveWorkingDirectory 'bsafilelist.txt') -Encoding ASCII

    # Archive.exe consumes this command file from its working directory.
    @(
        'Log: Archive.log'
        'New Archive'
        'Check: Menus'
        'Check: Misc'
        'Check: Retain Directory Names'
        'Check: Retain File Names'
        'Set File Group Root: Data\'
        'Add File Group: bsafilelist.txt'
        'Save Archive: AHZmoreHUD.bsa'
    ) | Set-Content -LiteralPath (Join-Path $archiveWorkingDirectory 'bsascript.txt') -Encoding ASCII

    $archiveProcess = Start-Process -FilePath $ArchiveExe -ArgumentList 'bsascript.txt' -WorkingDirectory $archiveWorkingDirectory -Wait -PassThru -NoNewWindow
    if ($archiveProcess.ExitCode -ne 0)
    {
        throw "Archive.exe failed with exit code $($archiveProcess.ExitCode)."
    }

    $builtBsa = Join-Path $archiveWorkingDirectory 'AHZmoreHUD.bsa'
    if (-not (Test-Path -LiteralPath $builtBsa -PathType Leaf))
    {
        throw "Archive.exe did not create $builtBsa."
    }

    # Both variants share the freshly built BSA and DLL; only the plugin file differs.
    $packages = @(
        @{ Kind = 'Legacy ESP'; Extension = '.esp'; Suffix = '' },
        @{ Kind = 'Light ESL'; Extension = '.esl'; Suffix = 'esl' }
    )
    $results = @()

    foreach ($package in $packages)
    {
        $packageVersion = $Version + $package.Suffix
        $versionDirectory = Join-Path $OutputDirectory $packageVersion

        # Remove only this exact generated version directory to prevent stale files entering the archive.
        if (Test-Path -LiteralPath $versionDirectory)
        {
            Remove-Item -LiteralPath $versionDirectory -Recurse -Force
        }

        $packageStagingDirectory = Join-Path $versionDirectory 'staging'
        $packageDataDirectory = Join-Path $packageStagingDirectory 'Data'
        $packagePluginDirectory = Join-Path $packageDataDirectory 'SKSE\Plugins'
        New-Item -ItemType Directory -Path $packagePluginDirectory -Force | Out-Null

        $pluginSource = Join-Path $SourceDataDirectory ("AHZmoreHUD" + $package.Extension)
        $pluginName = "AHZmoreHUD$($package.Extension)"
        Copy-Item -LiteralPath $builtBsa -Destination (Join-Path $packageDataDirectory 'AHZmoreHUD.bsa') -Force
        Copy-Item -LiteralPath $pluginSource -Destination (Join-Path $packageDataDirectory $pluginName) -Force
        Copy-Item -LiteralPath $PluginDll -Destination (Join-Path $packagePluginDirectory 'AHZmoreHUDPlugin.dll') -Force
        Copy-Item -LiteralPath $PluginPdb -Destination (Join-Path $packagePluginDirectory 'AHZmoreHUDPlugin.pdb') -Force

        # Keep the GPL text at the package root, beside Data. Mod managers can expose it
        # to users without treating it as a Skyrim Data file during installation/deployment.
        Copy-Item -LiteralPath $LicenseFile -Destination (Join-Path $packageStagingDirectory 'LICENSE.txt') -Force

        # Keep loose copies beside the final archive for inspection and Nexus troubleshooting.
        Copy-Item -LiteralPath $builtBsa -Destination (Join-Path $versionDirectory 'AHZmoreHUD.bsa') -Force
        Copy-Item -LiteralPath $pluginSource -Destination (Join-Path $versionDirectory $pluginName) -Force

        $archiveName = 'AHZmoreHUD' + $packageVersion.Replace('.', '_') + '.7z'
        $releaseArchive = Join-Path $versionDirectory $archiveName
        Push-Location -LiteralPath $packageStagingDirectory
        try
        {
            & $SevenZipExe a $releaseArchive 'Data' 'LICENSE.txt' '-mx5' '-t7z' | Out-Host
            $sevenZipExitCode = $LASTEXITCODE
        }
        finally
        {
            Pop-Location
        }

        if ($sevenZipExitCode -ne 0)
        {
            throw "7-Zip failed for $($package.Kind) with exit code $sevenZipExitCode."
        }

        if (-not (Test-Path -LiteralPath $releaseArchive -PathType Leaf))
        {
            throw "7-Zip did not create $releaseArchive."
        }

        $archiveListing = & $SevenZipExe l -slt $releaseArchive
        if ($LASTEXITCODE -ne 0)
        {
            throw "7-Zip could not verify $releaseArchive."
        }

        $archivePaths = $archiveListing | ForEach-Object {
            if ($_ -match '^Path = (.+)$')
            {
                $Matches[1]
            }
        }
        if ($archivePaths -notcontains 'LICENSE.txt')
        {
            throw "The package license is missing from the root of $releaseArchive."
        }
        if ($archivePaths -contains 'Data\LICENSE.txt')
        {
            throw "The package license must not be deployed under Data in $releaseArchive."
        }

        Remove-Item -LiteralPath (Join-Path $versionDirectory 'staging') -Recurse -Force
        $results += [pscustomobject]@{
            Kind = $package.Kind
            Path = $releaseArchive
        }
    }

    return $results
}
finally
{
    # Always remove the unique temporary tree, including after an Archive.exe or 7-Zip failure.
    if (Test-Path -LiteralPath $tempDirectory)
    {
        Remove-Item -LiteralPath $tempDirectory -Recurse -Force
    }
}
