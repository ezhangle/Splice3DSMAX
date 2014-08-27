

@echo on


set CURRDIR=%~dp0

rem Ensure that the Splice plugin will be able to find the extensions 
set FABRIC_EXTS_PATH=%FABRIC_EXTS_PATH%;%CURRDIR%..\..\Exts

rem Save the Scintilla folder to an environment variable that will be accessed in MaxScript
set SCINTILLANETDIR=%CURRDIR%ScintillaNET\

rem Launch Max
call "D:\Program Files\Autodesk\3ds Max 2014\3dsmax.exe"

echo on