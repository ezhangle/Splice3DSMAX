#pragma once // hpp files are actually headers

#include "FabricTranslationLayer.h"
#include "DynamicParamBlocks/DynPBCustAttrClassDesc.h"
#include "DynamicParamBlocks/DynamicDialog/DynamicDialogTemplate.h"
#include "DynamicParamBlocks/DynPBUndo.h"
#include "FabricLogging.h"
#include "MaxConversionFns.h"
#include "Fabric3dsmax.h"
#include "FabricRestoreObjects.h"
#include "ASTWrapper/KLASTManager.h"

//////////////////////////////////////////////////////////////////////////
//--- FabricTranslationLayer -------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------
// Constructor/Destructor
//---------------------------------------------------------------------------------------------------------------
#pragma region//Constructor/Destructor
template<typename TBaseClass, typename TResultType>
FabricTranslationLayer<TBaseClass, TResultType>::FabricTranslationLayer(BOOL loading)
	:	m_hPanel(NULL)
	,	m_pInterface(NULL)
	,	m_pMtlInterface(NULL)
	,	m_dialogTemplate(NULL)
	,	m_paramMap(NULL)
	,	m_pblock(NULL)
	,	m_valid(NEVER)
	,	_m_isSyncing(false)
{
	InstanceCreated();

	if (!loading)
		Init(loading);
}

template<typename TBaseClass, typename TResultType>
FabricTranslationLayer<TBaseClass, TResultType>::~FabricTranslationLayer()
{
	// We _must_ have been released by now
	DbgAssert(m_pblock == NULL);

	// Now is a good time to release any old pbdescs
	// If we are in a reset, our pblock would have 
	// been released by now, and the descriptor would
	// be registered with our CD for release.
	DynPBCustAttrClassDesc* ourCD = GetClassDesc();
	ourCD->ReleaseObsoletePBDesc();

	InstanceDeleted();
}

template<typename TBaseClass, typename TResultType>
bool FabricTranslationLayer<TBaseClass, TResultType>::Init(BOOL loading)
{
	MAXSPLICE_CATCH_BEGIN;

	//if (!loading)
	{
		// create an empty binding
		FabricCore::DFGBinding binding = GetHost().createBindingToNewGraph();
		SetBinding(binding);

		// Enforce loading the Util extension to ensure that EvalContext is available to us
		GetClient().loadExtension("Util", "", false);

		// given a scene base object or modifier, look for a referencing node via successive 
		// reference enumerations up through the ref hierarchy untill we find an inode.
		MSTR name;
		INode* node = GetCOREInterface7()->FindNodeFromBaseObject(m_pblock);
		if (node)
			name = node->GetName();
		else
			name = _M("3dsMaxDFGGraph");
		CStr cName = name.ToCStr();
		m_binding.getExec().setTitle(cName.data());
	}

	MAXSPLICE_CATCH_RETURN(false);

	return true;
}
#pragma endregion

//---------------------------------------------------------------------------------------------------------------
// Dynamic Parameter block descriptor creation methods
//---------------------------------------------------------------------------------------------------------------
#pragma region//pblock creation methods

// Copy the current pblocks descriptor, or return a new one
template<typename TBaseClass, typename TResultType>
ParamBlockDesc2* FabricTranslationLayer<TBaseClass, TResultType>::CopyPBDescriptor()
{
	// Create the default new descriptor
	ParamBlockDesc2* pNewDesc = GetClassDesc()->CreatePBDesc();

	// if we already have a pblock, clone the descriptor
	if (m_pblock != NULL)
	{
		// Get current descriptor.
		ParamBlockDesc2* pbDesc = m_pblock->GetDesc();

		// We cannot assign memory on a class that Max implements.
		// Because of this, we must run through the parameters and
		// basically create them again.  Once the memory is
		// allocated, we can assign our own values to the ParamDef's

		for (int i = 0; i < pbDesc->Count(); i++)
		{
			// Find the param Id for this index.
			ParamID curPId = pbDesc->IndextoID(i);
			// This should not be possible, indices are tightly packed
			DbgAssert(curPId >= 0);
			ParamDef& pbDef = pbDesc->GetParamDef(curPId);

			// Calling AddMaxParameter to create a new ParamDef
			// to copy our existing paramdef.
			ParamID newPid = AddMaxParameter(pNewDesc, pbDef.type, pbDef.int_name, curPId);

			// Ensure we copy available options
			CStr argName = CStr::FromMCHAR(pbDef.int_name);
			SyncMaxParamLimits(argName, newPid);
			SyncMaxParamDefault(argName, newPid);
		}
	}
	// Return the new descriptor. This object is now the
	// responsibility of the caller
	return pNewDesc;
}

