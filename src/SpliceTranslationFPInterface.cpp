#include "StdAfx.h"
#include "SpliceTranslationFPInterface.h"

#include <MaxScript/MaxScript.h>
#include <maxscript\foundation\functions.h>
#include <maxscript\maxwrapper\mxsobjects.h>
#include "MaxDFGWidget.h"
#include "DockableWidget.h"

#include "../qt-solutions/qtwinmigrate/src/qwinwidget.h"
#include "../qt-solutions/qtwinmigrate/src/QMfcApp"
#include "Splice3dsmax.h"

SpliceTranslationFPInterface::SpliceTranslationFPInterface()
	: m_klEditor(nullptr)
	, m_pDFGWidgetWindow(nullptr)
	, m_fabricCmdHandler(this)
{

}

SpliceTranslationFPInterface::~SpliceTranslationFPInterface()
{
	CloseDFGGraphEditor();
}

BOOL SpliceTranslationFPInterface::ShowDFGGraphEditor()
{
	SAFE_DELETE(m_pDFGWidgetWindow);

	/*bool ownApplication = */ QMfcApp::pluginInstance(hInstance);	
	m_pDFGWidgetWindow = DockableWindow::Create(_T("Fabric DFG"));
	HWND h = m_pDFGWidgetWindow->GetHWND();
	QWinWidget* dlg = new QWinWidget(h);
	/*MaxDFGWidget* pWidget =*/ new MaxDFGWidget(dlg, GetBinding(), &m_fabricCmdHandler);

	m_pDFGWidgetWindow->SetWidget(dlg);
	return TRUE;
}

void SpliceTranslationFPInterface::CloseDFGGraphEditor()
{
	SAFE_DELETE(m_pDFGWidgetWindow);
}

//////////////////////////////////////////////////////////////////////////
#pragma region Fabric commands exposed to MaxScript

#pragma region a few conv functions
#define TO_CSTR(x) (x).ToCStr().data()
QPointF Convert(const Point2& v) { return QPointF(v.x, v.y); }

void Convert(const Tab<MSTR*>& from, std::vector<FTL::StrRef>& to) {
	to.resize(from.Count());
	for (int i = 0; i < from.Count(); i++) {
		to[i] = TO_CSTR(*from[i]);
	}
}

void Convert(const Tab<Point2*>& from, std::vector<QPointF>& to) {
	to.resize(from.Count());
	for (int i = 0; i < from.Count(); i++) {
		to[i] = Convert(*from[i]);
	}
}

Tab<TSTR*> Convert(const std::vector<std::string>& from)
{
	Tab<TSTR*> to;
	to.SetCount(from.size());
	for (int i = 0; i < from.size(); i++) {
		to[i] = new MSTR(MSTR::FromACP(from[i].c_str()));
	}
	return to;
}

#pragma endregion

// The following are direct mappers to the commands defined by the DFGCmdHandler classed.
void SpliceTranslationFPInterface::DFGRemoveNodes(const Tab<TSTR*>& nodeNames, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN;
		std::vector<FTL::StrRef> cNodeNames;
		Convert(nodeNames, cNodeNames);
		m_fabricCmdHandler.dfgDoRemoveNodes(m_binding, TO_CSTR(execPath), m_binding.getExec(), cNodeNames);
	MAXSPLICE_CATCH_END;
}

