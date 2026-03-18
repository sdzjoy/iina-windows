# Project Maintenance Handover - IINA for Windows

Guide for continuing development and maintenance of IINA Windows.

---

## 1. Project Overview

| Property | Value |
|----------|-------|
| Name | IINA for Windows |
| Version | 0.1.0 (Public Beta) |
| Language | C++17, QML |
| Frameworks | Qt 6.7+, libmpv |
| Build System | CMake 3.21+ |
| CI/CD | GitHub Actions |
| License | GPL-3.0 |
| Platform | Windows 10/11 x64 |

## 2. Repository Structure

```
iina-windows/
  CMakeLists.txt              # Main build configuration
  BUILDING.md                 # Build instructions
  CHANGELOG.md                # Version history
  KNOWN_GAPS.md               # Missing features and roadmap
  LICENSE                     # GPL-3.0
  README.md                   # English documentation
  THIRD_PARTY_LICENSES.txt    # Third-party notices
  .github/workflows/
    ci.yml                    # CI build on push/PR
    release.yml               # Release builds on tag push
  installer/
    iina-windows.iss          # InnoSetup installer script
    appcast.xml               # WinSparkle update feed
  scripts/
    download-deps.ps1         # Download libmpv
    package-portable.ps1      # Build portable ZIP
  src/
    main.cpp                  # Entry point
    application.h/cpp         # App orchestrator
    core/
      mpvengine.h/cpp         # libmpv wrapper (720 lines - largest file)
      mpvrenderer.h/cpp       # OpenGL rendering bridge
      playercore.h/cpp        # High-level player controller
      shortcutmanager.h/cpp   # Keyboard shortcut system
      subtitlesearch.h/cpp    # OpenSubtitles API
    models/
      playlistmodel.h/cpp     # Playlist model
      chaptermodel.h/cpp      # Chapter model
      historymodel.h/cpp      # History model (JSON persistence)
      trackmodel.h/cpp        # Track model (audio/video/subtitle)
    utils/
      config.h/cpp            # QSettings wrapper
      logger.h/cpp            # File logging
      autoupdate.h/cpp        # WinSparkle wrapper (prepared, not active)
  qml/
    Main.qml                  # Main window (463 lines - largest QML file)
    components/               # UI components (11 files)
    pages/                    # Settings pages (2 files)
  resources/icons/            # 17 SVG icons
  translations/
    iina_zh_CN.ts             # Chinese translation (172 strings)
  docs/                       # Project documentation
```

## 3. Architecture

```
+-------------------+     +-------------------+
|     QML UI        |     |   Settings/Config |
| (Main.qml +      |<--->|   (QSettings INI) |
|  14 components)   |     +-------------------+
+--------+----------+
         |  Q_PROPERTY bindings
+--------+----------+
|   PlayerCore      |     +-------------------+
|  (orchestrator)   |<--->|  ShortcutManager  |
+--------+----------+     |  (JSON keybinds)  |
         |                +-------------------+
+--------+----------+
|    MpvEngine      |     +-------------------+
|  (libmpv C API)   |<--->|  HistoryModel     |
|  46 Q_PROPERTYs   |     |  (JSON file)      |
+--------+----------+     +-------------------+
         |
+--------+----------+
|    MpvRenderer    |
| (QQuickFBO +      |
|  OpenGL 3.3)      |
+--------+----------+
         |
+--------+----------+
|     libmpv        |
| (libmpv-2.dll)    |
+-------------------+
```

### Data Flow
1. User interacts with QML UI
2. QML property bindings call C++ methods on PlayerCore/MpvEngine
3. MpvEngine sends mpv commands via `mpv_command_async()`
4. mpv processes command, fires events
5. MpvEngine event loop updates Q_PROPERTYs
6. QML UI reacts to property changes via bindings

### Key Design Decisions
- **No QtWidgets**: Entire UI is QML + Qt Quick Controls 2
- **Material Dark**: Theme set globally via `QQuickStyle::setStyle("Material")`
- **mpv C API only**: No mpv Lua/JS scripting; all control via `client.h`
- **JSON persistence**: History and shortcuts stored as JSON files (not database)
- **Translation**: Qt Linguist (.ts -> .qm), embedded as Qt resources

## 4. Key Files to Know

### When Adding a New Feature
1. **`src/core/mpvengine.h/cpp`** - Add mpv property/command wrappers here
2. **`src/core/playercore.h/cpp`** - Add high-level player logic here
3. **`qml/Main.qml`** - Add menus, keyboard shortcuts, dialogs here
4. **`qml/components/ControlBar.qml`** - Add control bar buttons here
5. **`CMakeLists.txt`** - Add new source files and QML files here

### When Adding a Translation
1. Add `qsTr()` / `tr()` to new strings
2. Edit `translations/iina_zh_CN.ts` - add new `<message>` entries
3. Run `lupdate` (or rebuild) to compile .qm

### When Fixing a Bug
- Check `%APPDATA%/IINA/iina.log` for runtime errors
- `src/core/mpvengine.cpp` handles most playback-related issues
- `qml/Main.qml` handles most UI-related issues

## 5. Build & Release Process

