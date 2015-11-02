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
		if (owner == nullptr)
			return _M("ERROR: No Owner");

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

void SetMaxParamLimits(SpliceTranslationFPInterface* pOwner, const char* argName)
{
	int id = pOwner->GetPortParamID(argName);
	if (id < 0)
		return;
	ParamID pid = ParamID(id);

	std::string rangeStr = pOwner->GetPortMetaData(argName, "uiRange");
	if (rangeStr.empty())
		return;

	ParamBlockDesc2* pDesc = pOwner->GetPBlock()->GetDesc();
	ParamDef& def = pDesc->GetParamDef(pid);
	int baseType = base_type(def.type);

	if (rangeStr.length() < 2)
		return;
	DbgAssert(rangeStr[0] == '(' && rangeStr[rangeStr.length() - 1] == ')');
	//rangeStr = rangeStr.substr(1, rangeStr.length() - 1);
	size_t splitIdx = rangeStr.find(',');
	std::string firstValStr = rangeStr.substr(1, splitIdx - 1);
	std::string secondValStr = rangeStr.substr(splitIdx + 1, rangeStr.length() - (splitIdx + 2));
	
	switch((int)baseType)
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
//
//template<typename TType>
//void SetMaxParamDefault(ParamBlockDesc2* pDesc, ParamID pid, FabricCore::Variant& defaultVal) {
//	TType def;
//	SpliceToMaxValue(defaultVal, def);
//	pDesc->ParamOption(pid, p_default, def, p_end); 
//}
//
//template<typename TType>
//void SetMaxParamDefault(ParamBlockDesc2* pDesc, ParamID pid, FabricCore::RTVal& defaultVal) {
//	TType def;
//	SpliceToMaxValue(defaultVal, def);
//	pDesc->ParamOption(pid, p_default, def, p_end);
//}
//
//void SetMaxParamDefault(ParamBlockDesc2* pDesc, ParamID pid, FabricCore::DFGBinding& binding, const char* argName) {
//
//	char const *resolvedType = GetPortType(binding, argName);
//	if (!resolvedType)
//		return;
//
//	FabricCore::RTVal defaultVal = binding.getExec().getPortDefaultValue(argName, resolvedType);
//	if(!defaultVal.isValid())
//		return;
//
//	ParamDef& def = pDesc->GetParamDef(pid);
//	int baseType = base_type(def.type);
//	switch((int)baseType)
//	{
//	case TYPE_BOOL:
//	case TYPE_INT:
//		{
//			SetMaxParamDefault<int>(pDesc, pid, defaultVal);
//			break;
//		}
//	case TYPE_FLOAT:	
//	case TYPE_ANGLE:	
//	case TYPE_PCNT_FRAC:	
//	case TYPE_WORLD:
//		{
//			SetMaxParamDefault<float>(pDesc, pid, defaultVal);
//			break;
//		}
//	case TYPE_RGBA:
//		{
//			SetMaxParamDefault<Color>(pDesc, pid, defaultVal);
//			break;
//		}
//	case TYPE_POINT3:
//		{
//			SetMaxParamDefault<Point3>(pDesc, pid, defaultVal);
//			break;
//		}
//	case TYPE_FRGBA:
//	case TYPE_POINT4:
//		{
//			SetMaxParamDefault<Point4>(pDesc, pid, defaultVal);
//			break;
//		}
//	case TYPE_MATRIX3:
//		{
//			SetMaxParamDefault<Matrix3>(pDesc, pid, defaultVal);
//			break;
//		}
//	case TYPE_STRING:
//		{
//			SetMaxParamDefault<MSTR>(pDesc, pid, defaultVal);
//			break;
//		}
//	case TYPE_INODE:
//	case TYPE_REFTARG:
//		{
//			// No default possible for this type.
//			break;
//		}
//	default:
//		DbgAssert(0 && "Implment me");
//		//case TYPE_INODE:	pDesc->ParamOption(pid, p_ui, TYPE_PICKNODEBUTTON, 0, p_end); break;
//		//case TYPE_MTL:		pDesc->ParamOption(pid, p_ui, TYPE_MTLBUTTON, 0, p_end); break;
//		//case TYPE_TEXMAP:	pDesc->ParamOption(pid, p_ui, TYPE_TEXMAPBUTTON, 0, p_end); break;
//	}
//}

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

template<typename TType>
void SetMaxParamValue(IParamBlock2* pblock, TimeValue t, ParamID pid, FabricCore::RTVal& value) {
	// Check for existence!
	if (pblock->IDtoIndex(pid) < 0)
		return;
	TType def;
	SpliceToMaxValue(value, def);
	pblock->SetValue(pid, t, def);
}

void SetMaxParamFromSplice(IParamBlock2* pblock, ParamID pid, FabricCore::DFGBinding& binding, const char* argName)
{
	if (pblock == nullptr)
		return;

	FabricCore::DFGExec exec = binding.getExec();
	if (!exec.haveExecPort(argName))
		return;

	FabricCore::RTVal currentVal = binding.getArgValue(argName);
	if (!currentVal.isValid())
		return;

	TimeValue t = GetCOREInterface()->GetTime();
	ParamDef& def = pblock->GetParamDef(pid);
	int baseType = base_type(def.type);
	switch ((int)baseType)
	{
	case TYPE_BOOL:
	case TYPE_INT:
	{
		SetMaxParamValue<int>(pblock, t, pid, currentVal);
		break;
	}
	case TYPE_FLOAT:
	case TYPE_ANGLE:
	case TYPE_PCNT_FRAC:
	case TYPE_WORLD:
	{
		SetMaxParamValue<float>(pblock, t, pid, currentVal);
		break;
	}
	case TYPE_RGBA:
	{
		SetMaxParamValue<Color>(pblock, t, pid, currentVal);
		break;
	}
	case TYPE_POINT3:
	{
		SetMaxParamValue<Point3>(pblock, t, pid, currentVal);
		break;
	}
	case TYPE_FRGBA:
	case TYPE_POINT4:
	{
		SetMaxParamValue<Point4>(pblock, t, pid, currentVal);
		break;
	}
	case TYPE_MATRIX3:
	{
		SetMaxParamValue<Matrix3>(pblock, t, pid, currentVal);
		break;
	}
	case TYPE_STRING:
	{
		SetMaxParamValue<MSTR>(pblock, t, pid, currentVal);
		break;
	}
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
std::string AddSpliceParameter(SpliceTranslationFPInterface* pOwner, int type, const MCHAR* pName, FabricCore::DFGPortType mode)
{
	return AddSpliceParameter(pOwner, type, CStr::FromMCHAR(pName).data(), mode);
}

//////////////////////////////////////////////////////////////////////////
// Helper functions for accessing options
int GetPortParamID(const FabricCore::DFGExec& exec, const char* argName)
{
	const char* idstr = const_cast<FabricCore::DFGExec&>(exec).getExecPortMetadata(argName, MAX_PID_OPT);
	if (idstr != nullptr && idstr[0] != '\0') {
		return atoi(idstr);
	}
	return -1;
}

std::string GetPortConnection(FabricCore::DFGBinding& binding, const char* argName) 
{
	return binding.getExec().getExecPortMetadata(argName, MAX_SRC_OPT);
}

void SetPortConnection(SpliceTranslationFPInterface* pOwner, const char* argName, const char* name)
{
	pOwner->SetPortMetaData(argName, MAX_SRC_OPT, name, "");
}

//int GetPortConnectionIndex(FabricCore::DFGBinding& binding, const char* argName) 
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
//void SetPortConnectionIndex(FabricCore::DFGBinding& binding, const char* argName, int index) 
//{
//	if (aPort)
//	{
//		return aPort.setOption(MAX_SRC_IDX_OPT, GetVariant(index));
//	}
//}
//
//bool GetPortPostConnectionUI(FabricCore::DFGBinding& binding, const char* argName) 
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
//void SetPortPostConnectionUI(FabricCore::DFGBinding& binding, const char* argName, bool postUI) 
//{
//	if (aPort)
//	{
//		aPort.setOption(MAX_POST_UI_OPT, GetVariant(postUI));
//	}
//}
//
//const char* GetPortName( FabricCore::DFGBinding& binding, const char* argName )
//{
//
//	return aPort->getName();
//}
//
const char* GetPortType( const FabricCore::DFGExec& exec, const char* argName )
{
	return const_cast<FabricCore::DFGExec&>(exec).getExecPortResolvedType(argName);
}
//

int GetPort3dsMaxType(const FabricCore::DFGExec& exec, const char* argName)
{
	const char* idstr = const_cast<FabricCore::DFGExec&>(exec).getExecPortMetadata(argName, MAX_PARM_TYPE_OPT);
	int maxType = -2;
	if (idstr != nullptr && idstr[0] != '\0') {
		maxType = atoi(idstr);
	}

	const char* portType = GetPortType(exec, argName);
	if (maxType > 0)
	{
		// If our type is not legal for our current port type,
		// reset it to the default
		BitArray legalTypes = SpliceTypeToMaxTypes(portType);
		if (!legalTypes[maxType])
			maxType = -2;
	}
	if (maxType == -2)
		maxType = SpliceTypeToDefaultMaxType(portType);

	return maxType;
}
//
//void SetPort3dsMaxType(FabricCore::DFGBinding& binding, const char* argName, int type)
//{
//	char buff[20];
//	_itoa_s(type, buff, 10);
//	bool doUndo = theHold.Holding();
//	if (doUndo)
//		theHold.Put(new FabricCoreRestoreObj());
//	binding.getExec().setExecPortMetadata(argName, MAX_PARM_TYPE_OPT, buff, doUndo);
//}

//bool SetPortOption(FabricCore::DFGBinding& binding, const char* argName, const char* option, FPValue* value)
//{
//	MAXSPLICE_CATCH_BEGIN
//
//	//if (value == nullptr)
//	//	return false;
//	//FabricCore::Variant variant = GetVariant(*value);
//	//// if (!variant.isNull()) Do we want to allow setting Null values (remove option?);
//	//	aPort->setOption(option, &variant);
//	//return true;
//	MAXSPLICE_CATCH_RETURN(false);
//}

bool SetPortValue(FabricCore::DFGBinding& binding, const char* argName, FPValue* value)
{
	MAXSPLICE_CATCH_BEGIN
	if (value == nullptr)
		return false;

	Interval iv;
	TimeValue t = GetCOREInterface()->GetTime();
	MaxValueToSplice(binding, argName, t, iv, *value);
	return true;
#pragma message("TEST THIS")
	MAXSPLICE_CATCH_END
	return false;
}

//////////////////////////////////////////////////////////////////////////
// Converting types to/from Fabric

static const std::string _array_denom("Array");
bool is_array(const std::string& value)
{
	if (_array_denom.size() > value.size()) return false;
	return std::equal(_array_denom.rbegin(), _array_denom.rend(), value.rbegin());

}
bool IsPortArray(const FabricCore::DFGExec& exec, const char* argName)
{
	return is_array(GetPortType(exec, argName));
}
// If the 'value' string is an array type, remove the Array
// denominator and return true, else false;
inline bool sub_array(std::string& type)
{
	if (is_array(type)) {
		type.resize(type.size() - _array_denom.size());
		return true;
	}
	return false;
}

BitArray SpliceTypeToMaxTypes(const char* cType)
{
	
	if (cType == NULL)
		return BitArray();

	int arr_mod = 0;
	std::string sType = cType;
	if (sub_array(sType))
		arr_mod = TYPE_TAB;

	BitArray res(TYPE_DOUBLE + arr_mod);
	if (sType == "Integer" ||
		sType == "Size" ||
		sType == "UInt32" ||
		sType == "UInt16" ||
		sType == "UInt8" ||
		sType == "UInt64" ||
		sType == "SInt32" ||
		sType == "SInt16" ||
		sType == "SInt8" ||
		sType == "SInt64")
	{
		res.Set(TYPE_INT + arr_mod);
	}
	else if (sType == "Scalar" || 
		sType == "Float32" || 
		sType == "Float64")
	{
		res.Set(TYPE_FLOAT + arr_mod);
		res.Set(TYPE_ANGLE + arr_mod);
		res.Set(TYPE_WORLD + arr_mod);
		res.Set(TYPE_PCNT_FRAC + arr_mod);
	}
	else if (sType == "Color")
	{
		res.Set(TYPE_POINT3 + arr_mod);
		res.Set(TYPE_POINT4 + arr_mod);
		res.Set(TYPE_FRGBA + arr_mod);
	}
	else if (sType == "Vec3")
	{
		res.Set(TYPE_POINT3 + arr_mod);
		res.Set(TYPE_INODE + arr_mod);	// Devolves to position
	}
	else if (sType == "Vec4")
	{
		res.Set(TYPE_POINT4 + arr_mod);
		res.Set(TYPE_FRGBA + arr_mod);
	}
	else if (sType == "Boolean")
		res.Set(TYPE_BOOL + arr_mod);
	else if (sType == "Mat44")
	{
		res.Set(TYPE_MATRIX3 + arr_mod);
		res.Set(TYPE_INODE + arr_mod);
	}
	else if (sType == "Quat")
	{
		// There is no Type QUAT in PB2 land
		res.Set(TYPE_INODE + arr_mod); // devolves to rotation
		res.Set(TYPE_POINT4 + arr_mod);
		res.Set(TYPE_REFTARG + arr_mod); // Can only be type Rotation Controller.
		res.Set(TYPE_QUAT + arr_mod);
	}
	else if (sType == "String")
	{
		res.Set(TYPE_STRING + arr_mod);
		res.Set(TYPE_FILENAME + arr_mod);
	}
	else if (sType == "PolygonMesh")
	{
		res.Set(TYPE_INODE + arr_mod);
		res.Set(TYPE_REFTARG + arr_mod); // Can only be type Object
		res.Set(TYPE_MESH + arr_mod);
	}
	// All param types can be set to a reference target when connecting ports.
	res.Set(TYPE_REFTARG + arr_mod);
	return res;
}

int SpliceTypeToMaxType(const char* cType)
{
	std::string sType = cType;
	bool isArray = sub_array(sType);
	int res = -1;
	// Max only supports 1 type of int, boring old SInt32
	// We'll accept the rest though, and hope we don't 
	// overflow anywhere
	if (sType == "Integer" || 
		sType == "Size" || 
		sType == "SInt32" || 
		sType == "SInt16" || 
		sType == "UInt32" ||
		sType == "UInt64" || 
		sType == "UInt16")
		res = TYPE_INT;
	else if (sType == "Boolean")
		res = TYPE_BOOL;
	// Similarily, we only support a single FP type
	else if (sType == "Scalar" ||
		sType == "Float16" ||
		sType == "Float32" ||
		sType == "Float64")
		res = TYPE_FLOAT;
	else if (sType == "Color")
		res =  TYPE_FRGBA;
	else if (sType == "Vec2")
		res =  TYPE_POINT2;
	else if (sType == "Vec3")
		res =  TYPE_POINT3;
	else if (sType == "Vec4")
		res =  TYPE_POINT4;
	else if (sType == "Mat44")
		res =  TYPE_MATRIX3;
	else if (sType == "Quat")
		res =  TYPE_QUAT;
	else if (sType == "String")
		res =  TYPE_STRING;
	else if (sType == "PolygonMesh")
		res =  TYPE_MESH;
	else 
	{
		// DbgAssert(0 && "NOTE: Add Default translation Type for this Splice Type");
	}

	if (res >= 0 && isArray)
		res |= TYPE_TAB;

	return res;
}

// This function is simply here to override the default PB2 type for PolygonMesh.
// The correct type for PolyMesh is TYPE_MESH, but unfortunately the PB2
// doesn't support that data, so we need it to create an INODE parameter instead.
int SpliceTypeToDefaultMaxType(const char* cType)
{
	if (cType == nullptr)
		return -1;

	if (strcmp(cType, "PolygonMesh") == 0)
		return TYPE_INODE;
	return SpliceTypeToMaxType(cType);
}

//
//std::string MaxTypeToSpliceType(int type)
//{
//	switch (type) {
//	case TYPE_BOOL:
//	case TYPE_bool:
//		return "Boolean";
//	case TYPE_INT:
//	case TYPE_RADIOBTN_INDEX:
//	case TYPE_INDEX:
//		return "SInt32";
//	case TYPE_FLOAT:
//	case TYPE_ANGLE:
//	case TYPE_PCNT_FRAC:
//	case TYPE_WORLD:
//	case TYPE_TIMEVALUE:
//		return "Float32";
//	case TYPE_POINT2:
//		return "Vec2";
//	case TYPE_RGBA:
//	case TYPE_POINT3:
//	case TYPE_COLOR_CHANNEL:
//		return "Vec3";
//	case TYPE_STRING:
//	case TYPE_FILENAME:
//		return  "String";
//
//	}
//		//TYPE_MTL,					//!< A pointer to a material object. This can be one of three types: a reference owned by the parameter block, a reference owned by the block owner, or no reference management (just a copy of the pointer).
//		//TYPE_TEXMAP,				//!< A pointer to a texture map object. This can be one of three types: a reference owned by the parameter block, a reference owned by the block owner, or no reference management (just a copy of the pointer). 
//		//TYPE_BITMAP,				//!< A pointer to a bitmap object. This can be one of three types: a reference owned by the parameter block, a reference owned by the block owner, or no reference management (just a copy of the pointer).
//		TYPE_INODE,					//!< A pointer to a node. This can be one of three types: a reference owned by the parameter block, a reference owned by the block owner, or no reference management (just a copy of the pointer). 
//		TYPE_REFTARG,				//!< A pointer to a reference target. All reference targets in this group can be one of three types: reference owned by parameter block, reference owned by block owner, or no reference management (just a copy of the pointer).
//		// new for R4...
//		TYPE_MATRIX3,				//!< A standard 3ds Max matrix.
//		TYPE_POINT4,
//		TYPE_FRGBA,
//
//		// only for published function parameter types, not pblock2 parameter types...
//		TYPE_ENUM,
//		TYPE_VOID,
//		TYPE_INTERVAL,
//		TYPE_ANGAXIS,
//		TYPE_QUAT,
//		TYPE_RAY,
//		TYPE_POINT2,
//		TYPE_BITARRAY,
//		TYPE_CLASS,
//		TYPE_MESH,
//		TYPE_OBJECT,
//		TYPE_CONTROL,
//		TYPE_POINT,
//		TYPE_TSTR,
//		TYPE_IOBJECT,
//		TYPE_INTERFACE,
//		TYPE_HWND,
//		TYPE_NAME,
//		TYPE_COLOR,
//		TYPE_FPVALUE,
//		TYPE_VALUE,
//		TYPE_DWORD,
//		TYPE_bool,
//		TYPE_INTPTR,
//		TYPE_INT64,
//		TYPE_DOUBLE,
//
//	int res = -1;
//	// Max only supports 1 type of int, boring old SInt32
//	// We'll accept the rest though, and hope we don't 
//	// overflow anywhere
//	if (strcmp(cType, "Integer") == 0 ||
//		strcmp(cType, "Size") == 0 ||
//		strcmp(cType, "SInt32") == 0 ||
//		strcmp(cType, "SInt16") == 0 ||
//		strcmp(cType, "UInt32") == 0 ||
//		strcmp(cType, "UInt64") == 0 ||
//		strcmp(cType, "UInt16") == 0)
//		res = TYPE_INT;
//	else if (strcmp(cType, "Boolean") == 0)
//		res = TYPE_BOOL;
//	// Similarily, we only support a single FP type
//	else if (strcmp(cType, "Scalar") == 0 ||
//		strcmp(cType, "Float16") == 0 ||
//		strcmp(cType, "Float32") == 0 ||
//		strcmp(cType, "Float64") == 0)
//		res = TYPE_FLOAT;
//	else if (strcmp(cType, "Color") == 0)
//		res = TYPE_FRGBA;
//	else if (strcmp(cType, "Vec2") == 0)
//		res = TYPE_POINT2;
//	else if (strcmp(cType, "Vec3") == 0)
//		res = TYPE_POINT3;
//	else if (strcmp(cType, "Vec4") == 0)
//		res = TYPE_POINT4;
//	else if (strcmp(cType, "Mat44") == 0)
//		res = TYPE_MATRIX3;
//	else if (strcmp(cType, "Quat") == 0)
//		res = TYPE_QUAT;
//	else if (strcmp(cType, "String") == 0)
//		res = TYPE_STRING;
//	else if (strcmp(cType, "PolygonMesh") == 0)
//		res = TYPE_MESH;
//	else
//	{
//		// DbgAssert(0 && "NOTE: Add Default translation Type for this Splice Type");
//	}
//
//	if (isArray)
//		res |= TYPE_TAB;
//
//	return res;
//}

////////////////////////////////////////////////////////////////////////////
//bool IsPortPersistable(DFGWrapper::ExecPort& port) {
//	if (!port.isValid())
//		return false;
//
//	FabricCore::RTVal rtVal = port.getArgValue();
//	if(rtVal.isValid())
//	{
//		if(rtVal.isObject())
//		{
//			if(!rtVal.isNullObject())
//			{
//				//FabricCore::RTVal objectRtVal = FabricCore::RTVal::Create(port.getWrappedCoreBinding(), "Object", 1, &rtVal);
//				if(rtVal.isValid())
//				{
//					//FabricCore::RTVal persistable = FabricCore::RTVal::Construct(port.getWrappedCoreExec()., "Persistable", objectRtVal);
//					//if(!persistable.isNullObject())
//						return true;
//				}
//			}
//		}
//	}
//	return false;
//}

std::string AddSpliceParameter(SpliceTranslationFPInterface* pOwner, const char* type, const char* cName, FabricCore::DFGPortType mode, const char* inExtension)
{
	try
	{
		std::string res = pOwner->GetCommandHandler()->dfgDoAddPort(pOwner->GetBinding(), "", pOwner->GetBinding().getExec(), cName, mode, type, "", inExtension, "");
		return cName;
	}
	catch(FabricCore::Exception e) 
	{
		CStr message = "ERROR on AddPort to Splice: ";
		logMessage(message + e.getDesc_cstr());
		return nullptr;
	}
}

std::string AddSpliceParameter(SpliceTranslationFPInterface* pOwner, int type, const char* cName, FabricCore::DFGPortType mode, const char* inExtension)
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
	case TYPE_INODE:
	case TYPE_MTL:
	case TYPE_TEXMAP:

	default:
		DbgAssert(false); // What do we have here?
		return std::string();
	}

	return AddSpliceParameter(pOwner, strType.data(), cName, mode);
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

			// A few static vars are shared between save and load
			static int filterIndex = 0;
			static MSTR initialDir = MSTR::FromACP(std::getenv("FABRIC_DIR"));

			// If we are the KL editor button, pop that editor
			if (wParam == IDC_BTN_EDIT_KL)
			{
				//curInstance->ShowKLEditor();
				curInstance->ShowDFGGraphEditor();
			}
			else if (wParam == IDC_SAVE)
			{
				MSTR filename = curInstance->GetGraphName();
				filename += _M(".dfg.json");

				FilterList filterList;
				filterList.Append(_M("DFG Preset files(*.dfg.json)"));
				filterList.Append(_M("*.dfg.json"));

				filterList.Append(_M("All Files (*.*)"));
				filterList.Append(_M("*.*"));

				filterList.SetFilterIndex(filterIndex);

				if (GetCOREInterface8()->DoMaxSaveAsDialog(NULL, GetString(IDS_SAVE_PRESET), filename, initialDir, filterList))
				{
					filterIndex = filterList.GetFilterIndex();
					curInstance->SaveToFile(filename.data());
				}
			}
			else if (wParam == IDC_LOAD)
			{
				MSTR filename = curInstance->GetGraphName();
				filename += _M(".dfg.json");

				FilterList filterList;
				filterList.Append(_M("DFG Preset files(*.dfg.json)"));
				filterList.Append(_M("*.dfg.json"));

				filterList.Append(_M("All Files (*.*)"));
				filterList.Append(_M("*.*"));

				filterList.SetFilterIndex(filterIndex);

				if (GetCOREInterface8()->DoMaxOpenDialog(NULL, GetString(IDS_LOAD_PRESET), filename, initialDir, filterList))
				{
					filterIndex = filterList.GetFilterIndex();
					curInstance->LoadFromFile(filename.data(), true);
				}
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
void ParameterBlockValuesToSplice(FabricCore::DFGBinding& binding, const char* argName, TimeValue t, IParamBlock2* pblock, ParamID pid, Interval& ivValid)
{
	MAXSPLICE_CATCH_BEGIN

	int type = pblock->GetParameterType(pid);
	int nParams = 1;
	if (is_tab(type)){
		nParams = pblock->Count(pid);
		//DbgAssert(port->getPortType() isArray());
	}

	TResultType* pVals = new TResultType[nParams];
	for (int i = 0; i < nParams; i++)
		pblock->GetValue(pid, t, pVals[i], ivValid, i);
	MaxValuesToSplice<TResultType, TConvertType>(binding, argName, t, ivValid, pVals, nParams);
	delete pVals;

	MAXSPLICE_CATCH_END
}

template<typename TResultType>
void ParameterBlockValuesToSplice(FabricCore::DFGBinding& binding, const char* argName, TimeValue t, IParamBlock2* pblock, ParamID pid, Interval& ivValid)
{
	ParameterBlockValuesToSplice<TResultType, TResultType>(binding, argName, t, pblock, pid, ivValid);
}

// This helper function converts from INode to the appropriate Splice type
void MaxPtrToSplice(FabricCore::DFGBinding& binding, const char* argName, TimeValue t, IParamBlock2* pblock, ParamID id, Interval& ivValid)
{
	FabricCore::DFGExec exec = binding.getExec();
	if (!exec.haveExecPort(argName))
		return;

	// There is no "INode" type in splice, so try to figure out a conversion
	const char* cType = exec.getExecPortResolvedType(argName);
	int maxTypeRequired = SpliceTypeToMaxType(cType);
	switch(maxTypeRequired)
	{
	case TYPE_POINT3:
		{
			ParameterBlockValuesToSplice<INode*, Point3>(binding, argName, t, pblock, id, ivValid);
			break;
		}
	case TYPE_QUAT:
		{
			ParameterBlockValuesToSplice<INode*, Quat>(binding, argName, t, pblock, id, ivValid);
			break;
		}
	case TYPE_MATRIX3:
		{
			ParameterBlockValuesToSplice<INode*, Matrix3>(binding, argName, t, pblock, id, ivValid);
			break;
		}
	case TYPE_MESH:
		{
			// Convert to mesh if possible
			ParameterBlockValuesToSplice<INode*, Mesh>(binding, argName, t, pblock, id, ivValid);
			break;
		}
	default:
		DbgAssert(false); // what do we have here?
	}
}
// Pblock conversion function
void TransferAllMaxValuesToSplice(TimeValue t, IParamBlock2* pblock, FabricCore::DFGBinding& binding, std::vector<Interval>& paramValids, Interval& ivValid)
{
	if (pblock == NULL)
		return;

	FabricCore::DFGExec exec = binding.getExec();

	// Iterate over all PB2 parameters, send to associated splice port
	unsigned int nPorts = exec.getExecPortCount();
	for (size_t i = 0; i < nPorts; i++)
	{
		const char* argName = exec.getExecPortName(i);
		ParamID pid = ParamID(::GetPortParamID(exec, argName));
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
			ParameterBlockValuesToSplice<int>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_TIMEVALUE:
			ParameterBlockValuesToSplice<TimeValue, float>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_FLOAT:
		case TYPE_ANGLE:
		case TYPE_WORLD:
		case TYPE_PCNT_FRAC:
			ParameterBlockValuesToSplice<float>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_RGBA:
			ParameterBlockValuesToSplice<Color>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		//case TYPE_POINT2:
		//	MaxValueToSplice(port, pblock->GetPoint3(id, t));
		//	break;
		case TYPE_POINT3:
			ParameterBlockValuesToSplice<Point3>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_FRGBA:
			ParameterBlockValuesToSplice<Point4>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_POINT4:
			ParameterBlockValuesToSplice<Point4>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_BOOL:
			ParameterBlockValuesToSplice<int, bool>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_MATRIX3:
			ParameterBlockValuesToSplice<Matrix3>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_STRING:
		case TYPE_FILENAME:
			ParameterBlockValuesToSplice<const MCHAR*>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_INODE:
			{
				MaxPtrToSplice(binding, argName, t, pblock, pid, paramValids[pidx]);
				break;
			}
		case TYPE_REFTARG:
			{
				ReferenceTarget* pSrcContainer = pblock->GetReferenceTarget(pid);
				if (pSrcContainer == nullptr)
					continue;

				// If we have a reftarg type, we may be connected to another graph
				std::string portConnection = GetPortConnection(binding, argName);
				if (portConnection.length() > 0) {
					
					SpliceTranslationFPInterface* pSrcContInterface = static_cast<SpliceTranslationFPInterface*>(pSrcContainer->GetInterface(ISPLICE__INTERFACE));
					if (pSrcContInterface == nullptr)
						continue;

					pSrcContInterface->TriggerEvaluate(t, paramValids[pidx]);
					//DFGWrapper::ExecPortPtr srcPort = pSrcContInterface->GetPort(portConnection.data());
					//if (srcPort)
					//{
					//	FabricCore::RTVal srcVal = srcPort->getArgValue();
					//	//int srcIndex = GetPortConnectionIndex(port);
					//	// If we are connected to an array, take only a single element
					//	//if (srcIndex >= 0 && srcVal.isArray()) {
					//	//	// Check for OOR
					//	//	if ((uint32_t)srcIndex >= srcVal.getArraySize())
					//	//		continue;
					//	//	srcVal = srcVal.getArrayElement(srcIndex);
					//	//}
					//	port->setArgValue(srcVal);
					//}
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

#pragma region Instance counting

static int s_nInstances = 0;
static FabricCore::Client s_client;
static FabricCore::DFGHost s_Host;
static FabricCore::RTVal s_drawing;

FabricCore::Client& GetClient() 
{
	if (!s_client.isValid())
	{
		// create a client
		FabricCore::Client::CreateOptions options;
		memset(&options, 0, sizeof(options));
		options.optimizationType = FabricCore::ClientOptimizationType_Background;
		options.guarded = 1;
		FabricCore::Client::ReportCallback pCallback = &myLogFunc;
		s_client = FabricCore::Client(pCallback, nullptr, &options);
	}
	return s_client;
}

FabricCore::DFGHost& GetHost()
{
	if (!s_Host.isValid())
	{
		FabricCore::Client client = GetClient();
		s_Host = client.getDFGHost();
	}
	return s_Host;
}

FabricCore::RTVal& GetDrawing()
{
	if (!s_drawing.isValid())
	{
		s_drawing = FabricCore::RTVal::Construct(GetClient(), "OGLInlineDrawing", 0, 0);
		if (!s_drawing.isValid())
		{
			//log("[GLWidget] Error: Cannot construct OGLInlineDrawing RTVal (extension loaded?)\n");
		}
		else
			s_drawing = s_drawing.callMethod("OGLInlineDrawing", "getInstance", 0, 0);
	}
	return s_drawing;
}
void InstanceCreated() 
{ 
	s_nInstances++;  
}

void InstanceDeleted() 
{ 
	s_nInstances--;  
	// If there are no instances left, release the client.
	if (s_nInstances == 0)
	{
		ReleaseAll();
	}
}

bool AnyInstances() { return s_nInstances > 0;  }

extern void ReleaseAll()
{
	// Ensure we have no more live classes?
	DbgAssert(s_nInstances == 0);

	// Assume we have no more need for the undo stack...
	GetQtUndoStack()->clear();
	if (s_Host.isValid())
		s_Host.flushUndoRedo();

	// Release our static variables
	s_Host = FabricCore::DFGHost();
	s_client = FabricCore::Client();
	s_drawing = FabricCore::RTVal();
}

#pragma endregion
