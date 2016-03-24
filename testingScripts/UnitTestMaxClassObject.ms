rootFolder = systemTools.getEnvVariable("FABRIC3DSMAXDIR")
fileIn (rootFolder + "samples\\UnitTestUtilFns.ms")

sm = FabricMesh()
-- Init required params
nodeName = sm.name
initTriCounts = #(506, 255)

-- Create a sphere
as1 = sm.DFGInstPreset "Fabric.Exts.Geometry.Func.AddSphere" [487.000000,183.000000]   execPath:""
epm1 = sm.DFGInstPreset "Fabric.Exts.Geometry.PolygonMesh.EmptyPolygonMesh" [56.000000,178.000000]   execPath:""
sm.DFGConnect (epm1 + ".mesh") (as1 + ".this")  execPath:""
sm.DFGAddPort "radius" #DFGPortIN "Scalar" portToConnect:(as1 + ".radius") extDep:"" metaData:"" execPath:""
sm.DFGAddPort "detail" #DFGPortIN "Size" portToConnect:(as1 + ".detail") extDep:"" metaData:"" execPath:""
sm.DFGConnect (as1 + ".this") "outputValue"   execPath:""

sm.detail = 10
sm.radius= 32

FAssert (CompArray (GetTriMeshFaceCount sm) initTriCounts) "Bad Init Tri Count"
FAssert (CompBB sm 110.851) "Bad init size"

-- Validate radius is being set correctly
sm.radius = 50
FAssert (CompBB sm 172.802) "Bad 5 size"

-- Change input type for PolyMesh from none to INode
sm.DFGAddPort "this" 0 "PolygonMesh" portToConnect:(as1 + ".this")
sm.DFGRemoveNodes #(epm1)
sm.DFGRenamePort "this" "inMesh"

-- Pipe in a teapot to copy
tp = Teapot()
tp.radius = 50
tp.position = Point3 10 10 0

sm.inMesh = tp
FAssert (CompArray (GetTriMeshFaceCount sm) #(1530, 785)) "Bad Tri Count on Set Mesh"
FAssert (CompBB sm 228.915) "Bad 5 size"

-- Fix normals
rc1 = sm.DFGInstPreset "Fabric.Compounds.PolygonMesh.Modify.RecomputeNormals" [647.000000,159.000000]   execPath:""
sm.DFGMoveNodes #(rc1) #([728.000000,188.000000])   execPath:""
sm.DFGConnect (as1 + ".this") (rc1 + ".mesh")   execPath:""
sm.DFGConnect (rc1 + ".result") "outputValue"   execPath:""

-- Test clone
maxOps.cloneNodes sm cloneType:#copy newNodes:&nnl
nnl = nnl[1]
move nnl [0,-93.7512,0]
FAssert (CompArray (GetTriMeshFaceCount nnl) #(1530, 785)) "Bad Tri Count on Cloned Mesh"
FAssert (CompBB nnl 228.915) "Bad Cloned size"

-- Verify its updating correctly (and a proper clone, not an instance	
nnl.radius = 20
FAssert (CompBB nnl 213.466) "Bad Cloned size"
FAssert (CompBB sm 228.915) "Bad Cloned size"

print "All Work"
