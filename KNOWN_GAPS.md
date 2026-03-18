# Known Gaps - IINA for Windows v0.1.0

Features and capabilities not yet implemented, with priority and target timeline.

## Gap Categories

- **P0 Critical** - Blocks release or causes significant user friction
- **P1 Important** - Noticeable absence, high-demand feature
- **P2 Nice-to-have** - Enhancement, lower user impact
- **N/A** - macOS-only, will not be implemented

---

## Feature Gaps

### Playback & Media

| Gap | macOS Reference | Priority | Target | Notes |
|-----|----------------|----------|--------|-------|
| A-B loop | PlayerCore ab-loop-a/b | P2 | M2 | Set loop start/end for repeated playback |
| Audio equalizer | FilterWindowController 10-band EQ | P2 | M2 | Video filters done; audio EQ not yet |
| Seek bar thumbnail preview | ThumbnailPeekView | P1 | M2 | Hover shows video frame preview |
| Seek bar chapter markers | PlaySlider chapter ticks | P2 | M2 | Visual marks at chapter positions on seek bar |
| Playlist UI drag reorder | NSTableView drag | P1 | M2 | Backend ready (playlistMove); QML drag UI missing |
| History search/filter | HistoryController search | P2 | M2 | Full-text search within playback history |
| Mini player / music mode | MiniPlayerWindowController | P1 | M2 | Compact floating player |
| Picture-in-Picture | VideoPIPViewController | P2 | M2 | Always-on-top small video window |
| Media inspector | InspectorWindowController | P2 | M2 | Codec, bitrate, resolution, frame rate details |

### Subtitle

| Gap | macOS Reference | Priority | Target | Notes |
|-----|----------------|----------|--------|-------|
| Full system font picker | NSFontPanel | P1 | M2 | Currently: 5 curated fonts |
| Full color picker | NSColorPanel | P1 | M2 | Currently: 4 preset colors |
| Shooter subtitle source | ShooterSubtitle.swift | P2 | M3 | Chinese subtitle API |
| OpenSubtitles file hash | File hash matching | P2 | M2 | Currently: query string only |

### Interface

| Gap | macOS Reference | Priority | Target | Notes |
|-----|----------------|----------|--------|-------|
| Full 8-tab preferences | PrefXXXViewController | P1 | M2 | Currently: 4 tabs |
| Light theme | System appearance | P2 | M2 | Currently: dark only |
| Mica/Acrylic backdrop | N/A (macOS vibrancy) | P2 | M3 | Windows 11 translucent window |
| OSC customization | PrefOSCToolbarSettingsSheetController | P2 | M2 | Control bar layout customization |
| Language selector in settings | Manual locale override | P2 | M2 | Currently: system locale only |

### System Integration

| Gap | macOS Reference | Priority | Target | Notes |
|-----|----------------|----------|--------|-------|
| File type associations | Info.plist UTI | P0 | M2 | Requires installer or manual registry |
| Media keys (SMTC) | MPNowPlayingInfoCenter | P1 | M2 | Keyboard media keys (play/pause/next/prev) |
| Sleep prevention | IOKit IOPMAssertionCreate | P1 | M2 | Prevent sleep during playback |
| URL protocol (iina://) | macOS URL scheme | P2 | M3 | Custom URL handler |
| Code signing (Authenticode) | Xcode signing | P0 | M2 | SmartScreen trust |
| Auto-update | Sparkle framework | P1 | M2 | WinSparkle infrastructure prepared |
| Microsoft Store / WinGet | N/A | P2 | M3 | Alternative distribution |
| Portable mode | N/A | P2 | M2 | Detect portable marker, store config locally |

### Extensibility

| Gap | macOS Reference | Priority | Target | Notes |
|-----|----------------|----------|--------|-------|
| Plugin system (JS) | JavascriptPlugin.swift (18 APIs) | P2 | M4 | Will use Qt QJSEngine |
| yt-dlp integration | io.iina.ytdl | P1 | M4 | Stream download/playback |
| Browser extension | Chrome Open In IINA | P2 | M4 | Depends on URL protocol |
| Full CLI tool | iina-cli/main.swift | P2 | M4 | IPC socket, pipe control |

---

## Technical Limitations

| Limitation | Description | Resolution |
|-----------|-------------|------------|
| OpenGL 3.3 required | Very old GPUs without OpenGL 3.3 fail | D3D11 fallback via `--vo=gpu-next` planned for M2 |
| MSVC-only build | No MinGW/Clang support | MSVC provides best Qt + Windows integration |
| No ARM64 build | x64 only | ARM64 Qt + mpv builds needed (low priority) |
| Single instance | No single-instance enforcement | Planned: named mutex + pipe for M2 |
| No GPU info OSD | No way to verify HW decode in UI | Media inspector planned for M2 |

---

## Feature Parity Summary

| Status | Count | Percentage |
|--------|-------|-----------|
| Implemented (DONE) | 38 | 56% |
| Adapted (Windows equivalent) | 9 | 13% |
| Partial | 6 | 9% |
| Deferred | 15 | 22% |
| Dropped (macOS-only) | 8 | N/A |
| **Total tracked** | **68 + 8 dropped** | **78% parity** |

---

## Milestone Roadmap

### M2 - Enhanced Experience (Next)
Focus: UI polish, system integration, code signing

Key items:
- File associations (installer-based)
- Code signing (Authenticode certificate)
- Media keys (SMTC integration)
- Sleep prevention
- Mini player
- Full preferences (8 tabs)
- Seek bar thumbnails
- Playlist drag reorder
- Light theme option
- Auto-update activation

### M3 - International & Community
Focus: Localization, community features

Key items:
- Additional localizations (ja_JP, ko_KR, etc.)
- Crowdin integration
- Shooter subtitle source
- URL protocol handler
- Microsoft Store submission

### M4 - Extensibility
Focus: Plugin system, integrations

Key items:
- QJSEngine plugin system
- yt-dlp integration
- Browser extension
- Full CLI with IPC

### M5 - Polish
Focus: Edge cases, advanced features

Key items:
- Mica/Acrylic visual effects
- ARM64 build
- D3D11 rendering fallback
- Advanced subtitle features
