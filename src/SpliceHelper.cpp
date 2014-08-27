/**********************************************************************
 *<
	FILE: pthelp.cpp

	DESCRIPTION:  A point helper implementation

	CREATED BY: 

	HISTORY: created 14 July 1995

 *>	Copyright (c) 1995, All Rights Reserved.
 **********************************************************************/


#include "FabricSplice3dsmax.h"
#include "SpliceMesh.h"

// class variable for SpliceMesh class.
IObjParam *SpliceMesh::ip = NULL;
SpliceMesh *SpliceMesh::editOb = NULL;


class SpliceMeshClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() { return 1; }
	void *			Create(BOOL /*loading*/ = FALSE) { return new SpliceMesh; }
	const MCHAR *	ClassName() { static MSTR className = GetString(IDS_DB_SPLICEMESH_CLASS); return className.data(); }
	SClass_ID		SuperClassID() { return HELPER_CLASS_ID; }
	Class_ID		ClassID() { return SPLICEMESH_CLASS_ID; }
	 
	const TCHAR* 	Category() { return _T("FabricSplice");  }
	const TCHAR*	InternalName() {return _T("SpliceMesh");}
	HINSTANCE		HInstance() { return MaxSDK::GetHInstance(); }			// returns owning module handle
};

static SpliceMeshClassDesc spliceMeshObjDesc;

ClassDesc* GetSpliceMeshDesc() { return &spliceMeshObjDesc; }

#define PBLOCK_REF_NO	 0

// block IDs
enum { spliceMesh_params, };

// spliceMesh_params IDs

 enum { 
	spliceMesh_asset, 

	spliceMesh_scaleFactor, 

	spliceMesh_size
};

////////////////////////////////////////////////////////////////////

class SpliceMeshDlgProc : public ParamMap2UserDlgProc 
{
public:

	INT_PTR DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{	
		switch (msg) {

		case WM_INITDIALOG:
			{
				break;
			}
		case WM_COMMAND:

			break;
		}
		
		return FALSE;
	}

	void DeleteThis() {};
};

static SpliceMeshDlgProc spliceMeshDlgProc;

#if MAX_VERSION_MAJOR < 15
// Fix define changes for older versions of max
#define p_end end
#endif

// per instance block
static ParamBlockDesc2 spliceMesh_param_blk( 
	
	spliceMesh_params, _T("SpliceMeshParameters"),  0, &spliceMeshObjDesc, P_AUTO_CONSTRUCT+P_AUTO_UI, PBLOCK_REF_NO,

	//rollout
	IDD_SPLICEMESH_PARAM, IDS_SPLICEMESH_PARAMS, 0, 0, &spliceMeshDlgProc,
	
	// params
	
	spliceMesh_asset,	_T("Asset"),			TYPE_STRING,		0,	IDS_APPLICATION,
		p_ui,			TYPE_EDITBOX,			IDC_ASSET,
		p_end,	
		
	// Helper params
	spliceMesh_scaleFactor,_T("Scale"),			TYPE_WORLD,			0,	IDS_SCALE_FACTOR,
		p_default, 		1.0,	
		p_ms_default,	1.0,
		p_range, 		0.0f, float(1.0E30), 
		p_ui, 			TYPE_SPINNER, EDITTYPE_UNIVERSE, IDC_SCALE_FACTOR, IDC_SCALE_FACTORSPIN, SPIN_AUTOSCALE, 
		p_end, 

	// Helper params
	spliceMesh_size,		_T("size"),				TYPE_WORLD,			0,	IDS_POINT_SIZE,
		p_default, 		20.0,	
		p_ms_default,	20.0,
		p_range, 		0.0f, float(1.0E30), 
		p_ui, 			TYPE_SPINNER, EDITTYPE_UNIVERSE, IDC_POINT_SIZE, IDC_POINT_SIZESPIN, SPIN_AUTOSCALE, 
		p_end, 

	p_end
	);





void SpliceMesh::BeginEditParams(
		IObjParam *ip, ULONG flags,Animatable *prev)
{	
	this->ip = ip;
	editOb   = this;
	spliceMeshObjDesc.BeginEditParams(ip, this, flags, prev);	
}

