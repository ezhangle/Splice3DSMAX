/**********************************************************************
*<
FILE: DllEntry.cpp

DESCRIPTION:	Returns the ClassDesc (factory pattern) classes 
				to allow Max to create our plugins

CREATED BY:		Ingenuity Engine
				Coded by Stephen Taylor, T&A Development

*>
**********************************************************************/

#include "StdAfx.h"
#include "SpliceStaticFPInterface.h"
#include <notify.h>
#include <MaxScript/MaxScript.h>
#include "SpliceEvents.h"

// This function is called by Windows when the DLL is loaded.  This 
// function may also be called many times during time critical operations
// like rendering.  Therefore developers need to be careful what they
// do inside this function.  In the code below, note how after the DLL is
// loaded the first time only a few statements are executed.
BOOL WINAPI DllMain(HINSTANCE hinstDLL,ULONG fdwReason,LPVOID /*lpvReserved*/)
{
	if( fdwReason == DLL_PROCESS_ATTACH )
	{
		hInstance = hinstDLL;
		// Hang on to this DLL's instance handle.
		DisableThreadLibraryCalls(hinstDLL);
	}
	return(TRUE);
}


extern ClassDesc2* GetDynPBlockClassDesc();;
extern DynPBCustAttrClassDesc* GetSpliceControllerDesc();

#ifdef __cplusplus
extern "C" {  // only need to export C interface if
	// used by C++ source code
#endif

// This function returns a string that describes the DLL and where the user
// could purchase the DLL if they don't have it.
__declspec( dllexport ) const TCHAR* LibDescription()
{
	return GetString(IDS_LIBDESCRIPTION);
}

// This function returns the number of plug-in classes this DLL
__declspec( dllexport ) int LibNumberClasses()
{
	return 11;
}

// This function returns the number of plug-in classes this DLL

__declspec( dllexport ) ClassDesc* LibClassDesc(int i)
{
	switch(i) {
	case 0: return SpliceTranslationLayer<Control, float>::GetClassDesc();
	case 1: return SpliceTranslationLayer<Control, Point3>::GetClassDesc();
	case 2: return SpliceTranslationLayer<Control, Quat>::GetClassDesc();
	case 3: return SpliceTranslationLayer<Control, Matrix3>::GetClassDesc();
	case 8: return SpliceTranslationLayer<OSModifier, Mesh>::GetClassDesc();
	case 9: return SpliceTranslationLayer<GeomObject, Mesh>::GetClassDesc();
	case 10: return SpliceTranslationLayer<ReferenceTarget, int>::GetClassDesc();
//	case 3: return GetDynPBlockClassDesc();
		default: return 0;
	}
	return NULL;
}

// This function returns a pre-defined constant indicating the version of 
// the system under which it was compiled.  It is used to allow the system
// to catch obsolete DLLs.
__declspec( dllexport ) ULONG LibVersion()
{
	return VERSION_3DSMAX;
}

// By returning FALSE from CanAutoDefer, we opt
// out of 3ds Max's deferred loading. 
__declspec( dllexport ) ULONG CanAutoDefer()
{
	return FALSE;
}

// Initialize things
void OnStartup(void* /*param*/, NotifyInfo* /*info*/)
{
	FabricSplice::Initialize();

	// setup the callback functions
	InitLoggingTimer();
	SpliceStaticFPInterface::GetInstance()->EnableLogging(SpliceStaticFPInterface::LOG_ALL);

	// Magic initialization stuff for maxscript.
	static bool menus_setup = false;
	if (!menus_setup) {
		init_MAXScript();
		menus_setup = TRUE;

		// On first run, evaluate the script that defines our function
		char* mxsMenuSetup = nullptr;
		size_t buffSize = 0;
		if (_dupenv_s(&mxsMenuSetup, &buffSize, "SPLICE3DSMAXDIR") == 0) {
			MSTR mxsMenuSetupPath = MSTR::FromACP(mxsMenuSetup, buffSize);
			mxsMenuSetupPath = mxsMenuSetupPath + _T("SetupMenu.ms");
			filein_script(mxsMenuSetupPath.data());
			free(mxsMenuSetup);
		}
	}

	// Cleanup once callback is done.
	UnRegisterNotification(OnStartup, nullptr);
}

// Clean up splice, and unhook all notifications.
void OnShutdown(void* param, NotifyInfo* info)
{
	// On shutdown we release all info
	SpliceStaticFPInterface::GetInstance()->DestroyClient(true);
	UnRegisterNotification(OnShutdown, nullptr);
	// Cleanup once callback is done.
}

__declspec( dllexport ) int LibInitialize(void)
{
	// We need to initialize our MaxScript exposure for all classes as well.
	SpliceTranslationLayer<Control, float>::InitMixinInterface();
	SpliceTranslationLayer<Control, Point3>::InitMixinInterface();
	SpliceTranslationLayer<Control, Quat>::InitMixinInterface();
	SpliceTranslationLayer<Control, Matrix3>::InitMixinInterface();
	SpliceTranslationLayer<OSModifier, Mesh>::InitMixinInterface();
	SpliceTranslationLayer<GeomObject, Mesh>::InitMixinInterface();
	SpliceTranslationLayer<ReferenceTarget, int>::InitMixinInterface();

	// Force init/cleanup of client
	RegisterNotification(OnStartup, NULL, NOTIFY_SYSTEM_STARTUP);
	RegisterNotification(OnShutdown, NULL, NOTIFY_SYSTEM_SHUTDOWN);
	return TRUE;
}

// This function is called once, just before the plugin is unloaded. 
// Perform one-time plugin un-initialization in this method."
// The system doesn't pay attention to a return value.
__declspec( dllexport ) int LibShutdown(void)
{
	// Disable logging, the MaxScript system has already exit
	FabricSplice::Logging::setLogFunc(nullptr);
	FabricSplice::Logging::setLogErrorFunc(nullptr);
	FabricSplice::Logging::setCompilerErrorFunc(nullptr);
	FabricSplice::Logging::setKLReportFunc(nullptr);
	FabricSplice::Logging::setKLStatusFunc(nullptr);

    FabricSplice::Finalize();
	return TRUE;
}

#ifdef __cplusplus
}
#endif