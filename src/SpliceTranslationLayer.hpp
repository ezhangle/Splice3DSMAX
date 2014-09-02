#pragma once // hpp files are actually headers

#include "SpliceTranslationLayer.h"
#include "DynamicParamBlocks/DynPBCustAttrClassDesc.h"
#include "DynamicParamBlocks/DynamicDialog/DynamicDialogTemplate.h"
#include "DynamicParamBlocks/DynPBUndo.h"
#include "SpliceLogging.h"
#include "MaxConversionFns.h"

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
	// Delete the current operator(s).
	m_graph.clear();

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
		// create a graph to hold our dependency graph nodes.
		m_graph = FabricSplice::DGGraph("myGraph");
		m_graph.constructDGNode();
		m_graph.setUserPointer(this);

		ResetPorts();
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

	// Ensure that our max->splice connection is broken for this parameter
	DbgAssert(false && "Check This");

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

//template<typename TBaseClass, typename TResultType>
//ParamID SpliceTranslationLayer<TBaseClass, TResultType>::SetNewParameter( ParamType2 type, const TCHAR* name )
//{
//	// Every time we add a parameter, we create an entirely new ParamBlockDesc
//	ParamBlockDesc2* pNewDesc = CopyPBDescriptor();
//	DbgAssert(pNewDesc != NULL);
//
//	// Add the parameter to the descriptor
//	ParamID pid = AddMaxParameter(pNewDesc, type);
//
//	// Now we have an ID for the block, map the name in our local string cache
//	//NameData& paramData = AddParamData(pid, name);
//	ParamDef& pDef = pNewDesc->GetParamDef(pid);
//	pDef.int_name = _tcsdup(name); // retrieves our locally allocated string
//
//	// Before changing any references, begin holding.
//	theHold.Begin();
//
//	CStr cName = CStr::FromMCHAR(name);
//	// Create a matching Splice port to send the data too
//	// TODO: Manage Undo/Redo
//#pragma message("NOTE: Undo needed to be implemented here")
//	FabricSplice::DGPort port = AddSpliceParameter(m_graph, type, cName, FabricSplice::Port_Mode_IN);
//	ConnData newConnection(pid, port);
//	m_dParamData.push_back(newConnection);
//
//	// Create a new parameter block, cloning the values of our current parameter block
//	CreateParamBlock(pNewDesc);
//
//	// If we are displayed in the UI, it will hold references to our
//	// original parameter block.  Sending this message forces a complete refresh.
//	NotifyDependents(FOREVER, 0, REFMSG_SUBANIM_STRUCTURE_CHANGED);
//
//	// We are done!  Tell the undo system to wrap all the changes up
//	// in a simple single action labeled something intelligent!
//	theHold.Accept(_M("CustAttr PB Change"));
//
//	return pid;
//}
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
void SpliceTranslationLayer<TBaseClass, TResultType>::BeginEditParams(IObjParam *ip, ULONG UNUSED(flags), Animatable *UNUSED(prev))
{
	this->m_pInterface = ip;
	m_hPanel = ip->AddRollupPage(
		hInstance,
		MAKEINTRESOURCE(IDD_PARAM_GENERATION_UI),
		DlgProc,
		GetString(IDS_PARAMS),
		(LPARAM)this);

	// Generate our Dialog, and assign an mParamMap
	if (GeneratePBlockUI())
	{
		m_paramMap = CreateCPParamMap2(m_pblock, ip, hInstance, (DLGTEMPLATE*)m_dialogTemplate->GetDLGTEMPLATE(), _M("The Dynamic Params"), 0);
	}
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::EndEditParams(IObjParam *ip, ULONG UNUSED(flags), Animatable *UNUSED(next))
{
	ip->DeleteRollupPage(m_hPanel);

	if (m_paramMap != NULL)
	{
		ip->DeleteRollupPage(m_paramMap->GetHWnd());
		m_paramMap = NULL;
	}
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
		(LPARAM)this);

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
			if (m_paramMap != NULL)
			{
				if (m_pInterface != NULL) m_pInterface->DeleteRollupPage(m_paramMap->GetHWnd());
				else if (m_pMtlInterface != NULL) m_pMtlInterface->DeleteRollupPage(m_paramMap->GetHWnd());
				else { DbgAssert(0); } // shouldn't be possible
				m_paramMap = NULL;				// This has been deleted now
			}

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

void WriteString(ISave *isave, const char* s, USHORT chunkID)
{

}

// Save our local parameters
template<typename TBaseClass, typename TResultType>
IOResult SpliceTranslationLayer<TBaseClass, TResultType>::Save( ISave *isave )
{	
	// Save out all the data needed to recreate parameters
	isave->BeginChunk(PARAM_SPLICE_DATA);
	std::string data = m_graph.getPersistenceDataJSON();
	isave->WriteCString(data.data());
	isave->EndChunk();

	// Save out the information on which parameters are hooked up to what in the Max UI
	//int nInParams = GetNumPorts();
	//for( size_t i=0; i < nInParams; ++i )
	//{
	//	isave->BeginChunk(PARAM_DATA_CHUNK);
	//	//ConnData& connection = m_dParamData[i];

	//	isave->BeginChunk(PARAM_NAME_CHUNK);
	//	isave->WriteCString(GetPortName(i));
	//	isave->EndChunk();

	//	//isave->BeginChunk(PARAM_SPLICE_TYPE_CHUNK);
	//	//isave->WriteCString(connection.second.getDataType());
	//	//isave->EndChunk();

	//	ParamID pid = connection.first;
	//	isave->BeginChunk(PARAM_MAX_DATA_CHUNK);
	//	isave->Write(&pid, sizeof(pid), &dummy);
	//	isave->EndChunk();

	//	isave->EndChunk();
	//}
	
	// Save the value port.
	isave->BeginChunk(PARAM_VALUE_NAME);
	const char* outName = m_valuePort.getName();
	isave->WriteCString(outName);
	isave->EndChunk();


	//// Save out operator data.
	//int nOperators = m_graph.getKLOperatorCount();
	//for (int i = 0; i < nOperators; i++)
	//{
	//	isave->BeginChunk(KL_CODE_CHUNK);
	//	
	//	isave->BeginChunk(KL_CODE_NAME_CHUNK);
	//	isave->WriteCString(m_graph.getKLOperatorName(i));
	//	isave->EndChunk();

	//	isave->BeginChunk(KL_CODE_OUTPORT_CHUNK);
	//	isave->WriteCString(GetOutPortName());
	//	isave->EndChunk();

	//	isave->BeginChunk(KL_CODE_SOURCE_CHUNK);
	//	isave->WriteCString(GetKLCode().data());
	//	isave->EndChunk();
	//	
	//	isave->EndChunk();
	//}
	return IO_OK;
}

//// PLCB hooks up the names from Splice->Max
//class SpliceLayerNamePLCB : public PostLoadCallback
//{
//private:
////	std::vector<ConnData>& m_dConnData;
//	ReferenceMaker* m_pOwner;
//
//public:
//	SpliceLayerNamePLCB(ReferenceTarget* owner);
//	virtual void proc( ILoad *iload );
//};
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
				iload->ReadCStringChunk(&buff);
				m_graph.setFromPersistenceDataJSON(buff);
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
		//case PARAM_DATA_CHUNK:
		//	{
		//		// Load our splice parameters.  Don't worry about
		//		// Max parameters, they are handled by our ClassDesc2
		//		std::string paramName;
		//		std::string paramType;
		//		std::string extension;
		//		ParamID pid = -1;
		//		while( IO_OK == (result = iload->OpenChunk()) )
		//		{
		//			switch (iload->CurChunkID())
		//			{
		//			case PARAM_NAME_CHUNK:
		//				iload->ReadCStringChunk(&temp);
		//				paramName = temp;
		//				break;
		//			//case PARAM_SPLICE_TYPE_CHUNK:
		//			//	iload->ReadCStringChunk(&temp);
		//			//	paramType = temp;
		//			//	break;
		//			case PARAM_MAX_DATA_CHUNK:
		//				{
		//					ULONG pidSize = sizeof(pid);
		//					iload->Read(&pid, pidSize, &pidSize);
		//				}
		//				break;
		//			}
		//			iload->CloseChunk();
		//		}

		//		if (!paramName.empty())
		//		{
		//			
		//			FabricSplice::DGPort aNode = m_graph.getDGPort(paramName.data()); // AddSpliceParameter(m_graph, paramType.data(), paramName.data(), FabricSplice::Port_Mode_IN);
		//			m_dParamData.push_back(ConnData(pid, aNode));
		//		}
		//	}
		//	break;
		/*case KL_CODE_CHUNK:
			{
				std::string opName;
				std::string klCode;
				while (IO_OK == (result = iload->OpenChunk()))
				{
					switch (iload->CurChunkID())
					{
					case KL_CODE_NAME_CHUNK:
						iload->ReadCStringChunk(&temp);
						opName = temp;
						break;
					case KL_CODE_OUTPORT_CHUNK:
						iload->ReadCStringChunk(&temp);
						SetOutPortName(temp);
						break;
					case KL_CODE_SOURCE_CHUNK:
						iload->ReadCStringChunk(&temp);
						klCode = temp;
						break;
					}
					iload->CloseChunk();
				}

				if (!opName.empty() && !klCode.empty())
				{
					SetKLCode(opName, klCode);
				}
			}*/
		}

		iload->CloseChunk();
	}
	// We should always have a time port.
	m_timePort = m_graph.getDGPort("time");
	// We should have named an out port
	if (!outName.empty())
		m_valuePort = m_graph.getDGPort(outName.data());

	// PLCB hooks up our parameter block to the max ports
	//iload->RegisterPostLoadCallback(new SpliceLayerNamePLCB(this));
	return IO_OK;
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
}

