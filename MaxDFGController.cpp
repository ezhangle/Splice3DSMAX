#include "StdAfx.h"
#include "MaxDFGController.h"
#include "hold.h"

using namespace FabricUI;
using namespace GraphView;

MaxDFGController::MaxDFGController(GraphView::Graph * graph, FabricServices::Commands::CommandStack * stack, FabricCore::Client * client, FabricServices::DFGWrapper::Host * host, FabricServices::ASTWrapper::KLASTManager * manager, bool overTakeBindingNotifications /*= true*/)
	: DFGController(graph, stack, client, host, manager, overTakeBindingNotifications)
{

}

bool MaxDFGController::beginInteraction()
{
	theHold.Begin();
	return __super::beginInteraction();
}

bool MaxDFGController::endInteraction()
{
	bool res = __super::endInteraction();
	theHold.Accept(_M("Compound Action"));
	return res;
}


Node * MaxDFGController::addNodeFromPreset(QString preset, QPointF pos)
{
	HoldActions hold(_M("addNodeFromPreset"));
	return Controller::addNodeFromPreset(preset, pos);
}

bool MaxDFGController::moveNode(Node * node, QPointF pos, bool isTopLeftPos /*= false*/)
{
	HoldActions hold(_M("moveNode"));
	return __super::moveNode(node, pos, isTopLeftPos);
}

bool MaxDFGController::renameNode(Node * node, QString title)
{
	HoldActions hold(_M("renameNode"));
	return __super::renameNode(node, title);
}

Pin * MaxDFGController::addPin(Node * node, QString name, PortType pType, QColor color, QString dataType /*= ""*/)
{
	HoldActions hold(_M("addPin"));
	return __super::addPin(node, name, pType, color, dataType);
}

bool MaxDFGController::removePin(Pin * pin)
{
	HoldActions hold(_M("removePin"));
	return __super::removePin(pin);
}

Port * MaxDFGController::addPort(QString name, PortType pType, QColor color, QString dataType /*= ""*/)
{
	HoldActions hold(_M("addPort"));
	return Controller::addPort(name, pType, color, dataType);
}

bool MaxDFGController::removePort(Port * port)
{
	HoldActions hold(_M("removePort"));
	return Controller::removePort(port);
}

Port * MaxDFGController::addPortFromPin(Pin * pin, PortType pType)
{
	HoldActions hold(_M("addPortFromPin"));
	return __super::addPortFromPin(pin, pType);
}

bool MaxDFGController::renamePort(Port * port, QString title)
{
	HoldActions hold(_M("renamePort"));
	return Controller::renamePort(port, title);
}

bool MaxDFGController::addConnection(ConnectionTarget * src, ConnectionTarget * dst)
{
	HoldActions hold(_M("addConnection"));
	return __super::addConnection(src, dst);
}

bool MaxDFGController::removeConnection(ConnectionTarget * src, ConnectionTarget * dst)
{
	HoldActions hold(_M("removeConnection"));
	return __super::removeConnection(src, dst);
}

bool MaxDFGController::removeConnection(Connection * conn)
{
	HoldActions hold(_M("removeConnection"));
	return Controller::removeConnection(conn);
}