void SpliceTranslationFPInterface::DFGConnect(const MSTR& srcPath, const MSTR& destPath, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	InvalidateAll();
	return m_fabricCmdHandler.dfgDoConnect(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(srcPath), TO_CSTR(destPath));
	MAXSPLICE_CATCH_END
}
void SpliceTranslationFPInterface::DFGDisconnect(const MSTR& srcPath, const MSTR& destPath, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	InvalidateAll();
	return m_fabricCmdHandler.dfgDoDisconnect(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(srcPath), TO_CSTR(destPath));
	MAXSPLICE_CATCH_END
}
MSTR SpliceTranslationFPInterface::DFGAddGraph(const MSTR& title, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoAddGraph(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(title), Convert(pos)).c_str());
	MAXSPLICE_CATCH_RETURN("Exception occured")
}
MSTR SpliceTranslationFPInterface::DFGAddFunc(const MSTR& title, const MSTR& initialCode, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoAddFunc(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(title), TO_CSTR(initialCode), Convert(pos)).c_str());
	MAXSPLICE_CATCH_RETURN("Exception occured")
}
MSTR SpliceTranslationFPInterface::DFGInstPreset(const MSTR& presetPath, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoInstPreset(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(presetPath), Convert(pos)).c_str());
	MAXSPLICE_CATCH_RETURN("Exception occured")
}
MSTR SpliceTranslationFPInterface::DFGAddVar(const MSTR& desiredNodeName, const MSTR& dataType, const MSTR& extDep, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoAddVar(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(desiredNodeName), TO_CSTR(dataType), TO_CSTR(extDep), Convert(pos)).c_str());
	MAXSPLICE_CATCH_RETURN("Exception occured")
}
MSTR SpliceTranslationFPInterface::DFGAddGet(const MSTR& desiredNodeName, const MSTR& varPath, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoAddGet(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(desiredNodeName), TO_CSTR(varPath), Convert(pos)).c_str());
	MAXSPLICE_CATCH_RETURN("Exception occured")
}
MSTR SpliceTranslationFPInterface::DFGAddSet(const MSTR& desiredNodeName, const MSTR& varPath, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoAddSet(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(desiredNodeName), TO_CSTR(varPath), Convert(pos)).c_str());
	MAXSPLICE_CATCH_RETURN("Exception occured")
}
MSTR SpliceTranslationFPInterface::DFGAddPort(const MSTR& desiredPortName, int portType, const MSTR& portSpec, const MSTR& portToConnect, const MSTR& extDep, const MSTR& metaData, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	FabricCore::DFGPortType fType = static_cast<FabricCore::DFGPortType>(portType);
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoAddPort(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(desiredPortName), fType, TO_CSTR(portSpec), TO_CSTR(portToConnect), TO_CSTR(extDep), TO_CSTR(metaData)).c_str());
	MAXSPLICE_CATCH_RETURN("Exception occured")
}
MSTR SpliceTranslationFPInterface::DFGEditPort(const MSTR& portName, const MSTR& desiredNewPortName, const MSTR& typeSpec, const MSTR& extDep, const MSTR& metaData, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	std::string res = m_fabricCmdHandler.dfgDoEditPort(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(portName), TO_CSTR(desiredNewPortName), TO_CSTR(typeSpec), TO_CSTR(extDep), TO_CSTR(metaData));
	SyncMetaDataFromPortToParam(TO_CSTR(portName));
	return  MSTR::FromACP(res.data());
	MAXSPLICE_CATCH_RETURN("Exception occured")
}
void SpliceTranslationFPInterface::DFGRemovePort(const MSTR& portName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	InvalidateAll();
	return m_fabricCmdHandler.dfgDoRemovePort(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(portName));
	MAXSPLICE_CATCH_END
}
void SpliceTranslationFPInterface::DFGResizeBackdrop(const MSTR& backDropNodeName, Point2 topLeft, Point2 size, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	QSizeF qSize(size.x, size.y);
	return m_fabricCmdHandler.dfgDoResizeBackDrop(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(backDropNodeName), Convert(topLeft), qSize);
	MAXSPLICE_CATCH_END
}
void SpliceTranslationFPInterface::DFGMoveNodes(Tab<TSTR*> nodeNames, Tab<Point2*> topLeftPoss, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	std::vector<FTL::StrRef> cNodeNames;
	Convert(nodeNames, cNodeNames);
	std::vector<QPointF> qTopLeftPoss;
	Convert(topLeftPoss, qTopLeftPoss);
	return m_fabricCmdHandler.dfgDoMoveNodes(m_binding, TO_CSTR(execPath), m_binding.getExec(), cNodeNames, qTopLeftPoss);
	MAXSPLICE_CATCH_END
}
MSTR SpliceTranslationFPInterface::DFGImplodeNodes(Tab<TSTR*> nodeNames, const MSTR& desiredNewNodeName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	std::vector<FTL::StrRef> cNodeNames;
	Convert(nodeNames, cNodeNames);
	return MSTR::FromACP(m_fabricCmdHandler.dfgDoImplodeNodes(m_binding, TO_CSTR(execPath), m_binding.getExec(), cNodeNames, TO_CSTR(desiredNewNodeName)).c_str());
	MAXSPLICE_CATCH_RETURN("Exception occured")
}
Tab<TSTR*> SpliceTranslationFPInterface::DFGExplodeNodes(const MSTR& nodeName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	std::vector<std::string> res = m_fabricCmdHandler.dfgDoExplodeNode(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(nodeName));
	return Convert(res);
	MAXSPLICE_CATCH_RETURN(Tab<TSTR*>());
}
void SpliceTranslationFPInterface::DFGAddBackdrop(const MSTR& title, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoAddBackDrop(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(title), Convert(pos));
	MAXSPLICE_CATCH_END
}
void SpliceTranslationFPInterface::DFGSetNodeTitle(const MSTR& nodeName, const MSTR& newTitle, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSetNodeTitle(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(nodeName), TO_CSTR(newTitle));
	MAXSPLICE_CATCH_END
}
void SpliceTranslationFPInterface::DFGSetNodeComment(const MSTR& nodeName, const MSTR& comment, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSetNodeComment(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(nodeName), TO_CSTR(comment));
	MAXSPLICE_CATCH_END
}
void SpliceTranslationFPInterface::DFGSetCode(const MSTR& code, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSetCode(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(code));
	MAXSPLICE_CATCH_END
}
MSTR SpliceTranslationFPInterface::DFGRenamePort(const MSTR& oldName, const MSTR& newDesiredName, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	int id = GetPortParamID(TO_CSTR(oldName));
	std::string res = m_fabricCmdHandler.dfgDoRenamePort(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(oldName), TO_CSTR(newDesiredName));
	if (id >= 0)
		SetMaxParamName(GetPBlock()->GetDesc(), (ParamID)id, newDesiredName);
	return MSTR::FromACP(res.c_str());
	MAXSPLICE_CATCH_RETURN("Exception occured")
}
Tab<TSTR*> SpliceTranslationFPInterface::DFGPaste(const MSTR& json, Point2 pos, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	std::vector<std::string> res = m_fabricCmdHandler.dfgDoPaste(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(json), Convert(pos));
	return Convert(res);
	MAXSPLICE_CATCH_RETURN(Tab<TSTR*>())
}
void SpliceTranslationFPInterface::DFGSetArgType(const MSTR& argName, const MSTR& argType)
{
	MAXSPLICE_CATCH_BEGIN
	m_fabricCmdHandler.dfgDoSetArgType(m_binding, TO_CSTR(argName), TO_CSTR(argType));
	SyncMetaDataFromPortToParam(TO_CSTR(argName));
	MAXSPLICE_CATCH_END
}
void SpliceTranslationFPInterface::DFGSetArgValue(const MSTR& argName, const FPValue* argValue)
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
void SpliceTranslationFPInterface::DFGSetPortDefaultValue(const MSTR& portName, const FPValue* fpvalue, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	CStr cPortName = portName.ToCStr();
	FabricCore::RTVal rtVal = m_binding.getArgValue(cPortName.data());
	if (rtVal.isValid())
	{
		ConvertToRTVal(*fpvalue, rtVal);
		m_fabricCmdHandler.dfgDoSetPortDefaultValue(m_binding, TO_CSTR(execPath), m_binding.getExec(), cPortName.data(), rtVal);
		SyncMetaDataFromPortToParam(cPortName.data());
	}
	MAXSPLICE_CATCH_END
}
void SpliceTranslationFPInterface::DFGSetRefVarPath(const MSTR& refName, const MSTR& varPath, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSetRefVarPath(m_binding, TO_CSTR(execPath), m_binding.getExec(), TO_CSTR(refName), TO_CSTR(varPath));
	MAXSPLICE_CATCH_END
}
void SpliceTranslationFPInterface::DFGReorderPorts(Tab<int> indices, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	std::vector<unsigned int> vindices(indices.Addr(0), indices.Addr(0) + indices.Count());
	return m_fabricCmdHandler.dfgDoReorderPorts(m_binding, TO_CSTR(execPath), m_binding.getExec(), vindices);
	MAXSPLICE_CATCH_END
}
void SpliceTranslationFPInterface::DFGSetExtDeps(Tab<TSTR*> extDeps, const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	std::vector<FTL::StrRef> cExtDeps;
	Convert(extDeps, cExtDeps);
	return m_fabricCmdHandler.dfgDoSetExtDeps(m_binding, TO_CSTR(execPath), m_binding.getExec(), cExtDeps);
	MAXSPLICE_CATCH_END
}
void SpliceTranslationFPInterface::DFGSplitFromPreset(const MSTR& execPath)
{
	MAXSPLICE_CATCH_BEGIN
	return m_fabricCmdHandler.dfgDoSplitFromPreset(m_binding, TO_CSTR(execPath), m_binding.getExec());
	MAXSPLICE_CATCH_END
}

