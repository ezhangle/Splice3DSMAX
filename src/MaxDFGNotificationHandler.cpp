#include "StdAfx.h"
#include "MaxDFGNotificationHandler.h"
#include "SpliceTranslationFPInterface.h"

void bindingNotificationCallback(void * userData, char const *jsonStr, uint32_t jsonLength);

MaxDFGNotificationHandler::MaxDFGNotificationHandler(SpliceTranslationFPInterface* pInterface)
	: m_pInterface(pInterface)
{
	updateBinding(GetBinding());
}

MaxDFGNotificationHandler::~MaxDFGNotificationHandler()
{

}

#pragma region callback translation

void MaxDFGNotificationHandler::updateBinding(FabricCore::DFGBinding& binding)
{
	if (binding.isValid())
		binding.setNotificationCallback(bindingNotificationCallback, this);
}

void bindingNotificationCallback(void * userData, char const *jsonStr, uint32_t jsonLength)
{
	MaxDFGNotificationHandler* handler = reinterpret_cast<MaxDFGNotificationHandler*>(userData);
	handler->callback(jsonStr);
}

void MaxDFGNotificationHandler::callback(FTL::CStrRef jsonStr)
{
	try
	{
		FTL::JSONStrWithLoc jsonStrWithLoc(jsonStr);
		FTL::OwnedPtr<FTL::JSONObject> jsonObject(
			FTL::JSONValue::Decode(jsonStrWithLoc)->cast<FTL::JSONObject>()
			);
		FTL::StrRef descStr = jsonObject->getString(FTL_STR("desc"));

		if (descStr == FTL_STR("nodeInserted"))
		{
			onNodeInserted(
				jsonObject->getString(FTL_STR("nodeName")),
				jsonObject->get(FTL_STR("nodeDesc"))->cast<FTL::JSONObject>()
				);
		}
		else if (descStr == FTL_STR("nodeRemoved"))
		{
			onNodeRemoved(
				jsonObject->getString(FTL_STR("nodeName"))
				);
		}
		else if (descStr == FTL_STR("nodePortInserted"))
		{
			onNodePortInserted(
				jsonObject->getString(FTL_STR("nodeName")),
				jsonObject->getString(FTL_STR("portName")),
				jsonObject->get(FTL_STR("nodePortDesc"))->cast<FTL::JSONObject>()
				);
		}
		else if (descStr == FTL_STR("nodePortRemoved"))
		{
			onNodePortRemoved(
				jsonObject->getString(FTL_STR("nodeName")),
				jsonObject->getString(FTL_STR("portName"))
				);
		}
		else if (descStr == FTL_STR("execPortInserted"))
		{
			onExecPortInserted(
				jsonObject->getString(FTL_STR("portName")),
				jsonObject->get(FTL_STR("execPortDesc"))->cast<FTL::JSONObject>()
				);
		}
		else if (descStr == FTL_STR("execPortRemoved"))
		{
			onExecPortRemoved(
				jsonObject->getString(FTL_STR("portName"))
				);
		}
		else if (descStr == FTL_STR("portsConnected"))
		{
			onPortsConnected(
				jsonObject->getString(FTL_STR("srcPath")),
				jsonObject->getString(FTL_STR("dstPath"))
				);
		}
		else if (descStr == FTL_STR("portsDisconnected"))
		{
			onPortsDisconnected(
				jsonObject->getString(FTL_STR("srcPath")),
				jsonObject->getString(FTL_STR("dstPath"))
				);
		}
		else if (descStr == FTL_STR("nodeMetadataChanged"))
		{
			onNodeMetadataChanged(
				jsonObject->getString(FTL_STR("nodeName")),
				jsonObject->getString(FTL_STR("key")),
				jsonObject->getString(FTL_STR("value"))
				);
		}
		else if (descStr == FTL_STR("nodeTitleChanged"))
		{
			onNodeTitleChanged(
				jsonObject->getString(FTL_STR("nodeName")),
				jsonObject->getString(FTL_STR("title"))
				);
		}
		else if (descStr == FTL_STR("execPortRenamed"))
		{
			onExecPortRenamed(
				jsonObject->getString(FTL_STR("oldPortName")),
				jsonObject->getString(FTL_STR("newPortName")),
				jsonObject->get(FTL_STR("execPortDesc"))->cast<FTL::JSONObject>()
				);
		}
		else if (descStr == FTL_STR("nodePortRenamed"))
		{
			onNodePortRenamed(
				jsonObject->getString(FTL_STR("nodeName")),
				jsonObject->getString(FTL_STR("oldPortName")),
				jsonObject->getString(FTL_STR("newPortName"))
				);
		}
		else if (descStr == FTL_STR("execMetadataChanged"))
		{
			onExecMetadataChanged(
				jsonObject->getString(FTL_STR("key")),
				jsonObject->getString(FTL_STR("value"))
				);
		}
		else if (descStr == FTL_STR("extDepAdded"))
		{
			onExtDepAdded(
				jsonObject->getString(FTL_STR("name")),
				jsonObject->getString(FTL_STR("versionRange"))
				);
		}
		else if (descStr == FTL_STR("extDepRemoved"))
		{
			onExtDepRemoved(
				jsonObject->getString(FTL_STR("name")),
				jsonObject->getString(FTL_STR("versionRange"))
				);
		}
		else if (descStr == FTL_STR("nodeCacheRuleChanged"))
		{
			onNodeCacheRuleChanged(
				jsonObject->getString(FTL_STR("nodeName")),
				jsonObject->getString(FTL_STR("cacheRule"))
				);
		}
		else if (descStr == FTL_STR("execCacheRuleChanged"))
		{
			onExecCacheRuleChanged(
				jsonObject->getString(FTL_STR("cacheRule"))
				);
		}
		else if (descStr == FTL_STR("execPortResolvedTypeChanged"))
		{
			onExecPortResolvedTypeChanged(
				jsonObject->getString(FTL_STR("portName")),
				jsonObject->getStringOrEmpty(FTL_STR("newResolvedType"))
				);
		}
		else if (descStr == FTL_STR("execPortTypeSpecChanged"))
		{
			onExecPortTypeSpecChanged(
				jsonObject->getString(FTL_STR("portName")),
				jsonObject->getStringOrEmpty(FTL_STR("newTypeSpec"))
				);
		}
		else if (descStr == FTL_STR("nodePortResolvedTypeChanged"))
		{
			onNodePortResolvedTypeChanged(
				jsonObject->getString(FTL_STR("nodeName")),
				jsonObject->getString(FTL_STR("portName")),
				jsonObject->getStringOrEmpty(FTL_STR("newResolvedType"))
				);
		}
		else if (descStr == FTL_STR("nodePortMetadataChanged"))
		{
			onNodePortMetadataChanged(
				jsonObject->getString(FTL_STR("nodeName")),
				jsonObject->getString(FTL_STR("portName")),
				jsonObject->getString(FTL_STR("key")),
				jsonObject->getString(FTL_STR("value"))
				);
		}
		else if (descStr == FTL_STR("execPortMetadataChanged"))
		{
			onExecPortMetadataChanged(
				jsonObject->getString(FTL_STR("portName")),
				jsonObject->getString(FTL_STR("key")),
				jsonObject->getString(FTL_STR("value"))
				);
		}
		else if (descStr == FTL_STR("execPortTypeChanged"))
		{
			onExecPortTypeChanged(
				jsonObject->getString(FTL_STR("portName")),
				jsonObject->getString(FTL_STR("newExecPortType"))
				);
		}
		else if (descStr == FTL_STR("nodePortTypeChanged"))
		{
			onNodePortTypeChanged(
				jsonObject->getString(FTL_STR("nodeName")),
				jsonObject->getString(FTL_STR("portName")),
				jsonObject->getString(FTL_STR("newNodePortType"))
				);
		}
		else if (descStr == FTL_STR("refVarPathChanged"))
		{
			onRefVarPathChanged(
				jsonObject->getString(FTL_STR("refName")),
				jsonObject->getString(FTL_STR("newVarPath"))
				);
		}
		else if (descStr == FTL_STR("funcCodeChanged"))
		{
			onFuncCodeChanged(
				jsonObject->getString(FTL_STR("code"))
				);
		}
		else if (descStr == FTL_STR("extDepsChanged"))
		{
			onExecExtDepsChanged(
				jsonObject->getString(FTL_STR("extDeps"))
				);
		}
		else if (descStr == FTL_STR("execPortDefaultValuesChanged"))
		{
			onExecPortDefaultValuesChanged(
				jsonObject->getString(FTL_STR("portName"))
				);
		}
		else if (descStr == FTL_STR("nodePortDefaultValuesChanged"))
		{
			onNodePortDefaultValuesChanged(
				jsonObject->getString(FTL_STR("nodeName")),
				jsonObject->getString(FTL_STR("portName"))
				);
		}
		else if (descStr == FTL_STR("removedFromOwner"))
		{
			onRemovedFromOwner();
		}
		else if (descStr == FTL_STR("execPortsReordered"))
		{
			const FTL::JSONArray * newOrder = jsonObject->maybeGetArray(FTL_STR("newOrder"));
			if (newOrder)
			{
				std::vector<unsigned int> indices;
				for (size_t i = 0; i < newOrder->size(); i++)
				{
					const FTL::JSONValue * indexVal = newOrder->get(i);
					unsigned int index = indexVal->getSInt32Value();
					indices.push_back(index);
				}

				if (indices.size() > 0)
					onExecPortsReordered(indices.size(), &indices[0]);
			}
		}
		else if (descStr == FTL_STR("nodePortsReordered"))
		{
			FTL::CStrRef nodeName = jsonObject->getString(FTL_STR("nodeName"));
			const FTL::JSONArray * newOrder = jsonObject->maybeGetArray(FTL_STR("newOrder"));
			if (newOrder)
			{
				std::vector<unsigned int> indices;
				for (size_t i = 0; i < newOrder->size(); i++)
				{
					const FTL::JSONValue * indexVal = newOrder->get(i);
					unsigned int index = indexVal->getSInt32Value();
					indices.push_back(index);
				}

				if (indices.size() > 0)
					onNodePortsReordered(nodeName, indices.size(), &indices[0]);
			}
		}
		else if (descStr == FTL_STR("execDidAttachPreset"))
		{
			FTL::CStrRef presetFilePath = jsonObject->getString(FTL_STR("presetFilePath"));
			onExecDidAttachPreset(presetFilePath);
		}
		else if (descStr == FTL_STR("instExecDidAttachPreset"))
		{
			FTL::CStrRef nodeName = jsonObject->getString(FTL_STR("nodeName"));
			FTL::CStrRef presetFilePath = jsonObject->getString(FTL_STR("presetFilePath"));
			onInstExecDidAttachPreset(nodeName, presetFilePath);
		}
		else if (descStr == FTL_STR("execWillDetachPreset"))
		{
			FTL::CStrRef presetFilePath = jsonObject->getString(FTL_STR("presetFilePath"));
			onExecWillDetachPreset(presetFilePath);
		}
		else if (descStr == FTL_STR("instExecWillDetachPreset"))
		{
			FTL::CStrRef nodeName = jsonObject->getString(FTL_STR("nodeName"));
			FTL::CStrRef presetFilePath = jsonObject->getString(FTL_STR("presetFilePath"));
			onInstExecWillDetachPreset(nodeName, presetFilePath);
		}
		else if (descStr == FTL_STR("execPresetFileRefCountDidChange"))
		{
			int newPresetFileRefCount = jsonObject->getSInt32(FTL_STR("newPresetFileRefCount"));
			onExecPresetFileRefCountDidChange(newPresetFileRefCount);
		}
		else if (descStr == FTL_STR("instExecPresetFileRefCountDidChange"))
		{
			FTL::CStrRef nodeName = jsonObject->getString(FTL_STR("nodeName"));
			int newPresetFileRefCount = jsonObject->getSInt32(FTL_STR("newPresetFileRefCount"));
			onInstExecPresetFileRefCountDidChange(nodeName, newPresetFileRefCount);
		}
		else if (descStr == FTL_STR("execPortMetadataChanged"))
		{
			onExecPortMetadataChanged(
				jsonObject->getString(FTL_STR("portName")),
				jsonObject->getString(FTL_STR("key")),
				jsonObject->getString(FTL_STR("value"))
				);
		}
		else
		{
			printf(
				"NotificationRouter::callback: Unhandled notification:\n%s\n",
				jsonStr.data()
				);
		}
	}
	catch (FabricCore::Exception e)
	{
		printf(
			"NotificationRouter::callback: caught Core exception: %s\n",
			e.getDesc_cstr()
			);
	}
	catch (FTL::JSONException e)
	{
		printf(
			"NotificationRouter::callback: caught FTL::JSONException: %s\n",
			e.getDescCStr()
			);
	}
}

