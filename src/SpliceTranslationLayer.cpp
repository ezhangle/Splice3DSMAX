#include "StdAfx.h"
#include "MeshNormalSpec.h"

//////////////////////////////////////////////////////////////////////////
//--- DynPBDefAccessor -------------------------------------------------------
#pragma region//Class DynPBDefAccessor
/*! Class DynPBDefAccessor
	This class is the default accessor for all dynamically generated
	parameters.  This is because we want to register our objects as 
	dynamically generated names, and this class provides an implementation
	to find names from the owner class */
class IntNameAccessor : public PBAccessor
{
	MSTR GetLocalName(ReferenceMaker* owner, ParamID id, int UNUSED(tabIndex)) {
		IParamBlock2* pblock = owner->GetParamBlock(0);
		if (pblock != 0)
		{
			ParamDef& pdef = pblock->GetParamDef(id);
			return pdef.int_name;
		}
		return _M("ERROR: Missing Name");
	}
};

static IntNameAccessor theDefAccessor;
#pragma endregion

//////////////////////////////////////////////////////////////////////////
//--- Utility Functions -------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------
// Add a parameter to the given descriptor
//---------------------------------------------------------------------------------------------------------------
#pragma region//AddParameter

bool CopyValue(int type, ParamID newId, IParamBlock2* pNewBlock, ParamID oldId, IParamBlock2* pCopyBlock)
{
	int numValues = 1;
	if (is_tab(type)) 
	{
		numValues = pCopyBlock->Count(oldId);
		pNewBlock->SetCount(newId, numValues);
	}

	// Copy all values from copy block to newblock
	for (int i = 0; i < numValues; i++)
	{
		// Is this value animated?  Keep the reference if so
		Control* anim = pCopyBlock->GetControllerByID(oldId);
		if (anim != NULL) 
		{
			pNewBlock->SetControllerByID(oldId, i, anim);
		}
		else
		{
			// Constant value, copy it over.
			switch (type)
			{
			case TYPE_FLOAT:
			case TYPE_ANGLE:
			case TYPE_PCNT_FRAC:
			case TYPE_WORLD:
				pNewBlock->SetValue(newId, i, pCopyBlock->GetFloat(oldId));
				break;
			case TYPE_BOOL:
			case TYPE_INT:
				pNewBlock->SetValue(newId, i, pCopyBlock->GetInt(oldId));
				break;
			case TYPE_POINT3:
				{
					Point3 v = pCopyBlock->GetPoint3(oldId);
					pNewBlock->SetValue(newId, i, v);
				}
				break;
			case TYPE_FRGBA:
			case TYPE_POINT4:
				{
					Point4 v = pCopyBlock->GetPoint4(oldId);
					pNewBlock->SetValue(newId, i, v);
				}
				break;
			case TYPE_MTL:
				pNewBlock->SetValue(newId, i, pCopyBlock->GetMtl(oldId));
				break;
			case TYPE_TEXMAP:
				pNewBlock->SetValue(newId, i, pCopyBlock->GetTexmap(oldId));
				break;
			case TYPE_MATRIX3:
				{
					Matrix3 v = pCopyBlock->GetMatrix3(oldId);
					pNewBlock->SetValue(newId, i, v);
					break;
				}
			case TYPE_INODE:
				pNewBlock->SetValue(newId, i, pCopyBlock->GetINode(oldId));
				break;
			case TYPE_STRING:
				{
					CONST_2010 MCHAR* theString = pCopyBlock->GetStr(oldId);
					if (theString != NULL) 
						pNewBlock->SetValue(newId, i, theString);
				}
				break;
			case TYPE_RGBA:
				{
					Color c = pCopyBlock->GetColor(oldId);
					pNewBlock->SetValue(newId, i, c);
					break;
				}
			default:
				DbgAssert(FALSE); // Implement Me
				return false;
			}
		}
	}
	return true;
}

IParamBlock2* CreateParamBlock( ParamBlockDesc2* pDesc, IParamBlock2* pCopyBlock, ReferenceTarget* pOwner)
{
	IParamBlock2* pNewBlock = NULL;
	if (pDesc != NULL)
	{
		// Create the parameter block
		pNewBlock = CreateParameterBlock2( pDesc, pOwner );

		// If we have been passed a parameter block - lets scrape its value so
		// we are as close a copy of this as possible.
		if (pCopyBlock != NULL)
		{
			// iterate all our current parameters
			ParamBlockDesc2* pOrigDesc = pCopyBlock->GetDesc();
			for (int i = 0; i < pOrigDesc->Count(); i++)
			{
				// Find the param Id for this index.
				ParamID id = pDesc->IndextoID(i);

				// if the id is -ve, we have removed that 
				// parameter (and therefore have nothing to copy)
				if (id == -1)
					continue;

				// Get parameter definitions
				const ParamDef& pbNewDef = pDesc->GetParamDef(id);
				// Search parameter definitions for matching name
				for (int j = 0; j < pOrigDesc->Count(); j++)
				{
					const ParamDef& pbOrigDef = pOrigDesc->paramdefs[j];
					if ((pbOrigDef.type == pbNewDef.type) &&
						(_tcscmp(pbOrigDef.int_name, pbNewDef.int_name) == 0))
					{
						// Mathing values, copy 'em over.
						CopyValue(pbNewDef.type, id, pNewBlock, pbOrigDef.ID, pCopyBlock);
					}
				}
			}
		}
	}
	return pNewBlock;
}