//// Load the splice graph for this entity from the given filename
//bool LoadFromFile(const MCHAR* filename, bool createMaxParams);
//bool SaveToFile(const MCHAR* filename);
//
//bool RestoreFromJSON(const MSTR& json, bool createMaxParams);
//MSTR ExportToJSON();

// Allow introspecting the ports on this graph
int SpliceTranslationFPInterface::GetPortCount(const MSTR& execPath)
{
	return m_binding.getExec().getExecPortCount();
}

MSTR SpliceTranslationFPInterface::GetPortName(int i, const MSTR& execPath)
{
	return MSTR::FromACP(m_binding.getExec().getExecPortName(i));
}

const char* SpliceTranslationFPInterface::GetPortType(const char* portName, const char* execPath)
{
	return ::GetPortType(GetBinding(), portName);
}

MSTR SpliceTranslationFPInterface::GetPortType(const MSTR& portName, const MSTR& execPath)
{
	return MSTR::FromACP(GetPortType(TO_CSTR(portName), TO_CSTR(execPath)));
}


bool SpliceTranslationFPInterface::GetPortValue(const char* argName, FPValue& value)
{
	MAXSPLICE_CATCH_BEGIN

	FabricCore::RTVal rtVal = m_binding.getArgValue(argName);
	const char* cType = GetPortType(argName);
	int type = SpliceTypeToMaxType(cType);

	switch (type)
	{
	case TYPE_INT:
	{
		int i;
		SpliceToMaxValue(rtVal, i);
		value.Load(TYPE_INT, i);
	}
	break;
	case TYPE_BOOL:
	{
		bool b;
		SpliceToMaxValue(rtVal, b);
		value.Load(TYPE_BOOL, b);
		break;
	}
	case TYPE_FLOAT:
	{
		float f;
		SpliceToMaxValue(rtVal, f);
		value.Load(TYPE_FLOAT, f);
		break;
	}
	case TYPE_FRGBA:
	{
		Point4 p4;
		SpliceToMaxValue(rtVal, p4);
		value.Load(TYPE_FRGBA, p4);
		break;
	}
	case TYPE_POINT2:
	{
		Point2 p2;
		SpliceToMaxValue(rtVal, p2);
		value.Load(TYPE_POINT2, p2);
		break;
	}
	case TYPE_POINT3:
	{
		Point3 v;
		SpliceToMaxValue(rtVal, v);
		value.Load(TYPE_POINT3, v);
		break;
	}
	case TYPE_POINT4:
	{
		Point4 p4;
		SpliceToMaxValue(rtVal, p4);
		value.Load(TYPE_POINT4, p4);
		break;
	}
	case TYPE_MATRIX3:
	{
		Matrix3 m;
		SpliceToMaxValue(rtVal, m);
		value.Load(TYPE_MATRIX3, m);
		break;
	}
	case TYPE_QUAT:
	{
		Quat q;
		SpliceToMaxValue(rtVal, q);
		value.Load(TYPE_QUAT, q);
		break;
	}
	case TYPE_STRING:
	{
		TSTR tstr;
		SpliceToMaxValue(rtVal, tstr);
		value.Load(TYPE_TSTR, tstr);
		break;
	}
	case TYPE_MESH:
	{
		Mesh mesh;
		SpliceToMaxValue(rtVal, mesh);
		value.Load(TYPE_MESH, mesh);
		break;
	}

	}
	return true;
#pragma message("TEST THIS")
	MAXSPLICE_CATCH_END
		return false;
}

