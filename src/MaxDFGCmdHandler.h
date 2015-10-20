#pragma once


#include <FabricUI/DFG/DFGUICmdHandler.h>

class MaxDFGCmdHandler : public FabricUI::DFG::DFGUICmdHandler
{
	virtual void dfgDoRemoveNodes(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::ArrayRef<FTL::StrRef> nodeNames) override;

	virtual void dfgDoConnect(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef srcPath, FTL::CStrRef dstPath) override;

	virtual void dfgDoDisconnect(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef srcPath, FTL::CStrRef dstPath) override;

	virtual std::string dfgDoAddGraph(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef title, QPointF pos) override;

	virtual std::string dfgDoAddFunc(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef title, FTL::CStrRef initialCode, QPointF pos) override;

	virtual std::string dfgDoInstPreset(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef presetPath, QPointF pos) override;

	virtual std::string dfgDoAddVar(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef desiredNodeName, FTL::CStrRef dataType, FTL::CStrRef extDep, QPointF pos) override;

	virtual std::string dfgDoAddGet(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef desiredNodeName, FTL::CStrRef varPath, QPointF pos) override;

	virtual std::string dfgDoAddSet(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef desiredNodeName, FTL::CStrRef varPath, QPointF pos) override;

	virtual std::string dfgDoAddPort(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef desiredPortName, FabricCore::DFGPortType portType, FTL::CStrRef typeSpec, FTL::CStrRef portToConnect, FTL::StrRef extDep, FTL::CStrRef metaData) override;

	virtual std::string dfgDoEditPort(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::StrRef oldPortName, FTL::StrRef desiredNewPortName, FTL::StrRef typeSpec, FTL::StrRef extDep, FTL::StrRef uiMetadata) override;

	virtual void dfgDoRemovePort(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef portName) override;

	virtual void dfgDoMoveNodes(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::ArrayRef<FTL::StrRef> nodeNames, FTL::ArrayRef<QPointF> newTopLeftPoss) override;

	virtual void dfgDoResizeBackDrop(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef backDropNodeName, QPointF newTopLeftPos, QSizeF newSize) override;

	virtual std::string dfgDoImplodeNodes(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::ArrayRef<FTL::StrRef> nodeNames, FTL::CStrRef desiredNodeName) override;

	virtual std::vector<std::string> dfgDoExplodeNode(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef nodeName) override;

	virtual void dfgDoAddBackDrop(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef title, QPointF pos) override;

	virtual void dfgDoSetNodeTitle(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef nodeName, FTL::CStrRef title) override;

	virtual void dfgDoSetNodeComment(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef nodeName, FTL::CStrRef comment) override;

	virtual void dfgDoSetCode(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef code) override;

	virtual std::string dfgDoRenamePort(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef oldPortName, FTL::CStrRef desiredNewPortName) override;

	virtual std::vector<std::string> dfgDoPaste(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef json, QPointF cursorPos) override;

	virtual void dfgDoSetArgType(FabricCore::DFGBinding const &binding, FTL::CStrRef argName, FTL::CStrRef typeName) override;

	virtual void dfgDoSetArgValue(FabricCore::DFGBinding const &binding, FTL::CStrRef argName, FabricCore::RTVal const &value) override;

	virtual void dfgDoSetPortDefaultValue(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef portPath, FabricCore::RTVal const &value) override;

	virtual void dfgDoSetRefVarPath(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::CStrRef refName, FTL::CStrRef varPath) override;

	virtual void dfgDoReorderPorts(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, const std::vector<unsigned int> & indices) override;

	virtual void dfgDoSetExtDeps(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec, FTL::ArrayRef<FTL::StrRef> extDeps) override;

	virtual void dfgDoSplitFromPreset(FabricCore::DFGBinding const &binding, FTL::CStrRef execPath, FabricCore::DFGExec const &exec) override;

};
