
-- Bring in util functions
rootFolder = systemTools.getEnvVariable("FABRIC3DSMAXDIR")
fileIn (rootFolder + "samples\\UnitTestUtilFns.ms")

filein (rootFolder + "samples\\UnitTestFabricInterface.ms")
filein (rootFolder + "samples\\UnitTestMaxClassObject.ms")
filein (rootFolder + "samples\\UnitTestMaxClassModifier.ms")
filein (rootFolder + "samples\\UnitTestMaxClassWSModifier.ms")
filein (rootFolder + "samples\\UnitTestMaxClassControllers.ms")
filein (rootFolder + "samples\\UnitTestPortConnection.ms")
filein (rootFolder + "samples\\UnitTestLoadSaveMaxFile.ms")
filein (rootFolder + "samples\\UnitTestLoadSaveSpliceFile.ms")
	
fn DoTestAll = 
(
	-- Test each function and make sure it runs cleanly
	if not (TestFunction TestFabricInterface) do ( return false )

	if not (TestFunction TestMaxClassObject) do ( return false )

	if not (TestFunction TestMaxClassModifier) do ( return false )

	if not (TestFunction TestMaxClassWSModifier) do ( return false )

	if not (TestFunction TestMaxClassControllers) do ( return false )

	if not (TestFunction TestPortConnection) do ( return false )

	if not (TestFunction TestLoadSaveMaxFile) do ( return false )

	if not (TestFunction TestLoadSaveSpliceFile) do ( return false )
	
	true
)

DoTestAll()