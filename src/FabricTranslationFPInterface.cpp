#include "StdAfx.h"
#include "FabricTranslationFPInterface.h"

#include <MaxScript/MaxScript.h>
#include <maxscript\foundation\functions.h>
#include <maxscript\maxwrapper\mxsobjects.h>
#include "MaxDFGWidget.h"
#include "DockableWidget.h"

#include <qwinwidget.h>
#include "Fabric3dsmax.h"

FabricTranslationFPInterface::FabricTranslationFPInterface()
	: m_klEditor(nullptr)
	, m_pDFGWidgetWindow(nullptr)
	, m_fabricCmdHandler(this)
{

}

FabricTranslationFPInterface::~FabricTranslationFPInterface()
{
	CloseDFGGraphEditor();
}


BOOL FabricTranslationFPInterface::ShowDFGGraphEditor()
{
	CloseDFGGraphEditor();

	m_pDFGWidgetWindow = DockableWindow::Create(_T("Fabric Engine - Canvas"), this);
	HWND h = m_pDFGWidgetWindow->GetHWND();
	QWinWidget* dlg = new QWinWidget(h);
	MaxDFGWidget* pWidget = new MaxDFGWidget(dlg, GetBinding(), &m_fabricCmdHandler);

	pWidget->SetCallbackItem( CastToRefTarg() );

	m_pDFGWidgetWindow->SetWidget(dlg);
	return TRUE;
}

void FabricTranslationFPInterface::CloseDFGGraphEditor()
{
	SAFE_DELETE(m_pDFGWidgetWindow);
}

//////////////////////////////////////////////////////////////////////////
#pragma region Fabric commands exposed to MaxScript

#pragma region a few conv functions

QPointF Convert(const Point2& v) { return QPointF(v.x, v.y); }

void Convert(const Tab<MSTR*>& from, QStringList& to) {
	to.reserve(from.Count());
	for (int i = 0; i < from.Count(); i++) {
		to.push_back(ToQStr(*from[i]));
	}
}

void Convert(const Tab<Point2*>& from, QList<QPointF>& to) {
	to.reserve(from.Count());
	for (int i = 0; i < from.Count(); i++) {
		to.push_back(Convert(*from[i]));
	}
}

void Convert( const Tab<int>& from, QList<int>& to )
{
	to.reserve( from.Count() );
	for (int i = 0; i < from.Count(); ++i)
	{
		to.push_back(from[i]);
	}
}

Tab<TSTR*> Convert(const QStringList& from)
{
	Tab<TSTR*> to;
	to.SetCount(from.size());
	for (int i = 0; i < from.size(); i++) {
		to[i] = new MSTR(ToMstr(from[i]));
	}
	return to;
}

#pragma endregion

// The following are direct mappers to the commands defined by the DFGCmdHandler classed.
void FabricTranslationFPInterface::DFGRemoveNodes(const Tab<TSTR*>& nodeNames, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN;
	QStringList cNodeNames;
		Convert(nodeNames, cNodeNames);
		m_fabricCmdHandler.dfgDoRemoveNodes(m_binding, ToQStr(execPath), GetExec(execPath), cNodeNames);
	MAXSPLICE_CATCH_END;
}

