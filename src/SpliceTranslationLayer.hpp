#pragma once // hpp files are actually headers

#include "SpliceTranslationLayer.h"
#include "DynamicParamBlocks/DynPBCustAttrClassDesc.h"
#include "DynamicParamBlocks/DynamicDialog/DynamicDialogTemplate.h"
#include "DynamicParamBlocks/DynPBUndo.h"
#include "SpliceLogging.h"
#include "MaxConversionFns.h"
#include "Splice3dsmax.h"
#include "SpliceRestoreObjects.h"
#include "MaxDFGNotificationRouter.h"
#include "MaxDFGController.h"
#include "ASTWrapper/KLASTManager.h"
#include "MaxCommandStack.h"

//////////////////////////////////////////////////////////////////////////
//--- SpliceTranslationLayer -------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------
// Constructor/Destructor
//---------------------------------------------------------------------------------------------------------------
#pragma region//Constructor/Destructor
template<typename TBaseClass, typename TResultType>
SpliceTranslationLayer<TBaseClass, TResultType>::SpliceTranslationLayer(BOOL loading)
	:	m_hPanel(NULL)
	,	m_pInterface(NULL)
	,	m_pMtlInterface(NULL)
	,	m_dialogTemplate(NULL)
	,	m_paramMap(NULL)
	,	m_pblock(NULL)
	,	m_valid(NEVER)
{
	InstanceCreated();

	if (!loading)
		Init(loading);
}