FPValue SpliceTranslationFPInterface::GetPortValue(const MSTR& portName, const MSTR& execPath)
{
	// We make the val static so we don't leak pointers assigned to it.
	static FPValue val;
	// Free whatever memory was assigned last time
	val.Free();
	GetPortValue(TO_CSTR(portName), val);
	return val;
}

int SpliceTranslationFPInterface::GetMaxTypeForArg(const char* argName)
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

int SpliceTranslationFPInterface::GetMaxTypeForArg(const MSTR& argName)
{
	return GetMaxTypeForArg(TO_CSTR(argName));
}

int SpliceTranslationFPInterface::SetMaxTypeForArg(const char* argName, int type)
{
	char buff[20];
	_itoa_s(type, buff, 10);
	SetPortMetaData(argName, MAX_PARM_TYPE_OPT, buff);
	SyncMetaDataFromPortToParam(argName);
	return -1;

}
int SpliceTranslationFPInterface::SetMaxTypeForArg(const MSTR& argName, int type)
{
	return SetMaxTypeForArg(TO_CSTR(argName), type);
}

BitArray SpliceTranslationFPInterface::GetLegalMaxTypesForArg(const MSTR& argName)
{
	return SpliceTypeToMaxTypes(GetPortType(TO_CSTR(argName)));
}

