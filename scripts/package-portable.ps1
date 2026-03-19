# package-portable.ps1 - Create portable ZIP distribution of IINA for Windows
# Usage: .\scripts\package-portable.ps1 -BuildDir "build\Release" -Version "0.1.0"

param(
    [Parameter(Mandatory=$true)]
    [string]$BuildDir,

    [Parameter(Mandatory=$false)]
    [string]$Version = "0.1.0",

    [Parameter(Mandatory=$false)]
    [string]$OutputDir = "dist"
)

$ErrorActionPreference = "Stop"

# Resolve paths
$ProjectRoot = Split-Path -Parent $PSScriptRoot
$BuildDirAbs = Join-Path $ProjectRoot $BuildDir
$OutputDirAbs = Join-Path $ProjectRoot $OutputDir
$PortableName = "IINA-Windows-$Version-Portable"
$PortableTempDir = Join-Path $env:TEMP $PortableName
$PortableZip = Join-Path $OutputDirAbs "$PortableName.zip"

Write-Host "IINA Windows - Portable Package Builder" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Version:    $Version"
Write-Host "Build Dir:  $BuildDirAbs"
Write-Host "Output:     $PortableZip"
Write-Host ""

# Validate build directory
if (-not (Test-Path $BuildDirAbs)) {
    Write-Error "Build directory not found: $BuildDirAbs"
    exit 1
}

if (-not (Test-Path (Join-Path $BuildDirAbs "iina-windows.exe"))) {
    Write-Error "iina-windows.exe not found in build directory"
    exit 1
}

# Create output directory
New-Item -ItemType Directory -Force -Path $OutputDirAbs | Out-Null

# Clean temporary directory
if (Test-Path $PortableTempDir) {
    Write-Host "Cleaning temporary directory..."
    Remove-Item -Recurse -Force $PortableTempDir
}
New-Item -ItemType Directory -Force -Path $PortableTempDir | Out-Null

Write-Host "Copying application files..." -ForegroundColor Green

# Copy main executable
Copy-Item (Join-Path $BuildDirAbs "iina-windows.exe") $PortableTempDir -Force

# Copy all DLLs (Qt runtime + libmpv)
Write-Host "  - DLLs"
Get-ChildItem $BuildDirAbs -Filter "*.dll" | ForEach-Object {
    Copy-Item $_.FullName $PortableTempDir -Force
}

# Copy ALL subdirectories from the build dir (platforms/, imageformats/,
# tls/, styles/, qml/, translations/, etc. created by windeployqt)
Get-ChildItem $BuildDirAbs -Directory | ForEach-Object {
    Write-Host "  - $($_.Name)/"
    Copy-Item $_.FullName $PortableTempDir -Recurse -Force
}

Write-Host "Adding documentation..." -ForegroundColor Green

# Copy documentation
Copy-Item (Join-Path $ProjectRoot "LICENSE") (Join-Path $PortableTempDir "LICENSE.txt") -Force
Copy-Item (Join-Path $ProjectRoot "README.md") $PortableTempDir -Force
Copy-Item (Join-Path $ProjectRoot "THIRD_PARTY_LICENSES.txt") $PortableTempDir -Force

# Create portable marker file
Write-Host "Creating portable marker..." -ForegroundColor Green
$PortableMarkerPath = Join-Path $PortableTempDir "portable.txt"
@"
IINA Portable Mode
==================

This file indicates that IINA is running in portable mode.

In portable mode, IINA will:
- Store settings in the application directory (config/settings.ini)
- Store playback history locally (config/history.json)
- Store logs in the logs/ subdirectory
- NOT write to the Windows registry or user profile

To enable system-wide installation mode, delete this file.

Version: $Version
Package Date: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
"@ | Out-File -FilePath $PortableMarkerPath -Encoding UTF8

# Create README for portable version
Write-Host "Creating portable README..." -ForegroundColor Green
$PortableReadmePath = Join-Path $PortableTempDir "README-PORTABLE.txt"
@"
IINA for Windows - Portable Edition
====================================

