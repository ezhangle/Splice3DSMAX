#pragma once


#include <FabricUI/DFG/DFGUICmdHandler_QUndo.h>

class SpliceTranslationFPInterface;

class MaxDFGCmdHandler : public FabricUI::DFG::DFGUICmdHandler_QUndo
{
private:

	SpliceTranslationFPInterface* m_pTranslationLayer;

public:
	MaxDFGCmdHandler(SpliceTranslationFPInterface* pTranslation);
	~MaxDFGCmdHandler();

	void dfgDoRemoveNodes(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::ArrayRef<FTL::StrRef> nodeNames
		) override;

	void dfgDoConnect(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef srcPath,
		FTL::CStrRef dstPath
		) override;

	void dfgDoDisconnect(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef srcPath,
		FTL::CStrRef dstPath
		) override;

	std::string dfgDoAddGraph(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef title,
		QPointF pos
		) override;

	std::string dfgDoAddFunc(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef title,
		FTL::CStrRef initialCode,
		QPointF pos
		) override;

	std::string dfgDoInstPreset(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef presetPath,
		QPointF pos
		) override;

	std::string dfgDoAddVar(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef desiredNodeName,
		FTL::CStrRef dataType,
		FTL::CStrRef extDep,
		QPointF pos
		) override;

	std::string dfgDoAddGet(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef desiredNodeName,
		FTL::CStrRef varPath,
		QPointF pos
		) override;

	std::string dfgDoAddSet(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef desiredNodeName,
		FTL::CStrRef varPath,
		QPointF pos
		) override;

	std::string dfgDoAddPort(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef desiredPortName,
		FabricCore::DFGPortType portType,
		FTL::CStrRef typeSpec,
		FTL::CStrRef portToConnect,
		FTL::StrRef extDep,
		FTL::CStrRef metaData
		) override;

	std::string dfgDoEditPort(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::StrRef oldPortName,
		FTL::StrRef desiredNewPortName,
		FTL::StrRef typeSpec,
		FTL::StrRef extDep,
		FTL::StrRef uiMetadata
		) override;

	void dfgDoRemovePort(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef portName
		) override;

	void dfgDoResizeBackDrop(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef backDropNodeName,
		QPointF newTopLeftPos,
		QSizeF newSize
		) override;

	void dfgDoMoveNodes(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::ArrayRef<FTL::StrRef> nodeNames,
		FTL::ArrayRef<QPointF> newTopLeftPoss
		) override;

	std::string dfgDoImplodeNodes(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::ArrayRef<FTL::StrRef> nodeNames,
		FTL::CStrRef desiredNodeName
		) override;

	std::vector<std::string> dfgDoExplodeNode(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef nodeName
		) override;

	void dfgDoAddBackDrop(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef title,
		QPointF pos
		) override;

	void dfgDoSetNodeTitle(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef nodeName,
		FTL::CStrRef newTitle
		) override;

	void dfgDoSetNodeComment(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef nodeName,
		FTL::CStrRef comment
		) override;

	void dfgDoSetCode(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef code
		) override;

	std::string dfgDoRenamePort(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef oldName,
		FTL::CStrRef desiredNewName
		) override;

	std::vector<std::string> dfgDoPaste(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef json,
		QPointF cursorPos
		) override;

	void dfgDoSetArgType(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef argName,
		FTL::CStrRef typeName
		) override;

	void dfgDoSetArgValue(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef argName,
		FabricCore::RTVal const &value
		) override;

	void dfgDoSetPortDefaultValue(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef portOrPinPath,
		FabricCore::RTVal const &value
		) override;

	void dfgDoSetRefVarPath(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::CStrRef refName,
		FTL::CStrRef varPath
		) override;

	void dfgDoReorderPorts(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		const std::vector<unsigned int> & indices
		) override;

	void dfgDoSetExtDeps(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec,
		FTL::ArrayRef<FTL::StrRef> extDeps
		) override;

	void dfgDoSplitFromPreset(
		FabricCore::DFGBinding const &binding,
		FTL::CStrRef execPath,
		FabricCore::DFGExec const &exec
		) override;
};


