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
#include <object.h>
#include <ParticleFlow/OneClickCreateCallback.h>

#define SpliceMesh_CLASS_ID	Class_ID(0x2ebd61e9, 0x33da121d)

//////////////////////////////////////////////////////////////////////////
class SpliceMesh : public SpliceTranslationLayer<GeomObject, Mesh>
{
public:

	//Constructor/Destructor
	SpliceMesh(BOOL loading);
	virtual ~SpliceMesh();	

	virtual void DeleteThis() { delete this; }	

	// From BaseObject
	virtual CreateMouseCallBack* GetCreateMouseCallBack();
	virtual int Display(TimeValue t, INode* inode, ViewExp *vpt, int flags);
	virtual int HitTest(TimeValue t, INode* inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt);
	virtual void Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt);
	
	// Return the name that will appear in the history browser (modifier stack)
	virtual const TCHAR *GetObjectName() { return GetString(IDS_SPLICEMESH_CLASS_NAME); }
	
	virtual void GetWorldBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );
	virtual void GetLocalBoundBox(TimeValue t, INode *mat, ViewExp *vpt, Box3& box );

	virtual void GetDeformBBox(TimeValue t, Box3& box, Matrix3 *tm, BOOL useSel );
	// Return the default name of the node when it is created.
	virtual void InitNodeName(TSTR& s) { s = GetString(IDS_SPLICEMESH_CLASS_NAME); }
	
	// From Object
	virtual BOOL HasUVW();
	virtual void SetGenUVW(BOOL sw);
	virtual int CanConvertToType(Class_ID obtype);
	virtual Object* ConvertToType(TimeValue t, Class_ID obtype);
	virtual void GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist);
	virtual int IntersectRay(TimeValue t, Ray& ray, float& at, Point3& norm);
	virtual ObjectState Eval(TimeValue t) { return ObjectState(this); };		
	virtual Interval ObjectValidity(TimeValue t) { return m_valid; }

	// From GeomObject
	virtual Mesh* GetRenderMesh(TimeValue t, INode *inode, View& view, BOOL& needDelete);

	//From Animatable
	virtual Class_ID ClassID() {return SpliceMesh_CLASS_ID;}		
	virtual void GetClassName(TSTR& s) {s = GetString(IDS_SPLICEMESH_CLASS_NAME);}

private:
	// From SpliceTranslationLayer
	int GetValueType() { return TYPE_MESH; }

	bool CloneSpliceData(ParentClass* pMyClone) { return true; }
};

class SpliceMeshClassDesc: public DynPBCustAttrClassDesc {
	public:
	void *			Create(BOOL loading = FALSE) { return new SpliceMesh(loading); }
	const MCHAR *	ClassName() { static MSTR className = GetString(IDS_SPLICEMESH_CLASS_NAME); return className.data(); }
	SClass_ID		SuperClassID() { return GEOMOBJECT_CLASS_ID; }
	Class_ID		ClassID() { return SpliceMesh_CLASS_ID; }
	const TCHAR*	InternalName() {return _T("SpliceMesh");}
};

DynPBCustAttrClassDesc* SpliceMesh::ParentClass::GetClassDesc() 
{ 
	static SpliceMeshClassDesc spliceMeshDesc;
	return &spliceMeshDesc; 
}


//--- SpliceMesh -------------------------------------------------------

SpliceMesh::SpliceMesh(BOOL loading)
{
}

SpliceMesh::~SpliceMesh()
{
}
//From Object
BOOL SpliceMesh::HasUVW() 
{ 
	return TRUE; 
}

void SpliceMesh::SetGenUVW(BOOL sw) 
{  
	if (sw==HasUVW()) return;
	//#pragma message(TODO("TODO: Set the plugin's internal value to sw"))
}

//From BaseObject
CreateMouseCallBack* SpliceMesh::GetCreateMouseCallBack() 
{
	return OneClickCreateCallBack::Instance();
}

int SpliceMesh::Display(TimeValue t, INode* inode, ViewExp* vpt, int flags)
{
	// TODO: Nitrous support here
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	pMeshNoConst->render(vpt->getGW(), inode->Mtls(), NULL, flags, inode->NumMtls());
	return 0;
}

int SpliceMesh::HitTest(TimeValue t, INode* inode, int type, int crossing, 
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

void SpliceMesh::Snap(TimeValue t, INode* inode, SnapInfo* snap, IPoint2* p, ViewExp* vpt)
{
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	Matrix3 tm = inode->GetNodeTM(t);
	pMeshNoConst->snap(vpt->getGW(), snap, p, tm);
}

void SpliceMesh::GetWorldBoundBox(TimeValue t, INode* pNode, ViewExp* /*vpt*/, Box3& box )
{
	GetDeformBBox(t, box, &pNode->GetNodeTM(t), FALSE);
}

void SpliceMesh::GetLocalBoundBox(TimeValue t, INode *mat, ViewExp * /*vpt*/, Box3& box )
{
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	box = pMeshNoConst->getBoundingBox();
}

void SpliceMesh::GetDeformBBox(TimeValue t, Box3& box, Matrix3* tm, BOOL useSel )
{
	GetLocalBoundBox(t, NULL, NULL, box);
	if (tm)
		box = box * *tm;
}

Mesh* SpliceMesh::GetRenderMesh(TimeValue t, 
						INode *inode, View& view, BOOL& needDelete)
{
	needDelete = FALSE;
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	return const_cast<Mesh*>(&mesh);
}


Object* SpliceMesh::ConvertToType(TimeValue t, Class_ID obtype)
{
	if (obtype==triObjectClassID) 
	{
		TriObject* pTriObj = reinterpret_cast<TriObject*>(CreateInstance(GEOMOBJECT_CLASS_ID, triObjectClassID));
		if (pTriObj != NULL)
		{
			Interval ivDontCare;
			pTriObj->mesh = Evaluate(t, ivDontCare);
		}
		return pTriObj;
	}
	return NULL;
}

int SpliceMesh::CanConvertToType(Class_ID obtype)
{
	if (obtype==triObjectClassID) 
	{
		return 1;
	}
	return 0;
}

// From Object
int SpliceMesh::IntersectRay(
		TimeValue t, Ray& ray, float& at, Point3& norm)
{
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	return pMeshNoConst->IntersectRay(ray,at,norm);
}

void SpliceMesh::GetCollapseTypes(Tab<Class_ID> &clist,Tab<TSTR*> &nlist)
{
    Object::GetCollapseTypes(clist, nlist);
	//#pragma message(TODO("Append any any other collapse type the plugin supports"))
}
