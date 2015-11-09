//**************************************************************************/
// DESCRIPTION: Generates rotation values using Fabric
// AUTHOR: Stephen Taylor
//***************************************************************************/

#include "StdAfx.h"
#include "FabricControl.hpp"

#define FabricControlRotation_CLASS_ID	Class_ID(0x122e256a, 0x6fcb1b17)

class FabricControlRotation : public FabricControl<Quat> {
public:

	//From Animatable
	Class_ID ClassID() {return FabricControlRotation_CLASS_ID;}		
	SClass_ID SuperClassID() { return CTRL_ROTATION_CLASS_ID; }
	void GetClassName(TSTR& s) {s = GetString(IDS_SPLICE_ROT_CTRL_CLASS_NAME);}

	void Copy(Control *);
	void GetValue(TimeValue,void *,Interval &,GetSetMethod);
	void SetValue(TimeValue,void *,int,GetSetMethod);

	//Constructor/Destructor
	FabricControlRotation(BOOL loading);
	~FabricControlRotation();	

private:

	int GetValueType() { return TYPE_QUAT; }
	int GetParentValueType() override { return TYPE_MATRIX3; }
};

class FabricControlRotationClassDesc : public DynPBCustAttrClassDesc {
public:
	FabricControlRotationClassDesc()
	{
	}
	void *			Create(BOOL loading) { return new FabricControlRotation(loading); }
	const MCHAR *	ClassName() { static MSTR s = GetString(IDS_SPLICE_ROT_CTRL_CLASS_NAME); return s.data(); }
	SClass_ID		SuperClassID() { return CTRL_ROTATION_CLASS_ID; }
	Class_ID		ClassID() { return FabricControlRotation_CLASS_ID; }
	const TCHAR*	InternalName() {return _T("FabricRotationController");}
};

DynPBCustAttrClassDesc* FabricTranslationLayer<Control, Quat>::GetClassDesc()
{
	static FabricControlRotationClassDesc spliceControllerDesc;
	return &spliceControllerDesc; 
}

FabricControlRotation::FabricControlRotation(BOOL loading)
	: ParentClass(loading)
{
}


FabricControlRotation::~FabricControlRotation() 
{
}


void FabricControlRotation::Copy(Control *)
{

}

void FabricControlRotation::GetValue(TimeValue t, void *val, Interval &interval, GetSetMethod method)
{
	HoldSuspend hs(); // Prevents us from creating undo objects when setting values to Fabric
	if(method == CTRL_RELATIVE)
	{
		Invalidate(); // Evaluate every time in case parent changes too
		Matrix3* pInVal = reinterpret_cast<Matrix3*>(val);
		Point3 pos = pInVal->GetTrans();
		MaxValueToFabric(m_binding, m_parentArgName.c_str(), 0, interval, *pInVal);
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
		MaxValueToFabric(m_binding, m_parentArgName.c_str(), 0, interval, Matrix3(1));
		*pOutVal = Evaluate(t, interval);
	}

	// We cannot detect validity intervals from KL graphs, 
	// so just set validity to the current instant.
	interval.SetInstant(t);
}



void FabricControlRotation::SetValue(TimeValue,void *value,int,GetSetMethod)
{
	// TODO:

}
