fn CompArray a b = (a as string == b as string)
fn CompBB anode expSize = (
	bb = (nodeLocalBoundingBox anode)
	size = length (bb[1] - bb[2])
	res =abs(size - expSize) < 0.001
)
fn FindPort sm name = (
	for i = 1 to sm.PortCount do (
		if (sm.GetPortName i) == name do return true
	)
	return false
)

-- Create the first mesh
sm = SpliceMesh()
-- Init required params
nodeName = sm.name
sm.OutPort = nodeName + "Val"

-- Init base compilation
klCode =  "
require Math;
require PolygonMesh;

operator " + nodeName + "(io PolygonMesh " + sm.OutPort + ") {
	Xfo init;
	init.setIdentity();
	" + sm.OutPort + " = PolygonMesh();
	" + sm.OutPort + ".addSphere(init, 32, 10, true, true);
}"
sm.SetKLCode nodeName klCode
assert (CompArray (GetTriMeshFaceCount sm) #(576, 312)) message:"Bad Init Tri Count" options:#(#dialog)
assert (CompBB sm 110.851) message:"Bad init size" options:#(#dialog)

sm2 = SpliceMesh()
sm2.addInputPort "inMesh" "PolygonMesh" -1
sm2.OutPort = "SpliceMeshVal"
klCode = "
require Math;
require Geometry;

operator cloneMesh(
  io PolygonMesh SpliceMeshVal,
	in PolygonMesh inMesh
) {
  SpliceMeshVal = inMesh;
}
";
sm2.SetKLCode "cloneMesh" klCode

-- Set the output from sm as the input on sm2
sm2.ConnectPorts "inMesh" sm.baseobject (nodeName + "Val")
assert (CompArray (GetTriMeshFaceCount sm) #(576, 312)) message:"Bad Init Tri Count" options:#(#dialog)
assert (CompBB sm 110.851) message:"Bad init size" options:#(#dialog)

----------------------------------------------------------------
-- Test array accesses
dataContainer = SpliceReferenceTarget()
dataContainer.addOutputPort "ptTransforms" "Mat44" isArray:true
klCode = "
require Math;
require Geometry;

operator genTransforms(
  in EvalContext context,
	io Mat44 ptTransforms[])
{
	report('Eval: ' + context.time);
	for (Integer i = 0; i < ptTransforms.size(); i++) {
		ptTransforms[i].setIdentity();
	  Vec3 trans(i * 10, sin(context.time) * 10, 0);
		ptTransforms[i].setTranslation(trans);
	}
}
";

dataContainer.setKLCode "genTransforms" klCode

p1 = point()
spliceCtrl = SpliceMatrixController()
p1.transform.controller = spliceCtrl
spliceCtrl.OutPort = "returnedValue"
spliceCtrl.addInputPort "inMtx" "Mat44" -1
klCode = "
require Math;

operator returnValue(
  io Mat44 returnedValue, 
  in Mat44 inMtx, 
) {
  returnedValue = inMtx;
}
";
spliceCtrl.setKLCode "returnValue" klCode
spliceCtrl.ConnectPorts "inMtx"  dataContainer "ptTransforms" srcPortIndex:0