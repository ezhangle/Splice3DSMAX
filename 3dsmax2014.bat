

@echo on


set CURRDIR=%~dp0
rem Ensure that the Splice plugin will be able to find the extensions 
set FABRIC_EXTS_PATH=%FABRIC_EXTS_PATH%;%CURRDIR%..\..\Exts

rem Ensure the Splice API folder and the Scintilla folder are included in the paths. 
set PATH=%PATH%;%CURRDIR%\ScintillaNET

call "D:\Program Files\Autodesk\3ds Max 2014\3dsmax.exe"

echo on