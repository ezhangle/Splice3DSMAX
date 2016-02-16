setlocal  enableextensions enabledelayedexpansion
@echo off
IF NOT DEFINED QT_DIR (
    ECHO ERROR: QT_DIR not set.  Please ensure QT_DIR is set or nmake is on the PATH
    GOTO ERROR_EXIT
)

SET PATH=%PATH%;%QT_DIR%\bin
WHERE qmake > NUL 2>&1
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: qmake not found.  Please ensure QT_DIR is set or nmake is on the PATH
    GOTO ERROR_EXIT
)
ECHO -- Found qmake... --

WHERE nmake > NUL 2>&1
IF %ERRORLEVEL% NEQ 0 (
    ECHO ERROR: nmake not found.  Please run this script from a Visual Studio Command Prompt
    GOTO ERROR_EXIT
)
for /f "tokens=*" %%i in ('WHERE nmake') do set NMAKE_PATH=%%i
If "%NMAKE_PATH%"=="%NMAKE_PATH:amd64=%" (
    echo ERROR: Win32 nmake detected.  We only support x64 versions, please start the 2013 x64 VS Command Prompt 
    GOTO ERROR_EXIT
)
If "%NMAKE_PATH%"=="%NMAKE_PATH:12.0=%" (
    echo ERROR: We require the VS2013 version of nmake.  We only support x64 versions, please start the 2013 x64 VS Command Prompt 
    GOTO ERROR_EXIT
)
ECHO -- Found nmake... --

REM Enforce VS 2013 (at least for now)
SET QMAKESPEC=%QT_DIR%\mkspecs\win32-msvc2013

REM --------------------------------------------------
REM Get around Qt's rediculous hard-coded binary paths
REM --------------------------------------------------

SET _QT_CONF_F=%QT_DIR%\bin\qt.conf
SET QT_DIR_FWDSLASH=%QT_DIR:\=/%
IF NOT EXIST %_QT_CONF_F% (
    echo  -- Generating new qt.conf file @ %_QT_CONF_F% -- 

    @echo [Paths] >> %_QT_CONF_F%
    @echo Prefix = %QT_DIR_FWDSLASH% >> %_QT_CONF_F%
)

cd "%~dp0qt-solutions\qtwinmigrate"
ECHO -- Building Config --
call configure -library

ECHO -- Cleaning old build files --
REM remove any old build files
IF EXIST Makefile (
    nmake distclean
)

REM create new build config
qmake
IF %ERRORLEVEL% NEQ 0 (
  ECHO ERROR: qmake exitted with non-0 status
  goto ERROR_EXIT
)
nmake
IF %ERRORLEVEL% NEQ 0 (
  ECHO ERROR: nmake exitted with non-0 status
  goto ERROR_EXIT
)

echo -- Build Successful --
:ERROR_EXIT
pause
:EOF
endlocal
