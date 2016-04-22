#pragma once

#include <ifnpub.h>
#include "MaxDFGCmdHandler.h"

#define ISPLICE__INTERFACE Interface_ID(0x5a5f19c9, 0x25881449)

class DockableWindow;

// We need to make this function a templated type, just so
// we can get multiple compilations for our different clients.
// This is necessary, as there needs to be a unique instance of
// the class created for each client, and each implementation
// of GetDesc needs to return the unique type.
class FabricTranslationFPInterface : public FPMixinInterface {
private:
	Value* m_klEditor;

	DockableWindow* m_pDFGWidgetWindow;

protected:

	// Binding to a DFG graph   
	FabricCore::DFGBinding m_binding;

	// The command handler to route commands into 
	MaxDFGCmdHandler m_fabricCmdHandler;

public:

	FabricTranslationFPInterface();
	~FabricTranslationFPInterface();

	// The follow exposes our functions to Max
	enum FN_IDS {
		// System management callbacks
		fn_showDFGGraphEditor,
		fn_closeDFGGraphEditor,

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
		fn_dfgSetTitle,
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
		fn_getArgValue,

		fn_hasSrcPort,
		fn_hasDstPort,

		fn_getNodeCount,
		fn_getNodeName,
		fn_getNodeType,

		fn_getMaxConnectedType,
		fn_setMaxConnectedType,
		fn_getLegalMaxTypes,

		fn_setPortMetaData,
		fn_getPortMetaData,
		fn_setArgMinMax,

		fn_getExecCode,

		fn_connectArgs,

		prop_getOutPortName,
		prop_SetOutPortName,

		num_params,

		port_mode_enum
	};

	BEGIN_FUNCTION_MAP	
		FN_0(fn_showDFGGraphEditor, TYPE_BOOL, ShowDFGGraphEditor);
		VFN_0(fn_closeDFGGraphEditor, CloseDFGGraphEditor);

