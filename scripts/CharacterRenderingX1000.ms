resetMaxFile #noPrompt
thisfile = getThisScriptFilename()
witcherFbx =  (getFilenamePath thisfile) + "testfiles/Witcher_Creep.fbx"


m = SpliceMesh()
m.CreatePort "fbxFile" "String" -2
m.CreatePort "numCharacters" "Integer" -2
m.CreatePort "spacing" "Scalar" -2
m.CreatePort "fbxCharacter" "FbxCharacter" -1 Extension:"FbxHelpers"
m.CreatePort "characters" "Character" -1 isArray:true Extension:"Characters"
m.CreatePort "poseTextures" "InlineMatrixArrayTexture" -1 isArray:true Extension:"InlineDrawing"

entryPt = "characterDrawing"
klCode = "
require InlineDrawing;
require FbxHelpers;

function Color randomColor(io Size offset)
{
  const Size seed = 16841;
  return Color(
    mathRandomFloat32(seed, offset++),
    mathRandomFloat32(seed, offset++),
    mathRandomFloat32(seed, offset++)
    );
}

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

  String fbxFile, 
  Integer numCharacters, 
  Scalar spacing, 

  io FbxCharacter fbxCharacter, 
  io Character characters[], 
  io InlineMatrixArrayTexture poseTextures[]
) {

  InlineDrawing drawing = OGLInlineDrawing_GetInstance();

  if(fbxCharacter.getFilePath() != fbxFile && fbxFile != \"\"){
    fbxCharacter.load(fbxFile);
    characters.resize(0);
    poseTextures.resize(0);

    if(!drawing.hasShader('skinnedSurfaceShader')){
      InlineShader skinnedSurfaceShader = OGLSkinnedSurfaceShader('skinnedSurfaceShader');
      drawing.registerShader(skinnedSurfaceShader);
    }
  }
  if(fbxCharacter.initialized() && numCharacters != characters.size()){
    InlineShader skinnedSurfaceShader = drawing.getShader('skinnedSurfaceShader');

    Size offset = 0;
    characters.resize(numCharacters);
    poseTextures.resize(numCharacters);

    Integer gridDim = sqrt(Scalar(numCharacters))+1;
    Integer inst=0;
    for(Integer i=0; i<gridDim; i++){
      for(Integer j=0; j<gridDim; j++){
        if(inst >= numCharacters)
          break;

        if(characters[inst] == null){
          characters[inst] = Character(fbxCharacter, \"_\" + inst);
          InlineTransform characterTransform = StaticInlineTransform('characterTransform', Xfo());
          drawing.getRoot().addChild(characterTransform);
          poseTextures[inst] = InlineMatrixArrayTexture(fbxCharacter.getName() + \"_Transforms\"+inst, fbxCharacter.getSkinningMatrices());

          InlineMaterial characterMaterial = skinnedSurfaceShader.getOrCreateMaterial(\"character\"+inst);
          characterMaterial.setUniform(InlineUniform('u_diffuseColor', randomColor(offset)));
          for(Integer geom=0; geom<fbxCharacter.getNumGeometries(); geom++){
            InlineMeshShape skinShape = InlineMeshShape(fbxCharacter.getName() + \"_\" + geom, fbxCharacter.getGeometry(geom));

            InlineInstance skinInstance = SimpleInlineInstance(fbxCharacter.getName() + \"_\" + geom, characterTransform, skinShape, characterMaterial);
            skinInstance.setInstanceUniform(InlineUniform('skinningMatrices', InlineTexture(poseTextures[inst])));
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

m.fbxFile = witcherFbx
m.spacing = 200
m.numCharacters =600

Sphere radius:2.0 smooth:on segs:32 chop:0 slice:off sliceFrom:0 sliceTo:0 mapcoords:on recenter:off pos:[6538.58,6780.46,0] isSelected:on
	Sphere radius:96.3105 smooth:on segs:32 chop:0 slice:off sliceFrom:0 sliceTo:0 mapcoords:on recenter:off pos:[-5148.51,-5426.51,0] isSelected:on

select $SpliceMesh001
Splice.Rendering = true