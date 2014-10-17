

@echo off

set CURRDIR=%~dp0
set TARGETDIR=%CURRDIR%build\

REM To allow users to run this file, and load splice without installing
REM anything, we generate a new ini file and pass it to Max to override
REM its current plugin directories setting.  We recreate the default
REM plugin ini file so hopefully nothing changes too much.  If you have a non
REM default plugin path settings, remove the -p argument to max at the end of 
REM this script and add the path to the 3dsMax
echo Generating new plugin ini file @ %TARGETDIR%

echo %0
del "%TARGETDIR%Generated.Plugin.Splice.ini"
REM add in original plugins
@echo [Directories] >> "%TARGETDIR%Generated.Plugin.Splice.ini"
@echo Additional MAX plug-ins=%ADSK_3DSMAX_x64_2014%\PlugIns\ >> "%TARGETDIR%Generated.Plugin.Splice.ini"
@echo Splice Plugins=%~1 >> "%TARGETDIR%Generated.Plugin.Splice.ini"
@echo [Help] >> "%TARGETDIR%Generated.Plugin.Splice.ini"
@echo mental ray Help=http://www.autodesk.com/mentalray-help-2013-enu >> "%TARGETDIR%Generated.Plugin.Splice.ini"

rem echo on
