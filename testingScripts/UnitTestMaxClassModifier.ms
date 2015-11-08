rootFolder = systemTools.getEnvVariable("SPLICE3DSMAXDIR")
fileIn (rootFolder + "UnitTestUtilFns.ms")


m = plane()
m.length = 200
m.width = 50
m.lengthSegs = 100
m.widthSegs = 100

dfgMod = FabricModifier()
addmodifier m dfgMod

dfgMod.DFGAddFunc "waveMod" "
dfgEntry {
  // TODO
}
" [573.000000,187.000000]

dfgMod.DFGAddPort "inMesh" 1 "PolygonMesh" portToConnect:"" extDep:"" metaData:"" execPath:"waveMod"
dfgMod.DFGAddPort "phaseOffset" 0 "Float32" portToConnect:"" extDep:"" metaData:"" execPath:"waveMod"
dfgMod.DFGAddPort "frequency" 0 "Float32" portToConnect:"" extDep:"" metaData:"" execPath:"waveMod"
dfgMod.DFGAddPort "amplitude" 0 "Float32" portToConnect:"" extDep:"" metaData:"" execPath:"waveMod"

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

dfgMod.DFGConnect "BaseMesh" "waveMod.inMesh"   execPath:""
dfgMod.DFGAddPort "phaseOffset" 0 "Float32" portToConnect:"waveMod.phaseOffset" extDep:"" metaData:"" execPath:""
dfgMod.DFGAddPort "frequency" 0 "Float32" portToConnect:"waveMod.frequency" extDep:"" metaData:"" execPath:""
dfgMod.DFGAddPort "amplitude" 0 "Float32" portToConnect:"waveMod.amplitude" extDep:"" metaData:"" execPath:""

dfgMod.DFGConnect "waveMod.inMesh" "outputValue"   execPath:""

dfgMod.frequency = 1;
dfgMod.amplitude  = 10;

assert (CompBB m 207.122) message:"Incorrect modifier BB" options:#(#dialog)

-- Test the modifier inherits the underlying object correctly
assert (CompArray (GetTriMeshFaceCount m) #(20000, 10201)) message:"Bad Init Tri Count" options:#(#dialog)
m.baseobject.widthSegs = 200
assert (CompArray (GetTriMeshFaceCount m) #(40000, 20301)) message:"Bad Init Tri Count" options:#(#dialog)	
