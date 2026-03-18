# download-deps.ps1 - Download pre-built libmpv for Windows
# Usage: powershell -ExecutionPolicy Bypass -File scripts/download-deps.ps1

param(
    [string]$Arch = "x86_64"
)

$ErrorActionPreference = "Stop"
# $PSScriptRoot = <repo>/scripts, so one Split-Path gives <repo>
$ProjectRoot = Split-Path -Parent $PSScriptRoot
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

# --- Download libmpv ---
# Use GitHub API to find latest mpv-dev release from shinchiro/mpv-winbuild-cmake.
# SourceForge uses JavaScript redirects that break automated downloads.
$mpvArchive = Join-Path $TempDir "mpv-dev.7z"

Write-Host "Downloading libmpv development files..."
if (-not (Test-Path $mpvArchive) -or ((Get-Item $mpvArchive).LastWriteTime -lt (Get-Date).AddDays(-7))) {
    $downloaded = $false

    # Method 1: GitHub API - shinchiro/mpv-winbuild-cmake latest release
    Write-Host "  Trying GitHub (shinchiro/mpv-winbuild-cmake)..."
    try {
        $headers = @{ "Accept" = "application/vnd.github+json" }
        $latestRelease = Invoke-RestMethod -Uri "https://api.github.com/repos/shinchiro/mpv-winbuild-cmake/releases/latest" -Headers $headers
        $devAsset = $latestRelease.assets | Where-Object { $_.name -match "mpv-dev-x86_64.*\.7z$" } | Select-Object -First 1
        if ($devAsset) {
            Write-Host "  Found: $($devAsset.name)"
            curl.exe -L -o $mpvArchive $devAsset.browser_download_url
            $fileSize = (Get-Item $mpvArchive).Length
            if ($LASTEXITCODE -eq 0 -and $fileSize -gt 1MB) {
                Write-Host "  Downloaded $([math]::Round($fileSize / 1MB, 1)) MB"
                $downloaded = $true
            } else {
                Write-Host "  Download too small ($fileSize bytes), trying next source..."
            }
        }
    } catch {
        Write-Host "  GitHub API failed: $_"
    }

    # Method 2: SourceForge direct CDN URL (bypasses JavaScript redirect page)
    if (-not $downloaded) {
        Write-Host "  Trying SourceForge direct CDN..."
        $sfUrl = "https://downloads.sourceforge.net/project/mpv-player-windows/libmpv/mpv-dev-$Arch-latest.7z"
        curl.exe -L -A "Mozilla/5.0 (Windows NT 10.0; Win64; x64)" -o $mpvArchive $sfUrl
        $fileSize = (Get-Item $mpvArchive -ErrorAction SilentlyContinue).Length
        if ($LASTEXITCODE -eq 0 -and $fileSize -gt 1MB) {
            Write-Host "  Downloaded $([math]::Round($fileSize / 1MB, 1)) MB"
            $downloaded = $true
        } else {
            Write-Host "  SourceForge CDN also failed ($fileSize bytes)"
        }
    }

    if (-not $downloaded) {
        throw "Failed to download libmpv from all sources. Check network or update download URLs."
    }
}

Write-Host "Extracting..."
$mpvExtract = Join-Path $TempDir "mpv-dev"
if (Test-Path $mpvExtract) { Remove-Item -Recurse -Force $mpvExtract }
7z x $mpvArchive -o"$mpvExtract" -y | Out-Null

# The archive may extract into a subdirectory - find where files actually are
$dllFile = Get-ChildItem $mpvExtract -Filter "libmpv-2.dll" -Recurse | Select-Object -First 1
if (-not $dllFile) {
    # List what was extracted for debugging
    Write-Host "Archive contents:"
    Get-ChildItem $mpvExtract -Recurse | ForEach-Object { Write-Host "  $($_.FullName)" }
    throw "libmpv-2.dll not found in extracted archive"
}
$mpvExtractRoot = $dllFile.DirectoryName

# Copy files
Write-Host "Installing to deps/..."
Copy-Item (Join-Path $mpvExtractRoot "libmpv-2.dll") $LibDir -Force

# Try to copy mpv.lib or generate from .dll.a
$mpvLib = Join-Path $mpvExtractRoot "mpv.lib"
$mpvDllA = Join-Path $mpvExtractRoot "libmpv.dll.a"
if (Test-Path $mpvLib) {
    Copy-Item $mpvLib $LibDir -Force
} elseif (Test-Path $mpvDllA) {
    Copy-Item $mpvDllA (Join-Path $LibDir "mpv.lib") -Force
} else {
    Write-Warning "No import library found. You may need to generate mpv.lib from libmpv-2.dll using lib.exe or dlltool."
}

# Copy headers - search recursively for mpv headers
$headerDir = Join-Path $mpvExtractRoot "include" "mpv"
if (-not (Test-Path $headerDir)) {
    # Try one level up
    $parentDir = Split-Path $mpvExtractRoot -Parent
    $headerDir = Join-Path $parentDir "include" "mpv"
}
if (Test-Path $headerDir) {
    Copy-Item (Join-Path $headerDir "*.h") $IncludeDir -Force
} else {
    # Headers might be anywhere in the extract
    $headers = Get-ChildItem $mpvExtract -Filter "*.h" -Recurse
    foreach ($h in $headers) {
        Copy-Item $h.FullName $IncludeDir -Force
    }
}

# --- Download WinSparkle (auto-update library) ---
Write-Host ""
Write-Host "Downloading WinSparkle (auto-update library)..."
$winsparkleUrl = "https://github.com/vslavik/winsparkle/releases/download/v0.8.1/WinSparkle-0.8.1.zip"
$winsparkleArchive = Join-Path $TempDir "winsparkle.zip"

try {
    curl.exe -L -o $winsparkleArchive $winsparkleUrl
    if ($LASTEXITCODE -ne 0) {
        throw "Failed to download WinSparkle (curl exit code: $LASTEXITCODE)"
    }

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
        # WinSparkle header goes to deps/include/ (not deps/include/mpv/)
        # because WINSPARKLE_INCLUDE_DIR is set to deps/include in CMakeLists.txt
        $winsparkleIncDir = Join-Path $DepsDir "include"
        Copy-Item $winsparkleHeader.FullName $winsparkleIncDir -Force
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
