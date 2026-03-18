# Changelog

All notable changes to IINA for Windows will be documented in this file.

The format follows [Keep a Changelog](https://keepachangelog.com/en/1.1.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2026-03-18

Initial public beta release. Windows port of IINA built with Qt 6 and libmpv.

### Added

**Core Playback**
- Local file playback via file dialog, drag-and-drop, and command-line arguments
- URL/network stream playback (mpv-native, supports yt-dlp if installed)
- Play, Pause, Stop, Seek (5s/30s/60s increments)
- Volume control (0-150%) with mouse wheel support
- Mute toggle
- Fullscreen (F key, double-click, menu, button)
- Playback speed control (0.25x - 4.0x) with keyboard shortcuts
- Frame stepping (forward and backward)
- Chapter navigation (panel, menu, keyboard shortcuts)
- Playback history with per-file resume position (JSON-backed, max 200 entries)
- Recent files menu
- Screenshot capture (PNG, JPG, WebP) with configurable save directory
- Video filters panel (brightness, contrast, saturation, gamma, hue)
- Hardware decoding (auto-safe default, configurable)

**Subtitle System**
- External subtitle file loading (dialog, drag-and-drop)
- Auto-matching subtitles from same directory (by base name)
- Subtitle track selection and cycling
- Subtitle styling: font, size, color (4 presets), border, position, delay
- Subtitle scale adjustment
- Online subtitle search via OpenSubtitles REST API

**User Interface**
- Material Dark theme (Qt Quick Controls 2)
- Menu bar: File, Playback, Video, Audio, Subtitle, Window, Help
- Bottom control bar with seek bar, transport controls, volume, panel toggles
- Side panels: Playlist, Chapters, History
- On-screen display (OSD) for volume, speed, screenshot, subtitle delay feedback
- Right-click context menu
- About dialog with version and attribution
- Custom window title (shows media filename/title)

**Settings**
- 4-tab settings dialog: General, Video, Audio, Subtitle
- Resume playback position toggle
- Pause on minimize toggle
- Dark theme toggle
- Hardware decoding mode selector
- Screenshot format and directory configuration
- Default volume setting
- Subtitle appearance customization (font, size, color, border, position, delay)
- Auto-load subtitles from same directory
- Full keyboard shortcut configuration with GUI editor

**Keyboard Shortcuts**
- 33 configurable actions across 5 categories (Playback, Seek, Volume, Subtitle, Window)
- GUI shortcut editor with key capture
- Per-action and global reset to defaults
- Persistent JSON-based shortcut storage

**Localization**
- Full Simplified Chinese (zh_CN) translation (172 strings)
- Qt Linguist translation infrastructure (.ts/.qm)
- Automatic system locale detection
- Resource-embedded translations with filesystem fallback
- Terminology glossary (170+ terms)

**Build & Distribution**
- CMake 3.21+ build system
- GitHub Actions CI pipeline (Windows build)
- InnoSetup installer script with file associations
- Portable ZIP packaging script
- WinSparkle auto-update infrastructure (placeholder)
- `scripts/download-deps.ps1` for libmpv acquisition
- GPL-3.0 license with third-party notices

**Documentation**
- English and Chinese README
- Build instructions (BUILDING.md)
- Feature parity matrix (macOS vs Windows)
- UI design decisions document
- Localization coverage and QA reports
- Project maintenance handover guide

### Architecture

- **UI Framework**: Qt 6 QML with Material Dark theme
- **Media Engine**: libmpv via C API (mpv_handle + mpv_render_context)
- **Rendering**: OpenGL 3.3 Core via QQuickFramebufferObject
- **Pattern**: MVVM via Qt property system (C++ models exposed to QML)
- **Config**: QSettings (INI) for preferences, JSON for history and shortcuts
- **Logging**: File-based logging to %APPDATA%/IINA/iina.log

### Known Limitations

- Requires OpenGL 3.3+ (Direct3D fallback planned)
- Playlist drag-and-drop reorder not yet implemented in UI
- Subtitle font picker limited to 5 curated fonts (full picker planned)
- Subtitle color limited to 4 presets (full picker planned)
- No file type associations without installer
- No media key (SMTC) integration
- No auto-update (infrastructure prepared but not active)
- Settings has 4 tabs (8-tab full preferences planned)

[0.1.0]: https://github.com/iina/iina-windows/releases/tag/v0.1.0
