-----------------------------------------------------------------------------
-- Test import of Fabric Files.
--   This unit test validates the ability to load/save Fabric files.
--   Tested:
--      Raw scene load of Fabric file
--      Object save of splice file
--      Object load of splice file.
-----------------------------------------------------------------------------

fn DoLoadFile filepath = (
	numObjects = objects.count
	res = Fabric.loadFromFile filepath
	-- We should have a newly created mesh object
	assert (objects.count == numObjects + 1) message:("Did not create node for:" + filepath) options:#(#dialog)
	fabricObj = objects[numObjects + 1]
	fabricObj
)

fn IsEquivVec a b = (
	return (length(a - b) < 0.1)
)
-- Common data
rootFolder = systemTools.getEnvVariable("SPLICE3DSMAXDIR")
fabricDir = systemTools.getEnvVariable("FABRIC_DIR")
sampleDir = fabricDir + "Samples\\Canvas\\"
tempFile = systemTools.getEnvVariable("TEMP") + "\\temp.canvas"
-- Bring in util functions
fileIn (rootFolder + "samples\\UnitTestUtilFns.ms")

-- All the examples here actually only happen in fabric rendering space, so we need rendering to see whats happening
Fabric.rendering = true

-------------------------------------------------------------------
alembic_inspectfile = DoLoadFile (sampleDir + "alembic_inspectfile.canvas")
alembic_inspectfile.filePath = fabricDir + "Resources\\boxes.abc"
alembic_inspectfile.index = 1
FAssert(IsEquivVec (alembic_inspectfile.getArgValue "animatedPos") [4.97493,0,0]) "Bad pos from abc file"
alembic_inspectfile.index = 2
FAssert(IsEquivVec (alembic_inspectfile.getArgValue "animatedPos") [-1.07305,0,0]) "Bad pos from abc file"

-------------------------------------------------------------------
arrays_grid_points = DoLoadFile (sampleDir + "arrays_grid_points.dfg.canvas")
-- This is rendered, so we can't check it.
arrays_grid_points.U = 10
arrays_grid_points.h = 10
arrays_grid_points.width = 10
arrays_grid_points.height = 10

-------------------------------------------------------------------
arrays_interpolate_on_array = DoLoadFile (sampleDir + "arrays_interpolate_on_array.canvas")
arrays_interpolate_on_array.timeline = 10

-------------------------------------------------------------------
arrays_random_points = DoLoadFile (sampleDir + "arrays_random_points.canvas")
arrays_random_points.count = 50

-------------------------------------------------------------------
-- Do full load/save/load loop here

bullet_boxes = DoLoadFile (sampleDir + "bullet_boxes.canvas");
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
