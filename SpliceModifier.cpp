//**************************************************************************/
// Copyright (c) 1998-2007 Autodesk, Inc.
// All rights reserved.
// 
// These coded instructions, statements, and computer programs contain
// unpublished proprietary information written by Autodesk, Inc., and are
// protected by Federal copyright law. They may not be disclosed to third
// parties or copied or duplicated in any form, in whole or in part, without
// the prior written consent of Autodesk, Inc.
//**************************************************************************/
// DESCRIPTION: Appwizard generated plugin
// AUTHOR: 
//***************************************************************************/

#include "StdAfx.h"
//#include <object.h>
#include <triobj.h>
#include "FabricSplice.h"

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
	virtual void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel );
	
	// From Object
	virtual int CanConvertToType(Class_ID obtype);
	virtual Object* ConvertToType(TimeValue t, Class_ID obtype);
	virtual void GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist);
	virtual int IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm);

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
	int GetValueType() { return TYPE_MESH; }
	bool CloneSpliceData(ParentClass* pMyClone) { return true; }
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


//--- SpliceModifier -------------------------------------------------------
// Only initialize the splice graph if loading.  Else,
// we initialize the class on every time the Modifier list
// is dropped down in the UI.  If Splice is not initialized,
// this takes about 4s (every drop down), which is just not polite.
SpliceModifier::SpliceModifier(BOOL loading)
	: ParentClass(loading == TRUE)
{
}

SpliceModifier::~SpliceModifier()
{
}

void SpliceModifier::RefAdded(RefMakerHandle rm)
{
	if (Init())
	{
		SetOutPortName("modifierMesh");
	}
}

//////////////////////////////////////////////////////////////////////////
Interval SpliceModifier::LocalValidity(TimeValue t)
{
	return m_valid;
}

//////////////////////////////////////////////////////////////////////////
void SpliceModifier::ModifyObject( TimeValue t, ModContext &mc, ObjectState* os, INode *node )
{
	if (!m_graph.isValid() || m_graph.getKLOperatorCount() == 0)
		return;

	// Send our input to Max
	Interval ivValid = FOREVER;
	// A modifier is a special kind of mesh, in that we pipe our
	// mesh into the output port as an IO port
	MaxValuesToSplice<Object*, Mesh>(m_valuePort, t, ivValid, &os->obj, 1);
	
	// Set our output.
	TriObject* pTriObj = static_cast<TriObject*>(os->obj);
	pTriObj->GetMesh() = Evaluate(t, ivValid);;

	// We may have changed any of these attributes.
	// OPTIMIZE: We should not invalidate things we haven't changed
	// Perhaps we should add the ability to set channels via MxS
	pTriObj->UpdateValidity(GEOM_CHAN_NUM, ivValid);
	pTriObj->UpdateValidity(TOPO_CHAN_NUM, ivValid);
	pTriObj->UpdateValidity(TEXMAP_CHAN_NUM, ivValid);
	pTriObj->UpdateValidity(VERT_COLOR_CHAN_NUM, ivValid);
}

int SpliceModifier::Display(TimeValue t, INode* inode, ViewExp* vpt, int flags)
{
	// TODO: Nitrous support here
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	pMeshNoConst->render(vpt->getGW(), inode->Mtls(), NULL, flags, inode->NumMtls());
	return 0;
}

int SpliceModifier::HitTest(TimeValue t, INode* inode, int type, int crossing, 
						int flags, IPoint2* p, ViewExp* vpt)
{
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);

	HitRegion hitRegion;
	GraphicsWindow *gw = vpt->getGW();  
	Material *mtl = gw->getMaterial();     
	gw->setTransform(inode->GetObjectTM(t));
	MakeHitRegion(hitRegion, type, crossing, 4, p);
	return pMeshNoConst->select(gw, mtl, &hitRegion, flags & HIT_ABORTONHIT);
}

void SpliceModifier::Snap(TimeValue t, INode* inode, SnapInfo* snap, IPoint2* p, ViewExp* vpt)
{
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	Matrix3 tm = inode->GetNodeTM(t);
	pMeshNoConst->snap(vpt->getGW(), snap, p, tm);
}

void SpliceModifier::GetWorldBoundBox(TimeValue t, INode* mat, ViewExp* /*vpt*/, Box3& box )
{
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	box = pMeshNoConst->getBoundingBox(&mat->GetNodeTM(t));
}

void SpliceModifier::GetLocalBoundBox(TimeValue t, INode *mat, ViewExp * /*vpt*/, Box3& box )
{
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	box = pMeshNoConst->getBoundingBox();
}

void SpliceModifier::GetDeformBBox(TimeValue t, Box3& box, Matrix3* tm, BOOL useSel )
{
	//#pragma message(TODO("Compute the bounding box in the objects local coordinates or the optional space defined by tm."))
}

Object* SpliceModifier::ConvertToType(TimeValue t, Class_ID obtype)
{
	if (obtype==triObjectClassID) 
	{
		TriObject* pTriObj = reinterpret_cast<TriObject*>(CreateInstance(GEOMOBJECT_CLASS_ID, polyObjectClassID));
		if (pTriObj != NULL)
		{
			Interval ivDontCare;
			pTriObj->GetMesh() = Evaluate(t, ivDontCare);
		}
		return pTriObj;
	}
	return NULL;
}

int SpliceModifier::CanConvertToType(Class_ID obtype)
{
	if (obtype==triObjectClassID) 
	{
		return 1;
	}
	return 0;
}

// From Object
int SpliceModifier::IntersectRay(
	TimeValue t, Ray& ray, float& at, Point3& norm)
{
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	return pMeshNoConst->IntersectRay(ray,at,norm);
}

void SpliceModifier::GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist)
{
	//BaseObject::GetCollapseTypes(clist, nlist);
	//#pragma message(TODO("Append any any other collapse type the plugin supports"))
}
