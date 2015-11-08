
#pragma once

#include "ifnpub.h"

#define IFabric_STATIC_INTERFACE Interface_ID(0xb7750cf, 0x7e1b3bed)

class FabricStaticFPInterface : public FPStaticInterface {
	
	// This paramter stores what logging functions have been turned on so far.
	int m_FabrigLogging;

public:

	enum LOGGING_IDS {
		LOG_GENERIC = 1 << 0,
		LOG_ERROR = 1 << 1,
		LOG_COMPILER_ERROR = 1 << 2,
		LOG_KL_REPORTS = 1 << 3,
		LOG_KL_STATUS = 1 << 4,

		LOG_ALL = LOG_GENERIC|LOG_ERROR|LOG_COMPILER_ERROR|LOG_KL_REPORTS|LOG_KL_STATUS,
		LOG_ENUM_COUNT = 6
	};


	// The follow exposes our functions to Max
	enum FN_IDS {
		// System management callbacks
		fn_importFabricFile,
		fn_exportFabricFile,

		fn_getGlobalOperatorCount,
		fn_getGlobalOperatorName,
		fn_loadFabricExtension,
		fn_registerFabricExtension,

		fn_enableLogging,
		fn_disableLogging,

		fn_destroyClient,

		prop_getFabricRendering,
		prop_setFabricRendering,
		prop_getFabricManip,
		prop_setFabricManip,

		loggingEnums
	};
	BEGIN_FUNCTION_MAP	
		FN_1(fn_importFabricFile, TYPE_BOOL, ImportFabricFile, TYPE_TSTR_BV);
		FN_2(fn_exportFabricFile, TYPE_BOOL, ExportFabricFile, TYPE_TSTR_BV, TYPE_REFTARG);
		
		FN_0(fn_getGlobalOperatorCount, TYPE_INT, GetGlobalKLOperatorCount);
		FN_1(fn_getGlobalOperatorName, TYPE_TSTR_BV, GetGlobalKLOperatorName, TYPE_INDEX);
		
		FN_3(fn_loadFabricExtension, TYPE_BOOL, LoadExtension, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_bool);
		FN_6(fn_registerFabricExtension, TYPE_bool, RegisterExtension, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_TAB_BR, TYPE_bool, TYPE_bool);

		FN_1(fn_enableLogging, TYPE_INT, EnableLogging, TYPE_ENUM);
		FN_1(fn_disableLogging, TYPE_INT, DisableLogging, TYPE_ENUM);

		VFN_1(fn_destroyClient, DestroyClient, TYPE_bool);

		PROP_FNS(prop_getFabricRendering, GetFabricRendering, prop_setFabricRendering, SetFabricRendering, TYPE_bool);
		PROP_FNS(prop_getFabricManip, GetFabricManip, prop_setFabricManip, SetFabricManip, TYPE_bool);


	END_FUNCTION_MAP

	// Implement the functions exposed above
	void ShowKLEditor();

	BOOL ImportFabricFile(const MSTR& file);
	BOOL ExportFabricFile(const MSTR& file, ReferenceTarget* FabricEntity);

	int GetGlobalKLOperatorCount();
	MSTR GetGlobalKLOperatorName(int index);

	BOOL LoadExtension(const MSTR& extension, const MSTR& version, bool reload);
	bool RegisterExtension(const MSTR& extension, const MSTR& version, const MSTR& override, const Tab<MSTR*>& files, bool load, bool reload);

	// Set the currently enabled/disabled logging
	int EnableLogging(int v);
	int DisableLogging(int v);

	void DestroyClient(bool force/*=false*/);

	bool GetFabricRendering();
	void SetFabricRendering(bool isRendering);

	bool GetFabricManip();
	void SetFabricManip(bool isManipulating);

	// General purpose exposure
	static FabricStaticFPInterface* GetInstance();

private:

	// Declares the constructor of this class. It is private so a single static instance of this class
	// may be instantiated.
	DECLARE_DESCRIPTOR(FabricStaticFPInterface);
};