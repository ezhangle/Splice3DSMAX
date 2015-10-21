#pragma once

#include <ifnpub.h>

#define ISPLICE__INTERFACE Interface_ID(0x5a5f19c9, 0x25881449)

class DockableWindow;

// Call the given function name, passing a single argument pTarger, and optionally returning it's result
extern Value* CallMaxScriptFunction(MCHAR* function, ReferenceTarget* fnArgument, bool returnResult);
extern Value* CallMaxScriptFunction(MCHAR* function, Value* fnArgument, bool returnResult);

// We need to make this function a templated type, just so
// we can get multiple compilations for our different clients.
// This is necessary, as there needs to be a unique instance of
// the class created for each client, and each implementation
// of GetDesc needs to return the unique type.
class SpliceTranslationFPInterface : public FPMixinInterface {
private:
	Value* m_klEditor;

	DockableWindow* m_pDFGWidgetWindow;

public:

	SpliceTranslationFPInterface();
	~SpliceTranslationFPInterface();

	// The follow exposes our functions to Max
	enum FN_IDS {
		// System management callbacks
		fn_showDFGGraphEditor,

		fn_loadFromFile,
		fn_saveToFile,

		fn_restoreFromJson,
		fn_exportToJson,
		
		fn_addInputPort,
		fn_addOutputPort,
		fn_addIOPort,

		fn_removePortName,
		fn_getPortName,
		fn_setPortName,
		fn_getPortType,
		fn_isPortArray,

		fn_connectPorts,

		fn_getMaxConnectedType,
		fn_setMaxConnectedType,
		fn_getLegalMaxTypes,

		fn_setPortMetaData,
		fn_getPortMetaData,
		fn_setPortMinMax,
		fn_setPortValue,
		fn_getPortValue,

		// DFG-Specific functionality
		fn_newEmptyGraph,
		fn_newEmptyFunc,
		fn_addNodeFromPreset,

		fn_getFuncKLCode,
		fn_setFuncKLCode,

		prop_getPortCount,
		prop_portNames,

		prop_getOutPortName,
		prop_SetOutPort,

		//prop_getOutPortArrayIdx,
		//prop_setOutPortArrayIdx,

		//prop_getAllPortSignature,

		prop_klEditor,

		num_params
	};

	BEGIN_FUNCTION_MAP	
		FN_0(fn_showDFGGraphEditor, TYPE_BOOL, ShowDFGGraphEditor);

		FN_2(fn_loadFromFile, TYPE_bool, LoadFromFile, TYPE_FILENAME, TYPE_bool);
		FN_1(fn_saveToFile, TYPE_bool, SaveToFile, TYPE_FILENAME);

		FN_2(fn_restoreFromJson, TYPE_bool, RestoreFromJSONMSTR, TYPE_TSTR_BV, TYPE_bool);
		FN_0(fn_exportToJson, TYPE_TSTR_BV, ExportToJSONMSTR);

		FN_5(fn_addInputPort, TYPE_TSTR_BV, AddInputPortMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_INT, TYPE_BOOL, TYPE_TSTR_BV);
		FN_4(fn_addOutputPort, TYPE_TSTR_BV, AddOutputPortMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_BOOL, TYPE_TSTR_BV);
		FN_5(fn_addIOPort, TYPE_TSTR_BV, AddIOPortMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_INT, TYPE_BOOL, TYPE_TSTR_BV);

		FN_1(fn_removePortName, TYPE_bool, RemovePortMSTR, TYPE_TSTR_BV);
		FN_1(fn_getPortName, TYPE_TSTR_BV, GetPortNameMSTR, TYPE_INT);
		FN_2(fn_setPortName, TYPE_TSTR_BV, SetPortNameMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV);
		FN_1(fn_getPortType, TYPE_TSTR_BV, GetPortTyeMSTR, TYPE_TSTR_BV);
		FN_1(fn_isPortArray, TYPE_bool, IsPortArrayMSTR, TYPE_TSTR_BV);

