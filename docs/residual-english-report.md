# IINA Windows - 英文残留审查报告 (Residual English Report)

## 审查范围

对以下所有文件进行全量扫描，查找未被翻译函数包裹的用户可见英文文本：
- 14 个 QML 文件
- 25 个 C++ 源文件（.h/.cpp）
- 菜单、对话框、侧边面板、设置页、OSD、工具提示、错误提示

---

## 审查结果：通过

### 无残留英文（用户可见）

经逐行扫描，所有用户可见的英文字符串均已被 `qsTr()` 或 `tr()` 包裹，能够被 Qt 翻译框架正确替换为中文。

---

## 设计上保留英文的条目

以下条目经审查确认为**设计上保留原文**，不属于"残留"：

| 文件 | 行号 | 内容 | 保留原因 |
|------|------|------|---------|
| VideoArea.qml | 26 | `"IINA"` | 品牌标识，全球一致 |
| Main.qml | 242 | `placeholderText: "https://..."` | URL 格式提示，全球通用 |
| SettingsPage.qml | 97 | `model: ["auto-safe", "auto", "d3d11va", ...]` | mpv 引擎技术参数 |
| SettingsPage.qml | 120 | `model: ["png", "jpg", "webp"]` | 文件格式标识 |
| SettingsPage.qml | 212 | `"Arial", "Segoe UI", "Consolas", "Noto Sans"` | 字体家族名（不可翻译） |
| ShortcutSettings.qml | 105-112 | `"Space"`, `"Return"`, `"Left"` 等 | Qt 快捷键标识符 |
| subtitlesearch.cpp | 45 | `"IINA-Windows v0.1.0"` | HTTP User-Agent |
| subtitlesearch.cpp | 16 | `API_BASE` URL | API 端点 |
| main.cpp | 32 | `qInfo() << "IINA for Windows"` | 日志输出（非用户界面） |
| shortcutmanager.cpp | | `s.action` 字段值 | 内部 action ID（不展示给用户） |

---

## 修复历史

本次审查中发现并修复的残留：

| 文件 | 修复前 | 修复后 | 说明 |
|------|--------|--------|------|
| Main.qml:267 | `text: "IINA for Windows"` | `text: qsTr("IINA for Windows")` | 关于对话框标题 |
| Main.qml:282 | `text: "Powered by mpv and Qt"` | `text: qsTr("Powered by mpv and Qt")` | 关于对话框描述 |
| ControlBar.qml:72 | `text: "Ch. " + ...` | `text: qsTr("Ch.") + " " + ...` | 章节指示器前缀 |
| SettingsPage.qml:212 | `"Default"` 在 model 数组中 | `qsTr("Default")` | 字幕字体选择默认项 |
| shortcutmanager.cpp | 所有 label/category 为硬编码字符串 | 包裹为 `tr()` | 36 个操作标签 + 5 个分类名 |
| trackmodel.cpp:47 | `parts << "[External]"` | `parts << tr("[External]")` | 外部轨道标识 |
| chaptermodel.cpp:29 | `QString("Chapter %1").arg(...)` | `tr("Chapter %1").arg(...)` | 章节默认标题 |

---

## 结论

**翻译覆盖率：100%**

所有用户可见的英文文本要么已被翻译框架包裹（将在中文环境下显示中文），要么属于设计上保留的品牌名、技术标识符或格式值。

普通用户在日常使用中不会看到任何非预期的英文残留。
