#pragma once

#include <ifnpub.h>

#define ISPLICE__INTERFACE Interface_ID(0x5a5f19c9, 0x25881449)

extern void DoShowKLEditor(ReferenceTarget* pTarget);

// We need to make this function a templated type, just so
// we can get multiple compilations for our different clients.
// This is necessary, as there needs to be a unique instance of
// the class created for each client, and each implementation
// of GetDesc needs to return the unique type.
class SpliceTranslationFPInterface : public FPMixinInterface {
public:
	// The follow exposes our functions to Max
	enum FN_IDS {
		// System management callbacks
		fn_showSceneGraphEditor,

		fn_setSpliceGraph,
		
		fn_getOpCount,
		fn_getOpName,

		fn_getKLCode,
		fn_getKLOpName,
		fn_setKLCode,
	
		fn_createPort,
		fn_deletePort,
		fn_getPortName,
		fn_setPortName,
		fn_getPortType,
		fn_isPortArray,

		fn_getMaxConnectedType,
		fn_setMaxConnectedType,
		fn_getLegalMaxTypes,

		prop_getKLFile,
		prop_setKLFile,

		prop_numPorts,
		prop_portNames,

		prop_getOutPortName,
		prop_setOutPortName,

		prop_getOutPortArrayIdx,
		prop_setOutPortArrayIdx,

		num_params
	};

	BEGIN_FUNCTION_MAP	
		FN_0(fn_showSceneGraphEditor, TYPE_BOOL, ShowSceneGraphEditor);

		FN_1(fn_setSpliceGraph, TYPE_BOOL, SetSpliceGraph, TYPE_REFTARG);

		FN_0(fn_getOpCount, TYPE_INT, GetOperatorCount);
		FN_1(fn_getOpName, TYPE_TSTR_BV, GetOperatorNameMSTR, TYPE_INT);

		FN_2(fn_setKLCode, TYPE_TSTR_BV, SetKLCodeMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV);
		FN_0(fn_getKLCode, TYPE_TSTR_BV, GetKLCodeMSTR);
		FN_0(fn_getKLOpName, TYPE_TSTR_BV, GetKLOperatorNameMSTR);

		FN_5(fn_createPort, TYPE_INT, CreatePortMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_INT, TYPE_BOOL, TYPE_TSTR_BV)
		FN_1(fn_deletePort, TYPE_bool, DeletePort, TYPE_INT);
		FN_1(fn_getPortName, TYPE_TSTR_BV, GetPortNameMSTR, TYPE_INT);
		VFN_2(fn_setPortName, SetPortNameMSTR, TYPE_INT, TYPE_TSTR_BV);
		FN_1(fn_getPortType, TYPE_TSTR_BV, GetPortTyeMSTR, TYPE_INT);
		FN_1(fn_isPortArray, TYPE_bool, IsPortArray, TYPE_INT);

		FN_1(fn_getMaxConnectedType, TYPE_INT, GetMaxConnectedType, TYPE_INT);
		FN_2(fn_setMaxConnectedType, TYPE_INT, SetMaxConnectedType, TYPE_INT, TYPE_INT);
		FN_1(fn_getLegalMaxTypes, TYPE_BITARRAY_BV, GetLegalMaxTypes, TYPE_INT);

		// Properties 

		PROP_FNS(prop_getKLFile, GetKLFileMSTR, prop_setKLFile, SetKLFileMSTR, TYPE_TSTR_BV)
		RO_PROP_FN(prop_numPorts, GetNumPorts, TYPE_INT)
		PROP_FNS(prop_getOutPortName, GetOutPortNameMSTR, prop_setOutPortName, SetOutPortNameMSTR, TYPE_TSTR_BV)
		PROP_FNS(prop_getOutPortArrayIdx, GetOutPortArrayIdx, prop_setOutPortArrayIdx, SetOutPortArrayIdx, TYPE_INT)

	END_FUNCTION_MAP

	// Implement the functions exposed above
	BOOL ShowSceneGraphEditor() { /* TODO */ return FALSE; };

	// Graph management functions

	virtual int GetOperatorCount() = 0;
	virtual std::string GetOperatorName(int i) = 0;

	virtual std::string GetKLCode() = 0;
	virtual std::string GetKLOperatorName() = 0;
	virtual std::string SetKLCode(const std::string& name, const std::string& script) = 0;

	// Get/Set a link to an external KL file
	virtual const char* GetKLFile() = 0;
	virtual std::string SetKLFile(const char* file) = 0;

	// Port creation/management

	// Get the number of ports on this graph
	virtual int GetNumPorts() = 0;
	//virtual Tab<TSTR*> GetPortNames()=0;

