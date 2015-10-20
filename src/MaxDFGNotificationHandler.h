#pragma once;

#include <FabricUI/DFG/DFGNotificationRouter.h>

class SpliceTranslationFPInterface;

class MaxDFGNotificationRouter : public FabricUI::DFG::DFGNotificationRouter
{
public:

	MaxDFGNotificationRouter(SpliceTranslationFPInterface* pInterface, FabricCore::DFGBinding& binding, FabricCore::DFGExec exec);
	~MaxDFGNotificationRouter();

#pragma region DFG-derived functions

	// DFG Change notifications
	void onNotification(FTL::CStrRef json);

	void onNodeInserted(
		FTL::CStrRef nodeName,
		FTL::JSONObject const *jsonObject
		) override;
	void onNodeRemoved(
		FTL::CStrRef nodeName
		) override;
	void onNodePortInserted(
		FTL::CStrRef nodeName,
		FTL::CStrRef portName,
		FTL::JSONObject const *jsonObject
		) override;
	void onNodePortRemoved(
		FTL::CStrRef nodeName,
		FTL::CStrRef portName
		) override;
	void onExecPortInserted(
		FTL::CStrRef portName,
		FTL::JSONObject const *jsonObject
		) override;
	void onExecPortRemoved(
		FTL::CStrRef portName
		) override;
	void onPortsConnected(
		FTL::CStrRef srcPath,
		FTL::CStrRef dstPath
		) override;
	void onPortsDisconnected(
		FTL::CStrRef srcPath,
		FTL::CStrRef dstPath
		) override;
	void onNodeMetadataChanged(
		FTL::CStrRef nodePath,
		FTL::CStrRef key,
		FTL::CStrRef value
		) override;
	void onNodeTitleChanged(
		FTL::CStrRef nodePath,
		FTL::CStrRef title
		) override;
	void onExecPortRenamed(
		FTL::CStrRef oldPortName,
		FTL::CStrRef newPortName,
		FTL::JSONObject const *execPortJSONObject
		) override;
	void onNodePortRenamed(
		FTL::CStrRef nodeName,
		FTL::CStrRef oldPortName,
		FTL::CStrRef newPortName
		) override;
	void onExecMetadataChanged(
		FTL::CStrRef key,
		FTL::CStrRef value
		) override;
	void onExtDepAdded(
		FTL::CStrRef extension,
		FTL::CStrRef version
		) override;
	void onExtDepRemoved(
		FTL::CStrRef extension,
		FTL::CStrRef version
		) override;
	void onNodeCacheRuleChanged(
		FTL::CStrRef nodeName,
		FTL::CStrRef newCacheRule
		) override;
	void onExecCacheRuleChanged(
		FTL::CStrRef newCacheRule
		) override;
	void onExecPortResolvedTypeChanged(
		FTL::CStrRef portName,
		FTL::CStrRef newResolvedType
		) override;
	void onExecPortTypeSpecChanged(
		FTL::CStrRef portName,
		FTL::CStrRef typeSpec
		) override;
	void onNodePortResolvedTypeChanged(
		FTL::CStrRef nodeName,
		FTL::CStrRef portName,
		FTL::CStrRef newResolvedType
		) override;
	void onExecPortMetadataChanged(
		FTL::CStrRef portName,
		FTL::CStrRef key,
		FTL::CStrRef value
		) override;
	void onNodePortMetadataChanged(
		FTL::CStrRef nodeName,
		FTL::CStrRef portName,
		FTL::CStrRef key,
		FTL::CStrRef value
		) override;
	void onExecPortTypeChanged(
		FTL::CStrRef portName,
		FTL::CStrRef execPortType
		) override;
	void onNodePortTypeChanged(
		FTL::CStrRef nodeName,
		FTL::CStrRef portName,
		FTL::CStrRef nodePortType
		) override;
	void onRefVarPathChanged(
		FTL::CStrRef refName,
		FTL::CStrRef newVarPath
		) override;
	void onFuncCodeChanged(
		FTL::CStrRef code
		) override;

#pragma endregion

	// Members
	SpliceTranslationFPInterface* m_pInterface;

	// Our parent only returns us the Exec pointer in const form, but
	// for a lot of our functions we need to call functions that
	// have not yet been labelled as const.  Const-cast until FE cleans up their const-ness
	FabricCore::DFGExec& GetExec() { return const_cast<FabricCore::DFGExec&>(getCoreDFGExec()); }
	FabricCore::DFGBinding& GetBinding() { return const_cast<FabricCore::DFGBinding&>(getCoreDFGBinding()); }
};