//**************************************************************************/
// DESCRIPTION: Generates Matrix3 values using Splice
// AUTHOR: Stephen Taylor
//***************************************************************************/

#include "StdAfx.h"
#include "SpliceTranslationLayer.hpp"
#include "Splice3dsmax.h"

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
	Invalidate();
	Matrix3* pInVal = reinterpret_cast<Matrix3*>(val);
	if(method == CTRL_ABSOLUTE)
	{
		MaxValueToSplice(m_parentValuePort, 0, interval, Matrix3::Identity);
	}
	else
	{
		// if our parents value has changed, invalidate our cache
		if (!(m_cachedParentVal == *pInVal))
			Invalidate();
		MaxValueToSplice(m_parentValuePort, 0, interval, *pInVal);
		// Cache parent transform for next eval
		m_cachedParentVal = *pInVal;
	}
	*pInVal = Evaluate(t, interval);

}



void SpliceControlMatrix::SetValue(TimeValue,void *value,int,GetSetMethod)
{
	// TODO:

}