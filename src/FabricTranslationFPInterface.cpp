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
	/*MaxDFGWidget* pWidget = */new MaxDFGWidget(dlg, GetBinding(), &m_fabricCmdHandler);

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
		to[i] = TO_CSTR(*from[i]);
	}
}

void Convert(const Tab<Point2*>& from, QList<QPointF>& to) {
	to.reserve(from.Count());
	for (int i = 0; i < from.Count(); i++) {
		to[i] = Convert(*from[i]);
	}
}

Tab<TSTR*> Convert(const QStringList& from)
{
	Tab<TSTR*> to;
	to.SetCount(from.size());
	for (int i = 0; i < from.size(); i++) {
		to[i] = new MSTR(MSTR::FromACP(from[i].toStdString().c_str()));
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
		m_fabricCmdHandler.dfgDoRemoveNodes(m_binding, TO_CSTR(execPath), GetExec(execPath), cNodeNames);
	MAXSPLICE_CATCH_END;
}

void FabricTranslationFPInterface::DFGConnect(const MSTR& srcPath, const MSTR& destPath, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	InvalidateAll();
	return m_fabricCmdHandler.dfgDoConnect(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(srcPath), TO_CSTR(destPath));
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGDisconnect(const MSTR& srcPath, const MSTR& destPath, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	InvalidateAll();
	return m_fabricCmdHandler.dfgDoDisconnect(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(srcPath), TO_CSTR(destPath));
	MAXSPLICE_CATCH_END
}
MSTR FabricTranslationFPInterface::DFGAddGraph(const MSTR& title, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoAddGraph(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(title), Convert(pos)).toStdString().c_str());
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGAddFunc(const MSTR& title, const MSTR& initialCode, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoAddFunc(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(title), TO_CSTR(initialCode), Convert(pos)).toStdString().c_str());
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGInstPreset(const MSTR& presetPath, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoInstPreset(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(presetPath), Convert(pos)).toStdString().c_str());
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGAddVar(const MSTR& desiredNodeName, const MSTR& dataType, const MSTR& extDep, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoAddVar(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(desiredNodeName), TO_CSTR(dataType), TO_CSTR(extDep), Convert(pos)).toStdString().c_str());
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGAddGet(const MSTR& desiredNodeName, const MSTR& varPath, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoAddGet(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(desiredNodeName), TO_CSTR(varPath), Convert(pos)).toStdString().c_str());
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGAddSet(const MSTR& desiredNodeName, const MSTR& varPath, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoAddSet(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(desiredNodeName), TO_CSTR(varPath), Convert(pos)).toStdString().c_str());
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGAddPort(const MSTR& desiredPortName, int portType, const MSTR& portSpec, const MSTR& portToConnect, const MSTR& extDep, const MSTR& metaData, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	FabricCore::DFGPortType fType = static_cast<FabricCore::DFGPortType>(portType);
	CStr cExecPath = execPath.ToCStr();
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoAddPort(m_binding, cExecPath.data(), GetExec(cExecPath.data()), TO_CSTR(desiredPortName), fType, TO_CSTR(portSpec), TO_CSTR(portToConnect), TO_CSTR(extDep), TO_CSTR(metaData)).toStdString().c_str());
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
MSTR FabricTranslationFPInterface::DFGEditPort(const MSTR& portName, const MSTR& desiredNewPortName, const MSTR& typeSpec, const MSTR& extDep, const MSTR& metaData, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QString res = m_fabricCmdHandler.dfgDoEditPort(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(portName), TO_CSTR(desiredNewPortName), TO_CSTR(typeSpec), TO_CSTR(extDep), TO_CSTR(metaData));
	SyncMetaDataFromPortToParam(TO_CSTR(portName));
	return  MSTR::FromACP(res.toStdString().c_str());
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
void FabricTranslationFPInterface::DFGRemovePort(const MSTR& portName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	InvalidateAll();
	return m_fabricCmdHandler.dfgDoRemovePort(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(portName));
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGResizeBackdrop(const MSTR& backDropNodeName, Point2 topLeft, Point2 size, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QSizeF qSize(size.x, size.y);
	return m_fabricCmdHandler.dfgDoResizeBackDrop(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(backDropNodeName), Convert(topLeft), qSize);
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGMoveNodes(Tab<TSTR*> nodeNames, Tab<Point2*> topLeftPoss, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QStringList cNodeNames;
	Convert(nodeNames, cNodeNames);
	QList<QPointF> qTopLeftPoss;
	Convert(topLeftPoss, qTopLeftPoss);
	return m_fabricCmdHandler.dfgDoMoveNodes(m_binding, TO_CSTR(execPath), GetExec(execPath), cNodeNames, qTopLeftPoss);
	MAXSPLICE_CATCH_END
}
MSTR FabricTranslationFPInterface::DFGImplodeNodes(Tab<TSTR*> nodeNames, const MSTR& desiredNewNodeName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QStringList cNodeNames;
	Convert(nodeNames, cNodeNames);
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoImplodeNodes(m_binding, TO_CSTR(execPath), GetExec(execPath), cNodeNames, TO_CSTR(desiredNewNodeName)).toStdString().c_str());
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
Tab<TSTR*> FabricTranslationFPInterface::DFGExplodeNodes(const MSTR& nodeName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QStringList res = m_fabricCmdHandler.dfgDoExplodeNode(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(nodeName));
	return Convert(res);
	MAXSPLICE_CATCH_RETURN(Tab<TSTR*>());
}
void FabricTranslationFPInterface::DFGAddBackdrop(const MSTR& title, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoAddBackDrop(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(title), Convert(pos));
	MAXSPLICE_CATCH_END
}
/* r3d
void FabricTranslationFPInterface::DFGSetTitle(const MSTR& newTitle, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSetTitle(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(newTitle));
	MAXSPLICE_CATCH_END
}
*/

void FabricTranslationFPInterface::DFGSetNodeComment(const MSTR& nodeName, const MSTR& comment, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSetNodeComment(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(nodeName), TO_CSTR(comment));
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGSetCode(const MSTR& code, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSetCode(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(code));
	MAXSPLICE_CATCH_END
}
MSTR FabricTranslationFPInterface::DFGRenamePort(const MSTR& oldName, const MSTR& newDesiredName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	int id = GetPortParamID(TO_CSTR(oldName));
	QString res = m_fabricCmdHandler.dfgDoRenamePort(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(oldName), TO_CSTR(newDesiredName));
	if (id >= 0)
		SetMaxParamName(GetPBlock()->GetDesc(), (ParamID)id, newDesiredName);
	return MSTR::FromACP(res.toStdString().c_str());
	MAXSPLICE_CATCH_RETURN(_M("Exception occured"))
}
Tab<TSTR*> FabricTranslationFPInterface::DFGPaste(const MSTR& json, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QStringList res = m_fabricCmdHandler.dfgDoPaste(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(json), Convert(pos));
	return Convert(res);
	MAXSPLICE_CATCH_RETURN(Tab<TSTR*>())
}
/* r3d
void FabricTranslationFPInterface::DFGSetArgType(const MSTR& argName, const MSTR& argType)
{
	MAXSPLICE_CATCH_BEGIN
	m_fabricCmdHandler.dfgDoSetArgType(m_binding, TO_CSTR(argName), TO_CSTR(argType));
	SyncMetaDataFromPortToParam(TO_CSTR(argName));
	MAXSPLICE_CATCH_END
}
*/

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
		m_fabricCmdHandler.dfgDoSetPortDefaultValue(m_binding, TO_CSTR(execPath), exec, cPortName.data(), defVal);
	}
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGSetRefVarPath(const MSTR& refName, const MSTR& varPath, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSetRefVarPath(m_binding, TO_CSTR(execPath), GetExec(execPath), TO_CSTR(refName), TO_CSTR(varPath));
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGReorderPorts(Tab<int> indices, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QList<int> vindices;
  for (unsigned int i = 0; i < indices.Count(); ++i)
  {
    vindices[i] =indices[i];
  }
	return m_fabricCmdHandler.dfgDoReorderPorts(m_binding, TO_CSTR(execPath), GetExec(execPath), vindices);
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGSetExtDeps(Tab<TSTR*> extDeps, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QStringList cExtDeps;
	Convert(extDeps, cExtDeps);
	return m_fabricCmdHandler.dfgDoSetExtDeps(m_binding, TO_CSTR(execPath), GetExec(execPath), cExtDeps);
	MAXSPLICE_CATCH_END
}
void FabricTranslationFPInterface::DFGSplitFromPreset(const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSplitFromPreset(m_binding, TO_CSTR(execPath), GetExec(execPath));
	MAXSPLICE_CATCH_END
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
	return MSTR::FromACP(GetExec(execPath).getExecPortName(i));
	MAXSPLICE_CATCH_RETURN(_M(" ** Exception Occured"));
}

const char* FabricTranslationFPInterface::GetPortType(const char* portName, const char* execPath)
{

	return ::GetPortType(GetExec(execPath), portName);
}

MSTR FabricTranslationFPInterface::GetPortType(const MSTR& portName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(GetPortType(TO_CSTR(portName), TO_CSTR(execPath)));
	MAXSPLICE_CATCH_RETURN(_M(" ** Exception Occured"));
}

bool FabricTranslationFPInterface::HasSrcPort(const char* portName)
{
	return GetExec(nullptr).hasSrcPorts(portName);
}

bool FabricTranslationFPInterface::HasSrcPort(const MSTR& portName)
{
	MAXSPLICE_CATCH_BEGIN
		return HasSrcPort(TO_CSTR(portName));
	MAXSPLICE_CATCH_RETURN(_M(" ** Exception Occured"));
}

bool FabricTranslationFPInterface::HasDstPort(const char* portName)
{
	return GetExec(nullptr).hasDstPorts(portName);
}

bool FabricTranslationFPInterface::HasDstPort(const MSTR& portName)
{
	MAXSPLICE_CATCH_BEGIN
		return HasDstPort(TO_CSTR(portName));
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
	return MSTR::FromACP(GetExec(execPath).getNodeName(i));
	MAXSPLICE_CATCH_RETURN(_M(" ** Exception Occured"));
}

int FabricTranslationFPInterface::GetNodeType(const MSTR& NodeName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	FabricCore::DFGNodeType nodeType = GetExec(execPath).getNodeType(TO_CSTR(NodeName));
	return nodeType;
	MAXSPLICE_CATCH_RETURN(0);
}


bool FabricTranslationFPInterface::GetArgValue(const char* argName, FPValue& value/*, const MSTR& execPath*/)
{
	MAXSPLICE_CATCH_BEGIN

	//FabricCore::DFGExec exec = GetExec(execPath);

	//exec.getPort
	FabricCore::RTVal rtVal = m_binding.getArgValue(argName);
	const char* cType = GetPortType(argName);
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
		value.Load(TYPE_POINT2, p2);
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
		DbgAssert(!"Cannot convert Fabric type");
		static MSTR errorStr;
		errorStr = _M("Cannot convert Fabric type: ");
		errorStr += MSTR::FromACP(cType);
		value.Load(TYPE_STRING, errorStr);
		return false;
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
	GetArgValue(TO_CSTR(portName), val);
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
	return GetMaxTypeForArg(TO_CSTR(argName));
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
	return SetMaxTypeForArg(TO_CSTR(argName), type);
	MAXSPLICE_CATCH_RETURN(-1);
}

BitArray FabricTranslationFPInterface::GetLegalMaxTypesForArg(const MSTR& argName)
{
	MAXSPLICE_CATCH_BEGIN
	return FabricTypeToMaxTypes(GetPortType(TO_CSTR(argName)));
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
	SetPortMetaData(TO_CSTR(argName), TO_CSTR(option), TO_CSTR(value), TO_CSTR(execPath));
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
	return MSTR::FromACP(GetPortMetaData(TO_CSTR(port), TO_CSTR(option), TO_CSTR(execPath)));
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
	assert(readBytes == fileSize);

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
	return RestoreFromJSON(TO_CSTR(json), createMaxParams);
	MAXSPLICE_CATCH_RETURN( false );
}

bool FabricTranslationFPInterface::RestoreFromJSON(const char* json, bool createMaxParams)
{
	MAXSPLICE_CATCH_BEGIN

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
				BitArray compatibleTypes = FabricTypeToMaxTypes(GetPortType(portName));
				// If this splice type is compatible with this classes output,
				// set this port as our outport
				if (compatibleTypes[GetValueType()]) {
					SetOutPortName(MSTR::FromACP(portName));
				}
			}
			else
				SyncMetaDataFromPortToParam(portName);
		}
	}
	InvalidateAll();
	return true;

	MAXSPLICE_CATCH_RETURN( false );
}

MSTR FabricTranslationFPInterface::ExportToJSON()
{
	return MSTR::FromACP(m_binding.exportJSON().getCStr());
}

MSTR FabricTranslationFPInterface::GetExecCode(const MSTR& execPath) {
	MAXSPLICE_CATCH_BEGIN
		return MSTR::FromACP(GetExec(TO_CSTR(execPath)).getCode());
	MAXSPLICE_CATCH_RETURN(_M("Exception getting code"));
}
//////////////////////////////////////////////////////////////////////////

MSTR FabricTranslationFPInterface::GetGraphName()
{
	const char* title = GetExec(nullptr).getTitle();
	return MSTR::FromACP(title);
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
	return GetExec(TO_CSTR(execPath));
}

//////////////////////////////////////////////////////////////////////////
FabricTranslationFPInterface* GetFabricInterface( ReferenceTarget* pTarg )
{
	if (pTarg)
		return static_cast<FabricTranslationFPInterface*>(pTarg->GetInterface(ISPLICE__INTERFACE));
	return nullptr;
}
