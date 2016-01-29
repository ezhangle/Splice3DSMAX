makelocal
cd "%~dp0qt-solutions\qtwinmigrate"
configure -library
qmake
nmake
endlocal