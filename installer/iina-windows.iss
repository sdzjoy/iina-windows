; IINA for Windows - InnoSetup Installer Script
; Builds a Windows installer with file associations, shortcuts, and proper uninstallation

#define MyAppName "IINA"
#define MyAppVersion "0.1.0"
#define MyAppPublisher "IINA Contributors"
#define MyAppURL "https://iina.io"
#define MyAppExeName "iina-windows.exe"
#define MyAppGUID "{{8C5F8E4A-1B2C-4D3E-9F0A-1A2B3C4D5E6F}"

[Setup]
; Basic application information
AppId={#MyAppGUID}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
AppCopyright=Copyright (C) 2026 IINA Contributors
VersionInfoVersion={#MyAppVersion}
VersionInfoCompany={#MyAppPublisher}
VersionInfoDescription=Modern video player for Windows
VersionInfoProductName={#MyAppName}
VersionInfoProductVersion={#MyAppVersion}

; Installation paths
DefaultDirName={autopf}\IINA
DefaultGroupName=IINA
AllowNoIcons=yes
DisableProgramGroupPage=yes

; Licensing
LicenseFile=..\LICENSE
InfoBeforeFile=..\README.md

; Output
OutputDir=..\dist
OutputBaseFilename=IINA-Windows-{#MyAppVersion}-Setup
SetupIconFile=..\resources\icons\iina.ico
UninstallDisplayIcon={app}\{#MyAppExeName}

; Compression
Compression=lzma2/max
SolidCompression=yes
LZMAUseSeparateProcess=yes
LZMANumBlockThreads=2

; Install modes - support both per-user and per-machine
PrivilegesRequiredOverridesAllowed=dialog
ArchitecturesInstallIn64BitMode=x64compatible
ArchitecturesAllowed=x64compatible

; UI
WizardStyle=modern
DisableWelcomePage=no
DisableReadyPage=no
WindowResizable=yes

; Uninstaller
UninstallDisplayName={#MyAppName}
UninstallFilesDir={app}\uninst

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"
; Chinese Simplified requires ChineseSimplified.isl from Inno Setup Languages pack
; Uncomment the next line if the language file is available
;Name: "chinesesimplified"; MessagesFile: "compiler:Languages\ChineseSimplified.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"
Name: "quicklaunchicon"; Description: "{cm:CreateQuickLaunchIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
Name: "fileassoc"; Description: "Associate video file types with IINA"; GroupDescription: "File Associations:"; Flags: checkedonce
Name: "contextmenu"; Description: "Add ""Open with IINA"" to context menu"; GroupDescription: "Integration:"; Flags: checkedonce

[Files]
; Main executable
Source: "..\build\Release\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion

; Qt runtime DLLs (deployed by windeployqt)
Source: "..\build\Release\*.dll"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs

; Qt plugins and QML modules
Source: "..\build\Release\plugins\*"; DestDir: "{app}\plugins"; Flags: ignoreversion recursesubdirs
Source: "..\build\Release\qml\*"; DestDir: "{app}\qml"; Flags: ignoreversion recursesubdirs

; Translations
Source: "..\build\Release\translations\*"; DestDir: "{app}\translations"; Flags: ignoreversion recursesubdirs

; Resources
Source: "..\build\Release\resources\*"; DestDir: "{app}\resources"; Flags: ignoreversion recursesubdirs

; Documentation
Source: "..\LICENSE"; DestDir: "{app}"; DestName: "LICENSE.txt"; Flags: ignoreversion
Source: "..\README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\THIRD_PARTY_LICENSES.txt"; DestDir: "{app}"; Flags: ignoreversion

[Icons]
; Start Menu shortcuts
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"

; Desktop shortcut (optional task)
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

; Quick Launch shortcut (optional task)
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: quicklaunchicon

[Registry]
; File associations - video formats
Root: HKA; Subkey: "Software\Classes\.mp4\OpenWithProgids"; ValueType: string; ValueName: "IINA.VideoFile"; ValueData: ""; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\.mkv\OpenWithProgids"; ValueType: string; ValueName: "IINA.VideoFile"; ValueData: ""; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\.avi\OpenWithProgids"; ValueType: string; ValueName: "IINA.VideoFile"; ValueData: ""; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\.mov\OpenWithProgids"; ValueType: string; ValueName: "IINA.VideoFile"; ValueData: ""; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\.wmv\OpenWithProgids"; ValueType: string; ValueName: "IINA.VideoFile"; ValueData: ""; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\.flv\OpenWithProgids"; ValueType: string; ValueName: "IINA.VideoFile"; ValueData: ""; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\.webm\OpenWithProgids"; ValueType: string; ValueName: "IINA.VideoFile"; ValueData: ""; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\.m4v\OpenWithProgids"; ValueType: string; ValueName: "IINA.VideoFile"; ValueData: ""; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\.mpg\OpenWithProgids"; ValueType: string; ValueName: "IINA.VideoFile"; ValueData: ""; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\.mpeg\OpenWithProgids"; ValueType: string; ValueName: "IINA.VideoFile"; ValueData: ""; Flags: uninsdeletevalue; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\.ts\OpenWithProgids"; ValueType: string; ValueName: "IINA.VideoFile"; ValueData: ""; Flags: uninsdeletevalue; Tasks: fileassoc

; IINA.VideoFile ProgID
Root: HKA; Subkey: "Software\Classes\IINA.VideoFile"; ValueType: string; ValueName: ""; ValueData: "Video File"; Flags: uninsdeletekey; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\IINA.VideoFile\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Tasks: fileassoc
Root: HKA; Subkey: "Software\Classes\IINA.VideoFile\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: fileassoc

; Context menu - "Open with IINA"
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.mp4\shell\OpenWithIINA"; ValueType: string; ValueName: ""; ValueData: "Open with IINA"; Flags: uninsdeletekey; Tasks: contextmenu
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.mp4\shell\OpenWithIINA"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\{#MyAppExeName}"; Tasks: contextmenu
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.mp4\shell\OpenWithIINA\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: contextmenu

Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.mkv\shell\OpenWithIINA"; ValueType: string; ValueName: ""; ValueData: "Open with IINA"; Flags: uninsdeletekey; Tasks: contextmenu
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.mkv\shell\OpenWithIINA"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\{#MyAppExeName}"; Tasks: contextmenu
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.mkv\shell\OpenWithIINA\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: contextmenu

Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.avi\shell\OpenWithIINA"; ValueType: string; ValueName: ""; ValueData: "Open with IINA"; Flags: uninsdeletekey; Tasks: contextmenu
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.avi\shell\OpenWithIINA"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\{#MyAppExeName}"; Tasks: contextmenu
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.avi\shell\OpenWithIINA\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: contextmenu

Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.mov\shell\OpenWithIINA"; ValueType: string; ValueName: ""; ValueData: "Open with IINA"; Flags: uninsdeletekey; Tasks: contextmenu
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.mov\shell\OpenWithIINA"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\{#MyAppExeName}"; Tasks: contextmenu
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.mov\shell\OpenWithIINA\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: contextmenu

Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.wmv\shell\OpenWithIINA"; ValueType: string; ValueName: ""; ValueData: "Open with IINA"; Flags: uninsdeletekey; Tasks: contextmenu
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.wmv\shell\OpenWithIINA"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\{#MyAppExeName}"; Tasks: contextmenu
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.wmv\shell\OpenWithIINA\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: contextmenu

Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.webm\shell\OpenWithIINA"; ValueType: string; ValueName: ""; ValueData: "Open with IINA"; Flags: uninsdeletekey; Tasks: contextmenu
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.webm\shell\OpenWithIINA"; ValueType: string; ValueName: "Icon"; ValueData: "{app}\{#MyAppExeName}"; Tasks: contextmenu
Root: HKA; Subkey: "Software\Classes\SystemFileAssociations\.webm\shell\OpenWithIINA\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: contextmenu

; App registration for Windows 10+ "Open With" dialog
Root: HKA; Subkey: "Software\IINA\Capabilities"; ValueType: string; ValueName: "ApplicationName"; ValueData: "IINA"; Flags: uninsdeletekey
Root: HKA; Subkey: "Software\IINA\Capabilities"; ValueType: string; ValueName: "ApplicationDescription"; ValueData: "Modern video player for Windows"
Root: HKA; Subkey: "Software\IINA\Capabilities\FileAssociations"; ValueType: string; ValueName: ".mp4"; ValueData: "IINA.VideoFile"
Root: HKA; Subkey: "Software\IINA\Capabilities\FileAssociations"; ValueType: string; ValueName: ".mkv"; ValueData: "IINA.VideoFile"
Root: HKA; Subkey: "Software\IINA\Capabilities\FileAssociations"; ValueType: string; ValueName: ".avi"; ValueData: "IINA.VideoFile"
Root: HKA; Subkey: "Software\IINA\Capabilities\FileAssociations"; ValueType: string; ValueName: ".mov"; ValueData: "IINA.VideoFile"
Root: HKA; Subkey: "Software\IINA\Capabilities\FileAssociations"; ValueType: string; ValueName: ".wmv"; ValueData: "IINA.VideoFile"
Root: HKA; Subkey: "Software\IINA\Capabilities\FileAssociations"; ValueType: string; ValueName: ".flv"; ValueData: "IINA.VideoFile"
Root: HKA; Subkey: "Software\IINA\Capabilities\FileAssociations"; ValueType: string; ValueName: ".webm"; ValueData: "IINA.VideoFile"
Root: HKA; Subkey: "Software\IINA\Capabilities\FileAssociations"; ValueType: string; ValueName: ".m4v"; ValueData: "IINA.VideoFile"
Root: HKA; Subkey: "Software\IINA\Capabilities\FileAssociations"; ValueType: string; ValueName: ".mpg"; ValueData: "IINA.VideoFile"
Root: HKA; Subkey: "Software\IINA\Capabilities\FileAssociations"; ValueType: string; ValueName: ".mpeg"; ValueData: "IINA.VideoFile"
Root: HKA; Subkey: "Software\IINA\Capabilities\FileAssociations"; ValueType: string; ValueName: ".ts"; ValueData: "IINA.VideoFile"

Root: HKA; Subkey: "Software\RegisteredApplications"; ValueType: string; ValueName: "IINA"; ValueData: "Software\IINA\Capabilities"; Flags: uninsdeletevalue

[Run]
; Option to launch after installation
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent

[Code]
// Custom installation logic
procedure InitializeWizard();
begin
  // Could add custom pages here if needed
end;

function InitializeSetup(): Boolean;
begin
  Result := True;
  // Check for required dependencies (could check for VC++ runtime, etc.)
end;
