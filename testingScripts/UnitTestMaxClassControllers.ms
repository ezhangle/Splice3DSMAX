rootFolder = systemTools.getEnvVariable("FABRIC3DSMAXDIR")
fileIn (rootFolder + "samples\\UnitTestUtilFns.ms")

-----------------------------------------------------------------------------------------------------------
-- Test basic controller structure, time change
p1 = point()
dfgCtrller = FabricMatrixController()
p1.controller = dfgCtrller

dfgCtrller.DFGInstPreset "Fabric.Exts.Util.EvalContext.CurrentEvalContext" [341.000000,183.000000]   execPath:""
dfgCtrller.DFGInstPreset "Fabric.Exts.Math.Mat44.SetTranslation" [237.000000,183.000000]   execPath:""
dfgCtrller.DFGConnect "parentValue" "SetTranslation.this"   execPath:""
dfgCtrller.DFGInstPreset "Fabric.Core.Math.Mul" [200.000000,183.000000]   execPath:""
dfgCtrller.DFGInstPreset "Fabric.Exts.Math.Vec3.Vec3" [341.000000,183.000000]   execPath:""
dfgCtrller.DFGConnect "Vec3.result" "SetTranslation.tr"   execPath:""
dfgCtrller.DFGConnect "Mul.result" "Vec3.x"   execPath:""
dfgCtrller.DFGInstPreset "Fabric.Exts.Util.EvalContext.GetTime" [-42.000000,183.000000]   execPath:""
dfgCtrller.DFGConnect "CurrentEvalContext.context" "GetTime.this"   execPath:""
dfgCtrller.DFGConnect "GetTime.result" "Mul.lhs"   execPath:""
dfgCtrller.DFGSetPortDefaultValue "Mul.rhs" 7   execPath:""
dfgCtrller.DFGConnect "SetTranslation.this" "outputValue"   execPath:""

dfgCtrller.DFGMoveNodes #("GetTime") #([186.000000,83.000000])   execPath:""
dfgCtrller.DFGMoveNodes #("CurrentEvalContext") #([-15.000000,78.000000])   execPath:""
dfgCtrller.DFGMoveNodes #("Mul") #([321.000000,79.000000])   execPath:""
dfgCtrller.DFGMoveNodes #("Vec3") #([444.000000,111.000000])   execPath:""
dfgCtrller.DFGMoveNodes #("SetTranslation") #([589.000000,213.000000])   execPath:""

sliderTime = 0f
FAssert (p1.transform.position == [0, 0, 0]) "Bad Initial Position"
sliderTime = 75f
FAssert (p1.transform.position == [17.5, 0, 0]) "Bad Anim Position"

-------------------------------------------------------------------------------------------------------------
-- Test Position controller, parenting
p2 = point()
dfgCtrl = FabricPositionController()
p2.controller.position.controller = dfgCtrl
p2.parent = p1

dfgCtrl.DFGInstPreset "Fabric.Exts.Math.Mat44.MultiplyVector4" [316.000000,183.000000]   execPath:""
dfgCtrl.DFGMoveNodes #("MultiplyVector4") #([255.000000,197.000000])   execPath:""
dfgCtrl.DFGMoveNodes #("MultiplyVector4") #([406.000000,196.000000])   execPath:""
dfgCtrl.DFGInstPreset "Fabric.Exts.Math.Vec4.Vec4" [65.000000,183.000000]   execPath:""
dfgCtrl.DFGMoveNodes #("Vec4") #([113.000000,163.000000])   execPath:""
dfgCtrl.DFGConnect "Vec4.result" "MultiplyVector4.other"   execPath:""
dfgCtrl.DFGConnect "parentValue" "MultiplyVector4.this"   execPath:""
dfgCtrl.DFGSetPortDefaultValue "Vec4.z" 26   execPath:""
dfgCtrl.DFGSetPortDefaultValue "Vec4.t" 1   execPath:""
dfgCtrl.DFGInstPreset "Fabric.Exts.Math.Func.ToVec3" [495.000000,183.000000]   execPath:""
dfgCtrl.DFGMoveNodes #("ToVec3") #([613.000000,194.000000])   execPath:""
dfgCtrl.DFGConnect "MultiplyVector4.result" "ToVec3.this"   execPath:""
dfgCtrl.DFGConnect "ToVec3.result" "outputValue"   execPath:""

sliderTime = 0f
assert (p2.transform.position == [0, 0, 26]) message:"Bad Inheritted Position" options:#(#dialog)
sliderTime = 75f
assert (p2.transform.position == [17.5, 0, 26]) message:"Bad Inheritted Animation" options:#(#dialog)

