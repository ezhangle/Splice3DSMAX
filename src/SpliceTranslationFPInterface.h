#pragma once

#include <ifnpub.h>
#include "MaxDFGCmdHandler.h"

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

protected:

	// Binding to a DFG graph   
	FabricCore::DFGBinding m_binding;

	// The command handler to route commands into 
	MaxDFGCmdHandler m_fabricCmdHandler;

public:

	SpliceTranslationFPInterface();
	~SpliceTranslationFPInterface();

	// The follow exposes our functions to Max
	enum FN_IDS {
		// System management callbacks
		fn_showDFGGraphEditor,

		fn_dfgRemoveNodes,
		fn_dfgConnect,
		fn_dfgDisconnect,
		fn_dfgAddGraph,
		fn_dfgAddFunc,
		fn_dfgInstPreset,
		fn_dfgAddVar,
		fn_dfgAddSet,
		fn_dfgAddGet,
		fn_dfgAddPort,
		fn_dfgEditPort,
		fn_dfgRenamePort,
		fn_dfgSetPortDefaultValue,
		fn_dfgRemovePort,
		fn_dfgResizeBackdrop,
		fn_dfgMoveNodes,
		fn_dfgImplodeNodes,
		fn_dfgExplodeNodes,
		fn_dfgAddBackdrop,
		fn_dfgSetNodeTitle,
		fn_dfgSetNodeComment,
		fn_dfgSetCode,
		fn_dfgPaste,
		fn_dfgSetArgType,
		fn_dfgSetArgValue,
		fn_dfgSetRefVarPath,
		fn_dfgReorderPorts,
		fn_dfgSetExtDeps,
		fn_dfgSplitFromPreset,

		fn_loadFromFile,
		fn_saveToFile,

		fn_restoreFromJson,
		fn_exportToJson,


		fn_getPortCount,
		fn_getPortName,
		fn_getPortType,
		fn_getPortValue,

		fn_getMaxConnectedType,
		fn_setMaxConnectedType,
		fn_getLegalMaxTypes,

		fn_setPortMetaData,
		fn_getPortMetaData,
		fn_setPortMinMax,


		fn_connectArgs,

		prop_getOutPortName,
		prop_SetOutPortName,

		num_params,

		port_mode_enum
	};

	BEGIN_FUNCTION_MAP	
		FN_0(fn_showDFGGraphEditor, TYPE_BOOL, ShowDFGGraphEditor);

		VFN_2(fn_dfgRemoveNodes,						DFGRemoveNodes,			TYPE_TSTR_TAB_BV,	TYPE_TSTR_BV);
		VFN_3(fn_dfgConnect,							DFGConnect,				TYPE_TSTR_BV,		TYPE_TSTR_BV, TYPE_TSTR_BV);
		VFN_3(fn_dfgDisconnect,							DFGDisconnect,			TYPE_TSTR_BV,		TYPE_TSTR_BV, TYPE_TSTR_BV);
		FN_3(fn_dfgAddGraph,		TYPE_TSTR_BV,		DFGAddGraph,			TYPE_TSTR_BV,		TYPE_POINT2, TYPE_TSTR_BV);
		FN_4(fn_dfgAddFunc,			TYPE_TSTR_BV,		DFGAddFunc,				TYPE_TSTR_BV,		TYPE_TSTR_BV, TYPE_POINT2, TYPE_TSTR_BV);
		FN_3(fn_dfgInstPreset,		TYPE_TSTR_BV,		DFGInstPreset,			TYPE_TSTR_BV,		TYPE_POINT2, TYPE_TSTR_BV);
		FN_5(fn_dfgAddVar,			TYPE_TSTR_BV,		DFGAddVar,				TYPE_TSTR_BV,		TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_POINT2, TYPE_TSTR_BV);
		FN_4(fn_dfgAddGet,			TYPE_TSTR_BV,		DFGAddGet,				TYPE_TSTR_BV,		TYPE_TSTR_BV, TYPE_POINT2, TYPE_TSTR_BV);
		FN_4(fn_dfgAddSet,			TYPE_TSTR_BV,		DFGAddSet,				TYPE_TSTR_BV,		TYPE_TSTR_BV, TYPE_POINT2, TYPE_TSTR_BV);
		FN_7(fn_dfgAddPort,			TYPE_TSTR_BV,		DFGAddPort,				TYPE_TSTR_BV,		TYPE_ENUM, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_BV);
		FN_6(fn_dfgEditPort,		TYPE_TSTR_BV,		DFGEditPort,			TYPE_TSTR_BV,		TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_BV);
		VFN_2(fn_dfgRemovePort,							DFGRemovePort,			TYPE_TSTR_BV,		TYPE_TSTR_BV);
		VFN_4(fn_dfgResizeBackdrop,						DFGResizeBackdrop,		TYPE_TSTR_BV,		TYPE_POINT2, TYPE_POINT2, TYPE_TSTR_BV);
		VFN_3(fn_dfgMoveNodes,							DFGMoveNodes,			TYPE_TSTR_TAB_BV,	TYPE_POINT2_TAB_BR, TYPE_TSTR_BV);
		FN_3(fn_dfgImplodeNodes,	TYPE_TSTR_BV,		DFGImplodeNodes,		TYPE_TSTR_TAB_BV,	TYPE_TSTR_BV, TYPE_TSTR_BV);
		FN_2(fn_dfgExplodeNodes,	TYPE_TSTR_TAB_BV,	DFGExplodeNodes,		TYPE_TSTR_BV,		TYPE_TSTR_BV);
		VFN_3(fn_dfgAddBackdrop,						DFGAddBackdrop,			TYPE_TSTR_BV,		TYPE_POINT2, TYPE_TSTR_BV);
		VFN_3(fn_dfgSetNodeTitle,						DFGSetNodeTitle,		TYPE_TSTR_BV,		TYPE_TSTR_BV, TYPE_TSTR_BV);
		VFN_3(fn_dfgSetNodeComment,						DFGSetNodeComment,		TYPE_TSTR_BV,		TYPE_TSTR_BV, TYPE_TSTR_BV);
		VFN_2(fn_dfgSetCode,							DFGSetCode,				TYPE_TSTR_BV,		TYPE_TSTR_BV);
		FN_3(fn_dfgRenamePort,		TYPE_TSTR_BV,		DFGRenamePort,			TYPE_TSTR_BV,		TYPE_TSTR_BV, TYPE_TSTR_BV);
		FN_3(fn_dfgPaste,			TYPE_TSTR_TAB_BV,	DFGPaste,				TYPE_TSTR_BV,		TYPE_POINT2, TYPE_TSTR_BV);
		VFN_2(fn_dfgSetArgType,							DFGSetArgType,			TYPE_TSTR_BV,		TYPE_TSTR_BV);
		VFN_2(fn_dfgSetArgValue,						DFGSetArgValue,			TYPE_TSTR_BV,		TYPE_FPVALUE);
		VFN_3(fn_dfgSetPortDefaultValue,				DFGSetPortDefaultValue,	TYPE_TSTR_BV,		TYPE_FPVALUE, TYPE_TSTR_BV);
		VFN_3(fn_dfgSetRefVarPath,						DFGSetRefVarPath,		TYPE_TSTR_BV,		TYPE_TSTR_BV, TYPE_TSTR_BV);
		VFN_2(fn_dfgReorderPorts,						DFGReorderPorts,		TYPE_INT_TAB_BR,	TYPE_TSTR_BV);
		VFN_2(fn_dfgSetExtDeps,							DFGSetExtDeps,			TYPE_TSTR_TAB_BV,	TYPE_TSTR_BV);
		VFN_1(fn_dfgSplitFromPreset,					DFGSplitFromPreset,		TYPE_TSTR_BV)		


		FN_2(fn_loadFromFile, TYPE_bool, LoadFromFile, TYPE_FILENAME, TYPE_bool);
		FN_1(fn_saveToFile, TYPE_bool, SaveToFile, TYPE_FILENAME);

		FN_2(fn_restoreFromJson, TYPE_bool, RestoreFromJSON, TYPE_TSTR_BR, TYPE_bool);
		FN_0(fn_exportToJson, TYPE_TSTR_BV, ExportToJSON);

		FN_1(fn_getPortCount, TYPE_INT, GetPortCount, TYPE_TSTR_BV);
		FN_2(fn_getPortName, TYPE_TSTR_BV, GetPortName, TYPE_INDEX, TYPE_TSTR_BV);
		FN_2(fn_getPortType, TYPE_TSTR_BV, GetPortType, TYPE_TSTR_BR, TYPE_TSTR_BV);
		FN_2(fn_getPortValue, TYPE_FPVALUE, GetPortValue, TYPE_TSTR_BR, TYPE_TSTR_BV);
		
		FN_1(fn_getMaxConnectedType, TYPE_INT, GetMaxTypeForArg, TYPE_TSTR_BV);
		FN_2(fn_setMaxConnectedType, TYPE_INT, SetMaxTypeForArg, TYPE_TSTR_BV, TYPE_INT);
		FN_1(fn_getLegalMaxTypes, TYPE_BITARRAY_BV, GetLegalMaxTypesForArg, TYPE_TSTR_BV);

		FN_4(fn_setPortMetaData, TYPE_bool, SetPortMetaData, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_BV, TYPE_TSTR_BV);
		FN_3(fn_getPortMetaData, TYPE_TSTR_BV, GetPortMetaData, TYPE_TSTR_BR, TYPE_TSTR_BR, TYPE_TSTR_BV);

		FN_4(fn_setPortMinMax, TYPE_bool, SetPortUIMinMax, TYPE_TSTR_BR, TYPE_FPVALUE, TYPE_FPVALUE, TYPE_TSTR_BR);

		FN_5(fn_connectArgs, TYPE_bool, ConnectArgs, TYPE_TSTR_BR, TYPE_REFTARG, TYPE_TSTR_BR, TYPE_INT, TYPE_bool);
		
		// Properties 
		PROP_FNS(prop_getOutPortName, GetOutPortName, prop_SetOutPortName, SetOutPortName, TYPE_TSTR_BV);
		
	END_FUNCTION_MAP

	// Implement the functions exposed above
	BOOL ShowDFGGraphEditor();

	// The following are direct mappers to the commands defined by the DFGCmdHandler classed.
	void DFGRemoveNodes(const Tab<TSTR*>& nodeNames, const MSTR& execPath);
	void DFGConnect(const MSTR& srcPath, const MSTR& destPath, const MSTR& execPath);
	void DFGDisconnect(const MSTR& srcPath, const MSTR& destPath, const MSTR& execPath);
	MSTR DFGAddGraph(const MSTR& title, Point2 pos, const MSTR& execPath);
	MSTR DFGAddFunc(const MSTR& title, const MSTR& initialCode, Point2 pos, const MSTR& execPath);
	MSTR DFGInstPreset(const MSTR& filename, Point2 pos, const MSTR& execPath);
	MSTR DFGAddVar(const MSTR& desiredNodeName, const MSTR& dataType, const MSTR& extDep, Point2 pos, const MSTR& execPath);
	MSTR DFGAddGet(const MSTR& desiredNodeName, const MSTR& varPath, Point2 pos, const MSTR& execPath);
	MSTR DFGAddSet(const MSTR& desiredNodeName, const MSTR& varPath, Point2 pos, const MSTR& execPath);
	MSTR DFGAddPort(const MSTR& desiredPortName, int portType, const MSTR& portSpec, const MSTR& portToConnect, const MSTR& extDep, const MSTR& metaData, const MSTR& execPath);
	MSTR DFGEditPort(const MSTR& portName, const MSTR& desiredNewPortName, const MSTR& typeSpec, const MSTR& extDep, const MSTR& metaData, const MSTR& execPath);
	void DFGRemovePort(const MSTR& portName, const MSTR& execPath);
	void DFGResizeBackdrop(const MSTR& backDropNodeName, Point2 topLeft, Point2 size, const MSTR& execPath);
	void DFGMoveNodes(Tab<TSTR*> nodeNames, Tab<Point2*> topLeftPoss, const MSTR& execPath);
	MSTR DFGImplodeNodes(Tab<TSTR*> nodeNames, const MSTR& desiredNewNodeName, const MSTR& execPath);
	Tab<TSTR*> DFGExplodeNodes(const MSTR& nodeName, const MSTR& execPath);
	void DFGAddBackdrop(const MSTR& title, Point2 pos, const MSTR& execPath);
	void DFGSetNodeTitle(const MSTR& nodeName, const MSTR& newTitle, const MSTR& execPath);
	void DFGSetNodeComment(const MSTR& nodeName, const MSTR& comment, const MSTR& execPath);
	void DFGSetCode(const MSTR& code, const MSTR& execPath);
	MSTR DFGRenamePort(const MSTR& oldName, const MSTR& newDesiredName, const MSTR& execPath);
	Tab<TSTR*> DFGPaste(const MSTR& json, Point2 pos, const MSTR& execPath);
	void DFGSetArgType(const MSTR& argName, const MSTR& argType);
	void DFGSetArgValue(const MSTR& argName, const FPValue* argValue);
	void DFGSetPortDefaultValue(const MSTR& portName, const FPValue* value, const MSTR& execPath);
	void DFGSetRefVarPath(const MSTR& refName, const MSTR& varPath, const MSTR& execPath);
	void DFGReorderPorts(Tab<int> indices, const MSTR& execPath);
	void DFGSetExtDeps(Tab<TSTR*> extDeps, const MSTR& execPath);
	void DFGSplitFromPreset(const MSTR& execPath);

	// Allow introspecting the ports on this graph
	int GetPortCount(const MSTR& execPath);
	MSTR GetPortName(int i, const MSTR& execPath);
	MSTR GetPortType(const MSTR& portName, const MSTR& execPath);
	FPValue GetPortValue(const MSTR& portName, const MSTR& execPath);
	// Returns if the in port is an array type or not
	//virtual bool IsPortArray(const char* port)=0;


	int GetMaxTypeForArg(const MSTR& argName);
	int SetMaxTypeForArg(const MSTR& argName, int type);
	BitArray GetLegalMaxTypesForArg(const MSTR& argName);

	// Allow setting various options on our ports
	bool SetPortMetaData(const MSTR& port, const MSTR& option, const MSTR& value, const MSTR& execPath);
	MSTR GetPortMetaData(const MSTR& port, const MSTR& option, const MSTR& execPath);

	// Convenience functions
	bool SetPortUIMinMax(const MSTR& port, FPValue* uiMin, FPValue* uiMax, const MSTR& execPath);

	// Load the splice graph for this entity from the given filename
	bool LoadFromFile(const MCHAR* filename, bool createMaxParams);
	bool SaveToFile(const MCHAR* filename);

	bool RestoreFromJSON(const MSTR& json, bool createMaxParams);
	bool RestoreFromJSON(const char* json, bool createMaxParams);
	MSTR ExportToJSON();

	// Connect myPortName to the output port on pSrcContainer named srcPortName
	// Returns true if successfully connected, false if for any reason the
	// port was not connected.  Once connected, each evaluation the output
	// from srcPortName will be transferred into the in-port myPortName
	virtual bool ConnectArgs(const MSTR& myPortName, ReferenceTarget* pSrcContainer, const MSTR& srcPortName, int srcPortIndex, bool postConnectionsUI) = 0;
	// Disconnect a previously connected port.  Returns true if the port was previously connected and
	// has been successfully disconnected, false if disconnect failed or if no connection existed.
	virtual bool DisconnectArgs(const MSTR& myPortName) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Props
	virtual MSTR GetOutPortName() = 0;
	virtual bool SetOutPortName(const MSTR& name) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Conv easing functions
	int GetMaxTypeForArg(const char* argName)				{ return GetMaxTypeForArg(MSTR::FromACP(argName)); }
	int SetMaxTypeForArg(const char* argName, int type)		{ return SetMaxTypeForArg(MSTR::FromACP(argName), type); }

	//////////////////////////////////////////////////////////////////////////
	// General management functions allows different templated
	// types to speak to each other.
	
	/*! Gets the type of the data evaluated by this class
	This return value needs to match the TDataType template param */
	virtual int GetValueType() = 0;

	virtual MSTR GetGraphName();
	// Get the fabric graph driving this max class.
	//virtual FabricCore::DFGBinding& GetBinding() = 0;
	// Get the command handler (mostly for sending to the UI)
	//virtual FabricUI::DFG::DFGUICmdHandler* GetCmdHandler() = 0;

	// Reset any auto-generated ports.
	virtual void ResetPorts()=0;





	// Allow setting values directly on our ports
	//bool SetPortValue(const char* port, FPValue* value);


	//virtual const char* AddNewEmptyGraph(const char* name) = 0;
	//virtual const char* AddNewEmptyFunc(const char* name) = 0;
	//virtual const char* AddNodeFromPreset(const char* name, const char* path) = 0;


	// Allow external classes to trigger evaluations on us
	virtual void TriggerEvaluate(TimeValue t, Interval& ivValid)=0;
	
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

	// Access a few of the internals
	FabricCore::DFGBinding& GetBinding() { return m_binding; }
	const FabricCore::DFGBinding& GetBinding() const { return m_binding; }
	void SetBinding(FabricCore::DFGBinding binding) { m_binding = binding; }
	// This function allows us to go up the other pants leg of 
	//virtual ReferenceTarget* CastToRefTarg()=0;