ParamID AddMaxParameter( ParamBlockDesc2* pDesc, int type, const MCHAR* sName, ParamID desiredId/*=-1*/ )
{
	if (type < 0)
		return -1;

	// Add a new parameter to our new descriptor
	// All parameters have computed names and are animatable
	// so long as they are not reference types...
	int flags = P_COMPUTED_NAME;
	int baseType = base_type(type);
	if (!reftarg_type(baseType))
		flags |= P_ANIMATABLE;
	if (is_tab(type))
		flags |= P_VARIABLE_SIZE;

	// Find a free ParamID
	ParamID pid = desiredId;
	if (pid == -1)
		pid = pDesc->Count();
	while (pDesc->IDtoIndex(pid) != -1)
		pid++;

	// If our requested index is not in range, disable it
	//if (index >= pid)
	//	index = -1;

	// If we have a requested index, and there is no matching ID,
	// we assign ID == index.
	//if (index >= 0 && pDesc->IDtoIndex((ParamID)index) < 0)
	//	pid = (ParamID)index;

	// Create the parameter.  Every parameter requires an
	// accessor, so set it now.
	if (is_tab(type))
	{
		pDesc->AddParam(pid, NULL, type, 0, flags, 0, 
			p_accessor, &theDefAccessor,
			p_end);
	}
	else
	{
		pDesc->AddParam(pid, NULL, type, flags, 0, 
			p_accessor, &theDefAccessor,
			p_end);
	}

	// A quick way to define UI options.
	// Note that we always pass 0 for the ctrl id, this is because
	// we honestly don't know what our ctrl Id's will be.
	switch((int)baseType)
	{
	case TYPE_FLOAT:	
	case TYPE_ANGLE:	
	case TYPE_PCNT_FRAC:	
	case TYPE_WORLD:	
		pDesc->ParamOption(pid, p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, 0, 0, SPIN_AUTOSCALE, p_end);
		pDesc->ParamOption(pid, p_range, -99999999.0, 99999999.0, p_end); 
		break;
	case TYPE_INT:		
		pDesc->ParamOption(pid, p_ui, TYPE_SPINNER, EDITTYPE_INT, 0, 0, SPIN_AUTOSCALE, p_end);  
		pDesc->ParamOption(pid, p_range, -99999999, 99999999, p_end); 
		break;
	case TYPE_RGBA:		pDesc->ParamOption(pid, p_ui, TYPE_COLORSWATCH, 0, p_end); break;
	case TYPE_POINT3:	
		pDesc->ParamOption(pid, p_ui, TYPE_SPINNER, EDITTYPE_UNIVERSE, 0, 0, 0, 0, 0, 0, SPIN_AUTOSCALE, p_end); 
		pDesc->ParamOption(pid, p_range, -99999999.0, 99999999.0, p_end); 
		break;
	case TYPE_POINT4:	
		pDesc->ParamOption(pid, p_ui, TYPE_SPINNER, EDITTYPE_UNIVERSE, 0, 0, 0, 0, 0, 0, 0, 0, SPIN_AUTOSCALE, p_end); 
		pDesc->ParamOption(pid, p_range, -99999999.0, 99999999.0, p_end); 
		break;
	case TYPE_BOOL:		pDesc->ParamOption(pid, p_ui, TYPE_SINGLECHEKBOX, 0, p_end); break;
	case TYPE_INODE:	pDesc->ParamOption(pid, p_ui, TYPE_PICKNODEBUTTON, 0, p_end); break;
	case TYPE_MTL:		pDesc->ParamOption(pid, p_ui, TYPE_MTLBUTTON, 0, p_end); break;
	case TYPE_TEXMAP:	pDesc->ParamOption(pid, p_ui, TYPE_TEXMAPBUTTON, 0, p_end); break;
	case TYPE_STRING:	pDesc->ParamOption(pid, p_ui, TYPE_EDITBOX, 0, p_end); break;
	}

	// Our new parameter isn't named yet.  Assign
	// the same name our last parameter held.
	SetMaxParamName(pDesc, pid, sName);
	return pid;
}

ParamID AddMaxParameter(ParamBlockDesc2* pDesc, int type, const char* cName )
{
	MSTR sName = MSTR::FromACP(cName);
	return AddMaxParameter(pDesc, type, sName.data());
}

