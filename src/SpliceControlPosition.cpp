//**************************************************************************/
// DESCRIPTION: Generates position values using Splice
// AUTHOR: Stephen Taylor
//***************************************************************************/

#include "StdAfx.h"
#include "SpliceControl.hpp"

#define SpliceControlPosition_CLASS_ID	Class_ID(0x34952088, 0x7d9a01d7)

class SpliceControlPosition : public SpliceControl<Point3> {
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
	: ParentClass(loading)
{
	if (!loading)
		ResetPorts();
}


SpliceControlPosition::~SpliceControlPosition() 
{
}		

void SpliceControlPosition::ResetPorts()
{
	m_parentValuePort = AddSpliceParameter(m_binding, TYPE_MATRIX3, _M("parentValue"), FabricCore::DFGPortType_In);
	m_parentValuePort->setMetadata("uiHidden", "true", false);
	ParentClass::ResetPorts();
}

void SpliceControlPosition::Copy(Control *)
{

}

void SpliceControlPosition::GetValue(TimeValue t, void *val, Interval &interval, GetSetMethod method)
{
	if(method == CTRL_RELATIVE)
	{
		Invalidate(); // Evaluate every time in case parent changes too
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



void SpliceControlPosition::SetValue(TimeValue,void *value,int,GetSetMethod)
{
	// TODO:

}