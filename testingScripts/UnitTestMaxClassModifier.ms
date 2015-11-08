fn CompArray a b = (a as string == b as string)
fn CompBB anode expSize = (
	bb = (nodeLocalBoundingBox anode)
	size = length (bb[1] - bb[2])
	res =abs(size - expSize) < 0.001
)

m = plane()
m.length = 200
m.width = 50
m.lengthSegs = 100
m.widthSegs = 100

spliceMod = FabricModifier()
spliceMod.AddInputPort "amplitude" "Scalar" -2
spliceMod.AddInputPort "frequency" "Scalar" -2
spliceMod.AddInputPort "phaseOffset" "Scalar" -2

addmodifier m spliceMod

entryPt = "FabricModifier";
klCode = "
require Math;
require Geometry;

operator FabricModifier(io PolygonMesh modifierMesh, in Scalar amplitude, in Scalar frequency, in Scalar phaseOffset, in EvalContext context) {
  for (Integer i = 0; i < modifierMesh.pointCount(); i++)
  {
		Vec3 v = modifierMesh.getPointPosition(i);
		v.z = sin(context.time + phaseOffset + v.y * frequency / 10) * amplitude;
		modifierMesh.setPointPosition(i, v);
  }
}";
spliceMod.SetKLCode entryPt klCode

spliceMod.frequency = 1;
spliceMod.amplitude  = 10;

assert (CompBB m 207.122) message:"Incorrect modifier BB" options:#(#dialog)

-- Test the modifier inherits the underlying object correctly
assert (CompArray (GetTriMeshFaceCount m) #(20000, 10201)) message:"Bad Init Tri Count" options:#(#dialog)
m.baseobject.widthSegs = 200
assert (CompArray (GetTriMeshFaceCount m) #(40000, 20301)) message:"Bad Init Tri Count" options:#(#dialog)	
