//////////////////////////////////////////////////////////////////////////
// FabricTranslationLayer
//
// Description:
//	This file describes the translation layer between
//	Max native datatypes and inputs and the Fabric System.
//	It is responsible for managing the input/output of values
//	from the Fabric system.  This class is purely usage agnostic,
//	and can be applied to any of Max's native interface types.
//	However, any usage-specific interface functions cannot 
//	be implemented here.

#pragma once

//////////////////////////////////////////////////////////////////////////
// includes and forward declarations

#include <vector>
#include "FabricTranslationFPInterface.h"
#include "MaxConversionFns.h"

namespace DynamicDialog
{
	class CDynamicDialogTemplate;
};
class IParamBlock2;
class DynPBCustAttrClassDesc;


// Stores a connection between Max data (ParamID) and Fabric data (DGPort)
//typedef std::pair<ParamID, DFGWrapper::ExecPortPtr> ConnData;
#define MAX_PID_OPT "MaxPID"
#define MAX_SRC_OPT "SrcPort"
#define MAX_SRC_IDX_OPT "SrcIdx"
#define MAX_POST_UI_OPT "SrcPostUI"
#define MAX_PARM_TYPE_OPT "MaxType"
#define FABRIC_UI_HIDDEN "uiHidden"
//////////////////////////////////////////////////////////////////////////
#pragma region Utility functions
/** Add a new parameter definition to the given pblock descriptor
	This function will search out the next available ID, and set this
	to the appropriate parameter type.
	\pDesc - The descriptor to add the parameter too.  Its important that
	this has not been used to create a parameter block yet.
	\type - The type of parameter to create
	\return The ID of the newly created parameter definition */
ParamID AddMaxParameter(ParamBlockDesc2* pDesc, int type, const MCHAR* sName, ParamID desiredId = -1);
ParamID AddMaxParameter(ParamBlockDesc2* pDesc, int type, const char* cName);
void SetMaxParamName(ParamBlockDesc2* pDesc, ParamID pid, const MCHAR* name);
void SetMaxParamFromFabric(FabricTranslationFPInterface* pOwner, ParamID pid, const char* argName);

/*! Generate a Win32 dialog for the passed pblok
	\param pblock - The list of parameters to generate UI for
	\return the new dialog if successful, else NULL. */
DynamicDialog::CDynamicDialogTemplate* GeneratePBlockUI(IParamBlock2* pblock);

/** Create a new parameter block, copying the
	values off similarly named parameters on pCopyThis where possible */
IParamBlock2* CreateParamBlock(ParamBlockDesc2* pDesc, IParamBlock2* pCopyThis, ReferenceTarget* pOwner);

/** Get all Max types that can be converted to the given Fabric type */
BitArray FabricTypeToMaxTypes(const char* spliceType);

/** Returns the Max ParamType2 that matches the named Fabric type
	if the type is not legal for PB2, returns -1 */
int FabricTypeToMaxType(const char* cType);

/** Returns the default Max ParamType2 that matches the named Fabric type */
int FabricTypeToDefaultMaxType(const char* cType);

/** Add a new value port to the given DGGraph
	This adds in the appropriate Fabric type to recieve
	the Max parameter type, and connects the port.  The return
	value should be cached as the port top set/get values
	from the splice parameter
	\rGraph - a reference to the Fabric sub-graph to add the parameter too
	\type - The Max type that will be set
	\pName - The name of the parameter
	\mode - whether this is a read/write variable */
std::string AddFabricParameter(FabricTranslationFPInterface* pOwner, const char* type, const char* cName, FabricCore::DFGPortType mode, const char* inExtension = "", const char* metadata = "");
std::string AddFabricParameter(FabricTranslationFPInterface* pOwner, int type, const MCHAR* pName, FabricCore::DFGPortType mode, const char* inExtension = "", const char* metadata = "");
std::string AddFabricParameter(FabricTranslationFPInterface* pOwner, int type, const char* pName, FabricCore::DFGPortType mode, const char* inExtension = "", const char* metadata = "");

