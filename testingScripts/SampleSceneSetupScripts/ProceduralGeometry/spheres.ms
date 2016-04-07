

spheresMesh = FabricMesh()
spheresMesh.AddInputPort "nbSpheres" "Integer" -2
spheresMesh.AddInputPort "spans" "Integer" -2
spheresMesh.AddInputPort "spans2" "Integer" -2
spheresMesh.AddInputPort "shells" "Integer" -2
spheresMesh.AddInputPort "rootMat" "Mat44" 17
spheresMesh.AddInputPort "offsetMat" "Mat44" 17
spheresMesh.AddInputPort "moveA" "Vec3" -2
spheresMesh.AddInputPort "angleA" "Scalar" -2
spheresMesh.AddInputPort "angleB" "Scalar" -2
spheresMesh.AddInputPort "angleC" "Scalar" -2
spheresMesh.AddInputPort "thickness" "Scalar" -2
spheresMesh.AddInputPort "scaleFactor" "Scalar" -2

spheresMesh.OutPort = "FabricMeshVal"

spheresMesh.nbSpheres = 5;
spheresMesh.spans = 11;
spheresMesh.spans2 = 12;
spheresMesh.shells = 36;
spheresMesh.angleA = 1.539;
spheresMesh.angleB = 2.422;
spheresMesh.angleC = 6.280;
spheresMesh.thickness = 0.1;
spheresMesh.scaleFactor = 1.1;


p1 = Point pos:[79.4925,-34.2898,0]
p2 = Point pos:[-40.0207,-43.753,0]
p2.parent = p1

spheresMesh.rootMat = p1;
spheresMesh.offsetMat = p2;


entryPt = "FabricMesh";
klCode = "require Math;
require Geometry;

function createSphere(Xfo root, Integer spans, Integer spans2, io PolygonMesh sphere, Scalar angleA ,Scalar angleB)
{
  Vec3 profile[];
  for(Size i=0;i<spans;i++)
  {
    Vec3 pos(0, 1, 0.001);
    if(i > 0)
    {
      Quat q;
      q.setFromEulerAngles(Vec3(i * angleA / Scalar(spans - 1), 0.0, 0.0));
      pos = q.rotateVector(pos);
    }
    profile.push(pos);
  }

  Xfo xfos[];
  for(Size i=0;i<spans2;i++)
  {
    Xfo offset;
    offset.setIdentity();
    if(i > 0)
      offset.ori.setFromEulerAngles(Vec3(0.0, -Scalar(i) * angleB / Scalar(spans2 - 1), 0.0));
    xfos.push(root * offset);
  }

  sphere.addExtrusion(xfos, profile, false);
}

operator FabricMesh(
  io PolygonMesh FabricMeshVal, 
  in Scalar angleA, 
  in Scalar angleB, 
  in Scalar angleC, 
  in Vec3 moveA, 
  in Integer nbSpheres, 
  in Mat44 offsetMat, 
  in Mat44 rootMat, 
  in Scalar scaleFactor, 
  in Integer shells, 
  in Integer spans, 
  in Integer spans2, 
  in Scalar thickness
) {
  FabricMeshVal.clear();
  
  Xfo root;
  root.setFromMat44(rootMat);

  Xfo offset;
  offset.setFromMat44(offsetMat);
  
  Xfo inc;
  inc.setIdentity();
  inc.tr = moveA / Scalar(shells-1);
  inc.sc *= scaleFactor;
  inc.ori.setFromEulerAngles(Vec3(0.1, 0.1, 0.0));

  
  for(Size i=0;i<nbSpheres;i++)
  {
    Xfo base = root;
    if(i > 0)
    {
      Quat q;
      q.setFromEulerAngles(Vec3(0.0, 0.0, Scalar(i) * angleC / Scalar(nbSpheres)));
      base.ori = base.ori * q;
    }
    base = base * offset;
    for(Size j=0;j<shells;j++)
    {
      PolygonMesh sphere = PolygonMesh();
      createSphere(base, spans, spans2, sphere, angleA, angleB);
      FabricMeshVal.mergeMesh(sphere);
      base = base * inc;
    }
  }
  
  FabricMeshVal.addShell(thickness);
  FabricMeshVal.recomputePointNormals(Math_degToRad(60));
}
";
spheresMesh.SetKLCode entryPt klCode

