m = FabricMesh()
m.AddInputPort "numPoints" "Integer" -2
m.AddInputPort "radius" "Scalar" -2
m.AddInputPort "points" "Points" -1

entryPt = "drawingPoints";
klCode = "
require Geometry;
require InlineDrawing;

function Vec3 randomVector(Scalar radius, io Size offset)
{
  const Size seed = 16841;
  return Vec3(
    (mathRandomFloat32(seed, offset++) - 0.5) * radius,
    (mathRandomFloat32(seed, offset++) - 0.5) * radius,
    (mathRandomFloat32(seed, offset++) - 0.5) * radius
    );
}

operator drawingPoints(Integer numPoints, Scalar radius, io Points points) {

  InlineDrawing drawing = OGLInlineDrawing_GetInstance();

  if(points.size() == 0){
    InlineShader flatShader = drawing.registerShader(OGLFlatShader(\"OGLFlatShader\"));

    InlineMaterial pointsMaterial = flatShader.getOrCreateMaterial(\"points\");

    InlineTransform pointsTransform = StaticInlineTransform(\"pointsTransform\", Xfo());
    drawing.getRoot().addChild(pointsTransform);
    InlineShape shape = InlinePointsShape(\"points\", points);

    InlineInstance instance = SimpleInlineInstance(\"points\", pointsTransform, shape, pointsMaterial);
    instance.setInstanceUniform(InlineUniform(\"u_diffuseColor\", Color(1.0, 0.0, 0.0)));
  }

  Size offset = 0;
  points.resize(numPoints);
  for( Size i = 0; i < numPoints; ++i ) {
    points.setPosition( i, randomVector( radius, offset ) );
  }
}";
m.SetKLCode entryPt klCode

m.radius = 10.0f
m.numPoints = 100

Fabric.Rendering = true
