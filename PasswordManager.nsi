; PasswordManager Installer Script
; NSIS (Nullsoft Scriptable Install System) script

!include "MUI2.nsh"
!include "FileFunc.nsh"

; Application information
!define PRODUCT_NAME "Password Manager"
!define PRODUCT_VERSION "1.0.0"
!define PRODUCT_PUBLISHER "Your Company"
!define PRODUCT_WEB_SITE "https://yourcompany.com"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\PasswordManager.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; MUI Settings
!define MUI_ABORTWARNING
; Varsayılan simgeleri kullan
!define MUI_ICON "${NSISDIR}\Contrib\Graphics\Icons\modern-install.ico"
!define MUI_UNICON "${NSISDIR}\Contrib\Graphics\Icons\modern-uninstall.ico"

; Welcome page
!insertmacro MUI_PAGE_WELCOME
; Directory page
!insertmacro MUI_PAGE_DIRECTORY
; Instfiles page
!insertmacro MUI_PAGE_INSTFILES
; Finish page
!define MUI_FINISHPAGE_RUN "$INSTDIR\PasswordManager.exe"
!insertmacro MUI_PAGE_FINISH

; Uninstaller pages
!insertmacro MUI_UNPAGE_INSTFILES

; Language files
!insertmacro MUI_LANGUAGE "English"

; Installer attributes
Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "PasswordManager_Setup.exe"
InstallDir "$PROGRAMFILES64\PasswordManager"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "MainSection" SEC01
  SetOutPath "$INSTDIR"
  SetOverwrite ifnewer
  
  ; Main executable
  File /nonfatal "..\..\build\Debug\PasswordManager.exe"
  
  ; DLL dosyaları
  File /nonfatal "..\..\build\Debug\Qt6Core.dll"
  File /nonfatal "..\..\build\Debug\Qt6Gui.dll"
  File /nonfatal "..\..\build\Debug\Qt6Widgets.dll"
  File /nonfatal "..\..\build\Debug\Qt6Sql.dll"
  File /nonfatal "..\..\build\Debug\libcrypto-3-x64.dll"
  File /nonfatal "..\..\build\Debug\libssl-3-x64.dll"
  File /nonfatal "..\..\build\Debug\opengl32sw.dll"
  File /nonfatal "..\..\build\Debug\D3Dcompiler_47.dll"
  
  ; Alt dizinleri oluştur ve DLL'leri kopyala
  CreateDirectory "$INSTDIR\platforms"
  CreateDirectory "$INSTDIR\sqldrivers"
  
  SetOutPath "$INSTDIR\platforms"
  File /nonfatal "..\..\build\Debug\platforms\qwindows.dll"
  
  SetOutPath "$INSTDIR\sqldrivers"
  File /nonfatal "..\..\build\Debug\sqldrivers\qsqlite.dll"
  
  SetOutPath "$INSTDIR"
  
  ; Create shortcuts
  CreateDirectory "$SMPROGRAMS\Password Manager"
  CreateShortCut "$SMPROGRAMS\Password Manager\Password Manager.lnk" "$INSTDIR\PasswordManager.exe"
  CreateShortCut "$DESKTOP\Password Manager.lnk" "$INSTDIR\PasswordManager.exe"
  
  ; Register application
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\PasswordManager.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\PasswordManager.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout" "${PRODUCT_WEB_SITE}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher" "${PRODUCT_PUBLISHER}"
  
  ; Write uninstaller
  WriteUninstaller "$INSTDIR\uninst.exe"
SectionEnd

Section -Post
  ; Get installation size
  ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
  IntFmt $0 "0x%08X" $0
  WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "EstimatedSize" "$0"
SectionEnd

; Uninstaller section
Section Uninstall
  ; Remove shortcuts, if any
  Delete "$SMPROGRAMS\Password Manager\Password Manager.lnk"
  Delete "$DESKTOP\Password Manager.lnk"
  RMDir "$SMPROGRAMS\Password Manager"
  
  ; Remove files and uninstaller
  Delete "$INSTDIR\PasswordManager.exe"
  Delete "$INSTDIR\*.dll"
  Delete "$INSTDIR\platforms\*.dll"
  Delete "$INSTDIR\sqldrivers\*.dll"
  Delete "$INSTDIR\uninst.exe"
  
  ; Remove directories used
  RMDir "$INSTDIR\platforms"
  RMDir "$INSTDIR\sqldrivers"
  RMDir "$INSTDIR"
  
  ; Remove registry keys
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd 