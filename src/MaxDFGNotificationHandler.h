///////////////////////////////////////////////////////
//
// This class header has been almost completely copy-pasted
// DFGNotificationRouter.  We want to respond to notifications
// but there appears to be no built-in general-purpose class
// that does the job of converting text notify messages to
// compiler-friendly function calls etc.
//
#pragma once;

class SpliceTranslationFPInterface;

class MaxDFGNotificationHandler
{
public:

	MaxDFGNotificationHandler(SpliceTranslationFPInterface* pInterface);
	virtual ~MaxDFGNotificationHandler();

	// Must call when owning class changes/updates it's bindings
	void updateBinding(FabricCore::DFGBinding& binding);

	// DFG Change notifications
	void callback(FTL::CStrRef json);

#pragma region notification methods

	void onNodeInserted(
		FTL::CStrRef nodeName,
		FTL::JSONObject const *jsonObject
		);
	void onNodeRemoved(
		FTL::CStrRef nodeName
		);
	void onNodePortInserted(
		FTL::CStrRef nodeName,
		FTL::CStrRef portName,
		FTL::JSONObject const *jsonObject
		);
	void onNodePortRemoved(
		FTL::CStrRef nodeName,
		FTL::CStrRef portName
		);
	void onExecPortInserted(
		FTL::CStrRef portName,
		FTL::JSONObject const *jsonObject
		);
	void onExecPortRemoved(
		FTL::CStrRef portName
		);
	void onPortsConnected(
		FTL::CStrRef srcPath,
		FTL::CStrRef dstPath
		);
	void onPortsDisconnected(
		FTL::CStrRef srcPath,
		FTL::CStrRef dstPath
		);
	void onNodeMetadataChanged(
		FTL::CStrRef nodePath,
		FTL::CStrRef key,
		FTL::CStrRef value
		);
	void onNodeTitleChanged(
		FTL::CStrRef nodePath,
		FTL::CStrRef title
		);
	void onExecPortRenamed(
		FTL::CStrRef oldPortName,
		FTL::CStrRef newPortName,
		FTL::JSONObject const *execPortJSONObject
		);
	void onNodePortRenamed(
		FTL::CStrRef nodeName,
		FTL::CStrRef oldPortName,
		FTL::CStrRef newPortName
		);
	void onExecMetadataChanged(
		FTL::CStrRef key,
		FTL::CStrRef value
		);
	void onExtDepAdded(
		FTL::CStrRef extension,
		FTL::CStrRef version
		);
	void onExtDepRemoved(
		FTL::CStrRef extension,
		FTL::CStrRef version
		);
	void onNodeCacheRuleChanged(
		FTL::CStrRef nodeName,
		FTL::CStrRef newCacheRule
		);
	void onExecCacheRuleChanged(
		FTL::CStrRef newCacheRule
		);
	void onExecPortResolvedTypeChanged(
		FTL::CStrRef portName,
		FTL::CStrRef newResolvedType
		);
	void onExecPortTypeSpecChanged(
		FTL::CStrRef portName,
		FTL::CStrRef typeSpec
		);
	void onNodePortResolvedTypeChanged(
		FTL::CStrRef nodeName,
		FTL::CStrRef portName,
		FTL::CStrRef newResolvedType
		);
	void onNodePortMetadataChanged(
		FTL::CStrRef nodeName,
		FTL::CStrRef portName,
		FTL::CStrRef key,
		FTL::CStrRef value
		);
	void onExecPortMetadataChanged(
		FTL::CStrRef portName,
		FTL::CStrRef key,
		FTL::CStrRef value
		);
	void onExecPortTypeChanged(
		FTL::CStrRef portName,
		FTL::CStrRef execPortType
		);
	void onNodePortTypeChanged(
		FTL::CStrRef nodeName,
		FTL::CStrRef portName,
		FTL::CStrRef nodePortType
		);
	void onRefVarPathChanged(
		FTL::CStrRef refName,
		FTL::CStrRef newVarPath
		);
	void onFuncCodeChanged(
		FTL::CStrRef code
		);
	void onExecExtDepsChanged(
		FTL::CStrRef extDeps
		);
	void onExecPortDefaultValuesChanged(
		FTL::CStrRef portName
		);
	void onNodePortDefaultValuesChanged(
		FTL::CStrRef nodeName,
		FTL::CStrRef portName
		);
	void onRemovedFromOwner();
	void onExecPortsReordered(
		unsigned int indexCount,
		unsigned int * indices
		);
	void onNodePortsReordered(
		FTL::CStrRef nodeName,
		unsigned int indexCount,
		unsigned int * indices
		);
	void onExecDidAttachPreset(
		FTL::CStrRef presetFilePath
		);
	void onInstExecDidAttachPreset(
		FTL::CStrRef nodeName,
		FTL::CStrRef presetFilePath
		);
	void onExecWillDetachPreset(
		FTL::CStrRef presetFilePath
		);
	void onInstExecWillDetachPreset(
		FTL::CStrRef nodeName,
		FTL::CStrRef presetFilePath
		);
	void onExecPresetFileRefCountDidChange(
		int newPresetFileRefCount
		);
	void onInstExecPresetFileRefCountDidChange(
		FTL::CStrRef nodeName,
		int newPresetFileRefCount
		);

#pragma endregion

	// Members
	SpliceTranslationFPInterface* m_pInterface;

	// Our parent only returns us the Exec pointer in const form, but
	// for a lot of our functions we need to call functions that
	// have not yet been labelled as const.  Const-cast until FE cleans up their const-ness
	FabricCore::DFGExec GetExec();
	FabricCore::DFGBinding& GetBinding();
};