# IINA Windows - Deferred Features

Features explicitly not implemented in the current milestone, with reasons and target timeline.

---

## Status Definitions

- **DEFERRED** - Will be implemented in a future milestone
- **PARTIAL** - Core functionality exists, full feature planned later
- **DROPPED** - Will not be implemented (macOS-only or not applicable)

---

## Deferred Features by Milestone

### M2 - Enhanced Experience (Next)

| Feature | macOS Reference | Reason for Deferral | Notes |
|---------|----------------|-------------------|-------|
| Mini player / music mode | `MiniPlayerWindowController.swift` | Requires second window management, compact layout | Floating mini player with album art and basic controls |
| Picture-in-Picture | `VideoPIPViewController.swift` | Requires always-on-top window management | Windows: custom always-on-top window (no PIP.framework) |
| Seek bar thumbnail preview | `ThumbnailPeekView.swift` | Requires mpv thumbnail generation pipeline | Hover over seek bar shows video frame preview |
| Audio equalizer | `FilterWindowController.swift` 10-band EQ | Complex UI (10 sliders + presets), lower priority than video filters | Video filters (brightness/contrast/saturation/gamma/hue) are done |
| A-B loop | `PlayerCore.swift` ab-loop-a/b | Niche feature, lower priority | Set loop start/end points for repeated playback |
| Media inspector | `InspectorWindowController.swift` | Metadata display window, not core playback | Shows codec, bitrate, resolution, frame rate details |
| Full preferences (8 tabs) | `PrefXXXViewController.swift` | Current 4-tab settings covers core needs | Add: Network, Advanced, OSC customization, Key Bindings tabs |
| Light theme | System appearance support | Material Dark covers primary use case | Add theme toggle: dark/light/system |
| History search | Full-text search in history | History list is functional, search is enhancement | Filter history entries by filename/title |
| Media keys (SMTC) | `MPNowPlayingInfoCenter` | Requires Windows.Media.SystemMediaTransportControls COM | Play/pause/next/prev from keyboard media keys |
| Sleep prevention | IOKit `IOPMAssertionCreate` | Requires `SetThreadExecutionState` Win32 API | Prevent sleep during video playback |
| Seek bar chapter marks | Chapter slider marks in `PlaySlider.swift` | Chapter panel and menu navigation are done; visual marks on seek bar are enhancement | Draw tick marks on seek bar at chapter positions |
| Mica/Acrylic backdrop | N/A (macOS vibrancy) | Requires DWM API calls, Windows 11 only | Optional translucent window backdrop |

### M3 - International & Community

| Feature | macOS Reference | Reason for Deferral | Notes |
|---------|----------------|-------------------|-------|
| zh-Hans localization | 55 .lproj directories | Translation infrastructure (`qsTr()`) is ready; strings need translation | First target language: Simplified Chinese |
| Shooter subtitle source | `ShooterSubtitle.swift` | Chinese-language subtitle source, lower priority outside China | API integration similar to OpenSubtitles |
| Additional localizations | Crowdin integration | Depends on M3 zh-Hans as template | Community translation workflow |

### M4 - Extensibility

| Feature | macOS Reference | Reason for Deferral | Notes |
|---------|----------------|-------------------|-------|
| Plugin system (JavaScript) | `JavascriptPlugin.swift` (18 API modules) | Major architecture: requires QJSEngine sandboxing, API surface design | macOS uses JavaScriptCore; Windows will use Qt's QJSEngine |
| yt-dlp plugin | `io.iina.ytdl` | Depends on plugin system | Download/stream integration via yt-dlp |
| Browser extensions | `browser/Chrome_Open_In_IINA/` | Depends on URL protocol handler + plugin system | Chrome/Edge "Open in IINA" extension |
| Full CLI tool | `iina-cli/main.swift` | Basic command-line args work; full CLI (pipe, IPC) is enhancement | Named pipe or socket for external control |

### M5 - Distribution & System Integration

| Feature | macOS Reference | Reason for Deferral | Notes |
|---------|----------------|-------------------|-------|
| Auto-update | Sparkle framework | Requires WinSparkle integration + update server | Check for updates on launch, download/install |
| Code signing (Authenticode) | Xcode code signing | Requires EV certificate + signing infrastructure | Sign .exe and .dll for SmartScreen trust |
| Installer (NSIS/Inno Setup) | DMG creation | Requires installer scripting + file association registration | Creates Start Menu shortcuts, optional file associations |
| Microsoft Store / WinGet | N/A | Depends on installer and signing | Alternative distribution channel |
| File associations | Info.plist UTI declarations | Requires installer to register file types in Windows registry | .mp4, .mkv, .avi etc. open with IINA |
| URL protocol (`iina://`) | macOS URL scheme handler | Requires Windows protocol handler registration | `iina://play/https://...` URL format |
| Portable mode | N/A | Enhancement: detect run from removable drive | Settings stored next to .exe instead of AppData |

---

## Partial Implementations (Done but Incomplete)

| Feature | What's Done | What's Missing | Target |
|---------|------------|----------------|--------|
| Subtitle font | Curated dropdown (5 fonts) | Full system font picker | M2 |
| Subtitle color | 4 preset colors | Full color picker | M2 |
| Online subtitle search | OpenSubtitles REST API query | File hash matching, download progress, Shooter source | M2-M3 |
| Settings dialog | 4 tabs (General, Video, Audio, Subtitle) | 8 tabs (+ Network, Advanced, OSC, Key Bindings) | M2 |
| CLI | Command-line file arguments | IPC socket, pipe control, full option parsing | M4 |
| Localization | `qsTr()` wrappers on all UI strings | Actual translations (zh-Hans first) | M3 |

---

## Dropped Features (Will Not Implement)

| macOS Feature | Reason |
|--------------|--------|
| Touch Bar support | MacBook Pro hardware only |
| Force Touch / pressure gestures | MacBook trackpad hardware only |
| Safari extension | Safari is macOS-only browser |
| Handoff / Continuity | Apple ecosystem feature |
| macOS Spaces fullscreen animation | macOS-specific window management |
| NSVisualEffectView vibrancy | macOS-only; Windows will use Mica/Acrylic (different API, different visual) |
| Hardened Runtime / App Sandbox | macOS-specific security model |
| PIP.framework | macOS private framework; replaced by always-on-top window approach |

---

## Decision Criteria

Features were deferred based on:

1. **Core playback impact** - Does it affect the primary video playback experience? If no, defer.
2. **Platform dependency** - Does it require Windows-specific APIs (SMTC, DWM, registry)? If complex, defer to dedicated milestone.
3. **User frequency** - How often does a typical user need this feature? Daily-use features prioritized over occasional-use.
4. **Implementation complexity** - Plugin system (M4) requires architectural foundation. Installer (M5) requires signing infrastructure.
5. **Dependency chain** - Browser extension depends on URL protocol, which depends on installer. Chain deferred together.
