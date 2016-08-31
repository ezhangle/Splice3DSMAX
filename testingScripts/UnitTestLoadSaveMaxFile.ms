----------------------------------------------------------------------
--
-- Test save/load of Fabric classes in the MaxScene
--	We simply run each MaxClass unit test, save, load 
--   and validate the result
--
----------------------------------------------------------------------
fn TestLoadSaveMaxFile = 
(
	tempFile = systemTools.getEnvVariable("TEMP") + "\\temp.max"
	--------------------------------------------------------------------------------
	-- First test, controllers
	fileIn (rootFolder + "samples\\UnitTestMaxClassControllers.ms")
	throw "fuck"
	TestFunction TestControllers doReset:false
	res = saveMaxFile tempFile useNewFile:false quiet:true
	FAssert res "Failed saving file UnitTestMaxClassControllers"
	res = loadMaxFile tempFile quiet:true
	FAssert res "Failed saving file UnitTestMaxClassControllers"

	-- Validate load was successfuly
	sliderTime = 50f
	FAssert (($Point002.transform as string) == "(matrix3 [1,-2.38419e-007,-2.5332e-007] [2.45869e-007,0.798292,0.60227] [1.3411e-007,-0.60227,0.798292] [8.39596,-11.0702,13.6661])") "Bad rotation after setting dfg float input"
	FAssert (($Point003.transform as string) == "(matrix3 [0.739942,0.516122,0.431398] [-0.620885,0.277274,0.733227] [0.258819,-0.810393,0.525619] [1.66667,10,0])") "Bad rotation values"
	throw "fuck"
	print(" -- Saved/Loaded Controllers successfully --")
	--------------------------------------------------------------------------------
	-- Next, test object
	resetMaxFile #noPrompt
	fileIn (rootFolder + "samples\\UnitTestMaxClassObject.ms")
	res = saveMaxFile tempFile useNewFile:false quiet:true
	FAssert res "Failed saving file UnitTestMaxClassObject"
	res = loadMaxFile tempFile quiet:true
	FAssert res "Failed saving file UnitTestMaxClassObject"

	FAssert (CompArray (GetTriMeshFaceCount $FabricMesh001) #(1530, 785)) "Bad Tri Count on Set Mesh"
	FAssert (CompBB $FabricMesh001 228.915) "Bad 5 size"
		
	print(" -- Saved/Loaded Objects successfully --")

	--------------------------------------------------------------------------------
	-- Test Modifiers
	resetMaxFile #noPrompt
	fileIn (rootFolder + "samples\\UnitTestMaxClassModifier.ms")
	res = saveMaxFile tempFile useNewFile:false quiet:true
	FAssert res "Failed saving file UnitTestMaxClassObject"
	res = loadMaxFile tempFile quiet:true
	FAssert res "Failed saving file UnitTestMaxClassObject"

	FAssert (CompBB $Plane001 207.122) "Bad modified object size"

	print(" -- Saved/Loaded Modifiers successfully --")
		
	--------------------------------------------------------------------------------
	-- Test WSModifiers
	resetMaxFile #noPrompt
	fileIn (rootFolder + "samples\\UnitTestMaxClassWSModifier.ms")
	res = saveMaxFile tempFile useNewFile:false quiet:true
	FAssert res "Failed saving file UnitTestMaxClassObject"
	res = loadMaxFile tempFile quiet:true
	FAssert res "Failed loading file UnitTestMaxClassObject"

	FAssert (CompBB $Sphere001 131.938) "Bad WSModifier loaded size"
	print(" -- Saved/Loaded WSModifiers successfully --")
)