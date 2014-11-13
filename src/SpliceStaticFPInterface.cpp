#include "StdAfx.h"
#include "SpliceStaticFPInterface.h"
#include "MaxScript/MaxScript.h"
#include "SpliceEvents.h"
#include "FabricSplice.h"
#include "FabricCore.h"

#if MAX_VERSION_MAJOR < 15
#define p_end end
#endif

SpliceStaticFPInterface* SpliceStaticFPInterface::GetInstance()
{
	// The single instance of this class
	// Initialize our instance.  This part is tells Max about the
	// functions exposed in our function map.
	static SpliceStaticFPInterface _theInstance(
		// Describe our interface
		ISPLICE_STATIC_INTERFACE , _T("Splice"), 0, NULL, FP_CORE,
		// Describe our function(s)
			SpliceStaticFPInterface::fn_showSceneGraphEditor, _T("ShowSceneGraphEditor"), 0, TYPE_BOOL, 0, 0, 
			SpliceStaticFPInterface::fn_importSpliceFile, _T("LoadFromFile"), 0, TYPE_BOOL, 0, 1,
				_M("file"),	0,	TYPE_TSTR_BV,
			
			SpliceStaticFPInterface::fn_getGlobalOperatorCount, _T("GetGlobalOperatorCount"), 0, TYPE_INT, 0, 0,
			SpliceStaticFPInterface::fn_getGlobalOperatorName, _T("GetGlobalOperatorName"), 0, TYPE_TSTR_BV, 0, 1,
				_M("index"),	0,	TYPE_INDEX,
			SpliceStaticFPInterface::fn_loadFabricExtension, _T("LoadExtension"), 0, TYPE_BOOL, 0, 3,
				_M("extension"),	0,	TYPE_TSTR_BV,
				_M("version"),	0,	TYPE_TSTR_BV,
				_M("reload"),	0,	TYPE_bool,

			SpliceStaticFPInterface::fn_enableLogging, _T("EnableLoggers"), 0, TYPE_INT, 0, 1,
				_M("loggers"),	0,	TYPE_ENUM, SpliceStaticFPInterface::loggingEnums,
			SpliceStaticFPInterface::fn_disableLogging, _T("DisableLoggers"), 0, TYPE_INT, 0, 1,
				_M("loggers"),	0,	TYPE_ENUM, SpliceStaticFPInterface::loggingEnums,


		properties,
			SpliceStaticFPInterface::prop_getSpliceRendering,SpliceStaticFPInterface::prop_setSpliceRendering, _T("Rendering"), 0,TYPE_bool,
			SpliceStaticFPInterface::prop_getSpliceManip,SpliceStaticFPInterface::prop_setSpliceManip, _T("Manipulation"), 0,TYPE_bool,

		enums,
			SpliceStaticFPInterface::loggingEnums, LOG_ENUM_COUNT,
				_T("logGeneric"), LOG_GENERIC,
				_T("logErrors"), LOG_ERROR,
				_T("logCompiler"), LOG_COMPILER_ERROR,
				_T("logKLReports"), LOG_KL_REPORTS,
				_T("logKLStatus"), LOG_KL_STATUS,
				_T("logAll"), LOG_ALL,
		p_end
		);
	return &_theInstance;
}

// Implement the functions exposed above
BOOL SpliceStaticFPInterface::ShowSceneGraphEditor()
{
	//if (CPApplication::m_applicationState == CPApplication::kReady)
	//{
	//	SGCAPI::Wrapper* pWrapper = CPApplication::GetInstance()->GetWrapper();
	//	if (pWrapper == NULL)
	//		return FALSE;

	//	SGCAPI::Widget widget = pWrapper->getApplication()->getWidget("Xfo Options");
	//	HWND mainWindowId = GetCOREInterface()->GetMAXHWnd();
	//	widget.show((long)mainWindowId);
	//	return TRUE;
	//}
	return FALSE;
}
//
//ClassDesc2* GetClassDesc(int paramType)
//{
//	// TODO: Only the TYPE_MESH makes sense in a traditional
//	// import scenario.  
//	switch(paramType)
//	{
//	case TYPE_FLOAT:
//	case TYPE_INT:
//	case TYPE_BOOL:
//		return SpliceTranslationLayer<Control, float>::GetClassDesc();
//	case TYPE_POINT3:
//		return SpliceTranslationLayer<Control, Point3>::GetClassDesc();
//	//case TYPE_FRGBA:
//	//case TYPE_POINT4:
//	//	return SpliceTranslationLayer<Control, Point4>::GetClassDesc();
//	case TYPE_QUAT:
//		return SpliceTranslationLayer<Control, Quat>::GetClassDesc();
//	case TYPE_MATRIX3:
//		return SpliceTranslationLayer<Control, Matrix3>::GetClassDesc();
//	case TYPE_MESH:
//		return SpliceTranslationLayer<GeomObject, Mesh>::GetClassDesc();
//
//	}
//	return NULL;
//}