		VFN_2(fn_dfgRemoveNodes,						DFGRemoveNodes,			TYPE_TSTR_TAB_BV,	TYPE_TSTR);
		VFN_3(fn_dfgConnect,							DFGConnect,				TYPE_TSTR,		TYPE_TSTR, TYPE_TSTR);
		VFN_3(fn_dfgDisconnect,							DFGDisconnect,			TYPE_TSTR,		TYPE_TSTR, TYPE_TSTR);
		FN_3(fn_dfgAddGraph,		TYPE_TSTR_BV,		DFGAddGraph,			TYPE_TSTR,		TYPE_POINT2, TYPE_TSTR);
		FN_4(fn_dfgAddFunc,			TYPE_TSTR_BV,		DFGAddFunc,				TYPE_TSTR,		TYPE_TSTR, TYPE_POINT2, TYPE_TSTR);
		FN_3(fn_dfgInstPreset,		TYPE_TSTR_BV,		DFGInstPreset,			TYPE_TSTR,		TYPE_POINT2, TYPE_TSTR);
		FN_5(fn_dfgAddVar,			TYPE_TSTR_BV,		DFGAddVar,				TYPE_TSTR,		TYPE_TSTR, TYPE_TSTR, TYPE_POINT2, TYPE_TSTR);
		FN_4(fn_dfgAddGet,			TYPE_TSTR_BV,		DFGAddGet,				TYPE_TSTR,		TYPE_TSTR, TYPE_POINT2, TYPE_TSTR);
		FN_4(fn_dfgAddSet,			TYPE_TSTR_BV,		DFGAddSet,				TYPE_TSTR,		TYPE_TSTR, TYPE_POINT2, TYPE_TSTR);
		FN_7(fn_dfgAddPort,			TYPE_TSTR_BV,		DFGAddPort,				TYPE_TSTR,		TYPE_ENUM, TYPE_TSTR, TYPE_TSTR, TYPE_TSTR, TYPE_TSTR, TYPE_TSTR);
		FN_6(fn_dfgEditPort,		TYPE_TSTR_BV,		DFGEditPort,			TYPE_TSTR,		TYPE_TSTR, TYPE_TSTR, TYPE_TSTR, TYPE_TSTR, TYPE_TSTR);
		VFN_2(fn_dfgRemovePort,							DFGRemovePort,			TYPE_TSTR,		TYPE_TSTR);
		VFN_4(fn_dfgResizeBackdrop,						DFGResizeBackdrop,		TYPE_TSTR,		TYPE_POINT2, TYPE_POINT2, TYPE_TSTR);
		VFN_3(fn_dfgMoveNodes,							DFGMoveNodes,			TYPE_TSTR_TAB_BV,	TYPE_POINT2_TAB_BR, TYPE_TSTR);
		FN_3(fn_dfgImplodeNodes,	TYPE_TSTR_BV,		DFGImplodeNodes,		TYPE_TSTR_TAB_BV,	TYPE_TSTR, TYPE_TSTR);
		FN_2(fn_dfgExplodeNodes,	TYPE_TSTR_TAB_BV,	DFGExplodeNodes,		TYPE_TSTR,		TYPE_TSTR);
		VFN_3(fn_dfgAddBackdrop,						DFGAddBackdrop,			TYPE_TSTR,		TYPE_POINT2, TYPE_TSTR);
		VFN_3(fn_dfgSetNodeComment,						DFGSetNodeComment,		TYPE_TSTR,		TYPE_TSTR, TYPE_TSTR);
		VFN_2(fn_dfgSetCode,							DFGSetCode,				TYPE_TSTR,		TYPE_TSTR);
		FN_3(fn_dfgRenamePort,		TYPE_TSTR_BV,		DFGRenamePort,			TYPE_TSTR,		TYPE_TSTR, TYPE_TSTR);
		FN_3(fn_dfgPaste,			TYPE_TSTR_TAB_BV,	DFGPaste,				TYPE_TSTR,		TYPE_POINT2, TYPE_TSTR);
		VFN_2(fn_dfgSetArgValue,						DFGSetArgValue,			TYPE_TSTR,		TYPE_FPVALUE);
		VFN_3(fn_dfgSetPortDefaultValue,				DFGSetPortDefaultValue,	TYPE_TSTR,		TYPE_FPVALUE, TYPE_TSTR);
		VFN_3(fn_dfgSetRefVarPath,						DFGSetRefVarPath,		TYPE_TSTR,		TYPE_TSTR, TYPE_TSTR);
		VFN_2(fn_dfgReorderPorts,						DFGReorderPorts,		TYPE_INT_TAB_BR,	TYPE_TSTR);
		VFN_2(fn_dfgSetExtDeps,							DFGSetExtDeps,			TYPE_TSTR_TAB_BV,	TYPE_TSTR);
		VFN_1(fn_dfgSplitFromPreset,					DFGSplitFromPreset,		TYPE_TSTR)		


		FN_2(fn_loadFromFile, TYPE_bool, LoadFromFile, TYPE_FILENAME, TYPE_bool);
		FN_1(fn_saveToFile, TYPE_bool, SaveToFile, TYPE_FILENAME);

		FN_2(fn_restoreFromJson, TYPE_bool, RestoreFromJSON, TYPE_TSTR, TYPE_bool);
		FN_0(fn_exportToJson, TYPE_TSTR_BV, ExportToJSON);

		FN_1(fn_getPortCount, TYPE_INT, GetPortCount, TYPE_TSTR);
		FN_2(fn_getPortName, TYPE_TSTR_BV, GetPortName, TYPE_INDEX, TYPE_TSTR);
		FN_2(fn_getPortType, TYPE_TSTR_BV, GetPortType, TYPE_TSTR, TYPE_TSTR);
		FN_2(fn_getArgValue, TYPE_FPVALUE_BR, GetArgValue, TYPE_TSTR, TYPE_TSTR);

