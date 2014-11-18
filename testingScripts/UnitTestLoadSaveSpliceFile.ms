-----------------------------------------------------------------------------
-- Test import of Splice Files.
--   This unit test validates the ability to load/save Splice files.
--   Tested:
--      Raw scene load of Splice file
--      Object save of splice file
--      Object load of splice file.
-----------------------------------------------------------------------------
fn CompArray a b = (a as string == b as string)

fn DoLoadFile filepath = (
	numObjects = objects.count
	res = Splice.loadFromFile filepath
	-- We should have a newly created mesh object
	assert (objects.count == numObjects + 1) message:("Did not create node for:" + filepath) options:#(#dialog)
	spliceObj = objects[numObjects + 1]
	spliceObj
)

-- All the examples here actually only happen in splice, so we need rendering to see whats happening
Splice.rendering = true

-- We will load a few sample files from the installed SpliceStandalone
sysEnv = dotNetClass "System.Environment"
fabricDir = sysEnv.GetEnvironmentVariable "FABRIC_DIR"
sampleDir = fabricDir + "Samples\\SpliceStandalone\\"
tempDir = sysEnv.GetEnvironmentVariable "TEMP"

procGeo = DoLoadFile (sampleDir + "ProceduralGeometry\procgeo_revolution.splice")
assert (procGeo.PortCount == 3) message:"Could not set param on new geo" options:#(#dialog)
procGeo.factor = 2
assert (procGeo.factor == 2) message:"Could not set param on new geo" options:#(#dialog)

procQuads = DoLoadFile (sampleDir + "ProceduralGeometry\procgeo_quads.splice")
procQuads

-- Try something really tough, import bullet
bulletOp = DoLoadFile (sampleDir + "Bullet\RigidBodies_BasicDemo.splice");
-- Ironically, the bullet op doesn't take advantage of the EvalContext time parameter

-- Now, we test loading and saving off a proper max operator
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

res = sm.saveToFile (tempDir + "\\sphere.splice")
assert (res) message:"Could not set save splice file" options:#(#dialog)

delete sm
sm = DoLoadFile (tempDir + "\\sphere.splice");
assert (CompArray (GetTriMeshFaceCount sm) #(576, 312)) message:"Did not restore output values in scene" options:#(#dialog)
	
	
delete sm
sm = SpliceMesh()
sm.loadFromFile (tempDir + "\\sphere.splice") true
assert (CompArray (GetTriMeshFaceCount sm) #(576, 312)) message:"Did not restore output values in object" options:#(#dialog)