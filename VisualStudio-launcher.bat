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

SET FABRIC_SCENE_GRAPH_DIR=%REPODIR%..\..\..\

SET QT_DIR=%FABRIC_SCENE_GRAPH_DIR%\ThirdParty\PreBuilt\Windows\x86_64\VS2013\Release\qt\4.8.7
SET QTWINMIGRATE_DIR=%FABRIC_SCENE_GRAPH_DIR%\ThirdParty\PreBuilt\Windows\x86_64\VS2013\Release\qt-solutions\qtwinmigrate

echo Starting VS Launcher
call "C:\Program Files (x86)\Common Files\microsoft shared\MSEnv\VSLauncher.exe" "%SLN%"

echo on
