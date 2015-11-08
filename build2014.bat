
set BOOST_DIR=%FABRIC_SCENE_GRAPH_DIR%\ThirdParty\PreBuilt\Windows\x86_64\Release\boost\1.55.0\lib
set ADSK_3DSMAX_SDK_2014=%FABRIC_SCENE_GRAPH_DIR%\ThirdParty\PreBuilt\Windows\x86_64\Release\3dsmax\3ds Max 2014 SDK\maxsdk

c:\Windows\Microsoft.NET\Framework64\v4.0.30319\msbuild.exe "/p:Configuration=Release 2014" /p:Platform=x64 Fabric3dsmax.vcxproj

