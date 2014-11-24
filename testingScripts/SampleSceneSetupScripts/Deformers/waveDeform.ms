
m = plane()
m.length = 200
m.width = 50
m.lengthSegs = 100
m.widthSegs = 100

spliceMod = SpliceModifier()
spliceMod.AddInputPort "amplitude" "Scalar" -2
spliceMod.AddInputPort "frequency" "Scalar" -2

addmodifier m spliceMod

spliceMod.amplitude = 15.0;
spliceMod.frequency = 2.0;

entryPt = "waveDeformOp";
klCode = "
require Math;
require Geometry;

operator perVertexTask<<<index>>>(Scalar amplitude, Scalar frequency, io PolygonMesh mesh, Scalar time) {
  Vec3 pos = mesh.getPointPosition(index);
  pos.y = amplitude * sin(frequency * pos.x + time);
  mesh.setPointPosition(index, pos);
}

operator waveDeformOp(io PolygonMesh modifierMesh, Scalar amplitude, Scalar frequency, EvalContext context) {
  perVertexTask<<<modifierMesh.pointCount()>>>(amplitude, frequency, modifierMesh, context.time);
  modifierMesh.recomputePointNormals();
}
";
spliceMod.SetKLCode entryPt klCode


