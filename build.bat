call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" amd64
msbuild /m:%1 /p:Configuration=%2 /p:Platform=%3 %4
set batdir=%~dp0
xcopy %batdir%stage "%batdir%../../../stage/Windows/x86_64/Release/DCCIntegrations" /e /i /y
