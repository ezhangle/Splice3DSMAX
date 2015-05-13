//////////////////////////////////////////////////////////////////////////
// SpliceTranslationLayer
//
// Description:
//	This file describes the translation layer between
//	Max native datatypes and inputs and the Splice System.
//	It is responsible for managing the input/output of values
//	from the Splice system.  This class is purely usage agnostic,
//	and can be applied to any of Max's native interface types.
//	However, any usage-specific interface functions cannot 
//	be implemented here.

#pragma once

//////////////////////////////////////////////////////////////////////////
// includes and forward declarations

#include <vector>
#include "SpliceTranslationFPInterface.h"
#include "MaxConversionFns.h"

namespace DynamicDialog
{
	class CDynamicDialogTemplate;
};
class IParamBlock2;
class DynPBCustAttrClassDesc;

// Stores a connection between Max data (ParamID) and Splice data (DGPort)
//typedef std::pair<ParamID, DFGWrapper::PortPtr> ConnData;
#define MAX_PID_OPT "MaxPID"
#define MAX_SRC_OPT "SrcPort"
#define MAX_SRC_IDX_OPT "SrcIdx"
#define MAX_POST_UI_OPT "SrcPostUI"
//////////////////////////////////////////////////////////////////////////
#pragma region Utility functions
/** Add a new parameter definition to the given pblock descriptor
	This function will search out the next available ID, and set this
	to the appropriate parameter type.
	\pDesc - The descriptor to add the parameter too.  Its important that
		this has not been used to create a parameter block yet.
	\type - The type of parameter to create
	\return The ID of the newly created parameter definition */
ParamID AddMaxParameter(ParamBlockDesc2* pDesc, int type, const MCHAR* sName, ParamID desiredId=-1 );
ParamID AddMaxParameter(ParamBlockDesc2* pDesc, int type, const char* cName );
void SetMaxParamName(ParamBlockDesc2* pDesc, ParamID pid, const MCHAR* name);
void SetMaxParamLimits(ParamBlockDesc2* pDesc, ParamID pid, DFGWrapper::PortPtr& port);
void SetMaxParamDefault(ParamBlockDesc2* pDesc, ParamID pid, DFGWrapper::PortPtr& port);

/*! Generate a Win32 dialog for the passed pblok
	\param pblock - The list of parameters to generate UI for
	\return the new dialog if successful, else NULL. */
DynamicDialog::CDynamicDialogTemplate* GeneratePBlockUI(IParamBlock2* pblock);

/** Create a new parameter block, copying the 
	values off similarly named parameters on pCopyThis where possible */
IParamBlock2* CreateParamBlock(ParamBlockDesc2* pDesc, IParamBlock2* pCopyThis, ReferenceTarget* pOwner);

/** Get all Max types that can be converted to the given Splice type */
BitArray SpliceTypeToMaxTypes(const char* spliceType);

/** Returns the Max ParamType2 that matches the named Splice type (NOTE - may not be legal for PB2) */
int SpliceTypeToMaxType(const char* cType, bool isArray=false);

/** Returns the default Max ParamType2 that matches the named Splice type */
int SpliceTypeToDefaultMaxType(const char* cType);

/** Add a new value port to the given DGGraph 
	This adds in the appropriate Splice type to recieve
	the Max parameter type, and connects the port.  The return
	value should be cached as the port top set/get values
	from the splice parameter 
	\rGraph - a reference to the Splice sub-graph to add the parameter too
	\type - The Max type that will be set 
	\pName - The name of the parameter
	\mode - whether this is a read/write variable */
const DFGWrapper::PortPtr AddSpliceParameter(DFGWrapper::Binding& rBinding, const char* type, const char* cName, FabricCore::DFGPortType mode, bool isArray = false, const char* inExtension="");
const DFGWrapper::PortPtr AddSpliceParameter(DFGWrapper::Binding& rBinding, int type, const MCHAR* pName, FabricCore::DFGPortType mode);
const DFGWrapper::PortPtr AddSpliceParameter(DFGWrapper::Binding& rBinding, int type, const char* pName, FabricCore::DFGPortType mode, bool isArray = false, const char* inExtension="");