	// Splice port management
	// Create a new port.  A matching Max parameter 
	// will be added as well of maxType, if MaxType is -1 
	virtual int CreatePort(const char* klType, const char* name, int maxType=-1, bool isArray=false, const char* inExtension=nullptr) = 0;
	// Remove specified port and matching max parameter
	virtual bool DeletePort(int i) = 0;
	// Get name of port
	virtual const char* GetPortName(int i) = 0;
	virtual void SetPortName(int i, const char* name) = 0;
	virtual const char* GetPortType(int i) = 0;
	// Returns if the in port is an array type or not
	virtual bool IsPortArray(int i)=0;

	virtual const char* GetOutPortName() = 0;
	virtual bool SetOutPortName(const char* name) = 0;

	// If our out-port references a splice Array Type, return the index we fetch
	virtual int GetOutPortArrayIdx() = 0;
	// Set the index.  If idx == -1, set our out port to not be an array
	virtual void SetOutPortArrayIdx(int idx) = 0;

	//virtual MSTR GetPortMaxValue(int i);
	//virtual MSTR GetPortMinValue(int i);

	virtual int GetMaxConnectedType(int i) = 0;
	virtual int SetMaxConnectedType(int i, int type) = 0;
	virtual BitArray GetLegalMaxTypes(int i) = 0;

	// Get the fabric graph driving this max class.
	virtual const FabricSplice::DGGraph& GetSpliceGraph() = 0;
	
	// Set this instance to use the splice graph on the rtarg instance
	// This can be useful when setting a group of Splice instances to
	// all share a single graph.
	BOOL SetSpliceGraph(ReferenceTarget* rtarg);

	// Allow others to set the splice graph etc we are using.
	// These functions are not called directly from MaxScript, instead
	// they are used by the static interface
	virtual void SetSpliceGraph(const FabricSplice::DGGraph& graph) = 0;
	virtual void SetOutPort(const FabricSplice::DGPort& port) = 0;

	// Show the MaxScript-based editor
	void ShowKLEditor(ReferenceTarget* pTarget)	{ DoShowKLEditor(pTarget); }

protected:

#pragma region MSTR<->CStr conversion

	// Our MSTR version of the function converts to CStr and passes on to real handler
	inline MSTR ToMSTR(const char* v, int defValue)
	{
		MSTR resMSTR; 
		if (v != NULL) 
			resMSTR = MSTR::FromACP(v, strlen(v));
		else 
			resMSTR = GetString(defValue);
		return resMSTR;
	}
	inline MSTR ToMSTR(std::string v, int defValue) { return ToMSTR(v.data(), defValue); }

#define MSTR_GETTER(fnName, defValue) \
	MSTR fnName##MSTR() { \
		return ToMSTR(fnName(), defValue); \
	}
#define MSTR_SETTER(fnName) \
	void fnName##MSTR(const MSTR& inVal) { \
		CStr inValCStr = inVal.ToCStr(); \
		fnName(inValCStr); }

	// TODO: Add default vals for these
	MSTR_GETTER(GetKLOperatorName, 0);
	
	MSTR_GETTER(GetKLFile, 0);
	MSTR_SETTER(SetKLFile);

	MSTR_GETTER(GetOutPortName, 0);
	MSTR_SETTER(SetOutPortName);
	
	MSTR GetKLCodeMSTR() { return ToMSTR(GetKLCode().data(), 0); }
	MSTR SetKLCodeMSTR(MSTR name, MSTR script) { 
		return ToMSTR(SetKLCode(name.ToCStr().data(), script.ToCStr().data()).data(), 0); 
	}

	MSTR GetOperatorNameMSTR(int i) { return ToMSTR(GetOperatorName(i), 0); }
	MSTR GetPortNameMSTR(int i)	{ return ToMSTR(GetPortName(i), 0); }
	void SetPortNameMSTR(int i, MSTR name)	{ SetPortName(i, name.ToCStr()); }
	MSTR GetPortTyeMSTR(int i)	{ return ToMSTR(GetPortType(i), 0); }

	int CreatePortMSTR(const MSTR& name, const MSTR& type, int maxType, bool isArray, const MSTR& inExtension)
	{
		return CreatePort(name.ToCStr().data(), type.ToCStr().data(), maxType, isArray, inExtension.ToCStr().data());
	}
#pragma endregion
};


