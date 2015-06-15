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
		fn_showSceneGraphEditor,
		fn_showDFGGraphEditor,

		fn_setSpliceGraph,
		fn_loadFromFile,
		fn_saveToFile,

		fn_restoreFromJson,
		fn_exportToJson,
		
		//fn_getOpCount,
		//fn_getOpName,

		fn_getKLCode,
		fn_getKLOpName,
		fn_setKLCode,
	
		fn_addInputPort,
		fn_addOutputPort,
		fn_addIOPort,

		//fn_removePortIdx,
		fn_removePortName,
		fn_getPortName,
		fn_setPortName,
		fn_getPortType,
		fn_isPortArray,

		fn_connectPorts,

		fn_getMaxConnectedType,
		fn_setMaxConnectedType,
		fn_getLegalMaxTypes,

		fn_setPortOption,
		fn_setPortMinMax,
		fn_setPortValue,

		// DFG-Specific functionality
		fn_newEmptyGraph,
		fn_newEmptyFunc,
		fn_addNodeFromPreset,

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
		FN_0(fn_showSceneGraphEditor, TYPE_BOOL, ShowSceneGraphEditor);
		FN_0(fn_showDFGGraphEditor, TYPE_BOOL, ShowDFGGraphEditor);

		FN_1(fn_setSpliceGraph, TYPE_BOOL, SetSpliceGraph, TYPE_REFTARG);
		FN_2(fn_loadFromFile, TYPE_bool, LoadFromFile, TYPE_FILENAME, TYPE_bool);
		FN_1(fn_saveToFile, TYPE_bool, SaveToFile, TYPE_FILENAME);

		FN_2(fn_restoreFromJson, TYPE_bool, RestoreFromJSONMSTR, TYPE_TSTR_BV, TYPE_bool);
		FN_0(fn_exportToJson, TYPE_TSTR_BV, ExportToJSONMSTR);

		FN_2(fn_setKLCode, TYPE_TSTR_BV, SetKLCodeMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV);
		FN_0(fn_getKLCode, TYPE_TSTR_BV, GetKLCodeMSTR);
		FN_0(fn_getKLOpName, TYPE_TSTR_BV, GetKLOperatorNameMSTR);

		FN_5(fn_addInputPort, TYPE_INT, AddInputPortMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_INT, TYPE_BOOL, TYPE_TSTR_BV);
		FN_4(fn_addOutputPort, TYPE_INT, AddOutputPortMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_BOOL, TYPE_TSTR_BV);
		FN_5(fn_addIOPort, TYPE_INT, AddIOPortMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_INT, TYPE_BOOL, TYPE_TSTR_BV);

		//FN_1(fn_removePortIdx, TYPE_bool, RemovePort, TYPE_INDEX);
		FN_1(fn_removePortName, TYPE_bool, RemovePortMSTR, TYPE_TSTR_BV);
		FN_1(fn_getPortName, TYPE_TSTR_BV, GetPortNameMSTR, TYPE_INT);
		FN_2(fn_setPortName, TYPE_bool, SetPortNameMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV);
		FN_1(fn_getPortType, TYPE_TSTR_BV, GetPortTyeMSTR, TYPE_TSTR_BV);
		FN_1(fn_isPortArray, TYPE_bool, IsPortArrayMSTR, TYPE_TSTR_BV);

		FN_5(fn_connectPorts, TYPE_bool, ConnectPortMSTR, TYPE_TSTR_BV, TYPE_REFTARG, TYPE_TSTR_BV, TYPE_INT, TYPE_bool);
		
		FN_1(fn_getMaxConnectedType, TYPE_INT, GetMaxConnectedTypeMSTR, TYPE_TSTR_BV);
		FN_2(fn_setMaxConnectedType, TYPE_INT, SetMaxConnectedTypeMSTR, TYPE_TSTR_BV, TYPE_INT);
		FN_1(fn_getLegalMaxTypes, TYPE_BITARRAY_BV, GetLegalMaxTypesMSTR, TYPE_TSTR_BV);

		FN_3(fn_setPortOption, TYPE_bool, SetPortOptionMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_FPVALUE);
		FN_2(fn_setPortValue, TYPE_bool, SetPortValueMSTR, TYPE_TSTR_BV, TYPE_FPVALUE);

		FN_1(fn_newEmptyGraph, TYPE_bool, AddNewEmptyGraphMSTR, TYPE_TSTR_BV);
		FN_1(fn_newEmptyFunc, TYPE_bool, AddNewEmptyFuncMSTR, TYPE_TSTR_BV);
		FN_2(fn_addNodeFromPreset, TYPE_bool, AddNodeFromPresetMSTR, TYPE_TSTR_BV, TYPE_TSTR_BV);

		FN_3(fn_setPortMinMax, TYPE_bool, SetPortUIMinMaxMSTR, TYPE_TSTR_BV, TYPE_FPVALUE, TYPE_FPVALUE);

		// Properties 

		RO_PROP_FN(prop_getPortCount, GetPortCount, TYPE_INT)
		PROP_FNS(prop_getOutPortName, GetOutPortNameMSTR, prop_SetOutPort, SetOutPortMSTR, TYPE_TSTR_BV)
		//PROP_FNS(prop_getOutPortArrayIdx, GetOutPortArrayIdx, prop_setOutPortArrayIdx, SetOutPortArrayIdx, TYPE_INT)
		//RO_PROP_FN(prop_getAllPortSignature, GetAllPortSignatureMSTR, TYPE_TSTR_BV)

		RO_PROP_FN(prop_klEditor, GetKLEditor, TYPE_VALUE)
		
	END_FUNCTION_MAP

	// Implement the functions exposed above
	BOOL ShowSceneGraphEditor() { /* TODO */ return FALSE; };
	BOOL ShowDFGGraphEditor();

	// Get the fabric graph driving this max class.
	virtual DFGWrapper::Binding& GetBinding() = 0;

	virtual std::string GetKLCode() = 0;
	virtual std::string GetKLOperatorName() = 0;
	virtual std::string SetKLCode(const std::string& name, const std::string& script) = 0;

	// Port creation/management

	// Get the number of ports on this graph
	virtual int GetPortCount() = 0;
	virtual DFGWrapper::ExecPortPtr GetPort(int i)=0;
	virtual DFGWrapper::ExecPortPtr GetPort(const char* name)=0;

	// Splice port management
	// Create a new port.  A matching Max parameter 
	// will be added as well of maxType, if MaxType is -1 
	virtual int AddInputPort(const char* klType, const char* name, int maxType=-1, bool isArray=false, const char* inExtension=nullptr) = 0;
	virtual int AddOutputPort(const char* klType, const char* name, bool isArray=false, const char* inExtension=nullptr) = 0;
	virtual int AddIOPort(const char* klType, const char* name, int maxType=-1, bool isArray=false, const char* inExtension=nullptr) = 0;

	// Remove specified port and matching max parameter
	virtual bool RemovePort(const char* portName) = 0;

	// Reset any auto-generated ports.
	virtual void ResetPorts()=0;

	// Get name of port
	virtual const char* GetPortName(int i) = 0;
	virtual bool SetPortName(const char* oldName, const char* newName) = 0;
	virtual const char* GetPortType(const char* port) = 0;
	// Returns if the in port is an array type or not
	virtual bool IsPortArray(const char* port)=0;

	virtual const char* GetOutPortName() = 0;
	virtual bool SetOutPort(const char* name) = 0;

	virtual int GetMaxConnectedType(const char* portName) = 0;
	virtual int SetMaxConnectedType(const char* portName, int type) = 0;
	virtual BitArray GetLegalMaxTypes(const char* portName) = 0;

	// Allow setting various options on our ports
	virtual bool SetPortOption(const char* port, const char* option, FPValue* value)=0;
	// Allow setting values directly on our ports
	virtual bool SetPortValue(const char* port, FPValue* value)=0;

	// Convenience functions
	virtual bool SetPortUIMinMax(const char* port, FPValue* uiMin, FPValue* uiMax)=0;

	virtual bool AddNewEmptyGraph(const char* name) = 0;
	virtual bool AddNewEmptyFunc(const char* name) = 0;
	virtual bool AddNodeFromPreset(const char* name, const char* path) = 0;

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
	
	// Set this instance to use the splice graph on the rtarg instance
	// This can be useful when setting a group of Splice instances to
	// all share a single graph.
	BOOL SetSpliceGraph(ReferenceTarget* rtarg);

	// Allow others to set the splice graph etc we are using.
	// These functions are not called directly from MaxScript, instead
	// they are used by the static interface
