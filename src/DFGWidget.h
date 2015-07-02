//
//  The DFG widget allows for editing things on the widget.  Woot.
#pragma once


#include <DFG/DFGCombinedWidget.h>

using namespace FabricServices;
using namespace FabricUI;


class DFGWidget : public DFG::DFGCombinedWidget {
  
public:
	
	DFGWidget(QWidget * parent, FabricCore::DFGBinding& binding);
	~DFGWidget();

  //static QWidget * creator(QWidget * parent, const QString & name);
  
  //static void setCurrentUINodeName(const char * node);
  //static void mayaLog(const char * message);
  //static void closeWidgetsForBaseInterface(FabricDFGBaseInterface * interf);

	public slots:
	virtual void onValueChanged() override;
	virtual void onStructureChanged() override;
	virtual void onRecompilation() override;

private:

  //FabricServices::DFGWrapper::Host* m_host;
};