template<typename TBaseClass, typename TResultType>
std::string SpliceTranslationLayer<TBaseClass, TResultType>::GetKLOperatorName()
{ 
	if (m_graph.getKLOperatorCount() > 0)
	{
		// If we have a graph, get default name (TODO: Explicitly find
		// our operators name (can we do that?)
		return m_graph.getKLOperatorName();
	}

	// By default, we use the internal name as our operator name (The MaxScript name)
	std::string cDefName = CStr::FromMCHAR(GetClassDesc()->InternalName());
	return cDefName;
}

template<typename TBaseClass, typename TResultType>
std::string SpliceTranslationLayer<TBaseClass, TResultType>::SetKLCode(const std::string& entry, const std::string& script)
{
	if (entry.empty())
		return "ERROR: No entry point specified";// TODO: Globalize this
	if (script.empty())
		return "ERROR: Cannot cannot compile empty script";// TODO: Globalize this

	GatherCompilerResults doGather;
	if (!m_KLFile.empty())
	{
		// Warn that the link between file and this operator is breaking
		doGather.LogSomething("WARNING: Manually setting code of file-linked class");// TODO: Globalize this
		m_KLFile.clear();
	}

	// Try-catch
	try {
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
			doGather.LogSomething("OK");// TODO: Globalize this
		}
		else
		{
			// Theoretically, we should never actually reach this code.  Fabric will throw if somethins outta whack
			doGather.LogSomething("Unkown Error");
		}
	}
	catch(FabricSplice::Exception e)
	{
		doGather.LogSomething(e.what());
	}
	return doGather.GetGatheredResults();
}

