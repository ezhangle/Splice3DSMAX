fn DoAssert b msg = (
	assert (b) message:msg options:#(#dialog)
)

sm = SpliceMesh pos:[-27.1141,-29.5166,0] isSelected:on
sm.ShowDFGGraphEditor() -- No way to test this

$.DFGInstPreset "Fabric.Exts.Geometry.PolygonMesh.EmptyPolygonMesh" [212.000000,39.000000]  execPath:""

code =  "dfgEntry {
  // result = a + b;
}"
$.DFGAddFunc "func" code [412,167] execPath:""
code = "dfgEntry {
  report(lhs);
}"
$.DFGSetCode code execPath:"func"




sm.CloseDFGGraphEditor()