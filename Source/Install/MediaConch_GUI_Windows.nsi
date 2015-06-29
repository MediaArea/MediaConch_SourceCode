#NSIS: encoding=UTF-8
; Request application privileges for Windows Vista
RequestExecutionLevel admin

; Some defines
!define PRODUCT_NAME "MediaConch"
!define PRODUCT_PUBLISHER "MediaArea.net"
!define PRODUCT_VERSION "15.06"
!define PRODUCT_VERSION4 "${PRODUCT_VERSION}.0.0"
!define PRODUCT_WEB_SITE "http://MediaArea.net/MediaConch"
!define COMPANY_REGISTRY "Software\MediaArea.net"
!define PRODUCT_REGISTRY "Software\MediaArea.net\MediaConch"
!define PRODUCT_DIR_REGKEY "Software\Microsoft\Windows\CurrentVersion\App Paths\MediaConch.exe"
!define PRODUCT_UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME}"
!define PRODUCT_UNINST_ROOT_KEY "HKLM"

; Compression
SetCompressor /FINAL /SOLID lzma

; x64 stuff
!include "x64.nsh"

; MediaConch stuff
!include "MediaConch_Extensions.nsh"

; File size
!include FileFunc.nsh
!include WinVer.nsh

; Modern UI
!include "MUI2.nsh"
!define MUI_ABORTWARNING
!define MUI_ICON "..\..\Source\Resource\Image\MediaConch.ico"

; Language Selection Dialog Settings
!define MUI_LANGDLL_REGISTRY_ROOT "${PRODUCT_UNINST_ROOT_KEY}"
!define MUI_LANGDLL_REGISTRY_KEY "${PRODUCT_UNINST_KEY}"
!define MUI_LANGDLL_REGISTRY_VALUENAME "NSIS:Language"

; Installer pages
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
; !define MUI_FINISHPAGE_RUN "$INSTDIR\MediaConch.exe" //Removing it because it is run in admin privileges
!define MUI_WELCOMEFINISHPAGE_BITMAP "..\..\Source\Resource\Image\Windows_Finish.bmp"
!insertmacro MUI_PAGE_FINISH
; Uninstaller pages
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

; Language files
!insertmacro MUI_LANGUAGE "Arabic"
!insertmacro MUI_LANGUAGE "Albanian"
!insertmacro MUI_LANGUAGE "Belarusian"
!insertmacro MUI_LANGUAGE "Catalan"
!insertmacro MUI_LANGUAGE "Croatian"
!insertmacro MUI_LANGUAGE "Czech"
!insertmacro MUI_LANGUAGE "Danish"
!insertmacro MUI_LANGUAGE "Dutch"
!insertmacro MUI_LANGUAGE "English"
!insertmacro MUI_LANGUAGE "Farsi"
!insertmacro MUI_LANGUAGE "French"
!insertmacro MUI_LANGUAGE "German"
!insertmacro MUI_LANGUAGE "Greek"
!insertmacro MUI_LANGUAGE "Korean"
!insertmacro MUI_LANGUAGE "Hungarian"
!insertmacro MUI_LANGUAGE "Indonesian"
!insertmacro MUI_LANGUAGE "Italian"
!insertmacro MUI_LANGUAGE "Japanese"
!insertmacro MUI_LANGUAGE "Lithuanian"
!insertmacro MUI_LANGUAGE "Polish"
!insertmacro MUI_LANGUAGE "Portuguese"
!insertmacro MUI_LANGUAGE "PortugueseBR"
!insertmacro MUI_LANGUAGE "Romanian"
!insertmacro MUI_LANGUAGE "Russian"
!insertmacro MUI_LANGUAGE "SimpChinese"
!insertmacro MUI_LANGUAGE "Spanish"
!insertmacro MUI_LANGUAGE "Swedish"
!insertmacro MUI_LANGUAGE "Thai"
!insertmacro MUI_LANGUAGE "TradChinese"
!insertmacro MUI_LANGUAGE "Turkish"
!insertmacro MUI_LANGUAGE "Ukrainian"
!insertmacro MUI_RESERVEFILE_LANGDLL

; Info
VIProductVersion "${PRODUCT_VERSION4}"
VIAddVersionKey "CompanyName"      "${PRODUCT_PUBLISHER}"
VIAddVersionKey "ProductName"      "${PRODUCT_NAME}"
VIAddVersionKey "ProductVersion"   "${PRODUCT_VERSION4}"
VIAddVersionKey "FileDescription"  "Implementation checker, policy checker, reporter, and fixer"
VIAddVersionKey "FileVersion"      "${PRODUCT_VERSION4}"
VIAddVersionKey "LegalCopyright"   "${PRODUCT_PUBLISHER}"
VIAddVersionKey "OriginalFilename" "${PRODUCT_NAME}_GUI_${PRODUCT_VERSION}_Windows.exe"
BrandingText " "

