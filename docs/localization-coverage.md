# IINA Windows - 本地化覆盖报告 (Localization Coverage)

## 概述

| 指标 | 数值 |
|------|------|
| 翻译目标语言 | 简体中文 (zh_CN) |
| QML qsTr() 字符串总数 | 128 |
| C++ tr() 字符串总数 | 44 |
| 已翻译字符串 | 172 |
| 未翻译（保留原文）字符串 | 0 |
| 翻译覆盖率 | **100%** |

---

## 字符串来源分布

### QML 文件 (14 个文件，128 个字符串)

| 文件 | 字符串数 | 覆盖率 | 说明 |
|------|---------|--------|------|
| Main.qml | 52 | 100% | 菜单栏、对话框、快捷键、上下文菜单、OSD |
| ControlBar.qml | 10 | 100% | 工具提示、章节指示器 |
| SettingsPage.qml | 32 | 100% | 四个标签页所有标签和分组 |
| ShortcutSettings.qml | 5 | 100% | 快捷键配置对话框 |
| PlaylistPanel.qml | 7 | 100% | 播放列表面板 |
| ChapterPanel.qml | 3 | 100% | 章节面板 |
| HistoryPanel.qml | 6 | 100% | 历史记录面板 |
| FilterPanel.qml | 8 | 100% | 视频滤镜对话框 |
| OSD.qml | 3 | 100% | 屏幕提示 |
| VolumeControl.qml | 2 | 100% | 静音/取消静音 |
| VideoArea.qml | 1 | 100% | 空闲提示文本 |
| SubtitleMenu.qml | 3 | 100% | 字幕选择菜单 |
| SeekBar.qml | 0 | N/A | 无用户可见文本 |
| TitleBar.qml | 0 | N/A | 无用户可见文本 |

### C++ 文件 (5 个文件，44 个字符串)

| 文件 | 字符串数 | 覆盖率 | 说明 |
|------|---------|--------|------|
| shortcutmanager.cpp | 36 | 100% | 33 个操作标签 + 5 个分类名 |
| historymodel.cpp | 4 | 100% | 时间相对描述 |
| subtitlesearch.cpp | 2 | 100% | 错误提示 |
| trackmodel.cpp | 1 | 100% | 外部轨道标识 "[External]" |
| chaptermodel.cpp | 1 | 100% | 章节默认标题 "Chapter %1" |

---

## 不翻译的内容（设计决定）

以下内容按设计保留英文/原文：

| 内容 | 原因 |
|------|------|
| "IINA" 品牌标识 | 产品名不翻译 |
| 快捷键标识（Ctrl、Shift、Space 等） | Qt 系统标识符 |
| 文件扩展名（*.mp4、*.srt 等） | 技术标识符 |
| 硬件解码选项值（auto-safe、d3d11va 等） | mpv 配置值 |
| 截图格式（png、jpg、webp） | 格式标识符 |
| 字体名（Arial、Segoe UI 等） | 字体家族名 |
| URL placeholder（https://...） | 通用格式提示 |
| "GPL-3.0" | 许可证标识 |
| 用户代理字符串 "IINA-Windows v0.1.0" | 技术标识 |

---

## 翻译基础设施

| 组件 | 状态 |
|------|------|
| Qt .ts 文件 | `translations/iina_zh_CN.ts` 已创建 |
| CMake LinguistTools | 已集成，`qt_add_translations()` |
| main.cpp 翻译加载 | QTranslator 根据系统 locale 自动加载 |
| .qm 资源嵌入 | 编译时嵌入 Qt 资源系统 |
| 文件系统回退 | 支持从 translations/ 目录加载 |
| qsTr() 覆盖 | 所有 QML 用户可见字符串已包裹 |
| tr() 覆盖 | 所有 C++ 用户可见字符串已包裹 |

---

## 新增语言流程

要添加新语言（如 ja_JP），只需：

1. 复制 `translations/iina_zh_CN.ts` 为 `translations/iina_ja_JP.ts`
2. 修改 `<TS>` 标签的 `language` 属性为 `ja_JP`
3. 翻译所有 `<translation>` 标签内容
4. 在 `CMakeLists.txt` 的 `TS_FILES` 中添加新文件
5. 重新编译即可