#pragma endregion

////////////////////////////////////////////////////////////////////////////
#pragma region DFG-Derived functions

void MaxDFGNotificationHandler::onFuncCodeChanged(FTL::CStrRef code)
{
}


void MaxDFGNotificationHandler::onRefVarPathChanged(FTL::CStrRef refName, FTL::CStrRef newVarPath)
{
}


void MaxDFGNotificationHandler::onNodePortTypeChanged(FTL::CStrRef nodeName, FTL::CStrRef portName, FTL::CStrRef nodePortType)
{
}


void MaxDFGNotificationHandler::onExecPortTypeChanged(FTL::CStrRef portName, FTL::CStrRef execPortType)
{
}


void MaxDFGNotificationHandler::onNodePortMetadataChanged(FTL::CStrRef nodeName, FTL::CStrRef portName, FTL::CStrRef key, FTL::CStrRef value)
{
}


void MaxDFGNotificationHandler::onExecPortMetadataChanged(FTL::CStrRef portName, FTL::CStrRef key, FTL::CStrRef value)
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
}


void MaxDFGNotificationHandler::onNodePortResolvedTypeChanged(FTL::CStrRef nodeName, FTL::CStrRef portName, FTL::CStrRef newResolvedType)
{
}


void MaxDFGNotificationHandler::onExecPortTypeSpecChanged(FTL::CStrRef portName, FTL::CStrRef typeSpec)
{
}