/** Get various useful info's from fabric Ports */
int GetPortParamID(DFGWrapper::PortPtr& aPort);
void SetPortParamID(DFGWrapper::PortPtr& aPort, ParamID id);
std::string GetPortConnection(DFGWrapper::PortPtr& aPort);
void SetPortConnection(DFGWrapper::PortPtr& aPort, const char* name);
//int GetPortConnectionIndex(DFGWrapper::PortPtr& aPort);
//void SetPortConnectionIndex(DFGWrapper::PortPtr& aPort, int index);
//bool GetPortPostConnectionUI(DFGWrapper::PortPtr& aPort);
//void SetPortPostConnectionUI(DFGWrapper::PortPtr& aPort, bool postUi);
const char* GetPortName(DFGWrapper::PortPtr& aPort);
const char* GetPortType(DFGWrapper::PortPtr& aPort);

bool SetPortOption(DFGWrapper::PortPtr& aPort, const char* option, FPValue* value);
bool SetPortValue(DFGWrapper::PortPtr& aPort, FPValue* value);

/** Given a Max value, send it to the dgPort in the appropriate fashion */

/** For each valid in the parameter block, send it to the appropriate Splice port in paramData */
void TransferAllMaxValuesToSplice(TimeValue t, IParamBlock2* pblock, FabricCore::Client& client, DFGWrapper::Binding& rBinding, std::vector<Interval>& paramValidities, Interval& ivValid);

/** Callback from DFG actions */
void bindingNotificationCallback(void * userData, char const *jsonCString, uint32_t jsonLength);

// DlgCallback for our static UI
INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma endregion

//////////////////////////////////////////////////////////////////////////
template<typename TBaseClass, typename TResultType>
class SpliceTranslationLayer : public TBaseClass, public SpliceTranslationFPInterface, public FabricServices::DFGWrapper::View
{
public:
	// Use this typedef within a derivation to refer to the parent class (eg CloneSpliceData)
	typedef SpliceTranslationLayer<TBaseClass, TResultType> ParentClass;

protected:
#pragma region Max Parameter Management data
	// This is the input datablock for the Splice System.
	// Input parameters are constructed and placed in this
	// block.  The parameters are evaluated and passed to the
	// splice graph for evaluation.
	IParamBlock2* m_pblock;

	//! Stores the parameter connection data between Max and Splice.
	//! The ParamID identifies the parameter in the Max parameter block,
	//! and the DGPort connects to the port in Splice
	//std::vector<ConnData>						m_dParamData;

	//! a handle to our static UI.  This UI panel contains the controls to modify the parameter block
	HWND										m_hPanel;

	//! A handle to the UI interface passed in in BeginEditParams
	//! These cannot be static because our UI is not similar with
	//! classes of other types, and we need a way to detect if 
	//! this classes UI has been posted (See SetReference)
	Interface*									m_pInterface;
	IMtlParams*									m_pMtlInterface;

	//! A helper class to dynamically generate the dialog resource for the parameter block
	DynamicDialog::CDynamicDialogTemplate		*m_dialogTemplate;
	//! IParamMap2 is the glue that holds the actual UI dialog generated and the Parameter Block.
	//! This is returned when we post the dialog generated by m_dialogTemplate, and is valid till we release it.
	IParamMap2*									m_paramMap;

	//! This parameter stores our last evaluated result
	TResultType									m_value;

	//! We cache based on Max's animation validity intervals
	Interval m_valid;

#pragma endregion

#pragma region Splice Connections

	// Binding to a DFG
	FabricServices::DFGWrapper::Binding m_binding;
	FabricCore::Client m_client;
	FabricServices::DFGWrapper::Host* m_host;
	DFGWrapper::PortPtr	m_valuePort;

	// We store a vector recording the Max validity
	// of each parameter.  We only reset data on Splice
	// if the parameter has actually changed.
	std::vector<Interval> m_portValidities;
#pragma endregion

public:

	SpliceTranslationLayer(BOOL loading);
	~SpliceTranslationLayer();

	// Call this function to initialize splice graph
	// Returns true if work was performed (initialization was
	// necessary) or false if no work was performed
	// (was already initialized).
	bool Init(BOOL loading);

#pragma region Max-required functions
	// These functions allow a uniform exposure of our
	// parameters for all types to max

	// parameter block access etc