void SetMaxParamLimits(ParamBlockDesc2* pDesc, ParamID pid, DFGWrapper::PortPtr& port)
{
	ParamDef& def = pDesc->GetParamDef(pid);
	int baseType = base_type(def.type);

	FabricCore::Variant uiMin = port->getOption("uiMin");
	FabricCore::Variant uiMax = port->getOption("uiMax");
	if(uiMin.isNull() || uiMax.isNull())
		return;

	switch((int)baseType)
	{
	case TYPE_FLOAT:	
	case TYPE_ANGLE:	
	case TYPE_PCNT_FRAC:	
	case TYPE_WORLD:
		{
			float vMin;
			float vMax;
			SpliceToMaxValue(uiMax, vMax);
			SpliceToMaxValue(uiMin, vMin);
			pDesc->ParamOption(pid, p_range, vMin, vMax, p_end); 
			break;
		}
	case TYPE_INT:
		{
			int vMin;
			int vMax;
			SpliceToMaxValue(uiMax, vMax);
			SpliceToMaxValue(uiMin, vMin);
			pDesc->ParamOption(pid, p_range,  vMin, vMax, p_end); 
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
}

template<typename TType>
void SetMaxParamDefault(ParamBlockDesc2* pDesc, ParamID pid, FabricCore::Variant& defaultVal) {
	TType def;
	SpliceToMaxValue(defaultVal, def);
	pDesc->ParamOption(pid, p_default, def, p_end); 
}

template<typename TType>
void SetMaxParamDefault(ParamBlockDesc2* pDesc, ParamID pid, FabricCore::RTVal& defaultVal) {
	TType def;
	SpliceToMaxValue(defaultVal, def);
	pDesc->ParamOption(pid, p_default, def, p_end);
}

void SetMaxParamDefault(ParamBlockDesc2* pDesc, ParamID pid, DFGWrapper::PortPtr& port) {

	//if (!port->isValid())
	//	return;

	//FabricCore::RTVal defaultVal = port->getDefaultValue();
	//if(!defaultVal.isValid())
	//	return;

	//ParamDef& def = pDesc->GetParamDef(pid);
	//int baseType = base_type(def.type);
	//switch((int)baseType)
	//{
	//case TYPE_BOOL:
	//case TYPE_INT:
	//	{
	//		SetMaxParamDefault<int>(pDesc, pid, defaultVal);
	//		break;
	//	}
	//case TYPE_FLOAT:	
	//case TYPE_ANGLE:	
	//case TYPE_PCNT_FRAC:	
	//case TYPE_WORLD:
	//	{
	//		SetMaxParamDefault<float>(pDesc, pid, defaultVal);
	//		break;
	//	}
	//case TYPE_RGBA:
	//	{
	//		SetMaxParamDefault<Color>(pDesc, pid, defaultVal);
	//		break;
	//	}
	//case TYPE_POINT3:
	//	{
	//		SetMaxParamDefault<Point3>(pDesc, pid, defaultVal);
	//		break;
	//	}
	//case TYPE_FRGBA:
	//case TYPE_POINT4:
	//	{
	//		SetMaxParamDefault<Point4>(pDesc, pid, defaultVal);
	//		break;
	//	}
	//case TYPE_MATRIX3:
	//	{
	//		SetMaxParamDefault<Matrix3>(pDesc, pid, defaultVal);
	//		break;
	//	}
	//case TYPE_STRING:
	//	{
	//		SetMaxParamDefault<MSTR>(pDesc, pid, defaultVal);
	//		break;
	//	}
	//case TYPE_INODE:
	//case TYPE_REFTARG:
	//	{
	//		// No default possible for this type.
	//		break;
	//	}
	//default:
	//	DbgAssert(0 && "Implment me");
	//	//case TYPE_INODE:	pDesc->ParamOption(pid, p_ui, TYPE_PICKNODEBUTTON, 0, p_end); break;
	//	//case TYPE_MTL:		pDesc->ParamOption(pid, p_ui, TYPE_MTLBUTTON, 0, p_end); break;
	//	//case TYPE_TEXMAP:	pDesc->ParamOption(pid, p_ui, TYPE_TEXMAPBUTTON, 0, p_end); break;
	//}
}

void SetMaxParamName(ParamBlockDesc2* pDesc, ParamID pid, const MCHAR* name)
{
	if (pDesc == NULL)
		return;
	// Some ports don't have max params. Skip these ones. 
	// Note: We would only get here if a port has a pid option, but the max param has
	// been removed. The pid should be removed from the port when its max param is removed.
	if(pDesc->IDtoIndex(pid) != -1){
		// Each PB2 owns its own string memory, ensure its cleaned up.
		ParamDef& newDef = pDesc->GetParamDef(pid);
		free((void*)newDef.int_name);
		if (name != NULL)
			newDef.int_name = _tcsdup(name);
		else
			newDef.int_name = NULL;
	}
}

//////////////////////////////////////////////////////////////////////////
// useful defines

#define DLG_PAD		4
#define DLG_X		108
#define CTRL_HEIGHT	12
#define PARAM_Y		(CTRL_HEIGHT + (DLG_PAD/2))
#define PARAM_X		(DLG_X - DLG_PAD)

#define LBL_WIDTH	50
#define CTRL_WIDTH	(PARAM_X - LBL_WIDTH)

/*! Add a UI definition for the specified parameter to m_dialogTemplate.
	\param[in|out] ypos - The Y position of the element.  Before this function exits, this value should be incremented by 
		the height of the created dialog element.
	\param[in|out] ctrlId - The ID of the next dialog element to create.  This value must be incremented for each
		UI element created.
	\param[out] paramCtrl - Receives the Win32 ID of the newly created dialog element (if successful).
	\param width - The width to set the new element too
	\param ctlrClass - The UNICODE name of the Max-defined custom control class to create for this element.  Must match the
		custom control classes (deriving from ICustControl in the Max SDK)
	\param flags - The standard Win32 Window Styles */
BOOL GenerateParamUI( DynamicDialog::CDynamicDialogTemplate* dialog, SHORT &ypos, WORD &ctrlId, int &paramCtrl, SHORT width, const wchar_t* ctrlClass, UINT flags = WS_TABSTOP | WS_CHILD | WS_VISIBLE )
{
	if( dialog->AddControl( flags, 0, LBL_WIDTH + DLG_PAD, ypos, width, CTRL_HEIGHT, ctrlId, NULL, ctrlClass) )
	{
		ypos += PARAM_Y;
		paramCtrl = ctrlId++;
		return TRUE;
	}
	return FALSE;
}

/*! Add a the usual max spinner & editbox combo for the specified parameter to m_dialogTemplate.
	\param[in|out] ypos - The Y position of the element.  Before this function exits, this value should be incremented by 
		the height of the created dialog element.
	\param[in|out] ctrlId - The ID of the next dialog element to create.  This value must be incremented for each
		UI element created.
	\param[out] pbDef - A pointer to an array to receive the id's of the dialog elements created. */
BOOL GenerateParamSpinner( DynamicDialog::CDynamicDialogTemplate* dialog, SHORT &ypos, WORD &ctrlId, int* paramCtrl)
{
	// Add our spinner
	if( dialog->AddControl( WS_CHILD | WS_VISIBLE, 0, PARAM_X - 8, ypos, 8, CTRL_HEIGHT, ctrlId, NULL, L"SpinnerControl" ) )
	{
		paramCtrl[1] = ctrlId++;
		// And now the editbox.
		return GenerateParamUI(dialog, ypos, ctrlId, paramCtrl[0], CTRL_WIDTH - 16, L"CustEdit");
	}
	return FALSE;
}

/*! Get the size of the dialog needed to show the parameters */
int GetDlgSize(IParamBlock2* pblock)
{
	int dlgSize = 0;
	for (int i = 0; i < pblock->NumParams(); i++)
	{
		ParamID pid = pblock->IndextoID(i);
		int paramType = pblock->GetParameterType(pid);
		int count = is_tab(paramType) ? pblock->Count(pid) : 1;
		switch((int)base_type(paramType))
		{
		case TYPE_POINT3:
			dlgSize += PARAM_Y * 3 * count;
			break;
		case TYPE_POINT4:
			dlgSize += PARAM_Y * 4 * count;
			break;
		default:
			dlgSize += PARAM_Y * count;
			break;
		}
	}
	return dlgSize;
}

// Entry point - create dynamic dialog */
DynamicDialog::CDynamicDialogTemplate* GeneratePBlockUI(IParamBlock2* pblock)
{
	if (pblock == NULL) 
		return NULL;

	ParamBlockDesc2* pDesc = pblock->GetDesc();
	int paramCount = pDesc->Count();
	if (paramCount == 0)
		return NULL;

	int dlgSize = GetDlgSize(pblock);

	DynamicDialog::CDynamicDialogTemplate* dialogTemplate = new DynamicDialog::CDynamicDialogTemplate;
	if (!dialogTemplate)
		return false; // we should probably actually throw here.

	dialogTemplate->Create( DLG_X, dlgSize + DLG_PAD, DS_FIXEDSYS | DS_SETFONT | WS_CHILD, 0, NULL, _T("MS Shell Dlg"));

	SHORT ypos = DLG_PAD / 2;
	WORD ctrlId = 0;
	for (int i = 0; i < paramCount; i++)
	{
		ParamID pid = pDesc->IndextoID(i);
		ParamDef& pbDef = pDesc->GetParamDef(pid);

		// Post the name (in unicode)
		const MCHAR* pName = pbDef.int_name;

		dialogTemplate->AddStaticControl(WS_VISIBLE | WS_CHILD | WS_GROUP, 0, DLG_PAD / 2, ypos, LBL_WIDTH, CTRL_HEIGHT, -1, pName);
		//m_dialogTemplate->AddStaticControl(WS_VISIBLE, 0, 50, 50, 100, 50, ctrlId++, buf);

		switch ((int)pbDef.type)
		{
		case TYPE_FLOAT:
		case TYPE_ANGLE:
		case TYPE_PCNT_FRAC:
		case TYPE_WORLD:
		case TYPE_INT:
		case TYPE_INDEX:
		case TYPE_TIMEVALUE:
			GenerateParamSpinner(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs);
			break;
		case TYPE_STRING:	GenerateParamUI(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, L"CustEdit"); break;
		case TYPE_BOOL:		GenerateParamUI(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, L"Button", BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE); break;
		case TYPE_MTL:		GenerateParamUI(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, L"CustButton"); break;
		case TYPE_TEXMAP:	GenerateParamUI(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, L"CustButton"); break;
		case TYPE_INODE:	GenerateParamUI(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, L"CustButton"); break;
		case TYPE_RGBA:		GenerateParamUI(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, L"ColorSwatch"); break;
		case TYPE_POINT3:	
			GenerateParamSpinner(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs); 
			GenerateParamSpinner(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs + 2); 
			GenerateParamSpinner(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs + 4);
			break;
		case TYPE_POINT4:	
			GenerateParamSpinner(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs); 
			GenerateParamSpinner(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs + 2); 
			GenerateParamSpinner(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs + 4);
			GenerateParamSpinner(dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs + 6);
			break;
		}
	}
	return dialogTemplate;
}

// Add a parameter to the splice graph
const DFGWrapper::PortPtr AddSpliceParameter(DFGWrapper::Binding& rBinding, int type, const MCHAR* pName, FabricCore::DFGPortType mode)
{
	
	return AddSpliceParameter(rBinding, type, CStr::FromMCHAR(pName).data(), mode);
}

//////////////////////////////////////////////////////////////////////////
// Helper functions for accessing options
int GetPortParamID(DFGWrapper::PortPtr& aPort)
{
	if (aPort)
	{
		FabricCore::Variant option = aPort->getOption(MAX_PID_OPT);
		if(option.isSInt32())
			return (int)option.getSInt32();
	}
	return -1;
}

void SetPortParamID(DFGWrapper::PortPtr& aPort, ParamID id) 
{
	FabricCore::Variant var = GetVariant((int)id);
	aPort->setOption(MAX_PID_OPT, &var);
}

std::string GetPortConnection(DFGWrapper::PortPtr& aPort) 
{
	std::string res;
	if (aPort->isValid())
	{
		FabricCore::Variant option = aPort->getOption(MAX_SRC_OPT);
		if(option.isString())
			res = option.getStringData();
	}
	return res;
}

void SetPortConnection(DFGWrapper::PortPtr& aPort, const char* name) 
{
	if (aPort->isValid())
	{
		FabricCore::Variant variant = GetVariant(name);
		aPort->setOption(MAX_SRC_OPT, &variant);
	}
}

//int GetPortConnectionIndex(DFGWrapper::PortPtr& aPort) 
//{
//	if (aPort)
//	{
//		FabricCore::Variant option = aPort.getOption(MAX_SRC_IDX_OPT);
//		if(option.isSInt32())
//			return (int)option.getSInt32();
//	}
//	return -1;
//}
//
//void SetPortConnectionIndex(DFGWrapper::PortPtr& aPort, int index) 
//{
//	if (aPort)
//	{
//		return aPort.setOption(MAX_SRC_IDX_OPT, GetVariant(index));
//	}
//}
//
//bool GetPortPostConnectionUI(DFGWrapper::PortPtr& aPort) 
//{
//	if (aPort)
//	{
//		FabricCore::Variant option = aPort.getOption(MAX_POST_UI_OPT);
//		if(option.isBoolean())
//			return option.getBoolean();
//	}
//	return false;
//}
//
//void SetPortPostConnectionUI(DFGWrapper::PortPtr& aPort, bool postUI) 
//{
//	if (aPort)
//	{
//		aPort.setOption(MAX_POST_UI_OPT, GetVariant(postUI));
//	}
//}
//
const char* GetPortName( DFGWrapper::PortPtr& aPort )
{
	return aPort->getName();
}

const char* GetPortType( DFGWrapper::PortPtr& aPort )
{
	return aPort->getResolvedType();
}


bool SetPortOption(DFGWrapper::PortPtr& aPort, const char* option, FPValue* value)
{
	MAXSPLICE_CATCH_BEGIN()

	if (value == nullptr)
		return false;
	FabricCore::Variant variant = GetVariant(*value);
	// if (!variant.isNull()) Do we want to allow setting Null values (remove option?);
		aPort->setOption(option, &variant);
	return true;
	MAXSPLICE_CATCH_RETURN(false);
}

bool SetPortValue(DFGWrapper::PortPtr& aPort, FPValue* value)
{
	MAXSPLICE_CATCH_BEGIN()
	if (value == nullptr)
		return false;
	FabricCore::Variant variant = GetVariant(*value);
	if (!variant.isNull())
	{
		//aPort->setVariant(variant);
		FabricCore::RTVal rtVal = aPort->getArgValue();
		//ConvertToRTVal(*value, rtVal);
		//aPort->setArgValue(rtVal);
		//return true;
	}
	MAXSPLICE_CATCH_END
	return false;
}

//////////////////////////////////////////////////////////////////////////
// Converting types to/from Fabric
BitArray SpliceTypeToMaxTypes(const char* cType)
{
	BitArray res(TYPE_DOUBLE);
	if (strcmp(cType, "Integer") == 0 ||
		strcmp(cType, "Size") == 0)
		res.Set(TYPE_INT);
	else if (strcmp(cType, "Scalar") == 0)
	{
		res.Set(TYPE_FLOAT);
		res.Set(TYPE_ANGLE);
		res.Set(TYPE_WORLD);
		res.Set(TYPE_PCNT_FRAC);
	}
	else if (strcmp(cType, "Color") == 0)
	{
		res.Set(TYPE_POINT3);
		res.Set(TYPE_POINT4);
		res.Set(TYPE_FRGBA);
	}
	else if (strcmp(cType, "Vec3") == 0)
	{
		res.Set(TYPE_POINT3);
		res.Set(TYPE_INODE);	// Devolves to position
	}
	else if (strcmp(cType, "Vec4") == 0)
		res.Set(TYPE_POINT4);
	else if (strcmp(cType, "Boolean") == 0)
		res.Set(TYPE_BOOL);
	else if (strcmp(cType, "Mat44") == 0)
	{
		res.Set(TYPE_MATRIX3);
		res.Set(TYPE_INODE);
	}
	else if (strcmp(cType, "Quat") == 0)
	{
		// There is no Type QUAT in PB2 land
		res.Set(TYPE_INODE); // devolves to rotation
		res.Set(TYPE_POINT4);
		res.Set(TYPE_REFTARG); // Can only be type Rotation Controller.
		res.Set(TYPE_QUAT);
	}
	else if (strcmp(cType, "String") == 0)
	{
		res.Set(TYPE_STRING);
		res.Set(TYPE_FILENAME);
	}
	else if (strcmp(cType, "PolygonMesh") == 0)
	{
		res.Set(TYPE_INODE);
		res.Set(TYPE_REFTARG); // Can only be type Object
		res.Set(TYPE_MESH);
	}
	// All param types can be set to a reference target when connecting ports.
	res.Set(TYPE_REFTARG);
	return res;
}

int SpliceTypeToMaxType(const char* cType, bool isArray /*=false*/)
{
	int res = -1;
	// Max only supports 1 type of int, boring old SInt32
	// We'll accept the rest though, and hope we don't 
	// overflow anywhere
	if (strcmp(cType, "Integer") == 0 || 
		strcmp(cType, "Size") == 0 || 
		strcmp(cType, "SInt32") == 0 || 
		strcmp(cType, "SInt16") == 0 || 
		strcmp(cType, "UInt32") == 0 ||
		strcmp(cType, "UInt64") == 0 || 
		strcmp(cType, "UInt16") == 0)
		res = TYPE_INT;
	else if (strcmp(cType, "Boolean") == 0)
		res = TYPE_BOOL;
	// Similarily, we only support a single FP type
	else if (strcmp(cType, "Scalar") == 0 ||
		strcmp(cType, "Float16") == 0 ||
		strcmp(cType, "Float32") == 0 ||
		strcmp(cType, "Float64") == 0)
		res = TYPE_FLOAT;
	else if (strcmp(cType, "Color") == 0)
		res =  TYPE_FRGBA;
	else if (strcmp(cType, "Vec2") == 0)
		res =  TYPE_POINT2;
	else if (strcmp(cType, "Vec3") == 0)
		res =  TYPE_POINT3;
	else if (strcmp(cType, "Vec4") == 0)
		res =  TYPE_POINT4;
	else if (strcmp(cType, "Mat44") == 0)
		res =  TYPE_MATRIX3;
	else if (strcmp(cType, "Quat") == 0)
		res =  TYPE_QUAT;
	else if (strcmp(cType, "String") == 0)
		res =  TYPE_STRING;
	else if (strcmp(cType, "PolygonMesh") == 0)
		res =  TYPE_MESH;
	else 
	{
		// DbgAssert(0 && "NOTE: Add Default translation Type for this Splice Type");
	}

	if (isArray)
		res |= TYPE_TAB;

	return res;
}

// This function is simply here to override the default PB2 type for PolygonMesh.
// The correct type for PolyMesh is TYPE_MESH, but unfortunately the PB2
// doesn't support that data, so we need it to create an INODE parameter instead.
int SpliceTypeToDefaultMaxType(const char* cType)
{
	if (strcmp(cType, "PolygonMesh") == 0)
		return TYPE_INODE;
	return SpliceTypeToMaxType(cType);
}

bool IsPortPersistable(DFGWrapper::Port& port) {
	if (!port.isValid())
		return false;

	FabricCore::RTVal rtVal = port.getArgValue();
	if(rtVal.isValid())
	{
		if(rtVal.isObject())
		{
			if(!rtVal.isNullObject())
			{
				//FabricCore::RTVal objectRtVal = FabricCore::RTVal::Create(port.getWrappedCoreBinding(), "Object", 1, &rtVal);
				if(rtVal.isValid())
				{
					//FabricCore::RTVal persistable = FabricCore::RTVal::Construct(port.getWrappedCoreExec()., "Persistable", objectRtVal);
					//if(!persistable.isNullObject())
						return true;
				}
			}
		}
	}
	return false;
}

const DFGWrapper::PortPtr AddSpliceParameter(DFGWrapper::Binding& rBinding, const char* type, const char* cName, FabricCore::DFGPortType mode, bool isArray/*=false*/, const char* inExtension)
{
	std::string spliceType(type);
	if (isArray)
		spliceType = spliceType + "[]";

	DFGWrapper::GraphExecutablePtr graph = DFGWrapper::GraphExecutablePtr::StaticCast(rBinding.getExecutable());

	DFGWrapper::PortPtr port;// = graph->getPort(cName);
	// How can I test if this port is valid?
	//if (port->isValid() && port->isPort())
	//	return port;

	try {
		port = graph->addPort(cName, mode, type);
		port->setTypeSpec(type);
		// Does this port support custom persistence?  If so, mark it as persisted
		//if (IsPortPersistable(port)) 
		//	rGraph.setMemberPersistence(cName, true);
		return port;
	}
	catch(FabricCore::Exception e) 
	{
		CStr message = "ERROR on AddPort to Splice: ";
		logMessage(message + e.getDesc_cstr());
		return DFGWrapper::PortPtr();
	}
}

const DFGWrapper::PortPtr AddSpliceParameter(DFGWrapper::Binding& rBinding, int type, const char* cName, FabricCore::DFGPortType mode, bool isArray/*=false*/, const char* inExtension)
{
	std::string strType;
	switch ((int)type)
	{
	case TYPE_INT:		
	case TYPE_INDEX:
		strType = "Integer";
		break;
	case TYPE_FLOAT:
	case TYPE_TIMEVALUE:
	case TYPE_ANGLE:
	case TYPE_WORLD:
	case TYPE_PCNT_FRAC:
		strType = "Scalar";
		break;
	case TYPE_FRGBA:
	case TYPE_RGBA:
		strType = "Color";
		break;
	case TYPE_POINT2:
		strType = "Vec2";
		break;
	case TYPE_POINT3:
		strType = "Vec3";
		break;
	case TYPE_POINT4:
		strType = "Vec4";
		break;
	case TYPE_BOOL:
		strType = "Boolean";
		break;
	case TYPE_MATRIX3:
		strType = "Mat44";
		break;
	case TYPE_QUAT:
		strType = "Quat";
		break;
	case TYPE_STRING:
	case TYPE_FILENAME:
		strType = "String";
		break;
	case TYPE_MESH:
		strType = "PolygonMesh";
		break;
	case TYPE_INODE:		break;
	case TYPE_MTL:			break;
	case TYPE_TEXMAP:		break;

	default:
		DbgAssert(false); // What do we have here?
		break;
	}

	return AddSpliceParameter(rBinding, strType.data(), cName, mode, isArray);
}

#pragma endregion

//---------------------------------------------------------------------------------------------------------------
// Our static UI DlgProc.  Handles commands to create dynamic parameter blocks
//---------------------------------------------------------------------------------------------------------------
#pragma region//DLGPROC
INT_PTR CALLBACK DlgProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
	case WM_INITDIALOG:
		// We passed ourselves a pointer to our current instance in lParam, but
		// it will not be here next time.  Save in the USERDATA thingy.
		SetWindowLongPtr(hWnd, GWLP_USERDATA, (LONG_PTR)lParam);
		break;

	case WM_DESTROY:
		break;

	case WM_COMMAND:
		{
			SpliceTranslationFPInterface* curInstance = reinterpret_cast<SpliceTranslationFPInterface*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
			if (curInstance == NULL) 
				return 0;

			// If we are the KL editor button, pop that editor
			if (wParam == IDC_BTN_EDIT_KL)
			{
				//curInstance->ShowKLEditor();
				curInstance->ShowDFGGraphEditor();
			}
		}
		break;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MOUSEMOVE:
		break;

	default:
		return 0;
	}
	return 1;
}
#pragma endregion

