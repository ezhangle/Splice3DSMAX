#include "StdAfx.h"
#include "SpliceStaticFPInterface.h"
#include "MaxScript/MaxScript.h"
#include "SpliceEvents.h"
#include "FabricCore.h"
#include <fstream>      // std::ifstream

#if MAX_VERSION_MAJOR < 15
#define p_end end
#endif

FabricStaticFPInterface* FabricStaticFPInterface::GetInstance()
{
	// The single instance of this class
	// Initialize our instance.  This part is tells Max about the
	// functions exposed in our function map.
	static FabricStaticFPInterface _theInstance(
		// Describe our interface
		IFabric_STATIC_INTERFACE , _T("Fabric"), 0, NULL, FP_CORE,
		// Describe our function(s)
			FabricStaticFPInterface::fn_importFabricFile, _T("LoadFromFile"), 0, TYPE_BOOL, 0, 1,
				_M("file"),	0,	TYPE_TSTR_BV,
			
			FabricStaticFPInterface::fn_getGlobalOperatorCount, _T("GetGlobalOperatorCount"), 0, TYPE_INT, 0, 0,
			FabricStaticFPInterface::fn_getGlobalOperatorName, _T("GetGlobalOperatorName"), 0, TYPE_TSTR_BV, 0, 1,
				_M("index"),	0,	TYPE_INDEX,
			FabricStaticFPInterface::fn_loadFabricExtension, _T("LoadExtension"), 0, TYPE_BOOL, 0, 3,
				_M("extension"),	0,	TYPE_TSTR_BV,
				_M("version"),	0,	TYPE_TSTR_BV,
				_M("reload"),	0,	TYPE_bool,
			FabricStaticFPInterface::fn_registerFabricExtension, _T("RegisterExtension"), 0, TYPE_BOOL, 0, 6,
				_M("extension"),	0,	TYPE_TSTR_BV,
				_M("version"),	0,	TYPE_TSTR_BV,
				_M("versionOverride"),	0,	TYPE_TSTR_BV,
				_M("files"),	0,	TYPE_TSTR_TAB_BV,
				_M("load"),	0,	TYPE_bool,
				_M("reload"),	0,	TYPE_bool,

			FabricStaticFPInterface::fn_enableLogging, _T("EnableLoggers"), 0, TYPE_INT, 0, 1,
				_M("loggers"),	0,	TYPE_ENUM, FabricStaticFPInterface::loggingEnums,
			FabricStaticFPInterface::fn_disableLogging, _T("DisableLoggers"), 0, TYPE_INT, 0, 1,
				_M("loggers"),	0,	TYPE_ENUM, FabricStaticFPInterface::loggingEnums,

			FabricStaticFPInterface::fn_destroyClient, _T("DestroyClient"), 0, TYPE_INT, 0, 1,
				_M("force"),	0,	TYPE_BOOL, f_keyArgDefault, false,


		properties,
			FabricStaticFPInterface::prop_getFabricRendering,FabricStaticFPInterface::prop_setFabricRendering, _T("Rendering"), 0,TYPE_bool,
			FabricStaticFPInterface::prop_getFabricManip,FabricStaticFPInterface::prop_setFabricManip, _T("Manipulation"), 0,TYPE_bool,

		enums,
			FabricStaticFPInterface::loggingEnums, LOG_ENUM_COUNT,
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

BOOL FabricStaticFPInterface::ImportFabricFile(const TSTR& file)
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

		SpliceTranslationFPInterface* pFabricInterface = GetSpliceInterface(pRef);
		if (pFabricInterface != NULL)
			res = pFabricInterface->LoadFromFile(file, true);

		if (!res)
			pRef->MaybeAutoDelete();
		else {
			// Give the new node the name of the Fabric preset. 
			INode* pNode = GetCOREInterface()->CreateObjectNode(pRef);
			TSTR directory, filename, extension;
			SplitFilename (file, &directory, &filename, &extension);
			pNode->SetName(filename.ToWStr());
		}
	}
	return res;
}

BOOL FabricStaticFPInterface::ExportFabricFile(const MSTR& file, ReferenceTarget* FabricEntity) 
{
	SpliceTranslationFPInterface* pFabricInterface = GetSpliceInterface(FabricEntity);
	if (pFabricInterface == nullptr)
		return FALSE;

	CStr cFile = file.ToCStr();
	//FabricCore::DFGBinding* pGraph = const_cast<FabricCore::DFGBinding*>(&pFabricInterface->GetFabricGraph());
	return false ; //pGraph->saveToFile(cFile.data());
}