template<typename TBaseClass, typename TResultType>
bool FabricTranslationLayer<TBaseClass, TResultType>::DeleteMaxParameter(ParamID pid)
{
	// No block, nothing to delete
	if (m_pblock == nullptr)
		return true;

	// Test this id is valid
	if (m_pblock->IDtoIndex(pid) == -1)
		return false;

	// Create a new descriptor
	ParamBlockDesc2* pNewDesc = CopyPBDescriptor();

	// TODO: Undo here.
	// Manually free the memory associated with int_name, to prevent
	// memory allocated in this Dll being released in another dll
	SetMaxParamName(pNewDesc, pid, NULL);

	// Delete parameter from new descriptor
	pNewDesc->DeleteParam(pid);

	// Replace old pblock with new one (will copy values).
	CreateParamBlock(pNewDesc);

	// Invalidate everything 
	InvalidateAll();
	return true;
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::CreateParamBlock( ParamBlockDesc2* pDesc)
{
	IParamBlock2* pNewBlock = ::CreateParamBlock(pDesc, m_pblock, this);
	// This line finally assigns us the new parameter block.
	// At this point our old block will most likely be deleted.
	ReplaceReference(0, pNewBlock);
}

#pragma endregion

//---------------------------------------------------------------------------------------------------------------
// Parameter block dynamic dialog creation methods
//---------------------------------------------------------------------------------------------------------------
#pragma region // UI methods


template<typename TBaseClass, typename TResultType>
bool FabricTranslationLayer<TBaseClass, TResultType>::GeneratePBlockUI()
{
	if (m_dialogTemplate == NULL) 
		m_dialogTemplate = ::GeneratePBlockUI(m_pblock);

	return m_dialogTemplate != NULL;
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::UpdateUISpec()
{
	MaybeRemoveParamUI();
	SAFE_DELETE(m_dialogTemplate);
	MaybePostParamUI();
}

//---------------------------------------------------------------------------------------------------------------
// Inherited methods - UI related
//---------------------------------------------------------------------------------------------------------------

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::MaybeRemoveParamUI()
{
	if (m_paramMap != NULL)
	{
		if (m_pInterface != NULL) m_pInterface->DeleteRollupPage(m_paramMap->GetHWnd());
		else if (m_pMtlInterface != NULL) m_pMtlInterface->DeleteRollupPage(m_paramMap->GetHWnd());
		else { DbgAssert(0); } // shouldn't be possible
		m_paramMap = NULL;				// This has been deleted now
	}
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::MaybePostParamUI()
{
	if (m_pInterface != NULL && GeneratePBlockUI())
	{
		m_paramMap = CreateCPParamMap2(m_pblock, m_pInterface, hInstance, (DLGTEMPLATE*)m_dialogTemplate->GetDLGTEMPLATE(), _M("The Dynamic Params"), 0);
	}
	if (m_pMtlInterface != NULL)
	{
		//ParamDlg* pDlg = CreateMyAutoParamDlg(GetCOREInterface()->GetMEdit, m_pMtlInterface);
		//if (pDlg != NULL)
		//{
		//	// TODO: This work?
		//	pDlg->GetThing();
		//}
	}
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::BeginEditParams(IObjParam *ip, ULONG flags, Animatable *UNUSED(prev))
{
	this->m_pInterface = ip;
	m_hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PARAM_GENERATION_UI),
		DlgProc,
		GetString(IDS_PARAMS),
		(LPARAM)static_cast<FabricTranslationFPInterface*>(this));

	// Generate our Dialog, and assign an mParamMap
	MaybePostParamUI();

	// For each port, see if it wants to post a UI too
	if (m_pblock != nullptr) 
	{
		FabricCore::DFGExec exec = GetExec(nullptr);
		int portCount = exec.getExecPortCount();
		for (int i = 0; i < portCount; i++) 
		{
			const char* portName = exec.getExecPortName(i);
			if (GetPortPostConnectionUI(this, portName))
			{
				// UI Requested: what is the max connection for this parameter?
				int pid = ::GetPortParamID(exec, portName);
				if (pid != -1)
				{
					// Double check - we can only post UI for class types
					if (reftarg_type(m_pblock->GetParameterType(ParamID(pid)))) 
					{
						ReferenceTarget* pSrcContainer = m_pblock->GetReferenceTarget(ParamID(pid));
						if (pSrcContainer == nullptr)
							continue;

						//FabricTranslationFPInterface* pSrcContInterface = static_cast<FabricTranslationFPInterface*>(pSrcContainer->GetInterface(ISPLICE__INTERFACE));
						//if (pSrcContInterface == nullptr)
						//	continue;

						// Everything checks out... go ahead and post a UI
						pSrcContainer->BeginEditParams(ip, flags, nullptr);
					}
				}
			}
		}
	}
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::EndEditParams(IObjParam *ip, ULONG flags, Animatable *UNUSED(next))
{
	if (m_pblock != nullptr) 
	{
		// First, remove any connections UI's
		for (int i = 0; i < m_pblock->NumParams(); i++)
		{
			ParamID pid = m_pblock->IndextoID(i);
			if (reftarg_type(m_pblock->GetParameterType(pid))) 
			{
				ReferenceTarget* pSrcContainer = m_pblock->GetReferenceTarget(pid);
				if (pSrcContainer == nullptr)
					continue;
				pSrcContainer->EndEditParams(ip, flags, nullptr);
			}
		}
	}
	// Now delete my own UI
	ip->DeleteRollupPage(m_hPanel);

	MaybeRemoveParamUI();
	this->m_pInterface = NULL;
	m_hPanel = NULL;
}

template<typename TBaseClass, typename TResultType>
ParamDlg *FabricTranslationLayer<TBaseClass, TResultType>::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)
{
	m_pMtlInterface = imp;

	// First, add our static UI's
	m_hPanel = imp->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PARAM_GENERATION_UI),
		DlgProc,
		GetString(IDS_PARAMS).data(),
		(LPARAM)static_cast<FabricTranslationFPInterface*>(this));

	return CreateMyAutoParamDlg(hwMtlEdit, imp);
}

