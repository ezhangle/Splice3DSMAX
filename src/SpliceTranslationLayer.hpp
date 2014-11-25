#pragma once // hpp files are actually headers

#include "SpliceTranslationLayer.h"
#include "DynamicParamBlocks/DynPBCustAttrClassDesc.h"
#include "DynamicParamBlocks/DynamicDialog/DynamicDialogTemplate.h"
#include "DynamicParamBlocks/DynPBUndo.h"
#include "SpliceLogging.h"
#include "MaxConversionFns.h"
#include "Splice3dsmax.h"
#include "SpliceRestoreObjects.h"

//////////////////////////////////////////////////////////////////////////
//--- SpliceTranslationLayer -------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------
// Constructor/Destructor
//---------------------------------------------------------------------------------------------------------------
#pragma region//Constructor/Destructor
template<typename TBaseClass, typename TResultType>
SpliceTranslationLayer<TBaseClass, TResultType>::SpliceTranslationLayer(bool init/*=true*/)
	:	m_hPanel(NULL)
	,	m_pInterface(NULL)
	,	m_pMtlInterface(NULL)
	,	m_dialogTemplate(NULL)
	,	m_paramMap(NULL)
	,	m_pblock(NULL)
	,	m_valid(NEVER)
	,	m_valuePortIndex(-1)
{
	if (init)
		Init();
}

