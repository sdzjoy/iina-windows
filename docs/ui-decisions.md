# IINA Windows - UI Design Decisions

What stays IINA style, what adapts to Windows native, and why.

---

## Guiding Principles

1. **IINA identity first.** Menu structure, naming, information hierarchy, and mental model come from macOS IINA.
2. **Windows convention where users expect it.** Right-click context menu, mouse wheel = volume, Ctrl+key combos, standard window chrome.
3. **Don't imitate macOS.** No vibrancy blur, no traffic-light buttons, no NSMenu styling. Use Windows-native UI primitives styled with IINA's color palette.
4. **Material Dark as base.** Qt Quick Controls Material Dark provides a modern dark theme that maps well to IINA's dark aesthetic without trying to look like macOS.

---

## Decision Matrix

### A. Window & Chrome

| Element | macOS IINA | Windows IINA | Decision | Rationale |
|---------|-----------|-------------|----------|-----------|
| Title bar | Custom NSWindow with vibrancy | Standard Win32 title bar | **ADAPTED** | Users expect native title bar on Windows. Snap, Aero Shake, and title bar drag all work out of the box. |
| Window buttons | Traffic light (close/min/zoom) | Win32 min/max/close | **ADAPTED** | Non-negotiable Windows convention. |
| Menu bar | NSMenu (system-level) | Qt MenuBar inside window | **KEPT (adapted)** | Same menu structure (File, Playback, Video, Audio, Subtitle, Window, Help). Rendered inside window per Windows convention. |
| Menu item names | Same | Same | **KEPT** | "Open File...", "Fullscreen", "Screenshot" - IINA naming preserved exactly. |
| Accelerators | Cmd+O, Cmd+Q, etc. | Ctrl+O, Ctrl+Q, etc. | **ADAPTED** | Standard key mapping: Cmd -> Ctrl. |
| About dialog | NSAboutPanel (system template) | Custom Qt Dialog | **ADAPTED** | Same content (name, version, attribution, license), custom layout. |

### B. Playback Controls

| Element | macOS IINA | Windows IINA | Decision | Rationale |
|---------|-----------|-------------|----------|-----------|
| Control bar position | Bottom overlay | Bottom overlay | **KEPT** | Core IINA layout. Dark semi-transparent bar at bottom. |
| Control bar color | Vibrancy blur + dark | `#cc1a1a1a` (85% opaque dark) | **ADAPTED** | Same visual weight, no vibrancy (not available on Windows). Solid dark matches Material Dark theme. |
| Play/Pause button | Center of control bar | Left side of control bar | **ADAPTED** | Windows convention (VLC, MPC-HC, Films & TV). macOS IINA centers it. Windows users expect left-aligned transport. |
| Seek bar | Custom `PlaySlider` | `SeekBar.qml` with hover tooltip | **KEPT** | Same percentage-based seek. Tooltip shows time on hover. |
| Time display | Left of seek bar | Below seek bar, left side | **KEPT** | "HH:MM:SS / HH:MM:SS" format preserved. Consolas monospace font. |
| Volume control | Vertical slider in control bar | Horizontal slider in control bar | **ADAPTED** | Windows convention: horizontal volume. macOS IINA uses vertical. |
| Speed indicator | Not prominently shown | Accent-colored label when != 1.0x | **ENHANCED** | More visible speed feedback. Material.accent (teal) makes it noticeable. |

### C. Side Panels

| Element | macOS IINA | Windows IINA | Decision | Rationale |
|---------|-----------|-------------|----------|-----------|
| Panel pattern | NSOutlineView sidebar | QML side panel (280-300px) | **ADAPTED** | Same sidebar concept, different widget toolkit. Panels slide in from right. |
| Playlist panel | Sidebar with drag-reorder | `PlaylistPanel.qml` (280px) | **KEPT** | Same layout: item list with title, duration, current indicator, add/remove buttons. |
| Chapter panel | Chapter list in sidebar | `ChapterPanel.qml` (280px) | **KEPT** | Same layout: numbered chapters with time, current highlight. Click to seek. |
| History panel | Dedicated window | `HistoryPanel.qml` (300px, side panel) | **ADAPTED** | macOS uses separate window. Windows uses side panel for consistency with playlist/chapters. |
| Panel toggle | Menu and keyboard | Menu, keyboard, and control bar buttons | **ENHANCED** | Control bar has dedicated Playlist/Chapters buttons for quick toggle. |

### D. Interactions