	// This function is used to determine whether the descriptor passed applies to our pblock
	// For classes which do not instantiate every parameter block described, this function needs to be
	// implemented so max can determine which descriptors stored in our ClassDesc2 apply to this instance.
	// This is mainly used for maxscript.
	bool						IsParamBlockDesc2Used(ParamBlockDesc2 * desc) { return (m_pblock != NULL) && (m_pblock->GetDesc() == desc); }
	int							NumParamBlocks()					{ return 1; }								// return number of ParamBlocks in this instance
	IParamBlock2*				GetParamBlock(int i)				{ return (i == 0) ? m_pblock : NULL; }		// return i'th ParamBlock
	IParamBlock2*				GetParamBlockByID(BlockID id)		{ return (m_pblock == NULL || m_pblock->ID() == id) ? m_pblock : NULL; }	// return id'd ParamBlock

	// UI Begin/End
	void MaybeRemoveParamUI();
	void MaybePostParamUI();
	void BeginEditParams(IObjParam *ip, ULONG flags, Animatable *prev = NULL);
	void EndEditParams(IObjParam *ip, ULONG flags, Animatable *next = NULL);
	ParamDlg* CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp); // Creates UI for the material editor
	ParamDlg* CreateMyAutoParamDlg(HWND hwMtlEdit, IMtlParams* pMtlParams);

	// Reference management.  We only reference 1 item (the parameter block)
	int							NumRefs()							{ return 1;}
	virtual RefTargetHandle		GetReference(int i)					{ return (i == 0) ? m_pblock : NULL; }
	virtual void				SetReference(int i, RefTargetHandle rtarg);
	
#if MAX_VERSION_MAJOR < 17 // Max 2015 is ver 17
	RefResult					NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message);
#else
	RefResult					NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate);
#endif

	// We use the RefDeleted callback to detect when we are 
	// removed from the scene graph so we can take down the KLEditor
	void						RefDeleted();

	// We use RefAdded t detect wehn we can update the name of the graph in the eval
	// context. Only once we have a reference can we lookup the INode and get the name. 
	void						RefAdded(	RefMakerHandle 	rm);

	// Sub-anims are the entities that appear in the TrackView
	virtual	int					NumSubs()							{ return 1; }
	virtual	Animatable*			SubAnim( int i )					{ return (i == 0) ? m_pblock : NULL; }
	virtual MSTR				SubAnimName( int UNUSED(i) )		{ return _T("pblock"); }

	// Save and Load.
	IOResult					Save(ISave *isave);
	IOResult					Load(ILoad *iload);
	void						ReconnectPostLoad();

	ReferenceTarget*			Clone(RemapDir &remap);
	// Delete this virtual function ensures memory is de-allocated 
	// in the same dll it is allocated from
	void						DeleteThis()						{ delete this; }
#pragma endregion

	/*! Gets the type of the data evaluated by this class
		This return value needs to match the TDataType template param */
	virtual int GetValueType() = 0;

	/*! Returns the label previously stored for a given parameter ID
		\param index - The index of the parameter to return the string for */
	MSTR		GetUIString(int index) { return (index < GetPortCount()) ? ToMSTR(m_dParamData[index].second.getName(), 0) : _M("ERROR: OOR"); }

#pragma region Dynamic pblock handling methods

private:

	// The methods for defining the a new Parameter Block Desc

	/*! Given a descriptor, create a parameter block for it, and assign it to this class.
		The descriptor should not be modified once this function has been called.  This
		function will iterate the parameters in each block, and preserve the values and animations
		of any parameters that are equivalent between the 2 descriptors
		\param pDesc A complete description of the new Parameter block to construct
		\return A pointer to the newly created parameter block */
	void CreateParamBlock(ParamBlockDesc2* pDesc);
	/*! Returns a new instance of a parameter block descriptor, copying the descriptor
		for the current parameter block if possible.  If we currently have no parameter block
		simply return an empty new descriptor.
		The caller is responsible for managing the new descriptor
		\returns a copy of the current descriptor if possible, or a blank descriptor if not. */
	ParamBlockDesc2* CopyPBDescriptor();
	/*! Delete a parameter from the current parameter block 
		This does not modify the splice graph.
		\ParamID the ID of the parameter to delete
		\return true on success, else false */
	bool DeleteMaxParameter(ParamID pid);

	// Methods for defining the UI for our current parameter block

	/*! Generate if necessary and post a UI for our parameter block
		\param ip - An instance of the UI interface class passed to BeginEditParams 
		\return true if m_dialog is ready to use. */
	bool GeneratePBlockUI();

