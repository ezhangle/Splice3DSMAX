#include "StdAfx.h"
#include "MeshNormalSpec.h"
#include "ParameterBlockPatcher.h"

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

	TimeValue t = GetCOREInterface()->GetTime();
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
			switch ((int)base_type(type))
			{
			case TYPE_FLOAT:
			case TYPE_ANGLE:
			case TYPE_PCNT_FRAC:
			case TYPE_WORLD:
				pNewBlock->SetValue(newId, t, pCopyBlock->GetFloat(oldId, t, i), i);
				break;
			case TYPE_BOOL:
				pNewBlock->SetValue(newId, t, pCopyBlock->GetInt(oldId, t, i), i);
				break;
			case TYPE_INT:
				pNewBlock->SetValue(newId, t, pCopyBlock->GetInt(oldId, t, i), i);
				break;
			case TYPE_POINT2:
			{
				Point2 v;
				Interval iv;
				GetPB2Value( pCopyBlock, oldId, 0, v, iv, i );
				SetPB2Value( pNewBlock, newId, 0, v, i );
				break;
			}
			break;
			case TYPE_POINT3:
				{
					Point3 v = pCopyBlock->GetPoint3(oldId, t, i);
					pNewBlock->SetValue(newId, t, v, i);
				}
				break;
			case TYPE_FRGBA:
			case TYPE_POINT4:
				{
					Point4 v = pCopyBlock->GetPoint4(oldId, t, i);
					pNewBlock->SetValue(newId, t, v, t);
				}
				break;
			case TYPE_MTL:
				pNewBlock->SetValue(newId, t, pCopyBlock->GetMtl(oldId, t, i), i);
				break;
			case TYPE_TEXMAP:
				pNewBlock->SetValue(newId, t, pCopyBlock->GetTexmap(oldId, t, i), i);
				break;
			case TYPE_MATRIX3:
				{
					Matrix3 v = pCopyBlock->GetMatrix3(oldId, t, i);
					pNewBlock->SetValue(newId, t, v, i);
					break;
				}
			case TYPE_INODE:
				pNewBlock->SetValue(newId, t, pCopyBlock->GetINode(oldId, t, i), i);
				break;
			case TYPE_STRING:
				{
					CONST_2010 MCHAR* theString = pCopyBlock->GetStr(oldId, t, i);
					if (theString != NULL) 
						pNewBlock->SetValue(newId, t, theString, i);
				}
				break;
			case TYPE_RGBA:
				{
					Color c = pCopyBlock->GetColor(oldId, t, i);
					pNewBlock->SetValue(newId, t, c, i);
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
						break;
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
		if (is_tab( type ))
			pDesc->ParamOption( pid, p_ui, TYPE_FLOATLISTBOX, 0, 0, 0, 0, EDITTYPE_FLOAT, 0, 0, SPIN_AUTOSCALE, p_end );
		else
			pDesc->ParamOption( pid, p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, 0, 0, SPIN_AUTOSCALE, p_end );

		pDesc->ParamOption(pid, p_range, -99999999.0, 99999999.0, p_end); 
		break;
	case TYPE_INT:		
		if (is_tab( type ))
			pDesc->ParamOption( pid, p_ui, TYPE_INTLISTBOX, 0, 0, 0, 0, EDITTYPE_INT, 0, 0, SPIN_AUTOSCALE, p_end );
		else
			pDesc->ParamOption( pid, p_ui, TYPE_SPINNER, EDITTYPE_INT, 0, 0, SPIN_AUTOSCALE, p_end );
		pDesc->ParamOption(pid, p_range, -99999999, 99999999, p_end); 
		break;
	case TYPE_POINT2:
		pDesc->ParamOption( pid, p_ui, TYPE_SPINNER, EDITTYPE_UNIVERSE, 0, 0, 0, 0, SPIN_AUTOSCALE, p_end );
		pDesc->ParamOption( pid, p_range, -99999999.0, 99999999.0, p_end );
		break;
	case TYPE_POINT3:	
		pDesc->ParamOption(pid, p_ui, TYPE_SPINNER, EDITTYPE_UNIVERSE, 0, 0, 0, 0, 0, 0, SPIN_AUTOSCALE, p_end); 
		pDesc->ParamOption(pid, p_range, -99999999.0, 99999999.0, p_end); 
		break;
	case TYPE_POINT4:	
		pDesc->ParamOption(pid, p_ui, TYPE_SPINNER, EDITTYPE_UNIVERSE, 0, 0, 0, 0, 0, 0, 0, 0, SPIN_AUTOSCALE, p_end); 
		pDesc->ParamOption(pid, p_range, -99999999.0, 99999999.0, p_end); 
		break;
	case TYPE_RGBA:		pDesc->ParamOption( pid, p_ui, TYPE_COLORSWATCH, 0, p_end ); break;
	case TYPE_FRGBA:	pDesc->ParamOption( pid, p_ui, TYPE_COLORSWATCH_FRGBA, 0, p_end ); break;
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
	if (!value.isValid())
		return;

	TType def;
	if (value.isArray())
	{
		int count = value.getArraySize();
		pblock->SetCount( pid, count );

		for (int i = 0; i < count; i++)
		{
			FabricCore::RTVal arrValue = value.getArrayElementRef( i );
			FabricToMaxValue( arrValue, def );
			SetPB2Value( pblock, pid, t, def, i );
		}
	}
	else
	{
		FabricToMaxValue( value, def );
		SetPB2Value( pblock, pid, t, def, 0 );
	}
}

void SetMaxParamFromFabric(IParamBlock2* pblock, ParamID pid, FabricCore::DFGBinding& binding, const char* argName)
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
	case TYPE_POINT2:
	{
		SetMaxParamValue<Point2>( pblock, t, pid, currentVal );
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

#define LISTBOX_HEIGHT 90


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
	if( dialog->AddControl( WS_CHILD | WS_VISIBLE, 0, PARAM_X - 8, ypos, 8, CTRL_HEIGHT, ctrlId, NULL, SPINNERWINDOWCLASS ) )
	{
		paramCtrl[1] = ctrlId++;
		// And now the editbox.
		return GenerateParamUI(dialog, ypos, ctrlId, paramCtrl[0], CTRL_WIDTH - 16, CUSTEDITWINDOWCLASS );
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
		case TYPE_POINT2:
				dlgSize += PARAM_Y * 2 * count;
				break;
		case TYPE_POINT3:
			dlgSize += PARAM_Y * 3 * count;
			break;
		case TYPE_POINT4:
			dlgSize += PARAM_Y * 4 * count;
			break;
		default:
			if (is_tab( paramType ))
				dlgSize += (PARAM_Y * 2) + LISTBOX_HEIGHT;
			else 
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

		// Skip empty/unsupported types
		if (pbDef.ctrl_count == 0)
			continue;

		// Post the name (in unicode)
		const MCHAR* pName = pbDef.int_name;

		dialogTemplate->AddStaticControl(WS_VISIBLE | WS_CHILD | WS_GROUP, 0, DLG_PAD / 2, ypos, LBL_WIDTH, CTRL_HEIGHT, -1, pName);

		switch ((int)pbDef.type)
		{
			case TYPE_FLOAT:
			case TYPE_ANGLE:
			case TYPE_PCNT_FRAC:
			case TYPE_WORLD:
			case TYPE_INT:
			case TYPE_INDEX:
			case TYPE_TIMEVALUE:
				GenerateParamSpinner( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs );
				break;
			case TYPE_STRING:    GenerateParamUI( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, CUSTEDITWINDOWCLASS ); break;
			case TYPE_BOOL:        GenerateParamUI( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, L"Button", BS_AUTOCHECKBOX | WS_TABSTOP | WS_VISIBLE | WS_CHILD); break;
			case TYPE_MTL:        GenerateParamUI( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, CUSTBUTTONWINDOWCLASS ); break;
			case TYPE_TEXMAP:    GenerateParamUI( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, CUSTBUTTONWINDOWCLASS ); break;
			case TYPE_INODE:    GenerateParamUI( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, CUSTBUTTONWINDOWCLASS ); break;
			case TYPE_RGBA:        GenerateParamUI( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, COLORSWATCHWINDOWCLASS ); break;
			case TYPE_FRGBA:    GenerateParamUI( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs[0], CTRL_WIDTH, COLORSWATCHWINDOWCLASS ); break;
			case TYPE_POINT2:
				GenerateParamSpinner( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs );
				GenerateParamSpinner( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs + 2 );
				break;
			case TYPE_POINT3:
				GenerateParamSpinner( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs );
				GenerateParamSpinner( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs + 2 );
				GenerateParamSpinner( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs + 4 );
				break;
			case TYPE_POINT4:
				GenerateParamSpinner( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs );
				GenerateParamSpinner( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs + 2 );
				GenerateParamSpinner( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs + 4 );
				GenerateParamSpinner( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs + 6 );
				break;
			case TYPE_FLOAT_TAB:
			case TYPE_ANGLE_TAB:
			case TYPE_INT_TAB:
			{
				ypos += CTRL_HEIGHT;
				// Add in the actual list box
				dialogTemplate->AddControl( (WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL),
									0,
									DLG_PAD,
									ypos,
									DLG_X - (DLG_PAD * 2),
									LISTBOX_HEIGHT,
									ctrlId,
									NULL,
									_M("LISTBOX") );
				pbDef.ctrl_IDs[0] = ctrlId++;

				ypos += LISTBOX_HEIGHT - CTRL_HEIGHT;

				// Add in 3 buttons
				short btnWidth = (DLG_X - DLG_PAD) / 3;
				short btnXPos = (DLG_PAD / 2);
				dialogTemplate->AddControl( (WS_TABSTOP | WS_CHILD | WS_VISIBLE),
									0,
									btnXPos,
									ypos,
									btnWidth,
									CTRL_HEIGHT,
									ctrlId,
									_M("Add"),
									CUSTBUTTONWINDOWCLASS );
				pbDef.ctrl_IDs[1] = ctrlId++;
				btnXPos += btnWidth;
				dialogTemplate->AddControl( (WS_TABSTOP | WS_CHILD | WS_VISIBLE),
									0,
									btnXPos,
									ypos,
									btnWidth,
									CTRL_HEIGHT,
									ctrlId,
									_M( "Swp" ),
									CUSTBUTTONWINDOWCLASS );
				pbDef.ctrl_IDs[2] = ctrlId++;
				btnXPos += btnWidth;
				dialogTemplate->AddControl( (WS_TABSTOP | WS_CHILD | WS_VISIBLE),
									0,
									btnXPos,
									ypos,
									btnWidth,
									CTRL_HEIGHT,
									ctrlId,
									_M( "Del" ),
									CUSTBUTTONWINDOWCLASS );
				pbDef.ctrl_IDs[3] = ctrlId++;
				ypos += PARAM_Y;

				dialogTemplate->AddStaticControl( WS_VISIBLE | WS_CHILD | WS_GROUP, 0, DLG_PAD / 2, ypos, LBL_WIDTH, CTRL_HEIGHT, -1, _M("Swap Value") );
				GenerateParamSpinner( dialogTemplate, ypos, ctrlId, pbDef.ctrl_IDs + 4 );
				break;
			}
			default:
				ypos += PARAM_Y;
				break;
		}
	}
	return dialogTemplate;
}

// Add a parameter to the splice graph
std::string AddFabricParameter(FabricTranslationFPInterface* pOwner, int type, const MCHAR* pName, FabricCore::DFGPortType mode, const char* inExtension, const char* metadata)
{
	return AddFabricParameter(pOwner, type, CStr::FromMCHAR(pName).data(), mode, inExtension, metadata);
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

std::string GetPortConnection( FabricCore::DFGExec exec, const char* argName )
{
	return exec.getExecPortMetadata( argName, MAX_SRC_OPT );
}
std::string GetPortConnection(FabricCore::DFGBinding& binding, const char* argName) 
{
	return GetPortConnection( binding.getExec(), argName);
}

void SetPortConnection(FabricTranslationFPInterface* pOwner, const char* argName, const char* name)
{
	pOwner->SetPortMetaData(argName, MAX_SRC_OPT, name, "");
}

bool GetPortPostConnectionUI(FabricTranslationFPInterface* pOwner, const char* argName)
{
	const char* res = pOwner->GetPortMetaData(argName, MAX_POST_UI_OPT);
	return strcmp(res, "0") == 0 ? false : true;
}

void SetPortPostConnectionUI(FabricTranslationFPInterface* pOwner, const char* argName, bool postUI)
{
	pOwner->SetPortMetaData(argName, MAX_POST_UI_OPT, postUI ? "1" : "0", "");
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
	// If the port is hidden, then we have no UI (and no Max port) for it
	const char* hidden = const_cast<FabricCore::DFGExec&>(exec).getExecPortMetadata(argName, FABRIC_UI_HIDDEN);
	if (hidden != nullptr && strcmp(hidden, "true") == 0)
		return -1;

	const char* idstr = const_cast<FabricCore::DFGExec&>(exec).getExecPortMetadata(argName, MAX_PARM_TYPE_OPT);
	int maxType = -2;
	if (idstr != nullptr && idstr[0] != '\0') {
		maxType = atoi(idstr);
	}

	// A connected port is assumed always valid 
	// (we assume the connection has been made to a valid source)
	if (maxType == TYPE_REFTARG && !GetPortConnection( exec, argName ).empty())
		return maxType;


	const char* portType = GetPortType(exec, argName);
	if (maxType > 0)
	{
		// If our type is not legal for our current port type,
		// reset it to the default
		BitArray legalTypes = FabricTypeToMaxTypes(portType);
		if (!legalTypes[maxType])
			maxType = -2;
	}
	if (maxType == -2)
		maxType = FabricTypeToDefaultMaxType(portType);

	return maxType;
}

bool SetPortValue(FabricCore::DFGBinding& binding, const char* argName, FPValue* value)
{
	MAXSPLICE_CATCH_BEGIN
	if (value == nullptr)
		return false;

	Interval iv;
	TimeValue t = GetCOREInterface()->GetTime();
	MaxValueToFabric(binding, argName, t, iv, *value);
	return true;
#pragma message("TEST THIS")
	MAXSPLICE_CATCH_END
	return false;
}

//////////////////////////////////////////////////////////////////////////
// Converting types to/from Fabric

static const std::string _array_denom("Array");
static const std::string _brackets_denom( "[]" );
int is_array(const std::string& value)
{
	if (_brackets_denom.size() > value.size())
		return 0;
	if (std::equal( _brackets_denom.rbegin(), _brackets_denom.rend(), value.rbegin() ))
		return _brackets_denom.size();

	if (_array_denom.size() > value.size())
		return 0;
	if (std::equal(_array_denom.rbegin(), _array_denom.rend(), value.rbegin()))
		return _array_denom.size();

	return 0;
}

bool IsPortArray(const FabricCore::DFGExec& exec, const char* argName)
{
	return is_array(GetPortType(exec, argName));
}

bool AreTypesCompatible(int type1, int type2) {
	switch (type1)
	{
	case TYPE_FLOAT:
	case TYPE_ANGLE:
	case TYPE_PCNT_FRAC:
	case TYPE_WORLD:
	{
		switch (type2)
		{
			case TYPE_FLOAT:
			case TYPE_ANGLE:
			case TYPE_PCNT_FRAC:
			case TYPE_WORLD:
			{
				return TRUE;
				break;
			}
			default:
			{
				return False;
				break;
			}
		}
		break;
	}
		default:
		{
			return False;
			break;
		}
	}

}


// If the 'value' string is an array type, remove the Array
// denominator and return true, else false;
inline bool sub_array(std::string& type)
{
	int trim_count = is_array( type );
	if (trim_count) {
		type.resize(type.size() - trim_count );
		return true;
	}
	return false;
}

BitArray FabricTypeToMaxTypes(const char* cType)
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
		sType == "Index" ||
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
	else if (sType == "Vec2")
	{
		res.Set( TYPE_POINT2 + arr_mod );
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

int FabricTypeToMaxType(const char* cType)
{
	std::string sType = cType;
	bool isArray = sub_array(sType);
	int res = -1;
	// Max only supports 1 type of int, boring old SInt32
	// We'll accept the rest though, and hope we don't 
	// overflow anywhere
	if (sType == "Integer" || 
		sType == "Size" || 
		sType == "Index" || 
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
	else if (sType == "RGB")
		res = TYPE_RGBA;
	else if (sType == "Color" ||
			 sType == "RGBA")
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
		// DbgAssert(0 && "NOTE: Add Default translation Type for this Fabric Type");
	}

	if (res >= 0 && isArray)
		res |= TYPE_TAB;

	return res;
}

// This function is simply here to override the default PB2 type for PolygonMesh.
// The correct type for PolyMesh is TYPE_MESH, but unfortunately the PB2
// doesn't support that data, so we need it to create an INODE parameter instead.
int FabricTypeToDefaultMaxType(const char* cType)
{
	if (cType == nullptr)
		return -1;

	if (strcmp(cType, "PolygonMesh") == 0)
		return TYPE_INODE;
	return FabricTypeToMaxType(cType);
}

std::string AddFabricParameter(FabricTranslationFPInterface* pOwner, const char* type, const char* cName, FabricCore::DFGPortType mode, const char* inExtension, const char* metadata)
{
	try
	{
		std::string res = pOwner->GetCommandHandler()->dfgDoAddPort(pOwner->GetBinding(), "", pOwner->GetBinding().getExec(), cName, mode, type, "", inExtension, metadata).toStdString();
		return cName; // r3d shouldn't return res?
	}
	catch(FabricCore::Exception e) 
	{
		CStr message = "ERROR on AddPort to Fabric: ";
		logMessage(message + e.getDesc_cstr());
		return nullptr;
	}
}

std::string AddFabricParameter(FabricTranslationFPInterface* pOwner, int type, const char* cName, FabricCore::DFGPortType mode, const char* inExtension, const char* metadata)
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

	return AddFabricParameter(pOwner, strType.data(), cName, mode, inExtension, metadata);
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
			FabricTranslationFPInterface* curInstance = reinterpret_cast<FabricTranslationFPInterface*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
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
				
				FilterList filterList;
				filterList.Append(_M("Canvas files(*.canvas)"));
				filterList.Append(_M("*.canvas"));

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
				filename += _M(".canvas");

				FilterList filterList;
				filterList.Append(_M("Canvas files(*.canvas)"));
				filterList.Append(_M("*.canvas"));

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
#pragma region MaxToFabric conversion functions

//////////////////////////////////////////////////////////////////////////
template<typename TResultType, typename TConvertType>
void ParameterBlockValuesToFabric(FabricCore::DFGBinding& binding, const char* argName, TimeValue t, IParamBlock2* pblock, ParamID pid, Interval& ivValid)
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
		GetPB2Value( pblock, pid, t, pVals[i], ivValid, i);
	MaxValuesToFabric<TResultType, TConvertType>(binding, argName, t, ivValid, pVals, nParams);
	delete pVals;

	MAXSPLICE_CATCH_END
}

template<typename TResultType>
void ParameterBlockValuesToFabric(FabricCore::DFGBinding& binding, const char* argName, TimeValue t, IParamBlock2* pblock, ParamID pid, Interval& ivValid)
{
	ParameterBlockValuesToFabric<TResultType, TResultType>(binding, argName, t, pblock, pid, ivValid);
}

// This helper function converts from INode to the appropriate Fabric type
void MaxPtrToFabric(FabricCore::DFGBinding& binding, const char* argName, TimeValue t, IParamBlock2* pblock, ParamID id, Interval& ivValid)
{
	FabricCore::DFGExec exec = binding.getExec();
	if (!exec.haveExecPort(argName))
		return;

	// There is no "INode" type in splice, so try to figure out a conversion
	const char* cType = exec.getExecPortResolvedType(argName);
	int maxTypeRequired = FabricTypeToMaxType(cType);
	switch(maxTypeRequired)
	{
	case TYPE_POINT3:
		{
			ParameterBlockValuesToFabric<INode*, Point3>(binding, argName, t, pblock, id, ivValid);
			break;
		}
	case TYPE_QUAT:
		{
			ParameterBlockValuesToFabric<INode*, Quat>(binding, argName, t, pblock, id, ivValid);
			break;
		}
	case TYPE_MATRIX3:
		{
			ParameterBlockValuesToFabric<INode*, Matrix3>(binding, argName, t, pblock, id, ivValid);
			break;
		}
	case TYPE_MESH:
		{
			// Convert to mesh if possible
			ParameterBlockValuesToFabric<INode*, Mesh>(binding, argName, t, pblock, id, ivValid);
			break;
		}
	default:
		DbgAssert(false); // what do we have here?
	}
}
// Pblock conversion function
void TransferAllMaxValuesToFabric(TimeValue t, IParamBlock2* pblock, FabricCore::DFGBinding& binding, std::vector<Interval>& paramValids, Interval& ivValid)
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

#pragma message("TODO: Figure out how to match Max's caching with Fabrics")
		// Has this parameter already been translated?  Test 
		// the validity of the value currently in Fabric
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
			ParameterBlockValuesToFabric<int>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_TIMEVALUE:
			ParameterBlockValuesToFabric<TimeValue, float>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_FLOAT:
		case TYPE_ANGLE:
		case TYPE_WORLD:
		case TYPE_PCNT_FRAC:
			ParameterBlockValuesToFabric<float>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_RGBA:
			ParameterBlockValuesToFabric<Color>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_POINT2:
			ParameterBlockValuesToFabric<Point2>( binding, argName, t, pblock, pid, paramValids[pidx] );
			break;
		case TYPE_POINT3:
			ParameterBlockValuesToFabric<Point3>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_FRGBA:
			ParameterBlockValuesToFabric<Point4>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_POINT4:
			ParameterBlockValuesToFabric<Point4>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_BOOL:
			ParameterBlockValuesToFabric<int, bool>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_MATRIX3:
			ParameterBlockValuesToFabric<Matrix3>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_STRING:
		case TYPE_FILENAME:
			ParameterBlockValuesToFabric<const MCHAR*>(binding, argName, t, pblock, pid, paramValids[pidx]);
			break;
		case TYPE_INODE:
			{
				MaxPtrToFabric(binding, argName, t, pblock, pid, paramValids[pidx]);
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
					
					FabricTranslationFPInterface* pSrcContInterface = static_cast<FabricTranslationFPInterface*>(pSrcContainer->GetInterface(ISPLICE__INTERFACE));
					if (pSrcContInterface == nullptr)
						continue;

					pSrcContInterface->TriggerEvaluate(t, paramValids[pidx]);
					if (pSrcContInterface->HasSrcPort(portConnection.data()))
					{
						FabricCore::RTVal srcVal = pSrcContInterface->GetBinding().getArgValue(portConnection.data());
						binding.setArgValue(argName, srcVal, false);
					}
				}
				else 
				{
					// TODO
					//MaxPtrToFabric(port, t, )
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

FabricCore::Client& GetClient(bool doCreate/*=true*/, const char* contexId) 
{
	if (!s_client.isValid() && doCreate)
	{
		FabricCore::Client::ReportCallback pCallback = &myLogFunc;
		if (contexId != nullptr)
		{
			s_client = FabricCore::Client(pCallback, nullptr, contexId);
		}
		else
		{
			// create a client
			FabricCore::Client::CreateOptions options;
			memset(&options, 0, sizeof(options));
			options.optimizationType = FabricCore::ClientOptimizationType_Background;
			options.guarded = 1;

      // Make sure we don't consume a interactive license if we are render node
      options.licenseType = GetCOREInterface()->IsNetworkLicense() ?
        FabricCore::ClientLicenseType_Compute :
        FabricCore::ClientLicenseType_Interactive;

			s_client = FabricCore::Client(pCallback, nullptr, &options);
		}
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

#ifdef _DEBUG
std::list<ReferenceTarget*> s_instances;
#endif
void InstanceCreated( ReferenceTarget* instance )
{ 
#ifdef _DEBUG
	s_instances.push_back( instance );
#endif
	s_nInstances++;  
}

void InstanceDeleted( ReferenceTarget* instance)
{
#ifdef _DEBUG
	for (auto itr = s_instances.begin(); itr != s_instances.end(); itr++)
	{
		if (*itr == instance)
			*itr = nullptr;
	}
#endif

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
