

@echo off

if NOT EXIST Generated.Plugin.Splice.ini (
	REM To allow users to run this file, and load splice without installing
	REM anything, we generate a new ini file and pass it to Max to override
	REM its current plugin directories setting.  We recreate the default
	REM plugin ini file so hopefully nothing changes too much.  If you have a non
	REM default plugin path settings, remove the -p argument to max at the end of 
	REM this script and add the path to the 3dsMax
	echo Generating new plugin ini file.

	REM add in original plugins
	@echo [Directories] >> Generated.Plugin.Splice.ini
	@echo Additional MAX plug-ins=%ADSK_3DSMAX_x64_2014%\PlugIns\ >> Generated.Plugin.Splice.ini
	@echo Splice Plugins=%~dp0plugin\ >> Generated.Plugin.Splice.ini
	@echo [Help] >> Generated.Plugin.Splice.ini
	@echo mental ray Help=http://www.autodesk.com/mentalray-help-2013-enu >> Generated.Plugin.Splice.ini
)

set SPLICE3DSMAXDIR=%~dp0

echo Ensure that the Splice plugin will be able to find the extensions 
set FABRIC_EXTS_PATH=%FABRIC_EXTS_PATH%;%SPLICE3DSMAXDIR%..\..\Exts

echo Allow the system to load FabricCore dll
set PATH=%PATH%;%SPLICE3DSMAXDIR%..\..\lib

echo Save the Scintilla folder to an environment variable that will be accessed in MaxScript
set SCINTILLANETDIR=%SPLICE3DSMAXDIR%ScintillaNET\

rem Launch Max
call "%ADSK_3DSMAX_x64_2014%\3dsmax.exe" -vo -p %~dp0/Generated.Plugin.Splice.ini

rem echo on