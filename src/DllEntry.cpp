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
	return 10;
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

void RestartRendering(void* /*param*/, NotifyInfo* /*info*/)
{
	GetCOREInterface()->EnableSceneRedraw();
}

__declspec( dllexport ) int LibInitialize(void)
{
    FabricSplice::Initialize();
	
	// setup the callback functions
	SetDefaultLogging();
	SpliceStaticFPInterface::GetInstance()->EnableLogging(SpliceStaticFPInterface::LOG_ALL);

	// We need to initialize our MaxScript exposure for all classes as well.
	SpliceTranslationLayer<Control, float>::InitMixinInterface();
	SpliceTranslationLayer<Control, Point3>::InitMixinInterface();
	SpliceTranslationLayer<Control, Quat>::InitMixinInterface();
	SpliceTranslationLayer<Control, Matrix3>::InitMixinInterface();
	SpliceTranslationLayer<OSModifier, Mesh>::InitMixinInterface();
	SpliceTranslationLayer<GeomObject, Mesh>::InitMixinInterface();

	

	// Fix potential deadlock on startup with ATi cards
	// and really fast machines.  The problem is that 
	// when we are starting, we may still be initializing
	// our OGL Window.  In the glViewport function, we have
	// a 3-way tie for responsible lock, as we try to
	// trigger a render and block on drawMutex while the
	// render thread is waiting in glViewport, and our OGLFunc
	// is stuck waiting on GetMessage.  All in all, a giant
	// cluster-fuck.
	//GetCOREInterface()->DisableSceneRedraw();
	// Once the system has started (and OGL threads have init'ed), allow rendering
	//RegisterNotification(RestartRendering, NULL, NOTIFY_SYSTEM_STARTUP);
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