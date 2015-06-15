#include "StdAfx.h"

#include "DFGWidget.h"
#include "plugin.h"
#include "..\MaxCommandStack.h"


//std::string DFGWidget::s_currentUINodeName;
//std::map<DFGWidget*, FabricDFGBaseInterface*> DFGWidget::s_widgets;

DFGWidget::DFGWidget(QWidget * parent, FabricServices::DFGWrapper::Binding& binding)
	: DFG::DFGCombinedWidget(parent)
{
	FabricCore::Client client = GetClient();

	ASTWrapper::KLASTManager* manager = ASTWrapper::KLASTManager::retainGlobalManager(&client);
	DFGWrapper::GraphExecutablePtr graph = DFGWrapper::GraphExecutablePtr::StaticCast(binding.getExecutable());
	init(&client, manager, GetHost(), binding, graph, GetCommandStack(), false);
}

DFGWidget::~DFGWidget()
{
}


void DFGWidget::onValueChanged()
{
//	if (theHold.Holding())

}

void DFGWidget::onStructureChanged()
{
	//if (theHold.Holding())
	//	theHold.Accept(_M("Splice Structure Changed"));
}

void DFGWidget::onRecompilation()
{
	//FabricDFGBaseInterface * interf = FabricDFGBaseInterface::getInstanceByName(m_baseInterfaceName.c_str());
	//if(interf)
	//{
	//  interf->invalidateNode();
	//}
}