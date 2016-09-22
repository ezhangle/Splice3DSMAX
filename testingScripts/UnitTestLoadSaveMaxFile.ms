----------------------------------------------------------------------
--
-- Test save/load of Fabric classes in the MaxScene
--	We simply run each MaxClass unit test, save, load 
--   and validate the result
--
----------------------------------------------------------------------
rootFolder = systemTools.getEnvVariable("FABRIC3DSMAXDIR")
fileIn (rootFolder + "samples\\UnitTestMaxClassControllers.ms")
fileIn (rootFolder + "samples\\UnitTestMaxClassObject.ms")
fileIn (rootFolder + "samples\\UnitTestMaxClassModifier.ms")
fileIn (rootFolder + "samples\\UnitTestMaxClassWSModifier.ms")

fn TestLoadSaveMaxFile = 
(
	tempFile = systemTools.getEnvVariable("TEMP") + "\\temp.max"
	--------------------------------------------------------------------------------
	-- First test, controllers


	TestMaxClassControllers()
	res = saveMaxFile tempFile useNewFile:false quiet:true
	FAssert res "Failed saving file UnitTestMaxClassControllers"
	res = loadMaxFile tempFile quiet:true
	FAssert res "Failed loading file UnitTestMaxClassControllers"

	-- Validate load was successfuly
	sliderTime = 50f
	RedrawViews()
	m1 = $Point002.transform
	m2 = (matrix3 [1,-2.38419e-007,-2.5332e-007] [2.45869e-007,0.798292,0.60227] [1.3411e-007,-0.60227,0.798292] [8.39596,-11.0702,13.6661])
	FAssert (matricesAreEquivalent m1 m2) "Bad rotation after setting dfg float input"
	m1 = $Point003.transform 
	m2= (matrix3 [0.739942,0.516122,0.431398] [-0.620885,0.277274,0.733227] [0.258819,-0.810393,0.525619] [1.66667,10,0])
	FAssert (matricesAreEquivalent m1 m2) "Bad rotation values"
	print(" -- Saved/Loaded Controllers successfully --")
	--------------------------------------------------------------------------------
	-- Next, test object
	resetMaxFile #noPrompt
	TestMaxClassObject()

	res = saveMaxFile tempFile useNewFile:false quiet:true
	FAssert res "Failed saving file UnitTestMaxClassObject"
	res = loadMaxFile tempFile quiet:true
	FAssert res "Failed loading file UnitTestMaxClassObject"

	FAssert (CompArray (GetTriMeshFaceCount $FabricMesh001) #(1530, 785)) "Bad Tri Count on Set Mesh"
	FAssert (CompBB $FabricMesh001 228.915) "Bad 5 size"
		
	print(" -- Saved/Loaded Objects successfully --")

	--------------------------------------------------------------------------------
	-- Test Modifiers
	resetMaxFile #noPrompt
	TestMaxClassModifier()
		
	$Plane001.modifiers[1].enabled = false
		
	res = saveMaxFile tempFile useNewFile:false quiet:true
	FAssert res "Failed saving file UnitTestMaxClassModifier"
	res = loadMaxFile tempFile quiet:true
	FAssert res "Failed loading file UnitTestMaxClassModifier"

	FAssert (not $Plane001.modifiers[1].enabled) "Did not save/load enabled state"
	$Plane001.modifiers[1].enabled = true
	FAssert (CompBB $Plane001 207.122) "Bad modified object size"

	print(" -- Saved/Loaded Modifiers successfully --")
		
	--------------------------------------------------------------------------------
	-- Test WSModifiers
	resetMaxFile #noPrompt
	TestMaxClassWSModifier()

	$Sphere001.modifiers[1].enabled = false
	
	res = saveMaxFile tempFile useNewFile:false quiet:true
	FAssert res "Failed saving file UnitTestMaxClassObject"
	res = loadMaxFile tempFile quiet:true
	FAssert res "Failed loading file UnitTestMaxClassObject"

	FAssert (not $Sphere001.modifiers[1].enabled) "Did not save/load enabled state"
	$Sphere001.modifiers[1].enabled = true
	FAssert (CompBB $Sphere001 131.938) "Bad WSModifier loaded size"
	print(" -- Saved/Loaded WSModifiers successfully --")
)