; Password Manager - InnoSetup Script
; Inno Setup Script

#define MyAppName "Password Manager"
#define MyAppVersion "1.0.0"
#define MyAppPublisher "Your Company"
#define MyAppURL "https://yourcompany.com"
#define MyAppExeName "PasswordManager.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application. Do not use the same AppId value in installers for other applications.
AppId={{B9F4F2F0-5C7A-4D1C-8A1D-F6F8C3B2F2F0}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={autopf}\{#MyAppName}
DefaultGroupName={#MyAppName}
AllowNoIcons=yes
; Uncomment the following line to run in non administrative install mode (install for current user only.)
;PrivilegesRequired=lowest
OutputDir=.
OutputBaseFilename=PasswordManager_Setup
Compression=lzma
SolidCompression=yes
WizardStyle=modern

; Code signing options - Uncomment and configure these lines when you have a code signing certificate
; SignTool=signtool sign /f "CertificatePath.pfx" /p YourPassword /t http://timestamp.digicert.com $f
; SignedUninstaller=yes
; SignedUninstallerDir={app}

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked

[Files]
; Main executable
Source: "..\..\build\Debug\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion

; Visual C++ Debug Runtime DLLs
Source: "debug_dlls\*.dll"; DestDir: "{app}"; Flags: ignoreversion

; Qt Core DLLs
Source: "..\..\build\Debug\Qt6Core.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build\Debug\Qt6Cored.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build\Debug\Qt6Guid.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build\Debug\Qt6Widgetsd.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build\Debug\Qt6Sql.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build\Debug\Qt6Sqld.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build\Debug\Qt6Networkd.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build\Debug\Qt6Svgd.dll"; DestDir: "{app}"; Flags: ignoreversion

; OpenSSL DLLs
Source: "..\..\build\Debug\libcrypto-3-x64.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build\Debug\libssl-3-x64.dll"; DestDir: "{app}"; Flags: ignoreversion

; Other DLLs
Source: "..\..\build\Debug\opengl32sw.dll"; DestDir: "{app}"; Flags: ignoreversion
Source: "..\..\build\Debug\D3Dcompiler_47.dll"; DestDir: "{app}"; Flags: ignoreversion

; Visual C++ Redistributable
Source: "vc_redist.x64.exe"; DestDir: "{tmp}"; Flags: deleteafterinstall

; Qt plugins
Source: "..\..\build\Debug\platforms\*"; DestDir: "{app}\platforms"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\build\Debug\sqldrivers\*"; DestDir: "{app}\sqldrivers"; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "..\..\build\Debug\imageformats\*"; DestDir: "{app}\imageformats"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist
Source: "..\..\build\Debug\styles\*"; DestDir: "{app}\styles"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist
Source: "..\..\build\Debug\iconengines\*"; DestDir: "{app}\iconengines"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist
Source: "..\..\build\Debug\tls\*"; DestDir: "{app}\tls"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist
Source: "..\..\build\Debug\translations\*"; DestDir: "{app}\translations"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist
Source: "..\..\build\Debug\generic\*"; DestDir: "{app}\generic"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist
Source: "..\..\build\Debug\networkinformation\*"; DestDir: "{app}\networkinformation"; Flags: ignoreversion recursesubdirs createallsubdirs skipifsourcedoesntexist

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon

[Run]
; Install Visual C++ Redistributable
Filename: "{tmp}\vc_redist.x64.exe"; Parameters: "/install /quiet /norestart"; StatusMsg: "Installing Visual C++ Redistributable..."; Flags: waituntilterminated

; Launch application
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#StringChange(MyAppName, '&', '&&')}}"; Flags: nowait postinstall skipifsilent
