rootFolder = systemTools.getEnvVariable("SPLICE3DSMAXDIR")
fileIn (rootFolder + "samples\\UnitTestUtilFns.ms")

sm = FabricMesh()
-- Init required params
nodeName = sm.name
initTriCounts = #(506, 255)

-- Create a sphere
sm.DFGInstPreset "Fabric.Exts.Geometry.Func.AddSphere" [487.000000,183.000000]   execPath:""
sm.DFGInstPreset "Fabric.Exts.Geometry.PolygonMesh.EmptyPolygonMesh" [56.000000,178.000000]   execPath:""
sm.DFGConnect "EmptyPolygonMesh.mesh" "AddSphere.this"   execPath:""
sm.DFGAddPort "radius" 0 "Scalar" portToConnect:"AddSphere.radius" extDep:"" metaData:"" execPath:""
sm.DFGAddPort "detail" 0 "Size" portToConnect:"AddSphere.detail" extDep:"" metaData:"" execPath:""
sm.DFGConnect "AddSphere.this" "outputValue"   execPath:""

sm.detail = 10
sm.radius= 32

FAssert (CompArray (GetTriMeshFaceCount sm) initTriCounts) "Bad Init Tri Count"
FAssert (CompBB sm 110.851) "Bad init size"

-- Validate radius is being set correctly
sm.radius = 50
FAssert (CompBB sm 172.802) "Bad 5 size"

-- Change input type for PolyMesh from none to INode
sm.DFGAddPort "this" 0 "PolygonMesh" portToConnect:"AddSphere.this"
sm.DFGRemoveNodes #("EmptyPolygonMesh")
sm.DFGRenamePort "this" "inMesh"

-- Pipe in a teapot to copy
tp = Teapot()
tp.radius = 50
tp.position = Point3 10 10 0

sm.inMesh = tp
FAssert (CompArray (GetTriMeshFaceCount sm) #(1530, 785)) "Bad Tri Count on Set Mesh"
FAssert (CompBB sm 228.915) "Bad 5 size"
	
print "all tests passed!"
