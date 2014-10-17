@echo on

set REPODIR=%~dp0
set SLN=%REPODIR%/FabricSplice3dsmax.sln
set SGREPODIR=%REPODIR%\..\..\..
set FABRIC_DIR=%REPODIR%\..\..\..\stage\Windows\x86_64\Release
set BOOST_DIR=%SGREPODIR%\ThirdParty\PreBuilt\Windows\x86_64\Release\boost\1.55.0\lib

set DISTDIR=%FABRIC_DIR%\SpliceIntegrations\FabricSplice3dsmax2014

echo Allow the system to load FabricCore.1.13.dll
set PATH=%FABRIC_LIB_DIR%;%PATH%

set FABRIC_EXTS_PATH=%FABRIC_EXTS_PATH%;%FABRIC_DIR%\Exts
set SCINTILLANETDIR=%DISTDIR%ScintillaNET\

call %SGREPODIR%\Environment.bat

call "C:\Program Files (x86)\Microsoft Visual Studio 10.0\Common7\IDE\devenv.exe" %SLN%

pause.

echo on
