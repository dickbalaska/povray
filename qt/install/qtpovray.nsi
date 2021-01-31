;qtpovray NSIS Install Script
;Copyright (c) 2021 Dick Balaska and BuckoSoft

;--------------------------------
;Include Modern UI

!include "MUI2.nsh"

!define VERSION 3.80.15
!define	POVVERSION	3.8

;--------------------------------
;General

;Name and file
Name "qtpovray"
OutFile "qtpovray-${VERSION}.exe"
Unicode True
;SetCompressor LZMA

;Default installation folder
InstallDir "$PROGRAMFILES64\qtpovray"
  
;Get installation folder from registry if available
InstallDirRegKey HKCU "Software\qtpovray" ""

;Request application privileges for Windows Vista
RequestExecutionLevel admin

;--------------------------------
;Variables

	Var StartMenuFolder
	Var DataDir

;--------------------------------
;Interface Settings

	!define MUI_ABORTWARNING

;--------------------------------
;Pages

	!insertmacro MUI_PAGE_LICENSE "LICENSE"
	!insertmacro MUI_PAGE_COMPONENTS
	!insertmacro MUI_PAGE_DIRECTORY
  
	;Start Menu Folder Page Configuration
	!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
	!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\qtpovray" 
	!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
	!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
	!insertmacro MUI_PAGE_INSTFILES

	!define MUI_FINISHPAGE_RUN
	!define MUI_FINISHPAGE_RUN_FUNCTION LaunchQtpovray
	!define MUI_FINISHPAGE_RUN_TEXT "Launch qtpovray"
	!insertmacro MUI_PAGE_FINISH
  
	!insertmacro MUI_UNPAGE_CONFIRM
	!insertmacro MUI_UNPAGE_INSTFILES


;--------------------------------
;Languages
 
	!insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Program Files"

	SetOutPath "$INSTDIR"
	File qtpovray.exe
	File Qt5Core.dll
	File Qt5Gui.dll
	File Qt5Svg.dll
	File Qt5Widgets.dll
	File d3dcompiler_47.dll
	File libEGL.dll
	File libGLESv2.dll
	File opengl32sw.dll
	File vcredist_x64.exe

	SetOutPath "$INSTDIR\iconengines"
	File iconengines\qsvgicon.dll

	SetOutPath "$INSTDIR\imageformats"
	File imageformats\qgif.dll
	File imageformats\qicns.dll
	File imageformats\qico.dll
	File imageformats\qjpeg.dll
	File imageformats\qsvg.dll
	File imageformats\qtga.dll
	File imageformats\qtiff.dll
	File imageformats\qwbmp.dll
	File imageformats\qwebp.dll
  
	SetOutPath "$INSTDIR\platforms"
	File platforms\qwindows.dll
	
	SetOutPath "$INSTDIR\styles"
	File styles\qwindowsvistastyle.dll

	SetOutPath "$FONTS"
	File fonts\SourceCodePro\SourceCodePro-Bold.ttf  
	File fonts\SourceCodePro\SourceCodePro-It.ttf  
	File fonts\SourceCodePro\SourceCodePro-Regular.ttf 
	
	;Store installation folder
	WriteRegStr HKCU "Software\qtpovray" "" $INSTDIR
  
	;Create uninstaller
	WriteUninstaller "$INSTDIR\Uninstall.exe"
  
	!insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
	;Create shortcuts
	CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
	CreateShortcut "$SMPROGRAMS\$StartMenuFolder\qtpovray.lnk" "$INSTDIR\qtpovray.exe"
	CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
	!insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

Section "POV-Ray includes"
	StrCpy $DataDir $DOCUMENTS\qtpovray\v${POVVERSION}
	SetOutPath "$DataDir\include"
	!include "finclude.nsh"
SectionEnd

Section "Insert Menu items"
	StrCpy $DataDir $DOCUMENTS\qtpovray\v${POVVERSION}
	!include "finsert.nsh"
SectionEnd

Section "qtpovray Help"
	StrCpy $DataDir $DOCUMENTS\qtpovray\v${POVVERSION}
	SetOutPath "$DataDir\qtpovrayHelp"
	!include "fqthelp.nsh"
SectionEnd

Section "Sample scenes"
	StrCpy $DataDir $DOCUMENTS\qtpovray\v${POVVERSION}
	SetOutPath "$DataDir\scenes"
	!include "fscenes.nsh"
SectionEnd


;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

	Delete "$INSTDIR\qtpovray.exe"
	Delete "$INSTDIR\Qt5Core.dll"
	Delete "$INSTDIR\Qt5Gui.dll"
	Delete "$INSTDIR\Qt5Svg.dll"
	Delete "$INSTDIR\Qt5Widgets.dll"
	Delete "$INSTDIR\d3dcompiler_47.dll"
	Delete "$INSTDIR\libEGL.dll"
	Delete "$INSTDIR\libGLESv2.dll"
	Delete "$INSTDIR\opengl32sw.dll"
	Delete "$INSTDIR\vcredist_x64.exe"

	Delete "$INSTDIR\iconengines\qsvgicon.dll"
	RMDir  "$INSTDIR\iconengines"

	Delete "$INSTDIR\imageformats\qgif.dll"
	Delete "$INSTDIR\imageformats\qicns.dll"
	Delete "$INSTDIR\imageformats\qico.dll"
	Delete "$INSTDIR\imageformats\qjpeg.dll"
	Delete "$INSTDIR\imageformats\qsvg.dll"
	Delete "$INSTDIR\imageformats\qtga.dll"
	Delete "$INSTDIR\imageformats\qtiff.dll"
	Delete "$INSTDIR\imageformats\qwbmp.dll"
	Delete "$INSTDIR\imageformats\qwebp.dll"
	RMDir  "$INSTDIR\imageformats"
  
	Delete "$INSTDIR\platforms\qwindows.dll"
	RMDir  "$INSTDIR\platforms"
	
	Delete "$INSTDIR\styles\qwindowsvistastyle.dll"
	RMDir  "$INSTDIR\styles"


	StrCpy $DataDir $DOCUMENTS\qtpovray\v${POVVERSION}
	!include "fincludeUn.nsh"
	RMDir "$DataDir\include"

	!include "finsertUn.nsh"
	RMDir "$DataDir\Insert Menu"
	!include "fqthelpUn.nsh"
	RMDir "$DataDir\qtpovrayHelp"
	!include "fscenesUn.nsh"
	RMDir "$DataDir\scenes"

	RMDir "$DataDir"
	RMDir "$DOCUMENTS\qtpovray"

	Delete "$INSTDIR\Uninstall.exe"

	RMDir "$INSTDIR"
  

	!insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
	Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
	Delete "$SMPROGRAMS\$StartMenuFolder\qtpovray.lnk"
	RMDir "$SMPROGRAMS\$StartMenuFolder"
  
	DeleteRegKey /ifempty HKCU "Software\qtpovray"

SectionEnd

Function LaunchQtpovray
	SetOutPath $INSTDIR
	#ShellExecAsUser::ShellExecAsUser "" "$INSTDIR/qtpovray.exe"
	Exec '"$WINDIR\explorer.exe" "$INSTDIR\qtpovray.exe"'
FunctionEnd