void MaxDFGNotificationHandler::onExecPortResolvedTypeChanged(FTL::CStrRef portName, FTL::CStrRef newResolvedType)
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
}


void MaxDFGNotificationHandler::onExecCacheRuleChanged(FTL::CStrRef newCacheRule)
{
}


void MaxDFGNotificationHandler::onNodeCacheRuleChanged(FTL::CStrRef nodeName, FTL::CStrRef newCacheRule)
{
}


void MaxDFGNotificationHandler::onExtDepRemoved(FTL::CStrRef extension, FTL::CStrRef version)
{
}


void MaxDFGNotificationHandler::onExtDepAdded(FTL::CStrRef extension, FTL::CStrRef version)
{
}


void MaxDFGNotificationHandler::onExecMetadataChanged(FTL::CStrRef key, FTL::CStrRef value)
{
}


void MaxDFGNotificationHandler::onNodePortRenamed(FTL::CStrRef nodeName, FTL::CStrRef oldPortName, FTL::CStrRef newPortName)
{
}


void MaxDFGNotificationHandler::onExecPortRenamed(FTL::CStrRef oldPortName, FTL::CStrRef newPortName, FTL::JSONObject const *execPortJSONObject)
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
}


void MaxDFGNotificationHandler::onNodeTitleChanged(FTL::CStrRef nodePath, FTL::CStrRef title)
{
}