#pragma endregion

public:

#pragma region MaxScript functions

	BaseInterface* GetInterface(Interface_ID id)
	{
		if (id == ISPLICE__INTERFACE)
			return static_cast<SpliceTranslationFPInterface*>(this);
		return TBaseClass::GetInterface(id);
	}

	/*! Every child of the FPMixinInterface class must return
		a pointer to its descriptor. */
	FPInterfaceDesc* GetDesc() { return GetDescriptor<TBaseClass, TResultType>(); }

	FPInterfaceDesc* GetDescByID(Interface_ID id)
	{
		if (id == ISPLICE__INTERFACE)
			return GetDescriptor<TBaseClass, TResultType>();
		return SpliceTranslationFPInterface::GetDescByID(id);
	}

	ReferenceTarget* CastToRefTarg() { return this; }

	DFGWrapper::Binding& GetBinding()	{ return m_binding; }
	FabricCore::Client& GetClient()		{ return m_client; }
	DFGWrapper::Host* GetHost()			{ return m_host; }

	std::string GetKLCode();
	std::string GetKLOperatorName();
	std::string SetKLCode(const std::string& name, const std::string& script);

	// Port creation/management

	// Get the number of ports on this graph
	int GetPortCount() { return m_binding.getExecutable()->getPorts().size(); }
	DFGWrapper::PortPtr GetPort(int i) { return (i < GetPortCount() && i >= 0) ? m_binding.getExecutable()->getPort(i) : DFGWrapper::PortPtr(); }
	DFGWrapper::PortPtr GetPort(const char* name) { return m_binding.getExecutable()->getPort(name); }

	// Splice port management
	// Create a new port.  A matching Max parameter 
	// will be added as well of maxType, if MaxType is -1 
	int AddInputPort(const char* klType, const char* name, int maxType=-1, bool isArray=false, const char* inExtension=false);
	int AddOutputPort(const char* klType, const char* name, bool isArray=false, const char* inExtension=false);
	int AddIOPort(const char* klType, const char* name, int maxType=-1, bool isArray=false, const char* inExtension=false);

	// Remove specified port and matching max parameter
	bool RemovePort(const char* name);

	// Get name of port
	const char* GetPortName(int i);
	bool SetPortName(const char* oldName, const char* newName);
	const char* GetPortType(const char* i);
	bool IsPortArray(const char* i);

	// Get the name of the port we our value from
	const char* GetOutPortName() { return !m_valuePort.isNull() && m_valuePort->isValid() ? m_valuePort->getName() : "INVALIDPORT"; }
	bool SetOutPort(const char* name);

	// Connect myPortName to the output port on pSrcContainer named srcPortName
	bool ConnectPort(const char* myPortName, ReferenceTarget* pSrcContainer, const char* srcPortName, int srcPortIndex, bool postConnectionsUI );
	// Disconnect a previously connected port.
	bool DisconnectPort(const char* myPortName);

	//virtual MSTR GetPortMaxValue(int i);
	//virtual MSTR GetPortMinValue(int i);

	// Get the type of the max parameter connected to the splice in port (i)
	// \param i The index of the splice port
	virtual int GetMaxConnectedType(const char* portName);
	// Set the type of the Max parameter pushing data to the splice port
	// \param i The index of the splice port
	// \type The ParamType to set the matching Max parameter to.
	int SetMaxConnectedType(DFGWrapper::PortPtr& aPort, int maxType);
	virtual int SetMaxConnectedType(const char* portName, int type);
	// Returns an array of the max types that can be used to drive
	// data for splice port
	// \param i The index of the splice port
	// \return A BitArray, where each set bit indicates a legal ParamType for the given port
	virtual BitArray GetLegalMaxTypes(const char* portName) { return SpliceTypeToMaxTypes(GetPortType(portName)); }

	// Allow setting various options on ports 
	// Set UI limits.  This will not actually limit the value, but for sliders etc it will limit what is presented to the user.
	bool SetPortOption(const char* name, const char* option, FPValue* value);
	bool SetPortValue(const char* name, FPValue* value);
	bool SetPortUIMinMax(const char* port, FPValue* uiMin, FPValue* uiMax);

	// Add DFG Entities
	bool AddNewEmptyGraph(const char* name);
	bool AddNewEmptyFunc(const char* name);
	bool AddNodeFromPreset(const char* name, const char* path);

	// Set splice values
