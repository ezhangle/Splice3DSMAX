

spheresMesh = FabricMesh()
spheresMesh.AddInputPort "fraction" "Scalar" -2
spheresMesh.AddInputPort "close" "Boolean" -2
spheresMesh.AddInputPort "size" "Scalar" -2
spheresMesh.AddInputPort "shellThickness" "Scalar" -2
spheresMesh.OutPort = "geo"

spheresMesh.fraction = 0.8;
spheresMesh.size = 8.0;
spheresMesh.shellThickness = 2.0;

entryPt = "revolution";
klCode = "
require Geometry;
require Math;

operator revolution(Scalar fraction, Boolean close, Scalar size, Scalar shellThickness, io PolygonMesh geo) {
  geo.clear();
  
  Vec3 profile[];
  profile.push(Vec3(3, 0, 0) * size);
  profile.push(Vec3(4, 0, 3) * size);
  profile.push(Vec3(7, 0, 6) * size);
  profile.push(Vec3(1, 0, 9) * size);

  Xfo root;
  Vec3 center(0, 0, 0);
  Vec3 axis(0, 0, 1);
  UInt32 detail = 48;
  Vec2 range(0, fraction);

  geo.addRevolution(root, profile, center, axis, detail, range, close);
  geo.addShell(shellThickness);
  geo.recomputePointNormals(Math_degToRad(60.0));
}
";
spheresMesh.SetKLCode entryPt klCode

