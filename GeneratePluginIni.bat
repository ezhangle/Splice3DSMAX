

@echo off

set CURRDIR=%~dp0
set TARGETDIR=%CURRDIR%build\
set MAXVERSION=%~2

REM This file is now mostly obsolete, and is only used to generate
REM a plugin ini in VS post-build step so we have a correct ini
REM file when starting debugging 

echo Generating new plugin ini file @ %TARGETDIR%

SET MAX_ENV_VAR=ADSK_3DSMAX_x64_%MAXVERSION%
CALL SET MAX_PATH=%%%MAX_ENV_VAR%%%

del "%TARGETDIR%Generated.Plugin.Fabric.ini"
REM add in original plugins
@echo [Directories] >> "%TARGETDIR%Generated.Plugin.Fabric.ini"
@echo Additional MAX plug-ins=%MAX_PATH%plugins >> "%TARGETDIR%Generated.Plugin.Fabric.ini"
@echo Fabric Plugins=%~1 >> "%TARGETDIR%Generated.Plugin.Fabric.ini"
@echo [Help] >> "%TARGETDIR%Generated.Plugin.Fabric.ini"
@echo mental ray Help=http://www.autodesk.com/mentalray-help-2013-enu >> "%TARGETDIR%Generated.Plugin.Fabric.ini"

rem echo on