template<typename TBaseClass, typename TResultType>
SpliceTranslationLayer<TBaseClass, TResultType>::~SpliceTranslationLayer()
{
	// We _must_ have been released by now
	DbgAssert(m_pblock == NULL);

	SAFE_DELETE(m_router);

	// Now is a good time to release any old pbdescs
	// If we are in a reset, our pblock would have 
	// been released by now, and the descriptor would
	// be registered with our CD for release.
	DynPBCustAttrClassDesc* ourCD = GetClassDesc();
	ourCD->ReleaseObsoletePBDesc();

	InstanceDeleted();
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::Init(BOOL loading)
{
	MAXSPLICE_CATCH_BEGIN();

	//if (!loading)
	{
		// create an empty binding
		m_binding = GetHost().createBindingToNewGraph();
		m_binding.setNotificationCallback(bindingNotificationCallback, this);

		m_router = new MaxDFGNotificationRouter(this, m_binding, m_binding.getExec());

		FabricServices::ASTWrapper::KLASTManager* manager = FabricServices::ASTWrapper::KLASTManager::retainGlobalManager(&GetClient());
		m_ctrl = new MaxDFGController(NULL, GetCommandStack(), manager, m_binding, m_binding.getExec(), false);
		m_ctrl->setRouter(m_router);

		// set the graph on the view
		//setGraph(DFGWrapper::GraphExecutablePtr::StaticCast(m_binding.getExecutable()));

		//// create a graph to hold our dependency graph nodes.
		//m_graph = FabricCore::DFGBinding("3dsMaxGraph");
		//m_graph.constructDGNode();
		//m_graph.setUserPointer(this);

		// Set static context values
		//MSTR filepath = GetCOREInterface()->GetCurFilePath();
		//FabricCore::RTVal evalContext = m_graph.getEvalContext();
		//evalContext.setMember("host", FabricSplice::constructStringRTVal("3dsMax"));

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

		//evalContext.setMember("currentFilePath", FabricSplice::constructStringRTVal(filepath.ToCStr().data()));
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
ParamBlockDesc2* SpliceTranslationLayer<TBaseClass, TResultType>::CopyPBDescriptor()
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
			SetMaxParamLimits(pNewDesc, m_binding, argName);
			SetMaxParamDefault(pNewDesc, newPid, m_binding, argName);
		}
	}
	// Return the new descriptor. This object is now the
	// responsibility of the caller
	return pNewDesc;
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::DeleteMaxParameter(ParamID pid)
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
void SpliceTranslationLayer<TBaseClass, TResultType>::CreateParamBlock( ParamBlockDesc2* pDesc)
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
#pragma region //Dynamic UI methods


template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::GeneratePBlockUI()
{
	if (m_dialogTemplate == NULL) 
		m_dialogTemplate = ::GeneratePBlockUI(m_pblock);

	return m_dialogTemplate != NULL;
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::UpdateUISpec()
{
	MaybeRemoveParamUI();
	SAFE_DELETE(m_dialogTemplate);
	MaybePostParamUI();
}

#pragma endregion

//---------------------------------------------------------------------------------------------------------------
// Inherited methods - UI related
//---------------------------------------------------------------------------------------------------------------

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::MaybeRemoveParamUI()
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
void SpliceTranslationLayer<TBaseClass, TResultType>::MaybePostParamUI()
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
void SpliceTranslationLayer<TBaseClass, TResultType>::BeginEditParams(IObjParam *ip, ULONG flags, Animatable *UNUSED(prev))
{
	this->m_pInterface = ip;
	m_hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PARAM_GENERATION_UI),
		DlgProc,
		GetString(IDS_PARAMS),
		(LPARAM)static_cast<SpliceTranslationFPInterface*>(this));

	// Generate our Dialog, and assign an mParamMap
	MaybePostParamUI();

	// For each port, see if it wants to post a UI too
	if (m_pblock != nullptr) 
	{
		//int portCount = GetPortCount();
		//for (int i = 0; i < portCount; i++) 
		//{
		//	DFGWrapper::ExecPortPtr aPort = GetPort(i);
		//	if (GetPortPostConnectionUI(aPort))
		//	{
		//		// UI Requested: what is the max connection for this parameter?
		//		int pid = ::GetPortParamID(aPort);
		//		if (pid != -1)
		//		{
		//			// Double check - we can only post UI for class types
		//			if (reftarg_type(m_pblock->GetParameterType(ParamID(pid)))) 
		//			{
		//				ReferenceTarget* pSrcContainer = m_pblock->GetReferenceTarget(ParamID(pid));
		//				if (pSrcContainer == nullptr)
		//					continue;

		//				//SpliceTranslationFPInterface* pSrcContInterface = static_cast<SpliceTranslationFPInterface*>(pSrcContainer->GetInterface(ISPLICE__INTERFACE));
		//				//if (pSrcContInterface == nullptr)
		//				//	continue;

		//				// Everything checks out... go ahead and post a UI
		//				pSrcContainer->BeginEditParams(ip, flags, nullptr);
		//			}
		//		}
		//	}
		//}
	}
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::EndEditParams(IObjParam *ip, ULONG flags, Animatable *UNUSED(next))
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
ParamDlg *SpliceTranslationLayer<TBaseClass, TResultType>::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)
{
	m_pMtlInterface = imp;

	// First, add our static UI's
	m_hPanel = imp->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PARAM_GENERATION_UI),
		DlgProc,
		GetString(IDS_PARAMS).data(),
		(LPARAM)static_cast<SpliceTranslationFPInterface*>(this));

	return CreateMyAutoParamDlg(hwMtlEdit, imp);
}

template<typename TBaseClass, typename TResultType>
ParamDlg* SpliceTranslationLayer<TBaseClass, TResultType>::CreateMyAutoParamDlg(HWND hwMtlEdit, IMtlParams* pMtlParams) 
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
void SpliceTranslationLayer<TBaseClass, TResultType>::SetReference(int i, RefTargetHandle rtarg)
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
			if (theHold.Holding()) 
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
RefResult SpliceTranslationLayer<TBaseClass, TResultType>::NotifyRefChanged( Interval , RefTargetHandle , PartID&	, RefMessage message )
#else
template<typename TBaseClass, typename TResultType>
RefResult SpliceTranslationLayer<TBaseClass, TResultType>::NotifyRefChanged(const Interval& changeInt, RefTargetHandle hTarget, PartID& partID, RefMessage message, BOOL propagate)
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

		// If we are here, we _must_ be in between BeginEditParams
		// and EndEditParams.  Remove the current UI and repost
		UpdateUISpec();
	}

	return REF_SUCCEED;
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::RefDeleted() {
	// If we have no nodes referencing this class, then kill our UI
	//if (GetKLEditor() != nullptr)
	//{
		ULONG handle = 0;
		NotifyDependents(FOREVER, (PartID)&handle, REFMSG_GET_NODE_HANDLE);
		if (handle == 0)
			CloseKLEditor();
	//}
}



