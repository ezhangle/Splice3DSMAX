#include "StdAfx.h"
#include "MaxDFGNotificationRouter.h"
#include "SpliceTranslationFPInterface.h"


MaxDFGNotificationRouter::MaxDFGNotificationRouter(SpliceTranslationFPInterface* pInterface, FabricCore::DFGBinding& binding, FabricCore::DFGExec exec)
	: FabricUI::DFG::DFGNotificationRouter(binding, exec)
	, m_pInterface(pInterface)
{

}

MaxDFGNotificationRouter::~MaxDFGNotificationRouter()
{

}
////////////////////////////////////////////////////////////////////////////
#pragma region DFG-Derived functions
//


void MaxDFGNotificationRouter::onNotification(FTL::CStrRef json)
{
	FTL::JSONStrWithLoc jsonStrWithLoc(json);
	FTL::OwnedPtr<FTL::JSONObject> jsonObject(
		FTL::JSONValue::Decode(jsonStrWithLoc)->cast<FTL::JSONObject>()
		);
	FTL::StrRef descStr = jsonObject->getString(FTL_STR("desc"));

	// TODO: Remove this once the naming bug is fixed in DFGNotificationRouter
	if (descStr == FTL_STR("execPortMetadataChanged"))
	{
		onExecPortMetadataChanged(
			jsonObject->getString(FTL_STR("portName")),
			jsonObject->getString(FTL_STR("key")),
			jsonObject->getString(FTL_STR("value"))
			);
	}
	else
		__super::onNotification(json);
}

void MaxDFGNotificationRouter::onFuncCodeChanged(FTL::CStrRef code)
{
	__super::onFuncCodeChanged(code);
}


void MaxDFGNotificationRouter::onRefVarPathChanged(FTL::CStrRef refName, FTL::CStrRef newVarPath)
{
	__super::onRefVarPathChanged(refName, newVarPath);
}


void MaxDFGNotificationRouter::onNodePortTypeChanged(FTL::CStrRef nodeName, FTL::CStrRef portName, FTL::CStrRef nodePortType)
{
	__super::onNodePortTypeChanged(nodeName, portName, nodePortType);
}


void MaxDFGNotificationRouter::onExecPortTypeChanged(FTL::CStrRef portName, FTL::CStrRef execPortType)
{
	__super::onExecPortTypeChanged(portName, execPortType);
}


void MaxDFGNotificationRouter::onNodePortMetadataChanged(FTL::CStrRef nodeName, FTL::CStrRef portName, FTL::CStrRef key, FTL::CStrRef value)
{
	__super::onNodePortMetadataChanged(nodeName, portName, key, value);
}


void MaxDFGNotificationRouter::onExecPortMetadataChanged(FTL::CStrRef portName, FTL::CStrRef key, FTL::CStrRef value)
{
	if (key == MAX_PARM_TYPE_OPT)
	{
		int type = atoi(value.data());
		m_pInterface->SetMaxConnectedType(portName.c_str(), type);
	}
	else if (key == "uiHidden" || key == "uiOpaque")
	{
		if (value == "true")
		{
			// If our port is hidden or opaque, it means it has no Max connection
			m_pInterface->SetMaxConnectedType(portName.c_str(), -1);
		}
		else
		{
			// Recreate with default max type
			m_pInterface->SetMaxConnectedType(portName.c_str(), -2);
		}
		m_pInterface->UpdateUISpec();
	}
	else if (key == "uiRange")
	{
		::SetMaxParamLimits(m_pInterface->GetPBlock()->GetDesc(), m_pInterface->GetBinding(), portName.c_str());
		m_pInterface->UpdateUISpec();
	}
	else if (key == "scalarUnit")
	{
		// TODO: Sync our ScalarUnit label with Maya's
		m_pInterface->UpdateUISpec();
	}
	__super::onExecPortMetadataChanged(portName, key, value);
}


void MaxDFGNotificationRouter::onNodePortResolvedTypeChanged(FTL::CStrRef nodeName, FTL::CStrRef portName, FTL::CStrRef newResolvedType)
{
	__super::onNodePortResolvedTypeChanged(nodeName, portName, newResolvedType);
}


void MaxDFGNotificationRouter::onExecPortTypeSpecChanged(FTL::CStrRef portName, FTL::CStrRef typeSpec)
{
	__super::onExecPortTypeSpecChanged(portName, typeSpec);
}


void MaxDFGNotificationRouter::onExecPortResolvedTypeChanged(FTL::CStrRef portName, FTL::CStrRef newResolvedType)
{
	//	HoldActions hold(_M("Splice Port Type Changed"));
	if (theHold.RestoreOrRedoing())
		return;

	if (GetExec().getExecPortType(portName.c_str()) == FabricCore::DFGPortType_In)
	{
		int type = m_pInterface->GetMaxConnectedType(portName.c_str());
		// Attempt to set the same type back.  If the max type is no
		// longer legitimate, the type will be reset to default.
		m_pInterface->SetMaxConnectedType(portName.c_str(), type);
	}

	__super::onExecPortResolvedTypeChanged(portName, newResolvedType);
}


void MaxDFGNotificationRouter::onExecCacheRuleChanged(FTL::CStrRef newCacheRule)
{
	__super::onExecCacheRuleChanged(newCacheRule);
}


void MaxDFGNotificationRouter::onNodeCacheRuleChanged(FTL::CStrRef nodeName, FTL::CStrRef newCacheRule)
{
	__super::onNodeCacheRuleChanged(nodeName, newCacheRule);
}


