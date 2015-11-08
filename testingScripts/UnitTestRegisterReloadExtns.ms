resetMaxFile #noPrompt
thisfile = getThisScriptFilename()
persistableFile =  (getFilenamePath thisfile) + "persistableObj.kl"

-- Create any splice class, this is to ensure we have some sort of client context
m = FabricMesh()

-- Force an extension reload
Fabric.loadExtension "sdvs" "1.0.0" true

-- Load a new extension
fileList = #(persistableFile)
Fabric.registerExtension "TestingExt" "1.0.0" "" fileList true true

m.AddInputPort "obj" "MyObject" -1