		FN_5(fn_connectPorts, TYPE_bool, ConnectPortMSTR, TYPE_TSTR_BV, TYPE_REFTARG, TYPE_TSTR_BV, TYPE_INT, TYPE_bool);
		
		FN_1(fn_getMaxConnectedType, TYPE_INT, GetMaxConnectedTypeMSTR, TYPE_TSTR_BV);
		FN_2(fn_setMaxConnectedType, TYPE_INT, SetMaxConnectedTypeMSTR, TYPE_TSTR_BV, TYPE_INT);
		FN_1(fn_getLegalMaxTypes, TYPE_BITARRAY_BV, GetLegalMaxTypesMSTR, TYPE_TSTR_BV);

		FN_4(fn_setPortMetaData, TYPE_bool, SetPortMetaDataMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_bool);
		FN_2(fn_getPortMetaData, TYPE_TSTR_BV, GetPortMetaDataMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV);

		FN_2(fn_setPortValue, TYPE_bool, SetPortValueMSTR, TYPE_TSTR_BV, TYPE_FPVALUE);
		FN_1(fn_getPortValue, TYPE_FPVALUE_BR, GetPortValueMSTR, TYPE_TSTR_BV);

		FN_1(fn_newEmptyGraph, TYPE_TSTR_BV, AddNewEmptyGraphMSTR, TYPE_TSTR_BV);
		FN_1(fn_newEmptyFunc, TYPE_TSTR_BV, AddNewEmptyFuncMSTR, TYPE_TSTR_BV);
		FN_2(fn_addNodeFromPreset, TYPE_TSTR_BV, AddNodeFromPresetMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV);

		FN_1(fn_getFuncKLCode, TYPE_TSTR_BV, GetKLCodeForFuncMSTR, TYPE_TSTR_BV);
		FN_2(fn_setFuncKLCode, TYPE_bool, SetKLCodeForFuncMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV);

		FN_3(fn_setPortMinMax, TYPE_bool, SetPortUIMinMaxMSTR, TYPE_TSTR_BV, TYPE_FPVALUE, TYPE_FPVALUE);

		// Properties 

		RO_PROP_FN(prop_getPortCount, GetPortCount, TYPE_INT);
		PROP_FNS(prop_getOutPortName, GetOutPortNameMSTR, prop_SetOutPort, SetOutPortMSTR, TYPE_TSTR_BV);
		RO_PROP_FN(prop_klEditor, GetKLEditor, TYPE_VALUE);
		
	END_FUNCTION_MAP

	// Implement the functions exposed above
	BOOL ShowDFGGraphEditor();

	virtual MSTR GetGraphName();
	// Get the fabric graph driving this max class.
	virtual FabricCore::DFGBinding& GetBinding() = 0;
	// Get the command handler (mostly for sending to the UI)
	virtual FabricUI::DFG::DFGUICmdHandler* GetCmdHandler() = 0;

	// Port creation/management

	// Get the number of ports on this graph
	virtual int GetPortCount() = 0;

	// Splice port management
	// Create a new port.  A matching Max parameter 
	// will be added as well of maxType, if MaxType is -1 
	virtual const char* AddInputPort(const char* klType, const char* name, int maxType=-1, bool isArray=false, const char* inExtension=nullptr) = 0;
	virtual const char* AddOutputPort(const char* klType, const char* name, bool isArray=false, const char* inExtension=nullptr) = 0;
	virtual const char* AddIOPort(const char* klType, const char* name, int maxType=-1, bool isArray=false, const char* inExtension=nullptr) = 0;

	// Remove specified port and matching max parameter
	virtual bool RemovePort(const char* portName) = 0;

	// Reset any auto-generated ports.
	virtual void ResetPorts()=0;

	// Get name of port
	virtual const char* GetPortName(int i) = 0;
	virtual const char* SetPortName(const char* oldName, const char* newName) = 0;
	virtual const char* GetPortType(const char* port) = 0;
	// Returns if the in port is an array type or not
	virtual bool IsPortArray(const char* port)=0;

	virtual const char* GetOutPortName() = 0;
	virtual bool SetOutPort(const char* name) = 0;

