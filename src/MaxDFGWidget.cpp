#include "StdAfx.h"

#include "MaxDFGWidget.h"
#include "plugin.h"
#include <iInstanceMgr.h>


MaxDFGWidget::MaxDFGWidget(QWidget * parent, FabricCore::DFGBinding& binding, FabricUI::DFG::DFGUICmdHandler* cmdHandler)
	: m_binding(binding)
	, m_pOwner(NULL)
	, DFG::DFGCombinedWidget(parent)
{
	FabricCore::Client client = GetClient();

	ASTWrapper::KLASTManager* manager = ASTWrapper::KLASTManager::retainGlobalManager(&client);
	FabricCore::DFGExec exec = binding.getExec();
	init(client, manager, GetHost(), binding, "", exec, cmdHandler, false);

	QObject::connect(this, SIGNAL(portEditDialogCreated(FabricUI::DFG::DFGBaseDialog*)),
		this, SLOT(onPortEditDialogCreated(FabricUI::DFG::DFGBaseDialog*)));
	QObject::connect(this, SIGNAL(portEditDialogInvoked(FabricUI::DFG::DFGBaseDialog*)),
		this, SLOT(onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog*)));

	//qApp->installEventFilter( this );
}

MaxDFGWidget::~MaxDFGWidget()
{
}

//
// Find all INodes that reference this class in any way
class GatherINodes : public DependentEnumProc {
public:
	INodeTab m_gatheredNodes;

	virtual int proc( ReferenceMaker *rm ) override
	{
		INode* pNode = dynamic_cast<INode*>(rm);
		if (pNode != nullptr)
		{
			m_gatheredNodes.AppendNode( pNode );
			return REF_ENUM_SKIP;
		}
		return REF_ENUM_CONTINUE;
	}

};
void MaxDFGWidget::onSelectCanvasNodeInDCC()
{
	if (m_pOwner != nullptr)
	{
		GatherINodes gatherer;
		m_pOwner->DoEnumDependents( &gatherer );
		GetCOREInterface()->SelectNodeTab( gatherer.m_gatheredNodes, TRUE, TRUE );
	}
}

void MaxDFGWidget::onImportGraphInDCC()
{

}

void MaxDFGWidget::onExportGraphInDCC()
{
}

void MaxDFGWidget::onUndo()
{
	// Undo triggered in the Fabric Window menu
	PostMessage( GetCOREInterface()->GetMAXHWnd(), WM_COMMAND, 50034, 0 );
}

void MaxDFGWidget::onRedo()
{
	// Redo triggered in the Fabric Window menu
	PostMessage( GetCOREInterface()->GetMAXHWnd(), WM_COMMAND, 50035, 0 );
}


//////////////////////////////////////////////////////////////////////////

class FillComboBox : public BitArrayCallback {

	QComboBox* m_box;

	int m_maxType;

public:
	FillComboBox(QComboBox* box, int maxType) : m_box(box), m_maxType(maxType) {}

	static QString tr(const char *sourceText, const char * = 0)
	{
		return QString::fromLatin1(sourceText);
	}

