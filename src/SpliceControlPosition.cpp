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

#define SpliceControlPosition_CLASS_ID	Class_ID(0x34952088, 0x7d9a01d7)

class SpliceControlPosition : public SpliceTranslationLayer<Control, Point3> {
public:

	//From Animatable
	Class_ID ClassID() {return SpliceControlPosition_CLASS_ID;}		
	SClass_ID SuperClassID() { return CTRL_POSITION_CLASS_ID; }
	void GetClassName(TSTR& s) {s = GetString(IDS_SPLICE_POS_CTRL_CLASS_NAME);}

	void Copy(Control *);
	void GetValue(TimeValue,void *,Interval &,GetSetMethod);
	void SetValue(TimeValue,void *,int,GetSetMethod);

	//Constructor/Destructor
	SpliceControlPosition(BOOL loading);
	~SpliceControlPosition();	

private:

	void ResetPorts();

	int GetValueType() { return TYPE_POINT3; }

	bool CloneSpliceData(SpliceTranslationLayer<Control, Point3>* pMyClone) { return true; } ; // No cloning for me...

	FabricSplice::DGPort m_parentValuePort;
};

class SpliceControlPositionClassDesc : public DynPBCustAttrClassDesc {
public:
	SpliceControlPositionClassDesc()
	{
	}
	void *			Create(BOOL loading = FALSE) { return new SpliceControlPosition(loading); }
	const MCHAR *	ClassName() { static MSTR s = GetString(IDS_SPLICE_POS_CTRL_CLASS_NAME); return s.data(); }
	SClass_ID		SuperClassID() { return CTRL_POSITION_CLASS_ID; }
	Class_ID		ClassID() { return SpliceControlPosition_CLASS_ID; }
	const TCHAR*	InternalName() {return _T("SplicePositionController");}
};

DynPBCustAttrClassDesc* SpliceTranslationLayer<Control, Point3>::GetClassDesc()
{
	static SpliceControlPositionClassDesc spliceControllerDesc;
	return &spliceControllerDesc; 
}

SpliceControlPosition::SpliceControlPosition(BOOL loading)
{
	ResetPorts();
}


SpliceControlPosition::~SpliceControlPosition() 
{
}		

void SpliceControlPosition::ResetPorts()
{
	m_parentValuePort = AddSpliceParameter(m_graph, TYPE_MATRIX3, _M("parentValue"), FabricSplice::Port_Mode_IN);
	ParentClass::ResetPorts();
}

void SpliceControlPosition::Copy(Control *)
{

}

void SpliceControlPosition::GetValue(TimeValue t, void *val, Interval &interval, GetSetMethod method)
{
	try
	{
		if(method == CTRL_RELATIVE)
		{
			Matrix3* pInVal = reinterpret_cast<Matrix3*>(val);
			MaxValueToSplice(m_parentValuePort, 0, interval, *pInVal);
			pInVal->SetTrans(Evaluate(t, interval));
		}
		else
		{
			Point3* pOutVal = reinterpret_cast<Point3*>(val);
			MaxValueToSplice(m_parentValuePort, 0, interval, Matrix3::Identity);
			*pOutVal = Evaluate(t, interval);
		}
	}
	catch(FabricCore::Exception e)
	{
		logMessage(e.getDesc_cstr());
	}
}



void SpliceControlPosition::SetValue(TimeValue,void *value,int,GetSetMethod)
{
	// TODO:

}