/** Get various useful info's from fabric Ports 
	These functions are defined here and don't depend
	on the Translation class to enable them to be used
	from the UI layer */
int GetPortParamID(const FabricCore::DFGExec& exec, const char* argName);
int GetPort3dsMaxType(const FabricCore::DFGExec& exec, const char* argName);
const char* GetPortType(const FabricCore::DFGExec& exec, const char* argName);
bool IsPortArray(const FabricCore::DFGExec& exec, const char* argName);

std::string GetPortConnection(FabricTranslationFPInterface* pOwner, const char* argName);
void SetPortConnection(FabricTranslationFPInterface* pOwner, const char* argName, const char* name);
bool GetPortPostConnectionUI(FabricTranslationFPInterface* pOwner, const char* argName);
void SetPortPostConnectionUI(FabricTranslationFPInterface* pOwner, const char* argName, bool postUI);

/** Given a Max value, send it to the dgPort in the appropriate fashion */

/** For each valid in the parameter block, send it to the appropriate Fabric port in paramData */
void TransferAllMaxValuesToFabric(TimeValue t, IParamBlock2* pblock, FabricCore::DFGBinding& rBinding, std::vector<Interval>& paramValidities, Interval& ivValid);

// DlgCallback for our static UI
INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define MACROREC_GUARD MacroRecorder::MacroRecorderDisable _guard

#pragma endregion

//////////////////////////////////////////////////////////////////////////
template<typename TBaseClass, typename TResultType>
class FabricTranslationLayer : public TBaseClass, public FabricTranslationFPInterface
{
public:
	// Use this typedef within a derivation to refer to the parent class (eg CloneFabricData)
	typedef FabricTranslationLayer<TBaseClass, TResultType> ParentClass;

protected:
#pragma region Max Parameter Management data
	// This is the input datablock for the Fabric System.
	// Input parameters are constructed and placed in this
	// block.  The parameters are evaluated and passed to the
	// splice graph for evaluation.
	IParamBlock2* m_pblock;

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

#pragma region Fabric Connections

	// Store the name of the port/arg who
	// returns the value this class retursn to max
	std::string	m_outArgName;

	// We store a vector recording the Max validity
	// of each parameter.  We only reset data on Fabric
	// if the parameter has actually changed.
	std::vector<Interval> m_portValidities;

	//! The eval context is responsible for sending host info to the graph (eg time, filepath etc)
	FabricCore::RTVal m_evalContext;

	//! this switch prevents infinite recursion when synching Ports/Params
	bool _m_isSyncing;
	// INTERNAL: RAII class to ensure exceptions don't kill our evaluation
	class DoSyncing {
		FabricTranslationLayer<TBaseClass, TResultType>& m_owner;
		DoSyncing& operator=(const DoSyncing &tmp); // Suppress couldnt generate assignment error
	public:
		DoSyncing(FabricTranslationLayer<TBaseClass, TResultType>& owner) : m_owner(owner)
		{ m_owner._m_isSyncing = true;  }
		~DoSyncing()
		{ m_owner._m_isSyncing = false; }
	};
	friend DoSyncing;

#pragma endregion

public:

	FabricTranslationLayer(BOOL loading);
	~FabricTranslationLayer();

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
	int							NumRefs()							{ return 1; }
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
	void						RefAdded(RefMakerHandle 	rm);

	// Sub-anims are the entities that appear in the TrackView
	virtual	int					NumSubs()							{ return 1; }
	virtual	Animatable*			SubAnim(int i)					{ return (i == 0) ? m_pblock : NULL; }
	virtual MSTR				SubAnimName(int UNUSED(i))		{ return _T("pblock"); }

	// Save and Load.
	IOResult					Save(ISave *isave) override;
	IOResult					Load(ILoad *iload) override;
	// Allow implementation classes to save/load extra info
	virtual IOResult			SaveImpData(ISave* isave) { return IO_OK; };
	virtual IOResult			LoadImpData(ILoad* isave) { return IO_OK; };
	// Set names for max parameters from loaded splice data
	void						ReconnectPostLoad();

