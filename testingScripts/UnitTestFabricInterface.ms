-----------------------------------------------------------
--
-- Simple testing script exercises the interface exposed
-- by all Fabric classes.  It only performs superficial testing,
-- its testing the interface, not so much the underlying 
-- functionality

-----------------------------------------------------------
fn TestFabricInterface = 
(
	sm = FabricMesh pos:[-27.1141,-29.5166,0] isSelected:on
	FAssert((sm.GetPortCount() == 2) and ((sm.GetPortName 2) == "outputValue")) "Error Initializing FabricMesh"
	sm.ShowDFGGraphEditor() -- No way to test this

	epm = sm.DFGInstPreset "Fabric.Exts.Geometry.PolygonMesh.EmptyPolygonMesh" [212.000000,39.000000]  execPath:""
	FAssert (epm == "EmptyPolygonMesh_1") "Error creating EmptyPolyMesh node"

	code =  "dfgEntry {
	  // result = a + b;
	}"
	func = sm.DFGAddFunc "func" code [412,167] execPath:""
	FAssert(func == "func") ("Error creating empy func")
	code_ = sm.GetExecCode func

	FAssert(sm.GetPortCount execPath:func == 1 and sm.GetPortName 1 execPath:func == "exec") "Error Initializing func"
	sm.DFGAddPort "inVal" 0 "$TYPE$" portToConnect:"" extDep:"" execPath:func
	FAssert(sm.GetPortCount execPath:func == 2) "Error Initializing func port"

	code = "dfgEntry {
	  report(inVal);
	}"
	sm.DFGSetCode code execPath:func
	code_ = sm.GetExecCode func -- TODO
	FAssert(code_ == code) "Erroring setting func code"

	FAssert(sm.GetNodeCount() == 2) "Erroring getting node count"
	FAssert(sm.GetNodeName 1 == epm) "Erroring getting node name"
	FAssert(sm.GetNodeType epm == 0) "Erroring getting node type"

	sm.DFGConnect (epm + ".mesh") (func + ".inVal")  execPath:""

	res = sm.DFGAddPort "aport" #DFGPortIN "UInt32" portToConnect:"" extDep:"" execPath:""
	FAssert(sm.GetPortType res == "UInt32") "Error creating port type"
	FAssert(sm.GetMaxTypeForArg res == 1) "Created bad Max type"
	FAssert(sm.GetPortCount() == 3) "Error in port count"
	res = sm.DFGRenamePort res "aDiffPort"  execPath:""
	FAssert(sm.GetPortName 3 == res) "Error in port name"
	sm.DFGRemovePort res execPath:""
		
	graph = sm.DFGAddGraph "graph" [317.000000,137.000000]  execPath:""
	sm.DFGMoveNodes (graph) ([78.000000,96.000000])  execPath:""
	sm.DFGAddBackDrop "backdrop" [180.000000,196.000000]  execPath:"" -- Note, this call does not return a string (all other adds do)
	sm.DFGResizeBackDrop "backDrop" [180.000000,196.000000] [230.000000,94.000000] execPath:""
	sm.DFGMoveNodes ("backDrop") ([92.000000,146.000000])  execPath:""
	FAssert(sm.GetNodeCount() == 4) "Erroring making new nodes"
	sm.DFGRemoveNodes #("backDrop", graph)  execPath:""
	FAssert(sm.GetNodeCount() == 2) "Erroring deleting new nodes"

	aport = sm.DFGAddPort "aport" #DFGPortIN "UInt32" portToConnect:"" extDep:"" execPath:""
	aVar = sm.DFGAddVar "myVar" "UInt32" "" [255.000000,267.000000] execPath:""
	FAssert(sm.GetNodeCount() == 3) "Erroring creating var node"
	sm.DFGSetNodeComment aVar "This is a variable"  execPath:""
	sm.DFGDisconnect (epm + ".mesh") (func + ".inVal")  execPath:""
	sm.DFGConnect (aVar + ".value") (func + ".inVal")  execPath:""
	outVal = sm.DFGAddPort "outVal" #DFGPortOUT "UInt32" portToConnect:(func + ".inVal") extDep:"" execPath:""
	sm.DFGSetPortDefaultValue (func + ".inVal") 10   execPath:""
	sm.DFGConnect "aport" (aVar + ".value")  execPath:""

	aGet = sm.DFGAddGet "get" "myVar" [172.000000,270.000000] execPath:""
	aSet = sm.DFGAddSet "set" "myVar" [171.000000,312.000000] execPath:""
	sm.DFGMoveNodes (aSet) ([58.000000,152.000000])  execPath:""
	FAssert(sm.GetNodeCount() == 5) "Erroring creating Get/Set"

	--// We need to change the port type of mVar before we change the port type of aport
	sm.DFGRemoveNodes aVar  execPath:""
	aVar = sm.DFGAddVar "myVar" "Float32" "" [255.000000,267.000000] execPath:""

	sm.DFGEditPort aport #DFGPortIN typeSpec:"Float32"
	FAssert(sm.GetMaxTypeForArg aport == 0) "Resetting port type"

	sm.DFGConnect aport (aVar + ".value")  execPath:""

	-- Test changing the input type to Percent
	legalTypes = sm.GetLegalMaxTypesForArg aport
	FAssert legalTypes[6] "Get legal types"
	sm.SetMaxTypeForArg aport 6

	maxType = sm.GetPortMetaData "aport" "MaxType"
	FAssert (maxType == "6") "Set MaxType"

	-- Test that the input type is appropriately reflected in the values coming through
	sm.aport = 17
	redrawViews()
	FAssert(sm.GetArgValue "aport" == 0.17) "Getting val"

	sm.DFGEditPort aport #DFGPortIN metadata:"{
	  \"uiHidden\" : \"\",
	  \"uiOpaque\" : \"\",
	  \"uiPersistValue\" : \"\",
	  \"uiHardRange\" : \"(0, 10)\",
	  \"uiCombo\" : \"\",
	  \"MaxType\" : \"6\"
	  }" execPath:""

	uiRange = sm.GetPortMetaData aport "uiHardRange"
	FAssert (uiRange == "(0, 10)") "Set UIRange"

	sm.SetArgMinMax "aport" 0 5
	uiRange = sm.GetPortMetaData aport "uiHardRange"
	FAssert (uiRange == "(0, 5)") "ReSet UIRange"

	impGraph = sm.DFGImplodeNodes #(func, aGet, aSet) "graph"  execPath:""
	FAssert(sm.GetNodeCount() == 3) "imploding nodes"
	sm.DFGExplodeNode impGraph   execPath:""
	FAssert(sm.GetNodeCount() == 5) "Explode nodes"

	sm.DFGSetExtDeps ("Geometry")    execPath:""

	sm.DFGReorderPorts "" #(0, 3, 1, 2)    execPath:""

	tmpFile = systemTools.getEnvVariable("TEMP") + "\\tmpFile.canvas"
	sm.SaveToFile tmpFile
	jsonString = sm.ExportToJSON()
	 -- Compare the jsonString & tmpFile to see if they are equivalent
	--fin = openFile tmpFile mode:"r"
	--seek fin #eof
	--maxlen=filepos fin
	--seek fin 0
	--res = readChars fin maxlen errorAtEOF:false
	--res == jsonString
	-- From KDIff3  Files are equivalent, but not binary equal (so our test fails)

	FAssert(sm.GetNodeCount() == 5) "InitValue"
	sm.DFGRemoveNodes #(epm, func)    execPath:""
	FAssert(sm.GetNodeCount() == 3) "First Removed"
	sm.LoadFromFile tmpFile true
	FAssert(sm.GetNodeCount() == 5) "File Restore"
	sm.DFGRemoveNodes #(epm, func)    execPath:""
	FAssert(sm.GetNodeCount() == 3) "Second Removed"
	sm.RestoreFromJSON jsonString true
	FAssert(sm.GetNodeCount() == 5) "JSON Restore"

	sm.CloseDFGGraphEditor()
)
