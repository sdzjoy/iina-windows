# IINA Windows 版

基于 [IINA](https://github.com/iina/iina) 的 Windows 移植版本，使用 **Qt 6 (QML/C++)** 和 **libmpv** 构建。

## 功能特性

### 播放
- 本地文件播放（文件对话框、拖放、命令行）
- URL 播放（网络流、YouTube 需安装 yt-dlp）
- 播放 / 暂停 / 停止 / 快进快退 / 逐帧播放
- 鼠标滚轮音量控制（0-150%）
- 全屏切换（F 键、双击、按钮）
- 倍速控制（0.25x - 4.0x）
- 章节导航（面板、菜单、快捷键）
- 播放历史及续播
- 最近播放菜单
- 截图（PNG、JPG、WebP）
- 视频滤镜（亮度、对比度、饱和度、伽马、色相）
- 拖放支持（媒体文件 + 字幕文件）

### 字幕
- 外部字幕加载（对话框、拖放）
- 自动匹配（扫描同目录下同名字幕文件）
- 字幕轨道选择与切换
- 字幕样式设置（字体、颜色、描边、位置、延迟、缩放）
- 在线字幕搜索（OpenSubtitles REST API）

### 界面
- 深色主题（Material Dark）
- 右键上下文菜单
- 侧边面板：播放列表、章节、历史记录
- OSD 反馈（音量、倍速、截图、字幕延迟）
- 菜单栏（文件、播放、视频、音频、字幕、窗口、帮助）
- 设置对话框（通用、视频、音频、字幕）
- 可自定义快捷键（图形化配置界面）

### 快捷键

所有快捷键均可在 设置 > 配置快捷键 中自定义。

| 按键 | 功能 |
|------|------|
| Space | 播放 / 暂停 |
| 左/右方向键 | 快退/快进 5 秒 |
| Ctrl+左/右方向键 | 快退/快进 30 秒 |
| Shift+左/右方向键 | 快退/快进 60 秒 |
| 上/下方向键 | 音量 +5 / -5 |
| F | 切换全屏 |
| Escape | 退出全屏 |
| M | 切换静音 |
| P | 切换播放列表 |
| [ / ] | 减速/加速 0.25x |
| Backspace | 重置倍速为 1.0x |
| . / , | 逐帧前进 / 后退 |
| Ctrl+O | 打开文件 |
| Ctrl+Shift+O | 打开 URL |
| Ctrl+S | 截图 |
| Ctrl+Q | 退出 |
| S | 切换字幕轨道 |
| Z / Shift+Z | 字幕延迟 +0.5 秒 / -0.5 秒 |
| Shift+= / Shift+- | 字幕放大 / 缩小 |
| Ctrl+, | 设置 |
| Page Up / Page Down | 下一章节 / 上一章节 |

## 构建

详见 [docs/build-instructions-windows.md](build-instructions-windows.md)。

### 快速开始

```powershell
# 1. 安装依赖
#    - Visual Studio 2022（含 C++ 工作负载）
#    - Qt 6.7+（通过 Qt Online Installer）
#    - CMake 3.21+

# 2. 下载 libmpv
.\scripts\download-deps.ps1

# 3. 配置
cmake -B build -G "Visual Studio 17 2022" -DCMAKE_PREFIX_PATH="C:\Qt\6.7.0\msvc2019_64"

# 4. 编译
cmake --build build --config Release

# 5. 部署 Qt 运行时
windeployqt build\Release\iina-windows.exe --qmldir qml

# 6. 运行
.\build\Release\iina-windows.exe
```

## 项目结构

```
iina-windows/
  CMakeLists.txt              # 构建配置
  src/
    main.cpp                  # 入口
    application.h/cpp         # 应用单例
    core/
      mpvengine.h/cpp         # libmpv C API 封装
      mpvrenderer.h/cpp       # OpenGL 渲染到 QML
      playercore.h/cpp        # 播放器控制器
      shortcutmanager.h/cpp   # 快捷键管理
      subtitlesearch.h/cpp    # OpenSubtitles API 客户端
    models/
      playlistmodel.h/cpp     # 播放列表数据模型
      trackmodel.h/cpp        # 音频/字幕轨道模型
      chaptermodel.h/cpp      # 章节导航模型
      historymodel.h/cpp      # 播放历史（JSON）
    utils/
      config.h/cpp            # 设置（INI 格式）
      logger.h/cpp            # 文件日志
  qml/
    Main.qml                  # 主窗口
    components/               # UI 组件
    pages/                    # 设置页、快捷键配置
  translations/
    iina_zh_CN.ts             # 简体中文翻译
  resources/icons/            # SVG 图标
  docs/                       # 文档
  LICENSE                     # GPL-3.0
```

## 功能覆盖

macOS IINA 68 项功能中已实现或适配 53 项（78%）。详见 [docs/parity-matrix.md](parity-matrix.md)。

## 许可证

GPL-3.0 - 详见 [LICENSE](../LICENSE)

基于 [IINA](https://github.com/iina/iina)，由 lhc70000 及贡献者开发。

## 文档

- [功能对照表](parity-matrix.md) - macOS 与 Windows 功能对比
- [UI 设计决策](ui-decisions.md) - 保留 IINA 风格 vs 适配 Windows 原生
- [延期功能](deferred-features.md) - 未来里程碑规划
- [术语表](glossary.zh-CN.md) - 简体中文翻译术语规范
- [构建说明](build-instructions-windows.md) - 完整构建指南
