fn vecsAreEquivalent v1 v2 eps:1e-5 =
(
    distance v1 v2 < (length v1 * eps)
)

fn matricesAreEquivalent m1 m2 = 
(
    vecsAreEquivalent m1.row1 m2.row1 AND
    vecsAreEquivalent m1.row2 m2.row2 AND
    vecsAreEquivalent m1.row3 m2.row3 AND
    vecsAreEquivalent m1.row4 m2.row4
)

fn TestMaxClassControllers =
(
	-----------------------------------------------------------------------------------------------------------
	-- Test basic controller structure, time change
	print "Test basic controller"
	p1 = point()
	dfgCtrller = FabricMatrixController()
	p1.controller = dfgCtrller
	dfgCtrller.ShowDFGGraphEditor()

	evalNode = dfgCtrller.DFGInstPreset "Fabric.Exts.Util.EvalContext.CurrentEvalContext" [341.000000,183.000000]   execPath:""
	setTransNode = dfgCtrller.DFGInstPreset "Fabric.Exts.Math.Mat44.SetTranslation" [237.000000,183.000000]   execPath:""
	dfgCtrller.DFGConnect "parentValue" (setTransNode + ".this")   execPath:""
	mulNode = dfgCtrller.DFGInstPreset "Fabric.Core.Math.Mul" [200.000000,183.000000]   execPath:""
	vecNode = dfgCtrller.DFGInstPreset "Fabric.Exts.Math.Vec3.Vec3" [341.000000,183.000000]   execPath:""
	dfgCtrller.DFGConnect (vecNode + ".result") (setTransNode + ".tr")   execPath:""
	dfgCtrller.DFGConnect (mulNode + ".result") (vecNode + ".x")   execPath:""
	timeNode = dfgCtrller.DFGInstPreset "Fabric.Exts.Util.EvalContext.GetTime" [-42.000000,183.000000]   execPath:""
	dfgCtrller.DFGConnect (evalNode + ".context") (timeNode + ".this")   execPath:""
	dfgCtrller.DFGConnect (timeNode + ".result") (mulNode + ".lhs")   execPath:""
	dfgCtrller.DFGSetPortDefaultValue (mulNode + ".rhs") 7   execPath:""
	dfgCtrller.DFGConnect (setTransNode + ".this") "outputValue"   execPath:""

	dfgCtrller.DFGMoveNodes #(timeNode) #([186.000000,83.000000])   execPath:""
	dfgCtrller.DFGMoveNodes #(evalNode) #([-15.000000,78.000000])   execPath:""
	dfgCtrller.DFGMoveNodes #(mulNode) #([321.000000,79.000000])   execPath:""
	dfgCtrller.DFGMoveNodes #(vecNode) #([444.000000,111.000000])   execPath:""
	dfgCtrller.DFGMoveNodes #(setTransNode) #([589.000000,213.000000])   execPath:""

	sliderTime = 0f
	FAssert (p1.transform.position == [0, 0, 0]) "Bad Initial Position"
	sliderTime = 75f
	FAssert (p1.transform.position == [17.5, 0, 0]) "Bad Anim Position"

	-------------------------------------------------------------------------------------------------------------
	print "Test parenting"
	-- Test Position controller, parenting
	p2 = point()
	dfgCtrl = FabricPositionController()
	p2.controller.position.controller = dfgCtrl
	p2.parent = p1

	mulVec = dfgCtrl.DFGInstPreset "Fabric.Exts.Math.Mat44.MultiplyVector4" [316.000000,183.000000]   execPath:""
	dfgCtrl.DFGMoveNodes #(mulVec) #([406.000000,196.000000])   execPath:""
	vec4 = dfgCtrl.DFGInstPreset "Fabric.Exts.Math.Vec4.Vec4" [65.000000,183.000000]   execPath:""
	dfgCtrl.DFGMoveNodes #(vec4) #([113.000000,163.000000])   execPath:""
	dfgCtrl.DFGConnect (vec4 + ".result") (mulVec + ".other")   execPath:""
	dfgCtrl.DFGConnect "parentValue" (mulVec + ".this")   execPath:""
	dfgCtrl.DFGSetPortDefaultValue (vec4 + ".z") 26   execPath:""
	dfgCtrl.DFGSetPortDefaultValue (vec4 + ".t") 1   execPath:""
	toVec3 = dfgCtrl.DFGInstPreset "Fabric.Exts.Math.Func.ToVec3" [495.000000,183.000000]   execPath:""
	dfgCtrl.DFGMoveNodes #(toVec3) #([613.000000,194.000000])   execPath:""
	dfgCtrl.DFGConnect (mulVec + ".result") (toVec3 + ".this")   execPath:""
	dfgCtrl.DFGConnect (toVec3 + ".result") "outputValue"   execPath:""

	sliderTime = 0f
	assert (p2.transform.position == [0, 0, 26]) message:"Bad Inheritted Position" options:#(#dialog)
	sliderTime = 75f
	assert (p2.transform.position == [17.5, 0, 26]) message:"Bad Inheritted Animation" options:#(#dialog)

	-------------------------------------------------------------------------------------------------------------
	-- Test Rotation controller, transform order
	print "Test rotation"
	p3 = point()
	dfgCtrl = FabricRotationController()
	p3.controller.rotation.controller = dfgCtrl

	euler = dfgCtrl.DFGInstPreset "Fabric.Exts.Math.Euler.Euler" [185.000000,183.000000]   execPath:""
	dfgCtrl.DFGAddPort "x" 0 "Scalar" portToConnect:(euler + ".x") extDep:"" metaData:"" execPath:""
	dfgCtrl.DFGAddPort "y" 0 "Scalar" portToConnect:(euler + ".y") extDep:"" metaData:"" execPath:""
	dfgCtrl.DFGAddPort "z" 0 "Scalar" portToConnect:(euler + ".z") extDep:"" metaData:"" execPath:""
	toQuat = dfgCtrl.DFGInstPreset "Fabric.Exts.Math.Quat.SetFromEuler" [388.000000,183.000000]   execPath:""
	dfgCtrl.DFGConnect (euler + ".result") (toQuat + ".e")   execPath:""
	dfgCtrl.DFGConnect (toQuat + ".result") "outputValue"   execPath:""

	-- Convert ports to angles to be more Max'ish
	dfgCtrl.SetMaxTypeForArg "x" 5
	dfgCtrl.SetMaxTypeForArg "y" 5
	dfgCtrl.SetMaxTypeForArg "z" 5

	dfgCtrl.x = 20
	dfgCtrl.y = 15
	dfgCtrl.z = 40
	p3.position = [-10, 10, 0]

	sliderTime = 0f
	p3.parent = p1
	p2.parent = p3
	-- Jiggle slider time, max bug means p2 isn't updated yet
	sliderTime = 1f
	sliderTime = 0f

	testm = (matrix3 [1,-2.23517e-007,-1.63913e-007] [0,1,0] [1.19209e-007,0,1] [-3.2707,1.41048,23.5995])
	FAssert (matricesAreEquivalent p2.transform testm) "Bad child offset"
	testm = (matrix3 [0.739942,0.671834,0.0335364] [-0.620886,0.662946,0.418335] [0.258819,-0.330366,0.907673] [-10,10,0])
	FAssert (matricesAreEquivalent p3.transform testm) "Bad rotation values"
	sliderTime = 50f
	testm = (matrix3 [0.739942,0.671834,0.0335364] [-0.620886,0.662946,0.418335] [0.258819,-0.330366,0.907673] [1.66667,10,0])
	FAssert (matricesAreEquivalent p3.transform testm) "Bad rotation values"
	-------------------------------------------------------------------------------------------------------------
	-- Test float controller, input chaining
	print "Test floats, input chaining"
	dfgFloatCtrl = FabricFloatController()

	evalCtxt = dfgFloatCtrl.DFGInstPreset "Fabric.Exts.Util.EvalContext.CurrentEvalContext" [302.000000,183.000000]   execPath:""
	timeNode = dfgFloatCtrl.DFGInstPreset "Fabric.Exts.Util.EvalContext.GetTime" [408.000000,183.000000]   execPath:""
	dfgFloatCtrl.DFGConnect (evalCtxt + ".context") (timeNode + ".this")   execPath:""
	sinNode = dfgFloatCtrl.DFGInstPreset "Fabric.Core.Trigonometry.Sin" [301.000000,183.000000]   execPath:""
	dfgFloatCtrl.DFGConnect (timeNode + ".result") (sinNode + ".value")   execPath:""
	dfgFloatCtrl.DFGConnect (sinNode + ".result") "outputValue"   execPath:""

	dfgCtrl.x.controller = dfgFloatCtrl

	sliderTime = 0
	testm = (matrix3 [0.739942,0.642788,-0.198267] [-0.620885,0.766045,0.166366] [0.258819,0,0.965926] [-10,10,0])
	FAssert (matricesAreEquivalent p3.transform testm) "Bad rotation values"
	sliderTime = 50f
	testm = (matrix3 [1,-2.38419e-007,-2.5332e-007] [2.45869e-007,0.798292,0.60227] [1.3411e-007,-0.60227,0.798292] [8.39596,-11.0702,13.6661])
	FAssert (matricesAreEquivalent p2.transform testm) "Bad rotation after setting dfg float input"
	testm = (matrix3 [0.739942,0.516122,0.431398] [-0.620885,0.277274,0.733227] [0.258819,-0.810393,0.525619] [1.66667,10,0])
	FAssert (matricesAreEquivalent p3.transform testm) "Bad rotation values"
)