void MaxDFGNotificationHandler::onNodeMetadataChanged(FTL::CStrRef nodePath, FTL::CStrRef key, FTL::CStrRef value)
{
}


void MaxDFGNotificationHandler::onPortsDisconnected(FTL::CStrRef srcPath, FTL::CStrRef dstPath)
{
	m_pInterface->InvalidateAll();
}


void MaxDFGNotificationHandler::onPortsConnected(FTL::CStrRef srcPath, FTL::CStrRef dstPath)
{
	m_pInterface->InvalidateAll();
}


void MaxDFGNotificationHandler::onExecPortRemoved(FTL::CStrRef portName)
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
}


void MaxDFGNotificationHandler::onExecPortInserted(FTL::CStrRef portName, FTL::JSONObject const *jsonObject)
{
	if (theHold.RestoreOrRedoing())
		return;
}


void MaxDFGNotificationHandler::onNodePortRemoved(FTL::CStrRef nodeName, FTL::CStrRef portName)
{
}


void MaxDFGNotificationHandler::onNodePortInserted(FTL::CStrRef nodeName, FTL::CStrRef portName, FTL::JSONObject const *jsonObject)
{
}


void MaxDFGNotificationHandler::onNodeRemoved(FTL::CStrRef nodeName)
{
}


void MaxDFGNotificationHandler::onNodeInserted(FTL::CStrRef nodeName, FTL::JSONObject const *jsonObject)
{
}

