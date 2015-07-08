#include "StdAfx.h"

#include "DFGWidget.h"
#include "plugin.h"
#include "MaxCommandStack.h"


//std::string DFGWidget::s_currentUINodeName;
//std::map<DFGWidget*, FabricDFGBaseInterface*> DFGWidget::s_widgets;

DFGWidget::DFGWidget(QWidget * parent, FabricCore::DFGBinding& binding)
	: DFG::DFGCombinedWidget(parent)
{
	FabricCore::Client client = GetClient();

	ASTWrapper::KLASTManager* manager = ASTWrapper::KLASTManager::retainGlobalManager(&client);
	FabricCore::DFGExec exec = binding.getExec();
	init(client, manager, GetHost(), binding, exec, GetCommandStack(), false);
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