template<typename TBaseClass, typename TResultType>
ParamDlg* FabricTranslationLayer<TBaseClass, TResultType>::CreateMyAutoParamDlg(HWND hwMtlEdit, IMtlParams* pMtlParams) 
{
	if (m_pblock && GeneratePBlockUI())
	{
		IAutoMParamDlg* newDlg = CreateAutoMParamDlg(hwMtlEdit, pMtlParams, this, 
			m_pblock, GetClassDesc(), hInstance, 
			(DLGTEMPLATE*)m_dialogTemplate->GetDLGTEMPLATE(), 
			_M("Dynamic Params"),	// Or use pbd->int_name
			0);						// could be APPENDROLL_CLOSED

		return newDlg;
	}
	return NULL;
}

#pragma endregion

//---------------------------------------------------------------------------------------------------------------
// Inherited methods - Reference management
//---------------------------------------------------------------------------------------------------------------

#pragma region//Get/SetReference
template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::SetReference(int i, RefTargetHandle rtarg)
{
	if (i == 0)
	{
		m_valid = NEVER;
		// Always delete our dialog template
		SAFE_DELETE(m_dialogTemplate);

		if (m_pblock != NULL)
		{
			// If we are here, we _must_ be in between BeginEditParams
			// and EndEditParams.  Remove the current UI.
			MaybeRemoveParamUI();

			// We are re-assigning our parameter block
			// We need to make sure that we don't leave too many
			// descriptors lying around that have no parameter blocks
			ParamBlockDesc2* pbDesc = m_pblock->GetDesc();
			DbgAssert(pbDesc != NULL); // This is frankly impossible
			if (UndoOn())
			{
				// If we are holding, then we have to be prepared for the possibility that the user
				// undos their action.  In this case, we can't just delete our descriptor.  To
				// manage this correctly we use our own Undo object.
				theHold.Put(new DynPBUndo(pbDesc, this));
			}
			else
			{
				// Unfortunately we can't just delete the descriptor. the
				// parameter block will still need it later.  Register this
				// with our CD2 for later clean up
				GetClassDesc()->SetObsoletePBDesc(pbDesc);
			}
		}

		// Assign our new target.
		m_pblock = (IParamBlock2*)rtarg;

		// Do we need to repost our UI?
		MaybePostParamUI();
	}
}