		FN_1(fn_hasSrcPort, TYPE_bool, HasSrcPort, TYPE_TSTR);
		FN_1(fn_hasDstPort, TYPE_bool, HasDstPort, TYPE_TSTR);

		FN_1(fn_getNodeCount, TYPE_INT, GetNodeCount, TYPE_TSTR);
		FN_2(fn_getNodeName, TYPE_TSTR_BV, GetNodeName, TYPE_INDEX, TYPE_TSTR);
		FN_2(fn_getNodeType, TYPE_INT, GetNodeType, TYPE_TSTR, TYPE_TSTR);
		
		FN_1(fn_getMaxConnectedType, TYPE_INT, GetMaxTypeForArg, TYPE_TSTR);
		FN_2(fn_setMaxConnectedType, TYPE_INT, SetMaxTypeForArg, TYPE_TSTR, TYPE_INT);
		FN_1(fn_getLegalMaxTypes, TYPE_BITARRAY_BV, GetLegalMaxTypesForArg, TYPE_TSTR);

		FN_4(fn_setPortMetaData, TYPE_bool, SetPortMetaData, TYPE_TSTR, TYPE_TSTR, TYPE_TSTR, TYPE_TSTR);
		FN_3(fn_getPortMetaData, TYPE_TSTR_BV, GetPortMetaData, TYPE_TSTR, TYPE_TSTR, TYPE_TSTR);

		FN_3(fn_setArgMinMax, TYPE_bool, SetArgUIMinMax, TYPE_TSTR, TYPE_FPVALUE, TYPE_FPVALUE);

		FN_1(fn_getExecCode, TYPE_TSTR_BV, GetExecCode, TYPE_TSTR);

		FN_4(fn_connectArgs, TYPE_bool, ConnectArgs, TYPE_TSTR, TYPE_REFTARG, TYPE_TSTR, TYPE_bool);
		
		// Properties 
		PROP_FNS(prop_getOutPortName, GetOutPortName, prop_SetOutPortName, SetOutPortName, TYPE_TSTR_BV);
		
	END_FUNCTION_MAP

	// Implement the functions exposed above
	BOOL ShowDFGGraphEditor();
	void CloseDFGGraphEditor();

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
	void DFGSetNodeComment(const MSTR& nodeName, const MSTR& comment, const MSTR& execPath);
	void DFGSetCode(const MSTR& code, const MSTR& execPath);
	MSTR DFGRenamePort(const MSTR& oldName, const MSTR& newDesiredName, const MSTR& execPath);
	Tab<TSTR*> DFGPaste(const MSTR& json, Point2 pos, const MSTR& execPath);
	void DFGSetArgValue(const MSTR& argName, const FPValue* argValue);
	void DFGSetPortDefaultValue(const MSTR& portName, const FPValue* value, const MSTR& execPath);
	void DFGSetRefVarPath(const MSTR& refName, const MSTR& varPath, const MSTR& execPath);
	void DFGReorderPorts(Tab<int> indices, const MSTR& execPath);
	void DFGSetExtDeps(Tab<TSTR*> extDeps, const MSTR& execPath);
	void DFGSplitFromPreset(const MSTR& execPath);

	//////////////////////////////////////////////////////////////////////////
	// Allow introspecting the ports on this graph
	int GetPortCount(const MSTR& execPath);
	MSTR GetPortName(int i, const MSTR& execPath);
	const char* GetPortType(const char* portName, const char* execPath = "");
	MSTR GetPortType(const MSTR& portName, const MSTR& execPath);

	bool HasSrcPort(const MSTR& portName);
	bool HasSrcPort(const char* portName);
	bool HasDstPort(const MSTR& portName);
	bool HasDstPort(const char* portName);


	int GetNodeCount(const MSTR& execPath);
	MSTR GetNodeName(int i, const MSTR& execPath);
	int GetNodeType(const MSTR& nodeName, const MSTR& execPath);

