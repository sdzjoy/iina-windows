# Building IINA for Windows

Complete guide for building IINA Windows from source.

## Requirements

| Requirement | Version | Notes |
|------------|---------|-------|
| Windows | 10/11 x64 | Build host |
| Visual Studio 2022 | 17.x | "Desktop development with C++" workload |
| Qt | 6.7+ | Qt Online Installer or `aqtinstall` |
| CMake | 3.21+ | Included with Visual Studio |
| 7-Zip | Any | Extracting libmpv archive |
| PowerShell | 5.1+ | Pre-installed on Windows 10/11 |

### Required Qt Modules

Install these via Qt Online Installer:

- Qt Core, Qt GUI, Qt Quick, Qt Quick Controls 2
- Qt OpenGL, Qt Widgets, Qt Multimedia, Qt Network
- Qt Shader Tools (required for QML rendering)
- Qt Linguist Tools (required for translations)

## Build Steps

### 1. Clone

```powershell
git clone https://github.com/iina/iina-windows.git
cd iina-windows
```

### 2. Download libmpv

```powershell
powershell -ExecutionPolicy Bypass -File scripts/download-deps.ps1
```

This downloads pre-built libmpv (DLL + import lib + headers) from the mpv-player-windows project. After running:

```
deps/
  lib/
    libmpv-2.dll    (~40 MB)
    mpv.lib         (MSVC import library)
  include/
    mpv/
      client.h
      render.h
      render_gl.h
```

### 3. Configure

```powershell
# Set Qt path (adjust for your Qt version)
$env:CMAKE_PREFIX_PATH = "C:\Qt\6.7.3\msvc2019_64"

cmake -B build -G "Visual Studio 17 2022" -A x64
```

### 4. Build

```powershell
# Release build
cmake --build build --config Release --parallel

# Debug build (for development)
cmake --build build --config Debug --parallel
```

### 5. Deploy Qt Runtime

```powershell
cd build\Release
windeployqt --release --qmldir ..\..\qml iina-windows.exe
copy ..\..\deps\lib\libmpv-2.dll .
```

### 6. Run

```powershell
.\iina-windows.exe
# Or open a file directly:
.\iina-windows.exe "C:\Users\you\Videos\movie.mp4"
```

## Build Configurations

| Config | Flags | Use Case |
|--------|-------|----------|
| Debug | `/Od /Zi` | Development, debugger symbols |
| Release | `/O2 /DNDEBUG` | Distribution, optimized |
| RelWithDebInfo | `/O2 /Zi` | Profiling, crash analysis |

## IDE Setup

### Visual Studio 2022

```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.7.3\msvc2019_64"
cmake --open build
```

Set `iina-windows` as startup project. Add `PATH=<repo>\deps\lib;%PATH%` to Debug > Environment.

### Qt Creator

Open `CMakeLists.txt` directly. Select MSVC 2019/2022 64-bit kit. Qt Creator auto-detects Qt and CMake paths.

### VS Code

Install CMake Tools extension. Configure:

```json
{
    "cmake.configureArgs": [
        "-DCMAKE_PREFIX_PATH=C:\\Qt\\6.7.3\\msvc2019_64"
    ],
    "cmake.generator": "Visual Studio 17 2022"
}
```

## Creating Release Packages

### Portable ZIP

```powershell
powershell -ExecutionPolicy Bypass -File scripts/package-portable.ps1 -BuildDir build\Release
```

Output: `dist/IINA-Windows-0.1.0-portable.zip`

### Installer (InnoSetup)

Requires [InnoSetup 6+](https://jrsoftware.org/isinfo.php):

```powershell
# Build first, then deploy Qt, then run InnoSetup
cmake --build build --config Release --parallel
cd build\Release && windeployqt --release --qmldir ..\..\qml iina-windows.exe && cd ..\..
iscc installer/iina-windows.iss
```

Output: `dist/IINA-Windows-0.1.0-setup.exe`

## Dependency Details

### libmpv

The project uses pre-built libmpv from [shinchiro/mpv-winbuild-cmake](https://github.com/shinchiro/mpv-winbuild-cmake). libmpv statically bundles:

- FFmpeg (libavcodec, libavformat, libavutil, libswscale, libswresample)
- libass (subtitle rendering)
- libplacebo (GPU video processing)
- dav1d (AV1 decoder)
- HarfBuzz, FreeType, FriBidi (text rendering)
- Various codec libraries (opus, vorbis, etc.)

All dependencies are statically linked into `libmpv-2.dll`.

### Qt 6

Qt is dynamically linked. `windeployqt` collects all required Qt DLLs and QML modules into the build directory. Required at runtime:

- Qt6Core.dll, Qt6Gui.dll, Qt6Quick.dll, Qt6QuickControls2.dll
- Qt6OpenGL.dll, Qt6Widgets.dll, Qt6Network.dll
- Qt6Qml.dll, Qt6QmlModels.dll
- QML plugins (QtQuick, Material, etc.)
- Platform plugin (qwindows.dll)

## Troubleshooting

### "Cannot find Qt6" during CMake configure

Ensure `CMAKE_PREFIX_PATH` points to your Qt MSVC directory:

```powershell
cmake -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.7.3\msvc2019_64" -G "Visual Studio 17 2022" -A x64
```

### "Cannot find mpv.lib" during link

Run `scripts/download-deps.ps1`. If `mpv.lib` is missing from the archive, generate it:

```powershell
# In Developer Command Prompt
dumpbin /exports deps\lib\libmpv-2.dll > exports.txt
# Create mpv.def from exports
lib /def:mpv.def /out:deps\lib\mpv.lib /machine:x64
```

### OpenGL errors at runtime

- Update GPU drivers
- mpv requires OpenGL 3.3+ or Direct3D 11
- On VMs, try setting `MPV_VO=gpu-next` environment variable

### Video does not render (black screen)

- Verify `libmpv-2.dll` is in the same directory as `iina-windows.exe`
- Check `%APPDATA%/IINA/iina.log` for errors
- Try `--hwdec=no` to disable hardware decoding

### Build fails with LNK2019 (unresolved external)

Ensure MSVC 2022 C++ workload is installed. Check that you're building for x64 (not x86 or ARM).

## Cross-Compilation (Not Supported)

This project does not support cross-compilation from Linux/macOS. It must be built on a Windows host with MSVC.