void SpliceMesh::EndEditParams(IObjParam *ip, ULONG flags,Animatable *next)
{	
	editOb   = NULL;
	this->ip = NULL;
	spliceMeshObjDesc.EndEditParams(ip, this, flags, next);
	ClearAFlag(A_OBJ_CREATING);
}


SpliceMesh::SpliceMesh()
{
	pblock2 = NULL;
	spliceMeshObjDesc.MakeAutoParamBlocks(this);
	suspendSnap = FALSE;
	SetAFlag(A_OBJ_CREATING);
}

SpliceMesh::~SpliceMesh()
{
	DeleteAllRefsFromMe();
}

IParamArray *SpliceMesh::GetParamBlock()
{
	return (IParamArray*)pblock2;
}

int SpliceMesh::GetParamBlockIndex(int id)
{
	if (pblock2 && id>=0 && id<pblock2->NumParams()) return id;
	else return -1;
}


class SpliceMeshCreateCallBack: public CreateMouseCallBack {
	SpliceMesh *ob;
public:
	int proc( ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat );
	void SetObj(SpliceMesh *obj) { ob = obj; }
};

int SpliceMeshCreateCallBack::proc(ViewExp *vpt,int msg, int point, int flags, IPoint2 m, Matrix3& mat ) 
{
	if ( ! vpt 
#if MAX_VERSION_MAJOR > 14
		|| ! vpt->IsAlive() 
#endif
		)
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}

	#ifdef _OSNAP
	if (msg == MOUSE_FREEMOVE)
	{
		#ifdef _3D_CREATE
			vpt->SnapPreview(m,m,NULL, SNAP_IN_3D);
		#else
			vpt->SnapPreview(m,m,NULL, SNAP_IN_PLANE);
		#endif
	}
	#endif

	if (msg==MOUSE_POINT||msg==MOUSE_MOVE) {
		switch(point) {
		case 0: {

			// Find the node and plug in the wire color
			ULONG handle;
			ob->NotifyDependents(FOREVER, (PartID)&handle, REFMSG_GET_NODE_HANDLE);
			INode *node;
			node = GetCOREInterface()->GetINodeByHandle(handle);
			if (node) {
				Point3 color = GetUIColor(COLOR_POINT_OBJ);
				node->SetWireColor(RGB(color.x*255.0f, color.y*255.0f, color.z*255.0f));
			}

			ob->suspendSnap = TRUE;
			#ifdef _3D_CREATE	
				mat.SetTrans(vpt->SnapPoint(m,m,NULL,SNAP_IN_3D));
			#else	
				mat.SetTrans(vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE));
			#endif				
			break;
		}

		case 1:
			#ifdef _3D_CREATE	
				mat.SetTrans(vpt->SnapPoint(m,m,NULL,SNAP_IN_3D));
			#else	
				mat.SetTrans(vpt->SnapPoint(m,m,NULL,SNAP_IN_PLANE));
			#endif
			if (msg==MOUSE_POINT) {
				ob->suspendSnap = FALSE;
				return 0;
			}
			break;			
		}
	} 
	else 
		if (msg == MOUSE_ABORT) {		
			return CREATE_ABORT;
		}
	return 1;
}

static SpliceMeshCreateCallBack spliceMeshCreateCB;

CreateMouseCallBack* SpliceMesh::GetCreateMouseCallBack() {
	spliceMeshCreateCB.SetObj(this);
	return(&spliceMeshCreateCB);
}

void SpliceMesh::SetExtendedDisplay(int flags)
{
	extDispFlags = flags;
}


void SpliceMesh::GetLocalBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box ) 
{
	if ( ! vpt 
#if MAX_VERSION_MAJOR > 14
		|| ! vpt->IsAlive() 
#endif
		)
	{
		box.Init();
		return;
	}

	Matrix3 tm = inode->GetObjectTM(t);	
	
	float size;
	Interval iv = FOREVER;
	pblock2->GetValue(spliceMesh_size, t, size, iv);

	box =  Box3(Point3(0,0,0), Point3(0,0,0));
	box += Point3(size*0.5f,  0.0f, 0.0f);
	box += Point3( 0.0f, size*0.5f, 0.0f);
	box += Point3( 0.0f, 0.0f, size*0.5f);
	box += Point3(-size*0.5f,   0.0f,  0.0f);
	box += Point3(  0.0f, -size*0.5f,  0.0f);
	box += Point3(  0.0f,  0.0f, -size*0.5f);
}