bool SpliceTranslationFPInterface::SetPortMetaData(const char* argName, const char* option, const char* value, const char* execPath)
{
	bool canUndo = theHold.Holding();
	if (canUndo) {
		theHold.Put(new FabricCoreRestoreObj());
	}
	GetBinding().getExec().setExecPortMetadata(argName, option, value, canUndo);
	SyncMetaDataFromPortToParam(argName);
	return true;
}

bool SpliceTranslationFPInterface::SetPortMetaData(const MSTR& argName, const MSTR& option, const MSTR& value, const MSTR& execPath)
{
	SetPortMetaData(TO_CSTR(argName), TO_CSTR(option), TO_CSTR(value), TO_CSTR(execPath));
	return true;
}

const char* SpliceTranslationFPInterface::GetPortMetaData(const char* port, const char* option, const char* execPath /* = "" */)
{
	return GetBinding().getExec().getExecPortMetadata(port, option);
}

MSTR SpliceTranslationFPInterface::GetPortMetaData(const MSTR& port, const MSTR& option, const MSTR& execPath)
{
	return MSTR::FromACP(GetPortMetaData(TO_CSTR(port), TO_CSTR(option), TO_CSTR(execPath)));
}

bool SpliceTranslationFPInterface::SetPortUIMinMax(const MSTR& argName, FPValue* uiMin, FPValue* uiMax, const MSTR& execPath)
{
	// For now, we only support Float/Int max types
	if ((uiMin->type == TYPE_FLOAT && uiMax->type == TYPE_FLOAT) ||
		(uiMin->type == TYPE_INT && uiMax->type == TYPE_INT))
	{
		MCHAR buffer[128];
		if (uiMax->type == TYPE_FLOAT)
		{
			float min = uiMin->f;
			float max = uiMax->f;
			_stprintf_s(buffer, _M("(%f,%f)"), min, max);
		}
		else
		{
			int min = uiMin->i;
			int max = uiMax->i;
			_stprintf_s(buffer, _M("(%i,%i)"), min, max);
		}
		if (SetPortMetaData(argName, _M("uiRange"), buffer, execPath))
		{
			SyncMetaDataFromPortToParam(TO_CSTR(argName));
			return true;
		}
	}
	return false;
}

