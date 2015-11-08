//**************************************************************************/
// DESCRIPTION: Generates rotation values using Splice
// AUTHOR: Stephen Taylor
//***************************************************************************/

#include "StdAfx.h"
#include "SpliceControl.hpp"

#define SpliceControlRotation_CLASS_ID	Class_ID(0x122e256a, 0x6fcb1b17)

class SpliceControlRotation : public SpliceControl<Quat> {
public:

	//From Animatable
	Class_ID ClassID() {return SpliceControlRotation_CLASS_ID;}		
	SClass_ID SuperClassID() { return CTRL_ROTATION_CLASS_ID; }
	void GetClassName(TSTR& s) {s = GetString(IDS_SPLICE_ROT_CTRL_CLASS_NAME);}

	void Copy(Control *);
	void GetValue(TimeValue,void *,Interval &,GetSetMethod);
	void SetValue(TimeValue,void *,int,GetSetMethod);

	//Constructor/Destructor
	SpliceControlRotation(BOOL loading);
	~SpliceControlRotation();	

private:

	int GetValueType() { return TYPE_QUAT; }
};

class SpliceControlRotationClassDesc : public DynPBCustAttrClassDesc {
public:
	SpliceControlRotationClassDesc()
	{
	}
	void *			Create(BOOL loading) { return new SpliceControlRotation(loading); }
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

SpliceControlRotation::SpliceControlRotation(BOOL loading)
	: ParentClass(loading)
{
	ResetPorts();
}


SpliceControlRotation::~SpliceControlRotation() 
{
}


void SpliceControlRotation::Copy(Control *)
{

}

void SpliceControlRotation::GetValue(TimeValue t, void *val, Interval &interval, GetSetMethod method)
{
	if(method == CTRL_RELATIVE)
	{
		Invalidate(); // Evaluate every time in case parent changes too
		Matrix3* pInVal = reinterpret_cast<Matrix3*>(val);
		Point3 pos = pInVal->GetTrans();
		MaxValueToSplice(m_binding, m_parentArgName.c_str(), 0, interval, *pInVal);
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
		MaxValueToSplice(m_binding, m_parentArgName.c_str(), 0, interval, Matrix3(1));
		*pOutVal = Evaluate(t, interval);
	}

	// We cannot detect validity intervals from KL graphs, 
	// so just set validity to the current instant.
	interval.SetInstant(t);
}



void SpliceControlRotation::SetValue(TimeValue,void *value,int,GetSetMethod)
{
	// TODO:

}