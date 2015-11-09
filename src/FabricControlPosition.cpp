//**************************************************************************/
// DESCRIPTION: Generates position values using Fabric
// AUTHOR: Stephen Taylor
//***************************************************************************/

#include "StdAfx.h"
#include "FabricControl.hpp"

#define FabricControlPosition_CLASS_ID	Class_ID(0x34952088, 0x7d9a01d7)

class FabricControlPosition : public FabricControl<Point3> {
public:

	//From Animatable
	Class_ID ClassID() {return FabricControlPosition_CLASS_ID;}		
	SClass_ID SuperClassID() { return CTRL_POSITION_CLASS_ID; }
	void GetClassName(TSTR& s) {s = GetString(IDS_SPLICE_POS_CTRL_CLASS_NAME);}

	void Copy(Control *);
	void GetValue(TimeValue,void *,Interval &,GetSetMethod);
	void SetValue(TimeValue,void *,int,GetSetMethod);

	//Constructor/Destructor
	FabricControlPosition(BOOL loading);
	~FabricControlPosition();	

private:

	int GetValueType() { return TYPE_POINT3; }
	int GetParentValueType() override { return TYPE_MATRIX3; }
};

class FabricControlPositionClassDesc : public DynPBCustAttrClassDesc {
public:
	FabricControlPositionClassDesc()
	{
	}
	void *			Create(BOOL loading = FALSE) { return new FabricControlPosition(loading); }
	const MCHAR *	ClassName() { static MSTR s = GetString(IDS_SPLICE_POS_CTRL_CLASS_NAME); return s.data(); }
	SClass_ID		SuperClassID() { return CTRL_POSITION_CLASS_ID; }
	Class_ID		ClassID() { return FabricControlPosition_CLASS_ID; }
	const TCHAR*	InternalName() {return _T("FabricPositionController");}
};

DynPBCustAttrClassDesc* FabricTranslationLayer<Control, Point3>::GetClassDesc()
{
	static FabricControlPositionClassDesc spliceControllerDesc;
	return &spliceControllerDesc; 
}

FabricControlPosition::FabricControlPosition(BOOL loading)
	: ParentClass(loading)
{
}


FabricControlPosition::~FabricControlPosition() 
{
}		

void FabricControlPosition::Copy(Control *)
{

}

void FabricControlPosition::GetValue(TimeValue t, void *val, Interval &interval, GetSetMethod method)
{
	HoldSuspend hs(); // Prevents us from creating undo objects when setting values to Fabric
	if(method == CTRL_RELATIVE)
	{
		Invalidate(); // Evaluate every time in case parent changes too
		Matrix3* pInVal = reinterpret_cast<Matrix3*>(val);
		MaxValueToFabric(m_binding, m_parentArgName.c_str(), 0, interval, *pInVal);
		pInVal->SetTrans(Evaluate(t, interval));
	}
	else
	{
		Point3* pOutVal = reinterpret_cast<Point3*>(val);
		MaxValueToFabric(m_binding, m_parentArgName.c_str(), 0, interval, Matrix3::Identity);
		*pOutVal = Evaluate(t, interval);
	}
}



void FabricControlPosition::SetValue(TimeValue,void *value,int,GetSetMethod)
{
	// TODO:

}
