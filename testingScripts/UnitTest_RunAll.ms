
-- Bring in util functions
rootFolder = systemTools.getEnvVariable("FABRIC3DSMAXDIR")
fileIn (rootFolder + "samples\\UnitTestUtilFns.ms")
-- Bring in each function and test it
filein (rootFolder + "samples\\UnitTestFabricInterface.ms")
TestFunction TestFabricInterface

filein (rootFolder + "samples\\UnitTestMaxClassObject.ms")
TestFunction TestMaxClassObject

filein (rootFolder + "samples\\UnitTestMaxClassModifier.ms")
TestFunction TestMaxClassModifier

filein (rootFolder + "samples\\UnitTestMaxClassWSModifier.ms")
TestFunction TestMaxClassWSModifier

filein (rootFolder + "samples\\UnitTestMaxClassControllers.ms")
TestFunction TestMaxClassControllers

filein (rootFolder + "samples\\UnitTestPortConnection.ms")
TestFunction TestPortConnection

filein (rootFolder + "samples\\UnitTestLoadSaveMaxFile.ms")
TestFunction TestLoadSaveMaxFile

filein (rootFolder + "samples\\UnitTestLoadSaveSpliceFile.ms")
TestFunction TestLoadSaveSpliceFile