//
//  The DFG widget allows for editing things on the widget.  Woot.
#pragma once


#include <FabricUI/DFG/DFGCombinedWidget.h>
#include <FabricUI/DFG/DFGUICmdHandler_QUndo.h>

using namespace FabricServices;
using namespace FabricUI;


class MaxDFGWidget : public DFG::DFGCombinedWidget {
  
	Q_OBJECT

	FabricCore::DFGBinding& m_binding;
public:
	
	MaxDFGWidget(QWidget * parent, FabricCore::DFGBinding& binding, FabricUI::DFG::DFGUICmdHandler* cmdHandler);
	~MaxDFGWidget();

public slots:
	virtual void onValueChanged() override;
	virtual void onStructureChanged() override;
	virtual void onUndo() override;
	virtual void onRedo() override;

private slots:
	void onPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog * dialog);
	void onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog, FTL::JSONObjectEnc<> * additionalMetaData);
};
