//
//  The DFG widget allows for editing things on the widget.  Woot.
#pragma once


#include <DFG/DFGCombinedWidget.h>

using namespace FabricServices;
using namespace FabricUI;


class DFGWidget : public DFG::DFGCombinedWidget {
  
public:
	DFGWidget(QWidget * parent, FabricCore::Client& client, FabricServices::DFGWrapper::Binding& binding, FabricServices::DFGWrapper::Host* host);
	~DFGWidget();

  //static QWidget * creator(QWidget * parent, const QString & name);
  
  //static void setCurrentUINodeName(const char * node);
  //static void mayaLog(const char * message);
  //static void closeWidgetsForBaseInterface(FabricDFGBaseInterface * interf);

public slots:
  virtual void onRecompilation();
  virtual void onPortRenamed(QString path, QString newName);

private:

  FabricServices::DFGWrapper::Host* m_host;
};