int SpliceTranslationFPInterface::GetPortParamID(const char* argName)
{
	return ::GetPortParamID(GetBinding(), argName);
}
bool SpliceTranslationFPInterface::SetPortParamID(const char* argName, int id)
{
	char buff[20];
	_itoa_s(id, buff, 10);
	SetPortMetaData(argName, MAX_PID_OPT, buff, "");
	return true;
}

int SpliceTranslationFPInterface::GetPort3dsMaxType(const char* argName)
{
	return ::GetPort3dsMaxType(GetBinding(), argName);
}
bool SpliceTranslationFPInterface::SetPort3dsMaxType(const char* argName, int type)
{
	char buff[20];
	_itoa_s(type, buff, 10);
	SetPortMetaData(argName, MAX_PID_OPT, buff, "");
	return true;
}

//////////////////////////////////////////////////////////////////////////
//

bool SpliceTranslationFPInterface::LoadFromFile(const MCHAR* filename, bool createMaxParams)
{

	FILE * file = nullptr;
	errno_t erno = _tfopen_s(&file, filename, _T("rb"));
	if (!file)
	{
		erno;
		//log()
		// TODO - Fix logging!
	}

	fseek(file, 0, SEEK_END);
	int fileSize = ftell(file);
	rewind(file);

	char * buffer = (char*)malloc(fileSize + 1);
	buffer[fileSize] = '\0';

	size_t readBytes = fread(buffer, 1, fileSize, file);
	assert(readBytes == fileSize);

	fclose(file);

	RestoreFromJSON(buffer, createMaxParams);
	free(buffer);
	return true;
};

bool SpliceTranslationFPInterface::SaveToFile(const MCHAR* filename)
{

	FabricCore::String json = m_binding.exportJSON();

	FILE * file = nullptr;
	errno_t erno = _tfopen_s(&file, filename, _T("w"));
	if (!file)
	{
		erno;
		//log()
		// TODO - Fix logging!
		return false;
	}

	fwrite(json.getCStr(), sizeof(char), json.getSize(), file);
	fclose(file);

	return true;
};

bool SpliceTranslationFPInterface::RestoreFromJSON(const MSTR& json, bool createMaxParams)
{
	return RestoreFromJSON(TO_CSTR(json), createMaxParams);
}

bool SpliceTranslationFPInterface::RestoreFromJSON(const char* json, bool createMaxParams)
{
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
				BitArray compatibleTypes = SpliceTypeToMaxTypes(GetPortType(portName));
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
}

MSTR SpliceTranslationFPInterface::ExportToJSON()
{
	return MSTR::FromACP(m_binding.exportJSON().getCStr());
}

MSTR SpliceTranslationFPInterface::GetExecCode(const MSTR& execPath) {
	MAXSPLICE_CATCH_BEGIN
		return MSTR::FromACP(GetExec(TO_CSTR(execPath)).getCode());
	MAXSPLICE_CATCH_RETURN(_M("Exception getting code"));
}
//////////////////////////////////////////////////////////////////////////

//bool SpliceTranslationFPInterface::RemovePortMSTR(const MSTR& name) 
//{
//	CStr cName = name.ToCStr();
//	for (int i = 0; i < GetPortCount(); i++)
//	{
//		if (strcmp(GetPortName(i), cName.data()) == 0)
//			return RemovePort(i);
//	}
//	return false;
//}

//bool SpliceTranslationFPInterface::ConnectPortMSTR( const MSTR& myPortName, ReferenceTarget* pSrcContainer, const MSTR& srcPortName, int srcPortIndex, bool postConnectionsUI  )
//{
//	if (pSrcContainer == NULL)
//		return false;
//
//	CStr cMyPortName = myPortName.ToCStr();
//	CStr cSrcPortName = srcPortName.ToCStr();
//	return ConnectPort(cMyPortName, pSrcContainer, cSrcPortName, srcPortIndex, postConnectionsUI);
//}

MSTR SpliceTranslationFPInterface::GetGraphName()
{
	FabricCore::DFGBinding& binding = GetBinding();
	FabricCore::DFGExec exec = binding.getExec();
	const char* title = exec.getTitle();
	return MSTR::FromACP(title);
}