	bool GetArgValue(const char* argName, FPValue& value);
	const FPValue& GetArgValue(const MSTR& portName, const MSTR& execPath);
	// Returns if the in port is an array type or not
	//virtual bool IsPortArray(const char* port)=0;

	int GetMaxTypeForArg(const char* argName);
	int GetMaxTypeForArg(const MSTR& argName);

	int SetMaxTypeForArg(const char* argName, int type);
	int SetMaxTypeForArg(const MSTR& argName, int type);

	BitArray GetLegalMaxTypesForArg(const MSTR& argName);

	// Allow setting various options on our ports
	bool SetPortMetaData(const char* port, const char* option, const char* value, const char* execPath="");
	bool SetPortMetaData(const MSTR& port, const MSTR& option, const MSTR& value, const MSTR& execPath);

	const char* GetPortMetaData(const char* port, const char* option, const char* execPath = "");
	MSTR GetPortMetaData(const MSTR& port, const MSTR& option, const MSTR& execPath);

	// Convenience functions
	bool SetArgUIMinMax(const MSTR& port, FPValue* uiMin, FPValue* uiMax);
	int GetPortParamID(const char* argName);
	bool SetPortParamID(const char* argName, int id);
	int GetPort3dsMaxType(const char* argName);
	bool SetPort3dsMaxType(const char* argName, int type);

	// Load the splice graph for this entity from the given filename
	bool LoadFromFile(const MCHAR* filename, bool createMaxParams);
	bool SaveToFile(const MCHAR* filename);

	bool RestoreFromJSON(const MSTR& json, bool createMaxParams);
	bool RestoreFromJSON(const char* json, bool createMaxParams);
	MSTR ExportToJSON();

	MSTR GetExecCode(const MSTR& execPath);

	// Connect myPortName to the output port on pSrcContainer named srcPortName
	// Returns true if successfully connected, false if for any reason the
	// port was not connected.  Once connected, each evaluation the output
	// from srcPortName will be transferred into the in-port myPortName
	virtual bool ConnectArgs(const MSTR& myPortName, ReferenceTarget* pSrcContainer, const MSTR& srcPortName, bool postConnectionsUI) = 0;
	// Disconnect a previously connected port.  Returns true if the port was previously connected and
	// has been successfully disconnected, false if disconnect failed or if no connection existed.
	virtual bool DisconnectArgs(const MSTR& myPortName) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Props
	virtual MSTR GetOutPortName() = 0;
	virtual bool SetOutPortName(const MSTR& name) = 0;

	//////////////////////////////////////////////////////////////////////////
	// General management functions allows different templated
	// types to speak to each other.
	
	/*! Gets the type of the data evaluated by this class
	This return value needs to match the TDataType template param */
	virtual int GetValueType() = 0;

	virtual MSTR GetGraphName();

	// Reset any auto-generated ports.
	virtual void ResetPorts()=0;

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
	virtual void SyncMaxParamName(const char* dfgPort, int paramId) = 0;
	virtual void SyncMaxParamLimits(const char* dfgPort, int paramId) = 0;
	virtual void SyncMaxParamDefault(const char* dfgPort, int paramId) = 0;

	//////////////////////////////////////////////////////////////////////////
	// Access a few of the internals
	FabricCore::DFGBinding& GetBinding() { return m_binding; }
	const FabricCore::DFGBinding& GetBinding() const { return m_binding; }
	void SetBinding(FabricCore::DFGBinding binding);
	MaxDFGCmdHandler* GetCommandHandler();

	FabricCore::DFGExec GetExec(const char* execPath);
	FabricCore::DFGExec GetExec(const MSTR& execPath);

#pragma endregion
};