BOOL SpliceStaticFPInterface::ImportSpliceFile(const TSTR& file)
{
	ClassDesc2* pCD = SpliceTranslationLayer<GeomObject, Mesh>::GetClassDesc();
	BOOL res = FALSE;
	if (pCD != NULL)
	{
		//ClassDesc2* pCD = GetClassDesc(paramType);
		// No matter the kind of import, we always create a type mesh
		// This is because its the only type of object we can easily add
		// to the Scene graph, and is the only type that really makes sense
		Object* pRef = reinterpret_cast<Object*>(pCD->Create(TRUE));

		SpliceTranslationFPInterface* pSpliceInterface = GetSpliceInterface(pRef);
		if (pSpliceInterface != NULL)
			res = pSpliceInterface->LoadFromFile(file, true);

		if (!res)
			pRef->MaybeAutoDelete();
		else {
			INode* pNode = GetCOREInterface()->CreateObjectNode(pRef);
			MSTR name;
			name.FromACP(pSpliceInterface->GetKLOperatorName().data());
			pNode->SetName(name);
		}
	}
	return res;
}

BOOL SpliceStaticFPInterface::ExportSpliceFile(const MSTR& file, ReferenceTarget* spliceEntity) 
{
	SpliceTranslationFPInterface* pSpliceInterface = GetSpliceInterface(spliceEntity);
	if (pSpliceInterface == nullptr)
		return FALSE;

	CStr cFile = file.ToCStr();
	FabricSplice::DGGraph* pGraph = const_cast<FabricSplice::DGGraph*>(&pSpliceInterface->GetSpliceGraph());
	return pGraph->saveToFile(cFile.data());
}

int SpliceStaticFPInterface::GetGlobalKLOperatorCount()
{
	return FabricSplice::DGGraph::getGlobalKLOperatorCount();
}

MSTR SpliceStaticFPInterface::GetGlobalKLOperatorName(int index)
{
	const char* opName = FabricSplice::DGGraph::getGlobalKLOperatorName(index);
	MSTR rval;
	rval.FromACP(opName);
	return rval;
}

BOOL SpliceStaticFPInterface::LoadExtension( const MSTR& extension, const MSTR& version, bool reload)
{
	const FabricCore::Client* pClient = NULL;
	FECS_DGGraph_getClient(&pClient);

	// No client, nothing to do (the extension will be loaded when requested
	if(pClient == nullptr)
		return FALSE;

	CStr cExtension = extension.ToCStr();
	CStr cVersion = version.ToCStr();
	FEC_ClientLoadExtension(pClient->getFECClientRef(), cExtension.data(), cVersion.data(), reload);
	return TRUE;
}

int SpliceStaticFPInterface::EnableLogging(int loggers) {
	if (loggers & LOG_GENERIC)
		SetGenericLoggerEnabled(true);
	if (loggers & LOG_ERROR)
		SetErrorLoggerEnabled(true);
	if (loggers & LOG_COMPILER_ERROR)
		SetCompilerLoggerEnabled(true);
	if (loggers & LOG_KL_STATUS)
		SetKLStatusLoggerEnabled(true);
	if (loggers & LOG_KL_REPORTS)
		SetKLReportLoggerEnabled(true);

	m_FabrigLogging |= (loggers&LOG_ALL);
	return m_FabrigLogging;
}

int SpliceStaticFPInterface::DisableLogging(int loggers) {
	if (loggers & LOG_GENERIC)
		SetGenericLoggerEnabled(false);
	if (loggers & LOG_ERROR)
		SetErrorLoggerEnabled(false);
	if (loggers & LOG_COMPILER_ERROR)
		SetCompilerLoggerEnabled(false);
	if (loggers & LOG_KL_STATUS)
		SetKLStatusLoggerEnabled(false);
	if (loggers & LOG_KL_REPORTS)
		SetKLReportLoggerEnabled(false);

	m_FabrigLogging = m_FabrigLogging&~loggers;
	return m_FabrigLogging;
}

bool SpliceStaticFPInterface::GetSpliceRendering()
{
	return SpliceEvents::GetInstance()->ViewportRenderHooked();
}

void SpliceStaticFPInterface::SetSpliceRendering(bool isRendering)
{
	if (isRendering)
	{
		SpliceEvents::GetInstance()->HookViewportRender();
	}
	else
		SpliceEvents::GetInstance()->UnHookViewportRender();
}


bool SpliceStaticFPInterface::GetSpliceManip()
{
	return SpliceEvents::GetInstance()->MouseEventsHooked();
}

void SpliceStaticFPInterface::SetSpliceManip(bool isManipulating)
{
	if (isManipulating)
	{
		SpliceEvents::GetInstance()->HookMouseEvents();
	}
	else
		SpliceEvents::GetInstance()->UnHookMouseEvents();
}
