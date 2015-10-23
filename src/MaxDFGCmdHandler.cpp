#include "StdAfx.h"
#include "MaxDFGCmdHandler.h"




MaxDFGCmdHandler::MaxDFGCmdHandler(SpliceTranslationFPInterface* pTranslation)
	: DFGUICmdHandler_QUndo(GetQtUndoStack())
	, m_pTranslationLayer(pTranslation)
{

}

MaxDFGCmdHandler::~MaxDFGCmdHandler()
{

}

void MaxDFGCmdHandler::dfgDoRemoveNodes(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::ArrayRef<FTL::StrRef> nodeNames)
{
	DFGHoldActions hold(_M("DFG Remove Nodes"));
	return __super::dfgDoRemoveNodes(binding, execPath, exec, nodeNames);
}

void MaxDFGCmdHandler::dfgDoConnect(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef srcPath, FTL::CStrRef dstPath)
{
	DFGHoldActions hold(_M("DFG Connect"));
	return __super::dfgDoConnect(binding, execPath, exec, srcPath, dstPath);
}

void MaxDFGCmdHandler::dfgDoDisconnect(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef srcPath, FTL::CStrRef dstPath)
{
	DFGHoldActions hold(_M("DFG Disconnect"));
	return __super::dfgDoDisconnect(binding, execPath, exec, srcPath, dstPath);
}

std::string MaxDFGCmdHandler::dfgDoAddGraph(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef title, QPointF pos)
{
	DFGHoldActions hold(_M("DFG Add Graph"));
	return __super::dfgDoAddGraph(binding, execPath, exec, title, pos);
}

std::string MaxDFGCmdHandler::dfgDoAddFunc(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef title, FTL::CStrRef initialCode, QPointF pos)
{
	DFGHoldActions hold(_M("DFG Add Function"));
	return __super::dfgDoAddFunc(binding, execPath, exec, title, initialCode, pos);
}

std::string MaxDFGCmdHandler::dfgDoInstPreset(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef presetPath, QPointF pos)
{
	DFGHoldActions hold(_M("DFG Load Preset"));
	return __super::dfgDoInstPreset(binding, execPath, exec, presetPath, pos);
}

std::string MaxDFGCmdHandler::dfgDoAddVar(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef desiredNodeName, FTL::CStrRef dataType, FTL::CStrRef extDep, QPointF pos)
{
	DFGHoldActions hold(_M("DFG Add Var"));
	return __super::dfgDoAddVar(binding, execPath, exec, desiredNodeName, dataType, extDep, pos);
}

std::string MaxDFGCmdHandler::dfgDoAddGet(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef desiredNodeName, FTL::CStrRef varPath, QPointF pos)
{
	DFGHoldActions hold(_M("DFG Add Get"));
	return __super::dfgDoAddGet(binding, execPath, exec, desiredNodeName, varPath, pos);
}

std::string MaxDFGCmdHandler::dfgDoAddSet(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef desiredNodeName, FTL::CStrRef varPath, QPointF pos)
{
	DFGHoldActions hold(_M("DFG Add Set"));
	return __super::dfgDoAddSet(binding, execPath, exec, desiredNodeName, varPath, pos);
}

std::string MaxDFGCmdHandler::dfgDoAddPort(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef desiredPortName, FabricCore::DFGPortType portType, FTL::CStrRef typeSpec, FTL::CStrRef portToConnect, FTL::StrRef extDep, FTL::CStrRef metaData)
{
	DFGHoldActions hold(_M("DFG Add Port"));

	if (portType == FabricCore::DFGPortType_In)
	{
		int maxPortType = SpliceTypeToDefaultMaxType(typeSpec.c_str());
		if (maxPortType >= 0)
		{
			std::string sMetaData = metaData;
			FTL::JSONEnc<> enc(sMetaData);
			FTL::JSONObjectEnc<> objEnc(enc);
			{
				FTL::JSONEnc<> maxTypeEnc(objEnc, FTL_STR(MAX_PARM_TYPE_OPT));
				FTL::JSONFloat64Enc<> xS32Enc(maxTypeEnc, -2);
			}
			metaData = sMetaData;
		}
	}

	std::string res = __super::dfgDoAddPort(binding, execPath, exec, desiredPortName, portType, typeSpec, portToConnect, extDep, metaData);

	// If we have add a new 'in' port, by default we create a matching 3ds max port.
	int pid = m_pTranslationLayer->SyncMetaDataFromPortToParam(res.c_str());
	return res;
}

std::string MaxDFGCmdHandler::dfgDoEditPort(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::StrRef oldPortName, FTL::StrRef desiredNewPortName, FTL::StrRef typeSpec, FTL::StrRef extDep, FTL::StrRef uiMetadata)
{
	DFGHoldActions hold(_M("DFG Edit Port"));
	return __super::dfgDoEditPort(binding, execPath, exec, oldPortName, desiredNewPortName, typeSpec, extDep, uiMetadata);
}

void MaxDFGCmdHandler::dfgDoRemovePort(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef portName)
{
	DFGHoldActions hold(_M("DFG Remove Port"));
	if (!theHold.RestoreOrRedoing())
	{
		int pid = GetPortParamID(binding, execPath.c_str());
		if (pid >= 0)
		{
			m_pTranslationLayer->SetMaxTypeForArg(MSTR::FromACP(execPath.c_str()), -1);
		}
	}

	return __super::dfgDoRemovePort(binding, execPath, exec, portName);
}