// Inline fn for easy access to interface
FabricTranslationFPInterface* GetFabricInterface(ReferenceTarget* pTarg);

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
		FabricTranslationLayer<TBaseClass, TResultType>::GetClassDesc(), 
		0,
		// Describe our function(s)
			FabricTranslationFPInterface::fn_showDFGGraphEditor, _T("ShowDFGGraphEditor"), 0, TYPE_BOOL, 0, 0,
			FabricTranslationFPInterface::fn_closeDFGGraphEditor, _T("CloseDFGGraphEditor"), 0, TYPE_BOOL, 0, 0,

			//////////////////////////////////////////////////////////////////////////
			// Fabric DFG commands
			FabricTranslationFPInterface::fn_dfgRemoveNodes, _T("DFGRemoveNodes"), 0, 0, 0, 2,
				_M("nodeNames"), 0, TYPE_TSTR_TAB_BV,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgConnect, _T("DFGConnect"), 0, 0, 0, 3,
				_M("srcPath"), 0, TYPE_TSTR,
				_M("destPath"), 0, TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgDisconnect, _T("DFGDisconnect"), 0, 0, 0, 3,
				_M("srcPath"), 0, TYPE_TSTR,
				_M("destPath"), 0, TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgAddGraph, _T("DFGAddGraph"), 0, TYPE_TSTR_BV, 0, 3,
				_M("title"), 0, TYPE_TSTR,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgAddFunc, _T("DFGAddFunc"), 0, TYPE_TSTR_BV, 0, 4,
				_M("title"), 0, TYPE_TSTR,
				_M("initialCode"), 0, TYPE_TSTR,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgInstPreset, _T("DFGInstPreset"), 0, TYPE_TSTR_BV, 0, 3,
				_M("filename"), 0, TYPE_TSTR,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgAddVar, _T("DFGAddVar"), 0, TYPE_TSTR_BV, 0, 5,
				_M("desiredNodeName"), 0, TYPE_TSTR,
				_M("dataType"), 0, TYPE_TSTR,
				_M("extDep"), 0, TYPE_TSTR,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgAddSet, _T("DFGAddSet"), 0, TYPE_TSTR_BV, 0, 4,
				_M("desiredNodeName"), 0, TYPE_TSTR,
				_M("varPath"), 0, TYPE_TSTR,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgAddGet, _T("DFGAddGet"), 0, TYPE_TSTR_BV, 0, 4,
				_M("desiredNodeName"), 0, TYPE_TSTR,
				_M("varPath"), 0, TYPE_TSTR,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgAddPort, _T("DFGAddPort"), 0, TYPE_TSTR_BV, 0, 7,
				_M("desiredPortName"), 0, TYPE_TSTR,
				_M("portType"), 0, TYPE_ENUM, FabricTranslationFPInterface::port_mode_enum,
				_M("portSpec"), 0, TYPE_TSTR,
				_M("portToConnect"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				_M("extDep"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				_M("metaData"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgEditPort, _T("DFGEditPort"), 0, TYPE_TSTR_BV, 0, 6,
				_M("portName"), 0, TYPE_TSTR,
				_M("desiredNewPortName"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				_M("typeSpec"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				_M("extDep"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				_M("metaData"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgRemovePort, _T("DFGRemovePort"), 0, 0, 0, 2,
				_M("portName"), 0, TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgResizeBackdrop, _T("DFGResizeBackdrop"), 0, 0, 0, 4,
				_M("backDropNodeName"), 0, TYPE_TSTR,
				_M("pos"), 0, TYPE_POINT2,
				_M("size"), 0, TYPE_POINT2,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),

			FabricTranslationFPInterface::fn_dfgMoveNodes, _T("DFGMoveNodes"), 0, 0, 0, 3,
				_M("nodeNames"), 0, TYPE_TSTR_TAB_BV,
				_M("topLeftPoss"), 0, TYPE_POINT2_TAB_BR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgImplodeNodes, _T("DFGImplodeNodes"), 0, TYPE_TSTR_BV, 0, 3,
				_M("nodeNames"), 0, TYPE_TSTR_TAB_BV,
				_M("desiredNewNodeName"), 0, TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgExplodeNodes, _T("DFGExplodeNode"), 0, TYPE_TSTR_TAB_BV, 0, 2,
				_M("nodeName"), 0, TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgAddBackdrop, _T("DFGAddBackdrop"), 0, 0, 0, 3,
				_M("title"), 0, TYPE_TSTR,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgSetTitle, _T("DFGSetTitle"), 0, 0, 0, 2,
				_M("newTitle"), 0, TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgSetNodeComment, _T("DFGSetNodeComment"), 0, 0, 0, 3,
				_M("nodeName"), 0, TYPE_TSTR,
				_M("comment"), 0, TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgSetCode, _T("DFGSetCode"), 0, 0, 0, 2,
				_M("code"), 0, TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgRenamePort, _T("DFGRenamePort"), 0, TYPE_TSTR_BV, 0, 3,
				_M("oldName"), 0, TYPE_TSTR,
				_M("newDesiredName"), 0, TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgPaste, _T("DFGPaste"), 0, TYPE_TSTR_TAB_BV, 0, 3,
				_M("json"), 0, TYPE_TSTR,
				_M("pos"), 0, TYPE_POINT2, 
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgSetArgType, _T("DFGSetArgType"), 0, 0, 0, 2,
				_M("argName"), 0, TYPE_TSTR,
				_M("argType"), 0, TYPE_TSTR,
				
			FabricTranslationFPInterface::fn_dfgSetArgValue, _T("DFGSetArgValue"), 0, 0, 0, 2,
				_M("argName"), 0, TYPE_TSTR,
				_M("argValue"), 0, TYPE_FPVALUE_BR,
				
			FabricTranslationFPInterface::fn_dfgSetPortDefaultValue, _T("DFGSetPortDefaultValue"), 0, 0, 0, 3,
				_M("portName"), 0, TYPE_TSTR,
				_M("value"), 0, TYPE_FPVALUE_BR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgSetRefVarPath, _T("DFGSetRefVarPath"), 0, 0, 0, 3,
				_M("refName"), 0, TYPE_TSTR,
				_M("varPath"), 0, TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgReorderPorts, _T("DFGReorderPorts"), 0, 0, 0, 2,
				_T("indices"), 0, TYPE_INT_TAB_BR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgSetExtDeps, _T("DFGSetExtDeps"), 0, 0, 0, 2,
				_M("extDeps"), 0, TYPE_TSTR_TAB_BV,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_dfgSplitFromPreset, _T("DFGSplitFromPreset"), 0, 0, 0, 1,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),

			//////////////////////////////////////////////////////////////////////////
			// FabricMax custom functions

			FabricTranslationFPInterface::fn_loadFromFile, _T("LoadFromFile"), 0, TYPE_bool, 0, 2, 
				_M("filename"),	0,	TYPE_FILENAME,
				_M("createMaxParams"),	0,	TYPE_bool,
			FabricTranslationFPInterface::fn_saveToFile, _T("SaveToFile"), 0, TYPE_bool, 0, 1, 
				_M("filename"),	0,	TYPE_FILENAME,

			FabricTranslationFPInterface::fn_exportToJson, _T("ExportToJSON"), 0, TYPE_TSTR_BV, 0, 0,
			FabricTranslationFPInterface::fn_restoreFromJson, _T("RestoreFromJSON"), 0, TYPE_bool, 0, 2,
				_M("json"), 0, TYPE_TSTR,
				_M("createMaxParams"), 0, TYPE_bool,


			FabricTranslationFPInterface::fn_getPortCount, _T("GetPortCount"), 0, TYPE_TSTR_BV, 0, 1,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
			FabricTranslationFPInterface::fn_getPortName, _T("GetPortName"), 0, TYPE_TSTR_BV, 0, 2,
				_M("portIndex"),	0,	TYPE_INDEX,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
			FabricTranslationFPInterface::fn_getPortType, _T("GetPortType"), 0, TYPE_TSTR_BV, 0, 2,
				_M("portName"),		0,	TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
			FabricTranslationFPInterface::fn_getArgValue, _T("GetArgValue"), 0, TYPE_FPVALUE_BR, 0, 2,
				_M("portName"), 0, TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),

			FabricTranslationFPInterface::fn_hasSrcPort, _T("HasSrcPort"), 0, TYPE_bool, 0, 1,
				_M("portName"), 0, TYPE_TSTR,
			FabricTranslationFPInterface::fn_hasDstPort, _T("HasDstPort"), 0, TYPE_bool, 0, 1,
				_M("portName"), 0, TYPE_TSTR,

			FabricTranslationFPInterface::fn_getNodeCount, _T("GetNodeCount"), 0, TYPE_TSTR_BV, 0, 1,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
			FabricTranslationFPInterface::fn_getNodeName, _T("GetNodeName"), 0, TYPE_TSTR_BV, 0, 2,
				_M("nodeIndex"),	0,	TYPE_INDEX,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
			FabricTranslationFPInterface::fn_getNodeType, _T("GetNodeType"), 0, TYPE_INT, 0, 2,
				_M("nodeName"),		0,	TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),

			FabricTranslationFPInterface::fn_getMaxConnectedType, _T("GetMaxTypeForArg"), 0, TYPE_INT, 0, 1,
				_M("argName"),	0,	TYPE_TSTR, 
			FabricTranslationFPInterface::fn_setMaxConnectedType, _T("SetMaxTypeForArg"), 0, TYPE_INT, 0, 2,
				_M("argName"),	0,	TYPE_TSTR, 
				_M("maxType"),	0,	TYPE_INT,
			FabricTranslationFPInterface::fn_getLegalMaxTypes, _T("GetLegalMaxTypesForArg"), 0, TYPE_BITARRAY, 0, 1,
				_M("argName"),	0,	TYPE_TSTR, 

			FabricTranslationFPInterface::fn_setPortMetaData, _T("SetPortMetaData"), 0, TYPE_bool, 0, 4,
				_M("portName"),	0,	TYPE_TSTR, 
				_M("option"),	0,	TYPE_TSTR, 
				_M("value"),	0,	TYPE_TSTR, 
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
			FabricTranslationFPInterface::fn_getPortMetaData, _T("GetPortMetaData"), 0, TYPE_TSTR_BV, 0, 3,
				_M("portName"), 0, TYPE_TSTR,
				_M("option"), 	0, TYPE_TSTR,
				_M("execPath"), 0, TYPE_TSTR, f_keyArgDefault, EmptyStr(),
				
			FabricTranslationFPInterface::fn_setArgMinMax, _T("SetArgMinMax"), 0, TYPE_bool, 0, 3,
				_M("argName"),		0,	TYPE_TSTR, 
				_M("uiMin"),	0,	TYPE_FPVALUE, 
				_M("uiMax"),	0,	TYPE_FPVALUE, 

			FabricTranslationFPInterface::fn_getExecCode, _T("GetExecCode"), 0, TYPE_TSTR_BV, 0, 1,
				_M("execPath"), 0, TYPE_TSTR,
				
			FabricTranslationFPInterface::fn_connectArgs, _T("ConnectArgs"), 0, TYPE_bool, 0, 4,
				_M("myPortName"),	0,	TYPE_TSTR,
				_M("srcFabricGraph"),	0,	TYPE_REFTARG,
				_M("srcPortName"),	0,	TYPE_TSTR,
				_M("postUI"),		0,	TYPE_bool, f_keyArgDefault, true,

		properties,
			FabricTranslationFPInterface::prop_getOutPortName, FabricTranslationFPInterface::prop_SetOutPortName, _T("OutPort"), 0, TYPE_TSTR,

		enums,
			FabricTranslationFPInterface::port_mode_enum, 3,
				s_PortTypeEnumIN.data(), FabricCore::DFGPortType_In,
				s_PortTypeEnumIO.data(), FabricCore::DFGPortType_IO,
				s_PortTypeEnumOUT.data(), FabricCore::DFGPortType_Out,
		p_end
		);
	return &_ourDesc;
}