	virtual int GetMaxConnectedType(const char* portName) = 0;
	virtual int SetMaxConnectedType(const char* portName, int type) = 0;
	virtual BitArray GetLegalMaxTypes(const char* portName) = 0;

	// Allow setting various options on our ports
	bool SetPortMetaData(const char* port, const char* option, const char* value, bool canUndo);
	const char* GetPortMetaData(const char* port, const char* option);
	// Allow setting values directly on our ports
	bool SetPortValue(const char* port, FPValue* value);
	FPValue& GetPortValue(const char* port);

	// Convenience functions
	virtual bool SetPortUIMinMax(const char* port, FPValue* uiMin, FPValue* uiMax)=0;

	virtual const char* AddNewEmptyGraph(const char* name) = 0;
	virtual const char* AddNewEmptyFunc(const char* name) = 0;
	virtual const char* AddNodeFromPreset(const char* name, const char* path) = 0;

	virtual bool SetKLCodeForFunc(const char* name, const char* code) = 0;
	virtual const char* GetKLCodeForFunc(const char* name) = 0;

	// Connect myPortName to the output port on pSrcContainer named srcPortName
	// Returns true if successfully connected, false if for any reason the
	// port was not connected.  Once connected, each evaluation the output
	// from srcPortName will be transferred into the in-port myPortName
	virtual bool ConnectPort(const char* myPortName, ReferenceTarget* pSrcContainer, const char* srcPortName, int srcPortIndex, bool postConnectionsUI )=0;
	// Disconnect a previously connected port.  Returns true if the port was previously connected and
	// has been successfully disconnected, false if disconnect failed or if no connection existed.
	virtual bool DisconnectPort(const char* myPortName)=0;

	// Allow external classes to trigger evaluations on us
	virtual void TriggerEvaluate(TimeValue t, Interval& ivValid)=0;
	
	// Load the splice graph for this entity from the given filename
	virtual bool LoadFromFile(const MCHAR* filename, bool createMaxParams)=0;
	virtual bool SaveToFile(const MCHAR* filename)=0;

	virtual bool RestoreFromJSON(const char* json, bool createMaxParams) = 0;
	virtual void ExportToJSON(std::string& outString) = 0;

	// Show the MaxScript-based editor
	Value* GetKLEditor() { return m_klEditor; }
	void ShowKLEditor();
	void CloseKLEditor();
	void UpdateKLEditor();

	// After load, reconnect names.
	virtual void ReconnectPostLoad()=0;

	// A list of functions exposed just so DFGNotificationRouter can talk to us
	virtual void SetPblockParamName(int paramID, MSTR str) = 0;
	virtual void InvalidateAll() = 0;
	virtual void UpdateUISpec() = 0;

	virtual IParamBlock2* GetPBlock() = 0;

	//
	// Synchronize metadata from the dfg port passed
	// to the 3ds max parameter (if it exists).  This
	// includes things like max parameter type & 
	// value limits etc.
	//
	virtual int SyncMetaDataFromPortToParam(const char* dfgPort) = 0;
protected:

	// This function allows us to go up the other pants leg of 
	//virtual ReferenceTarget* CastToRefTarg()=0;
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

#define MSTR_OUTVAL(fnName, defValue) \
	MSTR fnName##MSTR() { \
		return ToMSTR(fnName(), defValue); \
	}
