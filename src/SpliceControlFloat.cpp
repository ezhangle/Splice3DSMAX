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

#define SpliceControlFloat_CLASS_ID	Class_ID(0x501d4dc2, 0x243979cd)

class SpliceControlFloat : public SpliceTranslationLayer<Control, float> {
public:

	//From Animatable
	Class_ID ClassID() {return SpliceControlFloat_CLASS_ID;}		
	SClass_ID SuperClassID() { return CTRL_FLOAT_CLASS_ID; }
	void GetClassName(TSTR& s) {s = GetString(IDS_SPLICE_FLT_CTRL_CLASS_NAME);}

	void Copy(Control *);
	void GetValue(TimeValue,void *,Interval &,GetSetMethod);
	void SetValue(TimeValue,void *,int,GetSetMethod);

	//Constructor/Destructor
	SpliceControlFloat();
	~SpliceControlFloat();	

private:

	void ResetPorts();

	int GetValueType() { return TYPE_FLOAT; }

	bool CloneSpliceData(SpliceTranslationLayer<Control, float>* pMyClone) { return true; } ; // No cloning for me...

	FabricSplice::DGPort m_parentValuePort;
};

class SpliceControlFloatClassDesc : public DynPBCustAttrClassDesc {
public:
	SpliceControlFloatClassDesc()
	{
	}
	void *			Create(BOOL /*loading*/ = FALSE) { return new SpliceControlFloat; }
	const MCHAR *	ClassName() { static MSTR s = GetString(IDS_SPLICE_FLT_CTRL_CLASS_NAME); return s.data(); }
	SClass_ID		SuperClassID() { return CTRL_FLOAT_CLASS_ID; }
	Class_ID		ClassID() { return SpliceControlFloat_CLASS_ID; }
	const TCHAR*	InternalName() {return _T("SpliceFloatController");}
};

DynPBCustAttrClassDesc* SpliceTranslationLayer<Control, float>::GetClassDesc()
{
	static SpliceControlFloatClassDesc spliceControllerDesc;
	return &spliceControllerDesc; 
}

SpliceControlFloat::SpliceControlFloat()
{
	ResetPorts();
}


SpliceControlFloat::~SpliceControlFloat() 
{
}		

void SpliceControlFloat::ResetPorts()
{
	m_parentValuePort = AddSpliceParameter(m_graph, TYPE_FLOAT, _M("parentValue"), FabricSplice::Port_Mode_IN);
	ParentClass::ResetPorts();
}

void SpliceControlFloat::Copy(Control *)
{

}

void SpliceControlFloat::GetValue(TimeValue t, void *val, Interval &interval, GetSetMethod method)
{
	try
	{
		float* pVal = reinterpret_cast<float*>(val);
		if(method == CTRL_RELATIVE)
		{
			MaxValueToSplice(m_parentValuePort, 0, interval, *pVal);
		}
		else
		{
			MaxValueToSplice(m_parentValuePort, 0, interval, float(0));
		}

		// Evaluate value from splice
		*pVal = Evaluate(t, interval);
	}
	catch(FabricCore::Exception e)
	{
		//logError(e.getDesc_cstr());
	}
}



void SpliceControlFloat::SetValue(TimeValue,void *value,int,GetSetMethod)
{
	// TODO:

}