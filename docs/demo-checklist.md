# IINA Windows MVP - Demo Checklist

## How to Run the Demo

1. Build the project following `build-instructions-windows.md`
2. Ensure `libmpv-2.dll` is in the same directory as the executable
3. Launch `iina-windows.exe`

## Feature Verification Checklist

### Core Playback

| # | Feature | How to Test | Status |
|---|---------|-------------|--------|
| 1 | Open local file via menu | File > Open File > select .mp4/.mkv | IMPLEMENTED |
| 2 | Open local file via drag-and-drop | Drag video file onto window | IMPLEMENTED |
| 3 | Open URL | File > Open URL > paste URL | IMPLEMENTED |
| 4 | Open file via command line | `iina-windows.exe path/to/video.mp4` | IMPLEMENTED |
| 5 | Play / Pause toggle | Click play button or press Space | IMPLEMENTED |
| 6 | Seek via progress bar | Click on seek bar | IMPLEMENTED |
| 7 | Seek via keyboard | Left/Right arrow keys (5s) | IMPLEMENTED |
| 8 | Volume control via slider | Drag volume slider | IMPLEMENTED |
| 9 | Volume control via keyboard | Up/Down arrow keys | IMPLEMENTED |
| 10 | Mute toggle | Click volume icon or press M | IMPLEMENTED |
| 11 | Fullscreen toggle | Click fullscreen button or press F | IMPLEMENTED |
| 12 | Exit fullscreen | Press Escape or F | IMPLEMENTED |
| 13 | Frame step forward | Press . (period) | IMPLEMENTED |
| 14 | Frame step backward | Press , (comma) | IMPLEMENTED |

### Subtitle Support

| # | Feature | How to Test | Status |
|---|---------|-------------|--------|
| 15 | Load external subtitle file | Subtitle > Load Subtitle File | IMPLEMENTED |
| 16 | Select subtitle track | Subtitle menu or control bar button | IMPLEMENTED |
| 17 | Disable subtitles | Select "None" in subtitle menu | IMPLEMENTED |
| 18 | Drag-drop subtitle file | Drop .srt/.ass file onto playing video | IMPLEMENTED |

### Playlist

| # | Feature | How to Test | Status |
|---|---------|-------------|--------|
| 19 | Show/hide playlist panel | Press P or click playlist button | IMPLEMENTED |
| 20 | Open multiple files at once | File > Open > select multiple | IMPLEMENTED |
| 21 | Add files to playlist | Click + in playlist panel | IMPLEMENTED |
| 22 | Play specific item | Click item in playlist | IMPLEMENTED |
| 23 | Remove item from playlist | Hover and click X | IMPLEMENTED |
| 24 | Clear playlist | Click clear button in playlist header | IMPLEMENTED |
| 25 | Next/Previous track | Click prev/next buttons or menu | IMPLEMENTED |

### Speed Control

| # | Feature | How to Test | Status |
|---|---------|-------------|--------|
| 26 | Increase speed | Press ] | IMPLEMENTED |
| 27 | Decrease speed | Press [ | IMPLEMENTED |
| 28 | Reset speed | Press Backspace | IMPLEMENTED |
| 29 | Speed via menu | Playback > Speed > select | IMPLEMENTED |

### UI Features

| # | Feature | How to Test | Status |
|---|---------|-------------|--------|
| 30 | Window title shows filename | Open a file, check title bar | IMPLEMENTED |
| 31 | Time display (position/duration) | Check control bar | IMPLEMENTED |
| 32 | OSD feedback | Change volume, verify overlay | IMPLEMENTED |
| 33 | Dark theme | Visual check | IMPLEMENTED |
| 34 | Settings dialog skeleton | Window > Settings | IMPLEMENTED |
| 35 | About dialog | Help > About IINA | IMPLEMENTED |
| 36 | Idle state (drop hint) | Launch without file | IMPLEMENTED |
| 37 | Keyboard shortcuts | All shortcuts listed above | IMPLEMENTED |
| 38 | Screenshot | Video > Screenshot or menu | IMPLEMENTED |
| 39 | Audio track selection | Audio menu | IMPLEMENTED |
| 40 | Seek bar hover tooltip | Hover over seek bar | IMPLEMENTED |

### Technical

| # | Feature | How to Test | Status |
|---|---------|-------------|--------|
| 41 | Hardware decoding | Check settings > Video > hwdec | IMPLEMENTED |
| 42 | Auto-hide controls in fullscreen | Enter fullscreen, wait 3s | IMPLEMENTED |
| 43 | Mouse wheel volume | Scroll on volume area | IMPLEMENTED |
| 44 | Double-click fullscreen | Double-click video area | IMPLEMENTED |
| 45 | Window resize | Drag window edges | IMPLEMENTED |

## Summary

- **Total features:** 45
- **Implemented:** 45
- **Pending verification on Windows:** All (requires Windows build + libmpv)
