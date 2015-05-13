//**************************************************************************/
// DESCRIPTION: Applies modifications to an input mesh using Splice
// AUTHOR: Stephen Taylor
//***************************************************************************/

#include "StdAfx.h"
#include "SpliceModifier.h"



//--- SpliceModifier -------------------------------------------------------
// Only initialize the splice graph if loading.  Else,
// we initialize the class on every time the Modifier list
// is dropped down in the UI.  If Splice is not initialized,
// this takes about 4s (every drop down), which is just not polite.
SpliceModifier::SpliceModifier(BOOL loading)
	: ParentClass(loading == TRUE)
{
	if (!loading)
		ResetPorts();
}

SpliceModifier::~SpliceModifier()
{
}

void SpliceModifier::RefAdded(RefMakerHandle rm)
{
	//if (Init(FALSE))
	//{
	//	SetPortName("outputValue", "modifierMesh");
	//}
}

Interval SpliceModifier::LocalValidity(TimeValue t)
{
	return m_valid;
}

void SpliceModifier::NotifyInputChanged( const Interval &changeInt, PartID partID, RefMessage message, ModContext *mc )
{
	if (message == REFMSG_CHANGE)
	{
		m_inputValid.SetEmpty();
		Invalidate();
	}
}

//////////////////////////////////////////////////////////////////////////
void SpliceModifier::ModifyObject( TimeValue t, ModContext &mc, ObjectState* os, INode *node )
{
	if (!m_binding.isValid())
		return;

	TriObject* pTriObj = static_cast<TriObject*>(os->obj);
	// If our cached value is valid, just use that and return
	Interval ivValid = FOREVER;
	if (m_valid.InInterval(t)) {
		pTriObj->GetMesh() = m_value;
		ivValid = m_valid;
	}
	else {
		// Send our input to Max	
		// A modifier is a special kind of mesh, in that we pipe our
		// mesh into the output port as an IO port
		if (!m_inputValid.InInterval(t)) 
		{
			m_inputValid.SetInfinite();
			MaxValuesToSplice<Object*, Mesh>(m_client, m_valuePort, t, m_inputValid, &os->obj, 1);
			ivValid &= m_inputValid;
		}

		// Set our output.
		pTriObj->GetMesh() = Evaluate(t, ivValid);;
	}
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