// Something has changed on our parameter block
#if MAX_VERSION_MAJOR < 17 // Max 2015 is ver 17
template<typename TBaseClass, typename TResultType>
RefResult FabricTranslationLayer<TBaseClass, TResultType>::NotifyRefChanged( Interval , RefTargetHandle , PartID&	, RefMessage message )
#else
template<typename TBaseClass, typename TResultType>
RefResult FabricTranslationLayer<TBaseClass, TResultType>::NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate)
#endif
{
	// set splices dirty flag.
	if (message == REFMSG_CHANGE)
	{
		// Invalidate our cached output value
		Invalidate();

		if (m_pblock != nullptr)
		{
			// Invalidate the input value for the param that changed
			ParamID pid = m_pblock->LastNotifyParamID();
			int pidx = m_pblock->IDtoIndex(pid);
			if (pidx < m_portValidities.size())
				m_portValidities[pidx].SetEmpty();
		}
	}
	else if (message == REFMSG_SUBANIM_STRUCTURE_CHANGED)
	{
		// If our structure changes, then erase all validities
		m_portValidities.clear();

		// If our structure is changing, its probably due to the
		// number/type of parameters we have changing.  In this
		// case we want to we want to show the new UI, so repost.
		if (m_pblock != nullptr)
		{
			// However, we also recieve this message for changes to
			// a parameters value (eg, when we add a reference to a node).
			// In these cases, the change could have been triggered by
			// a button in our UI, so we cannot remove the UI
			// or it will delete an object that is currently sitting
			// a few spots up on our stack.
			ParamID pid = m_pblock->LastNotifyParamID();
			if (pid == -1)
			{
				UpdateUISpec();
			}
		}
	}

	return REF_SUCCEED;
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::RefDeleted() {
	// If we have no nodes referencing this class, then kill our UI
	//if (GetKLEditor() != nullptr)
	//{
		ULONG handle = 0;
		NotifyDependents(FOREVER, (PartID)&handle, REFMSG_GET_NODE_HANDLE);
		if (handle == 0)
			CloseDFGGraphEditor();
	//}
}



template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::RefAdded(	RefMakerHandle 	rm) {
	// Note: We were using this to name our class, but it causes undo objects 
	// to be created at the worst time.  Skip for now.

	//if (m_binding.isValid())
	//{
	//	MAXSPLICE_CATCH_BEGIN;

	//	// Set static context values
	//	//MSTR filepath = GetCOREInterface()->GetCurFilePath();
	//	//FabricCore::RTVal evalContext = m_graph.getEvalContext();

	//	// given a scene base object or modifier, look for a referencing node via successive 
	//	// reference enumerations up through the ref hierarchy untill we find an inode.
	//	INode* node = GetCOREInterface7()->FindNodeFromBaseObject(this);
	//	if (node) {
	//		CoreHoldActions hold("DFG Set Title");
	//		m_binding.getExec().setTitle(TSTR(node->GetName()).ToCStr().data());
	//	}
	//	MAXSPLICE_CATCH_END;
	//}
}
#pragma endregion

//---------------------------------------------------------------------------------------------------------------
// Inherited methods - Everything Else
//---------------------------------------------------------------------------------------------------------------
#pragma region//Clone
template<typename TBaseClass, typename TResultType>
ReferenceTarget *FabricTranslationLayer<TBaseClass, TResultType>::Clone(RemapDir &remap)
{
	FabricTranslationLayer *pMyClone = reinterpret_cast<FabricTranslationLayer*>(GetClassDesc()->Create(TRUE));

	// If we are cloning - lets clone the parameter block
	if (m_pblock != NULL)
	{
		// We do make the assumption that each descriptor
		// is unique to a class (and a single pblock).
		// We can clone our current descriptor though...
		ParamBlockDesc2* pDesc = CopyPBDescriptor();
		// ... create a copy of our parameter block on the target
		pMyClone->ReplaceReference(0, ::CreateParamBlock(pDesc, m_pblock, this));
	}
	BaseClone( this, pMyClone, remap );

	// Now, allow our derived class to clone onto the new class
	CloneFabricData(pMyClone);

	return pMyClone;
}
#pragma endregion

//---------------------------------------------------------------------------------------------------------------
// Inherited methods - Serialization related
//---------------------------------------------------------------------------------------------------------------
#pragma region//Save/Load
//////////////////////////////////////////////////////////////////////////
// useful defines

#define	PARAM_DATA_CHUNK		0x01
#define	KL_CODE_CHUNK			0x02

#define PARAM_NAME_CHUNK		0x10
#define PARAM_SPLICE_TYPE_CHUNK 0x11
#define PARAM_MAX_DATA_CHUNK	0x12
#define PARAM_SPLICE_DATA		0x13
#define PARAM_VALUE_NAME		0x14
#define EXTRA_CLASS_DATA		0x15

#define KL_CODE_NAME_CHUNK		0x20
#define KL_CODE_OUTPORT_CHUNK	0x21
#define KL_CODE_SOURCE_CHUNK	0x22

// Save our local parameters
template<typename TBaseClass, typename TResultType>
IOResult FabricTranslationLayer<TBaseClass, TResultType>::Save( ISave *isave )
{	
	// Save out all the data needed to recreate parameters
	isave->BeginChunk(PARAM_SPLICE_DATA);
	FabricCore::DFGStringResult json = m_binding.exportJSON();
	isave->WriteCString(json.getCString());
	isave->EndChunk();

	// Save the value port.
	isave->BeginChunk(PARAM_VALUE_NAME);
	const char* outName = m_outArgName.c_str();
	isave->WriteCString(outName);
	isave->EndChunk();

	// Save additional values for derived values
	isave->BeginChunk(EXTRA_CLASS_DATA);
	SaveImpData(isave);
	isave->EndChunk();

	return IO_OK;
}

// Post load callback on load gives classes a chance to clean up
class RenameMaxParamsCallback : public PostLoadCallback {
private:
	FabricTranslationFPInterface* m_owner;

public:
	RenameMaxParamsCallback(FabricTranslationFPInterface* pOwner) : m_owner(pOwner) {}
	virtual void proc( ILoad *iload ) { m_owner->ReconnectPostLoad(); delete this; }
};

// Load our local parameters
template<typename TBaseClass, typename TResultType>
IOResult FabricTranslationLayer<TBaseClass, TResultType>::Load( ILoad *iload )
{
	IOResult result = IO_OK;
	std::string outName;
	while( IO_OK == (result = iload->OpenChunk()) )
	{
		switch (iload->CurChunkID())
		{
		case PARAM_SPLICE_DATA:
			{
				// First, read the size of the string.
				char *buff = nullptr;
				if (IO_OK == iload->ReadCStringChunk(&buff))
				{
					RestoreFromJSON(buff, false);
				}

				break;
			}
		case PARAM_VALUE_NAME:
			{
				// First, read the size of the string.
				char *buff = nullptr;
				if (IO_OK == iload->ReadCStringChunk(&buff))
					outName = buff;
				
				break;
			}
		case EXTRA_CLASS_DATA:
			{
				// Load implementation-specific data
				LoadImpData(iload);
				break;
			}
		}
		iload->CloseChunk();
	}

	iload->RegisterPostLoadCallback(new RenameMaxParamsCallback(this));
	// Get our output port
	SetOutPortName(MSTR::FromACP(outName.data()));

	return IO_OK;
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::ReconnectPostLoad()
{
	FabricCore::DFGExec exec = m_binding.getExec();
	int nPorts = exec.getExecPortCount();
	for (int i = 0; i < nPorts; i++) {
		const char* argName = exec.getExecPortName(i);
		int pid = GetPortParamID(argName);
		if (pid >= 0)
		{
			MSTR str = MSTR::FromACP(argName);
			SetMaxParamName(m_pblock->GetDesc(), (ParamID)pid, str);
			// Delete existing autogen UI
			SAFE_DELETE(m_dialogTemplate);
		}
	}
}
#pragma endregion

//---------------------------------------------------------------------------------------------------------------
// MaxScript-exposed functions.
//---------------------------------------------------------------------------------------------------------------

#pragma region FabricCommandInterface functions


// Connect myPortName to the output port on pSrcContainer named srcPortName
// Returns true if successfully connected, false if for any reason the
// port was not connected.  Once connected, each evaluation the output
// from srcPortName will be transferred into the in-port myPortName
template<typename TBaseClass, typename TResultType>
bool FabricTranslationLayer<TBaseClass, TResultType>::ConnectArgs(const MSTR& myPortName, ReferenceTarget* pSrcContainer, const MSTR& srcPortName, bool postConnectionsUI)
{
	if (pSrcContainer == nullptr)
		return false;
	
	FabricTranslationFPInterface* pSrcContInterface = static_cast<FabricTranslationFPInterface*>(pSrcContainer->GetInterface(ISPLICE__INTERFACE));
	if (pSrcContInterface == nullptr)
		return false;
	
	MAXSPLICE_CATCH_BEGIN

		CStr cSrcPort = srcPortName.ToCStr();
		CStr cDstPort = myPortName.ToCStr();
		// First, do our ports exist?
		bool srcExists = pSrcContInterface->HasDstPort(cSrcPort);
		bool dstExists = HasSrcPort(cDstPort);
		if (!srcExists || !dstExists)
			return false;

		// Are they of the same type?
		if (strcmp(pSrcContInterface->GetPortType(cSrcPort), GetPortType(cDstPort)) != 0)
			return false;
	
		// Ok - these ports are good to go.  Connect 'em up.
		int res = SetMaxTypeForArg(cDstPort, TYPE_REFTARG);
		if (res < 0)
			return false;
		m_pblock->SetValue((ParamID)res, 0, pSrcContainer);
		SetPortConnection(this, cDstPort, cSrcPort);
		SetPortPostConnectionUI(this, cDstPort, postConnectionsUI);
	
	MAXSPLICE_CATCH_RETURN(false)
	
	return true;
}
// Disconnect a previously connected port.  Returns true if the port was previously connected and
// has been successfully disconnected, false if disconnect failed or if no connection existed.
template<typename TBaseClass, typename TResultType>
bool FabricTranslationLayer<TBaseClass, TResultType>::DisconnectArgs(const MSTR& myPortName)
{
	MAXSPLICE_CATCH_BEGIN
	SetPortConnection(this, myPortName.ToCStr().data(), "");
	SetMaxTypeForArg(myPortName, -2);
	MAXSPLICE_CATCH_RETURN(false)
	return true;
}

//////////////////////////////////////////////////////////////////////////
// Props
template<typename TBaseClass, typename TResultType>
MSTR FabricTranslationLayer<TBaseClass, TResultType>::GetOutPortName()
{
	return MSTR::FromACP(m_outArgName.c_str());
}

template<typename TBaseClass, typename TResultType>
bool FabricTranslationLayer<TBaseClass, TResultType>::SetOutPortName(const MSTR& name)
{
	MAXSPLICE_CATCH_BEGIN

	// can this port be translated to our out-type?
	CStr cname = name.ToCStr();
	const char* portType = GetPortType(cname.data());
	BitArray legalTypes = FabricTypeToMaxTypes(portType);
	if (!legalTypes[GetValueType()])
		return false;
	
	// Everything checks out - this port is now our value
	m_outArgName = cname;
	Invalidate();
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	return true;
	
	MAXSPLICE_CATCH_RETURN(false);
}

#pragma endregion


template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::ResetPorts()
{
	MACROREC_GUARD;

	// Setup any necessary ports for the current graph
	m_outArgName = AddFabricParameter(this, GetValueType(), "outputValue", FabricCore::DFGPortType_Out, "", "{ \"uiPersistValue\": \"false\" }");
}

template<typename TBaseClass, typename TResultType>
int FabricTranslationLayer<TBaseClass, TResultType>::SyncMetaDataFromPortToParam(const char* argName)
{
	// Technically we shouldnt call functions that can trigger
	// calling a sync, but in this case its just easier to call
	// SetPortParamID (which recurses to this function) than
	// writing all that code again.
	if (_m_isSyncing)
		return -1;
	DoSyncing ds(*this);

	// Does this type already exist?
	int paramId = GetPortParamID(argName);
	
	// What type _should_ be set?
	int maxType = GetPort3dsMaxType(argName);

	// If we should not have a type, bail
	if (maxType < 0)
	{
		if (paramId >= 0)
		{
			DeleteMaxParameter((ParamID)paramId);
			paramId = -1;
		}
	}
	else
	{
		if (paramId >= 0)
		{
			// Check that its not the correct type already
			if (m_pblock->GetParamDef((ParamID)paramId).type != maxType)
			{
				// Is the type incompatible?  For now... guess so...
				DeleteMaxParameter((ParamID)paramId);
				paramId = -1;
			}
		}

		// Do we need to create a new type?
		if (paramId < 0)
		{
			// If this is a legal type for this parameter, then go ahead
			ParamBlockDesc2* pNewDesc = CopyPBDescriptor();
			paramId = AddMaxParameter(pNewDesc, maxType, argName);
			CreateParamBlock(pNewDesc);
		}
		else
		{
			// Existing type, but name might have changed:
			SyncMaxParamName(argName, paramId);
		}

		if (m_pblock != nullptr)
		{
			// Sync meta data
			SyncMaxParamLimits(argName, paramId);
			SyncMaxParamDefault(argName, paramId);
		}
	}
	MACROREC_GUARD;
	SetPortParamID(argName, paramId);
	return paramId;
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::SyncMaxParamName(const char* argName, int id)
{
	if (id < 0 || m_pblock == nullptr)
		return;

	// Assume argName is valid, and matches id
	MSTR v = MSTR::FromACP(argName);
	SetMaxParamName(m_pblock->GetDesc(), ParamID(id), v.data());
	UpdateUISpec();
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::SyncMaxParamLimits(const char* argName, int id)
{
	if (id < 0)
		return;
	ParamID pid = ParamID(id);
	ParamBlockDesc2* pDesc = m_pblock->GetDesc();
	ParamDef& def = pDesc->GetParamDef(pid);
	int baseType = base_type(def.type);

	std::string rangeStr = GetPortMetaData(argName, "uiRange");
	if (rangeStr.empty())
	{
		// If nothing set, reset to default.
		switch (baseType)
		{
		case TYPE_FLOAT:
		case TYPE_ANGLE:
		case TYPE_PCNT_FRAC:
		case TYPE_WORLD:
		{
			pDesc->ParamOption(pid, p_range, FLT_MIN, FLT_MAX, p_end);
			break;
		}
		case TYPE_INT:
		{
			pDesc->ParamOption(pid, p_range, INT_MIN, INT_MAX, p_end);
			break;
		}
		}
		return;
	}

	// TODO: Use FTL Json for processing this string
	if (rangeStr.length() < 2)
		return;
	DbgAssert(rangeStr[0] == '(' && rangeStr[rangeStr.length() - 1] == ')');
	size_t splitIdx = rangeStr.find(',');
	std::string firstValStr = rangeStr.substr(1, splitIdx - 1);
	std::string secondValStr = rangeStr.substr(splitIdx + 1, rangeStr.length() - (splitIdx + 2));

	switch ((int)baseType)
	{
	case TYPE_FLOAT:
	case TYPE_ANGLE:
	case TYPE_PCNT_FRAC:
	case TYPE_WORLD:
	{
		float vMin = (float)atof(firstValStr.c_str());
		float vMax = (float)atof(secondValStr.c_str());;
		pDesc->ParamOption(pid, p_range, vMin, vMax, p_end);
		break;
	}
	case TYPE_INT:
	{
		int vMin = atoi(firstValStr.c_str());
		int vMax = atoi(secondValStr.c_str());
		pDesc->ParamOption(pid, p_range, vMin, vMax, p_end);
		break;
	}
	//case TYPE_RGBA:		pDesc->ParamOption(pid, p_ui, TYPE_COLORSWATCH, 0, p_end); break;
	case TYPE_POINT3:
	{
		//Point3 uiMin = port.getOption()
		break;
	}
	//case TYPE_POINT4:	pDesc->ParamOption(pid, p_ui, TYPE_SPINNER, EDITTYPE_UNIVERSE, 0, 0, 0, 0, 0, 0, 0, 0, SPIN_AUTOSCALE, p_end); break;
	//case TYPE_BOOL:		pDesc->ParamOption(pid, p_ui, TYPE_SINGLECHEKBOX, 0, p_end); break;
	//case TYPE_INODE:	pDesc->ParamOption(pid, p_ui, TYPE_PICKNODEBUTTON, 0, p_end); break;
	//case TYPE_MTL:		pDesc->ParamOption(pid, p_ui, TYPE_MTLBUTTON, 0, p_end); break;
	//case TYPE_TEXMAP:	pDesc->ParamOption(pid, p_ui, TYPE_TEXMAPBUTTON, 0, p_end); break;
	//case TYPE_STRING:	pDesc->ParamOption(pid, p_ui, TYPE_EDITBOX, 0, p_end); break;
	}

	UpdateUISpec();
}

template<typename TType>
void SetMaxParamDefault(ParamBlockDesc2* pDesc, ParamID pid, FabricCore::Variant& defaultVal) {
	TType def;
	FabricToMaxValue(defaultVal, def);
	pDesc->ParamOption(pid, p_default, def, p_end);
}

template<typename TType>
void SetMaxParamDefault(ParamBlockDesc2* pDesc, ParamID pid, FabricCore::RTVal& defaultVal) {
	TType def;
	FabricToMaxValue(defaultVal, def);
	pDesc->ParamOption(pid, p_default, def, p_end);
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::SyncMaxParamDefault(const char* argName, int pid)
{
	if (pid < 0)
		return;

	char const *resolvedType = GetPortType(argName);
	if (!resolvedType)
		return;

	FabricCore::RTVal defaultVal = m_binding.getExec().getPortDefaultValue(argName, resolvedType);
	if (!defaultVal.isValid())
		return;

	ParamID paramId = (ParamID)pid;
	ParamBlockDesc2* pDesc = m_pblock->GetDesc();
	ParamDef& def = pDesc->GetParamDef((ParamID)paramId);
	int baseType = base_type(def.type);
	switch ((int)baseType)
	{
	case TYPE_BOOL:
	case TYPE_INT:
	{
		SetMaxParamDefault<int>(pDesc, paramId, defaultVal);
		break;
	}
	case TYPE_FLOAT:
	case TYPE_ANGLE:
	case TYPE_PCNT_FRAC:
	case TYPE_WORLD:
	{
		SetMaxParamDefault<float>(pDesc, paramId, defaultVal);
		break;
	}
	case TYPE_RGBA:
	{
		SetMaxParamDefault<Color>(pDesc, paramId, defaultVal);
		break;
	}
	case TYPE_POINT3:
	{
		SetMaxParamDefault<Point3>(pDesc, paramId, defaultVal);
		break;
	}
	case TYPE_FRGBA:
	case TYPE_POINT4:
	{
		SetMaxParamDefault<Point4>(pDesc, paramId, defaultVal);
		break;
	}
	case TYPE_MATRIX3:
	{
		SetMaxParamDefault<Matrix3>(pDesc, paramId, defaultVal);
		break;
	}
	case TYPE_STRING:
	{
		SetMaxParamDefault<MSTR>(pDesc, paramId, defaultVal);
		break;
	}
	case TYPE_INODE:
	case TYPE_REFTARG:
	{
		// No default possible for this type.
		break;
	}
	default:
		DbgAssert(0 && "Implment me");
	}
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
#pragma region Evaluation


template<typename TBaseClass, typename TResultType>
bool FabricTranslationLayer<TBaseClass, TResultType>::GraphCanEvaluate()
{
	if (_m_isSyncing)
		return false;

	if (!m_binding.isValid())
		return false;

	FabricCore::DFGExec rootExec = m_binding.getExec();
	if (!rootExec.isValid())
		return false;

	return (rootExec.getErrorCount() == 0);
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::SetupEvalContext(TimeValue t)
{
	FabricCore::Client& client = GetClient();
	if (!m_evalContext.isValid())
	{
		m_evalContext = FabricCore::RTVal::Create(client, "EvalContext", 0, 0);
		m_evalContext = m_evalContext.callMethod("EvalContext", "getInstance", 0, 0);
		m_evalContext.setMember("host", FabricCore::RTVal::ConstructString(client, "3dsMax"));

		CStr filename = GetCOREInterface()->GetCurFilePath().ToCStr();
		m_evalContext.setMember("currentFilePath", FabricCore::RTVal::ConstructString(client, filename.data()));
	}
	if (m_evalContext.isValid())
	{
		CStr graphName = GetGraphName().ToCStr();
		m_evalContext.setMember("graph", FabricCore::RTVal::ConstructString(client, graphName.data()));
		m_evalContext.setMember("time", FabricCore::RTVal::ConstructFloat32(client, TicksToSec(t)));
	}
}

template<typename TBaseClass, typename TResultType>
const TResultType& FabricTranslationLayer<TBaseClass, TResultType>::Evaluate(TimeValue t, Interval& ivValid)
{
	if (!GraphCanEvaluate())
		return m_value;

	// We set our synching flag here to indicate that we are sending
	// values to Fabric.  This is theoretically to prevent our params changing
	// while evaluating, but in reality is to prevent re-entry if 
	// we pop a dialog due to an exception.  If that happens, then Max starts
	// processing it's messages, and we recurse straight back into this function.
	DoSyncing ds(*this);
	// We suspend undo's while copying values over, as these values are 
	// managed by Max and Splice does not need to worry about undoing them
	HoldSuspend hs;

	// If our value is valid, do not re-evaluate
	if (!m_valid.InInterval(t))
	{
		MAXSPLICE_CATCH_BEGIN;

		SetupEvalContext(t);

		//bool usesTime = m_graph.usesEvalContext();
		//if(usesTime)
		//{
		//	// setup the context.  Perhaps we should do this regardless?
		//	FabricCore::RTVal evalContext = m_graph.getEvalContext();
		//	evalContext.setMember("time", Fabric::constructFloat32RTVal(TicksToSec(t)));
			m_valid.SetInstant(t);
		//	// Force a re-evaluate (in case no other input parameters change)
		//	m_graph.clearEvaluate();
		//	m_graph.requireEvaluate();
		//}
		//else
		//{
		//	// Reset our internal validity times;
		//	m_valid.SetInfinite();
		//}

		// Set  all Max values on their splice equivalents
		TransferAllMaxValuesToFabric(t, m_pblock, m_binding, m_portValidities, m_valid);

		// Trigger graph evaluation
		m_binding.execute();

		// Get our value back!
		FabricCore::DFGExec exec = m_binding.getExec();
		if (exec.haveExecPort(m_outArgName.c_str()) && exec.hasSrcPort(m_outArgName.c_str()))
		{
			FabricCore::RTVal rtOutVal = m_binding.getArgValue(m_outArgName.c_str());
			FabricToMaxValue(rtOutVal, m_value);
		}
		
		MAXSPLICE_CATCH_END;

	}

	// Let max know how long our value is valid for.
	ivValid &= m_valid;
	return m_value;
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::TriggerEvaluate( TimeValue t, Interval& ivValid )
{
	// Call evaluate
	Evaluate(t, ivValid);
}

template<typename TBaseClass, typename TResultType>
void FabricTranslationLayer<TBaseClass, TResultType>::InitMixinInterface()
{
	GetDescriptor<TBaseClass, TResultType>();
}