void FabricTranslationFPInterface::DFGConnect(const MSTR& srcPath, const MSTR& destPath, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	InvalidateAll();
	return m_fabricCmdHandler.dfgDoConnect(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(srcPath), ToQStr(destPath));
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGDisconnect( const Tab<TSTR*>& srcPaths, const Tab<TSTR*>& destPaths, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	InvalidateAll();
	QStringList cSrcPaths;
	Convert( srcPaths, cSrcPaths );
	QStringList cDestPaths;
	Convert( destPaths, cDestPaths );
	return m_fabricCmdHandler.dfgDoDisconnect(m_binding, ToQStr(execPath), GetExec(execPath), cSrcPaths, cDestPaths );
	MAXSPLICE_CATCH_END
}
MSTR FabricTranslationFPInterface::DFGAddGraph(const MSTR& title, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return ToMstr(m_fabricCmdHandler.dfgDoAddGraph(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(title), Convert(pos)));
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGAddFunc(const MSTR& title, const MSTR& initialCode, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return ToMstr(m_fabricCmdHandler.dfgDoAddFunc(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(title), ToQStr(initialCode), Convert(pos)));
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGInstPreset(const MSTR& presetPath, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return ToMstr(m_fabricCmdHandler.dfgDoInstPreset(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(presetPath), Convert(pos)));
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGAddVar(const MSTR& desiredNodeName, const MSTR& dataType, const MSTR& extDep, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return ToMstr(m_fabricCmdHandler.dfgDoAddVar(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(desiredNodeName), ToQStr(dataType), ToQStr(extDep), Convert(pos)));
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGAddGet(const MSTR& desiredNodeName, const MSTR& varPath, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return ToMstr(m_fabricCmdHandler.dfgDoAddGet(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(desiredNodeName), ToQStr(varPath), Convert(pos)));
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGAddSet(const MSTR& desiredNodeName, const MSTR& varPath, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return ToMstr(m_fabricCmdHandler.dfgDoAddSet(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(desiredNodeName), ToQStr(varPath), Convert(pos)));
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGAddPort(const MSTR& desiredPortName, int portType, const MSTR& portSpec, const MSTR& portToConnect, const MSTR& extDep, const MSTR& metaData, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	FabricCore::DFGPortType fType = static_cast<FabricCore::DFGPortType>(portType);
	CStr cExecPath = execPath.ToCStr();
	return ToMstr(m_fabricCmdHandler.dfgDoAddPort(m_binding, cExecPath.data(), GetExec(cExecPath.data()), ToQStr(desiredPortName), fType, ToQStr(portSpec), ToQStr(portToConnect), ToQStr(extDep), ToQStr(metaData)));
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGEditPort(const MSTR& portName, int portType, const MSTR& desiredNewPortName, const MSTR& typeSpec, const MSTR& extDep, const MSTR& metaData, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN

	FabricCore::DFGExec exec = GetExec( execPath );
	QString oldName = ToQStr( portName );
	FabricCore::DFGPortType type = (FabricCore::DFGPortType)portType;

	QString res = m_fabricCmdHandler.dfgDoEditPort(m_binding, ToQStr(execPath), exec, oldName, ToQStr(desiredNewPortName), type, ToQStr(typeSpec), ToQStr(extDep), ToQStr(metaData));
	return  ToMstr(res);
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
void FabricTranslationFPInterface::DFGRemovePort(const MSTR& portName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	InvalidateAll();
	return m_fabricCmdHandler.dfgDoRemovePort(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(portName));
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGResizeBackdrop(const MSTR& backDropNodeName, Point2 topLeft, Point2 size, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QSizeF qSize(size.x, size.y);
	return m_fabricCmdHandler.dfgDoResizeBackDrop(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(backDropNodeName), Convert(topLeft), qSize);
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGMoveNodes(Tab<TSTR*> nodeNames, Tab<Point2*> topLeftPoss, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QStringList cNodeNames;
	Convert(nodeNames, cNodeNames);
	QList<QPointF> qTopLeftPoss;
	Convert(topLeftPoss, qTopLeftPoss);
	return m_fabricCmdHandler.dfgDoMoveNodes(m_binding, ToQStr(execPath), GetExec(execPath), cNodeNames, qTopLeftPoss);
	MAXSPLICE_CATCH_END
}
MSTR FabricTranslationFPInterface::DFGImplodeNodes(Tab<TSTR*> nodeNames, const MSTR& desiredNewNodeName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QStringList cNodeNames;
	Convert(nodeNames, cNodeNames);
	return ToMstr(m_fabricCmdHandler.dfgDoImplodeNodes(m_binding, ToQStr(execPath), GetExec(execPath), cNodeNames, ToQStr(desiredNewNodeName)));
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
Tab<TSTR*> FabricTranslationFPInterface::DFGExplodeNodes(const MSTR& nodeName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QStringList res = m_fabricCmdHandler.dfgDoExplodeNode(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(nodeName));
	return Convert(res);
	MAXSPLICE_CATCH_RETURN(Tab<TSTR*>());
}
void FabricTranslationFPInterface::DFGAddBackdrop(const MSTR& title, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoAddBackDrop(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(title), Convert(pos));
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGSetNodeComment(const MSTR& nodeName, const MSTR& comment, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSetNodeComment(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(nodeName), ToQStr(comment));
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGSetCode(const MSTR& code, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSetCode(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(code));
	MAXSPLICE_CATCH_END
}
MSTR FabricTranslationFPInterface::DFGRenamePort(const MSTR& oldName, const MSTR& newDesiredName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	int id = GetPortParamID(ToCStr(oldName));
	QString res = m_fabricCmdHandler.dfgDoRenamePort(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(oldName), ToQStr(newDesiredName));
	if (id >= 0)
		SetMaxParamName(GetPBlock()->GetDesc(), (ParamID)id, newDesiredName);
	return ToMstr(res);
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
Tab<TSTR*> FabricTranslationFPInterface::DFGPaste(const MSTR& json, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QStringList res = m_fabricCmdHandler.dfgDoPaste(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(json), Convert(pos));
	return Convert(res);
	MAXSPLICE_CATCH_RETURN(Tab<TSTR*>())
}
void FabricTranslationFPInterface::DFGSetArgValue(const MSTR& argName, const FPValue* argValue)
{
	MAXSPLICE_CATCH_BEGIN
	CStr cArgName = argName.ToCStr();
	FabricCore::RTVal rtVal = m_binding.getArgValue(cArgName.data());
	if (rtVal.isValid())
	{
		ConvertToRTVal(*argValue, rtVal);
		m_fabricCmdHandler.dfgDoSetArgValue(m_binding, cArgName.data(), rtVal);
		InvalidateAll();
	}
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGSetPortDefaultValue(const MSTR& portName, const FPValue* fpvalue, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	CStr cPortName = portName.ToCStr();
	FabricCore::DFGExec exec = GetExec(execPath);
	const char* portType = exec.getNodePortResolvedType(cPortName);
	if (portType == nullptr || strlen(portType) == 0)
		throw TypeError(_M("Cannot set default type for unresolved port"), nullptr);

	FabricCore::RTVal defVal = FabricCore::RTVal::Construct(GetClient(), portType, 0, nullptr);
	if (defVal.isObject())
		defVal = FabricCore::RTVal::Create(GetClient(), portType, 0, nullptr);

	if (defVal.isValid())
	{
		ConvertToRTVal(*fpvalue, defVal);
		m_fabricCmdHandler.dfgDoSetPortDefaultValue(m_binding, ToQStr(execPath), exec, cPortName.data(), defVal);
	}
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGSetRefVarPath(const MSTR& refName, const MSTR& varPath, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSetRefVarPath(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(refName), ToQStr(varPath));
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGReorderPorts( const MSTR& itemPath, Tab<int> indices, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QList<int> qIndices;
	Convert( indices, qIndices );
	return m_fabricCmdHandler.dfgDoReorderPorts(m_binding, ToQStr(execPath), GetExec(execPath), ToQStr(itemPath), qIndices );
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGSetExtDeps(Tab<TSTR*> extDeps, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QStringList cExtDeps;
	Convert(extDeps, cExtDeps);
	return m_fabricCmdHandler.dfgDoSetExtDeps(m_binding, ToQStr(execPath), GetExec(execPath), cExtDeps);
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGSplitFromPreset(const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSplitFromPreset(m_binding, ToQStr(execPath), GetExec(execPath));
	MAXSPLICE_CATCH_END
}

MSTR FabricTranslationFPInterface::DFGDoEditNode( const MSTR& oldNodeName, const MSTR& desiredNewNodeName, const MSTR& nodeMetadata, const MSTR& execMetadata, const MSTR& execPath )
{
	MAXSPLICE_CATCH_BEGIN
	QString res =m_fabricCmdHandler.dfgDoEditNode( m_binding, ToQStr( execPath), GetExec( execPath ), ToQStr( oldNodeName), ToQStr( desiredNewNodeName), ToQStr( nodeMetadata), ToQStr( execMetadata ));
	return ToMstr(res.toStdString().data());
	MAXSPLICE_CATCH_RETURN( _M( "Exception occured" ) )
}

void FabricTranslationFPInterface::DFGDoDismissLoadDiags( Tab<int>& indices )
{
	MAXSPLICE_CATCH_BEGIN
	QList<int> params;
	params.reserve( indices.Count() );
	for (int i = 0; i < indices.Count(); i++)
		params.append( indices[i] );
	m_fabricCmdHandler.dfgDoDismissLoadDiags( m_binding, params );
	MAXSPLICE_CATCH_END
}

MSTR FabricTranslationFPInterface::DFGDoCreatePreset( const MSTR& nodeName, const MSTR& presetDirPath, const MSTR& presetName, const MSTR& execPath )
{
	MAXSPLICE_CATCH_BEGIN
	QString res = m_fabricCmdHandler.dfgDoCreatePreset( m_binding, ToQStr(execPath), GetExec( execPath ), ToQStr( nodeName), ToQStr( presetDirPath), ToQStr( presetName ));
	return ToMstr(res);
	MAXSPLICE_CATCH_RETURN( _M( "Exception occured" ) )
}

MSTR FabricTranslationFPInterface::DFGDoAddInstPort( const MSTR& instName, const MSTR& desiredPortName, int portType, const MSTR& typeSpec, const MSTR& pathToConnect, int connectType, const MSTR& extDep, const MSTR& metaData, const MSTR& execPath )
{
	MAXSPLICE_CATCH_BEGIN
	FabricCore::DFGPortType pt = (FabricCore::DFGPortType)portType;
	FabricCore::DFGPortType ct = (FabricCore::DFGPortType)connectType;
	QString res = m_fabricCmdHandler.dfgDoAddInstPort( m_binding, ToQStr( execPath ), GetExec( execPath ), ToQStr( instName ), ToQStr( desiredPortName ), pt, ToQStr( typeSpec ), ToQStr( pathToConnect ), ct, ToQStr( extDep ), ToQStr( metaData ) );
	return ToMstr( res );
	MAXSPLICE_CATCH_RETURN( _M( "Exception occured" ) )
}

MSTR FabricTranslationFPInterface::DFGDoAddInstBlockPort( const MSTR& instName, const MSTR& blockName, const MSTR& desiredPortName, const MSTR& typeSpec, const MSTR& pathToConnect, const MSTR& extDep, const MSTR& metaData, const MSTR& execPath )
{
	MAXSPLICE_CATCH_BEGIN
	QString res = m_fabricCmdHandler.dfgDoAddInstBlockPort( m_binding, ToQStr( execPath ), GetExec( execPath ), ToQStr( instName ), ToQStr( blockName ), ToQStr( desiredPortName ), ToQStr( typeSpec ), ToQStr( pathToConnect ), ToQStr( extDep ), ToQStr( metaData ) );
	return ToMstr( res );
	MAXSPLICE_CATCH_RETURN( _M( "Exception occured" ) )
}

MSTR FabricTranslationFPInterface::DFGDoAddBlock( const MSTR& desiredName, Point2 pos, const MSTR& execPath )
{
	MAXSPLICE_CATCH_BEGIN
		QPointF qp = Convert( pos );
		QString res = m_fabricCmdHandler.dfgDoAddBlock( m_binding, ToQStr( execPath ), GetExec( execPath ), ToQStr( desiredName ), qp);
		return ToMstr( res );
	MAXSPLICE_CATCH_RETURN( _M( "Exception occured" ) )
}

MSTR FabricTranslationFPInterface::DFGDoAddBlockPort( const MSTR& blockName, const MSTR& desiredPortName, int portType, const MSTR& typeSpec, const MSTR& pathToConnect, int connectType, const MSTR& extDep, const MSTR& metaData, const MSTR& execPath )
{
	MAXSPLICE_CATCH_BEGIN
		FabricCore::DFGPortType pt = (FabricCore::DFGPortType)portType;
		FabricCore::DFGPortType ct = (FabricCore::DFGPortType)connectType;
		QString res = m_fabricCmdHandler.dfgDoAddBlockPort( m_binding, ToQStr( execPath ), GetExec( execPath ), ToQStr( blockName ), ToQStr( desiredPortName ), pt, ToQStr( typeSpec ), ToQStr( pathToConnect ), ct, ToQStr( extDep ), ToQStr( metaData ) );
		return ToMstr( res );
	MAXSPLICE_CATCH_RETURN( _M( "Exception occured" ) )
}

// Allow introspecting the ports on this graph
int FabricTranslationFPInterface::GetPortCount(const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return GetExec(execPath).getExecPortCount();
	MAXSPLICE_CATCH_RETURN(-1);
}

MSTR FabricTranslationFPInterface::GetPortName(int i, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return ToMstr(GetExec(execPath).getExecPortName(i));
	MAXSPLICE_CATCH_RETURN(_M(" ** Exception Occured"));
}

FabricCore::DFGPortType FabricTranslationFPInterface::GetPortType( const MSTR& portName, const MSTR& execPath )
{
	MAXSPLICE_CATCH_BEGIN
		return GetExec( execPath ).getExecPortType( ToCStr( portName ) );
	MAXSPLICE_CATCH_RETURN( FabricCore::DFGPortType_In );
}

const char* FabricTranslationFPInterface::GetPortSpec(const char* portName, const char* execPath)
{

	return ::GetPortSpec(GetExec(execPath), portName);
}

MSTR FabricTranslationFPInterface::GetPortSpec(const MSTR& portName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return ToMstr(GetPortSpec(ToCStr(portName), ToCStr(execPath)));
	MAXSPLICE_CATCH_RETURN(_M(" ** Exception Occured"));
}

bool FabricTranslationFPInterface::HasSrcPort(const char* portName)
{
	return GetExec(nullptr).hasSrcPorts(portName);
}

bool FabricTranslationFPInterface::HasSrcPort(const MSTR& portName)
{
	MAXSPLICE_CATCH_BEGIN
	return HasSrcPort(ToCStr(portName));
	MAXSPLICE_CATCH_RETURN(_M(" ** Exception Occured"));
}

bool FabricTranslationFPInterface::HasDstPort(const char* portName)
{
	return GetExec(nullptr).hasDstPorts(portName);
}

bool FabricTranslationFPInterface::HasDstPort(const MSTR& portName)
{
	MAXSPLICE_CATCH_BEGIN
		return HasDstPort(ToCStr(portName));
	MAXSPLICE_CATCH_RETURN(_M(" ** Exception Occured"));
}

// Introspect nodes as well
int FabricTranslationFPInterface::GetNodeCount(const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return GetExec(execPath).getNodeCount();
	MAXSPLICE_CATCH_RETURN(0);
}

MSTR FabricTranslationFPInterface::GetNodeName(int i, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return ToMstr(GetExec(execPath).getNodeName(i));
	MAXSPLICE_CATCH_RETURN(_M(" ** Exception Occured"));
}

int FabricTranslationFPInterface::GetNodeType(const MSTR& NodeName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	FabricCore::DFGNodeType nodeType = GetExec(execPath).getNodeType(ToCStr(NodeName));
	return nodeType;
	MAXSPLICE_CATCH_RETURN(0);
}


bool FabricTranslationFPInterface::GetArgValue(const char* argName, FPValue& value/*, const MSTR& execPath*/)
{
	MAXSPLICE_CATCH_BEGIN

	FabricCore::RTVal rtVal = m_binding.getArgValue(argName);
	const char* cType = GetPortSpec(argName);
	int type = FabricTypeToMaxType(cType);

	switch (type)
	{
	case TYPE_INT:
	{
		int i = -1;
		FabricToMaxValue(rtVal, i);
		value.Load(TYPE_INT, i);
	}
	break;
	case TYPE_BOOL:
	{
		bool b = false;
		FabricToMaxValue(rtVal, b);
		value.Load(TYPE_BOOL, b);
		break;
	}
	case TYPE_FLOAT:
	{
		float f = -1.0f;
		FabricToMaxValue(rtVal, f);
		value.Load(TYPE_FLOAT, f);
		break;
	}
	case TYPE_FRGBA:
	{
		Point4 p4(-1,-1,-1,-1);
		FabricToMaxValue(rtVal, p4);
		value.Load(TYPE_FRGBA, p4);
		break;
	}
	case TYPE_POINT2:
	{
		Point2 p2(-1, -1);
		FabricToMaxValue(rtVal, p2);
		value.Load(TYPE_POINT2, &p2);
		break;
	}
	case TYPE_POINT3:
	{
		Point3 v(-1, -1, -1);
		FabricToMaxValue(rtVal, v);
		value.Load(TYPE_POINT3, v);
		break;
	}
	case TYPE_POINT4:
	{
		Point4 p4(-1, -1, -1, -1);
		FabricToMaxValue(rtVal, p4);
		value.Load(TYPE_POINT4, p4);
		break;
	}
	case TYPE_MATRIX3:
	{
		Matrix3 m(1);
		FabricToMaxValue(rtVal, m);
		value.Load(TYPE_MATRIX3, m);
		break;
	}
	case TYPE_QUAT:
	{
		Quat q(-1.0f, -1.0f, -1.0f, -1.0f);
		FabricToMaxValue(rtVal, q);
		value.Load(TYPE_QUAT, q);
		break;
	}
	case TYPE_STRING:
	{
		TSTR tstr;
		FabricToMaxValue(rtVal, tstr);
		value.Load(TYPE_TSTR, tstr);
		break;
	}
	case TYPE_MESH:
	{
		Mesh mesh;
		FabricToMaxValue(rtVal, mesh);
		value.Load(TYPE_MESH, mesh);
		break;
	}
	default:
	{
		TSTR tstr;
		if ((rtVal.isObject() || rtVal.isInterface()) && rtVal.isNullObject())
			tstr = _T( "null" );
		else
		{
			// For unconverted values we send back the json representation
			FabricCore::RTVal jsonValue = rtVal.getJSON();
			FabricToMaxValue( jsonValue, tstr );
		}
		value.Load( TYPE_STRING, tstr );
	}
	}
	return true;
#pragma message("TEST THIS")
	MAXSPLICE_CATCH_END
		return false;
}

const FPValue& FabricTranslationFPInterface::GetArgValue(const MSTR& portName, const MSTR& execPath)
{
	// We make the val static so we don't leak pointers assigned to it.
	static FPValue val;
	MAXSPLICE_CATCH_BEGIN
	// Free whatever memory was assigned last time
	val.Free();
	GetArgValue(ToCStr(portName), val);
	return val;
	MAXSPLICE_CATCH_RETURN(val);
}

int FabricTranslationFPInterface::GetMaxTypeForArg(const char* argName)
{
	int pid = GetPortParamID(argName);
	if (pid >= 0)
	{
		IParamBlock2* pblock = GetPBlock();
		if (pblock != nullptr)
			return pblock->GetParameterType((ParamID)pid);
	}
	return -2;
}

int FabricTranslationFPInterface::GetMaxTypeForArg(const MSTR& argName)
{
	MAXSPLICE_CATCH_BEGIN
	return GetMaxTypeForArg(ToCStr(argName));
	MAXSPLICE_CATCH_RETURN(-1);
}

int FabricTranslationFPInterface::SetMaxTypeForArg(const char* argName, int type)
{
	char buff[20];
	_itoa_s(type, buff, 10);
	SetPortMetaData(argName, MAX_PARM_TYPE_OPT, buff);
	return SyncMetaDataFromPortToParam(argName);
}

int FabricTranslationFPInterface::SetMaxTypeForArg(const MSTR& argName, int type)
{
	MAXSPLICE_CATCH_BEGIN
	return SetMaxTypeForArg(ToCStr(argName), type);
	MAXSPLICE_CATCH_RETURN(-1);
}

BitArray FabricTranslationFPInterface::GetLegalMaxTypesForArg(const MSTR& argName)
{
	MAXSPLICE_CATCH_BEGIN
	return FabricTypeToMaxTypes(GetPortSpec(ToCStr(argName)));
	MAXSPLICE_CATCH_RETURN(BitArray());
}

bool FabricTranslationFPInterface::SetPortMetaData(const char* argName, const char* option, const char* value, const char* execPath)
{
	bool canUndo = UndoOn();
	if (canUndo) {
		theHold.Put(new FabricCoreRestoreObj());
	}
	GetExec(execPath).setExecPortMetadata(argName, option, value, canUndo);
	SyncMetaDataFromPortToParam(argName);
	return true;
}

bool FabricTranslationFPInterface::SetPortMetaData(const MSTR& argName, const MSTR& option, const MSTR& value, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	SetPortMetaData( ToCStr(argName), ToCStr(option), ToCStr(value), ToCStr(execPath));
	return true;
	MAXSPLICE_CATCH_RETURN(false);
}

const char* FabricTranslationFPInterface::GetPortMetaData(const char* port, const char* option, const char* execPath /* = "" */)
{
	return GetExec(execPath).getExecPortMetadata(port, option);
}

MSTR FabricTranslationFPInterface::GetPortMetaData(const MSTR& port, const MSTR& option, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return ToMstr(GetPortMetaData(ToCStr(port), ToCStr(option), ToCStr(execPath)));
	MAXSPLICE_CATCH_RETURN(_M(" ** Exception Occured"));
}

bool FabricTranslationFPInterface::SetArgUIMinMax(const MSTR& argName, FPValue* uiMin, FPValue* uiMax)
{
	MAXSPLICE_CATCH_BEGIN

	// For now, we only support Float/Int max types
	if ((uiMin->type == TYPE_FLOAT && uiMax->type == TYPE_FLOAT) ||
		(uiMin->type == TYPE_INT && uiMax->type == TYPE_INT))
	{
		char buffer[128];
		if (uiMax->type == TYPE_FLOAT)
		{
			float min = uiMin->f;
			float max = uiMax->f;
			sprintf(buffer, "(%f, %f)", min, max);
		}
		else
		{
			int min = uiMin->i;
			int max = uiMax->i;
			sprintf(buffer, "(%i, %i)", min, max);
		}

		CStr cArgName = argName.ToCStr();
		if (SetPortMetaData(cArgName.data(), "uiHardRange", buffer, nullptr))
		{
			SyncMetaDataFromPortToParam(cArgName.data());
			return true;
		}
	}
	return false;
	MAXSPLICE_CATCH_RETURN(false);
}

int FabricTranslationFPInterface::GetPortParamID(const char* argName)
{
	return ::GetPortParamID(GetExec(nullptr), argName);
}
bool FabricTranslationFPInterface::SetPortParamID(const char* argName, int id)
{
	char buff[20];
	_itoa_s(id, buff, 10);
	SetPortMetaData(argName, MAX_PID_OPT, buff, "");
	return true;
}

int FabricTranslationFPInterface::GetPort3dsMaxType(const char* argName)
{
	return ::GetPort3dsMaxType(GetExec(nullptr), argName);
}
bool FabricTranslationFPInterface::SetPort3dsMaxType(const char* argName, int type)
{
	char buff[20];
	_itoa_s(type, buff, 10);
	SetPortMetaData(argName, MAX_PID_OPT, buff, "");
	return true;
}

//////////////////////////////////////////////////////////////////////////
//

bool FabricTranslationFPInterface::LoadFromFile(const MCHAR* filename, bool createMaxParams)
{

	FILE * file = nullptr;
	errno_t erno = _tfopen_s(&file, filename, _T("rb"));
	if (!file)
	{
		erno;
		CStr errMsg;
		errMsg.printf("ERROR %i: Could not open \'%s\'", erno, CStr::FromMCHAR(filename).data());
		logMessage(errMsg);
		return false;
	}

	fseek(file, 0, SEEK_END);
	int fileSize = ftell(file);
	rewind(file);

	char * buffer = (char*)malloc(fileSize + 1);
	buffer[fileSize] = '\0';

	size_t readBytes = fread(buffer, 1, fileSize, file);
	DbgAssert(readBytes == fileSize);

	fclose(file);

	bool res = RestoreFromJSON(buffer, createMaxParams);
	free(buffer);
	return res;
};

bool FabricTranslationFPInterface::SaveToFile(const MCHAR* filename)
{

	FabricCore::String json = m_binding.exportJSON();

	FILE * file = nullptr;
	errno_t erno = _tfopen_s(&file, filename, _T("w"));
	if (!file)
	{
		erno;
		CStr errMsg;
		errMsg.printf("ERROR %i: Could not open \'%s\'", erno, CStr::FromMCHAR(filename).data());
		logMessage(errMsg);
		return false;
	}

	fwrite(json.getCStr(), sizeof(char), json.getSize(), file);
	fclose(file);

	return true;
};

bool FabricTranslationFPInterface::RestoreFromJSON(const MSTR& json, bool createMaxParams)
{
	MAXSPLICE_CATCH_BEGIN
	return RestoreFromJSON(ToCStr(json), createMaxParams);
	MAXSPLICE_CATCH_RETURN( false );
}

bool FabricTranslationFPInterface::RestoreFromJSON(const char* json, bool createMaxParams)
{
	MAXSPLICE_CATCH_BEGIN

	// First, remove all current parameters
	CastToRefTarg()->DeleteReference(0);

	// The KL Editor has pointers to the current graph

	FabricCore::DFGBinding binding = GetHost().createBindingFromJSON(json);
	SetBinding(binding);

	// Setup port connections
	if (createMaxParams)
	{
		FabricCore::DFGExec exec = binding.getExec();

		for (unsigned int i = 0; i < exec.getExecPortCount(); i++)
		{
			// If this could be our out port?
			const char* portName = exec.getExecPortName(i);
			if (exec.getExecPortType(i) == FabricCore::DFGPortType_Out)
			{
				exec.getExecPortResolvedType(i);
				BitArray compatibleTypes = FabricTypeToMaxTypes(GetPortSpec(portName));
				// If this splice type is compatible with this classes output,
				// set this port as our outport
				if (compatibleTypes[GetValueType()]) {
					SetOutPortName(ToMstr(portName));
				}
			}
			else
				SyncMetaDataFromPortToParam(portName);
		}
	}
	CloseDFGGraphEditor();
	InvalidateAll();
	return true;

	MAXSPLICE_CATCH_RETURN( false );
}

MSTR FabricTranslationFPInterface::ExportToJSON()
{
	return ToMstr(m_binding.exportJSON().getCStr());
}

MSTR FabricTranslationFPInterface::GetExecCode(const MSTR& execPath) {
	MAXSPLICE_CATCH_BEGIN
		return ToMstr(GetExec(execPath).getCode());
	MAXSPLICE_CATCH_RETURN(_M("Exception getting code"));
}
//////////////////////////////////////////////////////////////////////////

MSTR FabricTranslationFPInterface::GetGraphName()
{
	const char* title = GetExec(nullptr).getTitle();
	return ToMstr(title);
}


static void BindingNotificationCallback(void * userData, char const *jsonCString, uint32_t jsonLength)
{
	FTL::CStrRef jsonStr( jsonCString );

	FTL::JSONStrWithLoc jsonStrWithLoc( jsonStr );
	FTL::OwnedPtr<FTL::JSONObject const> jsonObject(
		FTL::JSONValue::Decode( jsonStrWithLoc )->cast<FTL::JSONObject>()
		);

	FTL::CStrRef descStr = jsonObject->getString( FTL_STR( "desc" ) );

	if (descStr == FTL_STR( "dirty" ))
	{

		FabricTranslationFPInterface * owner =
			static_cast<FabricTranslationFPInterface *>(userData);
		owner->InvalidateAll();
	}
}

void FabricTranslationFPInterface::SetBinding( FabricCore::DFGBinding binding )
{
	m_binding = binding;
	m_binding.setNotificationCallback( BindingNotificationCallback, this );
}

MaxDFGCmdHandler* FabricTranslationFPInterface::GetCommandHandler()
{
	return &m_fabricCmdHandler;
}

FabricCore::DFGExec FabricTranslationFPInterface::GetExec(const char* execPath)
{
  // It is expected for our binding to be valid if we are here...
  DbgAssert( m_binding.isValid() );
  if (!m_binding.isValid())
	return FabricCore::DFGExec();

	FabricCore::DFGExec exec = m_binding.getExec();
	if (execPath && execPath[0] != '\0')
	{
		MAXSPLICE_CATCH_BEGIN
			exec = exec.getSubExec(execPath);
		MAXSPLICE_CATCH_END
	}
	return exec;
}
FabricCore::DFGExec FabricTranslationFPInterface::GetExec(const MSTR& execPath)
{
	return GetExec(ToCStr(execPath));
}

//////////////////////////////////////////////////////////////////////////
FabricTranslationFPInterface* GetFabricInterface( ReferenceTarget* pTarg )
{
	if (pTarg)
		return static_cast<FabricTranslationFPInterface*>(pTarg->GetInterface(ISPLICE__INTERFACE));
	return nullptr;
}