void MaxDFGNotificationRouter::onExtDepRemoved(FTL::CStrRef extension, FTL::CStrRef version)
{
	__super::onExtDepRemoved(extension, version);
}


void MaxDFGNotificationRouter::onExtDepAdded(FTL::CStrRef extension, FTL::CStrRef version)
{
	__super::onExtDepAdded(extension, version);
}


void MaxDFGNotificationRouter::onExecMetadataChanged(FTL::CStrRef key, FTL::CStrRef value)
{
	__super::onExecMetadataChanged(key, value);
}


void MaxDFGNotificationRouter::onNodePortRenamed(FTL::CStrRef nodeName, FTL::CStrRef oldPortName, FTL::CStrRef newPortName)
{
	__super::onNodePortRenamed(nodeName, oldPortName, newPortName);
}


void MaxDFGNotificationRouter::onExecPortRenamed(FTL::CStrRef oldPortName, FTL::CStrRef newPortName, FTL::JSONObject const *execPortJSONObject)
{
	if (theHold.RestoreOrRedoing())
		return;

	int pid = GetPortParamID(GetBinding(), newPortName.c_str());
	if (pid >= 0)
	{
		// Normally when we edit the pblock, changes
		// are notified in the form of a call to SetReference
		// However, when changing name, no references change
		// so we need to manually update the UI
		MSTR str = MSTR::FromACP(newPortName.c_str());
		m_pInterface->SetPblockParamName(pid, str);
	}

	__super::onExecPortRenamed(oldPortName, newPortName, execPortJSONObject);
}


void MaxDFGNotificationRouter::onNodeTitleChanged(FTL::CStrRef nodePath, FTL::CStrRef title)
{
	__super::onNodeTitleChanged(nodePath, title);
}


void MaxDFGNotificationRouter::onNodeMetadataChanged(FTL::CStrRef nodePath, FTL::CStrRef key, FTL::CStrRef value)
{
	__super::onNodeMetadataChanged(nodePath, key, value);
}


void MaxDFGNotificationRouter::onPortsDisconnected(FTL::CStrRef srcPath, FTL::CStrRef dstPath)
{
	m_pInterface->InvalidateAll();
	__super::onPortsDisconnected(srcPath, dstPath);
}


void MaxDFGNotificationRouter::onPortsConnected(FTL::CStrRef srcPath, FTL::CStrRef dstPath)
{
	m_pInterface->InvalidateAll();
	__super::onPortsConnected(srcPath, dstPath);
}


void MaxDFGNotificationRouter::onExecPortRemoved(FTL::CStrRef portName)
{
	if (theHold.RestoreOrRedoing())
		return;
	
	if (GetExec().getExecPortType(portName.c_str()) != FabricCore::DFGPortType_In)
	{
		m_pInterface->SetMaxConnectedType(portName.c_str(), -1);
	}
	
	// Ensure the max data is released
	//int pid = GetPortParamID(GetBinding(), portName.c_str());
	//if (pid >= 0)
	//	m_pInterface->DeleteMaxParameter((ParamID)pid);
	//
	//InvalidateAll();
	__super::onExecPortRemoved(portName);
}


void MaxDFGNotificationRouter::onExecPortInserted(FTL::CStrRef portName, FTL::JSONObject const *jsonObject)
{
	if (theHold.RestoreOrRedoing())
		return;
}


void MaxDFGNotificationRouter::onNodePortRemoved(FTL::CStrRef nodeName, FTL::CStrRef portName)
{
	__super::onNodePortRemoved(nodeName, portName);
}


void MaxDFGNotificationRouter::onNodePortInserted(FTL::CStrRef nodeName, FTL::CStrRef portName, FTL::JSONObject const *jsonObject)
{
	__super::onNodePortInserted(nodeName, portName, jsonObject);
}


void MaxDFGNotificationRouter::onNodeRemoved(FTL::CStrRef nodeName)
{
	__super::onNodeRemoved(nodeName);
}


void MaxDFGNotificationRouter::onNodeInserted(FTL::CStrRef nodeName, FTL::JSONObject const *jsonObject)
{
	__super::onNodeInserted(nodeName, jsonObject);
}

//void MaxDFGNotificationRouter::onExecPortRenamed(FabricServices::DFGWrapper::ExecPortPtr port, const char * oldName)
//{
//	if (theHold.RestoreOrRedoing())
//		return;
//
////	HoldActions hold(_M("Splice Rename Port"));
////	if (theHold.Holding())
////		theHold.Put(new SplicePortChangeObject(this));
//
//	int pid = GetPortParamID(port);
//	if (pid >= 0)
//	{
//		// Normally when we edit the pblock, changes
//		// are notified in the form of a call to SetReference
//		// However, when changing name, no references change
//		// so we need to manually update the UI
//		MSTR str = MSTR::FromACP(port->getName());
//		SetMaxParamName(m_pblock->GetDesc(), (ParamID)pid, str);
//		// Delete existing autogen UI
//		UpdateUISpec();
//	}
//}
//
//
//void MaxDFGNotificationRouter::onExecPortResolvedTypeChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * resolvedType)
//{
////	HoldActions hold(_M("Splice Port Type Changed"));
//	if (theHold.RestoreOrRedoing())
//		return;
//
//	if (port->getOutsidePortType() != FabricCore::DFGPortType_Out)
//		SetMaxConnectedType(port, GetPort3dsMaxType(port));
//}
//
//
//void MaxDFGNotificationRouter::onExecPortMetadataChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * key, const char * metadata)
//{

//}
#pragma endregion
