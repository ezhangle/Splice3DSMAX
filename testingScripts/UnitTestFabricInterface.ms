fn FAssert b msg = (
	assert (b) message:msg options:#(#dialog)
)

sm = SpliceMesh pos:[-27.1141,-29.5166,0] isSelected:on
FAssert((sm.GetPortCount() == 1) and ((sm.GetPortName 1) == "outputValue")) "Error Initializing SpliceMesh"
sm.ShowDFGGraphEditor() -- No way to test this

res = $.DFGInstPreset "Fabric.Exts.Geometry.PolygonMesh.EmptyPolygonMesh" [212.000000,39.000000]  execPath:""
FAssert (res == "EmptyPolygonMesh") "Error creating EmptyPolyMesh node"

code =  "dfgEntry {
  // result = a + b;
}"
res = $.DFGAddFunc "func" code [412,167] execPath:""
FAssert(res == "func") ("Error creating empy func")
code_ = $.GetExecCode res

FAssert(sm.GetPortCount execPath:"func" == 0) "Error Initializing func"
$.DFGAddPort "inVal" 0 "$TYPE$" portToConnect:"" extDep:"" execPath:"func"
FAssert(sm.GetPortCount execPath:"func" == 1) "Error Initializing func port"

code = "dfgEntry {
  report(inVal);
}"
$.DFGSetCode code execPath:"func"
code_ = $.GetExecCode res -- TODO
FAssert(code_ == code) "Erroring setting func code"

FAssert($.GetNodeCount() == 2) "Erroring getting node count"
FAssert($.GetNodeName 1 == "EmptyPolygonMesh") "Erroring getting node name"
FAssert($.GetNodeType "EmptyPolygonMesh" == 0) "Erroring getting node type"

$.DFGConnect "EmptyPolygonMesh.mesh" "func.inVal"  execPath:""

res = $.DFGAddPort "aport" 0 "UInt32" portToConnect:"" extDep:"" execPath:""
FAssert($.GetPortType res == "UInt32") "Error creating port type"
FAssert($.GetMaxTypeForArg res
FAssert($.GetPortCount() == 2) "Error in port count"
$.DFGRenamePort res "aDiffPort"  execPath:""
FAssert($.GetPortName 2 == "aDiffPort") "Error in port name"
$.DFGRemovePort "aDiffPort"   execPath:""

sm.CloseDFGGraphEditor()