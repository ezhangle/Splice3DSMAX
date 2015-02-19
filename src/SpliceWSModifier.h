//////////////////////////////////////////////////////////////////////////
// 
#define SpliceWSModifier_CLASS_ID	Class_ID(0x36cb2ec2, 0x5d196a8e)

//////////////////////////////////////////////////////////////////////////
class SpliceWSModifier : public SpliceTranslationLayer<WSModifier, Mesh>
{
public:

	//Constructor/Destructor
	SpliceWSModifier(BOOL loading);
	virtual ~SpliceWSModifier();	

	virtual void DeleteThis() { delete this; }	

	// From Modifier
	// It is impossible for us to tell what we have changed within splice, so specify everything
	virtual ChannelMask ChannelsUsed() { return OBJ_CHANNELS; }
	virtual ChannelMask ChannelsChanged() { return OBJ_CHANNELS; };
	virtual Class_ID InputType() { return triObjectClassID; }
	virtual Interval LocalValidity(TimeValue t);
	// We do not know if we change topology or not, so we need to return TRUE here.
	virtual BOOL ChangeTopology() { return TRUE; }
	virtual void NotifyInputChanged (const Interval &changeInt, PartID partID, RefMessage message, ModContext *mc);

	void ModifyObject(TimeValue t, ModContext &mc, ObjectState* os, INode *node);

	// From BaseObject
	virtual int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
	virtual int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
	virtual void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);

	// Return the name that will appear in the history browser (modifier stack)
	virtual const TCHAR *GetObjectName() { return GetString(IDS_SPLICE_WSMODIFIER_CLASS); }
	virtual void GetWorldBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
	virtual void GetLocalBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
	virtual void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel );

	// From Object
	virtual int CanConvertToType(Class_ID obtype);
	virtual Object* ConvertToType(TimeValue t, Class_ID obtype);
	virtual void GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist);
	virtual int IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm);

	virtual CreateMouseCallBack* GetCreateMouseCallBack() { return NULL; }

	//From Animatable
	virtual Class_ID ClassID() {return SpliceWSModifier_CLASS_ID;}		
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_SPLICE_WSMODIFIER_CLASS);}

	// From ReferenceTarget
	// Because we cannot initialize this class on creation,
	// we initialize on the first time a reference is attached.
	// This is probably the least costly (in unnecessary init's)
	// and most secure method of initializing our graph.
	void RefAdded(RefMakerHandle rm);

private:
	void ResetPorts();

	// From SpliceTranslationLayer
	int GetValueType() { return TYPE_MESH; }
	bool CloneSpliceData(ParentClass* pMyClone) { return true; }

	FabricSplice::DGPort m_nodeTransformPort;

	// Our cache is only valid if the node transform has not changed.
	// We cache the nodes transform, and invalidate our cache
	// if it changes.
	Matrix3 m_cachedNodeTM;
};