template<typename TBaseClass, typename TResultType>
std::string SpliceTranslationLayer<TBaseClass, TResultType>::SetKLFile( const char* filename )
{
	// Dont even attempt to load an empty string
	if (filename != NULL)
	{
		bool res = m_graph.loadFromFile(filename);
		if (res)
		{
			// Invalid current value
			m_valid = NEVER;

			// Clear existing ports?
			// On successful load, we want to hook up our ports
			UINT nPorts = m_graph.getDGPortCount();

			// Create the default new descriptor
			ParamBlockDesc2* pNewDesc = NULL;
			for (UINT i = 0; i < nPorts; i++)
			{
				FabricSplice::DGPort aPort = m_graph.getDGPort(i);

				FabricSplice::Port_Mode mode = aPort.getMode();
				int type = SpliceTypeToMaxType(aPort.getDataType());
				if (mode != FabricSplice::Port_Mode_IN)
				{
					// Is this our in-port?
					if (type == GetValueType())
						m_valuePort = aPort;
				}
				else
				{
					if (pNewDesc == NULL)
						pNewDesc = GetClassDesc()->CreatePBDesc();
					
					// Add a new parameter to our pblock
					ParamID pid = AddMaxParameter(pNewDesc, type, aPort.getName());
					
					// Store the max connection
					aPort.setOption(MAX_PID_OPT, GetVariant(pid));
				}
			}
			// reset our parameters to whats currently being used.
			CreateParamBlock(pNewDesc);
			m_KLFile = filename;
		}
	}
	return "OK";
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::CreatePort(const char* name, const char* spliceType, int maxType/* =-1 */, bool isArray/*=false*/, const char* inExtension)
{
	// The user has requested the default type (-2)
	if (maxType == -2)
		maxType = SpliceTypeToMaxType(spliceType);
	else if (maxType >= 0)
	{
		// Figure out what kind of parameter
		// we can/will create on the max side
		BitArray legalTypes = ::GetLegalMaxTypes(spliceType);
		// The requested max type is not legal for this splice type
		if (!legalTypes[maxType])
			maxType = SpliceTypeToMaxType(spliceType); // Reset to default
	}

	// Create the port
	FabricSplice::DGPort aPort = AddSpliceParameter(m_graph, spliceType, name, FabricSplice::Port_Mode_IN, isArray, inExtension);

	// Can/Should we create a matching max type for this?
	ParamID pid = -1; 
	if (maxType >= 0) 
	{
		if (isArray)
			maxType = maxType|TYPE_TAB;
		// Add a new parameter to our pblock
		ParamBlockDesc2* pNewDesc = CopyPBDescriptor();
		pid = AddMaxParameter(pNewDesc, maxType, aPort.getName());

		// Create new parameter block
		CreateParamBlock(pNewDesc);
	}

	// Store connection between port->paramblock
	aPort.setOption(MAX_PID_OPT, GetVariant(pid));

	return pid;
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::DeletePort(int i)
{
	if (i < 0 || i < GetNumPorts())
		return false;

	// Ensure the max data is released
	int pid = GetPortParamID(i);
	if (pid >= 0)
		DeleteMaxParameter((ParamID)pid);

	// Release Splice data.
	m_graph.removeDGPort(GetPortName(i));
	return true;
}

template<typename TBaseClass, typename TResultType>
bool SpliceTranslationLayer<TBaseClass, TResultType>::SetOutPortName(const char* name)
{ 
	// Do not set an empty name (fabric will throw)
	if (strcmp(name, "") == 0)
		return false;

	if (m_valuePort.isValid())
	{
		std::string oldName = m_valuePort.getName();
		// We have a valid value already, do we need to change it?
		if (strcmp(oldName.data(), name) == 0)
			return true;

		// We cannot rename, we must remove original and recreate
		
		m_graph.removeDGPort(oldName.data());
		m_graph.removeDGNodeMember(oldName.data());
	}
	// Create the connection to Fabric
	m_valuePort = AddSpliceParameter(m_graph, GetValueType(), name, FabricSplice::Port_Mode_IO, GetOutPortArrayIdx() >= 0);
	return m_valuePort.isValid();
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::GetOutPortArrayIdx()
{ 
	return m_valuePortIndex;
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::SetOutPortArrayIdx(int index)
{
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
}

template<typename TBaseClass, typename TResultType>
int SpliceTranslationLayer<TBaseClass, TResultType>::GetPortParamID(int i) 
{
	if (i >= 0 && i < GetNumPorts())
	{
		FabricSplice::DGPort aPort = m_graph.getDGPort(i);
		FabricCore::Variant option = aPort.getOption(MAX_PID_OPT);
		if(option.isSInt32())
			return (int)option.getSInt32();
		//return aPort.getIntegerOption(MAX_PID_OPT);
	}
	return -1;
}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::SetPortParamID(int i, ParamID id) 
{
	if (i >= 0 && i < GetNumPorts())
	{
		FabricSplice::DGPort aPort = m_graph.getDGPort(i);
		aPort.setOption(MAX_PID_OPT, GetVariant((int)id));
	}
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
int SpliceTranslationLayer<TBaseClass, TResultType>::SetMaxConnectedType(int i, int type)
{
	if (GetNumPorts() < i || i < 0)
		return -1;

	int baseType = base_type(type);
	// CHeck this is a legal request
	if (!GetLegalMaxTypes(i)[baseType])
		return 0;

	// Do we have an existing Max parameter?
	ParamID pid = (ParamID)GetPortParamID(i);
	if (pid != -1)
	{
		// Check that its not the correct type already
		if (m_pblock->GetParamDef(pid).type == type)
			return type;

		// Wrong type, remove original
		DeleteMaxParameter(pid);
	}

	// If this is a legal type for this parameter, then go ahead
	ParamBlockDesc2* pNewDesc = CopyPBDescriptor();
	ParamID newId = AddMaxParameter(pNewDesc, type, GetPortName(i));
	CreateParamBlock(pNewDesc);
	SetPortParamID(i, newId);
	return true;
}

//template<typename TBaseClass, typename TResultType>
//Tab<TSTR*> SpliceTranslationLayer<TBaseClass, TResultType>::GetSplicePorts()
//{
//	Tab<TSTR*> dPorts;
//	unsigned int cnt = m_graph.getDGPortCount();
//	dPorts.SetCount(cnt);
//	for (unsigned int i = 0; i < cnt; i++)
//	{
//		TSTR* pVal = new TSTR;
//		*pVal = MSTR::FromACP(m_graph.getDGPortName(i));
//		dPorts[i] = pVal;
//	}
//	return dPorts;
//}

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::SetSpliceGraph(const FabricSplice::DGGraph& graph) 
{ 
	// Reset all variables.
	m_graph = graph; 
	ResetPorts();
};

template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::ResetPorts()
{
	// Setup any necessary ports for the current graph
	m_valuePort = FabricSplice::DGPort();
	// Our graphs always include a time component
	m_timePort = AddSpliceParameter(m_graph, TYPE_TIMEVALUE, _M("time"), FabricSplice::Port_Mode_IN);
}

#pragma endregion

//////////////////////////////////////////////////////////////////////////
#pragma region Evaluation

template<typename TBaseClass, typename TResultType>
const TResultType& SpliceTranslationLayer<TBaseClass, TResultType>::Evaluate(TimeValue t, Interval& ivValid)
{
	if (!m_graph.isValid())
		return m_value;

	// Set time, if the input requests it.
	if (m_timePort.isValid())
	{
		MaxValueToSplice(m_timePort, 0, ivValid, TicksToSec(t));
	}

	// If our value is valid, do not re-evaluate
	if (!m_valid.InInterval(t))
	{
		try
		{
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
		}
		catch (FabricCore::Exception e) 
		{
			logMessage(e.getDescData());
		}
	}

	// Let max know how long our value is valid for.
	ivValid &= m_valid;
	return m_value;
}
template<typename TBaseClass, typename TResultType>
void SpliceTranslationLayer<TBaseClass, TResultType>::InitMixinInterface()
{
	GetDescriptor<TBaseClass, TResultType>();
}