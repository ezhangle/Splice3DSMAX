//
//  The DFG widget allows for editing things on the widget.  Woot.
#pragma once


#include <DFG/DFGCombinedWidget.h>

using namespace FabricServices;
using namespace FabricUI;


class MaxDFGWidget : public DFG::DFGCombinedWidget {
  
	Q_OBJECT

public:
	
	MaxDFGWidget(QWidget * parent, FabricCore::DFGBinding& binding);
	~MaxDFGWidget();

  //static QWidget * creator(QWidget * parent, const QString & name);
  
  //static void setCurrentUINodeName(const char * node);
  //static void mayaLog(const char * message);
  //static void closeWidgetsForBaseInterface(FabricDFGBaseInterface * interf);

	public slots:
	virtual void onValueChanged() override;
	virtual void onStructureChanged() override;
	virtual void onRecompilation() override;

	private slots:

	void whenPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog) override;
	void whenPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog) override;

	//void editDialogCreated(DFG::DFGBaseDialog * dialog);
	//void editDialogInvoked(DFG::DFGBaseDialog * dialog);
};
