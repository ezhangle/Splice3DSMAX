//**************************************************************************/
// DESCRIPTION: Generates float values using Splice and returns them to 3dsMax
// AUTHOR: Stephen Taylor
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

	DFGWrapper::PortPtr m_parentValuePort;
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
	m_parentValuePort = AddSpliceParameter(GetBinding(), TYPE_FLOAT, _M("parentValue"), FabricCore::DFGPortType_In);
	ParentClass::ResetPorts();
}

void SpliceControlFloat::Copy(Control *)
{

}

void SpliceControlFloat::GetValue(TimeValue t, void *val, Interval &interval, GetSetMethod method)
{

	float* pVal = reinterpret_cast<float*>(val);
	if(method == CTRL_RELATIVE)
	{
		Invalidate(); // Evaluate every time in case parent changes too
		MaxValueToSplice(m_parentValuePort, 0, interval, *pVal);
	}
	else
	{
		MaxValueToSplice(m_parentValuePort, 0, interval, float(0));
	}

	// Evaluate value from splice
	*pVal = Evaluate(t, interval);
}



void SpliceControlFloat::SetValue(TimeValue,void *value,int,GetSetMethod)
{
	// TODO:

}