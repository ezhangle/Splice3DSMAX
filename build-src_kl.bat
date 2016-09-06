setlocal

SET MAX_DIR=%~dp0
REM It seems KL2EDK no longer works with non-local folders...
cd "%FABRIC_DIR%\Exts\Builtin\Math"
mkdir h
kl2edk Math.fpm.json -o h
rd /s /q "%MAX_DIR%\src_kl\"
mkdir "%MAX_DIR%\src_kl"
MOVE h "%MAX_DIR%\src_kl\"
endlocal
