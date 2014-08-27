m = plane()
m.length = 200
m.width = 50
m.lengthSegs = 100
m.widthSegs = 100

spliceMod = SpliceModifier()
spliceMod.CreatePort "amplitude" "Scalar" -2
spliceMod.CreatePort "frequency" "Scalar" -2
spliceMod.CreatePort "phaseOffset" "Scalar" -2

addmodifier m spliceMod

entryPt = "SpliceModifier";
klCode = "
require Math;
require Geometry;

operator SpliceModifier(io PolygonMesh modifierMesh, in Scalar amplitude, in Scalar frequency, in Scalar phaseOffset, in Scalar time) {
  for (Integer i = 0; i < modifierMesh.pointCount(); i++)
  {
		Vec3 v = modifierMesh.getPointPosition(i);
		v.z = sin(time + phaseOffset + v.y * frequency / 10) * amplitude;
		modifierMesh.setPointPosition(i, v);
  }
}";
spliceMod.SetKLCode entryPt klCode

spliceMod.frequency = 1;
spliceMod.amplitude  = 10;
