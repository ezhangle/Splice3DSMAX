fn FindPort sm name = (
	ret = 0
	for i = 1 to sm.numPorts do (
		if (sm.GetPortName i) == name do ret = i
	)
	ret
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

-- Test adding new ports
sm.CreatePort "inSegments" "Size" 1
assert ((FindPort sm "inSegments") != 0) message:"Failed Creating inSegments" options:#(#dialog)
assert (hasProperty sm "inSegments") message:"Failed creading Max param for inSegments" options:#(#dialog)

-- Create scalar with default type 
sm.CreatePort "inRadius" "Scalar" -2
assert ((FindPort sm "inRadius") != 0)message:"Failed Creating inRadius" options:#(#dialog)
assert (hasProperty sm "inRadius") message: "Error: Created invalid max param inMesh" options:#(#dialog)

-- Create some more junk params
sm.CreatePort "aParam" "Scalar" -1
sm.CreatePort "aParam3" "PolygonMesh" -2
sm.CreatePort "aParam4" "Integer" -2

-- Remove one of the added ports
firstPort = sm.getPortName 1
sm.removePort firstPort