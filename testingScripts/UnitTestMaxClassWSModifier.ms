tp = Teapot radius:40 pos:[-14.2223,31.2546,0]
sp = Sphere radius:40 smooth:on segs:32 pos:[58.9291,-39.6191,0]

spliceMod = Fabric_WSModifier()
addModifier sp spliceMod

spliceMod.AddInputPort "projMesh" "PolygonMesh" -2
spliceMod.AddInputPort "projMtx" "Mat44" 17
spliceMod.AddInputPort "offset" "Scalar" -2

spliceMod.projMesh = tp
spliceMod.projMtx = tp
spliceMod.offset = 35

klCode = "
require Math;
require Geometry;

operator ParallelShrinkWrap<<<index>>>( io PolygonMesh source, SpatialQueryable target, Scalar offset, Mat44 srcToTarget, Mat44 targToSource ) {
  Ray ray;
  ray.start = srcToTarget * source.getPointPosition( index );
  ray.direction = -ray.start.unit();//Unit vector toward targets center

  GeometryLocation location = target.raycast(ray, true, 0.0, SCALAR_INFINITE);
  if( location.isValid() ) {
    // How close to source was this hit?
    Vec3 hitPos = target.getPositionAtLocation( location );
    // If the hit is closer than 10 units, then push back to 10 units
    Vec3 vFromHit = ray.start - hitPos;
    Scalar hitDist = vFromHit.length();
    if (hitDist < offset) {
      vFromHit *= offset / hitDist;
      hitPos += vFromHit;
      source.setPointPosition( index, targToSource * hitPos );
    }
  }
}
operator FabricWSModifier(
  io PolygonMesh modifierMesh, 
  in Mat44 nodeTransform, 
  in PolygonMesh projMesh, 
  in Mat44 projMtx,  
  in Scalar offset
){
  PolygonMesh source = modifierMesh;
  PolygonMesh target = projMesh;
  
  //Initialize options to use a sparse grid
  GenericValueContainer options = GenericValueContainer();
  PrepareForSpatialQueries_setSparseGrid(options);

  //Prepare for one query per source point

  Mat44 srcToTarget = nodeTransform * projMtx.inverse();
  Mat44 targToSource = srcToTarget.inverse();
  
  target.prepareForSpatialQueries( source.pointCount(), options );
  ParallelShrinkWrap<<<source.pointCount()>>>( source, target, offset, srcToTarget, targToSource );
}
"

spliceMod.SetKLCode "FabricWSModifier" klCode