; Modern UI end

Name "${PRODUCT_NAME} ${PRODUCT_VERSION}"
OutFile "..\..\Release\${PRODUCT_NAME}_GUI_${PRODUCT_VERSION}_Windows.exe"
InstallDir "$PROGRAMFILES64\${PRODUCT_NAME}"
InstallDirRegKey HKLM "${PRODUCT_DIR_REGKEY}" ""
ShowInstDetails nevershow
ShowUnInstDetails nevershow

Function .onInit
  ${If} ${RunningX64}
    SetRegView 64
  ${EndIf}
  !insertmacro MUI_LANGDLL_DISPLAY
FunctionEnd

Section "SectionPrincipale" SEC01
  SetOverwrite on
  SetOutPath "$SMPROGRAMS"
  CreateShortCut "$SMPROGRAMS\MediaConch.lnk" "$INSTDIR\MediaConch.exe" "" "" "" "" "" "Convenient unified display of the most relevant technical and tag data for video and audio files"
  SetOutPath "$INSTDIR"
  File "/oname=MediaConch.exe" "..\..\Project\MSVC2013\GUI\Win32\Release\MediaConch_GUI.exe"
  File "/oname=History.txt" "..\..\History_GUI.txt"
  File "..\..\License.html"
  File "/oname=ReadMe.txt" "..\..\Release\ReadMe_GUI_Windows.txt"

  # Create files
  WriteIniStr "$INSTDIR\${PRODUCT_NAME}.url" "InternetShortcut" "URL" "${PRODUCT_WEB_SITE}"

  # Delete files that might be present from older installation
  Delete "$INSTDIR\History_GUI.txt"
  Delete "$INSTDIR\Licence.txt"
  Delete "$INSTDIR\Licence.html"
  Delete "$INSTDIR\License.txt"
  Delete "$INSTDIR\ReadMe_Windows.txt"
  Delete "$SMPROGRAMS\MediaConch\Website.lnk"
  Delete "$SMPROGRAMS\MediaConch\Uninstall.lnk"
  Delete "$SMPROGRAMS\MediaConch\History.lnk"
  Delete "$SMPROGRAMS\MediaConch\MediaConch.lnk"
  RMDir  "$SMPROGRAMS\MediaConch"
SectionEnd

Section -Post
  WriteUninstaller "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "${PRODUCT_DIR_REGKEY}" "" "$INSTDIR\MediaConch.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayName"     "$(^Name)"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "Publisher"       "${PRODUCT_PUBLISHER}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayIcon"     "$INSTDIR\MediaConch.exe"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "DisplayVersion"  "${PRODUCT_VERSION}"
  WriteRegStr ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "URLInfoAbout"    "${PRODUCT_WEB_SITE}"
  !insertmacro MediaConch_Extensions_Install

  ${If} ${AtLeastWin7}
    ${GetSize} "$INSTDIR" "/S=0K" $0 $1 $2
    IntFmt $0 "0x%08X" $0 ; Convert the decimal KB value in $0 to DWORD, put it right back into $0
    WriteRegDWORD ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}" "EstimatedSize" "$0" ; Create/Write the reg key with the dword value
  ${EndIf}
SectionEnd


Section Uninstall
  SetRegView 64
  !insertmacro MediaConch_Extensions_Uninstall
  Exec 'regsvr32 "$INSTDIR\MediaConch_InfoTip.dll" /u /s'
  Sleep 3000

  Delete "$INSTDIR\${PRODUCT_NAME}.url"
  Delete "$INSTDIR\uninst.exe"
  Delete "$INSTDIR\MediaConch.exe"
  Delete "$INSTDIR\History.txt"
  Delete "$INSTDIR\License.html"
  Delete "$INSTDIR\ReadMe.txt"
  Delete "$SMPROGRAMS\MediaConch\Website.lnk"
  Delete "$SMPROGRAMS\MediaConch\MediaConch.lnk"
  Delete "$SMPROGRAMS\MediaConch\History.lnk"
  Delete "$SMPROGRAMS\MediaConch.lnk"

  RMDir "$SMPROGRAMS\MediaConch"
  RMDir "$INSTDIR"

  SetRegView 64
  DeleteRegKey HKLM "${PRODUCT_REGISTRY}"
  DeleteRegKey /ifempty HKLM "${COMPANY_REGISTRY}"
  DeleteRegKey HKCU "${PRODUCT_REGISTRY}"
  DeleteRegKey /ifempty HKCU "${COMPANY_REGISTRY}"
  DeleteRegKey ${PRODUCT_UNINST_ROOT_KEY} "${PRODUCT_UNINST_KEY}"
  DeleteRegKey HKLM "${PRODUCT_DIR_REGKEY}"
  SetAutoClose true
SectionEnd
