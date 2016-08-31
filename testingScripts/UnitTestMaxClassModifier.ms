fn TestMaxClassModifier =
(
	-----------------------------------------------------------------------------------------------------------
	m = plane()
	m.length = 200
	m.width = 50
	m.lengthSegs = 100
	m.widthSegs = 100

	dfgMod = FabricModifier()
	addmodifier m dfgMod

	wavemodnode = dfgMod.DFGAddFunc "waveMod" "
	dfgEntry {
	  // TODO
	}
	" [573.000000,187.000000] execPath:""

	inmeshport 			= dfgMod.DFGAddPort "inMesh" 1 "PolygonMesh" portToConnect:"" extDep:"Geometry" metaData:"" execPath:"waveMod"
	phaseoffsetport 	= dfgMod.DFGAddPort "phaseOffset" 0 "Float32" portToConnect:"" extDep:"" metaData:"" execPath:"waveMod"
	frequencyport		= dfgMod.DFGAddPort "frequency" 0 "Float32" portToConnect:"" extDep:"" metaData:"" execPath:"waveMod"
	amplitudeport 		= dfgMod.DFGAddPort "amplitude" 0 "Float32" portToConnect:"" extDep:"" metaData:"" execPath:"waveMod"



	dfgMod.DFGSetCode "dfgEntry {
	  for (UInt32 i = 0; i < inMesh.pointCount(); i++)
	  {
		Float32 time = 0; //context.time
			Vec3 v = inMesh.getPointPosition(i);
			v.z = sin(time + phaseOffset + v.y * frequency / 10) * amplitude;
			inMesh.setPointPosition(i, v);
	  }
	}
	"    execPath:"waveMod"

	dfgMod.DFGConnect "baseMesh" "waveMod.inMesh"   execPath:""
	dfgMod.DFGAddPort "phaseOffset" 0 "Float32" portToConnect:"waveMod.phaseOffset" extDep:"" metaData:"" execPath:""
	dfgMod.DFGAddPort "frequency" 0 "Float32" portToConnect:"waveMod.frequency" extDep:"" metaData:"" execPath:""
	dfgMod.DFGAddPort "amplitude" 0 "Float32" portToConnect:"waveMod.amplitude" extDep:"" metaData:"" execPath:""

	dfgMod.DFGConnect "waveMod.inMesh" "outputValue"   execPath:""

	dfgMod.frequency = 1;
	dfgMod.amplitude  = 10;

	FAssert (CompBB m 207.122) "Incorrect modifier BB"

	-- Test the modifier inherits the underlying object correctly
	FAssert(CompArray (GetTriMeshFaceCount m) #(20000, 10201)) "Bad Init Tri Count"
	m.baseobject.widthSegs = 200

	-- The following assertion does not work correctly, however the graph seems to build correctly	
	FAssert (CompArray (GetTriMeshFaceCount m) #(40000, 20301)) "Bad Init Tri Count"
)