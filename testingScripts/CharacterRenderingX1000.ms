resetMaxFile #noPrompt
thisfile = getThisScriptFilename()
witcherFbx =  (getFilenamePath thisfile) + "testfiles/Witcher_Creep.fbx"


m = SpliceMesh()
m.AddInputPort "filePath" "String" -2
m.AddInputPort "numCharacters" "Integer" -2
m.AddInputPort "spacing" "Scalar" -2
m.AddInputPort "fbxCharacter" "FbxCharacter" -1 Extension:"FbxHelpers"
m.AddInputPort "characters" "Character" -1 isArray:true Extension:"Characters"
m.AddInputPort "handles" "DrawingHandle" -1 isArray:true Extension:"Characters"
m.AddInputPort "poseTextures" "InlineMatrixArrayTexture" -1 isArray:true Extension:"InlineDrawing"

entryPt = "characterDrawing"
klCode = "

require InlineDrawing;
require FbxHelpers;

operator evalCharacter<<<index>>>(
  Scalar time,
  io Character characters[], 
  io InlineMatrixArrayTexture poseTextures[]
  ){
  const Size seed = 9819815;
  ClipPose pose = characters[index].getPose();
  pose.evaluate(time + (mathRandomFloat32(seed, index) * 5.0));
  poseTextures[index].update(characters[index].getSkinningMatrices());
}

operator characterDrawing(
  io Scalar time, 

  String filePath, 
  Integer numCharacters, 
  Scalar spacing, 

  io FbxCharacter fbxCharacter, 
  io Character characters[], 
  io DrawingHandle handles[], 
  io InlineMatrixArrayTexture poseTextures[]
) {

  if(numCharacters != characters.size()){

    characters.resize(0);
    poseTextures.resize(0);
    handles.resize(0);

    // Check for an absolute file path, then a relative path.
    FilePath filePathObj(filePath);
    if(!filePathObj.exists()){
      report('File not found:' + filePath);
      return;
    }

    report('Importing:' + filePathObj.string());
    fbxCharacter.load(filePathObj.string());

    // Now setup the rendering
    InlineDrawing drawing = OGLInlineDrawing_GetInstance();
    if(!drawing.hasShader('skinnedSurfaceShader')){
      InlineShader skinnedSurfaceShader = OGLSkinnedSurfaceShader('skinnedSurfaceShader');
      drawing.registerShader(skinnedSurfaceShader);
    }

    InlineShader skinnedSurfaceShader = drawing.getShader('skinnedSurfaceShader');


    // Create an InlineMeshShape for each of the geometries in the FbxCharacter.
    // All the instances will share these InlineMeshShapes
    InlineMeshShape skinShapes[];
    skinShapes.resize(fbxCharacter.getNumGeometries());
    for(Integer geom=0; geom<fbxCharacter.getNumGeometries(); geom++)
      skinShapes[geom] = InlineMeshShape(fbxCharacter.getName() + '_' + geom, fbxCharacter.getGeometry(geom));

    Size offset = 0;
    characters.resize(numCharacters);
    poseTextures.resize(numCharacters);
    handles.resize(numCharacters);

    Integer gridDim = sqrt(Scalar(numCharacters))+1;
    Integer inst=0;
    const Size seed = 16841;
    for(Integer i=0; i<gridDim; i++){
      for(Integer j=0; j<gridDim; j++){
        if(inst >= numCharacters)
          break;

        if(characters[inst] == null){
          // Construct a handle for this character instance. The handle will clean up the InlineDrawing when it is destroyed. 
          handles[inst] = DrawingHandle(fbxCharacter.getName() + '_' + inst);
          characters[inst] = Character(fbxCharacter, fbxCharacter.getName() + '_' + inst);
          poseTextures[inst] = InlineMatrixArrayTexture(fbxCharacter.getName() + '_Transforms'+inst, fbxCharacter.getSkinningMatrices());

          InlineMaterial characterMaterial = skinnedSurfaceShader.getOrCreateMaterial('character'+inst);
          characterMaterial.setUniform(InlineUniform('u_diffuseColor', randomColor(seed, ++offset)));

          for(Integer geom=0; geom<fbxCharacter.getNumGeometries(); geom++){
            InlineInstance skinInstance = SimpleInlineInstance(fbxCharacter.getName() + '_' + geom, handles[inst].rootTransform, skinShapes[geom], characterMaterial);
            skinInstance.setInstanceUniform(InlineUniform('skinningMatrices', poseTextures[inst]));
            skinInstance.setInstanceUniform(InlineUniform('bindShapeTransform', fbxCharacter.getBindShapeTransform(geom)));
          }
        }
        Xfo xfo(Quat(Euler(HALF_PI, 0.0, 0.0)));
        xfo.tr.x = (i * spacing) - (spacing * gridDim * 0.5);
        xfo.tr.y = (j * spacing) - (spacing * gridDim * 0.5);
        xfo.tr.z = 0.05;
        characters[inst].setGlobalXfo(xfo);
        inst++;
      }
    }
  }

  /////////////////
  // Now update the pose of the character, and push the skinningMatrices to the poseTexture.
  if(fbxCharacter.initialized()){
    evalCharacter<<<numCharacters>>>(time, characters, poseTextures);
  }
}"

m.SetKLCode entryPt klCode

m.filePath = witcherFbx
m.spacing = 200
m.numCharacters =600

Sphere radius:2.0 smooth:on segs:32 chop:0 slice:off sliceFrom:0 sliceTo:0 mapcoords:on recenter:off pos:[6538.58,6780.46,0] isSelected:on
	Sphere radius:96.3105 smooth:on segs:32 chop:0 slice:off sliceFrom:0 sliceTo:0 mapcoords:on recenter:off pos:[-5148.51,-5426.51,0] isSelected:on

select $SpliceMesh001
Splice.Rendering = true