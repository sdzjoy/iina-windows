# IINA for Windows - Build Instructions

## Prerequisites

- **Windows 10/11** (x64)
- **Visual Studio 2022** (Community or higher) with "Desktop development with C++" workload
- **Qt 6.7+** (install via Qt Online Installer or `aqtinstall`)
  - Required modules: QtCore, QtGui, QtQuick, QtQuickControls2, QtOpenGL, QtWidgets, QtMultimedia, QtNetwork, QtShaderTools
- **CMake 3.21+** (included with Visual Studio)
- **7-Zip** (for extracting libmpv archive)
- **PowerShell 5.1+** (pre-installed on Windows 10/11)

## Quick Start

### 1. Clone the Repository

```powershell
git clone https://github.com/iina/iina-windows.git
cd iina-windows
```

### 2. Download libmpv Dependencies

```powershell
powershell -ExecutionPolicy Bypass -File scripts/download-deps.ps1
```

This downloads pre-built libmpv development files (DLL, import library, headers) from the mpv-player-windows project and places them in `deps/`.

After running, you should see:
```
deps/
  lib/
    libmpv-2.dll    (~40 MB)
    mpv.lib         (import library)
  include/
    mpv/
      client.h
      render.h
      render_gl.h
```

### 3. Configure with CMake

Using a Qt-configured command prompt (or set `CMAKE_PREFIX_PATH`):

```powershell
# Option A: Use Qt's environment
# Open "Qt 6.7.x MSVC 2019 64-bit" command prompt from Start Menu

# Option B: Set Qt path manually
$env:CMAKE_PREFIX_PATH = "C:\Qt\6.7.3\msvc2019_64"

cmake -B build -G "Visual Studio 17 2022" -A x64
```

### 4. Build

```powershell
cmake --build build --config Release --parallel
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
```

Or open a video file:
```powershell
.\iina-windows.exe "C:\Users\you\Videos\example.mp4"
```

## Build Configurations

| Config | Use Case |
|--------|----------|
| `Debug` | Development with debugger symbols |
| `Release` | Optimized build for distribution |
| `RelWithDebInfo` | Release with debug symbols for profiling |

## Troubleshooting

### "Cannot find Qt6" error
Ensure `CMAKE_PREFIX_PATH` points to your Qt installation:
```powershell
cmake -B build -DCMAKE_PREFIX_PATH="C:\Qt\6.7.3\msvc2019_64" -G "Visual Studio 17 2022" -A x64
```

### "Cannot find mpv.lib"
Run the dependency download script:
```powershell
powershell -ExecutionPolicy Bypass -File scripts/download-deps.ps1
```

If `mpv.lib` is still missing, you may need to generate it from the DLL:
```powershell
# In Developer Command Prompt for VS 2022
dumpbin /exports deps\lib\libmpv-2.dll > mpv.def
# Edit mpv.def to create a proper .def file
lib /def:mpv.def /out:deps\lib\mpv.lib /machine:x64
```

### OpenGL errors at runtime
Ensure your GPU drivers are up to date. mpv requires OpenGL 3.3+ or Direct3D 11.

### Video doesn't render
Check that `libmpv-2.dll` is in the same directory as `iina-windows.exe`.

## Development with Visual Studio

After CMake configuration:
```powershell
cmake --open build
```
This opens the project in Visual Studio. Set `iina-windows` as the startup project.

## Development with Qt Creator

Open `CMakeLists.txt` directly in Qt Creator. Configure the kit to use MSVC 2019/2022 (64-bit).

## Project Structure

```
iina-windows/
  CMakeLists.txt          # Main build file
  src/
    main.cpp              # Entry point
    application.cpp/h     # Application singleton
    core/
      mpvengine.cpp/h     # libmpv C API wrapper
      mpvrenderer.cpp/h   # OpenGL rendering bridge
      playercore.cpp/h    # High-level player controller
    models/
      playlistmodel.cpp/h # Playlist data model
      trackmodel.cpp/h    # Audio/subtitle track model
    utils/
      logger.cpp/h        # File logging
      config.cpp/h        # Settings (INI-based)
  qml/
    Main.qml              # Main window
    components/
      VideoArea.qml       # Video rendering area
      ControlBar.qml      # Playback controls
      SeekBar.qml         # Seek/progress bar
      VolumeControl.qml   # Volume slider
      PlaylistPanel.qml   # Playlist sidebar
      SubtitleMenu.qml    # Subtitle selection
      TitleBar.qml        # Custom title bar
      OSD.qml             # On-screen display
    pages/
      SettingsPage.qml    # Settings dialog
  deps/
    include/mpv/          # mpv C API headers
    lib/                  # libmpv-2.dll + mpv.lib
  resources/icons/        # SVG icons
  .github/workflows/     # CI configuration
  scripts/               # Build scripts
```
