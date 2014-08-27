REM This file prepares the Dist folder for installation.
REM It should be run first, then the 'install' bat should
REM be run from the distribution folder

ECHO cleaning existing files
DEL "%~dp0..\..\dist\FabricSplice3dsmax\editor.ms"
DEL "%~dp0..\..\dist\FabricSplice3dsmax\Install.bat"
RMDIR /S /Q "%~dp0..\..\dist\FabricSplice3dsmax\samples"

ECHO making new links
mklink /H "%~dp0..\..\dist\FabricSplice3dsmax\editor.ms" "%~dp0editor.ms"
mklink /H "%~dp0..\..\dist\FabricSplice3dsmax\Install.bat" "%~dp0_Install._bat"

mkdir "%~dp0..\..\dist\FabricSplice3dsmax\samples\TestFiles"

mklink /H "%~dp0..\..\dist\FabricSplice3dsmax\samples\CharacterRendering.ms" "%~dp0CharacterRendering.ms"
mklink /H "%~dp0..\..\dist\FabricSplice3dsmax\samples\CharacterRenderingX1000.ms" "%~dp0CharacterRenderingX1000.ms"
mklink /H "%~dp0..\..\dist\FabricSplice3dsmax\samples\InlineDrawingTest.ms" "%~dp0InlineDrawingTest.ms"
mklink /H "%~dp0..\..\dist\FabricSplice3dsmax\samples\ManipulationTest.ms" "%~dp0ManipulationTest.ms"
mklink /H "%~dp0..\..\dist\FabricSplice3dsmax\samples\SampleMxSExtendKL.ms" "%~dp0SampleMxSExtendKL.ms"
mklink /H "%~dp0..\..\dist\FabricSplice3dsmax\samples\SurfaceAttrPaintManipulation.ms" "%~dp0SurfaceAttrPaintManipulation.ms"
mklink /H "%~dp0..\..\dist\FabricSplice3dsmax\samples\UnitTestSpliceObject.ms" "%~dp0UnitTestSpliceObject.ms"

mklink /H "%~dp0..\..\dist\FabricSplice3dsmax\samples\TestFiles\Bunny.obj" "%~dp0TestFiles\Bunny.obj"
mklink /H "%~dp0..\..\dist\FabricSplice3dsmax\samples\TestFiles\Witcher_Creep.fbx" "%~dp0TestFiles\Witcher_Creep.fbx"

ECHO trigger the actual install
CALL "%~dp0..\..\dist\FabricSplice3dsmax\Install.bat"