	virtual void proc(int n) override
	{
		QString arr = is_tab(n) ? tr(" Array") : "";
		switch ((int)base_type(n))
		{
		case TYPE_FLOAT: 
			m_box->addItem(tr("Float") + arr, QVariant(n));
			break;
		case TYPE_INT:
			m_box->addItem(tr("Int") + arr, QVariant(n));
			break;
		case TYPE_RGBA:
			m_box->addItem(tr("RGB") + arr, QVariant(n));
			break;
		case TYPE_POINT3:
			m_box->addItem(tr("Point3") + arr, QVariant(n));
			break;
		case TYPE_BOOL:
			m_box->addItem(tr("BOOL") + arr, QVariant(n));
			break;
		case TYPE_ANGLE:
			m_box->addItem(tr("Angle") + arr, QVariant(n));
			break;
		case TYPE_PCNT_FRAC:
			m_box->addItem(tr("Percent") + arr, QVariant(n));
			break;
		case TYPE_WORLD:
			m_box->addItem(tr("World") + arr, QVariant(n));
			break;
		case TYPE_STRING:
			m_box->addItem(tr("String") + arr, QVariant(n));
			break;
		case TYPE_FILENAME:
			m_box->addItem(tr("Filename") + arr, QVariant(n));
			break;
		case TYPE_HSV:
			m_box->addItem(tr("HSV") + arr, QVariant(n));
			break;
		case TYPE_COLOR_CHANNEL:
			m_box->addItem(tr("Color") + arr, QVariant(n));
			break;
		case TYPE_TIMEVALUE:
			m_box->addItem(tr("TimeValue") + arr, QVariant(n));
			break;
		case TYPE_RADIOBTN_INDEX:
			m_box->addItem(tr("Radio Idx") + arr, QVariant(n));
			break;
//		case TYPE_MTL:
//		case TYPE_TEXMAP:
//		case TYPE_BITMAP:
		case TYPE_INODE:
			m_box->addItem(tr("MaxNode") + arr, QVariant(n));
			break;
		case TYPE_REFTARG:
			m_box->addItem(tr("MaxClass") + arr, QVariant(n));
			break;
		case TYPE_INDEX:
			m_box->addItem(tr("Index") + arr, QVariant(n));
			break;
		case TYPE_MATRIX3:
			m_box->addItem(tr("Matrix") + arr, QVariant(n));
			break;
//		case TYPE_PBLOCK2:
		case TYPE_POINT4:
			m_box->addItem(tr("Point4") + arr, QVariant(n));
			break;
		case TYPE_FRGBA:
			m_box->addItem(tr("RGBA") + arr, QVariant(n));
			break;
		case TYPE_ENUM:
			m_box->addItem(tr("Enum") + arr, QVariant(n));
			break;

		//case TYPE_VOID:
		//case TYPE_INTERVAL:
		//case TYPE_ANGAXIS:
		//case TYPE_QUAT:
		//case TYPE_RAY:
		//case TYPE_POINT2:
		//case TYPE_BITARRAY:
		//case TYPE_CLASS:
		//case TYPE_MESH:
		//case TYPE_OBJECT:
		//case TYPE_CONTROL:
		//case TYPE_POINT:
		//case TYPE_TSTR:
		//case TYPE_IOBJECT:
		//case TYPE_INTERFACE:
		//case TYPE_HWND:
		//case TYPE_NAME:
		//case TYPE_COLOR:
		//case TYPE_FPVALUE:
		//case TYPE_VALUE:
		//case TYPE_DWORD:
		//case TYPE_bool:
		//case TYPE_INTPTR:
		//case TYPE_INT64:
		//case TYPE_DOUBLE:
		default:
			DbgAssert("Unknown type being translated");
			break;
		}

		// If the item being inserted is the current type, set it as selected
		if (n == m_maxType)
		{
			int currentLen = m_box->count();
			m_box->setCurrentIndex(currentLen - 1);
		}
	}
};

void MaxDFGWidget::onPortEditDialogCreated(DFG::DFGBaseDialog * dialog)
{
	DFG::DFGController * controller = dialog->getDFGController();
	if (!controller->isViewingRootGraph())
		return;

	DFG::DFGEditPortDialog * editPortDialog = (DFG::DFGEditPortDialog *)dialog;
	QString title = editPortDialog->title();

	QComboBox *comboBox = new QComboBox;
	bool isDisabled = true;
	if (title.length() > 0)
	{
		FabricCore::DFGExec exec = m_binding.getExec();
		QByteArray asName = title.toUtf8();
		const char* portName = asName.constData();

		// Do not enable 
		if (GetPortParamID(exec, portName) >= 0)
		{
			const char* fabricType = GetPortSpec(exec, portName);
			BitArray br = FabricTypeToMaxTypes(fabricType);
			if (br.AnyBitSet())
			{
				int maxType = GetPort3dsMaxType(exec, asName.constData());
				FillComboBox filler(comboBox, maxType);
				br.EnumSet(filler);
				isDisabled = false;
			}
		}
	}
	comboBox->setDisabled(isDisabled);
	dialog->addInput(comboBox, "3ds Max Type");
}

void MaxDFGWidget::onPortEditDialogInvoked(FabricUI::DFG::DFGBaseDialog * dialog, FTL::JSONObjectEnc<> * additionalMetaData)
{
	DFG::DFGController * controller = dialog->getDFGController();
	if (!controller->isViewingRootGraph())
		return;

	if (additionalMetaData)
	{
		QComboBox *comboBox = (QComboBox *)dialog->input("3ds Max Type");
		if (comboBox)
		{
			if (!comboBox->isEnabled())
				return;

			int selItem = comboBox->currentIndex();
			if (selItem < 0)
				return;
			int maxType = comboBox->itemData(selItem).toInt();

			DFG::DFGEditPortDialog * editPortDialog = (DFG::DFGEditPortDialog *)dialog;
			QString title = editPortDialog->title();
			if (title.length() > 0)
			{
				// Figure out what kind of parameter
				// we can/will create on the Fabric side
				QString spliceType = editPortDialog->dataType();
				QByteArray asFabricType = spliceType.toAscii();
				BitArray legalTypes = FabricTypeToMaxTypes(asFabricType.constData());
				// The requested max type is not legal for this splice type
				if (!legalTypes[maxType])
					maxType = FabricTypeToDefaultMaxType(asFabricType.constData()); // Reset to default

				CStr cMaxType;
				cMaxType.printf("%i", maxType);
				FTL::JSONEnc<> enc(*additionalMetaData, MAX_PARM_TYPE_OPT);
				FTL::JSONStringEnc<> valueEnc(enc, FTL::StrRef(cMaxType.data()));

				//FTL::JSONSInt32Enc<> typeS32Enc(paramTypeEnc, maxType);
			}
		}
	}
}