#define MSTR_INVAL(retType, fnName) \
	retType fnName##MSTR(const MSTR& inVal) { \
		return fnName(inVal.ToACP()); \
	}

	// TODO: Add default vals for these
	MSTR_OUTVAL(GetOutPortName, 0);

	MSTR_INVAL(bool, SetOutPort);
	
	MSTR GetPortNameMSTR(int i)	{ return ToMSTR(GetPortName(i), 0); }
	MSTR SetPortNameMSTR(const MSTR& oldName, const MSTR& newName)	{ return MSTR::FromACP(SetPortName(oldName.ToCStr(), newName.ToCStr())); }
	MSTR GetPortTyeMSTR(const MSTR& port)	{ return ToMSTR(GetPortType(port.ToCStr()), 0); }
	MSTR_INVAL(bool, IsPortArray);
	MSTR_INVAL(bool, GetMaxConnectedType);
	int SetMaxConnectedTypeMSTR(const MSTR& port, int type) { return SetMaxConnectedType(port.ToCStr(), type); }
	MSTR_INVAL(BitArray, GetLegalMaxTypes);


	MSTR AddInputPortMSTR(const MSTR& name, const MSTR& type, int maxType, bool isArray, const MSTR& inExtension)
	{
		return MSTR::FromACP(AddInputPort(name.ToCStr().data(), type.ToCStr().data(), maxType, isArray, inExtension.ToCStr().data()));
	}
	MSTR AddOutputPortMSTR(const MSTR& name, const MSTR& type, bool isArray, const MSTR& inExtension)
	{
		return MSTR::FromACP(AddOutputPort(name.ToCStr().data(), type.ToCStr().data(), isArray, inExtension.ToCStr().data()));
	}
	MSTR AddIOPortMSTR(const MSTR& name, const MSTR& type, int maxType, bool isArray, const MSTR& inExtension)
	{
		return MSTR::FromACP(AddIOPort(name.ToCStr().data(), type.ToCStr().data(), maxType, isArray, inExtension.ToCStr().data()));
	}
	MSTR_INVAL(bool, RemovePort);

	bool ConnectPortMSTR(const MSTR& myPortName, ReferenceTarget* pSrcContainer, const MSTR& srcPortName, int srcPortIndex, bool postConnectionsUI );
	MSTR_INVAL(bool, DisconnectPort);

	bool SetPortMetaDataMSTR(const MSTR& port, const MSTR& option, const MSTR& value, bool canUndo)	{ return SetPortMetaData(port.ToCStr(), option.ToCStr(), value.ToCStr(), canUndo); }
	MSTR GetPortMetaDataMSTR(const MSTR& port, const MSTR& option)	{ return MSTR::FromACP(GetPortMetaData(port.ToCStr(), option.ToCStr())); }

	bool SetPortValueMSTR(const MSTR& port, FPValue* value)							{ return SetPortValue(port.ToCStr(), value); }
	FPValue& GetPortValueMSTR(const MSTR& port)										{ return GetPortValue(port.ToCStr()); }
	bool SetPortUIMinMaxMSTR(const MSTR& port, FPValue* uiMin, FPValue* uiMax)		{ return SetPortUIMinMax(port.ToCStr(), uiMin, uiMax); }

	MSTR AddNewEmptyGraphMSTR(MSTR& graphName)						{ return MSTR::FromACP(AddNewEmptyGraph(graphName.ToCStr())); }
	MSTR AddNewEmptyFuncMSTR(MSTR& funcName)						{ return MSTR::FromACP(AddNewEmptyFunc(funcName.ToCStr())); }
	MSTR AddNodeFromPresetMSTR(const MSTR& name, const MSTR& path)	{ return MSTR::FromACP(AddNodeFromPreset(name.ToCStr(), path.ToCStr())); }

	bool SetKLCodeForFuncMSTR(const MSTR& funcName, MSTR& code)		{ return SetKLCodeForFunc(funcName.ToCStr(), code.ToCStr());	}
	MSTR GetKLCodeForFuncMSTR(const MSTR& name)						{ return MSTR::FromACP(GetKLCodeForFunc(name.ToCStr()));  }

	bool RestoreFromJSONMSTR(const MSTR& json, bool createMaxParams) { return RestoreFromJSON(json.ToCStr(), createMaxParams);  }
	MSTR ExportToJSONMSTR() {
		std::string jsonData;
		ExportToJSON(jsonData);
		return MSTR::FromACP(jsonData.data(), jsonData.length());
	}


#pragma endregion
};