//////////////////////////////////////////////////////////////////////////
#pragma region MaxToSplice conversion functions

//////////////////////////////////////////////////////////////////////////
template<typename TResultType, typename TConvertType>
void ParameterBlockValuesToSplice(FabricCore::Client& client, DFGWrapper::PortPtr& port, TimeValue t, IParamBlock2* pblock, ParamID pid, Interval& ivValid)
{
	MAXSPLICE_CATCH_BEGIN()

	int type = pblock->GetParameterType(pid);
	int nParams = 1;
	if (is_tab(type)){
		nParams = pblock->Count(pid);
		//DbgAssert(port->getPortType() isArray());
	}

	TResultType* pVals = new TResultType[nParams];
	for (int i = 0; i < nParams; i++)
		pblock->GetValue(pid, t, pVals[i], ivValid, i);
	MaxValuesToSplice<TResultType, TConvertType>(client, port, t, ivValid, pVals, nParams);
	delete pVals;

	MAXSPLICE_CATCH_END
}

template<typename TResultType>
void ParameterBlockValuesToSplice(FabricCore::Client& client, DFGWrapper::PortPtr& port, TimeValue t, IParamBlock2* pblock, ParamID pid, Interval& ivValid)
{
	ParameterBlockValuesToSplice<TResultType, TResultType>(client, port, t, pblock, pid, ivValid);
}

