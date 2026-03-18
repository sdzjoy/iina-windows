# generate-appcast-entry.ps1 - Generate appcast.xml entry for a release
# Usage: .\scripts\generate-appcast-entry.ps1 -Version "0.2.0" -InstallerPath "dist\IINA-Windows-0.2.0-Setup.exe"

param(
    [Parameter(Mandatory=$true)]
    [string]$Version,

    [Parameter(Mandatory=$true)]
    [string]$InstallerPath,

    [Parameter(Mandatory=$false)]
    [string]$ReleaseNotesUrl = "",

    [Parameter(Mandatory=$false)]
    [string]$MinWindowsVersion = "10.0.0"
)

$ErrorActionPreference = "Stop"

Write-Host "IINA Windows - Appcast Entry Generator" -ForegroundColor Cyan
Write-Host "=======================================" -ForegroundColor Cyan
Write-Host ""

# Validate installer exists
if (-not (Test-Path $InstallerPath)) {
    Write-Error "Installer not found: $InstallerPath"
    exit 1
}

# Get file info
$installerFile = Get-Item $InstallerPath
$fileSize = $installerFile.Length
$fileName = $installerFile.Name

# Generate release notes URL if not provided
if ([string]::IsNullOrEmpty($ReleaseNotesUrl)) {
    $ReleaseNotesUrl = "https://github.com/iina/iina-windows/releases/tag/v$Version"
}

# Generate publication date (RFC 822 format)
$pubDate = Get-Date -Format "ddd, dd MMM yyyy HH:mm:ss +0000" -AsUTC

# Generate download URL (assumes GitHub Releases)
$downloadUrl = "https://github.com/iina/iina-windows/releases/download/v$Version/$fileName"

Write-Host "Installer Information:" -ForegroundColor Green
Write-Host "  Version:         $Version"
Write-Host "  File:            $fileName"
Write-Host "  Size:            $fileSize bytes ($([math]::Round($fileSize / 1MB, 2)) MB)"
Write-Host "  Publication:     $pubDate"
Write-Host "  Release Notes:   $ReleaseNotesUrl"
Write-Host "  Download URL:    $downloadUrl"
Write-Host ""

# Calculate SHA256 checksum
Write-Host "Calculating SHA256 checksum..."
$sha256 = (Get-FileHash $InstallerPath -Algorithm SHA256).Hash
Write-Host "  SHA256: $sha256" -ForegroundColor Yellow
Write-Host ""

# Generate appcast XML entry
$appcastEntry = @"
    <item>
      <title>Version $Version</title>
      <sparkle:releaseNotesLink>$ReleaseNotesUrl</sparkle:releaseNotesLink>
      <pubDate>$pubDate</pubDate>
      <enclosure
        url="$downloadUrl"
        sparkle:version="$Version"
        sparkle:shortVersionString="$Version"
        length="$fileSize"
        type="application/octet-stream"
        sparkle:os="windows-x64"
      />
      <sparkle:minimumSystemVersion>$MinWindowsVersion</sparkle:minimumSystemVersion>
    </item>
"@

Write-Host "Generated Appcast Entry:" -ForegroundColor Green
Write-Host "=========================" -ForegroundColor Green
Write-Host $appcastEntry
Write-Host ""

# Save to file
$outputPath = "appcast-entry-v$Version.xml"
$appcastEntry | Out-File -FilePath $outputPath -Encoding UTF8 -NoNewline

Write-Host "Saved to: $outputPath" -ForegroundColor Cyan
Write-Host ""

# Generate full release notes snippet
$releaseNotesSnippet = @"
## IINA for Windows $Version

**Downloads:**
- [Installer (Recommended)]($downloadUrl) - $([math]::Round($fileSize / 1MB, 2)) MB
- [Portable Package](https://github.com/iina/iina-windows/releases/download/v$Version/IINA-Windows-$Version-Portable.zip)

**Checksum (SHA256):**
``````
$sha256
``````

**Installation:**
1. Download the installer
2. Run IINA-Windows-$Version-Setup.exe
3. Follow the installation wizard
4. Launch from Start Menu

**System Requirements:**
- Windows $MinWindowsVersion or later (64-bit)

---
"@

Write-Host "GitHub Release Notes Snippet:" -ForegroundColor Green
Write-Host "=============================" -ForegroundColor Green
Write-Host $releaseNotesSnippet
Write-Host ""

$releaseNotesPath = "release-notes-v$Version.md"
$releaseNotesSnippet | Out-File -FilePath $releaseNotesPath -Encoding UTF8

Write-Host "Saved release notes to: $releaseNotesPath" -ForegroundColor Cyan
Write-Host ""

Write-Host "Next Steps:" -ForegroundColor Yellow
Write-Host "1. Add the appcast entry to the TOP of installer/appcast.xml (newest first)"
Write-Host "2. Upload installer to GitHub Releases"
Write-Host "3. Deploy updated appcast.xml to https://iina.io/appcast.xml"
Write-Host "4. Test auto-update from previous version"
Write-Host ""

# Offer to open appcast.xml for editing
$openAppcast = Read-Host "Open installer/appcast.xml for editing? (y/n)"
if ($openAppcast -eq 'y') {
    $appcastPath = Join-Path (Split-Path -Parent $PSScriptRoot) "installer" "appcast.xml"
    if (Test-Path $appcastPath) {
        Start-Process notepad.exe $appcastPath
    } else {
        Write-Warning "appcast.xml not found at $appcastPath"
    }
}