// This templated static function creates a static
// descriptor per templated interface class
template<typename TBaseClass, typename TResultType>
FPInterfaceDesc* GetDescriptor()
{
	// The single instance of this class
	// Initialize our instance.  This part is tells Max about the
	// functions exposed in our function map.
	static FPInterfaceDesc _ourDesc(
		// Describe our interface
		ISPLICE__INTERFACE, 
		_T("CreationPlatform"), 
		0, 
		SpliceTranslationLayer<TBaseClass, TResultType>::GetClassDesc(), 
		0,
		// Describe our function(s)
			SpliceTranslationFPInterface::fn_showSceneGraphEditor, _T("ShowSceneGraphEditor"), 0, TYPE_BOOL, 0, 0, 

			SpliceTranslationFPInterface::fn_setSpliceGraph, _T("SetSpliceGraph"), 0, TYPE_BOOL, 0, 1,
				_M("source"),	0,	TYPE_REFTARG,

			SpliceTranslationFPInterface::fn_getOpCount, _T("GetOperatorCount"), 0, TYPE_INT, 0, 0, 
			SpliceTranslationFPInterface::fn_getOpName, _T("GetOperatorName"), 0, TYPE_TSTR_BV, 0, 1, 
				_M("index"),	0,	TYPE_INDEX,

			SpliceTranslationFPInterface::fn_getKLCode, _T("GetKLCode"), 0, TYPE_TSTR_BV, 0, 0, 
			SpliceTranslationFPInterface::fn_getKLOpName, _T("GetKLOperatorName"), 0, TYPE_TSTR_BV, 0, 0, 
			SpliceTranslationFPInterface::fn_setKLCode, _T("SetKLCode"), 0, TYPE_TSTR_BV, 0, 2,
				_M("name"),		0,	TYPE_TSTR_BV,
				_M("script"),	0,	TYPE_TSTR_BV,

			SpliceTranslationFPInterface::fn_createPort, _T("CreatePort"), 0, TYPE_INDEX, 0, 5,
				_M("name"),			0,	TYPE_TSTR_BV,
				_M("fabricType"),	0,	TYPE_TSTR_BV,
				_M("maxType"),		0,	TYPE_INT,
				_M("isArray"),		0,	TYPE_bool, f_keyArgDefault, false,
				_M("Extension"),	0,	TYPE_TSTR_BV, f_keyArgDefault, MSTR(),

			SpliceTranslationFPInterface::fn_deletePort, _T("DeletePort"), 0, TYPE_bool, 0, 1,
				_M("portIndex"),	0,	TYPE_INDEX,

			SpliceTranslationFPInterface::fn_getPortName, _T("GetPortName"), 0, TYPE_TSTR_BV, 0, 1,
				_M("portIndex"),	0,	TYPE_INDEX,
			SpliceTranslationFPInterface::fn_setPortName, _T("SetPortName"), 0, TYPE_VOID, 0, 2,
				_M("portIndex"),	0,	TYPE_INDEX,
				_M("name"),			0,	TYPE_TSTR_BV,
			SpliceTranslationFPInterface::fn_getPortType, _T("GetPortType"), 0, TYPE_TSTR_BV, 0, 1,
				_M("portIndex"),	0,	TYPE_INDEX,
			SpliceTranslationFPInterface::fn_isPortArray, _T("IsPortArray"), 0, TYPE_bool, 0, 1,
				_M("portIndex"),	0,	TYPE_INDEX,

			SpliceTranslationFPInterface::fn_getMaxConnectedType, _T("GetMaxType"), 0, TYPE_INT, 0, 1,
				_M("portIndex"),	0,	TYPE_INDEX,
			SpliceTranslationFPInterface::fn_setMaxConnectedType, _T("SetMaxType"), 0, TYPE_INT, 0, 2,
				_M("portIndex"),	0,	TYPE_INDEX,
				_M("maxType"),		0,	TYPE_INT,
			SpliceTranslationFPInterface::fn_getLegalMaxTypes, _T("GetLegalMaxTypes"), 0, TYPE_BITARRAY, 0, 1,
				_M("portIndex"),	0,	TYPE_INDEX,
		properties,
			SpliceTranslationFPInterface::prop_getKLFile, SpliceTranslationFPInterface::prop_setKLFile, _T("KLFile"), 0, TYPE_TSTR_BV,
			SpliceTranslationFPInterface::prop_numPorts, FP_NO_FUNCTION, _T("NumPorts"), 0, TYPE_INT,
			SpliceTranslationFPInterface::prop_getOutPortName, SpliceTranslationFPInterface::prop_setOutPortName, _T("OutPort"), 0, TYPE_TSTR_BV,
			SpliceTranslationFPInterface::prop_getOutPortArrayIdx, SpliceTranslationFPInterface::prop_setOutPortArrayIdx, _T("OutPortIndex"), 0, TYPE_INT,
		p_end
		);
	return &_ourDesc;
}