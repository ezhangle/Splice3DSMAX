#pragma once
#include <FabricUI\DFG\DFGController.h>

class MaxDFGController : public FabricUI::DFG::DFGController {

public:

	MaxDFGController(
		FabricUI::GraphView::Graph * graph,
		FabricUI::DFG::DFGWidget* dfgWidget,
		FabricServices::ASTWrapper::KLASTManager * manager,
		FabricUI::DFG::DFGUICmdHandler* cmdHandler,
		bool overTakeBindingNotifications=true);


	virtual bool beginInteraction() override;
	virtual bool endInteraction() override;

	//virtual FabricUI::GraphView::Node * addNodeFromPreset(QString preset, QPointF pos) override;

	//virtual bool moveNode(FabricUI::GraphView::Node * node, QPointF pos, bool isTopLeftPos = false) override;

	//virtual bool renameNode(FabricUI::GraphView::Node * node, QString title) override;

	//virtual FabricUI::GraphView::Pin * addPin(FabricUI::GraphView::Node * node, QString name, FabricUI::GraphView::PortType pType, QColor color, QString dataType = "") override;

	//virtual bool removePin(FabricUI::GraphView::Pin * pin) override;

	//virtual FabricUI::GraphView::Port * addPort(QString name, FabricUI::GraphView::PortType pType, QColor color, QString dataType = "") override;

	//virtual bool removePort(FabricUI::GraphView::Port * port) override;

	//virtual FabricUI::GraphView::Port * addPortFromPin(FabricUI::GraphView::Pin * pin, FabricUI::GraphView::PortType pType) override;

	//virtual bool renamePort(FabricUI::GraphView::Port * port, QString title) override;

	//virtual bool addConnection(FabricUI::GraphView::ConnectionTarget * src, FabricUI::GraphView::ConnectionTarget * dst) override;

	//virtual bool removeConnection(FabricUI::GraphView::ConnectionTarget * src, FabricUI::GraphView::ConnectionTarget * dst) override;

	//virtual bool removeConnection(FabricUI::GraphView::Connection * conn) override;
};