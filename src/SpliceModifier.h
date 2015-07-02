//////////////////////////////////////////////////////////////////////////
// 
#define SpliceModifier_CLASS_ID	Class_ID(0x14f5108f, 0x1f3e182e)

//////////////////////////////////////////////////////////////////////////
class SpliceModifier : public SpliceTranslationLayer<OSModifier, Mesh>
{
public:

	//Constructor/Destructor
	SpliceModifier(BOOL loading);
	virtual ~SpliceModifier();	

	virtual void DeleteThis() { delete this; }	

	// From Modifier
	// It is impossible for us to tell what we have changed within splice, so specify everything
	virtual ChannelMask ChannelsUsed() { return OBJ_CHANNELS; }
	virtual ChannelMask ChannelsChanged() { return OBJ_CHANNELS; };
	virtual Class_ID InputType() { return triObjectClassID; }
	virtual Interval LocalValidity(TimeValue t);
	virtual void NotifyInputChanged (const Interval &changeInt, PartID partID, RefMessage message, ModContext *mc);
	// We do not know if we change topology or not, so we need to return TRUE here.
	virtual BOOL ChangeTopology() { return TRUE; }

	void ModifyObject(TimeValue t, ModContext &mc, ObjectState* os, INode *node);

	// From BaseObject
	virtual int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
	virtual int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
	virtual void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);

	// Return the name that will appear in the history browser (modifier stack)
	virtual const TCHAR *GetObjectName() { return GetString(IDS_SpliceModifier_CLASS_NAME); }
	virtual void GetWorldBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
	virtual void GetLocalBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );

	virtual CreateMouseCallBack* GetCreateMouseCallBack() { return NULL; }

	//From Animatable
	virtual Class_ID ClassID() {return SpliceModifier_CLASS_ID;}		
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_SpliceModifier_CLASS_NAME);}

	// From ReferenceTarget
	// Because we cannot initialize this class on creation,
	// we initialize on the first time a reference is attached.
	// This is probably the least costly (in unnecessary init's)
	// and most secure method of initializing our graph.
	void RefAdded(RefMakerHandle rm);

private:
	// From SpliceTranslationLayer
	int GetValueType() override { return TYPE_MESH; }
	bool CloneSpliceData(ParentClass* pMyClone) override { return true; }
	void ResetPorts() override;

	// Pass input mesh in
	std::string m_inMeshPort;

	Interval m_inputValid;
};

class SpliceModifierClassDesc: public DynPBCustAttrClassDesc {
public:
	void *			Create(BOOL loading = FALSE) { return new SpliceModifier(loading); }
	const MCHAR *	ClassName() { static MSTR className = GetString(IDS_SpliceModifier_CLASS_NAME); return className.data(); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return SpliceModifier_CLASS_ID; }
	const TCHAR*	InternalName() {return _T("SpliceModifier");}
};

DynPBCustAttrClassDesc* SpliceModifier::ParentClass::GetClassDesc() 
{ 
	static SpliceModifierClassDesc SpliceModifierDesc;
	return &SpliceModifierDesc; 
}