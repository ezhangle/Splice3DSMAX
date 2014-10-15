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

#define SpliceControlMatrix_CLASS_ID	Class_ID(0x6a53772, 0x7c2c7c4b)

class SpliceControlMatrix : public SpliceTranslationLayer<Control, Matrix3> {
public:

	//From Animatable
	Class_ID ClassID() {return SpliceControlMatrix_CLASS_ID;}		
	SClass_ID SuperClassID() { return CTRL_MATRIX3_CLASS_ID; }
	void GetClassName(TSTR& s) {s = GetString(IDS_SPLICE_MAT_CTRL_CLASS_NAME);}

	void Copy(Control *);
	void GetValue(TimeValue,void *,Interval &,GetSetMethod);
	void SetValue(TimeValue,void *,int,GetSetMethod);

	//Constructor/Destructor
	SpliceControlMatrix();
	~SpliceControlMatrix();	

private:

	void ResetPorts();

	int GetValueType() { return TYPE_MATRIX3; }

	bool CloneSpliceData(SpliceTranslationLayer<Control, Matrix3>* pMyClone) { return true; } ; // No cloning for me...

	FabricSplice::DGPort m_parentValuePort;
};

class SpliceControlMatrixClassDesc : public DynPBCustAttrClassDesc {
public:
	SpliceControlMatrixClassDesc()
	{
	}
	void *			Create(BOOL /*loading*/ = FALSE) { return new SpliceControlMatrix; }
	const MCHAR *	ClassName() { static MSTR s = GetString(IDS_SPLICE_MAT_CTRL_CLASS_NAME); return s.data(); }
	SClass_ID		SuperClassID() { return CTRL_MATRIX3_CLASS_ID; }
	Class_ID		ClassID() { return SpliceControlMatrix_CLASS_ID; }
	const TCHAR*	InternalName() {return _T("SpliceMatrixController");}
};

DynPBCustAttrClassDesc* SpliceTranslationLayer<Control, Matrix3>::GetClassDesc()
{
	static SpliceControlMatrixClassDesc spliceControllerDesc;
	return &spliceControllerDesc; 
}

SpliceControlMatrix::SpliceControlMatrix()
{
	ResetPorts();
}


SpliceControlMatrix::~SpliceControlMatrix() 
{
}		

void SpliceControlMatrix::ResetPorts()
{
	// We add a parent value to, so that our Splice operator can evaluate relative to the input matrix
	// This is similar to how standard 3ds Max transforms plugins work
	m_parentValuePort = AddSpliceParameter(m_graph, TYPE_MATRIX3, _M("parentValue"), FabricSplice::Port_Mode_IN);
	ParentClass::ResetPorts();
}

void SpliceControlMatrix::Copy(Control *)
{

}



void SpliceControlMatrix::GetValue(TimeValue t, void *val, Interval &interval, GetSetMethod method)
{
	try
	{
		Matrix3* pInVal = reinterpret_cast<Matrix3*>(val);
		if(method == CTRL_ABSOLUTE)
		{
			MaxValueToSplice(m_parentValuePort, 0, interval, Matrix3::Identity);
		}
		else
		{
			MaxValueToSplice(m_parentValuePort, 0, interval, *pInVal);
		}
		*pInVal = Evaluate(t, interval);
	}
	catch(FabricCore::Exception e)
	{
		logMessage(e.getDesc_cstr());
	}
}



void SpliceControlMatrix::SetValue(TimeValue,void *value,int,GetSetMethod)
{
	// TODO:

}