MaxDFGCmdHandler* SpliceTranslationFPInterface::GetCommandHandler()
{
	return &m_fabricCmdHandler;
}

FabricCore::DFGExec SpliceTranslationFPInterface::GetExec(const char* execPath)
{
	FabricCore::DFGExec exec = m_binding.getExec();
	if (execPath && execPath[0] != '\0')
	{
		MAXSPLICE_CATCH_BEGIN
			exec = exec.getSubExec(execPath);
		MAXSPLICE_CATCH_END
	}
	return exec;
}

/*
Value* CallMaxScriptFunction(MCHAR* function, ReferenceTarget* fnArgument, bool returnResult) 
{
	Value* valueArg = MAXClass::make_wrapper_for(fnArgument);
	return CallMaxScriptFunction(function, valueArg, returnResult);
}

Value* CallMaxScriptFunction(MCHAR* function, Value* fnArgument, bool returnResult)
{
	if (fnArgument == nullptr)
		return nullptr;

	// We are going to called a function defined entirely in MaxScript
	// to pop the editor dialog.  This MxS argument is required to take
	// a single argument called interface which is a pointer to the class
	// whose KL we are going to edit.  This genius section of code was
	// lifted from:
	// <http://sourceforge.net/p/niftools/max-nif-plugin/ci/master/tree/NifCommon/nimorph.cpp#l88>, 
	// pretty damn snazzy stuff

	// Magic initialization stuff for maxscript.
	static bool script_initialized = false;
	if (!script_initialized) {
		init_MAXScript();
		script_initialized = TRUE;

		// On first run, evaluate the script that defines our function
		char* mxsEditor = nullptr;
		size_t buffSize = 0;
		if (_dupenv_s(&mxsEditor, &buffSize, "SCINTILLANETDIR") == 0) {
			MSTR mxsEditorPath = MSTR::FromACP(mxsEditor, buffSize);
			mxsEditorPath = mxsEditorPath + _T("SpliceScintillaEditor.ms");
			filein_script(mxsEditorPath.data());
			free(mxsEditor);
		}
	}
	init_thread_locals();
	push_alloc_frame();
	four_value_locals(name, fn, target, result);
	save_current_frames();
	set_error_trace_back_active(FALSE);

	try	{
		// Create the name of the maxscript function we want.
		// and look it up in the global names
		vl.name = Name::intern(function);
		vl.fn = globals->get(vl.name);

		// For some reason we get a global thunk back, so lets
		// check the cell which should point to the function.
		// Just in case if it points to another global thunk
		// try it again.
		while (vl.fn != NULL && is_globalthunk(vl.fn))
			vl.fn = static_cast<GlobalThunk*>(vl.fn)->cell;
		while (vl.fn != NULL && is_constglobalthunk(vl.fn))
			vl.fn = static_cast<ConstGlobalThunk*>(vl.fn)->cell;

		// Now we should have a MAXScriptFunction, which we can
		// call to do the actual conversion. If we didn't
		// get a MAXScriptFunction, we can't convert.
		if (vl.fn != NULL) {
			// Ok. our KL editor takes takes 1 parameter
			// Call the function and save the result.
			vl.result = static_cast<Primitive*>(vl.fn)->apply(&fnArgument, 1);
		}
	} catch (...) {
		clear_error_source_data();
		restore_current_frames();
		MAXScript_signals = 0;
	}

	// If we are returning this value, we need to protect it from GC
	Value* res = nullptr;
	if (returnResult)
		res = vl.result ? vl.result->make_heap_permanent() : nullptr;

	// Magic Max Script stuff to clear the frame and locals.
	pop_value_locals();
	pop_alloc_frame();
	return res;
}
*/
SpliceTranslationFPInterface* GetSpliceInterface( ReferenceTarget* pTarg )
{
	if (pTarg)
		return static_cast<SpliceTranslationFPInterface*>(pTarg->GetInterface(ISPLICE__INTERFACE));
	return nullptr;
}
