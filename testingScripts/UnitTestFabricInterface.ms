-----------------------------------------------------------
--
-- Simple testing script exercises the interface exposed
-- by all Fabric classes.  It only performs superficial testing,
-- its testing the interface, not so much the underlying 
-- functionality
--
-----------------------------------------------------------

rootFolder = systemTools.getEnvVariable("FABRIC3DSMAXDIR")
fileIn (rootFolder + "samples\\UnitTestUtilFns.ms")

sm = FabricMesh pos:[-27.1141,-29.5166,0] isSelected:on
FAssert((sm.GetPortCount() == 1) and ((sm.GetPortName 1) == "outputValue")) "Error Initializing FabricMesh"
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
FAssert($.GetMaxTypeForArg res == 1) "Created bad Max type"
FAssert($.GetPortCount() == 2) "Error in port count"
$.DFGRenamePort res "aDiffPort"  execPath:""
FAssert($.GetPortName 2 == "aDiffPort") "Error in port name"
$.DFGRemovePort "aDiffPort"   execPath:""
	
$.DFGAddGraph "graph" [317.000000,137.000000]  execPath:""
$.DFGMoveNodes ("graph") ([78.000000,96.000000])  execPath:""
$.DFGAddBackDrop "backdrop" [180.000000,196.000000]  execPath:""
$.DFGResizeBackDrop "backdrop" [180.000000,196.000000] [230.000000,94.000000] execPath:""
$.DFGMoveNodes ("backdrop") ([92.000000,146.000000])  execPath:""
FAssert($.GetNodeCount() == 4) "Erroring making new nodes"
$.DFGRemoveNodes #("backdrop", "graph")  execPath:""
FAssert($.GetNodeCount() == 2) "Erroring deleting new nodes"

res = $.DFGAddPort "aport" 0 "UInt32" portToConnect:"" extDep:"" execPath:""
$.DFGAddVar "myVar" "UInt32" "" [255.000000,267.000000] execPath:""
FAssert($.GetNodeCount() == 3) "Erroring deleting new nodes"
$.DFGSetNodeComment "myVar" "This is a variable"  execPath:""
$.DFGDisconnect "EmptyPolygonMesh.mesh" "func.inVal"  execPath:""
$.DFGConnect "myVar.value" "func.inVal"  execPath:""
$.DFGAddPort "inVal" 2 "UInt32" portToConnect:"func.inVal" extDep:"" execPath:""
$.DFGSetPortDefaultValue "func.inVal" 10   execPath:""
$.DFGConnect "aport" "myVar.value"  execPath:""

$.DFGAddGet "get" "myVar" [172.000000,270.000000] execPath:""
$.DFGAddSet "set" "myVar" [171.000000,312.000000] execPath:""
$.DFGMoveNodes ("set") ([58.000000,152.000000])  execPath:""
FAssert($.GetNodeCount() == 5) "Erroring deleting new nodes"

$.DFGSetNodeTitle "func" "PrintFunc"  execPath:""

$.DFGSetArgType "aport" "Float32"
FAssert($.GetMaxTypeForArg "aport" == 0) "Resetting port type"

legalTypes = $.GetLegalMaxTypesForArg "aport"
FAssert legalTypes[6] "Get legal types"
$.SetMaxTypeForArg "aport" 6

maxType = $.GetPortMetaData "aport" "MaxType"
FAssert (maxType == "6") "Set MaxType"

-- Test this
$.DFGEditPort "aport" desiredNewPortName:"aport" typeSpec:"Float32" metadata:"{
  \"uiHidden\" : \"\",
  \"uiOpaque\" : \"\",
  \"uiPersistValue\" : \"\",
  \"uiRange\" : \"(0, 10)\",
  \"uiCombo\" : \"\",
  \"MaxType\" : 1
  }" execPath:""

uiRange = $.GetPortMetaData "aport" "uiRange"
FAssert (uiRange == "(0, 10)") "Set UIRange"
$.SetArgMinMax "aport" 0 5
uiRange = $.GetPortMetaData "aport" "uiRange"
FAssert (uiRange == "(0,5)") "ReSet UIRange"

$.DFGImplodeNodes #("func", "get", "set") "graph"  execPath:""
FAssert($.GetNodeCount() == 3) "imploding nodes"
$.DFGExplodeNode "graph"   execPath:""
FAssert($.GetNodeCount() == 5) "Explode nodes"

$.DFGSetExtDeps ("Geometry")    execPath:""

$.aport = 17 -- Note, this won't trigger an evaluate, so won't get pushed through the graph
$.DFGSetArgValue "aport" 17
FAssert($.GetArgValue "aport" == 17) "Getting val"

$.DFGReorderPorts #(1, 2, 0)    execPath:""

tmpFile = systemTools.getEnvVariable("TEMP") + "\\tmpFile.canvas"
$.SaveToFile tmpFile
jsonString = $.ExportToJSON()
 -- Compare the jsonString & tmpFile to see if they are equivalent
--fin = openFile tmpFile mode:"r"
--seek fin #eof
--maxlen=filepos fin
--seek fin 0
--res = readChars fin maxlen errorAtEOF:false
--res == jsonString
-- From KDIff3  Files are equivalent, but not binary equal (so our test fails)

FAssert($.GetNodeCount() == 5) "InitValue"
$.DFGRemoveNodes #("EmptyPolygonMesh", "func")    execPath:""
FAssert($.GetNodeCount() == 3) "First Removed"
$.LoadFromFile tmpFile true
FAssert($.GetNodeCount() == 5) "File Restore"
$.DFGRemoveNodes #("EmptyPolygonMesh", "func")    execPath:""
FAssert($.GetNodeCount() == 3) "Second Removed"
$.RestoreFromJSON jsonString true
FAssert($.GetNodeCount() == 5) "JSON Restore"

sm.CloseDFGGraphEditor()
