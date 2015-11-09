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

assert (CompArray (GetTriMeshFaceCount sm) initTriCounts) message:"Bad Init Tri Count" options:#(#dialog)
assert (CompBB sm 110.851) message:"Bad init size" options:#(#dialog)

-- Validate radius is being set correctly
sm.radius = 50
assert (CompBB sm 172.802) message:"Bad 5 size" options:#(#dialog)

-- Change input type for PolyMesh from none to INode
sm.DFGAddPort "this" 0 "PolygonMesh" portToConnect:"AddSphere.this"
sm.DFGRemoveNodes #("EmptyPolygonMesh")
sm.DFGRenamePort "this" "inMesh"

-- Pipe in a teapot to copy
tp = Teapot()
tp.radius = 50
tp.position = Point3 10 10 0

sm.inMesh = tp
assert (CompArray (GetTriMeshFaceCount sm) #(1530, 785)) message:"Bad Tri Count on Set Mesh" options:#(#dialog)
assert (CompBB sm 228.915) message:"Bad 5 size" options:#(#dialog)
	
print "all tests passed!"
