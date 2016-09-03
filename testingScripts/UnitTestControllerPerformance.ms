nControllers = 400
nRows = floor(sqrt(nControllers))
nPerRow = floor(nControllers / nRows) + 1
spacing = 20

	-- Bring in util functions
rootFolder = systemTools.getEnvVariable("FABRIC3DSMAXDIR")
fileIn (rootFolder + "samples\\UnitTestUtilFns.ms")
	
fn RunScene messageStr = (	start = timeStamp()
	-- Redraw 100 frames
	for i = 1 to 100 do (
		sliderTime = i
	)
	end = timeStamp()

	elapsed = end - start
	format (messageStr + ": Drawing took % milliseconds (% fps)\n") elapsed (100 * 1000 / elapsed)
	elapsed
)

fn BuildScene modFn = (
	baseNode =  Box()
	for i = 1 to nRows do (
		for j = 1 to nPerRow do (
			anInst = instance baseNode
			p = [(i * spacing), (j * spacing), 0]
			move anInst p
			anInst.parent = baseNode
			modFn anInst
		)
	)
	RedrawViews()
	baseNode
)

fn ModMaxCtrl instNode = (
	addNewKey instNode.transform.controller 0
	addNewKey instNode.transform.controller 100
)

fn ModFabricCtrl aNode = (
	aNode.transform.controller = FabricMatrixController()
)

fn ModFabricCtrlAddZ aNode = (
	fabCtrl = FabricMatrixController()
	fabCtrl.DFGInstPreset "Fabric.Exts.Math.Mat44.Translation" [245.000000,200.000000]   execPath:""
	fabCtrl.DFGMoveNodes #("Translation_1") #([225.000000,241.000000])   execPath:""
	fabCtrl.DFGConnect "parentValue" "Translation_1.this"   execPath:""
	fabCtrl.DFGInstPreset "Fabric.Exts.Math.Vec3.Set" [333.000000,200.000000]   execPath:""
	fabCtrl.DFGMoveNodes #("Set_1") #([469.000000,200.000000])   execPath:""
	fabCtrl.DFGMoveNodes #("Set_1") #([470.000000,216.000000])   execPath:""
	fabCtrl.DFGRemoveNodes #("Set_1")    execPath:""
	fabCtrl.DFGInstPreset "Fabric.Exts.Math.Vec3.ComposeVec3" [285.000000,132.000000]   execPath:""
	fabCtrl.DFGMoveNodes #("ComposeVec3_1") #([393.000000,198.000000])   execPath:""
	fabCtrl.DFGMoveNodes #("Translation_1") #([74.000000,196.000000])   execPath:""
	fabCtrl.DFGMoveNodes #("ComposeVec3_1") #([369.000000,268.000000])   execPath:""
	fabCtrl.DFGInstPreset "Fabric.Exts.Math.Vec3.DecomposeVec3" [279.000000,200.000000]   execPath:""
	fabCtrl.DFGMoveNodes #("DecomposeVec3_1") #([215.000000,274.000000])   execPath:""
	fabCtrl.DFGMoveNodes #("ComposeVec3_1") #([397.000000,286.000000])   execPath:""
	fabCtrl.DFGConnect "Translation_1.result" "DecomposeVec3_1.value"   execPath:""
	fabCtrl.DFGConnect "DecomposeVec3_1.x" "ComposeVec3_1.x"   execPath:""
	fabCtrl.DFGConnect "DecomposeVec3_1.y" "ComposeVec3_1.y"   execPath:""
	fabCtrl.DFGSetPortDefaultValue "ComposeVec3_1.z" 10   execPath:""
	fabCtrl.DFGMoveNodes #("Translation_1") #([44.000000,167.000000])   execPath:""
	fabCtrl.DFGMoveNodes #("DecomposeVec3_1") #([185.000000,276.000000])   execPath:""
	fabCtrl.DFGMoveNodes #("ComposeVec3_1") #([323.000000,278.000000])   execPath:""
	fabCtrl.DFGInstPreset "Fabric.Exts.Math.Mat44.SetTranslation" [228.000000,200.000000]   execPath:""
	fabCtrl.DFGMoveNodes #("SetTranslation_1") #([381.000000,172.000000])   execPath:""
	fabCtrl.DFGConnect "parentValue" "SetTranslation_1.this"   execPath:""
	fabCtrl.DFGMoveNodes #("SetTranslation_1") #([447.000000,183.000000])   execPath:""
	fabCtrl.DFGConnect "ComposeVec3_1.result" "SetTranslation_1.tr"   execPath:""
	fabCtrl.DFGConnect "SetTranslation_1.this" "outputValue"   execPath:""
	aNode.transform.controller = fabCtrl
)

fn AnimateNode aNode = (
	with animate on (
		at time 100 (
			move aNode [10, 0, 0]
		)
	)
)

fn TestControllerPerformance = (

	Fabric.Rendering = false

	-- First, run with no overhead (best-case)
	resetMaxFile #noPrompt
	RunScene "EmptyScene"

	BuildScene ModMaxCtrl
	baseTime = RunScene "Max Controllers"
	resetMaxFile #noPrompt

	BuildScene ModFabricCtrl
	fabTime = RunScene "Fabric Controllers"
	
	FAssert (fabTime * 0.75 < baseTime) "Fabric Controllers are too slow!"
	print "Unparented performance acceptable"
	
	resetMaxFile #noPrompt
	baseNode = BuildScene ModMaxCtrl
	AnimateNode baseNode
	baseTime = RunScene "Max Controllers"
	
	resetMaxFile #noPrompt
	baseNode = BuildScene ModFabricCtrl
	AnimateNode baseNode
	fabTime = RunScene "Parented Fabric Controllers"	

	FAssert (fabTime * 0.25 < baseTime) "Fabric Controllers are too slow!"
	print "Parented performance acceptable"
	
	resetMaxFile #noPrompt
	baseNode = BuildScene ModFabricCtrlAddZ
	AnimateNode baseNode
	fabTime = RunScene "Parented Fabric Controllers"	
	
	Fabric.Rendering = true
)