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
//typedef std::pair<ParamID, FabricSplice::DGPort> ConnData;
#define MAX_PID_OPT "MaxPID"
#define MAX_SRC_OPT "SrcPort"
#define MAX_SRC_IDX_OPT "SrcIdx"
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

/*! Generate a Win32 dialog for the passed pblok
	\param pblock - The list of parameters to generate UI for
	\return the new dialog if successful, else NULL. */
DynamicDialog::CDynamicDialogTemplate* GeneratePBlockUI(IParamBlock2* pblock);

/** Create a new parameter block, copying the 
	values off similarly named parameters on pCopyThis where possible */
IParamBlock2* CreateParamBlock(ParamBlockDesc2* pDesc, IParamBlock2* pCopyThis, ReferenceTarget* pOwner);

/** Get all Max types that can be converted to the given Splice type */
BitArray GetLegalMaxTypes(const char* spliceType);

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
const FabricSplice::DGPort AddSpliceParameter(FabricSplice::DGGraph& rGraph, const char* type, const char* cName, FabricSplice::Port_Mode mode, bool isArray = false, const char* inExtension="");
const FabricSplice::DGPort AddSpliceParameter(FabricSplice::DGGraph& rGraph, int type, const MCHAR* pName, FabricSplice::Port_Mode mode);
const FabricSplice::DGPort AddSpliceParameter(FabricSplice::DGGraph& rGraph, int type, const char* pName, FabricSplice::Port_Mode mode, bool isArray = false, const char* inExtension="");

/** Get various useful info's from fabric Ports */
int GetPortParamID(FabricSplice::DGPort& aPort);
void SetPortParamID(FabricSplice::DGPort& aPort, ParamID id);
std::string GetPortConnection(FabricSplice::DGPort& aPort);
void SetPortConnection(FabricSplice::DGPort& aPort, const char* name);
int GetPortConnectionIndex(FabricSplice::DGPort& aPort);
void SetPortConnectionIndex(FabricSplice::DGPort& aPort, int index);
const char* GetPortName(FabricSplice::DGPort& aPort);
const char* GetPortType(FabricSplice::DGPort& aPort);

/** Given a Max value, send it to the dgPort in the appropriate fashion */

/** For each valid in the parameter block, send it to the appropriate Splice port in paramData */
void SetAllMaxValuesToSplice(TimeValue t, IParamBlock2* pblock, FabricSplice::DGGraph& rGraph, Interval& ivValid);

// DlgCallback for our static UI
INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#pragma endregion

//////////////////////////////////////////////////////////////////////////
template<typename TBaseClass, typename TResultType>
class SpliceTranslationLayer : public TBaseClass, public SpliceTranslationFPInterface
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

	// For now, till I figure out exactly how we go in->out of KL, restrict to 1 operator
	std::string m_operator;

	// The evaluation graph is the brains of the KL system. (I think)
	FabricSplice::DGGraph m_graph;
	// Every splice class generates a result
	FabricSplice::DGPort m_valuePort;
	// If our valuePort is an array port, this value specifies
	// which index in the array is our particular value
	// If we are not an array, this value will be -1.
	int m_valuePortIndex;

#pragma endregion

public:

	SpliceTranslationLayer(bool init = true);
	~SpliceTranslationLayer();

	// Call this function to initialize splice graph
	// Returns true if work was performed (initialization was
	// necessary) or false if no work was performed
	// (was already initialized).
	bool Init();

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
	RefResult					NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message);
	// We use the RefDeleted callback to detect when we are 
	// removed from the scene graph so we can take down the KLEditor
	void						RefDeleted();

	// Sub-anims are the entities that appear in the TrackView
	virtual	int					NumSubs()							{ return 1; }
	virtual	Animatable*			SubAnim( int i )					{ return (i == 0) ? m_pblock : NULL; }
	virtual MSTR				SubAnimName( int UNUSED(i) )		{ return _T("pblock"); }

	// Save and Load.
	IOResult					Save(ISave *isave);
	IOResult					Load(ILoad *iload);

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

	int GetOperatorCount();
	std::string GetOperatorName(int i);

	std::string GetKLCode();
	std::string GetKLOperatorName();
	std::string SetKLCode(const std::string& name, const std::string& script);

	// Port creation/management

	// Get the number of ports on this graph
	int GetPortCount() { return m_graph.getDGPortCount(); }
	FabricSplice::DGPort GetPort(int i) { return (i < GetPortCount() && i > 0) ? m_graph.getDGPort(i) : FabricSplice::DGPort(); }
	FabricSplice::DGPort GetPort(const char* name) { return m_graph.getDGPort(name); }

	// Splice port management
	// Create a new port.  A matching Max parameter 
	// will be added as well of maxType, if MaxType is -1 
	int AddInputPort(const char* klType, const char* name, int maxType=-1, bool isArray=false, const char* inExtension=false);
	int AddOutputPort(const char* klType, const char* name, bool isArray=false, const char* inExtension=false);
	int AddIOPort(const char* klType, const char* name, int maxType=-1, bool isArray=false, const char* inExtension=false);

	// Remove specified port and matching max parameter
	bool RemovePort(int i);

	// Get name of port
	const char* GetPortName(int i);
	bool SetPortName(int i, const char* name);
	const char* GetPortType(int i);
	bool IsPortArray(int i) { return (i < GetPortCount()) ? m_graph.getDGPort(i).isArray() : NULL; }

	std::string GetAllPortSignature() { return (m_graph) ? m_graph.generateKLOperatorParameterList().getString_cstr() : NULL; }

	// Get the name of the port we our value from
	const char* GetOutPortName() { return m_valuePort.getName(); }
	bool SetOutPortName(const char* name);

	int GetOutPortArrayIdx();
	void SetOutPortArrayIdx(int index);

	// Return the ID of port in our paramblock, or -1 if not connected
	int GetPortParamID(int index);
	void SetPortParamID(int index, ParamID id);

	// Connect myPortName to the output port on pSrcContainer named srcPortName
	bool ConnectPort(const char* myPortName, ReferenceTarget* pSrcContainer, const char* srcPortName, int srcPortIndex);
	// Disconnect a previously connected port.
	bool DisconnectPort(const char* myPortName);

	//virtual MSTR GetPortMaxValue(int i);
	//virtual MSTR GetPortMinValue(int i);

	// Get the type of the max parameter connected to the splice in port (i)
	// \param i The index of the splice port
	virtual int GetMaxConnectedType(int i);
	// Set the type of the Max parameter pushing data to the splice port
	// \param i The index of the splice port
	// \type The ParamType to set the matching Max parameter to.
	int SetMaxConnectedType(FabricSplice::DGPort& aPort, int maxType);
	virtual int SetMaxConnectedType(int i, int type);
	// Returns an array of the max types that can be used to drive
	// data for splice port
	// \param i The index of the splice port
	// \return A BitArray, where each set bit indicates a legal ParamType for the given port
	virtual BitArray GetLegalMaxTypes(int i) { return ::GetLegalMaxTypes(GetPortType(i)); }

	// Set splice values
	const FabricSplice::DGGraph& GetSpliceGraph() { return m_graph; }
	void SetSpliceGraph(const FabricSplice::DGGraph& graph, bool createMaxParams);
	void SetOutPort(const FabricSplice::DGPort& port) { m_valuePort = port; };

	// Load from a saved JSON file spec
	bool LoadFromFile(const MCHAR* filename, bool createMaxParams);
	bool SaveToFile(const MCHAR* filename);
	
	virtual void ResetPorts();

#pragma endregion

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