// Inline fn for easy access to interface
SpliceTranslationFPInterface* GetSpliceInterface(ReferenceTarget* pTarg);

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
			SpliceTranslationFPInterface::fn_showDFGGraphEditor, _T("ShowDFGGraphEditor"), 0, TYPE_BOOL, 0, 0,

			SpliceTranslationFPInterface::fn_loadFromFile, _T("LoadFromFile"), 0, TYPE_bool, 0, 2, 
				_M("filename"),	0,	TYPE_FILENAME,
				_M("createMaxParams"),	0,	TYPE_bool,
			SpliceTranslationFPInterface::fn_saveToFile, _T("SaveToFile"), 0, TYPE_bool, 0, 1, 
				_M("filename"),	0,	TYPE_FILENAME,

			SpliceTranslationFPInterface::fn_exportToJson, _T("ExportToJSON"), 0, TYPE_TSTR_BV, 0, 0,
			SpliceTranslationFPInterface::fn_restoreFromJson, _T("RestoreFromJSON"), 0, TYPE_bool, 0, 2,
				_M("json"), 0, TYPE_TSTR_BV,
				_M("createMaxParams"), 0, TYPE_bool,

			SpliceTranslationFPInterface::fn_addInputPort, _T("AddInputPort"), 0, TYPE_TSTR_BV, 0, 5,
				_M("name"),			0,	TYPE_TSTR_BV,
				_M("fabricType"),	0,	TYPE_TSTR_BV,
				_M("maxType"),		0,	TYPE_INT,
				_M("isArray"),		0,	TYPE_bool, f_keyArgDefault, false,
				_M("Extension"),	0,	TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
			SpliceTranslationFPInterface::fn_addOutputPort, _T("AddOutputPort"), 0, TYPE_TSTR_BV, 0, 4,
				_M("name"),			0,	TYPE_TSTR_BV,
				_M("fabricType"),	0,	TYPE_TSTR_BV,
				_M("isArray"),		0,	TYPE_bool, f_keyArgDefault, false,
				_M("Extension"),	0,	TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
			SpliceTranslationFPInterface::fn_addIOPort, _T("AddIOPort"), 0, TYPE_TSTR_BV, 0, 5,
				_M("name"),			0,	TYPE_TSTR_BV,
				_M("fabricType"),	0,	TYPE_TSTR_BV,
				_M("maxType"),		0,	TYPE_INT,
				_M("isArray"),		0,	TYPE_bool, f_keyArgDefault, false,
				_M("Extension"),	0,	TYPE_TSTR_BV, f_keyArgDefault, MSTR(),

			SpliceTranslationFPInterface::fn_removePortName, _T("RemovePort"), 0, TYPE_bool, 0, 1,
				_M("portName"),		0,	TYPE_TSTR_BV,

			SpliceTranslationFPInterface::fn_getPortName, _T("GetPortName"), 0, TYPE_TSTR_BV, 0, 1,
				_M("portIndex"),	0,	TYPE_INDEX,
			SpliceTranslationFPInterface::fn_setPortName, _T("SetPortName"), 0, TYPE_TSTR_BV, 0, 2,
				_M("oldName"),			0,	TYPE_TSTR_BV,
				_M("newName"),			0,	TYPE_TSTR_BV,
			SpliceTranslationFPInterface::fn_getPortType, _T("GetPortType"), 0, TYPE_TSTR_BV, 0, 1,
				_M("portName"),		0,	TYPE_TSTR_BV,
			SpliceTranslationFPInterface::fn_isPortArray, _T("IsPortArray"), 0, TYPE_bool, 0, 1,
				_M("portName"),		0,	TYPE_TSTR_BV,

			SpliceTranslationFPInterface::fn_connectPorts, _T("ConnectPorts"), 0, TYPE_bool, 0, 5,
				_M("myPortName"),	0,	TYPE_TSTR_BV,
				_M("srcSpliceGraph"),	0,	TYPE_REFTARG,
				_M("srcPortName"),	0,	TYPE_TSTR_BV,
				_M("srcPortIndex"),	0,	TYPE_INT, f_keyArgDefault, -1,
				_M("postUI"),		0,	TYPE_bool, f_keyArgDefault, true,

			SpliceTranslationFPInterface::fn_getMaxConnectedType, _T("GetMaxType"), 0, TYPE_INT, 0, 1,
				_M("port"),		0,	TYPE_TSTR_BV, 
			SpliceTranslationFPInterface::fn_setMaxConnectedType, _T("SetMaxType"), 0, TYPE_INT, 0, 2,
				_M("port"),		0,	TYPE_TSTR_BV, 
				_M("maxType"),	0,	TYPE_INT,
			SpliceTranslationFPInterface::fn_getLegalMaxTypes, _T("GetLegalMaxTypes"), 0, TYPE_BITARRAY, 0, 1,
				_M("port"),		0,	TYPE_TSTR_BV, 

			SpliceTranslationFPInterface::fn_setPortMetaData, _T("SetPortMetaData"), 0, TYPE_bool, 0, 4,
				_M("port"),		0,	TYPE_TSTR_BV, 
				_M("option"),	0,	TYPE_TSTR_BV, 
				_M("value"),	0,	TYPE_TSTR_BV, 
				_M("canUndo"),	0, TYPE_bool, f_keyArgDefault, true,

			SpliceTranslationFPInterface::fn_getPortMetaData, _T("GetPortMetaData"), 0, TYPE_TSTR_BV, 0, 2,
				_M("port"), 0, TYPE_TSTR_BV,
				_M("option"), 0, TYPE_TSTR_BV,

			SpliceTranslationFPInterface::fn_setPortValue, _T("SetPortValue"), 0, TYPE_bool, 0, 2,
				_M("port"),		0,	TYPE_TSTR_BV, 
				_M("value"),	0,	TYPE_FPVALUE, 
			SpliceTranslationFPInterface::fn_getPortValue, _T("GetPortValue"), 0, TYPE_FPVALUE_BR, 0, 1,
				_M("port"), 0, TYPE_TSTR_BV,

			SpliceTranslationFPInterface::fn_setPortMinMax, _T("SetPortMinMax"), 0, TYPE_bool, 0, 3,
				_M("port"),		0,	TYPE_TSTR_BV, 
				_M("uiMin"),	0,	TYPE_FPVALUE, 
				_M("uiMax"),	0,	TYPE_FPVALUE, 

			SpliceTranslationFPInterface::fn_newEmptyFunc, _T("AddNewEmptyFunc"), 0, TYPE_TSTR_BV, 0, 1,
				_M("name"), 0, TYPE_TSTR_BV,
			SpliceTranslationFPInterface::fn_newEmptyGraph, _T("AddNewEmptyGraph"), 0, TYPE_TSTR_BV, 0, 1,
				_M("name"), 0, TYPE_TSTR_BV,
			SpliceTranslationFPInterface::fn_addNodeFromPreset, _T("AddNodeFromPreset"), 0, TYPE_TSTR_BV, 0, 2,
				_M("name"), 0, TYPE_TSTR_BV,
				_M("presetPath"), 0, TYPE_TSTR_BV,

			SpliceTranslationFPInterface::fn_getFuncKLCode, _T("GetFuncKLCode"), 0, TYPE_TSTR_BV, 0, 1,
				_M("funcPath"), 0, TYPE_TSTR_BV,
			SpliceTranslationFPInterface::fn_setFuncKLCode, _T("SetFuncKLCode"), 0, TYPE_bool, 0, 2,
				_M("funcPath"), 0, TYPE_TSTR_BV,
				_M("code"), 0, TYPE_TSTR_BV,
		properties,
			SpliceTranslationFPInterface::prop_getPortCount, FP_NO_FUNCTION, _T("PortCount"), 0, TYPE_INT,
			SpliceTranslationFPInterface::prop_getOutPortName, SpliceTranslationFPInterface::prop_SetOutPort, _T("OutPort"), 0, TYPE_TSTR_BV,
			SpliceTranslationFPInterface::prop_klEditor,	FP_NO_FUNCTION,		_T("KLEditor"),		0,	TYPE_VALUE,
		p_end
		);
	return &_ourDesc;
}