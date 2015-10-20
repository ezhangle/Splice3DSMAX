#include "StdAfx.h"
#include "MaxDFGCmdHandler.h"


void MaxDFGCmdHandler::dfgDoRemoveNodes(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::ArrayRef<FTL::StrRef> nodeNames)
{
}

void MaxDFGCmdHandler::dfgDoSplitFromPreset(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec)
{
}

void MaxDFGCmdHandler::dfgDoSetExtDeps(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::ArrayRef<FTL::StrRef> extDeps)
{

}

void MaxDFGCmdHandler::dfgDoReorderPorts(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, const std::vector<unsigned int> & indices)
{

}

void MaxDFGCmdHandler::dfgDoSetRefVarPath(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef refName, FTL::CStrRef varPath)
{

}

void MaxDFGCmdHandler::dfgDoSetPortDefaultValue(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef portPath, FabricCore::RTVal const &value)
{

}

void MaxDFGCmdHandler::dfgDoSetArgValue(FabricCore::DFGBinding const &binding, FTL::CStrRef argName, FabricCore::RTVal const &value)
{

}

void MaxDFGCmdHandler::dfgDoSetArgType(FabricCore::DFGBinding const &binding, FTL::CStrRef argName, FTL::CStrRef typeName)
{

}

std::vector<std::string> MaxDFGCmdHandler::dfgDoPaste(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef json, QPointF cursorPos)
{
	return std::vector < std::string >();
}

std::string MaxDFGCmdHandler::dfgDoRenamePort(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef oldPortName, FTL::CStrRef desiredNewPortName)
{
	return "";
}

void MaxDFGCmdHandler::dfgDoSetCode(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef code)
{

}

void MaxDFGCmdHandler::dfgDoSetNodeComment(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef nodeName, FTL::CStrRef comment)
{

}

void MaxDFGCmdHandler::dfgDoSetNodeTitle(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef nodeName, FTL::CStrRef title)
{

}

void MaxDFGCmdHandler::dfgDoAddBackDrop(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef title, QPointF pos)
{

}

std::vector<std::string> MaxDFGCmdHandler::dfgDoExplodeNode(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef nodeName)
{
	return std::vector<std::string>();
}

std::string MaxDFGCmdHandler::dfgDoImplodeNodes(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::ArrayRef<FTL::StrRef> nodeNames, FTL::CStrRef desiredNodeName)
{
	return "";
}

void MaxDFGCmdHandler::dfgDoResizeBackDrop(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef backDropNodeName, QPointF newTopLeftPos, QSizeF newSize)
{

}

void MaxDFGCmdHandler::dfgDoMoveNodes(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::ArrayRef<FTL::StrRef> nodeNames, FTL::ArrayRef<QPointF> newTopLeftPoss)
{

}

void MaxDFGCmdHandler::dfgDoRemovePort(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef portName)
{

}

std::string MaxDFGCmdHandler::dfgDoEditPort(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::StrRef oldPortName, FTL::StrRef desiredNewPortName, FTL::StrRef typeSpec, FTL::StrRef extDep, FTL::StrRef uiMetadata)
{
	return "";
}

std::string MaxDFGCmdHandler::dfgDoAddPort(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef desiredPortName, FabricCore::DFGPortType portType, FTL::CStrRef typeSpec, FTL::CStrRef portToConnect, FTL::StrRef extDep, FTL::CStrRef metaData)
{
	const_cast<FabricCore::DFGExec&>(exec).addExecPort(desiredPortName.c_str(), portType, typeSpec.c_str());
	return "";
}

std::string MaxDFGCmdHandler::dfgDoAddSet(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef desiredNodeName, FTL::CStrRef varPath, QPointF pos)
{
	return "";
}

std::string MaxDFGCmdHandler::dfgDoAddGet(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef desiredNodeName, FTL::CStrRef varPath, QPointF pos)
{
	return "";
}

std::string MaxDFGCmdHandler::dfgDoAddVar(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef desiredNodeName, FTL::CStrRef dataType, FTL::CStrRef extDep, QPointF pos)
{
	return "";
}

std::string MaxDFGCmdHandler::dfgDoInstPreset(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef presetPath, QPointF pos)
{
	return "";
}

std::string MaxDFGCmdHandler::dfgDoAddFunc(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef title, FTL::CStrRef initialCode, QPointF pos)
{
	return "";
}

std::string MaxDFGCmdHandler::dfgDoAddGraph(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef title, QPointF pos)
{
	return "";
}

void MaxDFGCmdHandler::dfgDoDisconnect(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef srcPath, FTL::CStrRef dstPath)
{

}

void MaxDFGCmdHandler::dfgDoConnect(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef srcPath, FTL::CStrRef dstPath)
{

}
