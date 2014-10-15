fn CompArray a b = (a as string == b as string)
fn CompBB anode expSize = (
	bb = (nodeLocalBoundingBox anode)
	size = length (bb[1] - bb[2])
	res =abs(size - expSize) < 0.001
)

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
assert (CompArray (GetTriMeshFaceCount sm) #(576, 598)) message:"Bad Init Tri Count" options:#(#dialog)
assert (CompBB sm 110.851) message:"Bad init size" options:#(#dialog)

-- Add segments parameter, test setting it
sm.CreatePort "inSegments" "Size" 1
assert ((sm.GetPortName 1) == "inSegments") message:"Failed Creating inSegments" options:#(#dialog)
assert (hasProperty sm "inSegments") message:"Failed creading Max param for inSegments" options:#(#dialog)

klCode =  "
require Math;
require PolygonMesh;

operator " + nodeName + "(io PolygonMesh " + sm.OutPort + ", in Size inSegments) {
	Xfo init;
	init.setIdentity();
	" + sm.OutPort + " = PolygonMesh();
	" + sm.OutPort + ".addSphere(init, 32, inSegments, true, true);
}"
sm.SetKLCode nodeName klCode
sm.inSegments = 20
assert (CompArray (GetTriMeshFaceCount sm) #(1936, 1978)) message:"Bad Tri Count on Set Segements" options:#(#dialog)

-- Create unsupported parameter.  This parameter has no matching Max data
sm.CreatePort "inMesh" "PolygonMesh" -1
assert ((sm.GetPortName 2) == "inMesh") message:"Failed Creating inMesh" options:#(#dialog)
-- Assert no matching parameter was created
assert (hasProperty sm "inMesh" == false) message: "Error: Created invalid max param inMesh" options:#(#dialog)

-- Create scalar with default type 
sm.CreatePort "inRadius" "Scalar" -2
assert ((sm.GetPortName 3) == "inRadius") message:"Failed Creating inMesh" options:#(#dialog)
assert (hasProperty sm "inRadius") message: "Error: Created invalid max param inMesh" options:#(#dialog)

-- Pipe new radius parameter into sphere
klCode =  "
require Math;
require PolygonMesh;

operator " + nodeName + "(io PolygonMesh " + sm.OutPort + ", in Size inSegments, in Scalar inRadius) {
	Xfo init;
	init.setIdentity();
	" + sm.OutPort + " = PolygonMesh();
	" + sm.OutPort + ".addSphere(init, inRadius, inSegments, true, true);
}"
sm.SetKLCode nodeName klCode
assert (CompBB sm 0) message:"Bad 0 size" options:#(#dialog)
-- Validate radius is being set correctly
sm.inRadius = 50
assert (CompBB sm 173.205) message:"Bad 5 size" options:#(#dialog)
-- I don't know why, but our bounding box is not being set correctly


-- Assume for now all works as expected.
sm.SetMaxType 2 17
assert (hasProperty sm "inMesh") message: "Error: Did not convert inMesh" options:#(#dialog)

-- Pipe new in mesh
klCode =  "
require Math;
require PolygonMesh;

operator " + nodeName + "(io PolygonMesh " + sm.OutPort + ", in PolygonMesh inMesh, in Size inSegments, in Scalar inRadius) {
	Xfo init;
	init.setIdentity();
	" + sm.OutPort + " = inMesh;
	" + sm.OutPort + ".addSphere(init, inRadius, inSegments, true, true);
}"
sm.SetKLCode nodeName klCode

-- Pipe in a teapot to copy
tp = Teapot()
tp.radius = 50
tp.position = Point3 10 10 0

sm.inMesh = tp
assert (CompArray (GetTriMeshFaceCount sm) #(1040, 548)) message:"Bad Tri Count on Set Mesh" options:#(#dialog)
assert (CompBB sm 228.915) message:"Bad 5 size" options:#(#dialog)
	
print "all tests passed!"