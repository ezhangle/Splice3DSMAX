//**************************************************************************/
// DESCRIPTION: Generates Matrix3 values using Splice
// AUTHOR: Stephen Taylor
//***************************************************************************/

#include "StdAfx.h"
#include "SpliceControl.hpp"
#include "Splice3dsmax.h"

#define SpliceControlMatrix_CLASS_ID	Class_ID(0x6a53772, 0x7c2c7c4b)

class SpliceControlMatrix : public SpliceControl<Matrix3> {
public:

	//From Animatable
	Class_ID ClassID() {return SpliceControlMatrix_CLASS_ID;}		
	SClass_ID SuperClassID() { return CTRL_MATRIX3_CLASS_ID; }
	void GetClassName(TSTR& s) {s = GetString(IDS_SPLICE_MAT_CTRL_CLASS_NAME);}

	void Copy(Control *);
	void GetValue(TimeValue,void *,Interval &,GetSetMethod);
	void SetValue(TimeValue,void *,int,GetSetMethod);

	//Constructor/Destructor
	SpliceControlMatrix(BOOL loading);
	~SpliceControlMatrix();	

private:

	int GetValueType() { return TYPE_MATRIX3; }

	// Our cache is only valid if the parent has not changed.
	// We cache the parents transform, and invalidate our cache
	// if it changes.
	Matrix3 m_cachedParentVal;
};

class SpliceControlMatrixClassDesc : public DynPBCustAttrClassDesc {
public:
	SpliceControlMatrixClassDesc()
	{
	}
	void *			Create(BOOL loading) { return new SpliceControlMatrix(loading); }
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

SpliceControlMatrix::SpliceControlMatrix(BOOL loading)
	: ParentClass(loading)
{
	m_value.IdentityMatrix();
	if (!loading)
		ResetPorts();
}


SpliceControlMatrix::~SpliceControlMatrix() 
{
}		

void SpliceControlMatrix::Copy(Control *from)
{
	m_value.IdentityMatrix();
	TimeValue t = GetCOREInterface()->GetTime();
	Interval iv;
	from->GetValue(t, &m_value, iv, CTRL_RELATIVE);

}

void SpliceControlMatrix::GetValue(TimeValue t, void *val, Interval &interval, GetSetMethod method)
{
	Invalidate();
	Matrix3* pInVal = reinterpret_cast<Matrix3*>(val);
	if(method == CTRL_ABSOLUTE)
	{
		MaxValueToSplice(m_binding, m_parentArgName.c_str(), t, interval, Matrix3::Identity);
	}
	else
	{
		// if our parents value has changed, invalidate our cache
		if (!(m_cachedParentVal == *pInVal))
			Invalidate();
		MaxValueToSplice(m_binding, m_parentArgName.c_str(), t, interval, *pInVal);
		// Cache parent transform for next eval
		m_cachedParentVal = *pInVal;
	}
	*pInVal = Evaluate(t, interval);

}



void SpliceControlMatrix::SetValue(TimeValue,void *value,int,GetSetMethod)
{
	// Cache input val, we will return this value until our graph evaluates
	// This code allows us to assign the SpliceMatrix to a node, and it
	// will hold it's current transform until a graph is assigned.
	SetXFormPacket* packet = reinterpret_cast<SetXFormPacket*>(value);
	if (packet->command == XFORM_SET)
	{
		m_value = *(Matrix3*)value;
		Invalidate();
	}
}