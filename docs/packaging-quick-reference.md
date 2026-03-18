# Packaging Quick Reference

Quick commands for common packaging tasks.

## Build and Package (Complete Workflow)

```powershell
# 1. Download dependencies
.\scripts\download-deps.ps1

# 2. Configure and build
cmake -B build -G "Visual Studio 17 2022" -A x64 `
  -DCMAKE_PREFIX_PATH="C:\Qt\6.7.0\msvc2019_64" `
  -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel

# 3. Deploy Qt runtime
windeployqt --release --qmldir qml build\Release\iina-windows.exe
Copy-Item deps\lib\libmpv-2.dll build\Release\
Copy-Item deps\lib\WinSparkle.dll build\Release\

# 4. Create installer
& "C:\Program Files (x86)\Inno Setup 6\ISCC.exe" installer\iina-windows.iss

# 5. Create portable package
.\scripts\package-portable.ps1 -BuildDir "build\Release" -Version "0.1.0"
```

## Create Release

```powershell
# Tag and push
git tag v0.2.0
git push origin v0.2.0

# CI/CD will automatically:
# - Build Debug and Release
# - Create installer and portable packages
# - Create GitHub Release
# - Generate appcast entry snippet
```

## Update Appcast

```powershell
# Generate appcast entry
.\scripts\generate-appcast-entry.ps1 `
  -Version "0.2.0" `
  -InstallerPath "dist\IINA-Windows-0.2.0-Setup.exe"

# Copy output to installer/appcast.xml (add at TOP)
# Deploy appcast.xml to https://iina.io/appcast.xml
```

## Test Packages

```powershell
# Test installer
Start-Process "dist\IINA-Windows-0.1.0-Setup.exe"

# Test portable
Expand-Archive "dist\IINA-Windows-0.1.0-Portable.zip" -DestinationPath "test-portable"
Start-Process "test-portable\IINA-Windows-0.1.0-Portable\iina-windows.exe"

# Test auto-update (requires previous version installed)
# Run app → Help → Check for Updates
```

## Code Signing

```powershell
# Sign executable
signtool sign /f cert.pfx /p "password" `
  /t http://timestamp.digicert.com `
  /d "IINA for Windows" `
  build\Release\iina-windows.exe

# Sign installer
signtool sign /f cert.pfx /p "password" `
  /t http://timestamp.digicert.com `
  /d "IINA for Windows Installer" `
  dist\IINA-Windows-0.1.0-Setup.exe
```

## File Locations

| Item | Path |
|------|------|
| InnoSetup script | `installer/iina-windows.iss` |
| Appcast feed | `installer/appcast.xml` |
| Portable script | `scripts/package-portable.ps1` |
| Dependency downloader | `scripts/download-deps.ps1` |
| Appcast generator | `scripts/generate-appcast-entry.ps1` |
| CI/CD workflow | `.github/workflows/ci.yml` |
| Auto-update code | `src/utils/autoupdate.h/cpp` |
| Build output | `build/Release/` |
| Package output | `dist/` |

## Version Bump Checklist

When releasing a new version:

1. [ ] Update `CMakeLists.txt` → `VERSION 0.2.0`
2. [ ] Update `installer/iina-windows.iss` → `#define MyAppVersion "0.2.0"`
3. [ ] Update `README.md` if version is mentioned
4. [ ] Update `CHANGELOG.md` with release notes
5. [ ] Commit: `git commit -m "Bump version to 0.2.0"`
6. [ ] Tag: `git tag v0.2.0`
7. [ ] Push: `git push origin main --tags`

## CI/CD Environment Variables

Set these in your GitHub Actions workflow:

```yaml
env:
  QT_VERSION: '6.7.*'
  PROJECT_VERSION: '0.1.0'  # Auto-extracted from git tag
```

## Troubleshooting

**InnoSetup not found:**
```powershell
choco install innosetup -y
```

**WinSparkle.dll missing:**
```powershell
.\scripts\download-deps.ps1  # Re-run dependency downloader
```

**Qt runtime missing:**
```powershell
windeployqt --release --qmldir qml build\Release\iina-windows.exe
```

**Version mismatch:**
Ensure version is consistent across:
- CMakeLists.txt
- installer/iina-windows.iss
- Git tag (v0.x.x)

## Resources

- Full documentation: `docs/packaging-and-distribution.md`
- InnoSetup docs: https://jrsoftware.org/ishelp/
- WinSparkle: https://github.com/vslavik/winsparkle
