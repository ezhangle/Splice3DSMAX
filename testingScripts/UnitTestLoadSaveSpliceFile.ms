-----------------------------------------------------------------------------
-- Test import of Fabric Files.
--   This unit test validates the ability to load/save Fabric files.
--   Tested:
--      Raw scene load of Fabric file
--      Object save of splice file
--      Object load of splice file.
-----------------------------------------------------------------------------

fn DoLoadFile filepath = (
	fabricObj = FabricMesh()
	res = fabricObj.LoadFromFile filepath true
	fabricObj
)

fn IsEquivVec a b = (
	return (length(a - b) < 0.1)
)
-- Common data
rootFolder = systemTools.getEnvVariable("FABRIC3DSMAXDIR")
fabricDir = systemTools.getEnvVariable("FABRIC_DIR")
sampleDir = fabricDir + "\\Samples\\Canvas\\"
tempFile = systemTools.getEnvVariable("TEMP") + "\\temp.canvas"
-- Bring in util functions
fileIn (rootFolder + "samples\\UnitTestUtilFns.ms")

-- All the examples here actually only happen in fabric rendering space, so we need rendering to see whats happening
Fabric.rendering = true

-------------------------------------------------------------------
alembic_inspectfile = DoLoadFile (sampleDir + "Alembic\\02_alembic_get_transforms.canvas")
RedrawViews()

fget = alembic_inspectfile.DFGInstPreset "Fabric.Core.Array.Get" [967.773605,381.943403]   execPath:""
alembic_inspectfile.DFGMoveNodes #(fget) #([949.848755,357.415253])   execPath:""
alembic_inspectfile.DFGConnect "GetTransformArray_2.xfos" (fget + ".array")   execPath:""
tomat = alembic_inspectfile.DFGInstPreset "Fabric.Exts.Math.Xfo.ToMat44" [944.188698,231.943395]   execPath:""
alembic_inspectfile.DFGMoveNodes #(tomat) #([1116.830078,387.603516])   execPath:""
alembic_inspectfile.DFGConnect (fget + ".element") (tomat + ".this")   execPath:""
alembic_inspectfile.DFGAddPort "result" #DFGPortOUT "Mat44" portToConnect:(tomat + ".result")

m = $.GetArgValue "result"
FAssert(IsEquivVec m.pos [0,0,0]) "Bad pos from abc file"
$.DFGSetPortDefaultValue "Get_1.index" 1 execPath:""
m = $.GetArgValue "result"
FAssert(IsEquivVec m.pos [0,4.37156,0]) "Bad pos from abc file"
$.DFGSetPortDefaultValue "Get_1.index" 2 execPath:""
m = $.GetArgValue "result"
FAssert(IsEquivVec m.pos [0,8.68887,0]) "Bad pos from abc file"

-------------------------------------------------------------------
arrays_grid_points = DoLoadFile (sampleDir + "Miscellaneous\\arrays_grid_points.dfg.canvas")
-- This is rendered, so we can't check it.
arrays_grid_points.U = 10
arrays_grid_points.h = 10
arrays_grid_points.width = 10
arrays_grid_points.height = 10

-------------------------------------------------------------------
arrays_interpolate_on_array = DoLoadFile (sampleDir + "Miscellaneous\\arrays_interpolate_on_array.canvas")
arrays_interpolate_on_array.timeline = 10

-------------------------------------------------------------------
arrays_random_points = DoLoadFile (sampleDir + "Miscellaneous\\arrays_random_points.canvas")
arrays_random_points.count = 50

-------------------------------------------------------------------
-- Do full load/save/load loop here

bullet_boxes = DoLoadFile (sampleDir + "Bullet\\bullet_boxes.canvas");
-- Ironically, the bullet op doesn't take advantage of the EvalContext time parameter

bullet_boxes.SaveToFile tempFile
delete bullet_boxes
-- Now, we test loading and saving off a proper max operator
bullet_boxes = FabricMesh()
bullet_boxes.LoadFromFile tempFile true
for i = 1 to 200 do 
( 
	bullet_boxes.timeline = i as float / 30 
	RedrawViews()
)