| Element | macOS IINA | Windows IINA | Decision | Rationale |
|---------|-----------|-------------|----------|-----------|
| Mouse wheel on video | Configurable (default: seek) | Scroll = volume, Shift+scroll = seek | **ADAPTED** | Windows convention (VLC, MPC-HC, PotPlayer all use wheel = volume). Power users can Shift+scroll for seek. |
| Double-click | Toggle fullscreen | Toggle fullscreen | **KEPT** | Universal convention. |
| Right-click | No standard behavior | Context menu (Play/Pause, Stop, Fullscreen, Screenshot, Speed, Open, Settings) | **ADAPTED** | Right-click context menu is a strong Windows convention. Not standard in macOS apps. |
| Drag-and-drop | NSDocument open | `DropArea` with visual overlay | **KEPT** | Same behavior: drop media to play, drop .srt to load subtitle. Visual feedback adapted (rectangle overlay vs macOS drag badge). |
| Fullscreen | macOS Spaces animation | Win32 borderless fullscreen | **ADAPTED** | macOS Spaces slide-in animation is OS-specific. Windows: instant borderless fullscreen. |
| Auto-hide controls | Cursor idle timeout | Same (3s timeout in fullscreen) | **KEPT** | Same behavior: mouse movement shows controls, 3 seconds idle hides them. |

### E. Settings

| Element | macOS IINA | Windows IINA | Decision | Rationale |
|---------|-----------|-------------|----------|-----------|
| Settings layout | 8-tab NSPreferencePane | 4-tab Dialog (General, Video, Audio, Subtitle) | **PARTIAL** | MVP uses 4 core tabs. Full 8-tab layout planned for M2. |
| Settings persistence | NSUserDefaults (plist) | QSettings (INI file) | **ADAPTED** | Standard persistence for each platform. INI file is more transparent for power users. |
| Keyboard shortcuts config | Reads mpv input.conf | JSON-based with GUI editor | **ADAPTED** | macOS reuses mpv's format. Windows: dedicated JSON + full GUI with key capture, per-action reset, category tabs. More user-friendly. |
| Subtitle font picker | NSFontPanel (full system fonts) | Curated dropdown (Default, Arial, Segoe UI, Consolas, Noto Sans) | **PARTIAL** | Full font picker planned for M2. Curated list covers common use cases. |
| Subtitle color picker | Full NSColorPanel | Preset dropdown (White, Yellow, Cyan, Green) | **PARTIAL** | Full color picker planned for M2. Presets cover 90% of use cases. |

### F. Visual Theme

| Element | macOS IINA | Windows IINA | Decision | Rationale |
|---------|-----------|-------------|----------|-----------|
| Base theme | Dark + vibrancy | Material Dark (#1a1a1a) | **ADAPTED** | Same dark aesthetic. Material Dark provides consistent component styling. No vibrancy mimicry. |
| Accent color | System accent (default blue) | Material.Teal | **ADAPTED** | Teal provides good contrast on dark background. Matches IINA's modern feel without copying macOS blue. |
| Typography | System (San Francisco) | System (Segoe UI) + Consolas for time | **ADAPTED** | Platform-native fonts. Consolas for monospace time display (Windows-native monospace). |
| Icons | SF Symbols + custom | SVG icons (Material Design inspired) | **ADAPTED** | SF Symbols are macOS-only. Custom SVG icons follow Material Design language, consistent with Qt Material theme. |
| OSD | Custom overlay | `OSD.qml` overlay | **KEPT** | Same pattern: semi-transparent rounded pill showing feedback text with auto-dismiss. |

---

## Key Differences from macOS IINA (Summary)

1. **Right-click context menu** - Windows convention, not in macOS IINA.
2. **Mouse wheel = volume** - VLC/MPC convention on Windows. macOS IINA defaults to seek.
3. **Ctrl instead of Cmd** - Standard Windows key mapping.
4. **No vibrancy/blur** - Uses solid dark backgrounds. Mica/Acrylic deferred to M2.
5. **History as side panel** - macOS uses separate window. Windows integrates with existing side panel pattern.
6. **JSON shortcut storage** - macOS reuses mpv input.conf. Windows uses dedicated JSON with GUI editor.
7. **Horizontal volume slider** - macOS IINA uses vertical. Windows convention is horizontal.
8. **Left-aligned transport controls** - macOS centers play button. Windows left-aligns (VLC/MPC convention).

## What Stays IINA (Non-Negotiable)

1. **Menu structure:** File, Playback, Video, Audio, Subtitle, Window, Help.
2. **Menu item naming:** "Open File...", "Open URL...", "Fullscreen", "Screenshot", "Chapters".
3. **Control bar layout:** Bottom overlay with seek bar + transport + volume + panels.
4. **Side panel pattern:** Playlist, chapters, history as sliding panels.
5. **OSD pattern:** Feedback overlay for volume, speed, screenshot, subtitle delay.
6. **Dark-first aesthetic:** IINA is a dark-themed player. Light theme is optional (deferred).
7. **Feature hierarchy:** Playback > Subtitle > Interface > System integration.
8. **Resume playback:** Per-file position saved and restored.
9. **Drag-and-drop:** Media files open, subtitle files load.
10. **Keyboard-centric:** Full shortcut set, configurable.