void SpliceMesh::GetWorldBoundBox(TimeValue t, INode* inode, ViewExp* vpt, Box3& box )
{

	if ( ! vpt 
#if MAX_VERSION_MAJOR > 14
		|| ! vpt->IsAlive() 
#endif
		)
	{
		box.Init();
		return;
	}

	Matrix3 tm;
	tm = inode->GetObjectTM(t);
	Box3 lbox;

	GetLocalBoundBox(t, inode, vpt, lbox);
	box = Box3(tm.GetTrans(), tm.GetTrans());
	for (int i=0; i<8; i++) {
		box += lbox * tm;
	}
}


void SpliceMesh::Snap(TimeValue t, INode* inode, SnapInfo *snap, IPoint2 *p, ViewExp *vpt)
{
	if ( ! vpt 
#if MAX_VERSION_MAJOR > 14
		|| ! vpt->IsAlive() 
#endif
		)
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return;
	}

	if(suspendSnap)
		return;

	Matrix3 tm = inode->GetObjectTM(t);	
	GraphicsWindow *gw = vpt->getGW();	
	gw->setTransform(tm);

	Matrix3 invPlane = Inverse(snap->plane);

	// Make sure the vertex priority is active and at least as important as the best snap so far
	if(snap->vertPriority > 0 && snap->vertPriority <= snap->priority) {
		Point2 fp = Point2((float)p->x, (float)p->y);
		Point2 screen2;
		IPoint3 pt3;

		Point3 thePoint(0,0,0);
		// If constrained to the plane, make sure this point is in it!
		if(snap->snapType == SNAP_2D || snap->flags & SNAP_IN_PLANE) {
			Point3 test = thePoint * tm * invPlane;
			if(fabs(test.z) > 0.0001)	// Is it in the plane (within reason)?
				return;
		}
		gw->wTransPoint(&thePoint,&pt3);
		screen2.x = (float)pt3.x;
		screen2.y = (float)pt3.y;

		// Are we within the snap radius?
		int len = (int)Length(screen2 - fp);
		if(len <= snap->strength) {
			// Is this priority better than the best so far?
			if(snap->vertPriority < snap->priority) {
				snap->priority = snap->vertPriority;
				snap->bestWorld = thePoint * tm;
				snap->bestScreen = screen2;
				snap->bestDist = len;
			}
			else
			if(len < snap->bestDist) {
				snap->priority = snap->vertPriority;
				snap->bestWorld = thePoint * tm;
				snap->bestScreen = screen2;
				snap->bestDist = len;
			}
		}
	}
}




int SpliceMesh::DrawAndHit(TimeValue t, INode *inode, ViewExp *vpt)
{
	
	if ( ! vpt 
#if MAX_VERSION_MAJOR > 14
		|| ! vpt->IsAlive() 
#endif
		)
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}

	float size;
	Color color(inode->GetWireColor());	

	Interval ivalid = FOREVER;
	pblock2->GetValue(spliceMesh_size, t,         size, ivalid);

	Matrix3 tm(1);
	Point3 pt(0,0,0);
	Point3 pts[5];

	vpt->getGW()->setTransform(tm);	
	tm = inode->GetObjectTM(t);

	int limits = vpt->getGW()->getRndLimits();

	if (inode->Selected()) {
		vpt->getGW()->setColor( TEXT_COLOR, GetUIColor(COLOR_SELECTION) );
		vpt->getGW()->setColor( LINE_COLOR, GetUIColor(COLOR_SELECTION) );
	} 
	else if (!inode->IsFrozen() && !inode->Dependent()) {		
		vpt->getGW()->setColor( TEXT_COLOR, color);
		vpt->getGW()->setColor( LINE_COLOR, color);
	}
	size *= 0.5f;

	vpt->getGW()->setTransform(tm);

	if (!inode->IsFrozen() && !inode->Dependent() && !inode->Selected()) {
		vpt->getGW()->setColor( LINE_COLOR, color);
	}
	// X
	pts[0] = Point3(-size, -size, 0.0f); pts[1] = Point3(size, -size, 0.0f);
	vpt->getGW()->polyline(2, pts, NULL, NULL, FALSE, NULL);

	pts[0] = Point3(-size, size, 0.0f); pts[1] = Point3(size, size, 0.0f);
	vpt->getGW()->polyline(2, pts, NULL, NULL, FALSE, NULL);

	// Z
	pts[0] = Point3(-size, -size, 0.0f); pts[1] = Point3(-size, size, 0.0f);
	vpt->getGW()->polyline(2, pts, NULL, NULL, FALSE, NULL);

	pts[0] = Point3(size, -size, 0.0f); pts[1] = Point3(size, size, 0.0f);
	vpt->getGW()->polyline(2, pts, NULL, NULL, FALSE, NULL);

	vpt->getGW()->setRndLimits(limits);

	return 1;
}

