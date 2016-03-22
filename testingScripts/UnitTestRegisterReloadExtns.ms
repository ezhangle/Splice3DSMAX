resetMaxFile #noPrompt
thisfile = getThisScriptFilename()
persistableFile =  (getFilenamePath thisfile) + "persistableObj.kl"

-- Create any splice class, this is to ensure we have some sort of client context
m = FabricMesh()

-- Force an extension reload
-- For some reason, this expects 1 argument, but the declarations all require 3.
Fabric.loadExtension "sdvs" "1.0.0" true

-- Load a new extension
fileList = #(persistableFile)
-- For some reason this expects 5 arguments instead of 6. The declarations require 6.
Fabric.registerExtension "TestingExt" "1.0.0" "" fileList true true

m.AddInputPort "obj" "MyObject" -1
