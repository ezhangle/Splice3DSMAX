@echo OFF

set REPODIR=%~dp0
set SLN=%REPODIR%/FabricSplice3dsmax.sln
set SGREPODIR=%REPODIR%\..\..\..

IF NOT DEFINED FABRIC_DIR (
  SET FABRIC_DIR=%REPODIR%\..\..\..\stage\Windows\x86_64\Release
  ECHO Setting Fabric at %FABRIC_DIR%
  SET FABRIC_EXTS_PATH=%FABRIC_EXTS_PATH%;%FABRIC_DIR%\Exts
)
else ECHO Fabric found at %FABRIC_PATH%

echo Allow the system to load FabricCore.1.13.dll
SET PATH=%FABRIC_DIR%/lib;%PATH%

IF NOT DEFINED BOOST_DIR SET BOOST_DIR=%SGREPODIR%\ThirdParty\PreBuilt\Windows\x86_64\Release\boost\1.55.0\lib

set DISTDIR=%FABRIC_DIR%\SpliceIntegrations\FabricSplice3dsmax2014
set SCINTILLANETDIR=%DISTDIR%ScintillaNET\

rem call %SGREPODIR%\Environment.bat

call "C:\Program Files (x86)\Microsoft Visual Studio 11.0\Common7\IDE\devenv.exe" "%SLN%"

pause.

echo on
