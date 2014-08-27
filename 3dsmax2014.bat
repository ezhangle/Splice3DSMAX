

@echo on


set REPODIR=%~dp0
set SLN=%REPODIR%/FabricSplice3dsmax.sln
set SCINTILLA=%REPODIR%/ScintillaNET
set SGREPODIR=%REPODIR%\..\..\..
set STAGEDIR=%REPODIR%\..\..\..\stage\Windows\x86_64\Release

call %SGREPODIR%\Environment.bat
set FABRIC_EXTS_PATH=%FABRIC_EXTS_PATH%;%STAGEDIR%\Splice\Exts

set PATH=%PATH%;%STAGEDIR%\Splice\API\FabricSpliceAPI;C:\Users\Phil\Projects\Splice\C++\FabricSplice3dsmax\ScintillaNET

call "D:\Program Files\Autodesk\3ds Max 2014\3dsmax.exe"

echo on