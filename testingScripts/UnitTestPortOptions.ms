sm = SpliceMesh()
-- Init required params
nodeName = sm.name
sm.OutPort = nodeName + "Val"

sm.AddInputPort "inSegments" "Size" -2
sm.AddInputPort "inRadius" "Scalar" -2

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

assert (sm.inSegments == 0) message:"What value is this?" options:#(#dialog)
assert (sm.inRadius == 0) message:"What value is this?" options:#(#dialog)

sm.SetPortMinMax "inSegments" 3 15
sm.SetPortMinMax "inRadius" 5.0 1000.0

-- With the new limits set, we should have dragged our values up to at least minval
-- TODO: How can we verify this limit has been set?
-- assert (sm.inSegments == 3) message:"What value is this?" options:#(#dialog)
-- assert (sm.inRadius == 5) message:"What value is this?" options:#(#dialog)
select sm