### Daily Development
```powershell
cmake -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_PREFIX_PATH="C:\Qt\6.7.3\msvc2019_64"
cmake --build build --config Debug --parallel
```

### Creating a Release
```powershell
# 1. Update version in CMakeLists.txt (project VERSION)
# 2. Update CHANGELOG.md
# 3. Commit and tag
git tag v0.2.0
git push origin v0.2.0
# 4. GitHub Actions runs release.yml automatically
# 5. Review draft release on GitHub, publish when ready
```

### Manual Release (No CI)
```powershell
cmake --build build --config Release --parallel
cd build\Release
windeployqt --release --qmldir ..\..\qml iina-windows.exe
copy ..\..\deps\lib\libmpv-2.dll .
cd ..\..
# Installer
iscc installer/iina-windows.iss
# Portable
.\scripts\package-portable.ps1 -BuildDir build\Release -Version 0.2.0
```

## 6. Dependency Updates

### Updating Qt
1. Install new Qt version via Qt Online Installer
2. Update `CMAKE_PREFIX_PATH` to new Qt directory
3. Update `QT_VERSION` in `.github/workflows/release.yml`
4. Update `version` in `.github/workflows/ci.yml`
5. Test: rebuild, run windeployqt, verify

### Updating libmpv
1. `scripts/download-deps.ps1` downloads latest from SourceForge
2. Check `THIRD_PARTY_LICENSES.txt` for dependency changes
3. Test: rebuild, verify playback with various formats

### Updating InnoSetup Script
- Edit `installer/iina-windows.iss`
- Update version define: `#define MyAppVersion "0.2.0"`
- Test: `iscc installer/iina-windows.iss`

## 7. Common Tasks

### Add a New Keyboard Shortcut
1. `src/core/shortcutmanager.cpp`: Add `add("action_id", tr("Label"), tr("Category"), "DefaultKey");`
2. `qml/Main.qml`: Add `Shortcut { sequence: shortcuts.binding("action_id"); onActivated: { ... } }`
3. `translations/iina_zh_CN.ts`: Add `<message>` entries for label and category

### Add a New Settings Option
1. `src/utils/config.cpp`: Add getter/setter using `QSettings`
2. `qml/pages/SettingsPage.qml`: Add UI control in appropriate tab
3. Connect via property binding or signal/slot

### Add a New Side Panel
1. Create `qml/components/NewPanel.qml`
2. Add to `CMakeLists.txt` QML_FILES
3. Add toggle in `qml/Main.qml` and `qml/components/ControlBar.qml`
4. Create model in `src/models/` if data-driven

### Add a New Translation Language
1. Copy `translations/iina_zh_CN.ts` to `translations/iina_xx_XX.ts`
2. Change `language` attribute in `<TS>` tag
3. Translate all `<translation>` elements
4. Add to `TS_FILES` in `CMakeLists.txt`
5. Rebuild to compile .qm

## 8. Testing Checklist

Before each release:

- [ ] All video formats play (H.264, H.265, AV1, VP9, MPEG-4)
- [ ] External subtitles load (SRT, ASS, VTT)
- [ ] Auto-subtitle matching works
- [ ] All keyboard shortcuts function
- [ ] Settings persist across restart
- [ ] Playback history and resume work
- [ ] Installer installs/uninstalls cleanly
- [ ] Portable edition runs without install
- [ ] Chinese UI displays correctly
- [ ] DPI scaling works at 100%, 150%, 200%
- [ ] No crashes on file open, close, seek

## 9. Known Technical Debt

| Item | Description | Priority |
|------|-------------|----------|
| MpvEngine size | 720 lines, could be split into MpvPlayback + MpvTracks + MpvFilters | Low |
| Main.qml size | 463 lines, could extract MenuBar and Dialogs to separate files | Low |
| Portable mode | Config check in `config.cpp` needs portable.txt detection | Medium |
| WinSparkle integration | `autoupdate.h/cpp` written but not wired into main.cpp | Medium |
| Error handling | mpv errors logged but not shown to user | Medium |
| Test suite | No automated tests (unit or integration) | High |

## 10. External Resources

| Resource | URL |
|----------|-----|
| Qt Documentation | https://doc.qt.io/qt-6/ |
| mpv Manual | https://mpv.io/manual/master/ |
| mpv Client API | https://github.com/mpv-player/mpv/blob/master/libmpv/client.h |
| InnoSetup Docs | https://jrsoftware.org/ishelp/ |
| WinSparkle | https://winsparkle.org/ |
| macOS IINA Source | https://github.com/iina/iina |
| libmpv Builds | https://sourceforge.net/projects/mpv-player-windows/ |

## 11. Contact & Continuity

This project was developed as a Windows port of the macOS IINA media player. The codebase is self-contained and does not depend on the macOS IINA codebase at runtime - it is an independent Qt/C++ implementation that replicates the same user experience.

To continue development:
1. Read this document and `BUILDING.md`
2. Clone the repo, build on a Windows machine
3. Check `KNOWN_GAPS.md` for the feature roadmap
4. Check GitHub Issues for bug reports
5. Follow the milestone plan (M2 -> M3 -> M4 -> M5)

The architecture is straightforward: Qt QML for UI, C++ models for data, libmpv for media. Any Qt developer familiar with QML should be productive within a day.