Version: $Version
Package Date: $(Get-Date -Format "yyyy-MM-dd")

What is Portable Mode?
----------------------
This is a portable distribution of IINA that can be run from any location
without installation. All settings and data are stored within this folder.

Requirements
------------
- Windows 10 or later (64-bit)
- No additional dependencies required (all libraries included)

How to Run
----------
1. Extract this entire folder to any location (USB drive, Desktop, etc.)
2. Double-click iina-windows.exe to launch
3. Your settings will be saved in the "config" subfolder

Features
--------
- No installation required
- Run from USB drive or any folder
- Settings stored locally (not in registry)
- Fully self-contained

File Structure
--------------
iina-windows.exe          - Main application
*.dll                     - Runtime libraries (Qt, libmpv)
plugins/                  - Qt plugins (platforms, imageformats, etc.)
qml/                      - QML runtime modules
translations/             - Language files
config/                   - Settings (created on first run)
logs/                     - Application logs (created on first run)
portable.txt              - Portable mode marker
LICENSE.txt               - GPL-3.0 license
THIRD_PARTY_LICENSES.txt  - Third-party software notices

Keyboard Shortcuts
------------------
Space           - Play/Pause
Left/Right      - Seek -5s/+5s
Up/Down         - Volume +5/-5
F               - Fullscreen
Ctrl+O          - Open file
Ctrl+S          - Screenshot
And many more! See Settings > Configure Shortcuts

Support
-------
Website: https://iina.io
GitHub:  https://github.com/iina/iina-windows
Issues:  https://github.com/iina/iina-windows/issues

License
-------
IINA for Windows is free and open source software licensed under GPL-3.0.
See LICENSE.txt for full license text.

Copyright (C) 2026 IINA Contributors
"@ | Out-File -FilePath $PortableReadmePath -Encoding UTF8

# Create pre-configured config directory structure
$ConfigDir = Join-Path $PortableTempDir "config"
New-Item -ItemType Directory -Force -Path $ConfigDir | Out-Null

# Create a default settings.ini for portable mode
$DefaultSettingsPath = Join-Path $ConfigDir "settings.ini"
@"
[General]
portable_mode=true
language=auto
check_for_updates=true

[Video]
hardware_acceleration=auto
screenshot_format=png
screenshot_directory=screenshots

[Audio]
default_volume=100
volume_boost_enabled=true

[Subtitle]
auto_load=true
font_family=Arial
font_size=24

[Advanced]
log_level=info
"@ | Out-File -FilePath $DefaultSettingsPath -Encoding UTF8

Write-Host "Creating ZIP archive..." -ForegroundColor Green

# Remove existing ZIP if present
if (Test-Path $PortableZip) {
    Remove-Item $PortableZip -Force
}

# Create ZIP archive
try {
    Compress-Archive -Path "$PortableTempDir\*" -DestinationPath $PortableZip -CompressionLevel Optimal
    Write-Host "SUCCESS: Portable package created" -ForegroundColor Green
    Write-Host ""

    # Show package info
    $ZipInfo = Get-Item $PortableZip
    Write-Host "Package Information:" -ForegroundColor Cyan
    Write-Host "  File: $($ZipInfo.Name)"
    Write-Host "  Size: $([math]::Round($ZipInfo.Length / 1MB, 2)) MB"
    Write-Host "  Path: $($ZipInfo.FullName)"
    Write-Host ""

    # Count files
    $FileCount = (Get-ChildItem $PortableTempDir -Recurse -File).Count
    Write-Host "  Contains: $FileCount files"
    Write-Host ""

} catch {
    Write-Error "Failed to create ZIP archive: $_"
    exit 1
}

# Clean up temporary directory
Write-Host "Cleaning up..."
Remove-Item -Recurse -Force $PortableTempDir

Write-Host "Done! Portable package ready for distribution." -ForegroundColor Green
Write-Host ""
Write-Host "To distribute:" -ForegroundColor Yellow
Write-Host "  1. Upload $PortableZip to your release page"
Write-Host "  2. Users extract and run iina-windows.exe"
Write-Host "  3. No installation required!"
