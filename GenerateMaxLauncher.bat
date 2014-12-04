

@echo off

set TARGETDIR=%1
set MAXVERSION=%~2

echo %TARGETDIR%
echo %MAXVERSION%

if EXIST "%TARGETDIR%3dsmax%MAXVERSION%.bat" ( del "%TARGETDIR%3dsmax%MAXVERSION%.bat" )

@echo @echo off>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo.>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo if NOT EXIST Generated.Plugin.Splice.ini (>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo 	REM To allow users to run this file, and load splice without installing>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo 	REM anything, we generate a new ini file and pass it to Max to override>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo 	REM its current plugin directories setting.  We recreate the default>> "%TARGETDIR%3dsmax%MAXVERSION%.bat" 
@echo 	REM plugin ini file so hopefully nothing changes too much.  If you have a non>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo 	REM default plugin path settings, remove the -p argument to max at the end of >> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo 	REM this script and add the path to the 3dsMax>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo 	echo Generating new plugin ini file.>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo.>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo 	REM add in original plugins>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo 	@echo [Directories] ^>^> Generated.Plugin.Splice.ini>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo 	@echo Additional MAX plug-ins=%%ADSK_3DSMAX_x64_%MAXVERSION%%%\PlugIns\ ^>^> Generated.Plugin.Splice.ini>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo 	@echo Splice Plugins=%%~dp0plugin\ ^>^> Generated.Plugin.Splice.ini>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo 	@echo [Help] ^>^> Generated.Plugin.Splice.ini>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo 	@echo mental ray Help=http://www.autodesk.com/mentalray-help-2013-enu ^>^> Generated.Plugin.Splice.ini>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo )>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo.>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo set SPLICE3DSMAXDIR=%%~dp0>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo.>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo REM Ensure that the Splice plugin will be able to find the extensions >> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo set FABRIC_EXTS_PATH=%%FABRIC_EXTS_PATH%%;%%SPLICE3DSMAXDIR%%..\..\Exts>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo.>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo REM Allow the system to load FabricCore dll>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo set PATH=%%PATH%%;%%SPLICE3DSMAXDIR%%..\..\lib>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo.>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo REM Save the Scintilla folder to an environment variable that will be accessed in MaxScript>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo set SCINTILLANETDIR=%%SPLICE3DSMAXDIR%%ScintillaNET\>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo.>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo REM Launch Max>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo call "%%ADSK_3DSMAX_x64_%MAXVERSION%%%\3dsmax.exe" -vo -p %%SPLICE3DSMAXDIR%%/Generated.Plugin.Splice.ini>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo.>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"
@echo rem echo on>> "%TARGETDIR%3dsmax%MAXVERSION%.bat"

pause.