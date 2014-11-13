
#pragma once

#include "ifnpub.h"

#define ISPLICE_STATIC_INTERFACE Interface_ID(0xb7750cf, 0x7e1b3bed)

class SpliceStaticFPInterface : public FPStaticInterface {
	
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
		fn_showSceneGraphEditor,
		
		fn_importSpliceFile,
		fn_exportSpliceFile,

		fn_getGlobalOperatorCount,
		fn_getGlobalOperatorName,
		fn_loadFabricExtension,

		fn_enableLogging,
		fn_disableLogging,

		prop_getSpliceRendering,
		prop_setSpliceRendering,
		prop_getSpliceManip,
		prop_setSpliceManip,

		loggingEnums
	};
	BEGIN_FUNCTION_MAP	
		FN_0(fn_showSceneGraphEditor, TYPE_BOOL, ShowSceneGraphEditor);
		FN_1(fn_importSpliceFile, TYPE_BOOL, ImportSpliceFile, TYPE_TSTR_BV);
		FN_2(fn_exportSpliceFile, TYPE_BOOL, ExportSpliceFile, TYPE_TSTR_BV, TYPE_REFTARG);
		
		FN_0(fn_getGlobalOperatorCount, TYPE_INT, GetGlobalKLOperatorCount);
		FN_1(fn_getGlobalOperatorName, TYPE_TSTR_BV, GetGlobalKLOperatorName, TYPE_INDEX);
		
		FN_3(fn_loadFabricExtension, TYPE_BOOL, LoadExtension, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_bool);

		FN_1(fn_enableLogging, TYPE_INT, EnableLogging, TYPE_ENUM);
		FN_1(fn_disableLogging, TYPE_INT, DisableLogging, TYPE_ENUM);

		PROP_FNS(prop_getSpliceRendering, GetSpliceRendering, prop_setSpliceRendering, SetSpliceRendering, TYPE_bool);
		PROP_FNS(prop_getSpliceManip, GetSpliceManip, prop_setSpliceManip, SetSpliceManip, TYPE_bool);


	END_FUNCTION_MAP

	// Implement the functions exposed above
	BOOL ShowSceneGraphEditor();

	void ShowKLEditor();

	BOOL ImportSpliceFile(const MSTR& file);
	BOOL ExportSpliceFile(const MSTR& file, ReferenceTarget* spliceEntity);

	int GetGlobalKLOperatorCount();
	MSTR GetGlobalKLOperatorName(int index);

	BOOL LoadExtension(const MSTR& extension, const MSTR& version, bool reload);

	// Set the currently enabled/disabled logging
	int EnableLogging(int v);
	int DisableLogging(int v);

	bool GetSpliceRendering();
	void SetSpliceRendering(bool isRendering);

	bool GetSpliceManip();
	void SetSpliceManip(bool isManipulating);

	// General purpose exposure
	static SpliceStaticFPInterface* GetInstance();

private:

	// Declares the constructor of this class. It is private so a single static instance of this class
	// may be instantiated.
	DECLARE_DESCRIPTOR(SpliceStaticFPInterface);
};