//	const DFGWrapper::Binding& GetSpliceGraph() { return getGraph(); }
//	void SetSpliceGraph(const DFGWrapper::Binding& graph, bool createMaxParams);
//	void SetOutPort(const DFGWrapper::PortPtr& port) { m_valuePort = port; };

	// Load from a saved JSON file spec
	bool LoadFromFile(const MCHAR* filename, bool createMaxParams);
	bool SaveToFile(const MCHAR* filename);

	bool RestoreFromJSON(const char* json);
	void ExportToJSON(std::string &outJson);

	
	virtual void ResetPorts();

#pragma endregion

#pragma region DFG-derived functions

	  // notifications
  // for now we only implement onPortInserted and onPortRemoved
  virtual void onNotification(char const * json) {}
  virtual void onNodeInserted(FabricServices::DFGWrapper::NodePtr node) {}
  virtual void onNodeRemoved(FabricServices::DFGWrapper::NodePtr node) {}
  virtual void onPinInserted(FabricServices::DFGWrapper::PinPtr pin) {}
  virtual void onPinRemoved(FabricServices::DFGWrapper::PinPtr pin) {}
  virtual void onPortInserted(FabricServices::DFGWrapper::PortPtr port);
  virtual void onPortRemoved(FabricServices::DFGWrapper::PortPtr port);
  virtual void onEndPointsConnected(FabricServices::DFGWrapper::EndPointPtr src, FabricServices::DFGWrapper::EndPointPtr dst);
  virtual void onEndPointsDisconnected(FabricServices::DFGWrapper::EndPointPtr src, FabricServices::DFGWrapper::EndPointPtr dst) {}
  virtual void onNodeMetadataChanged(FabricServices::DFGWrapper::NodePtr node, const char * key, const char * metadata) {}
  virtual void onNodeTitleChanged(FabricServices::DFGWrapper::NodePtr node, const char * title) {}
  virtual void onPortRenamed(FabricServices::DFGWrapper::PortPtr port, const char * oldName);
  virtual void onPinRenamed(FabricServices::DFGWrapper::PinPtr pin, const char * oldName) {}
  virtual void onExecMetadataChanged(FabricServices::DFGWrapper::ExecutablePtr exec, const char * key, const char * metadata) {}
  virtual void onExtDepAdded(const char * extension, const char * version) {}
  virtual void onExtDepRemoved(const char * extension, const char * version) {}
  virtual void onNodeCacheRuleChanged(const char * path, const char * rule) {}
  virtual void onExecCacheRuleChanged(const char * path, const char * rule) {}
  virtual void onPortResolvedTypeChanged(FabricServices::DFGWrapper::PortPtr port, const char * resolvedType);
  virtual void onPortTypeSpecChanged(FabricServices::DFGWrapper::PortPtr port, const char * typeSpec) {}
  virtual void onPinResolvedTypeChanged(FabricServices::DFGWrapper::PinPtr pin, const char * resolvedType) {}
  virtual void onPortMetadataChanged(FabricServices::DFGWrapper::PortPtr port, const char * key, const char * metadata) {}
  virtual void onPinMetadataChanged(FabricServices::DFGWrapper::PinPtr pin, const char * key, const char * metadata) {}

#pragma endregion
  //////////////////////////////////////////////////////////////////////////

	/// Invalidate our cached values, will cause a re-evalaute of the graph next evaluation
	void Invalidate() { m_valid.SetEmpty(); }
	// Push our parameters to the Splice system, and get the results back...
	const TResultType& Evaluate(TimeValue t, Interval& ivValid);
	// Do an evaluation, but do not return the calculated value
	void TriggerEvaluate(TimeValue t, Interval& ivValid);

	/*! Every client should call this function to create the hook
		up our maxscript exposure */
	static void InitMixinInterface();

	/*! Every client of this class _must_ provide a Dynamic PB ClassDescriptor
		The client should create a static ClassDesc, and ensure that it is returned
		in DllMain - GetClassDesc function as well. */
	static DynPBCustAttrClassDesc* GetClassDesc();

	/*! A client may implement this class to do any cloning necessary
		when cloning the client class.  The argument passed
		is a fully constructed instance, missing only instance-specific
		data not available to the SpliceTranslationLayer class. */
	virtual bool CloneSpliceData(SpliceTranslationLayer* pMyClone) = 0;
};
