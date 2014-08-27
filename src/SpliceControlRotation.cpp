//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "StdAfx.h"
#include "SpliceTranslationLayer.hpp"

#define SpliceControlRotation_CLASS_ID	Class_ID(0x122e256a, 0x6fcb1b17)

class SpliceControlRotation : public SpliceTranslationLayer<Control, Quat> {
public:

	//From Animatable
	Class_ID ClassID() {return SpliceControlRotation_CLASS_ID;}		
	SClass_ID SuperClassID() { return CTRL_ROTATION_CLASS_ID; }
	void GetClassName(TSTR& s) {s = GetString(IDS_SPLICE_ROT_CTRL_CLASS_NAME);}

	void Copy(Control *);
	void GetValue(TimeValue,void *,Interval &,GetSetMethod);
	void SetValue(TimeValue,void *,int,GetSetMethod);

	//Constructor/Destructor
	SpliceControlRotation();
	~SpliceControlRotation();	

private:
	void ResetPorts();

	int GetValueType() { return TYPE_QUAT; }

	bool CloneSpliceData(SpliceTranslationLayer<Control, Quat>* pMyClone) { return true; } ; // No cloning for me...

	FabricSplice::DGPort m_parentValuePort;
};

class SpliceControlRotationClassDesc : public DynPBCustAttrClassDesc {
public:
	SpliceControlRotationClassDesc()
	{
	}
	void *			Create(BOOL /*loading*/ = FALSE) { return new SpliceControlRotation; }
	const MCHAR *	ClassName() { static MSTR s = GetString(IDS_SPLICE_ROT_CTRL_CLASS_NAME); return s.data(); }
	SClass_ID		SuperClassID() { return CTRL_ROTATION_CLASS_ID; }
	Class_ID		ClassID() { return SpliceControlRotation_CLASS_ID; }
	const TCHAR*	InternalName() {return _T("SpliceRotationController");}
};

DynPBCustAttrClassDesc* SpliceTranslationLayer<Control, Quat>::GetClassDesc()
{
	static SpliceControlRotationClassDesc spliceControllerDesc;
	return &spliceControllerDesc; 
}

SpliceControlRotation::SpliceControlRotation()
{
	ResetPorts();
}


SpliceControlRotation::~SpliceControlRotation() 
{
}

void SpliceControlRotation::ResetPorts()
{
	m_parentValuePort = AddSpliceParameter(m_graph, TYPE_MATRIX3, _M("parentValue"), FabricSplice::Port_Mode_IN);
	ParentClass::ResetPorts();
}

void SpliceControlRotation::Copy(Control *)
{

}



void SpliceControlRotation::GetValue(TimeValue t, void *val, Interval &interval, GetSetMethod method)
{
	try
	{
		if(method == CTRL_RELATIVE)
		{
			Matrix3* pInVal = reinterpret_cast<Matrix3*>(val);
			Point3 pos = pInVal->GetTrans();
			MaxValueToSplice(m_parentValuePort, 0, interval, *pInVal);
			const Quat& res = Evaluate(t, interval);
			// To apply the value relatively, we pre-rotate by the result
			Matrix3 tmRot;
			res.MakeMatrix(tmRot);
			*pInVal = *pInVal * tmRot;
			pInVal->SetTrans(pos);
		}
		else
		{
			Quat* pOutVal = reinterpret_cast<Quat*>(val);
			MaxValueToSplice(m_parentValuePort, 0, interval, Matrix3(1));
			*pOutVal = Evaluate(t, interval);
		}

		// We cannot detect validity intervals from KL graphs, 
		// so just set validity to the current instant.
		interval.SetInstant(t);
	}
	catch(FabricCore::Exception e)
	{
		//logError(e.getDesc_cstr());
	}
}



void SpliceControlRotation::SetValue(TimeValue,void *value,int,GetSetMethod)
{
	// TODO:

}