// This helper function converts from INode to the appropriate Splice type
void MaxPtrToSplice(FabricCore::Client& client, DFGWrapper::PortPtr& port, TimeValue t, IParamBlock2* pblock, ParamID id, Interval& ivValid)
{
	if (!port->isValid())
		return;

	// There is no "INode" type in splice, so try to figure out a conversion
	const char* cType = port->getResolvedType();
	int maxTypeRequired = SpliceTypeToMaxType(cType);
	switch(maxTypeRequired)
	{
	case TYPE_POINT3:
		{
			ParameterBlockValuesToSplice<INode*, Point3>(client, port, t, pblock, id, ivValid);
			break;
		}
	case TYPE_QUAT:
		{
			ParameterBlockValuesToSplice<INode*, Quat>(client, port, t, pblock, id, ivValid);
			break;
		}
	case TYPE_MATRIX3:
		{
			ParameterBlockValuesToSplice<INode*, Matrix3>(client, port, t, pblock, id, ivValid);
			break;
		}
	case TYPE_MESH:
		{
			// Convert to mesh if possible
			ParameterBlockValuesToSplice<INode*, Mesh>(client, port, t, pblock, id, ivValid);
			break;
		}
	default:
		DbgAssert(false); // what do we have here?
	}
}
// Pblock conversion function
void TransferAllMaxValuesToSplice(TimeValue t, IParamBlock2* pblock, FabricCore::Client& client, DFGWrapper::Binding& graph, std::vector<Interval>& paramValids, Interval& ivValid)
{
	if (pblock == NULL)
		return;

	// Iterate over all PB2 parameters, send to associated splice port
	DFGWrapper::ExecutablePtr pExecutable = graph.getExecutable();
	DFGWrapper::PortList allPorts = pExecutable->getPorts();
	for (size_t i = 0; i < allPorts.size(); i++)
	{
		DFGWrapper::PortPtr& port = allPorts[i];
		ParamID pid = ParamID(::GetPortParamID(port));
		// Its possible some params are not supported by Max.
		if (pid == -1)
			continue;

		int pidx = pblock->IDtoIndex(pid);
		if(pidx == -1){
			// Some ports report a ParamID, but infact its invalid. 
			// We now set the PID meta data to -1 on a port when its type set to '-None-' (-1).
			continue;
		}

		// Has this parameter already been translated?  Test 
		// the validity of the value currently in Splice
		if (pidx >= paramValids.size())
			paramValids.resize(pidx + 1);
		else
		{
			if (paramValids[pidx].InInterval(t))
				continue;
		}
		paramValids[pidx].SetInfinite();

		int type = pblock->GetParameterType(pid);
		switch ((int)base_type(type))
		{
		case TYPE_INT:		
		case TYPE_INDEX:
			ParameterBlockValuesToSplice<int>(client, port, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_TIMEVALUE:
			ParameterBlockValuesToSplice<TimeValue, float>(client, port, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_FLOAT:
		case TYPE_ANGLE:
		case TYPE_WORLD:
		case TYPE_PCNT_FRAC:
			ParameterBlockValuesToSplice<float>(client, port, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_RGBA:
			ParameterBlockValuesToSplice<Color>(client, port, t, pblock, pid, paramValids[pidx]);
			break;
		//case TYPE_POINT2:
		//	MaxValueToSplice(port, pblock->GetPoint3(id, t));
		//	break;
		case TYPE_POINT3:
			ParameterBlockValuesToSplice<Point3>(client, port, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_FRGBA:
			ParameterBlockValuesToSplice<Point4>(client, port, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_POINT4:
			ParameterBlockValuesToSplice<Point4>(client, port, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_BOOL:
			ParameterBlockValuesToSplice<int, bool>(client, port, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_MATRIX3:
			ParameterBlockValuesToSplice<Matrix3>(client, port, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_STRING:
		case TYPE_FILENAME:
			ParameterBlockValuesToSplice<const MCHAR*>(client, port, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_INODE:
			{
				MaxPtrToSplice(client, port, t, pblock, pid, paramValids[pidx]);
				break;
			}
		case TYPE_REFTARG:
			{
				ReferenceTarget* pSrcContainer = pblock->GetReferenceTarget(pid);
				if (pSrcContainer == nullptr)
					continue;

				// If we have a reftarg type, we may be connected to another graph
				std::string portConnection = GetPortConnection(port);
				if (portConnection.length() > 0) {
					
					SpliceTranslationFPInterface* pSrcContInterface = static_cast<SpliceTranslationFPInterface*>(pSrcContainer->GetInterface(ISPLICE__INTERFACE));
					if (pSrcContInterface == nullptr)
						continue;

					pSrcContInterface->TriggerEvaluate(t, paramValids[pidx]);
					DFGWrapper::PortPtr srcPort = pSrcContInterface->GetPort(portConnection.data());
					if (srcPort)
					{
						FabricCore::RTVal srcVal = srcPort->getArgValue();
						//int srcIndex = GetPortConnectionIndex(port);
						// If we are connected to an array, take only a single element
						//if (srcIndex >= 0 && srcVal.isArray()) {
						//	// Check for OOR
						//	if ((uint32_t)srcIndex >= srcVal.getArraySize())
						//		continue;
						//	srcVal = srcVal.getArrayElement(srcIndex);
						//}
						port->setArgValue(srcVal);
					}
				}
				else 
				{
					// TODO
					//MaxPtrToSplice(port, t, )
				}
				break;
			}
		case TYPE_MTL:			break;
		case TYPE_TEXMAP:		break;
		default:
			DbgAssert(false); // What do we have here?
			break;
		}

		ivValid &= paramValids[pidx];
	}
}

#pragma endregion

void bindingNotificationCallback(void * userData, char const *jsonCString, uint32_t jsonLength)
{
	if (!jsonCString)
		return;
	//BaseInterface * interf = (BaseInterface *)userData;

	FabricCore::Variant notificationVar = FabricCore::Variant::CreateFromJSON(jsonCString, jsonLength);

	const FabricCore::Variant * descVar = notificationVar.getDictValue("desc");
	std::string descStr = descVar->getStringData();

	if (descStr == "argTypeChanged")
	{
		printf("an argument type has changed. you might want to create a DCC port now.\n");
	}
}