bool IsTextEntry( QObject* obj )
{
	if (qobject_cast<QTextEdit*>(obj) != nullptr || qobject_cast<QLineEdit*>(obj) != nullptr || qobject_cast<QPlainTextEdit*>(obj) != nullptr)
		return true;

	const char* name = obj->metaObject()->className();
	if (strcmp( name, "FabricUI::DFG::DFGTabSearchWidget" ) == 0)
		return true;
	//if (strcmp( name, "FabricUI::KLEditor::KLSourceCodeWidget" ) == 0)
	//	return true;
	return false;
}

bool MaxDFGWidget::eventFilter( QObject *obj, QEvent *event )
{

	static bool kbd_pressed;
	switch (event->type())
	{
		//case QEvent::MouseMove:
		//{
		//	//const char* name = obj->metaObject()->className();
		//	//logMessage( name );
		//	QSizeGrip *sg = qobject_cast<QSizeGrip*>(obj);
		//	if (sg)
		//	{
		//		//			qDebug() << sg->parentWidget();

		//		QWidget* qw = sg->parentWidget();
		//		QString name = qw->objectName();
		//		ToMstr( name );
		//	}
		//	break;
		//}

		case QEvent::KeyPress:
		{
			QKeyEvent* qKeyEvent = static_cast<QKeyEvent*>(event);
			if (!kbd_pressed && qKeyEvent->matches( QKeySequence::Undo ))
				//if (qKeyEvent->modifiers() & Qt::CTRL && qKeyEvent->key() == Qt::Key_Z)
			{
				kbd_pressed = true; // We are receiving way too many auto-repeats!
				// Send the undo to Max
				//PostMessage( GetCOREInterface()->GetMAXHWnd(), WM_COMMAND, 50034, 0 );
				logMessage( "KeyPressed\n" );
				return false;
			}

			if (qKeyEvent->matches( QKeySequence::SelectAll ))
			{
				return false;
			}
		}
		case QEvent::KeyRelease:
		{
			QKeyEvent* qKeyEvent = static_cast<QKeyEvent*>(event);
			QString msg = "Released: ";
			msg += qKeyEvent->text();
			msg += "\n";
			logMessage( ToCStr(ToMstr(msg)) );
			kbd_pressed = false;
			break;
		}

		//case QEvent::FocusIn:
		//{
		//	//const char* name = obj->metaObject()->className();
		//	//logMessage( CStr(name) + " focused\n");
		//	if (IsTextEntry(obj))
		//	{
		//		// 
		//		DisableAccelerators();
		//	}
		//	break;
		//}
		//case QEvent::FocusOut:
		//{
		//	//The unfocus logic is not identical to the focus
		//	// to account for a strange notification issue
		//	// when working with DFGRegisteredTypeLineEdit.
		//	// The issue stems from the drop-down list displayed
		//	// as the user types in the box.  Technically, the
		//	// line-edit loses focus when this new UI is displayed,
		//	// which triggers our enableaccelerators call, however,
		//	// the user is still actually typing in the edit box
		//	// We can't catch the Focus on the new element (not sure
		//	// why)
		//	// However, experiments have shown that every element that we
		//	// focus off on also includes a FocusOut for a style sheet-
		//	// except this drop-down list focus.  So, if we enable on every
		//	// focus out, then we call the fn way too many times, but
		//	// will actually not trigger for this one edge case
		//	const char* name = obj->metaObject()->className();
		//	if (strcmp( name, "QStyleSheetStyle" ) == 0)
		//	{
		//		EnableAccelerators();
		//	}
		//	else if (strcmp( name, "FabricUI::DFG::DFGRegisteredTypeLineEdit" ) == 0)
		//	{
		//		/* Do nothing */
		//	}
		//	else if (IsTextEntry( obj ))
		//	{
		//		EnableAccelerators();
		//	}
		//	//logMessage( CStr( name ) + " unfocused\n" );
		//}
	}
	return QWidget::eventFilter(obj, event);
}

void MaxDFGWidget::keyPressEvent( QKeyEvent * event )
{
	if (event->matches( QKeySequence::Undo ))
	{
		onUndo();
		event->accept();
	}
	else if (event->matches( QKeySequence::Redo ))
	{
		onRedo();
		event->accept();
	}
	else
	{
		return __super::keyPressEvent( event );
	}
}
