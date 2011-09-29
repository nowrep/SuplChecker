RequestExecutionLevel admin
SetCompressor /SOLID /FINAL lzma

!define PRODUCT_NAME "SuplChecker"
!define /date PRODUCT_VERSION "0.8.0"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\suplchecker.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

!include "MUI.nsh"
!define MUI_ABORTWARNING
!define MUI_ICON "wininstall\install.ico"
!define MUI_UNICON "wininstall\uninstall.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "wininstall\welcome.bmp"
!define MUI_UNWELCOMEFINISHPAGE_BITMAP "wininstall\welcome.bmp"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE COPYRIGHT.txt
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES

!define MUI_FINISHPAGE_RUN "$INSTDIR\suplchecker.exe"
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "Czech"

!insertmacro MUI_RESERVEFILE_LANGDLL

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "${PRODUCT_NAME} ${PRODUCT_VERSION} Installer.exe"
InstallDir "$PROGRAMFILES\${PRODUCT_NAME}\"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails show
ShowUnInstDetails show

Section "!Hlavní komponenty" SecMain
  SectionIn RO
  KillProcDLL::KillProc "suplchecker.exe"
  Sleep 100
  SetOverwrite on

  SetOutPath "$INSTDIR"
  File "COPYRIGHT.txt"
  File "suplchecker.exe"
  File "msvcp100.dll"
  File "msvcr100.dll"
  File "phonon4.dll"
  File "QtCore4.dll"
  File "QtGui4.dll"
  File "QtNetwork4.dll"
  File "QtWebKit4.dll"

  SetOutPath "$INSTDIR\data"
  File "data\qt_cs.qm"

  SetOutPath "$INSTDIR\imageformats"
  File "imageformats\qgif4.dll"
  File "imageformats\qjpeg4.dll"

SectionEnd

Section "-StartMenu"
  SetOutPath "$INSTDIR"
  SetShellVarContext all
  CreateDirectory "$SMPROGRAMS\SuplChecker"
  CreateShortCut "$SMPROGRAMS\SuplChecker\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  CreateShortCut "$SMPROGRAMS\SuplChecker\SuplChecker.lnk" "$INSTDIR\suplchecker.exe"
SectionEnd

Section "Zástupce na ploše" SecDesktop
  SetOutPath "$INSTDIR"
  CreateShortCut "$DESKTOP\SuplChecker.lnk" "$INSTDIR\suplchecker.exe" "" 
SectionEnd

Section "-Uninstaller"
  WriteUninstaller "$INSTDIR\uninstall.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\suplchecker.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName" "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon" "$INSTDIR\suplchecker.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion" "${PRODUCT_VERSION}"
SectionEnd

Section Uninstall
  KillProcDLL::KillProc "suplchecker.exe"
  Sleep 100

  SetShellVarContext all
  Delete "$DESKTOP\SuplChecker.lnk"
  RMDir /r "$INSTDIR"
  RMDir /r "$SMPROGRAMS\SuplChecker"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
SectionEnd

BrandingText "${PRODUCT_NAME} ${PRODUCT_VERSION} Installer"
