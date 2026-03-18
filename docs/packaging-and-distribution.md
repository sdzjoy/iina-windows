# Packaging and Distribution Guide

This document covers the complete packaging and distribution workflow for IINA Windows, including installers, portable packages, and auto-updates.

## Table of Contents

1. [Overview](#overview)
2. [Prerequisites](#prerequisites)
3. [Building Packages](#building-packages)
4. [InnoSetup Installer](#innosetup-installer)
5. [Portable ZIP Package](#portable-zip-package)
6. [Auto-Update System (WinSparkle)](#auto-update-system-winsparkle)
7. [CI/CD Pipeline](#cicd-pipeline)
8. [Code Signing](#code-signing)
9. [Distribution Checklist](#distribution-checklist)

## Overview

IINA Windows supports three distribution formats:

1. **InnoSetup Installer** (.exe) - Full installation with system integration
2. **Portable ZIP** - Standalone package requiring no installation
3. **Auto-updates** - WinSparkle-powered automatic updates for installed versions

## Prerequisites

### Development Environment

- Windows 10 or later (64-bit)
- Visual Studio 2022 with C++ workload
- Qt 6.7+ (MSVC 2019 64-bit)
- CMake 3.21+
- PowerShell 5.1+

### Packaging Tools

- **InnoSetup 6.x**: Download from https://jrsoftware.org/isdl.php
  ```powershell
  choco install innosetup -y
  ```

- **7-Zip**: For extracting dependencies
  ```powershell
  choco install 7zip -y
  ```

### Dependencies

Run the dependency downloader to fetch libmpv and WinSparkle:

```powershell
.\scripts\download-deps.ps1
```

This downloads:
- libmpv (video playback engine)
- WinSparkle (auto-update library)

## Building Packages

### Step 1: Build the Application

```powershell
# Configure
cmake -B build -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_PREFIX_PATH="C:\Qt\6.7.0\msvc2019_64" `
  -DCMAKE_BUILD_TYPE=Release

# Build
cmake --build build --config Release --parallel

# Deploy Qt runtime
windeployqt --release --qmldir qml build\Release\iina-windows.exe

# Copy runtime DLLs
Copy-Item deps\lib\libmpv-2.dll build\Release\
Copy-Item deps\lib\WinSparkle.dll build\Release\
```

### Step 2: Create Installer

```powershell
# Run InnoSetup compiler
& "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" installer\iina-windows.iss

# Output: dist\IINA-Windows-0.1.0-Setup.exe
```

### Step 3: Create Portable Package

```powershell
.\scripts\package-portable.ps1 -BuildDir "build\Release" -Version "0.1.0"

# Output: dist\IINA-Windows-0.1.0-Portable.zip
```

## InnoSetup Installer

### Features

The InnoSetup installer (`installer/iina-windows.iss`) provides:

- **Installation Modes**: Per-user or per-machine (admin)
- **System Integration**:
  - Start Menu shortcuts
  - Desktop shortcut (optional)
  - Quick Launch shortcut (optional)
- **File Associations**: .mp4, .mkv, .avi, .mov, .wmv, .flv, .webm, .m4v, .mpg, .mpeg, .ts
- **Context Menu**: "Open with IINA" for video files
- **Uninstaller**: Complete removal of all installed components
- **License Display**: GPL-3.0 shown during installation

### Configuration

Edit `installer/iina-windows.iss` to customize:

```pascal
#define MyAppName "IINA"
#define MyAppVersion "0.1.0"  ; Update this for each release
#define MyAppPublisher "IINA Contributors"
#define MyAppURL "https://iina.io"
```

### Testing the Installer

1. Build the installer as shown above
2. Run on a clean VM or test machine
3. Verify:
   - Application launches correctly
   - File associations work (double-click a video file)
   - Context menu appears on video files
   - Start Menu shortcuts are created
   - Uninstaller removes all components

## Portable ZIP Package

### Features

The portable package (`scripts/package-portable.ps1`) creates:

- **Self-contained**: All dependencies included
- **No Installation**: Extract and run
- **Portable Mode Marker**: `portable.txt` file enables local config storage
- **Pre-configured**: Default settings.ini with portable mode enabled
- **Documentation**: README-PORTABLE.txt with usage instructions

### Directory Structure

```
IINA-Windows-0.1.0-Portable/
├── iina-windows.exe           ; Main executable
├── *.dll                      ; Qt, libmpv, WinSparkle
├── plugins/                   ; Qt plugins (platforms, etc.)
├── qml/                       ; QML runtime modules
├── translations/              ; Language files
├── config/                    ; Settings directory (pre-created)
│   └── settings.ini          ; Default configuration
├── portable.txt              ; Portable mode marker
├── README-PORTABLE.txt       ; Usage instructions
├── LICENSE.txt               ; GPL-3.0 license
└── THIRD_PARTY_LICENSES.txt  ; Third-party notices
```

### Portable Mode Behavior

When `portable.txt` exists, IINA:
- Stores settings in `config/settings.ini` (not registry)
- Stores history in `config/history.json`
- Creates logs in `logs/` subdirectory
- Does NOT write to `%APPDATA%` or registry

## Auto-Update System (WinSparkle)

### Architecture

IINA uses WinSparkle for Sparkle-compatible auto-updates on Windows.

**Components:**
- `src/utils/autoupdate.h/cpp`: Qt wrapper around WinSparkle API
- `installer/appcast.xml`: Update feed (Sparkle format)
- WinSparkle.dll: Third-party auto-update library

### Integration in Application

Initialize auto-updates in `main.cpp`:

```cpp
#include "utils/autoupdate.h"

int main(int argc, char *argv[]) {
    // ... QApplication setup ...

    // Initialize auto-update (only on Windows)
    if (AutoUpdate::isSupported()) {
        AutoUpdate::instance().initialize(
            "https://iina.io/appcast.xml",  // Appcast URL
            true                             // Enable automatic checks
        );
    }

    return app.exec();
}
```

### Manual Update Check

Add to UI (e.g., Help menu):

```cpp
void MainWindow::checkForUpdates() {
    if (AutoUpdate::isSupported()) {
        AutoUpdate::instance().checkForUpdates();
    } else {
        QMessageBox::information(this, "Updates",
            "Auto-update is only available on Windows.");
    }
}
```

### Appcast Feed

The appcast XML (`installer/appcast.xml`) lists available versions:

```xml
<item>
  <title>Version 0.2.0</title>
  <sparkle:releaseNotesLink>https://github.com/iina/iina-windows/releases/tag/v0.2.0</sparkle:releaseNotesLink>
  <pubDate>Wed, 18 Mar 2026 12:00:00 +0000</pubDate>
  <enclosure
    url="https://github.com/iina/iina-windows/releases/download/v0.2.0/IINA-Windows-0.2.0-Setup.exe"
    sparkle:version="0.2.0"
    sparkle:shortVersionString="0.2.0"
    length="52428800"
    type="application/octet-stream"
    sparkle:os="windows-x64"
  />
  <sparkle:minimumSystemVersion>10.0.0</sparkle:minimumSystemVersion>
</item>
```

### Hosting the Appcast

1. Generate appcast.xml with correct version info
2. Upload to a stable HTTPS URL (e.g., https://iina.io/appcast.xml)
3. Ensure the URL matches the one in `AutoUpdate::initialize()`
4. Update the file for each new release

### Testing Auto-Update

1. Build version 0.1.0 and install it
2. Create a fake 0.2.0 release in appcast.xml
3. Point appcast URL to local test server or GitHub
4. Run the app and trigger update check
5. Verify WinSparkle UI appears with update prompt

### Update Check Frequency

Default: 24 hours (86400 seconds)

Customize in code:
```cpp
AutoUpdate::instance().setCheckInterval(43200);  // 12 hours
```

Or in `autoupdate.cpp`:
```cpp
win_sparkle_set_update_check_interval(86400);
```

## CI/CD Pipeline

### GitHub Actions Workflow

The workflow (`.github/workflows/ci.yml`) automates:

1. **Matrix Builds**: Debug and Release configurations
2. **Dependency Download**: libmpv and WinSparkle
3. **Compilation**: CMake + MSVC
4. **Qt Deployment**: windeployqt for runtime libraries
5. **Installer Creation**: InnoSetup compilation
6. **Portable Packaging**: PowerShell script execution
7. **Artifact Upload**: Store build outputs
8. **GitHub Release**: Auto-create release on tag push

### Triggering a Release Build

Push a version tag:

```bash
git tag v0.2.0
git push origin v0.2.0
```

The CI pipeline will:
1. Build Debug and Release
2. Create installer and portable packages
3. Generate release notes
4. Create GitHub Release with artifacts
5. Output appcast.xml snippet for manual update

### Release Artifacts

After a successful release build:

- `IINA-Windows-{version}-Setup.exe` - Installer
- `IINA-Windows-{version}-Portable.zip` - Portable package
- Build artifacts for Debug and Release

## Code Signing

### Why Sign?

- **SmartScreen**: Avoid "Unknown Publisher" warnings
- **Trust**: Users can verify authenticity
- **Security**: Detect tampering

### Obtaining a Certificate

1. Purchase from a CA (DigiCert, Sectigo, etc.)
2. Cost: ~$100-400/year for standard code signing
3. EV (Extended Validation) avoids SmartScreen warnings immediately

### Signing Workflow

1. **Export certificate** to PFX file with private key
2. **Encode to Base64**:
   ```powershell
   $certBytes = [IO.File]::ReadAllBytes("cert.pfx")
   $certBase64 = [Convert]::ToBase64String($certBytes)
   echo $certBase64
   ```

3. **Add GitHub Secrets**:
   - `CODESIGN_CERT_BASE64`: Base64-encoded certificate
   - `CODESIGN_CERT_PASSWORD`: Certificate password

4. **Uncomment signing steps** in `.github/workflows/ci.yml`:
   ```yaml
   - name: Sign executable
     shell: pwsh
     run: |
       signtool sign /f cert.pfx /p "${{ secrets.CODESIGN_CERT_PASSWORD }}" `
         /t http://timestamp.digicert.com `
         /d "IINA for Windows" `
         build\Release\iina-windows.exe
   ```

### Files to Sign

- `build\Release\iina-windows.exe` - Main executable
- `dist\IINA-Windows-{version}-Setup.exe` - Installer

Do NOT sign DLLs (Qt, libmpv, WinSparkle) - they're third-party.

### Timestamping

Always timestamp signatures to ensure they remain valid after certificate expiration:

```powershell
signtool sign /f cert.pfx /p "password" `
  /t http://timestamp.digicert.com `  # Timestamp server
  /d "IINA for Windows" `
  /du "https://iina.io" `
  iina-windows.exe
```

## Distribution Checklist

### Pre-Release

- [ ] Update version in `CMakeLists.txt`
- [ ] Update version in `installer/iina-windows.iss`
- [ ] Update `CHANGELOG.md` with release notes
- [ ] Test build on clean Windows 10/11 VM
- [ ] Verify all features work in Release build
- [ ] Check file associations and context menu
- [ ] Test portable mode configuration
- [ ] Run installer and verify all components
- [ ] Test uninstaller removes all files

### Release

- [ ] Create and push version tag (`git tag v0.2.0`)
- [ ] Wait for CI/CD to complete
- [ ] Download and test installer from GitHub Release
- [ ] Download and test portable package
- [ ] Verify checksums in release notes

### Post-Release

- [ ] Update `installer/appcast.xml` with new version
- [ ] Deploy appcast.xml to https://iina.io/appcast.xml
- [ ] Test auto-update from previous version
- [ ] Announce release on website/social media
- [ ] Update documentation if needed

## Troubleshooting

### Installer Issues

**Problem**: "Setup was unable to create the directory"
**Solution**: Run installer as administrator or use per-user install mode

**Problem**: File associations not working
**Solution**: Reboot after installation, or manually set default app in Windows Settings

### Portable Package Issues

**Problem**: Settings not persisted between runs
**Solution**: Ensure `portable.txt` exists in the application directory

**Problem**: "VCRUNTIME140.dll not found"
**Solution**: Include VC++ redistributables or instruct users to install them

### Auto-Update Issues

**Problem**: Update check fails silently
**Solution**: Check logs for WinSparkle errors, verify appcast URL is HTTPS

**Problem**: "Update available" but nothing downloads
**Solution**: Verify `enclosure url` in appcast.xml is correct and accessible

**Problem**: Updates fail on Windows 7
**Solution**: WinSparkle requires Windows 10+, document this in system requirements

## Additional Resources

- InnoSetup Documentation: https://jrsoftware.org/ishelp/
- WinSparkle GitHub: https://github.com/vslavik/winsparkle
- Sparkle Appcast Spec: https://sparkle-project.org/documentation/
- Authenticode Signing: https://docs.microsoft.com/en-us/windows/win32/seccrypto/cryptography-tools

## Support

For questions or issues with packaging:
- Open an issue: https://github.com/iina/iina-windows/issues
- Discuss in Discussions: https://github.com/iina/iina-windows/discussions
