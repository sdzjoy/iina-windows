# download-deps.ps1 - Download pre-built libmpv for Windows
# Usage: powershell -ExecutionPolicy Bypass -File scripts/download-deps.ps1

param(
    [string]$Arch = "x86_64"
)

$ErrorActionPreference = "Stop"
$ProjectRoot = Split-Path -Parent (Split-Path -Parent $PSScriptRoot)
if (-not $ProjectRoot) { $ProjectRoot = Get-Location }

$DepsDir = Join-Path $ProjectRoot "deps"
$LibDir = Join-Path $DepsDir "lib"
$IncludeDir = Join-Path $DepsDir "include" "mpv"
$TempDir = Join-Path $env:TEMP "iina-deps"

Write-Host "IINA Windows - Dependency Downloader"
Write-Host "Architecture: $Arch"
Write-Host ""

# Create directories
New-Item -ItemType Directory -Force -Path $LibDir | Out-Null
New-Item -ItemType Directory -Force -Path $IncludeDir | Out-Null
New-Item -ItemType Directory -Force -Path $TempDir | Out-Null

# Download libmpv
$mpvUrl = "https://sourceforge.net/projects/mpv-player-windows/files/libmpv/mpv-dev-$Arch-latest.7z/download"
$mpvArchive = Join-Path $TempDir "mpv-dev.7z"

Write-Host "Downloading libmpv development files..."
if (-not (Test-Path $mpvArchive) -or ((Get-Item $mpvArchive).LastWriteTime -lt (Get-Date).AddDays(-7))) {
    Invoke-WebRequest -Uri $mpvUrl -OutFile $mpvArchive -MaximumRedirection 5 -UserAgent "Mozilla/5.0"
}

Write-Host "Extracting..."
$mpvExtract = Join-Path $TempDir "mpv-dev"
if (Test-Path $mpvExtract) { Remove-Item -Recurse -Force $mpvExtract }
7z x $mpvArchive -o"$mpvExtract" -y | Out-Null

# Copy files
Write-Host "Installing to deps/..."
Copy-Item (Join-Path $mpvExtract "libmpv-2.dll") $LibDir -Force

# Try to copy mpv.lib or generate from .dll.a
$mpvLib = Join-Path $mpvExtract "mpv.lib"
$mpvDllA = Join-Path $mpvExtract "libmpv.dll.a"
if (Test-Path $mpvLib) {
    Copy-Item $mpvLib $LibDir -Force
} elseif (Test-Path $mpvDllA) {
    Copy-Item $mpvDllA (Join-Path $LibDir "mpv.lib") -Force
} else {
    Write-Warning "No import library found. You may need to generate mpv.lib from libmpv-2.dll using lib.exe or dlltool."
}

# Copy headers
$headerDir = Join-Path $mpvExtract "include" "mpv"
if (Test-Path $headerDir) {
    Copy-Item (Join-Path $headerDir "*.h") $IncludeDir -Force
} else {
    # Headers might be at top level
    $headers = Get-ChildItem $mpvExtract -Filter "*.h" -Recurse
    foreach ($h in $headers) {
        Copy-Item $h.FullName $IncludeDir -Force
    }
}

# Download WinSparkle (auto-update library)
Write-Host ""
Write-Host "Downloading WinSparkle (auto-update library)..."
$winsparkleUrl = "https://github.com/vslavik/winsparkle/releases/download/v0.8.1/WinSparkle-0.8.1.zip"
$winsparkleArchive = Join-Path $TempDir "winsparkle.zip"

try {
    Invoke-WebRequest -Uri $winsparkleUrl -OutFile $winsparkleArchive -MaximumRedirection 5 -UserAgent "Mozilla/5.0"

    Write-Host "Extracting WinSparkle..."
    $winsparkleExtract = Join-Path $TempDir "winsparkle"
    if (Test-Path $winsparkleExtract) { Remove-Item -Recurse -Force $winsparkleExtract }
    Expand-Archive -Path $winsparkleArchive -DestinationPath $winsparkleExtract

    # Copy WinSparkle files
    Write-Host "Installing WinSparkle to deps/..."

    # Find the actual directory structure (varies by release)
    $winsparkleRoot = Get-ChildItem $winsparkleExtract -Directory | Select-Object -First 1
    if (-not $winsparkleRoot) {
        $winsparkleRoot = $winsparkleExtract
    }

    # Copy DLL (x64 version)
    $winsparkleX64Dir = Join-Path $winsparkleRoot.FullName "x64" "Release"
    if (-not (Test-Path $winsparkleX64Dir)) {
        $winsparkleX64Dir = Join-Path $winsparkleRoot.FullName "Release"
    }

    if (Test-Path (Join-Path $winsparkleX64Dir "WinSparkle.dll")) {
        Copy-Item (Join-Path $winsparkleX64Dir "WinSparkle.dll") $LibDir -Force
        Write-Host "  - WinSparkle.dll"
    } else {
        Write-Warning "WinSparkle.dll not found in expected location"
    }

    # Copy import library
    if (Test-Path (Join-Path $winsparkleX64Dir "WinSparkle.lib")) {
        Copy-Item (Join-Path $winsparkleX64Dir "WinSparkle.lib") $LibDir -Force
        Write-Host "  - WinSparkle.lib"
    } else {
        Write-Warning "WinSparkle.lib not found - you may need to generate it"
    }

    # Copy header
    $winsparkleHeader = Join-Path $winsparkleRoot.FullName "include" "winsparkle.h"
    if (-not (Test-Path $winsparkleHeader)) {
        $winsparkleHeader = Get-ChildItem $winsparkleRoot.FullName -Filter "winsparkle.h" -Recurse | Select-Object -First 1
    }

    if ($winsparkleHeader) {
        Copy-Item $winsparkleHeader.FullName $IncludeDir -Force
        Write-Host "  - winsparkle.h"
    } else {
        Write-Warning "winsparkle.h not found"
    }

} catch {
    Write-Warning "Failed to download WinSparkle: $_"
    Write-Warning "Auto-update feature will not be available. You can download manually from:"
    Write-Warning "  https://github.com/vslavik/winsparkle/releases"
}

Write-Host ""
Write-Host "Done! Dependencies installed to:"
Write-Host "  Libraries: $LibDir"
Write-Host "  Headers:   $IncludeDir"
Write-Host ""
Write-Host "Files:"
Get-ChildItem $LibDir | ForEach-Object { Write-Host "  $($_.Name) ($([math]::Round($_.Length / 1MB, 1)) MB)" }
Get-ChildItem $IncludeDir | ForEach-Object { Write-Host "  $($_.Name)" }