int SpliceMesh::HitTest(TimeValue t, INode *inode, int type, int crossing, int flags, IPoint2 *p, ViewExp *vpt) 
{
	if ( ! vpt 
#if MAX_VERSION_MAJOR > 14
		|| ! vpt->IsAlive() 
#endif
		)
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}

	Matrix3 tm(1);	
	HitRegion hitRegion;
	DWORD	savedLimits;
	Point3 pt(0,0,0);

	GraphicsWindow *gw = vpt->getGW();	
	gw->setTransform(tm);

   	tm = inode->GetObjectTM(t);		
	MakeHitRegion(hitRegion, type, crossing, 4, p);

	gw->setRndLimits(((savedLimits = gw->getRndLimits())|GW_PICK)&~GW_ILLUM);
	gw->setHitRegion(&hitRegion);
	gw->clearHitCode();

	DrawAndHit(t, inode, vpt);

	gw->setRndLimits(savedLimits);

	return gw->checkHitCode();
}


int SpliceMesh::Display(TimeValue t, INode* inode, ViewExp *vpt, int flags) 
{
	if ( ! vpt 
#if MAX_VERSION_MAJOR > 14
		|| ! vpt->IsAlive() 
#endif
		)
	{
		// why are we here
		DbgAssert(!_T("Invalid viewport!"));
		return FALSE;
	}

	DrawAndHit(t, inode, vpt);

	return(0);
}



//
// Reference Managment:
//

// This is only called if the object MAKES references to other things.
RefResult SpliceMesh::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget, 
		PartID& partID, RefMessage message ) 
{
	switch (message) {
	case REFMSG_CHANGE:
		if (editOb==this) InvalidateUI();
		break;
	}
	return(REF_SUCCEED);
}

void SpliceMesh::InvalidateUI()
{
	spliceMesh_param_blk.InvalidateUI(pblock2->LastNotifyParamID());
}

Interval SpliceMesh::ObjectValidity(TimeValue t)
{
	float size;

	Interval ivalid = FOREVER;
	pblock2->GetValue(spliceMesh_size, t,         size, ivalid);

	ivalid.SetInstant(t);
	return ivalid;
}

ObjectState SpliceMesh::Eval(TimeValue t)
{
	return ObjectState(this);
}

RefTargetHandle SpliceMesh::Clone(RemapDir& remap) 
{
	SpliceMesh* newob = new SpliceMesh();	
	newob->ReplaceReference(0, remap.CloneRef(pblock2));
	BaseClone(this, newob, remap);
	return(newob);
}



IOResult SpliceMesh::Load(ILoad *iload)
{
	return IO_OK;
}

IOResult SpliceMesh::Save(ISave *isave)
{
	return IO_OK;
}



TSTR SpliceMesh::GetAssetPath()
{
	return pblock2->GetStr(spliceMesh_asset);
}



float SpliceMesh::GetScaleFactor( TimeValue t, Interval& ivValid )
{
	float scale;
	pblock2->GetValue(spliceMesh_scaleFactor, t, scale, ivValid);
	return scale;
}