int FabricStaticFPInterface::GetGlobalKLOperatorCount()
{
	return 0; //FabricCore::DFGBinding::getGlobalKLOperatorCount();
}

MSTR FabricStaticFPInterface::GetGlobalKLOperatorName(int index)
{
	//const char* opName = FabricCore::DFGBinding::getGlobalKLOperatorName(index);
	//MSTR rval;
	//rval.FromACP(opName);
	//return rval;
	return _M("");
}

BOOL FabricStaticFPInterface::LoadExtension( const MSTR& extension, const MSTR& version, bool reload)
{
	//MAXFabric_CATCH_BEGIN

	CStr cExtension = extension.ToCStr();
	CStr cVersion = version.ToCStr();

	GetClient().loadExtension(cExtension.data(), cVersion.data(), reload);

	//// If the call failed for any reason, we need to clear the exception status from Fabric
	uint32_t length = FEC_GetLastExceptionLength();
	if ( length > 0 )
	{
		MSTR message = MSTR::FromACP(FEC_GetLastExceptionCString(), length);
		FEC_ClearLastException();
		throw UserThrownError(message.data(), FALSE);
	}

	//MAXFabric_CATCH_RETURN(false);
	return TRUE;
}

bool ReadFileIntoString(const char* file, std::string& outcontents) {
	std::ifstream is (file, std::ifstream::in);
	if (is) {
		// get length of file:
		is.seekg (0, is.end);
		std::streamoff length = is.tellg();
		is.seekg (0, is.beg);

		outcontents.resize(length);

		// read data as a block:
		is.read (const_cast<char*>(outcontents.data()), length);

		is.close();
		return true;
	}
	return false;
}

bool FabricStaticFPInterface::RegisterExtension( const MSTR& extension, const MSTR& version, const MSTR& override, const Tab<MSTR*>& files, bool load, bool reload )
{
	//const FabricCore::Client* pClient = NULL;
	//FECS_DGGraph_getClient(&pClient);

	//// No client, nothing to do (the extension will be loaded when requested
	//if(pClient == nullptr)
	//	return FALSE;

	//CStr cExtension = extension.ToCStr();
	//CStr cVersion = version.ToCStr();
	//CStr cOverride = version.ToCStr();
	//std::vector<std::string> fileNames;
	//std::vector<std::string> fileContents;
	//std::vector<FEC_KLSourceFile> fileHolder;

	//// Read file contents into the buffers
	//int numFiles = files.Count();
	//fileNames.resize(numFiles);
	//fileContents.resize(numFiles);
	//fileHolder.resize(numFiles);
	//for (int i = 0; i < numFiles; i++) {
	//	fileNames[i] = files[i]->ToACP();
	//	if (!ReadFileIntoString(fileNames[i].data(), fileContents[i]))
	//		return false;

	//	fileHolder[i].filenameCStr = fileNames[i].data();
	//	fileHolder[i].sourceCodeCStr = fileContents[i].data();
	//}

	//FEC_RegisterKLExtension(pClient->getFECClientRef(), cExtension, cVersion, cOverride, numFiles, &fileHolder[0], load, reload);

	//// If the call failed for any reason, we need to clear the exception status from Fabric
	//uint32_t length = FEC_GetLastExceptionLength();
	//if ( length > 0 )
	//{
	//	MSTR message = MSTR::FromACP(FEC_GetLastExceptionCString(), length);
	//	FEC_ClearLastException();
	//	throw UserThrownError(message.data(), FALSE);
	//}
	return true;
}


int FabricStaticFPInterface::EnableLogging(int loggers) {
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

int FabricStaticFPInterface::DisableLogging(int loggers) {
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

void FabricStaticFPInterface::DestroyClient(bool force) {
	SpliceEvents::ReleaseInstance();
	
	ReleaseAll();
}

bool FabricStaticFPInterface::GetFabricRendering()
{
	return SpliceEvents::GetInstance()->ViewportRenderHooked();
}

void FabricStaticFPInterface::SetFabricRendering(bool isRendering)
{
	if (isRendering)
	{
		SpliceEvents::GetInstance()->HookViewportRender();
	}
	else
		SpliceEvents::GetInstance()->UnHookViewportRender();
}


bool FabricStaticFPInterface::GetFabricManip()
{
	return SpliceEvents::GetInstance()->MouseEventsHooked();
}

void FabricStaticFPInterface::SetFabricManip(bool isManipulating)
{
	if (isManipulating)
	{
		SpliceEvents::GetInstance()->HookMouseEvents();
	}
	else
		SpliceEvents::GetInstance()->UnHookMouseEvents();
}

