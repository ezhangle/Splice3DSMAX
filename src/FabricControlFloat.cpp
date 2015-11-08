//**************************************************************************/
// DESCRIPTION: Generates float values using Fabric and returns them to 3dsMax
// AUTHOR: Stephen Taylor
//***************************************************************************/

#include "StdAfx.h"
#include "FabricControl.hpp"

#define FabricControlFloat_CLASS_ID	Class_ID(0x501d4dc2, 0x243979cd)

class FabricControlFloat : public FabricControl<float> {
public:

	//From Animatable
	Class_ID ClassID() {return FabricControlFloat_CLASS_ID;}		
	SClass_ID SuperClassID() { return CTRL_FLOAT_CLASS_ID; }
	void GetClassName(TSTR& s) {s = GetString(IDS_SPLICE_FLT_CTRL_CLASS_NAME);}

	void Copy(Control *);
	void GetValue(TimeValue,void *,Interval &,GetSetMethod);
	void SetValue(TimeValue,void *,int,GetSetMethod);

	//Constructor/Destructor
	FabricControlFloat(BOOL loading);
	~FabricControlFloat();	

private:

	int GetValueType() { return TYPE_FLOAT; }

	bool CloneFabricData(FabricTranslationLayer<Control, float>* pMyClone) { return true; } ; // No cloning for me...
};

class FabricControlFloatClassDesc : public DynPBCustAttrClassDesc {
public:
	FabricControlFloatClassDesc()
	{
	}
	void *			Create(BOOL loading) { return new FabricControlFloat(loading); }
	const MCHAR *	ClassName() { static MSTR s = GetString(IDS_SPLICE_FLT_CTRL_CLASS_NAME); return s.data(); }
	SClass_ID		SuperClassID() { return CTRL_FLOAT_CLASS_ID; }
	Class_ID		ClassID() { return FabricControlFloat_CLASS_ID; }
	const TCHAR*	InternalName() {return _T("FabricFloatController");}
};

DynPBCustAttrClassDesc* FabricTranslationLayer<Control, float>::GetClassDesc()
{
	static FabricControlFloatClassDesc spliceControllerDesc;
	return &spliceControllerDesc; 
}

FabricControlFloat::FabricControlFloat(BOOL loading)
	: ParentClass(loading)
{
	if (!loading)
		ResetPorts();
}


FabricControlFloat::~FabricControlFloat() 
{
}		

void FabricControlFloat::Copy(Control *)
{

}

void FabricControlFloat::GetValue(TimeValue t, void *val, Interval &interval, GetSetMethod method)
{

	float* pVal = reinterpret_cast<float*>(val);
	if(method == CTRL_RELATIVE)
	{
		Invalidate(); // Evaluate every time in case parent changes too
		MaxValueToFabric(m_binding, m_parentArgName.c_str(), 0, interval, *pVal);
	}
	else
	{
		MaxValueToFabric(m_binding, m_parentArgName.c_str(), 0, interval, float(0));
	}

	// Evaluate value from splice
	*pVal = Evaluate(t, interval);
}



void FabricControlFloat::SetValue(TimeValue,void *value,int,GetSetMethod)
{
	// TODO:

}
