fn FAssert b msg = (
	assert (b) message:msg options:#(#dialog)
)

sm = SpliceMesh pos:[-27.1141,-29.5166,0] isSelected:on
sm.ShowDFGGraphEditor() -- No way to test this

res = $.DFGInstPreset "Fabric.Exts.Geometry.PolygonMesh.EmptyPolygonMesh" [212.000000,39.000000]  execPath:""
FAssert (res == "EmptyPolygonMesh") "Error creating EmptyPolyMesh node"

code =  "dfgEntry {
  // result = a + b;
}"
res = $.DFGAddFunc "func" code [412,167] execPath:""
FAssert(res == "func") ("Error creating empy func")

$.DFGAddPort "inVal" 0 "$TYPE$" portToConnect:"" extDep:"" execPath:"func"

code = "dfgEntry {
  report(lhs);
}"
$.DFGSetCode code execPath:"func"
code_ = $.GetExecCode res



sm.CloseDFGGraphEditor()