//void MaxDFGNotificationHandler::onExecPortRenamed(FabricServices::DFGWrapper::ExecPortPtr port, const char * oldName)
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
//void MaxDFGNotificationHandler::onExecPortResolvedTypeChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * resolvedType)
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
//void MaxDFGNotificationHandler::onExecPortMetadataChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * key, const char * metadata)
//{

//}

void MaxDFGNotificationHandler::onExecExtDepsChanged(
	FTL::CStrRef extDeps
	)
{

}
void MaxDFGNotificationHandler::onExecPortDefaultValuesChanged(
	FTL::CStrRef portName
	)
{

}
void MaxDFGNotificationHandler::onNodePortDefaultValuesChanged(
	FTL::CStrRef nodeName,
	FTL::CStrRef portName
	)
{

}
void MaxDFGNotificationHandler::onRemovedFromOwner()
{

}
void MaxDFGNotificationHandler::onExecPortsReordered(
	unsigned int indexCount,
	unsigned int * indices
	)
{

}
void MaxDFGNotificationHandler::onNodePortsReordered(
	FTL::CStrRef nodeName,
	unsigned int indexCount,
	unsigned int * indices
	)
{

}
void MaxDFGNotificationHandler::onExecDidAttachPreset(
	FTL::CStrRef presetFilePath
	)
{

}
void MaxDFGNotificationHandler::onInstExecDidAttachPreset(
	FTL::CStrRef nodeName,
	FTL::CStrRef presetFilePath
	)
{

}
void MaxDFGNotificationHandler::onExecWillDetachPreset(
	FTL::CStrRef presetFilePath
	)
{

}
void MaxDFGNotificationHandler::onInstExecWillDetachPreset(
	FTL::CStrRef nodeName,
	FTL::CStrRef presetFilePath
	)
{

}
void MaxDFGNotificationHandler::onExecPresetFileRefCountDidChange(
	int newPresetFileRefCount
	)
{

}
void MaxDFGNotificationHandler::onInstExecPresetFileRefCountDidChange(
	FTL::CStrRef nodeName,
	int newPresetFileRefCount
	)
{

}

#pragma endregion


FabricCore::DFGExec MaxDFGNotificationHandler::GetExec()
{
	return GetBinding().getExec();
}

FabricCore::DFGBinding& MaxDFGNotificationHandler::GetBinding()
{
	return m_pInterface->GetBinding();
}