template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::RefAdded(	RefMakerHandle 	rm) {
	if (m_binding.isValid())
	{
		MAXSPLICE_CATCH_BEGIN();

		// Set static context values
		//MSTR filepath = GetCOREInterface()->GetCurFilePath();
		//FabricCore::RTVal evalContext = m_graph.getEvalContext();

		// given a scene base object or modifier, look for a referencing node via successive 
		// reference enumerations up through the ref hierarchy untill we find an inode.
		INode* node = GetCOREInterface7()->FindNodeFromBaseObject(this);
		if (node) {
			m_binding.getExec().setTitle(TSTR(node->GetName()).ToCStr().data());
		}

		MAXSPLICE_CATCH_END;
	}
}
#pragma endregion

//---------------------------------------------------------------------------------------------------------------
// Inherited methods - Everything Else
//---------------------------------------------------------------------------------------------------------------
#pragma region//Clone
template<typename TBaseClass, typename TResultType>
ReferenceTarget *SpliceTranslationLayer<TBaseClass, TResultType>::Clone(RemapDir &remap)
{
	SpliceTranslationLayer *pMyClone = reinterpret_cast<SpliceTranslationLayer*>(GetClassDesc()->Create(TRUE));

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
	CloneSpliceData(pMyClone);

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
IOResult SpliceTranslationLayer<TBaseClass, TResultType>::Save( ISave *isave )
{	
	// Save out all the data needed to recreate parameters
	isave->BeginChunk(PARAM_SPLICE_DATA);
	FabricCore::DFGStringResult json = m_binding.exportJSON();
	isave->WriteCString(json.getCString());
	isave->EndChunk();

	// Save the value port.
	isave->BeginChunk(PARAM_VALUE_NAME);
	const char* outName = GetOutPortName();
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
	SpliceTranslationFPInterface* m_owner;

public:
	RenameMaxParamsCallback(SpliceTranslationFPInterface* pOwner) : m_owner(pOwner) {}
	virtual void proc( ILoad *iload ) { m_owner->ReconnectPostLoad(); delete this; }
};

// Load our local parameters
template<typename TBaseClass, typename TResultType>
IOResult SpliceTranslationLayer<TBaseClass, TResultType>::Load( ILoad *iload )
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
					//Init(true);
					//m_binding = GetHost().createBindingFromJSON(buff);
					//m_binding.setNotificationCallback(bindingNotificationCallback, this);
					//// set the graph on the view
					//setGraph(DFGWrapper::GraphExecutablePtr::StaticCast(m_binding.getExecutable()));
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
	// Reset/reconnect our time/parent ports
	//ResetPorts();
	// Get our output port
	SetOutPort(outName.data());

	return IO_OK;
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::ReconnectPostLoad()
{
	//DFGWrapper::ExecPortList ports = m_binding.getExecutable()->getPorts();
	FabricCore::DFGExec exec = m_binding.getExec();
	int nPorts = exec.getExecPortCount();
	for (int i = 0; i < nPorts; i++) {
		const char* argName = exec.getExecPortName(i);
		int pid = GetPortParamID(m_binding, argName);
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
#pragma region MaxScript function

template<typename TBaseClass, typename TResultType>
std::string SpliceTranslationLayer<TBaseClass, TResultType>::GetKLCode()
{
	MAXSPLICE_CATCH_BEGIN()

	std::string cName = GetKLOperatorName();
	//if (m_graph.getKLOperatorCount() > 0)
	//{
	//	// If we have a valid graph, return code
	//	// for our classes operator
	//	return m_graph.getKLOperatorSourceCode(cName.data());
	//}

	// generate default operator code.

	// If we are generating sample code, ensure that we 
	// have all the necessary ports initialized
	// Otherwise they won't be included in the sample code
	if (strcmp(GetOutPortName(), "") == 0)
	{
		std::string cOutPortName = cName + "Val";
		SetOutPort(cOutPortName.data());
	}

	// Generate sample operator code and return
	//FabricCore::Variant generatedCode = m_graph.generateKLOperatorSourceCode(cName.data());
	//return generatedCode.getString_cstr();

	MAXSPLICE_CATCH_RETURN(std::string())
		return "";
}

template<typename TBaseClass, typename TResultType>
std::string SpliceTranslationLayer<TBaseClass, TResultType>::GetKLOperatorName()
{
	MAXSPLICE_CATCH_BEGIN()

	//if (m_graph.getKLOperatorCount() > 0)
	//{
	//	// If we have a graph, get default name (TODO: Explicitly find
	//	// our operators name (can we do that?)
	//	return m_graph.getKLOperatorName();
	//}

	// By default, we use the internal name as our operator name (The MaxScript name)
	std::string cDefName = CStr::FromMCHAR(GetClassDesc()->InternalName());
	return cDefName;

	MAXSPLICE_CATCH_RETURN(std::string())
}

template<typename TBaseClass, typename TResultType>
std::string SpliceTranslationLayer<TBaseClass, TResultType>::SetKLCode(const std::string& entry, const std::string& script)
{

	if (entry.empty())
		return "ERROR: No entry point specified";// TODO: Globalize this
	if (script.empty())
		return "ERROR: Cannot cannot compile empty script";// TODO: Globalize this

	GatherCompilerResults doGather;

	// Try-catch
	MAXSPLICE_CATCH_BEGIN()

		// Try initializing 
		bool success = false;

		//DFGWrapper::GraphExecutablePtr graph = DFGWrapper::GraphExecutablePtr::StaticCast(GetBinding().getExecutable());

		// add a report node
		//DFGWrapper::NodePtr reportNode = graph->addNodeFromPreset("Fabric.Core.Func.Report");
		//m_binding.getExecutable()
		//FabricCore::DFGExec& coreExec = m_binding.getExecutable()->getWrappedCoreExec();

		//const char* cnode = coreExec.addNodeWithNewFunc(entry.data());
		//cnode;
		//int nNodes = graph->getNodes().size();
		//if (nNodes == 1)
		//{
		//	DFGWrapper::NodePtr funcNode = graph->addNodeWithNewFunc("DummyFunc");
		//	bool wtf = funcNode->isFunc();
		//	wtf;
		//}
		//nNodes = graph->getNodes().size();

		//

		//DFGWrapper::FuncExecutablePtr realFunc = DFGWrapper::FuncExecutable::Create(GetBinding().getWrappedCoreBinding(), GetBinding().getWrappedCoreBinding().getExec(), entry.c_str());
		//nNodes = graph->getNodes().size();
		//realFunc->setCode(script.c_str());

		// add a report node
		//const char* crnode = coreExec.addNodeFromPreset("Fabric.Core.Func.Report");
		//crnode;
		//DFGWrapper::NodePtr reportNode = coreExec.addNodeFromPreset("Fabric.Core.Func.Report");

		//nNodes = coreExec.getNodeCount();

		//FabricCore::DFGExec& fnExec = coreExec.getNo(0);

		//coreExec.setCode()

		// Are we renaming?  In that case, remove the old operator
		//if (!m_operator.empty() && m_operator != entry)
		//{
		//	m_graph.removeKLOperator(m_operator.c_str());
		//}

		//// If this named operator exists on the graph, update it.
		//if (m_graph.hasKLOperator(entry.c_str()))
		//{
		//	success = m_graph.setKLOperatorSourceCode(entry.c_str(), script.c_str());
		//}
		//else // create a new operator
		//{
		//	success = m_graph.constructKLOperator(entry.c_str(), script.c_str());
		//}
		if (success)
		{
			//m_operator = entry;
			// Tell max, we might have changed here.
			NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
			m_valid = NEVER;
			doGather.LogSomething("OK");
		}                                                                                                                                                                                                                                                           
		else
		{
			// Theoretically, we should never actually reach this code.  Fabric will throw if somethins outta whack
			doGather.LogSomething("Unknown Error");
		}

	MAXSPLICE_CATCH_END;
	return doGather.GetGatheredResults();
}

//////////////////////////////////////////////////////////////////////////
// Add Ports
template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::AddInputPort(const char* name, const char* spliceType, int maxType/* =-1 */, bool isArray/*=false*/, const char* inExtension)
{
	//HoldActions hold(_M("Add Input Port"));

	//if (theHold.Holding())
	//	theHold.Put(new SplicePortChangeObject(this));

	AddSpliceParameter(m_binding, spliceType, name, FabricCore::DFGPortType_In, isArray, inExtension);
	// Can/Should we create a matching max type for this?
	// A matching Max type should automatically be generated
	return 0;
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::AddOutputPort(const char* name, const char* spliceType, bool isArray/*=false*/, const char* inExtension)
{
	//HoldActions hold(_M("Add Output Port"));
	//if (theHold.Holding())
	//	theHold.Put(new SplicePortChangeObject(this));
	// Create the port
	bool success = AddSpliceParameter(m_binding, spliceType, name, FabricCore::DFGPortType_Out, isArray, inExtension);
	return success ? 1 : 0;
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::AddIOPort(const char* name, const char* spliceType, int maxType/* =-1 */, bool isArray/*=false*/, const char* inExtension)
{
	HoldActions hold(_M("Add IO Port"));
	if (theHold.Holding())
		theHold.Put(new SplicePortChangeObject(this));

	// Create the port
	AddSpliceParameter(m_binding, spliceType, name, FabricCore::DFGPortType_IO, isArray, inExtension);

	// Can/Should we create a matching max type for this?
	return SetMaxConnectedType(name, maxType);
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::RemovePort(const char* argName)
{
	m_binding.getExec().removeExecPort(argName);
	return true;
}

template<typename TBaseClass, typename TResultType>
const char* SpliceTranslationLayer<TBaseClass, TResultType>::GetPortName(int i)
{
	return m_binding.getExec().getExecPortName(i);
}

template<typename TBaseClass, typename TResultType>
const char* SpliceTranslationLayer<TBaseClass, TResultType>::SetPortName(const char* oldName, const char* newName)
{
	MAXSPLICE_CATCH_BEGIN()

	if (strcmp(oldName, newName) == 0)
		return newName;

	// A rename will attempt the desired name, but in the case of a name-clash will return the new name
	const char* res = m_binding.getExec().renameExecPort(oldName, newName);
	return res;

	MAXSPLICE_CATCH_RETURN("*Exception*");
}

template<typename TBaseClass, typename TResultType>
const char* SpliceTranslationLayer<TBaseClass, TResultType>::GetPortType(const char* argName)
{
	return ::GetPortType(m_binding, argName);
}


template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::IsPortArray(const char* argName)
{
	FabricCore::RTVal rtVal = m_binding.getArgValue(argName);
	return rtVal.isValid() && rtVal.isArray();
}


template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::SetOutPort(const char* argName)
{
	MAXSPLICE_CATCH_BEGIN()

	// can this port be translated to our out-type?
	BitArray legalTypes = SpliceTypeToMaxTypes(::GetPortType(m_binding, argName));
	if (!legalTypes[GetValueType()])
		return false;

	// Everything checks out - this port is now our value
	m_outArgName = argName;
	Invalidate();
	NotifyDependents(FOREVER, PART_ALL, REFMSG_CHANGE);
	return true;

	MAXSPLICE_CATCH_RETURN(false);
}

//////////////////////////////////////////////////////////////////////////
template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::ConnectPort( const char* myargName, ReferenceTarget* pSrcContainer, const char* srcargName, int srcPortIndex, bool postConnectionsUI )
{
	//if (pSrcContainer == nullptr)
	//	return false;

	//SpliceTranslationFPInterface* pSrcContInterface = static_cast<SpliceTranslationFPInterface*>(pSrcContainer->GetInterface(ISPLICE__INTERFACE));
	//if (pSrcContInterface == nullptr)
	// return false;

	//MAXSPLICE_CATCH_BEGIN()

	//// First, do our ports exist?
	//DFGWrapper::ExecPortPtr srcPort = pSrcContInterface->GetPort(srcargName);
	//if (!srcPort || srcPort.getMode() == FabricCore::DFGPortType_In)
	//	return false;
	//DFGWrapper::ExecPortPtr destPort = GetPort(myargName);
	//if (!destPort || destPort.getMode() != FabricCore::DFGPortType_In)
	//	return false;

	//// Are they of the same type?
	//if (strcmp(srcPort.getDataType(), destPort.getDataType()) != 0)
	//	return false;

	//// Are we connecting to an array, and is that expected?
	//if ((srcPortIndex >= 0) && !srcPort.isArray())
	//	return false;

	//if (theHold.Holding())
	//	theHold.Put(new SplicePortChangeObject(this));

	//// Ok - these ports are good to go.  Connect 'em up.
	//int res = SetMaxConnectedType(destPort, TYPE_REFTARG);
	//if (res < 0)
	//	return false;
	//m_pblock->SetValue((ParamID)res, 0, pSrcContainer);
	//SetPortConnection(destPort, srcargName);
	//SetPortConnectionIndex(destPort, srcPortIndex);
	//SetPortPostConnectionUI(destPort, postConnectionsUI);

	//// Ensure there are enough indices in the RTVal array
	//if (srcPortIndex >= 0) 
	//{
	//	FabricCore::RTVal rtVal = srcPort.getRTVal();
	//	if (rtVal.getArraySize() <= (uint32_t)srcPortIndex)
	//		rtVal.setArraySize(srcPortIndex + 1);
	//}

	//MAXSPLICE_CATCH_RETURN(false)

	return true;
}


template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::DisconnectPort(const char* myargName)
{
	//DFGWrapper::ExecPortPtr connectedPort = GetPort(myargName);
	//if (connectedPort) 
	//{
	//	std::string connection = GetPortConnection(connectedPort);
	//	if (connection == "") 
	//	{
	//		if (theHold.Holding())
	//			theHold.Put(new SplicePortChangeObject(this));

	//		SetMaxConnectedType(connectedPort, -1);
	//		connectedPort.setOption(MAX_SRC_OPT, FabricCore::Variant());
	//		return true;
	//	}
	//}
	return false;
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::GetMaxConnectedType( const char* argName )
{
	int pid = ::GetPortParamID(m_binding, argName);
	if (pid >= 0)
	{
		return m_pblock->GetParameterType((ParamID)pid);
	}
	return -2;
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::SetMaxConnectedType(const char* argName, int maxType)
{
	// The user has requested the default type (-2)
	const char* spliceType = ::GetPortType(m_binding, argName);
	if (maxType == -2)
		maxType = SpliceTypeToDefaultMaxType(spliceType);
	else if (maxType == -1) 
	{ 
		/* do nothing here, delete was requested */ 
	}
	else
	{
		// Figure out what kind of parameter
		// we can/will create on the max side
		BitArray legalTypes = SpliceTypeToMaxTypes(spliceType);
		// The requested max type is not legal for this splice type
		if (!legalTypes[maxType])
			maxType = SpliceTypeToDefaultMaxType(spliceType); // Reset to default
	}

	// Cache the type being set
	SetPort3dsMaxType(m_binding, argName, maxType);

	// -1 means no max connection, remove one if exists
	int pid = ::GetPortParamID(m_binding, argName);
	if (maxType == -1) 
	{
		if (pid >= 0)
			DeleteMaxParameter((ParamID)pid);
		// Set -1 as the param id so we no longer look for a max param for this port.
		::SetPortParamID(m_binding, argName, -1);
		return -1;
	}

	// Do we have an existing Max parameter?
	if (pid >= 0 && m_pblock != nullptr)
	{
		// Check that its not the correct type already
		if (m_pblock->GetParamDef((ParamID)pid).type == maxType)
			return pid;

		// Wrong type, remove original
		DeleteMaxParameter((ParamID)pid);
	}

	// If this is a legal type for this parameter, then go ahead
	ParamBlockDesc2* pNewDesc = CopyPBDescriptor();
	ParamID newId = AddMaxParameter(pNewDesc, maxType, argName);
	SetMaxParamLimits(pNewDesc, m_binding, argName);
	SetMaxParamDefault(pNewDesc, newId, m_binding, argName);
	::SetPortParamID(m_binding, argName, newId);
	CreateParamBlock(pNewDesc);

	// Set the value to the current port value
	SetMaxParamFromSplice(m_pblock, newId, m_binding, argName);
	return newId;
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::SetPortOption(const char* argName, const char* option, FPValue* value)
{
	throw("Whats happening here");
	return ::SetPortOption(m_binding, argName, option, value);
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::SetPortValue(const char* argName, FPValue* value)
{
	return ::SetPortValue(m_binding, argName, value);
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::SetPortUIMinMax(const char* argName, FPValue* uiMin, FPValue* uiMax)
{
	return false;
	//DFGWrapper::ExecPortPtr aPort = GetPort(argName);
	//if (!aPort)
	//	return false;

	//bool res = true;
	//res &= ::SetPortOption(aPort, "uiMin", uiMin);
	//res &= ::SetPortOption(aPort, "uiMax", uiMax);
	//if (m_pblock != nullptr) 
	//{
	//	int pid = ::GetPortParamID(aPort);
	//	if (pid >= 0)
	//	{
	//		MaybeRemoveParamUI();
	//		SetMaxParamLimits(m_pblock->GetDesc(), (ParamID)pid, aPort);
	//		// Delete existing autogen UI
	//		SAFE_DELETE(m_dialogTemplate);
	//		MaybePostParamUI();
	//	}
	//}
	//return res;
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::AddNewEmptyGraph(const char* name)
{
	return false;
}
template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::AddNewEmptyFunc(const char* name)
{
	return false;
}
template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::AddNodeFromPreset(const char* name, const char* path)
{
	return false;
}

//template<typename TBaseClass, typename TResultType>
//void SpliceTranslationLayer<TBaseClass, TResultType>::SetSpliceGraph(const FabricCore::DFGBinding& binding, bool createMaxParams) 
//{ 
//	// First, clear any existing parameters
//	ReplaceReference(0, nullptr);
//
//	// Reset all variables.
//	m_binding = binding; 
//
//	// Invalid current value
//	m_valid = NEVER;
//
//	ResetPorts();
//
//	// Early exit
//	if (!m_binding.isValid())
//		return;

	// Try to find an output value for our class
	//bool foundOutPort = false;
	//int nPorts = m_graph.getDGPortCount();
	//for (int i = 0; i < nPorts; i++)
	//{
	//	std::string argName = m_graph.getDGPortName(i);
	//	DFGWrapper::ExecPortPtr port = m_graph.getDGPort(i);
	//	if(!port.isValid())
	//		continue;

	//	FabricCore::DFGPortType portMode = port.getMode();

	//	if (portMode != FabricCore::DFGPortType_In)
	//	{
	//		if (!foundOutPort)
	//		{
	//			const char* sDataType = port.getDataType();
	//			int type = SpliceTypeToMaxType(sDataType);
	//			// Is this legal for us?  If so, connect our value port
	//			if (type == GetValueType())
	//			{
	//				SetOutPort(port);
	//				bool isArray = port.isArray();
	//				if (isArray)
	//					SetOutPortArrayIdx(0);
	//				foundOutPort = true;
	//			}
	//		}
	//	}
	//	else if (createMaxParams)
	//	{
	//		// We have been requested to create Max parameters for all possible ports.
	//		SetMaxConnectedType(port, -2); // Create default type
	//	}
	//}

	//if (!foundOutPort)
	//	m_valuePort = DFGWrapper::ExecPortPtr();
//};

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::LoadFromFile(const MCHAR* filename, bool createMaxParams) 
{ 
	
	FILE * file = nullptr;
	errno_t erno = _tfopen_s(&file, filename, _T("rb"));
	if (!file)
	{
		erno;
		//log()
		// TODO - Fix logging!
	}

	fseek(file, 0, SEEK_END);
	int fileSize = ftell(file);
	rewind(file);

	char * buffer = (char*)malloc(fileSize + 1);
	buffer[fileSize] = '\0';

	size_t readBytes = fread(buffer, 1, fileSize, file);
	assert(readBytes == fileSize);

	fclose(file);

	RestoreFromJSON(buffer, createMaxParams);
	free(buffer);
	return true;
};

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::SaveToFile(const MCHAR* filename) 
{ 

	std::string json;
	ExportToJSON(json);

	FILE * file = nullptr;
	errno_t erno = _tfopen_s(&file, filename, _T("w"));
	if (!file)
	{
		erno;
		//log()
		// TODO - Fix logging!
		return false;
	}

	fwrite(json.data(), sizeof(char), json.size(), file);
	fclose(file);

	return true;
};

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::RestoreFromJSON(const char* json, bool createMaxParams)
{
	// The KL Editor has pointers to the current graph
	CloseKLEditor();

	m_binding = GetHost().createBindingFromJSON(json);
	m_binding.setNotificationCallback(bindingNotificationCallback, this);

	// Hook up our router again
	SAFE_DELETE(m_router);
	m_router = new MaxDFGNotificationRouter(this, m_binding, m_binding.getExec());
	m_router->setController(m_ctrl);

	// Setup port connections
	if (createMaxParams)
	{
		FabricCore::DFGExec exec = m_binding.getExec();

		for (unsigned int i = 0; i < exec.getExecPortCount(); i++)
		{
			const char* portName = exec.getExecPortName(i);
			// If this could be our out port?
			if (exec.getExecPortType(i) == FabricCore::DFGPortType_Out)
			{
				exec.getExecPortResolvedType(i);
				BitArray compatibleTypes = SpliceTypeToMaxTypes(::GetPortType(m_binding, portName));
				// If this splice type is compatible with this classes output,
				// set this port as our outport
				if (compatibleTypes[GetValueType()])
					m_outArgName = portName;
			}
			else
			{
				// for input ports, we should trigger Max type creation
				m_router->onExecPortResolvedTypeChanged(portName, "");
			}
		}
	}
	InvalidateAll();
	return true;
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::ExportToJSON(std::string &outJson)
{
	outJson = m_binding.exportJSON();
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::ResetPorts()
{
	// Setup any necessary ports for the current graph
	m_outArgName = AddSpliceParameter(GetBinding(), GetValueType(), "outputValue", FabricCore::DFGPortType_Out);
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
#pragma region Evaluation


template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::GraphCanEvaluate()
{
	if (!m_binding.isValid())
		return false;

	FabricCore::DFGExec rootExec = m_binding.getExec();
	if (!rootExec.isValid())
		return false;

	return (rootExec.getErrorCount() == 0);
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::SetupEvalContext(TimeValue t)
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
const TResultType& SpliceTranslationLayer<TBaseClass, TResultType>::Evaluate(TimeValue t, Interval& ivValid)
{
	if (!GraphCanEvaluate())
		return m_value;


	// If our value is valid, do not re-evaluate
	if (!m_valid.InInterval(t))
	{
		MAXSPLICE_CATCH_BEGIN();

		SetupEvalContext(t);

		//bool usesTime = m_graph.usesEvalContext();
		//if(usesTime)
		//{
		//	// setup the context.  Perhaps we should do this regardless?
		//	FabricCore::RTVal evalContext = m_graph.getEvalContext();
		//	evalContext.setMember("time", FabricSplice::constructFloat32RTVal(TicksToSec(t)));
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
		TransferAllMaxValuesToSplice(t, m_pblock, m_binding, m_portValidities, m_valid);

		// Trigger graph evaluation
		m_binding.execute();

		// Get our value back!
		if (m_binding.getExec().haveExecPort(m_outArgName.c_str()))
		{
			FabricCore::RTVal rtOutVal = m_binding.getArgValue(m_outArgName.c_str());
			SpliceToMaxValue(rtOutVal, m_value);
		}
		
		MAXSPLICE_CATCH_END;

	}

	// Let max know how long our value is valid for.
	ivValid &= m_valid;
	return m_value;
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::TriggerEvaluate( TimeValue t, Interval& ivValid )
{
	// Call evaluate
	Evaluate(t, ivValid);
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::InitMixinInterface()
{
	GetDescriptor<TBaseClass, TResultType>();
}