template<typename TBaseClass, typename TResultType>
SpliceTranslationLayer<TBaseClass, TResultType>::~SpliceTranslationLayer()
{
	// We _must_ have been released by now
	DbgAssert(m_pblock == NULL);

	// Now is a good time to release any old pbdescs
	// If we are in a reset, our pblock would have 
	// been released by now, and the descriptor would
	// be registered with our CD for release.
	DynPBCustAttrClassDesc* ourCD = GetClassDesc();
	ourCD->ReleaseObsoletePBDesc();
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::Init()
{
	if (!m_graph.isValid())
	{
		MAXSPLICE_CATCH_BEGIN();

		// create a graph to hold our dependency graph nodes.
		m_graph = FabricSplice::DGGraph("3dsMaxGraph");
		m_graph.constructDGNode();
		m_graph.setUserPointer(this);

		// Set static context values
		MSTR filepath = GetCOREInterface()->GetCurFilePath();
		FabricCore::RTVal evalContext = m_graph.getEvalContext();
		evalContext.setMember("host", FabricSplice::constructStringRTVal("3dsMax"));
		evalContext.setMember("graph", FabricSplice::constructStringRTVal(m_graph.getName()));
		evalContext.setMember("currentFilePath", FabricSplice::constructStringRTVal(filepath.ToCStr().data()));

		ResetPorts();

		MAXSPLICE_CATCH_RETURN(false);

		return true;
	}
	return false;
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
			AddMaxParameter(pNewDesc, pbDef.type, pbDef.int_name, curPId);
		}
	}
	// Return the new descriptor. This object is now the
	// responsibility of the caller
	return pNewDesc;
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::DeleteMaxParameter(ParamID pid)
{
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
void SpliceTranslationLayer<TBaseClass, TResultType>::BeginEditParams(IObjParam *ip, ULONG UNUSED(flags), Animatable *UNUSED(prev))
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
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::EndEditParams(IObjParam *ip, ULONG UNUSED(flags), Animatable *UNUSED(next))
{
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
template<typename TBaseClass, typename TResultType>
RefResult SpliceTranslationLayer<TBaseClass, TResultType>::NotifyRefChanged( Interval , RefTargetHandle , PartID&	, RefMessage message )
{
	// set splices dirty flag.
	if (message == REFMSG_CHANGE)
	{
		m_valid = NEVER;
		m_graph.clearEvaluate();
	}
	else if (message == REFMSG_SUBANIM_STRUCTURE_CHANGED)
	{
		// If we are here, we _must_ be in between BeginEditParams
		// and EndEditParams.  Remove the current UI.
		if (m_paramMap != NULL)
		{
			if (m_pInterface != NULL) m_pInterface->DeleteRollupPage(m_paramMap->GetHWnd());
			else if (m_pMtlInterface != NULL) m_pMtlInterface->DeleteRollupPage(m_paramMap->GetHWnd());
			else { DbgAssert(0); } // shouldn't be possible
			m_paramMap = NULL;				// This has been deleted now

			// Do we need to repost our UI?
			if (m_pInterface != NULL && GeneratePBlockUI())
			{
				m_paramMap = CreateCPParamMap2(m_pblock, m_pInterface, hInstance, (DLGTEMPLATE*)m_dialogTemplate->GetDLGTEMPLATE(), _M("The Dynamic Params"), 0);
			}
			if (m_pMtlInterface != NULL)
			{
				// TODO
			}
		}

	}

	return REF_SUCCEED;
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::RefDeleted() {
	// If we have no nodes referencing this class, then kill our UI
	if (GetKLEditor() != nullptr)
	{
		ULONG handle = 0;
		NotifyDependents(FOREVER, (PartID)&handle, REFMSG_GET_NODE_HANDLE);
		if (handle == 0)
			CloseKLEditor();
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

#define KL_CODE_NAME_CHUNK		0x20
#define KL_CODE_OUTPORT_CHUNK	0x21
#define KL_CODE_SOURCE_CHUNK	0x22

// Save our local parameters
template<typename TBaseClass, typename TResultType>
IOResult SpliceTranslationLayer<TBaseClass, TResultType>::Save( ISave *isave )
{	
	FabricSplice::PersistenceInfo info;
    info.hostAppName = FabricCore::Variant::CreateString("Splice 3dsmax");
    info.hostAppVersion = FabricCore::Variant::CreateString("2014");
	info.filePath = FabricCore::Variant::CreateString(WStr(isave->FileName()).ToCStr());

	// Save out all the data needed to recreate parameters
	isave->BeginChunk(PARAM_SPLICE_DATA);
	std::string data = m_graph.getPersistenceDataJSON(&info);
	isave->WriteCString(data.data());
	isave->EndChunk();

	// Save the value port.
	isave->BeginChunk(PARAM_VALUE_NAME);
	const char* outName = m_valuePort.getName();
	isave->WriteCString(outName);
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
				FabricSplice::PersistenceInfo info;
				info.hostAppName = FabricCore::Variant::CreateString("Splice 3dsmax");
				info.hostAppVersion = FabricCore::Variant::CreateString("2014");
				info.filePath = FabricCore::Variant::CreateString(WStr(iload->FileName()).ToCStr());

				// First, read the size of the string.
				char *buff = nullptr;
				iload->ReadCStringChunk(&buff);
				m_graph.setFromPersistenceDataJSON(buff, &info);
				break;
			}
		case PARAM_VALUE_NAME:
			{
				// First, read the size of the string.
				char *buff = nullptr;
				iload->ReadCStringChunk(&buff);
				outName = buff;
				break;
			}
		}

		iload->CloseChunk();
	}

	iload->RegisterPostLoadCallback(new RenameMaxParamsCallback(this));
	// Reset/reconnect our time/parent ports
	ResetPorts();
	// Get our output port
	if (!outName.empty())
		m_valuePort = m_graph.getDGPort(outName.data());

	return IO_OK;
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::ReconnectPostLoad()
{
	for (int i = 0; i < GetPortCount(); i++) {
		int pid = GetPortParamID(i);
		if (pid >= 0)
		{
			MSTR str = MSTR::FromACP(GetPortName(i));
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
int SpliceTranslationLayer<TBaseClass, TResultType>::GetOperatorCount()
{
	return m_graph.getKLOperatorCount();
}

template<typename TBaseClass, typename TResultType>
std::string SpliceTranslationLayer<TBaseClass, TResultType>::GetOperatorName(int i)
{
	return m_graph.getKLOperatorName(i);
}

template<typename TBaseClass, typename TResultType>
std::string SpliceTranslationLayer<TBaseClass, TResultType>::GetKLCode()
{
	MAXSPLICE_CATCH_BEGIN()

	std::string cName = GetKLOperatorName();
	if (m_graph.getKLOperatorCount() > 0)
	{
		// If we have a valid graph, return code
		// for our classes operator
		return m_graph.getKLOperatorSourceCode(cName.data());
	}

	// generate default operator code.

	// If we are generating sample code, ensure that we 
	// have all the necessary ports initialized
	// Otherwise they won't be included in the sample code
	if (strcmp(GetOutPortName(), "") == 0)
	{
		std::string cOutPortName = cName + "Val";
		SetOutPortName(cOutPortName.data());
	}

	// Generate sample operator code and return
	FabricCore::Variant generatedCode = m_graph.generateKLOperatorSourceCode(cName.data());
	return generatedCode.getString_cstr();

	MAXSPLICE_CATCH_RETURN(std::string())
}

template<typename TBaseClass, typename TResultType>
std::string SpliceTranslationLayer<TBaseClass, TResultType>::GetKLOperatorName()
{
	MAXSPLICE_CATCH_BEGIN()

	if (m_graph.getKLOperatorCount() > 0)
	{
		// If we have a graph, get default name (TODO: Explicitly find
		// our operators name (can we do that?)
		return m_graph.getKLOperatorName();
	}

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
		// Are we renaming?  In that case, remove the old operator
		if (!m_operator.empty() && m_operator != entry)
		{
			m_graph.removeKLOperator(m_operator.c_str());
		}

		// If this named operator exists on the graph, update it.
		if (m_graph.hasKLOperator(entry.c_str()))
		{
			success = m_graph.setKLOperatorSourceCode(entry.c_str(), script.c_str());
		}
		else // create a new operator
		{
			success = m_graph.constructKLOperator(entry.c_str(), script.c_str());
		}
		if (success)
		{
			m_operator = entry;
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

	MAXSPLICE_CATCH_END();
	return doGather.GetGatheredResults();
}

//////////////////////////////////////////////////////////////////////////
// Add Ports
template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::AddInputPort(const char* name, const char* spliceType, int maxType/* =-1 */, bool isArray/*=false*/, const char* inExtension)
{
	HoldActions hold(_M("Add Input Port"));

	if (theHold.Holding())
		theHold.Put(new SplicePortChangeObject(this));
	FabricSplice::DGPort aPort = AddSpliceParameter(m_graph, spliceType, name, FabricSplice::Port_Mode_IN, isArray, inExtension);
	// Can/Should we create a matching max type for this?
	return SetMaxConnectedType(aPort, maxType);
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::AddOutputPort(const char* name, const char* spliceType, bool isArray/*=false*/, const char* inExtension)
{
	HoldActions hold(_M("Add Output Port"));
	if (theHold.Holding())
		theHold.Put(new SplicePortChangeObject(this));
	// Create the port
	FabricSplice::DGPort aPort = AddSpliceParameter(m_graph, spliceType, name, FabricSplice::Port_Mode_OUT, isArray, inExtension);
	return -1;
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::AddIOPort(const char* name, const char* spliceType, int maxType/* =-1 */, bool isArray/*=false*/, const char* inExtension)
{
	HoldActions hold(_M("Add IO Port"));
	if (theHold.Holding())
		theHold.Put(new SplicePortChangeObject(this));

	// Create the port
	FabricSplice::DGPort aPort = AddSpliceParameter(m_graph, spliceType, name, FabricSplice::Port_Mode_IO, isArray, inExtension);

	// Can/Should we create a matching max type for this?
	return SetMaxConnectedType(aPort, maxType);
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::RemovePort(int i)
{
	if (i < 0 || i >= GetPortCount())
		return false;

	MAXSPLICE_CATCH_BEGIN()

	HoldActions hold(_M("Remove Port"));
	if (theHold.Holding())
		theHold.Put(new SplicePortChangeObject(this));

	// Ensure the max data is released
	int pid = GetPortParamID(i);
	if (pid >= 0)
		DeleteMaxParameter((ParamID)pid);

	// Release Splice data.
	m_graph.removeDGPort(GetPortName(i));

	MAXSPLICE_CATCH_RETURN(false);
	return true;
}


template<typename TBaseClass, typename TResultType>
const char* SpliceTranslationLayer<TBaseClass, TResultType>::GetPortName( int i )
{
	if (i < 0 || i >= GetPortCount() )
		return "ERROR: OOR Index on GetPortName";
	FabricSplice::DGPort aPort = m_graph.getDGPort(i);
	return ::GetPortName(aPort);
}


template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::SetPortName(int i, const char* name) 
{
	MAXSPLICE_CATCH_BEGIN()

	if (i < 0 || i >= GetPortCount())
		return false;

	FabricSplice::DGPort aPort = m_graph.getDGPort(i);
	const char* curname = aPort.getName();
	if (strcmp(curname, name) == 0)
		return true;

	HoldActions hold(_M("Set Port Name"));
	if (theHold.Holding())
		theHold.Put(new SplicePortChangeObject(this));

	// For now, just reset the port by deleting/recreating it.
	// This doesn't recreate any data, just the port connecting to it
	FabricSplice::Port_Mode mode = aPort.getMode();
	const char* member = aPort.getMember();
	// Add new port
	FabricSplice::DGPort renamedPort = m_graph.addDGPort(name, member, mode);

	std::string allValues = m_graph.getPersistenceDataJSON();

	// 
	const char* portType = ::GetPortType(renamedPort);

	// Clone option data from the old port
	FabricCore::Variant allData = m_graph.getPersistenceDataDict();
	const FabricCore::Variant* portData = allData.getDictValue("ports");
	DbgAssert(portData != 0);
	if (portData != nullptr) 
	{
		DbgAssert(portData->isArray());
		int numPorts = portData->getArraySize();
		for (int i = 0; i < numPorts; i++)
		{
			// We have an array of ports.  Iterate over it to 
			// find the element describing curname
			const FabricCore::Variant* portVal = portData->getArrayElement(i);
			if (portVal != nullptr) 
			{
				// Is this the port we are looking for?
				const FabricCore::Variant* namevar = portVal->getDictValue("name");
				if (namevar == nullptr || strcmp(namevar->getString_cstr(), curname) != 0)
					continue;

				// Double check, we might have 2 ports named the same
				// (if they have the same type, well... nothing we can do)
				const FabricCore::Variant* typevar = portVal->getDictValue("type");
				if (typevar == nullptr || strcmp(portType, typevar->getString_cstr()) != 0)
					continue;

				// This is the right element - copy over the options
				const FabricCore::Variant* options = portVal->getDictValue("options");
				if (options != nullptr) 
				{
					FabricCore::Variant::DictIter iter(*options);
					while (!iter.isDone())
					{
						const char* key = iter.getKey()->getString_cstr();
						const FabricCore::Variant* val = iter.getValue();
						renamedPort.setOption(key, *val);
						iter.next();
					}
				}
				break;
			}
		}
	}

	// We are done with old port, remove it.
	m_graph.removeDGPort(curname);
	
	// Now, rename the max parameter
	int paramID = ::GetPortParamID(renamedPort);
	if (paramID >= 0)
	{
		MSTR str = MSTR::FromACP(name);
		MaybeRemoveParamUI();
		SetMaxParamName(m_pblock->GetDesc(), (ParamID)paramID, str);
		// Delete existing autogen UI
		SAFE_DELETE(m_dialogTemplate);
		MaybePostParamUI();
	}

	// Verify everything was hooked back  up.
	renamedPort = m_graph.getDGPort(name);
	return renamedPort.isValid();

	MAXSPLICE_CATCH_RETURN(false);
}

template<typename TBaseClass, typename TResultType>
const char* SpliceTranslationLayer<TBaseClass, TResultType>::GetPortType( int i )
{
	if (i < 0 || i >= GetPortCount())
		return "ERROR: OOR index on GetPortType";
	FabricSplice::DGPort aPort = m_graph.getDGPort(i);
	return ::GetPortType(aPort);
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::SetOutPortName(const char* name)
{
	MAXSPLICE_CATCH_BEGIN()

	// Do not set an empty name (fabric will throw)
	if (strcmp(name, "") == 0)
		return false;

	if (m_valuePort.isValid())
	{
		std::string oldName = m_valuePort.getName();
		// We have a valid value already, do we need to change it?
		if (strcmp(oldName.data(), name) == 0)
			return true;

		if (theHold.Holding())
			theHold.Put(new SplicePortChangeObject(this));

		// We cannot rename, we must remove original and recreate
		
		m_graph.removeDGPort(oldName.data());
		m_graph.removeDGNodeMember(oldName.data());
	}
	// Create the connection to Fabric
	m_valuePort = AddSpliceParameter(m_graph, GetValueType(), name, FabricSplice::Port_Mode_IO, GetOutPortArrayIdx() >= 0);
	return m_valuePort.isValid();
	MAXSPLICE_CATCH_RETURN(false);
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::GetOutPortArrayIdx()
{ 
	return m_valuePortIndex;
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::SetOutPortArrayIdx(int index)
{
	MAXSPLICE_CATCH_BEGIN()

	// If we have created a port, we may need to change its type
	if (m_valuePort.isValid())
	{
		// If we have been set to an invalid index, disable our array-ness
		if (index < 0 && m_valuePort.isArray())
		{
			m_graph.removeDGPort(GetOutPortName());
			m_valuePort = AddSpliceParameter(m_graph, GetValueType(), GetOutPortName(), FabricSplice::Port_Mode_OUT, false);
		}
		// else if we are not an array, and an index has been requested, then... make it so
		else if (index >= 0 && !m_valuePort.isArray())
		{		
			m_graph.removeDGPort(GetOutPortName());
			m_graph.removeDGNodeMember(GetOutPortName());
			m_valuePort = AddSpliceParameter(m_graph, GetValueType(), GetOutPortName(), FabricSplice::Port_Mode_OUT, true);
		}

		// Ensure that the array is large enough
		if (index >= 0 && m_valuePort.getArrayCount() <= (UINT)index)
		{
			FabricCore::RTVal outValArray = m_valuePort.getRTVal();
			outValArray.setArraySize(index + 1);
		}
	}
	m_valuePortIndex = index;

	MAXSPLICE_CATCH_END()
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::GetPortParamID(int i) 
{
	MAXSPLICE_CATCH_BEGIN()
	if (i >= 0 && i < GetPortCount())
	{
		FabricSplice::DGPort aPort = m_graph.getDGPort(i);
		return ::GetPortParamID(aPort);
	}
	return -1;
	MAXSPLICE_CATCH_RETURN(-1)
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::SetPortParamID(int i, ParamID id) 
{
	MAXSPLICE_CATCH_BEGIN()
	if (i >= 0 && i < GetPortCount())
	{
		FabricSplice::DGPort aPort = m_graph.getDGPort(i);
		SetPortParamID(aPort, id);
	}
	MAXSPLICE_CATCH_END()
}

//////////////////////////////////////////////////////////////////////////
template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::ConnectPort( const char* myPortName, ReferenceTarget* pSrcContainer, const char* srcPortName, int srcPortIndex )
{
	if (pSrcContainer == nullptr)
		return false;

	SpliceTranslationFPInterface* pSrcContInterface = static_cast<SpliceTranslationFPInterface*>(pSrcContainer->GetInterface(ISPLICE__INTERFACE));
	if (pSrcContInterface == nullptr)
	 return false;

	MAXSPLICE_CATCH_BEGIN()

	// First, do our ports exist?
	FabricSplice::DGPort srcPort = pSrcContInterface->GetPort(srcPortName);
	if (!srcPort || srcPort.getMode() == FabricSplice::Port_Mode_IN)
		return false;
	FabricSplice::DGPort destPort = GetPort(myPortName);
	if (!destPort || destPort.getMode() != FabricSplice::Port_Mode_IN)
		return false;

	// Are they of the same type?
	if (strcmp(srcPort.getDataType(), destPort.getDataType()) != 0)
		return false;

	// Are we connecting to an array, and is that expected?
	if ((srcPortIndex >= 0) && !srcPort.isArray())
		return false;

	if (theHold.Holding())
		theHold.Put(new SplicePortChangeObject(this));

	// Ok - these ports are good to go.  Connect 'em up.
	int res = SetMaxConnectedType(destPort, TYPE_REFTARG);
	if (res < 0)
		return false;
	m_pblock->SetValue((ParamID)res, 0, pSrcContainer);
	SetPortConnection(destPort, srcPortName);
	SetPortConnectionIndex(destPort, srcPortIndex);

	// Ensure there are enough indices in the RTVal array
	if (srcPortIndex >= 0) 
	{
		FabricCore::RTVal rtVal = srcPort.getRTVal();
		if (rtVal.getArraySize() <= (uint32_t)srcPortIndex)
			rtVal.setArraySize(srcPortIndex + 1);
	}

	MAXSPLICE_CATCH_RETURN(false)

	return true;
}


template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::DisconnectPort(const char* myPortName)
{
	FabricSplice::DGPort connectedPort = GetPort(myPortName);
	if (connectedPort) 
	{
		std::string connection = GetPortConnection(connectedPort);
		if (connection == "") 
		{
			if (theHold.Holding())
				theHold.Put(new SplicePortChangeObject(this));

			SetMaxConnectedType(connectedPort, -1);
			connectedPort.setOption(MAX_SRC_OPT, FabricCore::Variant());
			return true;
		}
	}
	return false;
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::GetMaxConnectedType( int i )
{
	int pid = GetPortParamID(i);
	if (pid >= 0)
	{
		return m_pblock->GetParameterType((ParamID)pid);
	}
	return -1;
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::SetMaxConnectedType(FabricSplice::DGPort& aPort, int maxType)
{
	if (!aPort.isValid())
		return -1;

	// The user has requested the default type (-2)
	const char* spliceType = ::GetPortType(aPort);
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
		BitArray legalTypes = ::GetLegalMaxTypes(spliceType);
		// The requested max type is not legal for this splice type
		if (!legalTypes[maxType])
			maxType = SpliceTypeToDefaultMaxType(spliceType); // Reset to default
	}

	// -1 means no max connection, remove one if exists
	int pid = ::GetPortParamID(aPort);
	if (maxType == -1) 
	{
		if (pid >= 0)
			DeleteMaxParameter((ParamID)pid);
		return -1;
	}

	// Do we have an existing Max parameter?
	if (pid >= 0)
	{
		// Check that its not the correct type already
		if (m_pblock->GetParamDef((ParamID)pid).type == maxType)
			return pid;

		// Wrong type, remove original
		DeleteMaxParameter((ParamID)pid);
	}

	// If this is a legal type for this parameter, then go ahead
	ParamBlockDesc2* pNewDesc = CopyPBDescriptor();
	ParamID newId = AddMaxParameter(pNewDesc, maxType, ::GetPortName(aPort));
	CreateParamBlock(pNewDesc);
	::SetPortParamID(aPort, newId);
	return newId;
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::SetMaxConnectedType(int i, int maxType)
{
	if (i >= 0 && i < GetPortCount())
	{
		FabricSplice::DGPort aPort = m_graph.getDGPort(i);
		return SetMaxConnectedType(aPort, maxType);
	}
	return -1;
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::SetSpliceGraph(const FabricSplice::DGGraph& graph, bool createMaxParams) 
{ 
	// First, clear any existing parameters
	ReplaceReference(0, nullptr);

	// Reset all variables.
	m_graph = graph; 

	// Invalid current value
	m_valid = NEVER;

	ResetPorts();

	// Early exit
	if (!m_graph.isValid())
		return;

	// Try to find an output value for our class
	bool foundOutPort = false;
	int nPorts = m_graph.getDGPortCount();
	for (int i = 0; i < nPorts; i++)
	{
		std::string portName = m_graph.getDGPortName(i);
		FabricSplice::DGPort port = m_graph.getDGPort(i);
		if(!port.isValid())
			continue;

		FabricSplice::Port_Mode portMode = port.getMode();

		if (portMode != FabricSplice::Port_Mode_IN)
		{
			if (!foundOutPort)
			{
				const char* sDataType = port.getDataType();
				int type = SpliceTypeToMaxType(sDataType);
				// Is this legal for us?  If so, connect our value port
				if (type >= 0)
				{
					SetOutPort(port);
					bool isArray = port.isArray();
					if (isArray)
						SetOutPortArrayIdx(0);
					foundOutPort = true;
				}
			}
		}
		else if (createMaxParams)
		{
			// We have been requested to create Max parameters for all possible ports.
			SetMaxConnectedType(i, -2); // Create default type
		}
	}

	if (!foundOutPort)
		m_valuePort = FabricSplice::DGPort();
};

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::LoadFromFile(const MCHAR* filename, bool createMaxParams) 
{ 
	CStr cFilename = CStr::FromMCHAR(filename);
	FabricSplice::DGGraph graph("newGraph");
	bool res = graph.loadFromFile(cFilename);
	if (!res)
		return false;

	SetSpliceGraph(graph, true);
	return true;
};

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::SaveToFile(const MCHAR* filename) 
{ 
	if (!m_graph.isValid())
		return false;

	CStr cFile = CStr::FromMCHAR(filename);
	return m_graph.saveToFile(cFile.data());
};

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::ResetPorts()
{
	// Setup any necessary ports for the current graph
	m_valuePort = FabricSplice::DGPort();
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
#pragma region Evaluation

template<typename TBaseClass, typename TResultType>
const TResultType& SpliceTranslationLayer<TBaseClass, TResultType>::Evaluate(TimeValue t, Interval& ivValid)
{
	if (!m_graph.isValid())
		return m_value;

	// If our value is valid, do not re-evaluate
	if (!m_valid.InInterval(t))
	{
		MAXSPLICE_CATCH_BEGIN();

			// setup the context
			FabricCore::RTVal evalContext = m_graph.getEvalContext();
			evalContext.setMember("time", FabricSplice::constructFloat32RTVal(TicksToSec(t)));

			// Reset our internal validity times;
			m_valid.SetInfinite();
			// Set  all Max values on their splice equivalents
			SetAllMaxValuesToSplice(t, m_pblock, m_graph, m_valid);

			// Trigger graph evaluation
			m_graph.evaluate();

			// Get our value back!
			SpliceToMaxValue(m_valuePort, m_value, GetOutPortArrayIdx());

			// TODO: Until we have a reliable way to 
			// detect if the operator uses the time
			// port, our validity can only be the current frame
			m_valid.SetInstant(t);
		
		MAXSPLICE_CATCH_END();

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