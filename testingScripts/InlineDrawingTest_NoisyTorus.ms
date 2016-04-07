m = FabricMesh()
m.AddInputPort "speed" "Scalar" -2
m.AddInputPort "factor1" "Scalar" -2
m.AddInputPort "factor2" "Scalar" -2
m.AddInputPort "mesh" "PolygonMesh" -1

entryPt = "drawingPoints";
klCode = "


require Math;
require Geometry;
require Singletons;
require InlineDrawing;

function Float32 fade(Float32 t)
{
  return t * t * t * (t * (t * 6 - 15) + 10);
}

function Float32 lerp(Float32 t, Float32 a, Float32 b)
{
  return a + t * (b - a);
}

function Float32 grad(SInt32 hash, Float32 x, Float32 y, Float32 z) 
{
  SInt32 h = hash & 15;
  Float32  u = h < 8 ? x : y,
      v = h < 4 ? y : h==12||h==14 ? x : z;
  return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

function Float32 pnoise (Vec3 input)
{
  Float32 x = input.x;
  Float32 y = input.y;
  Float32 z = input.z;

  const Float32 p[] =
  [
    151, 160, 137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233,
    7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
    190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219,
    203, 117, 35, 11, 32, 57, 177, 33, 88, 237, 149, 56, 87, 174, 20,
    125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
    77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105,
    92, 41, 55, 46, 245, 40, 244, 102, 143, 54, 65, 25, 63, 161, 1,
    216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196, 135,
    130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52,
    217, 226, 250, 124, 123, 5, 202, 38, 147, 118, 126, 255, 82, 85,
    212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42, 223, 183,
    170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155,
    167, 43, 172, 9, 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224,
    232, 178, 185, 112, 104, 218, 246, 97, 228, 251, 34, 242, 193, 238,
    210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239,
    107, 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115,
    121, 50, 45, 127, 4, 150, 254, 138, 236, 205, 93, 222, 114, 67, 29,
    24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180, 151, 160,
    137, 91, 90, 15, 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140,
    36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23, 190, 6, 148, 247,
    120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32,
    57, 177, 33, 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68,
    175, 74, 165, 71, 134, 139, 48, 27, 166, 77, 146, 158, 231, 83, 111,
    229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
    102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187,
    208, 89, 18, 169, 200, 196, 135, 130, 116, 188, 159, 86, 164, 100,
    109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123, 5, 202, 38,
    147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17,
    182, 189, 28, 42, 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163,
    70, 221, 153, 101, 155, 167, 43, 172, 9, 129, 22, 39, 253, 19, 98,
    108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
    251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51,
    145, 235, 249, 14, 239, 107, 49, 192, 214, 31, 181, 199, 106, 157,
    184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254, 138, 236, 205,
    93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156,
    180
  ];

  SInt32   X = SInt32(floor(x)) & 255;
  SInt32   Y = SInt32(floor(y)) & 255;
  SInt32   Z = SInt32(floor(z)) & 255;
  x -= floor(x);
  y -= floor(y);
  z -= floor(z);
  Float32  u = fade(x);
  Float32  v = fade(y);
  Float32  w = fade(z);
  SInt32  A = p[X]+Y,
   AA = p[A]+Z, 
   AB = p[A+1]+Z,
   B = p[X+1]+Y, 
   BA = p[B]+Z, 
   BB = p[B+1]+Z;

  return
    lerp(w,lerp(v,lerp(u, grad(p[AA  ], x, y, z),
                  grad(p[BA  ], x-1, y, z)),
          lerp(u, grad(p[AB  ], x, y-1, z),
                  grad(p[BB  ], x-1, y-1, z))),
          lerp(v, lerp(u, grad(p[AA+1], x, y, z-1 ),
                  grad(p[BA+1], x-1, y, z-1)),
          lerp(u, grad(p[AB+1], x, y-1, z-1),
                  grad(p[BB+1], x-1, y-1, z-1))));
}

operator deform<<<index>>>(
  Float32 time,
  Float32 speed,
  Float32 factor1,
  Float32 factor2,
  io Vec3 positions[],
  Vec3 normals[],
  Vec3 originalPositions[],
  Vec3 originalNormals[]
) {
    Vec3 randomVel;
    // randomVel.x = Math_linearInterpolate(-1.0, 1.0, mathRandomScalar(index * 3, 18));
    // randomVel.y = Math_linearInterpolate(-1.0, 1.0, mathRandomScalar(index * 4, 19));
    // randomVel.z = Math_linearInterpolate(-1.0, 1.0, mathRandomScalar(index * 5, 20));
    randomVel = originalNormals[index];
    //randomVel *= 0.05;

    // Scalar a = Math_linearInterpolate(0.01, 3.14, mathRandomScalar(index * 6, 21));
    // Scalar f = Math_linearInterpolate(0.1, 1.0, mathRandomScalar(index * 7, 22));
    Scalar a = Math_linearInterpolate(0.01, 3.14, pnoise(originalPositions[index] * factor1));
    Scalar f = Math_linearInterpolate(0.1, 1.0, pnoise(originalPositions[index] * factor2 + Vec3(0.0, 1.0, 0.0) * time * 0.1));

    Scalar factor = 0.5 + 0.5 * sin(a + f * time * speed * 0.05);
    positions[index] = originalPositions[index] + randomVel * factor;
}

operator drawingPoints(Scalar speed, Scalar factor1, Scalar factor2, io PolygonMesh mesh, in EvalContext context) {

  GeometryAttributes attributes = null;
  Vec3Attribute positionsAttr = null;
  Vec3Attribute normalsAttr = null;
  Vec3Attribute originalPositionsAttr = null;
  Vec3Attribute originalNormalsAttr = null;
  if(mesh.pointCount() == 0) {
    
    mesh.addTorus(Xfo(), 1, 4, 64, true, true);
    InlineShape shape = InlineMeshShape('shape', mesh);

	  attributes = mesh.getAttributes();
    positionsAttr = attributes.getAttribute('positions', Vec3Attribute);
    normalsAttr = attributes.getAttribute('normals', Vec3Attribute);
    originalPositionsAttr = attributes.getOrCreateVec3Attribute('originalPositions');
    originalNormalsAttr = attributes.getOrCreateVec3Attribute('originalNormals');

    // clone the positions and normals
    originalPositionsAttr.values = positionsAttr.values.clone();
    originalNormalsAttr.values = normalsAttr.values.clone();

    // setup the shader and instance
    InlineDrawing drawing = OGLInlineDrawing_GetInstance();
    InlineShader shader = drawing.registerShader(OGLSurfaceShader());
    InlineMaterial mat = shader.getOrCreateMaterial('default');
    mat.setUniform('u_diffuseColor', Color(0.2, 0.5, 1.0));
    InlineTransform transfo = StaticInlineTransform('trafo', drawing.getRoot(), Xfo(Vec3(), Quat(Euler(0,PI, 0)), Vec3(20.0, 20.0, 2.0)));
    SimpleInlineInstance instance('shape', transfo, shape, mat);
  }
  else{
    attributes = mesh.getAttributes();
    positionsAttr = attributes.getAttribute('positions', Vec3Attribute);
    normalsAttr = attributes.getAttribute('normals', Vec3Attribute);
    originalPositionsAttr = attributes.getOrCreateVec3Attribute('originalPositions');
    originalNormalsAttr = attributes.getOrCreateVec3Attribute('originalNormals');
  }


  Size numPositions = positionsAttr.values.size();
  deform<<<numPositions>>>(
    context.time, 
    speed,
    factor1,
    factor2,
    positionsAttr.values, 
    normalsAttr.values,
    originalPositionsAttr.values,
    originalNormalsAttr.values
  );
  mesh.incrementPointPositionsVersion();
  mesh.getNormals().incrementVersion();
}";
m.SetKLCode entryPt klCode

m.speed = 100.0f
m.factor1 = 0.6f
m.factor2 = 0.5f

Fabric.Rendering = true