//#pragma region MSTR<->CStr conversion
//
//	// Our MSTR version of the function converts to CStr and passes on to real handler
//	inline MSTR ToMSTR(const char* v, int defValue)
//	{
//		MSTR resMSTR; 
//		if (v != NULL) 
//			resMSTR = MSTR::FromACP(v, strlen(v));
//		else 
//			resMSTR = GetString(defValue);
//		return resMSTR;
//	}
//	inline MSTR ToMSTR(std::string v, int defValue) { return ToMSTR(v.data(), defValue); }
//
//#define MSTR_OUTVAL(fnName, defValue) \
//	MSTR fnName##MSTR() { \
//		return ToMSTR(fnName(), defValue); \
//	}
//#define MSTR_INVAL(retType, fnName) \
//	retType fnName##MSTR(const MSTR& inVal) { \
//		return fnName(inVal.ToACP()); \
//	}
//
//	// TODO: Add default vals for these
//	MSTR_OUTVAL(GetOutPortName, 0);
//
//	MSTR_INVAL(bool, SetOutPort);
//	
//	MSTR GetPortNameMSTR(int i)	{ return ToMSTR(GetPortName(i), 0); }
//	MSTR SetPortNameMSTR(const MSTR& oldName, const MSTR& newName)	{ return MSTR::FromACP(SetPortName(oldName.ToCStr(), newName.ToCStr())); }
//	MSTR GetPortTyeMSTR(const MSTR& port)	{ return ToMSTR(GetPortType(port.ToCStr()), 0); }
//	MSTR_INVAL(bool, IsPortArray);
//	MSTR_INVAL(bool, GetMaxTypeForArg);
//	int SetMaxConnectedTypeMSTR(const MSTR& port, int type) { return SetMaxTypeForArg(port.ToCStr(), type); }
//	MSTR_INVAL(BitArray, GetLegalMaxTypesForArg);
//
//
//	MSTR AddInputPortMSTR(const MSTR& name, const MSTR& type, int maxType, bool isArray, const MSTR& inExtension)
//	{
//		return MSTR::FromACP(AddInputPort(name.ToCStr().data(), type.ToCStr().data(), maxType, isArray, inExtension.ToCStr().data()));
//	}
//	MSTR AddOutputPortMSTR(const MSTR& name, const MSTR& type, bool isArray, const MSTR& inExtension)
//	{
//		return MSTR::FromACP(AddOutputPort(name.ToCStr().data(), type.ToCStr().data(), isArray, inExtension.ToCStr().data()));
//	}
//	MSTR AddIOPortMSTR(const MSTR& name, const MSTR& type, int maxType, bool isArray, const MSTR& inExtension)
//	{
//		return MSTR::FromACP(AddIOPort(name.ToCStr().data(), type.ToCStr().data(), maxType, isArray, inExtension.ToCStr().data()));
//	}
//	MSTR_INVAL(bool, RemovePort);
//
//	bool ConnectPortMSTR(const MSTR& myPortName, ReferenceTarget* pSrcContainer, const MSTR& srcPortName, int srcPortIndex, bool postConnectionsUI );
//	MSTR_INVAL(bool, DisconnectPort);
//
//	bool SetPortMetaDataMSTR(const MSTR& port, const MSTR& option, const MSTR& value, bool canUndo)	{ return SetPortMetaData(port.ToCStr(), option.ToCStr(), value.ToCStr(), canUndo); }
//	MSTR GetPortMetaDataMSTR(const MSTR& port, const MSTR& option)	{ return MSTR::FromACP(GetPortMetaData(port.ToCStr(), option.ToCStr())); }
//
//	bool SetPortValueMSTR(const MSTR& port, FPValue* value)							{ return SetPortValue(port.ToCStr(), value); }
//	FPValue& GetPortValueMSTR(const MSTR& port)										{ return GetPortValue(port.ToCStr()); }
//	bool SetPortUIMinMaxMSTR(const MSTR& port, FPValue* uiMin, FPValue* uiMax)		{ return SetPortUIMinMax(port.ToCStr(), uiMin, uiMax); }
//
//	MSTR AddNewEmptyGraphMSTR(MSTR& graphName)						{ return MSTR::FromACP(AddNewEmptyGraph(graphName.ToCStr())); }
//	MSTR AddNewEmptyFuncMSTR(MSTR& funcName)						{ return MSTR::FromACP(AddNewEmptyFunc(funcName.ToCStr())); }
//	MSTR AddNodeFromPresetMSTR(const MSTR& name, const MSTR& path)	{ return MSTR::FromACP(AddNodeFromPreset(name.ToCStr(), path.ToCStr())); }
//
//	bool SetKLCodeForFuncMSTR(const MSTR& funcName, MSTR& code)		{ return SetKLCodeForFunc(funcName.ToCStr(), code.ToCStr());	}
//	MSTR GetKLCodeForFuncMSTR(const MSTR& name)						{ return MSTR::FromACP(GetKLCodeForFunc(name.ToCStr()));  }
//
//	bool RestoreFromJSONMSTR(const MSTR& json, bool createMaxParams) { return RestoreFromJSON(json.ToCStr(), createMaxParams);  }
//	MSTR ExportToJSONMSTR() {
//		std::string jsonData;
//		ExportToJSON(jsonData);
//		return MSTR::FromACP(jsonData.data(), jsonData.length());
//	}


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

			//////////////////////////////////////////////////////////////////////////
			// Fabric DFG commands
			SpliceTranslationFPInterface::fn_dfgRemoveNodes, _T("DFGRemoveNodes"), 0, 0, 0, 2,
				_M("nodeNames"), 0, TYPE_TSTR_TAB_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgConnect, _T("DFGConnect"), 0, 0, 0, 3,
				_M("srcPath"), 0, TYPE_TSTR_BV,
				_M("destPath"), 0, TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgDisconnect, _T("DFGDisconnect"), 0, 0, 0, 3,
				_M("srcPath"), 0, TYPE_TSTR_BV,
				_M("destPath"), 0, TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgAddGraph, _T("DFGAddGraph"), 0, TYPE_TSTR_BV, 0, 3,
				_M("title"), 0, TYPE_TSTR_BV,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgAddFunc, _T("DFGAddFunc"), 0, TYPE_TSTR_BV, 0, 4,
				_M("title"), 0, TYPE_TSTR_BV,
				_M("initialCode"), 0, TYPE_TSTR_BV,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgInstPreset, _T("DFGInstPreset"), 0, TYPE_TSTR_BV, 0, 3,
				_M("filename"), 0, TYPE_TSTR_BV,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgAddVar, _T("DFGAddVar"), 0, TYPE_TSTR_BV, 0, 5,
				_M("desiredNodeName"), 0, TYPE_TSTR_BV,
				_M("dataType"), 0, TYPE_TSTR_BV,
				_M("extDep"), 0, TYPE_TSTR_BV,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgAddSet, _T("DFGAddSet"), 0, TYPE_TSTR_BV, 0, 4,
				_M("desiredNodeName"), 0, TYPE_TSTR_BV,
				_M("varPath"), 0, TYPE_TSTR_BV,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgAddGet, _T("DFGAddGet"), 0, TYPE_TSTR_BV, 0, 4,
				_M("desiredNodeName"), 0, TYPE_TSTR_BV,
				_M("varPath"), 0, TYPE_TSTR_BV,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgAddPort, _T("DFGAddPort"), 0, TYPE_TSTR_BV, 0, 7,
				_M("desiredPortName"), 0, TYPE_TSTR_BV,
				_M("portType"), 0, TYPE_ENUM, SpliceTranslationFPInterface::port_mode_enum,
				_M("portSpec"), 0, TYPE_TSTR_BV,
				_M("portToConnect"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				_M("extDep"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				_M("metaData"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgEditPort, _T("DFGEditPort"), 0, TYPE_TSTR_BV, 0, 6,
				_M("portName"), 0, TYPE_TSTR_BV,
				_M("desiredNewPortName"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				_M("typeSpec"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				_M("extDep"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				_M("metaData"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgRemovePort, _T("DFGRemovePort"), 0, 0, 0, 2,
				_M("portName"), 0, TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgResizeBackdrop, _T("DFGResizeBackdrop"), 0, 0, 0, 4,
				_M("backDropNodeName"), 0, TYPE_TSTR_BV,
				_M("pos"), 0, TYPE_POINT2,
				_M("size"), 0, TYPE_POINT2,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),

			SpliceTranslationFPInterface::fn_dfgMoveNodes, _T("DFGMoveNodes"), 0, 0, 0, 3,
				_M("nodeNames"), 0, TYPE_TSTR_TAB_BV,
				_M("topLeftPoss"), 0, TYPE_POINT2_TAB_BR,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgImplodeNodes, _T("DFGImplodeNodes"), 0, TYPE_TSTR_BV, 0, 3,
				_M("nodeNames"), 0, TYPE_TSTR_TAB_BV,
				_M("desiredNewNodeName"), 0, TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgExplodeNodes, _T("DFGExplodeNodes"), 0, TYPE_TSTR_TAB_BV, 0, 2,
				_M("nodeName"), 0, TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgAddBackdrop, _T("DFGAddBackdrop"), 0, 0, 0, 3,
				_M("title"), 0, TYPE_TSTR_BV,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgSetNodeTitle, _T("DFGSetNodeTitle"), 0, 0, 0, 3,
				_M("nodeName"), 0, TYPE_TSTR_BV,
				_M("newTitle"), 0, TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgSetNodeComment, _T("DFGSetNodeComment"), 0, 0, 0, 3,
				_M("nodeName"), 0, TYPE_TSTR_BV,
				_M("comment"), 0, TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgSetCode, _T("DFGSetCode"), 0, 0, 0, 2,
				_M("code"), 0, TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgRenamePort, _T("DFGRenamePort"), 0, TYPE_TSTR_BV, 0, 3,
				_M("oldName"), 0, TYPE_TSTR_BV,
				_M("newDesiredName"), 0, TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgPaste, _T("DFGPaste"), 0, TYPE_TSTR_TAB_BV, 0, 3,
				_M("json"), 0, TYPE_TSTR_BV,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgSetArgType, _T("DFGSetArgType"), 0, 0, 0, 2,
				_M("argName"), 0, TYPE_TSTR_BV,
				_M("argType"), 0, TYPE_TSTR_BV,
				
			SpliceTranslationFPInterface::fn_dfgSetArgValue, _T("DFGSetArgValue"), 0, 0, 0, 2,
				_M("argName"), 0, TYPE_TSTR_BV,
				_M("argValue"), 0, TYPE_FPVALUE_BR,
				
			SpliceTranslationFPInterface::fn_dfgSetPortDefaultValue, _T("DFGSetPortDefaultValue"), 0, 0, 0, 3,
				_M("portName"), 0, TYPE_TSTR_BV,
				_M("value"), 0, TYPE_FPVALUE_BR,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgSetRefVarPath, _T("DFGSetRefVarPath"), 0, 0, 0, 3,
				_M("refName"), 0, TYPE_TSTR_BV,
				_M("varPath"), 0, TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgReorderPorts, _T("DFGReorderPorts"), 0, 0, 0, 2,
				_T("indices"), 0, TYPE_INT_TAB_BR,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgSetExtDeps, _T("DFGSetExtDeps"), 0, 0, 0, 2,
				_M("extDeps"), 0, TYPE_TSTR_TAB_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_dfgSplitFromPreset, _T("DFGSplitFromPreset"), 0, 0, 0, 1,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),

			//////////////////////////////////////////////////////////////////////////
			// FabricMax custom functions

			SpliceTranslationFPInterface::fn_loadFromFile, _T("LoadFromFile"), 0, TYPE_bool, 0, 2, 
				_M("filename"),	0,	TYPE_FILENAME,
				_M("createMaxParams"),	0,	TYPE_bool,
			SpliceTranslationFPInterface::fn_saveToFile, _T("SaveToFile"), 0, TYPE_bool, 0, 1, 
				_M("filename"),	0,	TYPE_FILENAME,

			SpliceTranslationFPInterface::fn_exportToJson, _T("ExportToJSON"), 0, TYPE_TSTR_BV, 0, 0,
			SpliceTranslationFPInterface::fn_restoreFromJson, _T("RestoreFromJSON"), 0, TYPE_bool, 0, 2,
				_M("json"), 0, TYPE_TSTR_BV,
				_M("createMaxParams"), 0, TYPE_bool,


			SpliceTranslationFPInterface::fn_getPortName, _T("GetPortCount"), 0, TYPE_TSTR_BV, 0, 1,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
			SpliceTranslationFPInterface::fn_getPortName, _T("GetPortName"), 0, TYPE_TSTR_BV, 0, 2,
				_M("portIndex"),	0,	TYPE_INDEX,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
			SpliceTranslationFPInterface::fn_getPortType, _T("GetPortType"), 0, TYPE_TSTR_BV, 0, 2,
				_M("portName"),		0,	TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
			SpliceTranslationFPInterface::fn_getPortValue, _T("GetPortValue"), 0, TYPE_FPVALUE_BR, 0, 2,
				_M("portName"), 0, TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),

			SpliceTranslationFPInterface::fn_getMaxConnectedType, _T("GetMaxTypeForArg"), 0, TYPE_INT, 0, 1,
				_M("argName"),	0,	TYPE_TSTR_BV, 
			SpliceTranslationFPInterface::fn_setMaxConnectedType, _T("SetMaxTypeForArg"), 0, TYPE_INT, 0, 2,
				_M("argName"),	0,	TYPE_TSTR_BV, 
				_M("maxType"),	0,	TYPE_INT,
			SpliceTranslationFPInterface::fn_getLegalMaxTypes, _T("GetLegalMaxTypesForArg"), 0, TYPE_BITARRAY, 0, 1,
				_M("argName"),	0,	TYPE_TSTR_BV, 

			SpliceTranslationFPInterface::fn_setPortMetaData, _T("SetPortMetaData"), 0, TYPE_bool, 0, 4,
				_M("portName"),	0,	TYPE_TSTR_BV, 
				_M("option"),	0,	TYPE_TSTR_BV, 
				_M("value"),	0,	TYPE_TSTR_BV, 
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
			SpliceTranslationFPInterface::fn_getPortMetaData, _T("GetPortMetaData"), 0, TYPE_TSTR_BV, 0, 3,
				_M("portName"), 0, TYPE_TSTR_BV,
				_M("option"), 	0, TYPE_TSTR_BV,
				_M("execPath"), 0, TYPE_TSTR_BV, f_keyArgDefault, MSTR(),
				
			SpliceTranslationFPInterface::fn_setPortMinMax, _T("SetArgMinMax"), 0, TYPE_bool, 0, 3,
				_M("argName"),		0,	TYPE_TSTR_BV, 
				_M("uiMin"),	0,	TYPE_FPVALUE, 
				_M("uiMax"),	0,	TYPE_FPVALUE, 

			SpliceTranslationFPInterface::fn_connectArgs, _T("ConnectArgs"), 0, TYPE_bool, 0, 5,
				_M("myPortName"),	0,	TYPE_TSTR_BV,
				_M("srcSpliceGraph"),	0,	TYPE_REFTARG,
				_M("srcPortName"),	0,	TYPE_TSTR_BV,
				_M("srcPortIndex"),	0,	TYPE_INT, f_keyArgDefault, -1,
				_M("postUI"),		0,	TYPE_bool, f_keyArgDefault, true,

		properties,
			SpliceTranslationFPInterface::prop_getOutPortName, SpliceTranslationFPInterface::prop_SetOutPortName, _T("OutPort"), 0, TYPE_TSTR_BV,

		enums,
			SpliceTranslationFPInterface::port_mode_enum, 3,
				_T("IN"), FabricCore::DFGPortType_In,
				_T("IO"), FabricCore::DFGPortType_IO,
				_T("OUT"), FabricCore::DFGPortType_Out,
		p_end
		);
	return &_ourDesc;
}