void MaxDFGCmdHandler::dfgDoResizeBackDrop(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef backDropNodeName, QPointF newTopLeftPos, QSizeF newSize)
{
	DFGHoldActions hold(_M("DFG Resize Back Drop"));
	return __super::dfgDoResizeBackDrop(binding, execPath, exec, backDropNodeName, newTopLeftPos, newSize);
}

void MaxDFGCmdHandler::dfgDoMoveNodes(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::ArrayRef<FTL::StrRef> nodeNames, FTL::ArrayRef<QPointF> newTopLeftPoss)
{
	DFGHoldActions hold(_M("DFG Move Nodes"));
	return __super::dfgDoMoveNodes(binding, execPath, exec, nodeNames, newTopLeftPoss);
}

std::string MaxDFGCmdHandler::dfgDoImplodeNodes(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::ArrayRef<FTL::StrRef> nodeNames, FTL::CStrRef desiredNodeName)
{
	DFGHoldActions hold(_M("DFG Implode Nodes"));
	return __super::dfgDoImplodeNodes(binding, execPath, exec, nodeNames, desiredNodeName);
}

std::vector<std::string> MaxDFGCmdHandler::dfgDoExplodeNode(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef nodeName)
{
	DFGHoldActions hold(_M("DFG Explode Nodes"));
	return __super::dfgDoExplodeNode(binding, execPath, exec, nodeName);
}

void MaxDFGCmdHandler::dfgDoAddBackDrop(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef title, QPointF pos)
{
	DFGHoldActions hold(_M("DFG Add BackDrop"));
	return __super::dfgDoAddBackDrop(binding, execPath, exec, title, pos);
}

void MaxDFGCmdHandler::dfgDoSetNodeTitle(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef nodeName, FTL::CStrRef newTitle)
{
	DFGHoldActions hold(_M("DFG Set Node Title"));
	return __super::dfgDoSetNodeTitle(binding, execPath, exec, nodeName, newTitle);
}

void MaxDFGCmdHandler::dfgDoSetNodeComment(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef nodeName, FTL::CStrRef comment)
{
	DFGHoldActions hold(_M("DFG Set Node Comment"));
	return __super::dfgDoSetNodeComment(binding, execPath, exec, nodeName, comment);
}

void MaxDFGCmdHandler::dfgDoSetCode(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef code)
{
	DFGHoldActions hold(_M("DFG Set Code"));
	return __super::dfgDoSetCode(binding, execPath, exec, code);
}

std::string MaxDFGCmdHandler::dfgDoRenamePort(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef oldName, FTL::CStrRef desiredNewName)
{
	DFGHoldActions hold(_M("DFG Rename Port"));
	return __super::dfgDoRenamePort(binding, execPath, exec, oldName, desiredNewName);
}

std::vector<std::string> MaxDFGCmdHandler::dfgDoPaste(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef json, QPointF cursorPos)
{
	DFGHoldActions hold(_M("DFG Paste"));
	return __super::dfgDoPaste(binding, execPath, exec, json, cursorPos);
}

void MaxDFGCmdHandler::dfgDoSetArgType(FabricCore::DFGBinding const &binding, FTL::CStrRef argName, FTL::CStrRef typeName)
{
	DFGHoldActions hold(_M("DFG Set Arg Type"));

	//	HoldActions hold(_M("Splice Port Type Changed"));
	if (binding.getExec().getExecPortType(argName.c_str()) == FabricCore::DFGPortType_In)
	{
		int type = m_pTranslationLayer->GetMaxTypeForArg(argName.c_str());
		// Attempt to set the same type back.  If the max type is no
		// longer legitimate, the type will be reset to default.
		m_pTranslationLayer->SetMaxTypeForArg(argName.c_str(), type);
	}
	return __super::dfgDoSetArgType(binding, argName, typeName);
}

void MaxDFGCmdHandler::dfgDoSetArgValue(FabricCore::DFGBinding const &binding, FTL::CStrRef argName, FabricCore::RTVal const &value)
{
	DFGHoldActions hold(_M("DFG Set Arg Value"));

	return __super::dfgDoSetArgValue(binding, argName, value);
}

void MaxDFGCmdHandler::dfgDoSetPortDefaultValue(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef portOrPinPath, FabricCore::RTVal const &value)
{
	DFGHoldActions hold(_M("DFG Set Def Value"));
	return __super::dfgDoSetPortDefaultValue(binding, execPath, exec, portOrPinPath, value);
}

void MaxDFGCmdHandler::dfgDoSetRefVarPath(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef refName, FTL::CStrRef varPath)
{
	DFGHoldActions hold(_M("DFG Set Ref Path"));
	return __super::dfgDoSetRefVarPath(binding, execPath, exec, refName, varPath);
}

void MaxDFGCmdHandler::dfgDoReorderPorts(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, const std::vector<unsigned int> & indices)
{
	DFGHoldActions hold(_M("DFG Re-order Ports"));
	return __super::dfgDoReorderPorts(binding, execPath, exec, indices);
}

void MaxDFGCmdHandler::dfgDoSetExtDeps(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::ArrayRef<FTL::StrRef> extDeps)
{
	DFGHoldActions hold(_M("DFG Set External Dependencies"));
	return __super::dfgDoSetExtDeps(binding, execPath, exec, extDeps);
}

void MaxDFGCmdHandler::dfgDoSplitFromPreset(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec)
{
	DFGHoldActions hold(_M("DFG Split From Preset"));
	return __super::dfgDoSplitFromPreset(binding, execPath, exec);
}