@echo on

set REPODIR=%~dp0
set SLN=%REPODIR%/FabricSplice3dsmax.sln
set SGREPODIR=%REPODIR%\..\..\..
set STAGEDIR=%REPODIR%\..\..\..\stage\Windows\x86_64\Release
set FABRIC_SPLICE_API_DIR=%STAGEDIR%\Splice\API\FabricSpliceAPI
set BOOST_DIR=%SGREPODIR%\ThirdParty\PreBuilt\Windows\x86_64\Release\boost\1.55.0\lib


call %SGREPODIR%\Environment.bat

call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe" %SLN%

pause.

echo on