//	virtual void SetSpliceGraph(const DFGWrapper::Binding& graph, bool createMaxParams) = 0;
//	virtual void SetOutPort(const DFGWrapper::PortPtr& port) = 0;

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
	MSTR_OUTVAL(GetKLOperatorName, 0);
	MSTR_OUTVAL(GetOutPortName, 0);

	MSTR_INVAL(bool, SetOutPort);
	
	MSTR GetKLCodeMSTR() { return ToMSTR(GetKLCode().data(), 0); }
	MSTR SetKLCodeMSTR(MSTR name, MSTR script) { 
		return ToMSTR(SetKLCode(name.ToCStr().data(), script.ToCStr().data()).data(), 0); 
	}

	MSTR GetPortNameMSTR(int i)	{ return ToMSTR(GetPortName(i), 0); }
	bool SetPortNameMSTR(const MSTR& oldName, const MSTR& newName)	{ return SetPortName(oldName.ToCStr(), newName.ToCStr()); }
	MSTR GetPortTyeMSTR(const MSTR& port)	{ return ToMSTR(GetPortType(port.ToCStr()), 0); }
	MSTR_INVAL(bool, IsPortArray);
	//bool IsPortArrayMSTR(const MSTR& port) { return IsPortArray(port.ToCStr()); }
	MSTR_INVAL(bool, GetMaxConnectedType);
	//int GetMaxConnectedTypeMSTR(const MSTR& port) { return GetMaxConnectedType(port.ToCStr()); }
	int SetMaxConnectedTypeMSTR(const MSTR& port, int type) { return SetMaxConnectedType(port.ToCStr(), type); }
	//BitArray GetLegalMaxTypesMSTR(const MSTR& port) { return GetLegalMaxTypes(port.ToCStr()); }
	MSTR_INVAL(BitArray, GetLegalMaxTypes);


	int AddInputPortMSTR(const MSTR& name, const MSTR& type, int maxType, bool isArray, const MSTR& inExtension)
	{
		return AddInputPort(name.ToCStr().data(), type.ToCStr().data(), maxType, isArray, inExtension.ToCStr().data());
	}
	int AddOutputPortMSTR(const MSTR& name, const MSTR& type, bool isArray, const MSTR& inExtension)
	{
		return AddOutputPort(name.ToCStr().data(), type.ToCStr().data(), isArray, inExtension.ToCStr().data());
	}
	int AddIOPortMSTR(const MSTR& name, const MSTR& type, int maxType, bool isArray, const MSTR& inExtension)
	{
		return AddIOPort(name.ToCStr().data(), type.ToCStr().data(), maxType, isArray, inExtension.ToCStr().data());
	}
	MSTR_INVAL(bool, RemovePort);

	bool ConnectPortMSTR(const MSTR& myPortName, ReferenceTarget* pSrcContainer, const MSTR& srcPortName, int srcPortIndex, bool postConnectionsUI );
	MSTR_INVAL(bool, DisconnectPort);

	bool SetPortOptionMSTR(const MSTR& port, const MSTR& option, FPValue* value)	{ return SetPortOption(port.ToCStr(), option.ToCStr(), value); }
	bool SetPortValueMSTR(const MSTR& port, FPValue* value)	{ return SetPortValue(port.ToCStr(), value); }
	bool SetPortUIMinMaxMSTR(const MSTR& port, FPValue* uiMin, FPValue* uiMax)	{ return SetPortUIMinMax(port.ToCStr(), uiMin, uiMax); }

	MSTR_INVAL(bool, AddNewEmptyGraph);
	MSTR_INVAL(bool, AddNewEmptyFunc);
	bool AddNodeFromPresetMSTR(const MSTR& name, const MSTR& path) { return AddNodeFromPreset(name.ToCStr(), path.ToCStr()); }

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
			SpliceTranslationFPInterface::fn_showSceneGraphEditor, _T("ShowSceneGraphEditor"), 0, TYPE_BOOL, 0, 0, 
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

			SpliceTranslationFPInterface::fn_setSpliceGraph, _T("SetSpliceGraph"), 0, TYPE_BOOL, 0, 1,
				_M("source"),	0,	TYPE_REFTARG,



			SpliceTranslationFPInterface::fn_getKLCode, _T("GetKLCode"), 0, TYPE_TSTR_BV, 0, 0, 
			SpliceTranslationFPInterface::fn_getKLOpName, _T("GetKLOperatorName"), 0, TYPE_TSTR_BV, 0, 0, 
			SpliceTranslationFPInterface::fn_setKLCode, _T("SetKLCode"), 0, TYPE_TSTR_BV, 0, 2,
				_M("name"),		0,	TYPE_TSTR_BV,
				_M("script"),	0,	TYPE_TSTR_BV,

			SpliceTranslationFPInterface::fn_addInputPort, _T("AddInputPort"), 0, TYPE_INDEX, 0, 5,
				_M("name"),			0,	TYPE_TSTR_BV,
				_M("fabricType"),	0,	TYPE_TSTR_BV,
				_M("maxType"),		0,	TYPE_INT,
				_M("isArray"),		0,	TYPE_bool, f_keyArgDefault, false,
				_M("Extension"),	0,	TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
			SpliceTranslationFPInterface::fn_addOutputPort, _T("AddOutputPort"), 0, TYPE_INDEX, 0, 4,
				_M("name"),			0,	TYPE_TSTR_BV,
				_M("fabricType"),	0,	TYPE_TSTR_BV,
				_M("isArray"),		0,	TYPE_bool, f_keyArgDefault, false,
				_M("Extension"),	0,	TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
			SpliceTranslationFPInterface::fn_addIOPort, _T("AddIOPort"), 0, TYPE_INDEX, 0, 5,
				_M("name"),			0,	TYPE_TSTR_BV,
				_M("fabricType"),	0,	TYPE_TSTR_BV,
				_M("maxType"),		0,	TYPE_INT,
				_M("isArray"),		0,	TYPE_bool, f_keyArgDefault, false,
				_M("Extension"),	0,	TYPE_TSTR_BV, f_keyArgDefault, MSTR(),

			//SpliceTranslationFPInterface::fn_removePortIdx,	 _T("RemovePortByIndex"), 0, TYPE_bool, 0, 1,
			//	_M("portIndex"),	0,	TYPE_INDEX,
			SpliceTranslationFPInterface::fn_removePortName, _T("RemovePort"), 0, TYPE_bool, 0, 1,
				_M("portName"),		0,	TYPE_TSTR_BV,

			SpliceTranslationFPInterface::fn_getPortName, _T("GetPortName"), 0, TYPE_TSTR_BV, 0, 1,
				_M("portIndex"),	0,	TYPE_INDEX,
			SpliceTranslationFPInterface::fn_setPortName, _T("SetPortName"), 0, TYPE_VOID, 0, 2,
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

			SpliceTranslationFPInterface::fn_setPortOption, _T("SetPortOption"), 0, TYPE_bool, 0, 3,
				_M("port"),		0,	TYPE_TSTR_BV, 
				_M("option"),	0,	TYPE_TSTR_BV, 
				_M("value"),	0,	TYPE_FPVALUE, 

			SpliceTranslationFPInterface::fn_setPortMinMax, _T("SetPortValue"), 0, TYPE_bool, 0, 2,
				_M("port"),		0,	TYPE_TSTR_BV, 
				_M("value"),	0,	TYPE_FPVALUE, 

			SpliceTranslationFPInterface::fn_setPortMinMax, _T("SetPortMinMax"), 0, TYPE_bool, 0, 3,
				_M("port"),		0,	TYPE_TSTR_BV, 
				_M("uiMin"),	0,	TYPE_FPVALUE, 
				_M("uiMax"),	0,	TYPE_FPVALUE, 

			SpliceTranslationFPInterface::fn_newEmptyFunc, _T("AddNewEmptyFunc"), 0, TYPE_bool, 0, 1,
				_M("name"), 0, TYPE_TSTR_BV,
			SpliceTranslationFPInterface::fn_newEmptyGraph, _T("AddNewEmptyGraph"), 0, TYPE_bool, 0, 1,
				_M("name"), 0, TYPE_TSTR_BV,
			SpliceTranslationFPInterface::fn_addNodeFromPreset, _T("AddNodeFromPreset"), 0, TYPE_bool, 0, 2,
				_M("name"), 0, TYPE_TSTR_BV,
				_M("presetPath"), 0, TYPE_TSTR_BV,


		properties,
			SpliceTranslationFPInterface::prop_getPortCount, FP_NO_FUNCTION, _T("PortCount"), 0, TYPE_INT,
			SpliceTranslationFPInterface::prop_getOutPortName, SpliceTranslationFPInterface::prop_SetOutPort, _T("OutPort"), 0, TYPE_TSTR_BV,
			SpliceTranslationFPInterface::prop_klEditor,	FP_NO_FUNCTION,		_T("KLEditor"),		0,	TYPE_VALUE,
		p_end
		);
	return &_ourDesc;
}