@echo OFF

set REPODIR=%~dp0
set SLN=%REPODIR%/Fabric3dsmax.sln

IF NOT DEFINED FABRIC_DIR (
  SET FABRIC_DIR=%REPODIR%..\..\..\stage\Windows\x86_64\Release\
  ECHO Setting Fabric at %FABRIC_DIR%
  SET FABRIC_EXTS_PATH=%FABRIC_EXTS_PATH%;%FABRIC_DIR%\Exts
) ELSE ECHO Fabric found at %FABRIC_DIR%

echo Allow the system to load FabricCore dll
SET PATH=%FABRIC_DIR%/lib;%PATH%

set DISTDIR=%FABRIC_DIR%\DCCIntegrations\Fabric3dsmax2014\
set SCINTILLANETDIR=%DISTDIR%ScintillaNET\

echo Starting VS Launcher
call "C:\Program Files (x86)\Common Files\microsoft shared\MSEnv\VSLauncher.exe" "%SLN%"


echo on
