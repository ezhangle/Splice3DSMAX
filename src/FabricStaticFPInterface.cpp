#include "StdAfx.h"
#include "FabricStaticFPInterface.h"
#include "MaxScript/MaxScript.h"
#include "FabricEvents.h"
#include "FabricCore.h"
#include <fstream>      // std::ifstream

#if MAX_VERSION_MAJOR < 15
#define p_end end
#endif

using namespace std;

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
				_M("file"),	0,	TYPE_TSTR,
			
			FabricStaticFPInterface::fn_loadFabricExtension, _T("LoadExtension"), 0, TYPE_BOOL, 0, 3,
				_M("extension"),	0,	TYPE_TSTR,
				_M("version"),	0,	TYPE_TSTR, f_keyArgDefault, EMPTY_STR,
				_M("reload"),	0,	TYPE_bool, f_keyArgDefault, false,
			FabricStaticFPInterface::fn_registerFabricExtension, _T("RegisterExtension"), 0, TYPE_BOOL, 0, 7,
				_M("extension"),	0,	TYPE_TSTR,
				_M("version"),	0,	TYPE_TSTR,
				_M("versionOverride"),	0,	TYPE_TSTR,
				_M("files"), 0, TYPE_TSTR_TAB_BV,
				_M("fileContents"), 0, TYPE_TSTR_TAB_BV, f_keyArgDefault, Tab<MSTR*>(),
				_M("load"),	0,	TYPE_bool,
				_M("reload"),	0,	TYPE_bool, f_keyArgDefault, false,

			FabricStaticFPInterface::fn_setReportLevelMax, _T("SetReportLevelMax"), 0, TYPE_BOOL, 0, 1,
				_M("level"),	0,	TYPE_ENUM, FabricStaticFPInterface::reportEnums,

			FabricStaticFPInterface::fn_destroyClient, _T("DestroyClient"), 0, TYPE_INT, 0, 1,
				_M("force"),	0,	TYPE_BOOL, f_keyArgDefault, false,


		properties,
			FabricStaticFPInterface::prop_getFabricRendering,FabricStaticFPInterface::prop_setFabricRendering, _T("Rendering"), 0,TYPE_bool,
			FabricStaticFPInterface::prop_getFabricManip,FabricStaticFPInterface::prop_setFabricManip, _T("Manipulation"), 0,TYPE_bool,
			FabricStaticFPInterface::prop_getContextId, FabricStaticFPInterface::prop_setContextId, _T("ContextId"), 0, TYPE_TSTR_BV,

		enums,
			FabricStaticFPInterface::reportEnums, 4,
				_T("Error"),	FabricCore::ReportLevel_Error,
				_T("Warning"),	FabricCore::ReportLevel_Warning,
				_T("Info"),		FabricCore::ReportLevel_Info,
				_T("Debug"),	FabricCore::ReportLevel_Debug,
		p_end
		);
	return &_theInstance;
}


BOOL FabricStaticFPInterface::ImportFabricFile(const TSTR& file)
{
	ClassDesc2* pCD = FabricTranslationLayer<GeomObject, Mesh>::GetClassDesc();
	BOOL res = FALSE;
	if (pCD != NULL)
	{
		//ClassDesc2* pCD = GetClassDesc(paramType);
		// No matter the kind of import, we always create a type mesh
		// This is because its the only type of object we can easily add
		// to the Scene graph, and is the only type that really makes sense
		Object* pRef = reinterpret_cast<Object*>(pCD->Create(TRUE));

		FabricTranslationFPInterface* pFabricInterface = GetFabricInterface(pRef);
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

BOOL FabricStaticFPInterface::LoadExtension( const MSTR& extension, const MSTR& version, bool reload)
{
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

bool FabricStaticFPInterface::RegisterExtension(const MSTR& extension, const MSTR& version, const MSTR& override, const Tab<MSTR*>& files, const Tab<MSTR*>& fileContents, bool load, bool reload)
{
	vector<FEC_KLSourceFile> fileHolder;
	vector<string> cFileNames;
	vector<string> cFileContents;

	// Read file contents into the buffers
	int numFiles = files.Count();
	fileHolder.resize(numFiles);
	cFileNames.resize(numFiles);
	cFileContents.resize(numFiles);
	for (int i = 0; i < numFiles; i++) {
		cFileNames[i] = files[i]->ToACP();
		if (fileContents.Count() > i)
			cFileContents[i] = fileContents[i]->ToACP();
		else
		{

			if (!ReadFileIntoString(cFileNames[i].data(), cFileContents[i]))
				return false;

		}

		fileHolder[i].filenameCStr = cFileNames[i].data();
		fileHolder[i].sourceCodeCStr = cFileContents[i].data();
	}

	return RegisterExtension(extension.ToCStr(), version.ToCStr(), override.ToCStr(), fileHolder, load, reload);
}

bool FabricStaticFPInterface::RegisterExtension( const char* extension, const char* version, const char* override, vector<FEC_KLSourceFile>& fileHolder, bool load, bool reload )
{
	FabricCore::Client client = GetClient();
	int numFiles = fileHolder.size();
	FabricCore::RegisterKLExtension(
		client,
		extension,
		version,
		override, // override
		numFiles,
		&fileHolder[0],
		load,  // load
		reload // reload
		);

	// If the call failed for any reason, we need to clear the exception status from Fabric
	uint32_t length = FEC_GetLastExceptionLength();
	if ( length > 0 )
	{
		MSTR message = MSTR::FromACP(FEC_GetLastExceptionCString(), length);
		FEC_ClearLastException();
		throw UserThrownError(message.data(), FALSE);
	}
	return true;
}


BOOL FabricStaticFPInterface::SetReportLevelMax(int level) {
	FabricCore::Client& client = GetClient(false);
	if (client.isValid())
	{
		client.setReportLevelMax(FabricCore::ReportLevel(level));
		return true;
	}
	return false;
}

void FabricStaticFPInterface::DestroyClient(bool force) {
	FabricEvents::ReleaseInstance();
	
	ReleaseAll();
}

bool FabricStaticFPInterface::GetFabricRendering()
{
	return FabricEvents::GetInstance()->ViewportRenderHooked();
}

void FabricStaticFPInterface::SetFabricRendering(bool isRendering)
{
	if (isRendering)
	{
		FabricEvents::GetInstance()->HookViewportRender();
	}
	else
		FabricEvents::GetInstance()->UnHookViewportRender();
}


bool FabricStaticFPInterface::GetFabricManip()
{
	return FabricEvents::GetInstance()->MouseEventsHooked();
}

void FabricStaticFPInterface::SetFabricManip(bool isManipulating)
{
	if (isManipulating)
	{
		FabricEvents::GetInstance()->HookMouseEvents();
	}
	else
		FabricEvents::GetInstance()->UnHookMouseEvents();
}


TSTR FabricStaticFPInterface::GetContextID()
{
	FabricCore::Client& client = GetClient(false);
	if (client.isValid())
		return MSTR::FromACP(client.getContextID());
	return _M("");
}

void FabricStaticFPInterface::SetContextID(const TSTR& context)
{
	FabricCore::Client& client = GetClient(false);
	if (client.isValid())
		throw UserThrownError(_M("Cannot set ContextID on running client"), FALSE);

	MAXSPLICE_CATCH_BEGIN
		GetClient(true, context.ToCStr().data());
	MAXSCRIPT_CATCH_END
}
