
m = plane()
m.length = 200
m.width = 50
m.lengthSegs = 100
m.widthSegs = 100

spliceMod = SpliceModifier()
spliceMod.AddInputPort "matrixA" "Mat44" 17
spliceMod.AddInputPort "matrixB" "Mat44" 17
spliceMod.AddInputPort "tangentLength" "Scalar" -2

addmodifier m spliceMod

p1 = Point pos:[79.4925,-34.2898,0]
p2 = Point pos:[-40.0207,-43.753,0]

spliceMod.matrixA = p1;
spliceMod.matrixB = p2;
spliceMod.tangentLength = 40.0;


entryPt = "bezierDeformOp";
klCode = "
require Math;
require Geometry;

function Vec3 bezier4Point(Vec3 a, Vec3 b, Vec3 c, Vec3 d, Scalar u) {
 Vec3 ab = a.linearInterpolate(b, u);
 Vec3 bc = b.linearInterpolate(c, u);
 Vec3 cd = c.linearInterpolate(d, u);
 Vec3 abbc = ab.linearInterpolate(bc, u);
 Vec3 bccd = bc.linearInterpolate(cd, u);
 return abbc.linearInterpolate(bccd, u);
}

operator bezierDeformTask(Index index, Xfo xfoA, Xfo xfoB, Vec3 b, Vec3 c, Scalar minX, Scalar maxX, io PolygonMesh mesh) {
  Vec3 pos = mesh.getPointPosition(index);
  Scalar u = (pos.x - minX) / (maxX - minX);
  Vec3 base = bezier4Point(xfoA.tr, b, c, xfoB.tr, u);
  Quat ori = xfoA.ori.sphericalLinearInterpolate(xfoB.ori, u);
  Vec3 scl = xfoA.sc.linearInterpolate(xfoB.sc, u);
  pos = base + (ori.getZaxis() * pos.z + ori.getYaxis() * pos.y) * scl;
  mesh.setPointPosition(index, pos);
}

operator bezierDeformOp(Mat44 matrixA, Mat44 matrixB, Scalar tangentLength, io PolygonMesh modifierMesh) {

  if(modifierMesh.pointCount() == 0)return;
  Xfo xfoA, xfoB;
  xfoA.setFromMat44(matrixA);
  xfoB.setFromMat44(matrixB);

  Vec3 b, c;
  b = xfoA.transformVector(Vec3(tangentLength, 0, 0));
  c = xfoB.transformVector(Vec3(-tangentLength, 0, 0));

  Scalar minX = modifierMesh.getPointPosition(0).x;
  Scalar maxX = modifierMesh.getPointPosition(0).x;
  for(Size i=1;i<modifierMesh.pointCount();i++) {
    if(modifierMesh.getPointPosition(i).x > maxX) maxX = modifierMesh.getPointPosition(i).x;
    if(modifierMesh.getPointPosition(i).x < minX) minX = modifierMesh.getPointPosition(i).x;
  }

  bezierDeformTask<<<modifierMesh.pointCount()>>>(xfoA, xfoB, b, c, minX, maxX, modifierMesh);
}";
spliceMod.SetKLCode entryPt klCode


