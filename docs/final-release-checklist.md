# Final Release Checklist - IINA Windows v0.1.0

Pre-release verification checklist. All items must be PASS before publishing.

---

## Build Artifacts

- [x] Release build compiles without errors (MSVC 2022, Release config)
- [x] windeployqt collects all Qt dependencies
- [x] libmpv-2.dll bundled in output directory
- [x] InnoSetup installer script produces .exe (`installer/iina-windows.iss`)
- [x] Portable ZIP script produces .zip (`scripts/package-portable.ps1`)
- [x] CI pipeline builds successfully (`.github/workflows/ci.yml`)
- [x] Release pipeline configured (`.github/workflows/release.yml`)

## Installer (InnoSetup)

- [x] Installs to Program Files\IINA
- [x] Creates Start Menu shortcut
- [x] Creates Desktop shortcut (optional)
- [x] File type associations (.mp4, .mkv, .avi, .mov, .wmv, .flv, .webm, .m4v, .mpg, .mpeg, .ts)
- [x] "Open with IINA" context menu for video files
- [x] Supports per-user and per-machine install
- [x] Uninstaller removes files, shortcuts, and registry entries
- [x] License displayed during installation (GPL-3.0)
- [x] English and Chinese language support in installer UI

## Portable Edition

- [x] ZIP extracts and runs without installation
- [x] `portable.txt` marker triggers local config mode
- [x] README-PORTABLE.txt included with usage instructions
- [x] No registry writes in portable mode
- [x] Settings saved to local `config/` directory

## Core Functionality

- [x] Local file playback (MP4, MKV, AVI, MOV, WebM, FLV)
- [x] Play / Pause / Stop / Seek
- [x] Volume control with mouse wheel
- [x] Fullscreen (F key, double-click, menu)
- [x] Playback speed adjustment (0.25x - 4.0x)
- [x] Frame stepping (forward/backward)
- [x] Chapter navigation
- [x] Playback history with resume
- [x] Screenshot capture (PNG, JPG, WebP)
- [x] Video filters (brightness, contrast, saturation, gamma, hue)
- [x] Hardware decoding (auto-safe default)

## Subtitle System

- [x] External subtitle loading (.srt, .ass, .ssa, .vtt)
- [x] Auto-matching from same directory
- [x] Track selection and cycling
- [x] Subtitle styling (font, size, color, border, position, delay)
- [x] Drag-and-drop subtitle loading
- [x] Online subtitle search (OpenSubtitles)

## User Interface

- [x] Material Dark theme
- [x] Menu bar (File, Playback, Video, Audio, Subtitle, Window, Help)
- [x] Control bar with transport controls
- [x] Side panels (Playlist, Chapters, History)
- [x] OSD feedback
- [x] Right-click context menu
- [x] Settings dialog (4 tabs)
- [x] Keyboard shortcut editor
- [x] About dialog with version and attribution

## Localization

- [x] Chinese (zh_CN) translation - 172 strings, 100% coverage
- [x] Qt Linguist infrastructure (.ts -> .qm)
- [x] System locale auto-detection
- [x] No visible English residuals in zh_CN mode
- [x] Consistent terminology per glossary
- [x] No accelerator/mnemonic conflicts
- [x] CJK text renders correctly at all DPI levels

## Documentation

- [x] README.md (English)
- [x] docs/README.zh-CN.md (Chinese)
- [x] BUILDING.md (build instructions)
- [x] CHANGELOG.md (version history)
- [x] KNOWN_GAPS.md (missing features and roadmap)
- [x] LICENSE (GPL-3.0)
- [x] THIRD_PARTY_LICENSES.txt (third-party notices)
- [x] docs/mac-vs-windows-final-matrix.md (feature comparison)
- [x] docs/regression-test-report.md (test results)
- [x] docs/maintenance-handover.md (project handover guide)
- [x] docs/glossary.zh-CN.md (Chinese terminology)
- [x] docs/localization-coverage.md (translation coverage)
- [x] docs/l10n-qa-report.md (localization QA)
- [x] docs/ui-decisions.md (design decisions)

## Quality Assurance

- [x] 12 video formats tested (H.264, H.265, AV1, VP9, MPEG-4, etc.)
- [x] 10 subtitle formats tested (SRT, ASS, VTT, PGS, etc.)
- [x] 9 resolutions tested (SD to 4K)
- [x] DPI scaling tested (100%, 125%, 150%, 200%)
- [x] Chinese UI verified (7 spot checks)
- [x] 33 core features verified
- [x] Installation/uninstallation tested
- [x] Portable edition tested
- [x] Edge cases tested (corrupt files, long filenames, etc.)

## Auto-Update Infrastructure

- [x] WinSparkle wrapper (`src/utils/autoupdate.h/cpp`)
- [x] Appcast XML template (`installer/appcast.xml`)
- [ ] WinSparkle wired into main.cpp (prepared, not activated)
- [ ] Code signing certificate (not yet acquired)
- [ ] Hosted appcast URL (not yet configured)

## Remaining Items (Non-Blocking)

- [ ] Acquire Authenticode code signing certificate
- [ ] Set up appcast hosting (GitHub Pages or CDN)
- [ ] Activate WinSparkle in main.cpp after signing
- [ ] Convert iina.svg to iina.ico for installer icon
- [ ] Create Windows Store submission (MSIX packaging)

---

## Release Decision

### Version: v0.1.0

### Release Type: **Public Beta**

### Rationale
- Core playback is solid (all formats, codecs, resolutions)
- UI is complete and polished (Material Dark, all panels, settings)
- Chinese localization at 100% coverage
- Installer and portable packages ready
- 78% feature parity with macOS IINA

### Not Yet Ready for Stable Release Because
- No code signing (SmartScreen warning)
- No auto-update mechanism active
- No media key (SMTC) integration
- No file association without installer
- No mini player or PiP
- Settings only 4/8 tabs
- No automated test suite

### Recommendation
**Ship as v0.1.0-beta.1** with:
- "Public Beta" label
- GitHub pre-release flag
- Clear beta disclaimer in README and installer
- Issue tracker link for bug reports
