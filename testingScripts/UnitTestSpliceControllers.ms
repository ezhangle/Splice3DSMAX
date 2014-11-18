-----------------------------------------------------------------------------------------------------------
-- Test basic controller structure, time change
p1 = point()
p1.controller = SpliceMatrixController()
p1.controller.OutPort = "outMatrixVal"
entryPt = "unitTestBasicTransform"
klCode = "
require Math;

operator unitTestBasicTransform(io Mat44 outMatrixVal, in Mat44 parentValue, in EvalContext context) {
  outMatrixVal.setTranslation(Vec3(context.time * 7, 0, 0));
}
"
p1.controller.SetKLCode entryPt klCode
sliderTime = 0f
assert (p1.transform.position == [0, 0, 0]) message:"Bad Initial Position" options:#(#dialog)
sliderTime = 75f
assert (p1.transform.position == [17.5, 0, 0]) message:"Bad Anim Position" options:#(#dialog)

-------------------------------------------------------------------------------------------------------------
-- Test Position controller, parenting
p2 = point()
spliceCtrl = SplicePositionController()
p2.controller.position.controller = spliceCtrl
p2.parent = p1
spliceCtrl.OutPort = "outPositionVal"
entryPt = "unitTestPositionInheritance"
klCode = "
require Math;

operator unitTestPositionInheritance(io Vec3 outPositionVal, in Mat44 parentValue) {
  Vec4 offset(0, 0, 26, 1);
  offset = parentValue * offset; // Make offset relative to parent
  outPositionVal = Vec3(offset.x, offset.y, offset.z);
}"

spliceCtrl.SetKLCode entryPt klCode
sliderTime = 0f
assert (p2.transform.position == [0, 0, 26]) message:"Bad Inheritted Position" options:#(#dialog)
sliderTime = 75f
assert (p2.transform.position == [17.5, 0, 26]) message:"Bad Inheritted Animation" options:#(#dialog)

-------------------------------------------------------------------------------------------------------------
-- Test Rotation controller, transform order

p3 = point()
spliceCtrl = SpliceRotationController()
p3.controller.rotation.controller = spliceCtrl
spliceCtrl.OutPort = "outRotationVal"
spliceCtrl.AddInputPort "xRot" "Scalar" 5
spliceCtrl.AddInputPort "yRot" "Scalar" 5
spliceCtrl.AddInputPort "zRot" "Scalar" 5

entryPt = "unitTestRotationChaining"
klCode = "
require Math;

operator unitTestRotationChaining(io Quat outRotationVal, in Scalar xRot, in Scalar yRot, in Scalar zRot) {
  outRotationVal = Quat(Euler(xRot, yRot, zRot));
}"

spliceCtrl.SetKLCode entryPt klCode
spliceCtrl.xRot = 20
spliceCtrl.yRot = 15
spliceCtrl.zRot = 40
p3.position = [-10, 10, 0]

sliderTime = 0f
p3.parent = p1
p2.parent = p3
-- Jiggle slider time, max bug means p2 isn't updated yet
sliderTime = 1f
sliderTime = 0f

assert ((p2.transform as string) == "(matrix3 [1,-2.23517e-007,-1.63913e-007] [0,1,0] [1.19209e-007,0,1] [-3.2707,1.41048,23.5995])") message:"Bad child offset" options:#(#dialog)
assert ((p3.transform as string) == "(matrix3 [0.739942,0.671834,0.0335364] [-0.620886,0.662946,0.418335] [0.258819,-0.330366,0.907673] [-10,10,0])") message:"Bad rotation values" options:#(#dialog)

-------------------------------------------------------------------------------------------------------------
-- Test float controller, input chaining

spliceFloatCtrl = SpliceFloatController()
spliceFloatCtrl.OutPort = "outSinTime"
entryPt = "unitTestFloatChaining"
klCode = "
require Math;

operator unitTestFloatChaining(io Scalar outSinTime, in EvalContext context) {
  outSinTime = sin(context.time);
}"

spliceFloatCtrl.SetKLCode entryPt klCode
spliceCtrl.xRot.controller = spliceFloatCtrl

sliderTime = 50f
assert ((p2.transform as string) == "(matrix3 [1,-2.38419e-007,-2.5332e-007] [2.45869e-007,0.798292,0.60227] [1.3411e-007,-0.60227,0.798292] [8.39596,-11.0702,13.6661])") message:"Bad rotation after setting splice float input" options:#(#dialog)
assert ((p3.transform as string) == "(matrix3 [0.739942,0.516122,0.431398] [-0.620885,0.277274,0.733227] [0.258819,-0.810393,0.525619] [1.66667,10,0])") message:"Bad rotation values"  options:#(#dialog)

entryPt = "unitTestSharedGraph"
klCode = "
require Math;

operator unitTestSharedGraph(io Mat44 outValArray[], Scalar xMult, Scalar yMult, Scalar zMult) {
  for (Integer i = 0; i < outValArray.size(); i++) {
    outValArray[i].setIdentity();
    outValArray[i].setTranslation(Vec3(i * xMult, i * yMult, i * zMult));
  }
}"

-------------------------------------------------------------------------------------------------------------
-- Test shared graphs, array output.

p4 = Point()
spliceCtrl = SpliceMatrixController()
p4.controller = spliceCtrl
spliceCtrl.OutPort = "outValArray"
spliceCtrl.OutPortIndex = 0

spliceCtrl.AddInputPort "xMult" "Scalar" -2
spliceCtrl.AddInputPort "yMult" "Scalar" -2
spliceCtrl.AddInputPort "zMult" "Scalar" -2

spliceCtrl.SetKLCode entryPt klCode

p5 = Point()
p5.controller = SpliceMatrixController()
p5.controller.SetSpliceGraph p4.controller
p5.controller.OutPort = "outValArray"
p5.controller.OutPortIndex = 1

p6 = Point()
p6.controller = SpliceMatrixController()
p6.controller.SetSpliceGraph p4.controller
p6.controller.OutPort = "outValArray"
p6.controller.OutPortIndex = 2

print "All Passed!"