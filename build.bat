call "C:\Program Files (x86)\Microsoft Visual Studio 12.0\VC\vcvarsall.bat" x86_amd64
msbuild /m:%1 /p:Configuration="Release %2" /p:Platform=%3 %4
set batdir=%~dp0
set FabricStageDir=%batdir%stage/Fabric3dsmax%2
set FabricFinalDir=%batdir%../../../stage/Windows/x86_64/Release/DCCIntegrations/Fabric3dsmax%2
xcopy "%FabricStageDir%"  "%FabricFinalDir%" /e /i /y
