
#pragma once

#include "ifnpub.h"

#define ISPLICE_STATIC_INTERFACE Interface_ID(0xb7750cf, 0x7e1b3bed)

class SpliceStaticFPInterface : public FPStaticInterface {
public:

	enum ENUM_IDS {
		en_axis,
		en_commandMode
	};
	// The follow exposes our functions to Max
	enum FN_IDS {
		// System management callbacks
		fn_showSceneGraphEditor,
		fn_showKlEditor,
		fn_loadSpliceFile,
		fn_getGlobalOperatorCount,
		fn_getGlobalOperatorName,
		fn_reloadFabricExtension,

		prop_getSpliceRendering,
		prop_setSpliceRendering,
		prop_getSpliceManip,
		prop_setSpliceManip
	};
	BEGIN_FUNCTION_MAP	
		FN_0(fn_showSceneGraphEditor, TYPE_BOOL, ShowSceneGraphEditor);
		VFN_0(fn_showKlEditor, ShowKLEditor);
		FN_1(fn_loadSpliceFile, TYPE_BOOL, LoadSpliceFile, TYPE_TSTR_BV);
		FN_0(fn_getGlobalOperatorCount, TYPE_INT, GetGlobalKLOperatorCount);
		FN_1(fn_getGlobalOperatorName, TYPE_TSTR_BV, GetGlobalKLOperatorName, TYPE_INDEX);
		FN_3(fn_reloadFabricExtension, TYPE_BOOL, LoadExtension, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_bool);

		PROP_FNS(prop_getSpliceRendering, GetSpliceRendering, prop_setSpliceRendering, SetSpliceRendering, TYPE_bool);
		PROP_FNS(prop_getSpliceManip, GetSpliceManip, prop_setSpliceManip, SetSpliceManip, TYPE_bool);
	END_FUNCTION_MAP

	// Implement the functions exposed above
	BOOL ShowSceneGraphEditor();

	void ShowKLEditor();

	BOOL LoadSpliceFile(const MSTR& file);

	int GetGlobalKLOperatorCount();
	MSTR GetGlobalKLOperatorName(int index);

	BOOL LoadExtension(const MSTR& extension, const MSTR& version, bool reload);

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