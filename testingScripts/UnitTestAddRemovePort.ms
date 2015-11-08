fn FindPort sm name = (
	ret = 0
	for i = 1 to sm.portCount do (
		if (sm.GetPortName i) == name do ret = i
	)
	ret
)

sm = FabricMesh()
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
sm.addInputPort "inSegments" "Size" 1
assert ((FindPort sm "inSegments") != 0) message:"Failed Creating inSegments" options:#(#dialog)
assert (hasProperty sm "inSegments") message:"Failed creading Max param for inSegments" options:#(#dialog)

-- Create scalar with default type 
sm.addInputPort "inRadius" "Scalar" -2
assert ((FindPort sm "inRadius") != 0)message:"Failed Creating inRadius" options:#(#dialog)
assert (hasProperty sm "inRadius") message: "Error: Created invalid max param inRadius" options:#(#dialog)

-- Create some more junk params
sm.addInputPort "aParam" "Scalar" -1
sm.addInputPort "aParam3" "PolygonMesh" -2
sm.addInputPort "aParam4" "Integer" -2

-- Remove 3 of the added ports
for i = 1 to 3 do (
	existingPortCnt = sm.portCount
	firstPort = sm.getPortName 2
	sm.removePort firstPort
	assert(sm.portCount == existingPortCnt - 1)message:"Invalid count after removal: " options:#(#dialog)
	assert((FindPort sm firstPort) == 0)message:("Failed Removing " + firstPort) options:#(#dialog)
)

-- Add/Remove a port with the same
sm.addInputPort "addRemoveParam" "Scalar" -2
assert ((FindPort sm "addRemoveParam") != 0)message:"Failed Creating addRemoveParam" options:#(#dialog)
sm.removePort "addRemoveParam"
assert ((FindPort sm "addRemoveParam") == 0)message:"Failed Deleting addRemoveParam" options:#(#dialog)
sm.addInputPort "addRemoveParam" "Integer" -2
assert ((FindPort sm "addRemoveParam") != 0)message:"Failed ReCreating addRemoveParam" options:#(#dialog)

-- Test adding an output port 
sm.addOutputPort "outScalar" "Scalar"
assert ((FindPort sm "outScalar") != 0)message:"Failed Creating outScalar" options:#(#dialog)
assert (not (hasProperty sm "outScalar")) message: "Error: Created invalid max param outScalar" options:#(#dialog)
	
-- TODO Test creating one of every type of Max connection
sm.addInputPort "aColor" "Color" -2
	
	
-- Test Saving
tmpFile = systemTools.getEnvVariable("TEMP") + "/portTest.max"
saveMaxFile tmpFile useNewFile:false --[quiet:<bool>] 
loadMaxFile tmpFile useFileUnits:true quiet:true
