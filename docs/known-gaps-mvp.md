# IINA Windows MVP - Known Gaps & Deferred Features

## Status Legend
- **DONE** - Implemented in this MVP
- **DEFERRED** - Planned for future milestone
- **REPLACED** - Windows-native equivalent planned
- **DROPPED** - macOS-only, will not be implemented

---

## Feature Status Matrix

### DONE (Implemented in MVP)

| Feature | Notes |
|---------|-------|
| Local file playback | File dialog, drag-drop, command line |
| URL playback | Open URL dialog, mpv handles URLs natively |
| Play / Pause / Stop | Button + Space key |
| Seek (click + keyboard) | Seek bar + Left/Right arrows |
| Volume control | Slider + Up/Down arrows + mouse wheel |
| Mute | Button + M key |
| Fullscreen | Button + F key + double-click + Escape to exit |
| Subtitle loading | External file via dialog or drag-drop |
| Subtitle track selection | Menu + control bar button |
| Audio track selection | Menu |
| Basic playlist | Sidebar panel with add/remove/clear/reorder |
| Playback speed | Keyboard + menu (0.25x-4.0x) |
| Frame stepping | Forward (.) and backward (,) |
| Screenshot | Via menu, saves to Pictures folder |
| OSD (on-screen display) | Volume and speed change feedback |
| Settings skeleton | 4-tab dialog (General, Video, Audio, Subtitle) |
| Hardware decoding | auto-safe default, configurable |
| Dark theme | Material Dark via Qt Quick Controls |
| Keyboard shortcuts | Full shortcut set (Space, arrows, F, M, P, etc.) |
| Window title | Shows media title or filename |
| Time display | Position / Duration in control bar |
| About dialog | Version, attribution, license |
| Menu bar | File, Playback, Video, Audio, Subtitle, Window, Help |
| GPL-3.0 license | LICENSE + THIRD_PARTY_LICENSES.txt |
| CI workflow | GitHub Actions Windows build |
| Logging | File-based logging to AppData |
| Config persistence | Volume, mute state saved across sessions |

### DEFERRED (Future Milestones)

| Feature | macOS Reference | Priority | Target Milestone |
|---------|----------------|----------|-----------------|
| Mini player / music mode | `MiniPlayerWindowController.swift` | P1 | M2 |
| Picture-in-Picture | `VideoPIPViewController.swift` | P2 | M2 |
| Seek bar thumbnail preview | `ThumbnailPeekView.swift` | P1 | M2 |
| Playback history | `HistoryController.swift` | P1 | M2 |
| Video filters (brightness, contrast) | `FilterWindowController.swift` | P2 | M2 |
| Audio equalizer | `FilterWindowController.swift` | P2 | M2 |
| Customizable keybindings | `PrefKeyBindingViewController.swift` | P1 | M2 |
| Chapter navigation | `MPVChapter.swift` | P1 | M2 |
| A-B loop | `PlayerCore.swift` | P2 | M2 |
| Media inspector | `InspectorWindowController.swift` | P2 | M2 |
| Full preferences (8 panels) | `PrefXXXViewController.swift` | P1 | M2 |
| On-screen controller customization | `PrefOSCToolbarSettingsSheetController.swift` | P2 | M2 |
| Online subtitle search | `OpenSubSubtitle.swift`, `ShooterSubtitle.swift` | P1 | M3 |
| Plugin system (JavaScript) | `JavascriptPlugin.swift` (18 API modules) | P2 | M4 |
| yt-dlp plugin | `io.iina.ytdl` | P1 | M4 |
| Browser extensions | `browser/Chrome_Open_In_IINA/` | P2 | M4 |
| CLI tool | `iina-cli/main.swift` | P2 | M4 |
| Auto-update (WinSparkle) | Sparkle (`AppDelegate.swift`) | P1 | M5 |
| Code signing (Authenticode) | Xcode signing | P0 | M5 |
| Installer (Inno Setup/NSIS) | DMG creation | P0 | M5 |
| Microsoft Store / WinGet | N/A | P2 | M5 |
| File associations | macOS Info.plist UTI | P1 | M5 |
| URL protocol (`iina://`) | macOS URL scheme | P2 | M5 |

### REPLACED (Windows-native equivalent)

| macOS Feature | Windows Replacement | Status |
|--------------|-------------------|--------|
| NSUserDefaults (plist) | QSettings INI file | DONE |
| Sparkle auto-update | WinSparkle (planned) | DEFERRED |
| NSMenu | Qt MenuBar | DONE |
| NSOpenGLView / CAOpenGLLayer | QQuickFramebufferObject + OpenGL | DONE |
| CVDisplayLink (vsync) | QML frame rendering (Qt manages vsync) | DONE |
| MPNowPlayingInfoCenter | Windows SMTC (planned) | DEFERRED |
| IOKit sleep prevention | SetThreadExecutionState (planned) | DEFERRED |
| macOS Keychain | Windows Credential Manager (planned) | DEFERRED |
| Apple Events / URL scheme | Windows protocol handler (planned) | DEFERRED |
| DMG installer | NSIS/Inno Setup + MSIX (planned) | DEFERRED |
| Notarization | Authenticode signing (planned) | DEFERRED |

### DROPPED (Will not be implemented)

| macOS Feature | Reason |
|--------------|--------|
| Touch Bar support | MacBook Pro hardware only |
| Force Touch / pressure gestures | MacBook trackpad hardware only |
| Safari extension | Safari is macOS-only |
| Handoff / Continuity | Apple ecosystem only |
| macOS Spaces fullscreen animation | macOS-specific window management |
| NSVisualEffectView vibrancy | Will use Windows Mica/Acrylic instead (different look) |
| Hardened Runtime / sandboxing | macOS-specific security model |
| PIP.framework (private API) | macOS private framework; custom always-on-top window planned instead |

---

## Architecture Gaps

| Gap | Description | Resolution Plan |
|-----|-------------|----------------|
| No JS plugin engine | JavaScriptCore is macOS-only | Will use QJSEngine (Qt built-in) in M4 |
| No SMTC integration | Media keys don't work | Planned for M2 via Windows.Media.SystemMediaTransportControls |
| No file associations | Double-clicking .mp4 doesn't open IINA | Planned for M5 installer |
| No portable mode | Settings always in AppData | Planned: detect run-from-removable-drive |
| No auto-update | Users must update manually | WinSparkle integration in M5 |
| Locale not configurable | Uses system locale | Planned: language selector in settings |
| No zh-Hans localization | UI is English-only | Translation infrastructure planned for M3 |

---

## Known Technical Limitations

1. **OpenGL requirement:** The mpv rendering path requires OpenGL 3.3+. Very old GPUs without OpenGL 3.3 support may not work. A Direct3D 11 fallback is planned for M2.

2. **Header stubs:** The `deps/include/mpv/` headers are stubs for project structure reference. Real headers are downloaded by `scripts/download-deps.ps1` from mpv's pre-built packages.

3. **Build environment:** The project was developed in a Linux sandbox. Full compilation verification requires a Windows machine with Qt 6 and MSVC installed.

4. **Volume slider wheel event:** The mouse wheel volume control uses a workaround (MouseArea over Slider) that may need refinement on actual hardware.

5. **Seek bar precision:** The seek bar uses percentage-based seeking. Sub-second precision may vary depending on media container keyframe intervals.
