//////////////////////////////////////////////////////////////////////////
// SpliceReference target allows us to create a generic splice node
// Its not really meant to be made part of the scene graph, instead this
// can be a generic data container, hosting
#include "StdAfx.h"

#define SpliceReferenceTarget_CLASS_ID Class_ID(0x7bce50e7, 0x5ddc0273)

class SpliceReferenceTarget : public SpliceTranslationLayer<ReferenceTarget, int> {

	//From Animatable
	Class_ID ClassID() {return SpliceReferenceTarget_CLASS_ID;}		
	SClass_ID SuperClassID() { return REF_TARGET_CLASS_ID; }
	void GetClassName(TSTR& s) {s = GetString(IDS_SPLICE_REFTARG_CLASS_NAME);}

	// We override our reset to prevent from generating a default out-port
	void ResetPorts() {}

	// We don't really have an out value type.
	int GetValueType() { return TYPE_REFTARG; }

	// We have no extra data to clone.
	bool CloneSpliceData(SpliceTranslationLayer* pMyClone) { return true; }

public:
	SpliceReferenceTarget();
	~SpliceReferenceTarget();
};

class SpliceReferenceTargetClassDesc : public DynPBCustAttrClassDesc {
public:
	SpliceReferenceTargetClassDesc()
	{
	}
	void *			Create(BOOL /*loading*/ = FALSE) { return new SpliceReferenceTarget; }
	const MCHAR *	ClassName() { static MSTR s = GetString(IDS_SPLICE_REFTARG_CLASS_NAME); return s.data(); }
	SClass_ID		SuperClassID() { return REF_TARGET_CLASS_ID; }
	Class_ID		ClassID() { return SpliceReferenceTarget_CLASS_ID; }
	const TCHAR*	InternalName() {return _T("SpliceReferenceTarget");}
};

DynPBCustAttrClassDesc* SpliceTranslationLayer<ReferenceTarget, int>::GetClassDesc()
{
	static SpliceReferenceTargetClassDesc spliceControllerDesc;
	return &spliceControllerDesc; 
}

SpliceReferenceTarget::SpliceReferenceTarget() {}
SpliceReferenceTarget::~SpliceReferenceTarget() {}