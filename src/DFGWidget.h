//
//  The DFG widget allows for editing things on the widget.  Woot.
#pragma once


#include <DFG/DFGCombinedWidget.h>

using namespace FabricServices;
using namespace FabricUI;

class FabricDFGBaseInterface;

class FabricDFGWidget : public DFG::DFGCombinedWidget {

  Q_OBJECT
  
public:
  FabricDFGWidget(QWidget * parent);
  ~FabricDFGWidget();

  static QWidget * creator(QWidget * parent, const QString & name);
  
  //static void setCurrentUINodeName(const char * node);
  //static void mayaLog(const char * message);
  //static void closeWidgetsForBaseInterface(FabricDFGBaseInterface * interf);

public slots:
  virtual void onRecompilation();
  virtual void onPortRenamed(QString path, QString newName);

private:

  FabricCore::Client m_mayaClient;
  std::string m_baseInterfaceName;
  static std::string s_currentUINodeName;
  static std::map<FabricDFGWidget*, FabricDFGBaseInterface*> s_widgets;
};

#endif 