	ReferenceTarget*			Clone(RemapDir &remap);
	// Delete this virtual function ensures memory is de-allocated 
	// in the same dll it is allocated from
	void						DeleteThis()						{ delete this; }
#pragma endregion

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

	/*! Update the posted UI spec (not values) */
	void UpdateUISpec();

#pragma endregion

public:

#pragma region MaxScript functions

	BaseInterface* GetInterface(Interface_ID id)
	{
		if (id == ISPLICE__INTERFACE)
			return static_cast<FabricTranslationFPInterface*>(this);
		return TBaseClass::GetInterface(id);
	}

	/*! Every child of the FPMixinInterface class must return
		a pointer to its descriptor. */
	FPInterfaceDesc* GetDesc() { return GetDescriptor<TBaseClass, TResultType>(); }

	FPInterfaceDesc* GetDescByID(Interface_ID id)
	{
		if (id == ISPLICE__INTERFACE)
			return GetDescriptor<TBaseClass, TResultType>();
		return FabricTranslationFPInterface::GetDescByID(id);
	}

	ReferenceTarget* CastToRefTarg() { return this; }
	//FabricCore::DFGBinding& GetBinding()	{ return m_binding; }

	//FabricUI::DFG::DFGUICmdHandler* GetCmdHandler() { return &m_maxCmdHandler; }

#pragma endregion

#pragma region fns from FabricCommandInterface

	// Connect myPortName to the output port on pSrcContainer named srcPortName
	// Returns true if successfully connected, false if for any reason the
	// port was not connected.  Once connected, each evaluation the output
	// from srcPortName will be transferred into the in-port myPortName
	bool ConnectArgs(const MSTR& myPortName, ReferenceTarget* pSrcContainer, const MSTR& srcPortName, bool postConnectionsUI) override;
	// Disconnect a previously connected port.  Returns true if the port was previously connected and
	// has been successfully disconnected, false if disconnect failed or if no connection existed.
	bool DisconnectArgs(const MSTR& myPortName) override;

	// Name of the port we pull the value we return to Max from
	MSTR GetOutPortName() override;
	bool SetOutPortName(const MSTR& name)  override;

#pragma endregion

	virtual void ResetPorts();

	void SetPblockParamName(int paramID, MSTR str) override { SetMaxParamName(m_pblock->GetDesc(), ParamID(paramID), str); UpdateUISpec();  }

	IParamBlock2* GetPBlock() { return m_pblock;  }

	int SyncMetaDataFromPortToParam(const char* portName);
	void SyncMaxParamName(const char* dfgPort, int paramId);
	void SyncMaxParamLimits(const char* dfgPort, int paramId);
	void SyncMaxParamDefault(const char* dfgPort, int paramId);

#pragma endregion

	//////////////////////////////////////////////////////////////////////////

	/// Check if our DFG Graph is valid (whether or not we can execute)
	bool GraphCanEvaluate();
	/// Invalidate our cached values, will cause a re-evalaute of the graph next evaluation
	void Invalidate() { m_valid.SetEmpty(); }
	// Send DCC info to graph (time, name, etc)
	void SetupEvalContext(TimeValue t);
	// Push our parameters to the Fabric system, and get the results back...
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
		data not available to the FabricTranslationLayer class. */
	virtual bool CloneFabricData(FabricTranslationLayer* pMyClone) = 0;

	/*! Invalidate all caches, notify Max, and request a redraw */
	void InvalidateAll()
	{
		Invalidate();
		NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
		Interface* pCore = GetCOREInterface();
		pCore->RedrawViews(pCore->GetTime());
	}

	// Count the number 
	static int s_nInstances;

};


extern FabricCore::Client& GetClient(bool doCreate= true, const char* contextId=nullptr);
extern FabricCore::DFGHost& GetHost();
extern FabricCore::RTVal& GetDrawing();

extern void InstanceCreated();
extern void InstanceDeleted();
extern bool AnyInstances();
extern void ReleaseAll();
