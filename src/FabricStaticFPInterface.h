
#pragma once

#include "ifnpub.h"

#define IFabric_STATIC_INTERFACE Interface_ID(0xb7750cf, 0x7e1b3bed)

class FabricStaticFPInterface : public FPStaticInterface {
	
	// This paramter stores what logging functions have been turned on so far.
	int m_FabrigLogging;

public:

	// The follow exposes our functions to Max
	enum FN_IDS {
		// System management callbacks
		fn_importFabricFile,

		fn_loadFabricExtension,
		fn_registerFabricExtension,

		fn_setReportLevelMax,

		fn_destroyClient,

		prop_getFabricRendering,
		prop_setFabricRendering,
		prop_getFabricManip,
		prop_setFabricManip,

		prop_getContextId,
		prop_setContextId,

		reportEnums
	};
	BEGIN_FUNCTION_MAP	
		FN_1(fn_importFabricFile, TYPE_BOOL, ImportFabricFile, TYPE_TSTR_BV);
		
		FN_3(fn_loadFabricExtension, TYPE_BOOL, LoadExtension, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_bool);
		FN_7(fn_registerFabricExtension, TYPE_bool, RegisterExtension, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_TAB_BR, TYPE_TSTR_TAB_BR, TYPE_bool, TYPE_bool);

		FN_1(fn_setReportLevelMax, TYPE_BOOL, SetReportLevelMax, TYPE_ENUM);

		VFN_1(fn_destroyClient, DestroyClient, TYPE_bool);

		PROP_FNS(prop_getFabricRendering, GetFabricRendering, prop_setFabricRendering, SetFabricRendering, TYPE_bool);
		PROP_FNS(prop_getFabricManip, GetFabricManip, prop_setFabricManip, SetFabricManip, TYPE_bool);
		PROP_FNS(prop_getContextId, GetContextID, prop_setContextId, SetContextID, TYPE_TSTR_BV);


	END_FUNCTION_MAP

	// Implement the functions exposed above
	BOOL ImportFabricFile(const MSTR& file);

	BOOL LoadExtension(const MSTR& extension, const MSTR& version, bool reload);
	bool RegisterExtension(const MSTR& extension, const MSTR& version, const MSTR& override, const Tab<MSTR*>& files, const Tab<MSTR*>& contents, bool load, bool reload);
	bool RegisterExtension(const char* extension, const char* version, const char* override, std::vector<FEC_KLSourceFile>& fileHolder, bool load, bool reload);
	// Set the currently enabled/disabled logging
	int SetReportLevelMax(int v);

	void DestroyClient(bool force/*=false*/);

	bool GetFabricRendering();
	void SetFabricRendering(bool isRendering);

	bool GetFabricManip();
	void SetFabricManip(bool isManipulating);

	TSTR GetContextID();
	void SetContextID(const TSTR& context);

	// General purpose exposure
	static FabricStaticFPInterface* GetInstance();

private:

	// Declares the constructor of this class. It is private so a single static instance of this class
	// may be instantiated.
	DECLARE_DESCRIPTOR(FabricStaticFPInterface);
};