@echo on

set REPODIR=%~dp0
set SLN=%REPODIR%/FabricSplice3dsmax.sln
set SGREPODIR=%REPODIR%\..\..\..
set STAGEDIR=%REPODIR%\..\..\..\stage\Windows\x86_64\Release

rem We only need the Horde ext in addition to the Splice Exts. 
set FABRIC_SPLICE_API_DIR=%STAGEDIR%\Splice\API\FabricSpliceAPI


call %SGREPODIR%\Environment.bat

call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe" %SLN%

pause.

echo on
