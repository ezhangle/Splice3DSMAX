-----------------------------------------------------------------------------
-- Test every maxscript function
--   Tested:
--      This unit test simply tests MxS exposed fn's in isolation.
--   Not Tested:
--      It does not test overall functionality
-----------------------------------------------------------------------------

m = FabricMesh()

-- Cannot validate UI is shown
-- m.ShowDFGGraphEditor()

res = m.AddInputPort "newPOrt" "Float32" -2
res = m.AddOutputPort "outPort" ""
res = m.AddIOPort "ioPort" "" -2
res = m.AddIOPort "ioPort" "" -2

assert (res == "ioPort_2") message:"Didn't get right port name back?" options:#(#dialog)

m.RemovePort "ioPort"
assert (m.PortCount == 4) message:"Didn't get remove port" options:#(#dialog)

-- Check remaining ports
assert (m.GetPortName 1 == "outputValue") message:"Incorrect first port" options:#(#dialog)
assert (m.GetPortName 2 == "newPOrt") message:"Incorrect 2nd port" options:#(#dialog)
assert (m.GetPortName 3 == "outPort") message:"Incorrect 3rd port" options:#(#dialog)
assert (m.GetPortName 4 == "ioPort_2") message:"Incorrect 4th port" options:#(#dialog)

m.SetPortName "ioPort_2" "renamedPort"
assert (m.GetPortName 4 == "renamedPort") message:"Incorrect renamed port" options:#(#dialog)

m.GetPortType "newPOrt" -- Not testable until we can set type via MxS
m.IsPortArray "newPOrt" -- Not testable until we can set type via MxS

m.GetMaxType "newPOrt"
m.SetMaxType "newPOrt" 5 -- Angle
m.GetLegalMaxTypes "newPOrt"

m.SetPortMetaData "outPort" "isAwesome" "true" canUndo:false
res = m.GetPortMetaData "outPort" "isAwesome" 
assert (res == "true") message:"MetaData not set" options:#(#dialog)

--m.SetPortValue "newPOrt" (Point3 0 10 0)
m.SetPortMinMax "newPOrt" 0 10
