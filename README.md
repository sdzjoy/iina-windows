# IINA for Windows

A Windows port of the [IINA](https://github.com/iina/iina) media player, built with **Qt 6 (QML/C++)** and **libmpv**.

[中文文档](docs/README.zh-CN.md)

## Downloads

| Package | Description |
|---------|-------------|
| **IINA-Windows-0.1.0-Setup.exe** | Installer (recommended) - file associations, Start Menu |
| **IINA-Windows-0.1.0-Portable.zip** | Portable edition - no installation required |

**Requirements:** Windows 10 or later (64-bit). No additional dependencies.

## Features

### Playback
- Local file playback (file dialog, drag-and-drop, command line)
- URL playback (network streams, YouTube via yt-dlp if installed)
- Play / Pause / Stop / Seek / Frame stepping
- Volume control with mouse wheel (0-150%)
- Fullscreen (F key, double-click, button)
- Playback speed control (0.25x - 4.0x)
- Chapter navigation (panel, menu, keyboard)
- Playback history with resume position
- Recent files menu
- Screenshot capture (PNG, JPG, WebP)
- Video filters (brightness, contrast, saturation, gamma, hue)
- Hardware decoding (auto-safe, d3d11va, dxva2)
- Drag-and-drop (media files + subtitle files)

### Subtitles
- External subtitle loading (dialog, drag-and-drop)
- Auto-matching (scans directory for matching subtitle files)
- Track selection and cycling
- Subtitle styling (font, color, border, position, delay, scale)
- Online subtitle search (OpenSubtitles REST API)

### Interface
- Material Dark theme
- Right-click context menu
- Side panels: Playlist, Chapters, History
- OSD feedback (volume, speed, screenshot, subtitle delay)
- Menu bar (File, Playback, Video, Audio, Subtitle, Window, Help)
- Settings dialog (General, Video, Audio, Subtitle)
- Configurable keyboard shortcuts with GUI editor
- Simplified Chinese (zh_CN) localization

### Keyboard Shortcuts

All shortcuts are configurable via Settings > Configure Shortcuts.

| Key | Action |
|-----|--------|
| Space | Play / Pause |
| Left / Right | Seek -5s / +5s |
| Ctrl+Left / Ctrl+Right | Seek -30s / +30s |
| Shift+Left / Shift+Right | Seek -60s / +60s |
| Up / Down | Volume +5 / -5 |
| F | Toggle fullscreen |
| Escape | Exit fullscreen |
| M | Toggle mute |
| P | Toggle playlist |
| [ / ] | Speed -0.25x / +0.25x |
| Backspace | Reset speed to 1.0x |
| . / , | Frame step / Frame back-step |
| Ctrl+O | Open file |
| Ctrl+Shift+O | Open URL |
| Ctrl+S | Screenshot |
| Ctrl+Q | Quit |
| S | Cycle subtitle track |
| Z / Shift+Z | Subtitle delay +0.5s / -0.5s |
| Shift+= / Shift+- | Subtitle scale up / down |
| Ctrl+, | Settings |
| Page Up / Page Down | Next / Previous chapter |

## Build

See [BUILDING.md](BUILDING.md) for full instructions.

### Quick Start

```powershell
# 1. Install prerequisites
#    - Visual Studio 2022 with C++ workload
#    - Qt 6.7+ (via Qt Online Installer)
#    - CMake 3.21+

# 2. Download libmpv
.\scripts\download-deps.ps1

# 3. Configure
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:\Qt\6.7.3\msvc2019_64"

# 4. Build
cmake --build build --config Release --parallel

# 5. Deploy Qt runtime
cd build\Release
windeployqt --release --qmldir ..\..\qml iina-windows.exe

# 6. Run
.\iina-windows.exe
```

### Creating Release Packages

```powershell
# Installer (requires InnoSetup 6+)
iscc installer/iina-windows.iss

# Portable ZIP
.\scripts\package-portable.ps1 -BuildDir build\Release -Version 0.1.0
```

## Project Structure

```
iina-windows/
  CMakeLists.txt              # Build configuration
  BUILDING.md                 # Build instructions
  CHANGELOG.md                # Version history
  KNOWN_GAPS.md               # Missing features / roadmap
  src/
    main.cpp                  # Entry point, translation loading
    application.h/cpp         # Application singleton
    core/
      mpvengine.h/cpp         # libmpv C API wrapper (46 Q_PROPERTYs)
      mpvrenderer.h/cpp       # OpenGL rendering into QML
      playercore.h/cpp        # High-level player controller
      shortcutmanager.h/cpp   # Configurable keyboard shortcuts
      subtitlesearch.h/cpp    # OpenSubtitles API client
    models/
      playlistmodel.h/cpp     # Playlist data model
      trackmodel.h/cpp        # Audio/subtitle track model
      chaptermodel.h/cpp      # Chapter navigation model
      historymodel.h/cpp      # Playback history (JSON persistence)
    utils/
      config.h/cpp            # Settings (QSettings INI)
      logger.h/cpp            # File logging
      autoupdate.h/cpp        # WinSparkle auto-update (prepared)
  qml/
    Main.qml                  # Main window (menus, shortcuts, dialogs)
    components/               # 11 UI components
    pages/                    # Settings, Shortcut editor
  translations/
    iina_zh_CN.ts             # Simplified Chinese (172 strings)
  installer/
    iina-windows.iss          # InnoSetup installer script
    appcast.xml               # WinSparkle update feed template
  scripts/
    download-deps.ps1         # Download libmpv
    package-portable.ps1      # Build portable ZIP
  resources/icons/            # 17 SVG icons
  .github/workflows/
    ci.yml                    # CI build on push/PR
    release.yml               # Release build on tag push
  docs/                       # Project documentation
  LICENSE                     # GPL-3.0
  THIRD_PARTY_LICENSES.txt    # Third-party notices
```

## Architecture

```
                    +------------------+
                    |    QML UI        |
                    | (Main.qml +     |
                    |  14 components)  |
                    +--------+---------+
                             |
                    +--------+---------+
                    |   PlayerCore     |
                    | (orchestrator)   |
                    +--------+---------+
                             |
         +----------+--------+---------+-----------+
         |          |                  |            |
  +------+------+  +--------+  +------+------+ +---+---+
  |  MpvEngine  |  |MpvObject|  |ShortcutMgr | |History|
  | (libmpv API)|  |(OpenGL  |  |(JSON keys) | |(JSON) |
  |  46 props   |  | render) |  |            | |       |
  +------+------+  +----+----+  +------------+ +-------+
         |              |
         +------+-------+
                |
       +--------+--------+
       |     libmpv      |
       | (mpv-2.dll)     |
       +-----------------+
```

## Feature Parity

59% overall parity with macOS IINA (124/212 features), 69% for core features.
See [docs/mac-vs-windows-final-matrix.md](docs/mac-vs-windows-final-matrix.md) for the full comparison.

## License

GPL-3.0 - See [LICENSE](LICENSE)

Based on [IINA](https://github.com/iina/iina) by lhc70000 and contributors.

## Documentation

### User Documentation
- [Chinese README / 中文文档](docs/README.zh-CN.md)
- [Chinese Release Notes / 发行说明](docs/release-notes.zh-CN.md)

### Developer Documentation
- [Build Instructions](BUILDING.md)
- [Changelog](CHANGELOG.md)
- [Known Gaps & Roadmap](KNOWN_GAPS.md)
- [Feature Parity Matrix](docs/mac-vs-windows-final-matrix.md)
- [UI Design Decisions](docs/ui-decisions.md)
- [Deferred Features](docs/deferred-features.md)
- [Maintenance Handover](docs/maintenance-handover.md)

### QA & Localization
- [Regression Test Report](docs/regression-test-report.md)
- [Release Checklist](docs/final-release-checklist.md)
- [Localization Coverage](docs/localization-coverage.md)
- [Localization QA](docs/l10n-qa-report.md)
- [Chinese Glossary](docs/glossary.zh-CN.md)

## Contributing

1. Fork the repository
2. Create a feature branch: `git checkout -b feature/my-feature`
3. Build and test locally (see [BUILDING.md](BUILDING.md))
4. Submit a pull request

For bug reports and feature requests, please use [GitHub Issues](https://github.com/iina/iina-windows/issues).