-------------------------------------------------------------------------------------------------------------
-- Test Rotation controller, transform order

p3 = point()
dfgCtrl = FabricRotationController()
p3.controller.rotation.controller = dfgCtrl

dfgCtrl.DFGInstPreset "Fabric.Exts.Math.Euler.Euler" [185.000000,183.000000]   execPath:""
dfgCtrl.DFGAddPort "x" 0 "Scalar" portToConnect:"Euler.x" extDep:"" metaData:"" execPath:""
dfgCtrl.DFGAddPort "y" 0 "Scalar" portToConnect:"Euler.y" extDep:"" metaData:"" execPath:""
dfgCtrl.DFGAddPort "z" 0 "Scalar" portToConnect:"Euler.z" extDep:"" metaData:"" execPath:""
dfgCtrl.DFGInstPreset "Fabric.Exts.Math.Quat.SetFromEuler" [388.000000,183.000000]   execPath:""
dfgCtrl.DFGConnect "Euler.result" "SetFromEuler.e"   execPath:""
dfgCtrl.DFGConnect "SetFromEuler.result" "outputValue"   execPath:""

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

FAssert ((p2.transform as string) == "(matrix3 [1,-2.23517e-007,-1.63913e-007] [0,1,0] [1.19209e-007,0,1] [-3.2707,1.41048,23.5995])") "Bad child offset"
FAssert ((p3.transform as string) == "(matrix3 [0.739942,0.671834,0.0335364] [-0.620886,0.662946,0.418335] [0.258819,-0.330366,0.907673] [-10,10,0])") "Bad rotation values"
sliderTime = 50f
FAssert ((p3.transform as string) == "(matrix3 [0.739942,0.671834,0.0335364] [-0.620886,0.662946,0.418335] [0.258819,-0.330366,0.907673] [1.66667,10,0])") "Bad rotation values"
-------------------------------------------------------------------------------------------------------------
-- Test float controller, input chaining

dfgFloatCtrl = FabricFloatController()

dfgFloatCtrl.DFGInstPreset "Fabric.Exts.Util.EvalContext.CurrentEvalContext" [302.000000,183.000000]   execPath:""
dfgFloatCtrl.DFGInstPreset "Fabric.Exts.Util.EvalContext.GetTime" [408.000000,183.000000]   execPath:""
dfgFloatCtrl.DFGConnect "CurrentEvalContext.context" "GetTime.this"   execPath:""
dfgFloatCtrl.DFGInstPreset "Fabric.Core.Trigonometry.Sin" [301.000000,183.000000]   execPath:""
dfgFloatCtrl.DFGConnect "GetTime.result" "Sin.value"   execPath:""
dfgFloatCtrl.DFGConnect "Sin.result" "outputValue"   execPath:""

dfgCtrl.x.controller = dfgFloatCtrl

sliderTime = 0
FAssert ((p3.transform as string) == "(matrix3 [0.739942,0.642788,-0.198267] [-0.620885,0.766045,0.166366] [0.258819,0,0.965926] [-10,10,0])") "Bad rotation values"
sliderTime = 50f
FAssert ((p2.transform as string) == "(matrix3 [1,-2.38419e-007,-2.5332e-007] [2.45869e-007,0.798292,0.60227] [1.3411e-007,-0.60227,0.798292] [8.39596,-11.0702,13.6661])") "Bad rotation after setting dfg float input"
FAssert ((p3.transform as string) == "(matrix3 [0.739942,0.516122,0.431398] [-0.620885,0.277274,0.733227] [0.258819,-0.810393,0.525619] [1.66667,10,0])") "Bad rotation values"

-------------------------------------------------------------------------------------------------------------
-- Test shared graphs, array output.

--p4 = Point()
--dfgCtrl = FabricMatrixController()
--p4.controller = dfgCtrl
--dfgCtrl.OutPort = "outValArray"
--dfgCtrl.OutPortIndex = 0

--dfgCtrl.AddInputPort "xMult" "Scalar" -2
--dfgCtrl.AddInputPort "yMult" "Scalar" -2
--dfgCtrl.AddInputPort "zMult" "Scalar" -2

--dfgCtrl.SetKLCode entryPt klCode

--p5 = Point()
--p5.controller = FabricMatrixController()
--p5.controller.SetFabricGraph p4.controller
--p5.controller.OutPort = "outValArray"
--p5.controller.OutPortIndex = 1

--p6 = Point()
--p6.controller = FabricMatrixController()
--p6.controller.SetFabricGraph p4.controller
--p6.controller.OutPort = "outValArray"
--p6.controller.OutPortIndex = 2

print "All Passed!"
