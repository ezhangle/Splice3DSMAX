#include "StdAfx.h"
//#include "MaxDFGNotificationHandler.h"
//#include "SpliceTranslationFPInterface.h"
//
//void bindingNotificationCallback(void * userData, char const *jsonStr, uint32_t jsonLength);
//
//MaxDFGNotificationHandler::MaxDFGNotificationHandler(SpliceTranslationFPInterface* pInterface)
//	: m_pInterface(pInterface)
//{
//	updateBinding(GetBinding());
//}
//
//MaxDFGNotificationHandler::~MaxDFGNotificationHandler()
//{
//
//}
//
//#pragma region callback translation
//
//void MaxDFGNotificationHandler::updateBinding(FabricCore::DFGBinding& binding)
//{
//	if (binding.isValid())
//		binding.setNotificationCallback(bindingNotificationCallback, this);
//}
//
//void bindingNotificationCallback(void * userData, char const *jsonStr, uint32_t jsonLength)
//{
//	MaxDFGNotificationHandler* handler = reinterpret_cast<MaxDFGNotificationHandler*>(userData);
//	handler->callback(jsonStr);
//}
//
//void MaxDFGNotificationHandler::callback(FTL::CStrRef jsonStr)
//{
//	//try
//	//{
//	//	FTL::JSONStrWithLoc jsonStrWithLoc(jsonStr);
//	//	FTL::OwnedPtr<FTL::JSONObject> jsonObject(
//	//		FTL::JSONValue::Decode(jsonStrWithLoc)->cast<FTL::JSONObject>()
//	//		);
//	//	FTL::StrRef descStr = jsonObject->getString(FTL_STR("desc"));
//
//	//	if (descStr == FTL_STR("argInserted")) 
//	//	{
//	//		onArgInserted(
//	//			jsonObject->getSInt32(FTL_STR("index")),
//	//			jsonObject->getString(FTL_STR("name")),
//	//			jsonObject->getStringOrEmpty(FTL_STR("type"))
//	//			);
//	//	}
//	//	else if (descStr == FTL_STR("argTypeChanged"))
//	//	{
//	//		onArgTypeChanged(
//	//			jsonObject->getSInt32(FTL_STR("index")),
//	//			jsonObject->getString(FTL_STR("name")),
//	//			jsonObject->getStringOrEmpty(FTL_STR("newType"))
//	//			);
//	//	}
//	//	else if (descStr == FTL_STR("argRenamed"))
//	//	{
//	//		onArgRenamed(
//	//			jsonObject->getString(FTL_STR("oldName")),
//	//			jsonObject->getStringOrEmpty(FTL_STR("newName"))
//	//			);
//	//	}
//	//	else if (descStr == FTL_STR("argRemoved")) {
//	//		onArgRemoved(jsonObject->getString(FTL_STR("name")));
//	//	}
//	//	else if (descStr == FTL_STR("argChanged")) {
//	//		onArgChanged(jsonObject->getString(FTL_STR("name")));
//	//	}
//	//	else if (descStr == FTL_STR("dirty")) {
//	//		onDirty();
//	//	}
//	//}
//	//catch (FabricCore::Exception e)
//	//{
//	//	printf(
//	//		"NotificationRouter::callback: caught Core exception: %s\n",
//	//		e.getDesc_cstr()
//	//		);
//	//}
//	//catch (FTL::JSONException e)
//	//{
//	//	printf(
//	//		"NotificationRouter::callback: caught FTL::JSONException: %s\n",
//	//		e.getDescCStr()
//	//		);
//	//}
//}
//
//#pragma endregion
//
//////////////////////////////////////////////////////////////////////////////
//#pragma region DFG-Derived functions
//
//void MaxDFGNotificationHandler::onArgInserted(int index, FTL::CStrRef name, FTL::CStrRef type)
//{
//
//}
//
////
////
////void MaxDFGNotificationHandler::onExecPortMetadataChanged(FTL::CStrRef portName, FTL::CStrRef key, FTL::CStrRef value)
////{
////	if (key == MAX_PARM_TYPE_OPT)
////	{
////		int type = atoi(value.data());
////		m_pInterface->SetMaxConnectedType(portName.c_str(), type);
////	}
////	else if (key == "uiHidden" || key == "uiOpaque")
////	{
////		if (value == "true")
////		{
////			// If our port is hidden or opaque, it means it has no Max connection
////			m_pInterface->SetMaxConnectedType(portName.c_str(), -1);
////		}
////		else
////		{
////			// Recreate with default max type
////			m_pInterface->SetMaxConnectedType(portName.c_str(), -2);
////		}
////		m_pInterface->UpdateUISpec();
////	}
////	else if (key == "uiRange")
////	{
////		::SetMaxParamLimits(m_pInterface->GetPBlock()->GetDesc(), m_pInterface->GetBinding(), portName.c_str());
////		m_pInterface->UpdateUISpec();
////	}
////	else if (key == "scalarUnit")
////	{
////		// TODO: Sync our ScalarUnit label with Maya's
////		m_pInterface->UpdateUISpec();
////	}
////}
//
//
//void MaxDFGNotificationHandler::onArgTypeChanged(int index, FTL::CStrRef name, FTL::CStrRef newType)
//{
//	//	HoldActions hold(_M("Splice Port Type Changed"));
//	if (theHold.RestoreOrRedoing())
//		return;
//
//	if (GetExec().getExecPortType(name.c_str()) == FabricCore::DFGPortType_In)
//	{
//		int type = m_pInterface->GetMaxTypeForArg(name.c_str());
//		// Attempt to set the same type back.  If the max type is no
//		// longer legitimate, the type will be reset to default.
//		m_pInterface->SetMaxTypeForArg(name.c_str(), type);
//	}
//}
//
//
//void MaxDFGNotificationHandler::onArgRenamed(FTL::CStrRef oldName, FTL::CStrRef newName)
//{
//	if (theHold.RestoreOrRedoing())
//		return;
//
//	int pid = GetPortParamID(GetBinding(), newName.c_str());
//	if (pid >= 0)
//	{
//		// Normally when we edit the pblock, changes
//		// are notified in the form of a call to SetReference
//		// However, when changing name, no references change
//		// so we need to manually update the UI
//		MSTR str = MSTR::FromACP(newName.c_str());
//		m_pInterface->SetPblockParamName(pid, str);
//	}
//}
//
//void MaxDFGNotificationHandler::onArgRemoved(FTL::CStrRef name)
//{
//	if (theHold.RestoreOrRedoing())
//		return;
//	
//	int pid = GetPortParamID(GetBinding(), name.c_str());
//	if (pid >= 0)
//	{
//		m_pInterface->SetMaxTypeForArg(name.c_str(), -1);
//	}
//
//	m_pInterface->InvalidateAll();
//}
//
//void MaxDFGNotificationHandler::onArgChanged(FTL::CStrRef name)
//{
//
//}
//
//void MaxDFGNotificationHandler::onDirty()
//{
//
//}
//
////
////void MaxDFGNotificationHandler::onExecPortResolvedTypeChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * resolvedType)
////{
//////	HoldActions hold(_M("Splice Port Type Changed"));
////	if (theHold.RestoreOrRedoing())
////		return;
////
////	if (port->getOutsidePortType() != FabricCore::DFGPortType_Out)
////		SetMaxConnectedType(port, GetPort3dsMaxType(port));
////}
////
////
////void MaxDFGNotificationHandler::onExecPortMetadataChanged(FabricServices::DFGWrapper::ExecPortPtr port, const char * key, const char * metadata)
////{
//
////}
//
//#pragma endregion
//
//
//FabricCore::DFGExec MaxDFGNotificationHandler::GetExec()
//{
//	return GetBinding().getExec();
//}
//
////FabricCore::DFGBinding& MaxDFGNotificationHandler::GetBinding()
////{
////	return m_pInterface->GetBinding();
////}
