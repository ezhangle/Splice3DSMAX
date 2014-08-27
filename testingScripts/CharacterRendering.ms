m = SpliceMesh()
m.CreatePort "FbxFile" "String" -2
m.CreatePort "fbxCharacter" "FbxCharacter" -1 Extension:"FbxHelpers"
m.CreatePort "poseTexture" "InlineMatrixArrayTexture" -1 Extension:"InlineDrawing"

entryPt = "characterDrawing"
klCode = "

require InlineDrawing;
require FbxHelpers;

operator characterDrawing(
  String FbxFile, 
  io Scalar time,
  io FbxCharacter fbxCharacter,
  io InlineMatrixArrayTexture poseTexture
) {

  InlineDrawing drawing = OGLInlineDrawing_GetInstance();

  if(fbxCharacter.getFilePath() != FbxFile && FbxFile != \"\"){
    fbxCharacter.load(FbxFile);
    Xfo xfo(Quat(Euler(HALF_PI, 0.0, 0.0)));
    fbxCharacter.setGlobalXfo(xfo);
	
    InlineShader skinnedSurfaceShader = OGLSkinnedSurfaceShader('skinnedSurfaceShader');
    drawing.registerShader(skinnedSurfaceShader);

    InlineMaterial characterMaterial = skinnedSurfaceShader.getOrCreateMaterial(\"character\");
    characterMaterial.setUniform(InlineUniform('u_diffuseColor', Color(1.0, 0.0, 0.0)));

    InlineTransform characterTransform = StaticInlineTransform('characterTransform', Xfo());
    drawing.getRoot().addChild(characterTransform);

    poseTexture = InlineMatrixArrayTexture(fbxCharacter.getName() + \"_Transforms\", fbxCharacter.getSkinningMatrices());

    for(Integer i=0; i<fbxCharacter.getNumGeometries(); i++){
      InlineMeshShape skinShape = InlineMeshShape(fbxCharacter.getName() + \"_\" + i, fbxCharacter.getGeometry(i));

      InlineInstance skinInstance = SimpleInlineInstance(fbxCharacter.getName(), characterTransform, skinShape, characterMaterial);
      skinInstance.setInstanceUniform(InlineUniform('skinningMatrices', InlineTexture(poseTexture)));
      skinInstance.setInstanceUniform(InlineUniform('bindShapeTransform', fbxCharacter.getBindShapeTransform(i)));
    }
  }

  ///
  // Update the pose
  if(fbxCharacter.initialized()){
    report('Eval at Time: ' + time);
    ClipPose pose = fbxCharacter.getPose();
    pose.evaluate(time);
    poseTexture.update(fbxCharacter.getSkinningMatrices());
  }
}"

m.SetKLCode entryPt klCode
m.FbxFile = "D:\Users\Stephen\Downloads\SpliceTestingScripts (1)\Witcher_Creep.fbx"

Splice.Rendering = true