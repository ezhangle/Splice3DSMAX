--------------------------------------------------------------------------
--
-- This unit test demonstrates connecting together various Splice Graphs.  This
-- would be useful in the case of using a single datasource to drive multiple outputs
--
--------------------------------------------------------------------------

rootFolder = systemTools.getEnvVariable("FABRIC3DSMAXDIR")
fileIn (rootFolder + "samples\\UnitTestUtilFns.ms")

dataSource = FabricReferenceTarget()

dataSource.DFGAddFunc "makeMatrices" "dfgEntry {
  // result = a + b;
}
" [506.776030,168.906259]  execPath:""
dataSource.DFGAddPort "count" 0 "Size" execPath:"makeMatrices"
dataSource.DFGAddPort "matrices" 2 "Mat44[]" execPath:"makeMatrices"
dataSource.DFGAddPort "time" 0 "Scalar" execPath:"makeMatrices"
dataSource.DFGSetCode "dfgEntry {
  //result = lhs + rhs;
  matrices.resize(count);
  for (UInt32 i = 0; i < count; i++) {
    matrices[i].setTranslation(Vec3(i * time * 5, i * 10, 0));
  }
}
"    execPath:"makeMatrices"
dataSource.DFGInstPreset "Fabric.Exts.Util.EvalContext.CurrentEvalContext" [-66.000000,137.000000]   execPath:""
dataSource.DFGInstPreset "Fabric.Exts.Util.EvalContext.GetTime" [-5.000000,126.000000]   execPath:""
dataSource.DFGConnect "CurrentEvalContext.context" "GetTime.this"   execPath:""
dataSource.DFGConnect "GetTime.result" "makeMatrices.time"   execPath:""

dataSource.DFGAddPort "value" 2 "Mat44[]" portToConnect:"makeMatrices.matrices"
dataSource.DFGMoveNodes #("makeMatrices") #([294.776031,155.906265])   execPath:""

-- Size is not defined in max. We therefore can't expose a "size" to max. We use a UInt32 instead
dataSource.DFGAddPort "count" 0 "UInt32" portToConnect:"makeMatrices.count" extDep:"" metaData:"" execPath:""

-- Right now this does not itterate over more than the first output. 
numOutputs = 1
-- This data source will generate 3 matrices
dataSource.count = numOutputs
dataSource.ShowDFGGraphEditor()

-- Create Max points that connect to DataSource and show its results.
pts = #()
for i = 0 to (numOutputs - 1) do (
	pt = Point()
	dfgCtrl = FabricMatrixController()
	--Show the dfg graph if desired
	--dfgCtrl.ShowDFGGraphEditor()
	
	CtrlGet = dfgCtrl.DFGInstPreset "Fabric.Core.Array.Get" [300.000000,183.000000]   execPath:""
	dfgCtrl.DFGMoveNodes #("Get") #([265.000000,168.000000])   execPath:""
	dfgCtrl.DFGAddPort "index" 0 "UInt32" portToConnect:(CtrlGet + ".index") extDep:"" metaData:"" execPath:""
	dfgCtrl.DFGAddPort "dataVal" 0 "Mat44[]" portToConnect:(CtrlGet + ".array")  extDep:"" metaData:"" execPath:""
	dfgCtrl.DFGConnect (CtrlGet + ".element")  "outputValue"   execPath:""
	dfgCtrl.ConnectArgs "dataVal" dataSource "value" postUI:(i == 0)
		
	pt.controller = dfgCtrl
	dfgCtrl.index = i
	FAssert (pt.transform.position == [0, i * 10, 0]) "Point didn't get the right position"
	append pts pt
)

sliderTime = 20f
for i = 0 to numOutputs - 1 do (
	pt = pts[i + 1]
	t = currentTime.ticks as float/4800
	expectedPosition = [i as float * t * 5, i * 10, 0]
	currPosition = pt.transform.position
	FAssert (currPosition == expectedPosition) ("Point didn't get the right position: " + (currPosition as string) + " != " +(expectedPosition as string))
)

sliderTime = 100f
for i = 0 to numOutputs - 1 do (
	pt = pts[i + 1]
	t = currentTime.ticks as float/4800
	expectedPosition = [i as float * t * 5, i * 10, 0]
	currPosition = pt.transform.position
	FAssert (currPosition == expectedPosition) ("Point didn't get the right position: " + (currPosition as string) + " != " +(expectedPosition as string))
)

print "Works if we only have numOUtputs=1"