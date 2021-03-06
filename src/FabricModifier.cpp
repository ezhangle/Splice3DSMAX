//**************************************************************************/
// DESCRIPTION: Applies modifications to an input mesh using Fabric
// AUTHOR: Stephen Taylor
//***************************************************************************/

#include "StdAfx.h"
#include "FabricModifier.h"
#include <MeshNormalSpec.h>



//--- FabricModifier -------------------------------------------------------
// Only initialize the splice graph if loading.  Else,
// we initialize the class on every time the Modifier list
// is dropped down in the UI.  If Fabric is not initialized,
// this takes about 4s (every drop down), which is just not polite.
FabricModifier::FabricModifier(BOOL loading)
	: ParentClass(true)
{
}

FabricModifier::~FabricModifier()
{
}

void FabricModifier::RefAdded(RefMakerHandle rm)
{
	if (!m_binding.isValid())
	{
		Init();
		ResetPorts();
	}
}

Interval FabricModifier::LocalValidity(TimeValue t)
{
	return m_valid;
}

void FabricModifier::NotifyInputChanged( const Interval &changeInt, PartID partID, RefMessage message, ModContext *mc )
{
	if (message == REFMSG_CHANGE)
	{
		m_inputValid.SetEmpty();
		Invalidate();
	}
}

//////////////////////////////////////////////////////////////////////////
void FabricModifier::ModifyObject( TimeValue t, ModContext &mc, ObjectState* os, INode *node )
{
	if (!m_binding.isValid())
		return;

	// If our graph won't evaluate, then we don't do anything
	if (!GraphCanEvaluate())
		return;

	TriObject* pTriObj = static_cast<TriObject*>(os->obj);
	// If our cached value is valid, just use that and return
	Interval ivValid = FOREVER;
	if (m_valid.InInterval(t)) 
	{
		pTriObj->GetMesh() = m_value;
		ivValid = m_valid;
	}
	else
	{
		// Send our input to Fabric
		// A modifier is a special kind of mesh, it
		// takes an input mesh and modifies it
		if (!m_inputValid.InInterval(t)) 
		{
			DoSyncing ds( *this );
			HoldSuspend hs; // Suspend undo, there is no need for FE to record this action
			m_inputValid.SetInfinite();
			MaxValuesToFabric<Object*, Mesh>(m_binding, m_inMeshPort.c_str(), t, m_inputValid, &os->obj, 1);
			ivValid &= m_inputValid;

			// If Fabric does not have an output value for us, then
			// nothing will be read into our cached value.  In this
			// case we don't want to change the base mesh, so ensure
			// that is what is in the cache.
			FabricCore::DFGExec exec = GetExec(nullptr);
			if (!(exec.haveExecPort(m_outArgName.c_str()) && exec.hasSrcPorts(m_outArgName.c_str()))) {
				m_value = pTriObj->GetMesh();
			}
		}

		// Evaluate the graph, get our results
		pTriObj->GetMesh() = Evaluate(t, ivValid);

		MeshNormalSpec* pNormalSpec = pTriObj->GetMesh().GetSpecifiedNormals();
		if (pNormalSpec != NULL)
		{
			// Indicate to the system that we're supporting the specified normals.
			pNormalSpec->SetFlag( MESH_NORMAL_MODIFIER_SUPPORT );

			// Indicate to the system that the non-explicit normals need recomputing:
			pNormalSpec->ClearFlag( MESH_NORMAL_NORMALS_COMPUTED );
		}
	}

	// We may have changed any of these attributes.
	// OPTIMIZE: We should not invalidate things we haven't changed
	// Perhaps we should add the ability to set channels via MxS
	pTriObj->UpdateValidity(GEOM_CHAN_NUM, ivValid);
	pTriObj->UpdateValidity(TOPO_CHAN_NUM, ivValid);
	pTriObj->UpdateValidity(TEXMAP_CHAN_NUM, ivValid);
	pTriObj->UpdateValidity(VERT_COLOR_CHAN_NUM, ivValid);
}

int FabricModifier::Display(TimeValue t, INode* inode, ViewExp* vpt, int flags)
{
	// TODO: Nitrous support here
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	pMeshNoConst->render(vpt->getGW(), inode->Mtls(), NULL, flags, inode->NumMtls());
	return 0;
}

int FabricModifier::HitTest(TimeValue t, INode* inode, int type, int crossing, 
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

void FabricModifier::Snap(TimeValue t, INode* inode, SnapInfo* snap, IPoint2* p, ViewExp* vpt)
{
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	Matrix3 tm = inode->GetNodeTM(t);
	pMeshNoConst->snap(vpt->getGW(), snap, p, tm);
}

void FabricModifier::GetWorldBoundBox(TimeValue t, INode* mat, ViewExp* /*vpt*/, Box3& box )
{
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	box = pMeshNoConst->getBoundingBox(&mat->GetNodeTM(t));
}

void FabricModifier::GetLocalBoundBox(TimeValue t, INode *mat, ViewExp * /*vpt*/, Box3& box )
{
	Interval ivDontCare;
	const Mesh& mesh = Evaluate(t, ivDontCare);
	Mesh* pMeshNoConst = const_cast<Mesh*>(&mesh);
	box = pMeshNoConst->getBoundingBox();
}

void FabricModifier::ResetPorts()
{
	MACROREC_GUARD;

	MAXSPLICE_CATCH_BEGIN

		// For a modifier, we create an IO port in order to more accurately represent max's method of working
		// with meshes.

		const char* metadata = "{ \n\
			\"uiHidden\" : \"true\", \n\
			\"uiPersistValue\" : \"false\" \n \
		}";

		const char* portSpec = MaxTypeToFabricType( GetValueType() );
		QString res = m_fabricCmdHandler.dfgDoAddPort( GetBinding(), "", GetExec( "" ), "baseMesh", FabricCore::DFGPortType_In, portSpec, "", "Geometry", metadata );
		m_inMeshPort = res.toStdString();

    ParentClass::ResetPorts();

		// We want to ensure this value does not show up as a parameter in Max
		if (!m_outArgName.empty())
		{
			// Ensure we have constructed a valid input value for Fabric
			FabricCore::RTVal defInVal = FabricCore::RTVal::Create(GetClient(), "PolygonMesh", 0, nullptr);
			GetBinding().setArgValue(m_inMeshPort.c_str(), defInVal, false);
			// Ensure we have constructed a valid input value for Fabric
			FabricCore::RTVal defOutVal = FabricCore::RTVal::Create(GetClient(), "PolygonMesh", 0, nullptr);
			GetBinding().setArgValue(m_outArgName.c_str(), defOutVal, false);
		}
	MAXSPLICE_CATCH_END
}

bool FabricModifier::CloneFabricData( ParentClass* pMyClone )
{
  static_cast<FabricModifier*>(pMyClone)->m_inMeshPort = m_inMeshPort;
  return true;
}


//////////////////////////////////////////////////////////////////////////
#define BASE_MESH_PORT_NAME		0x0100
#define IS_ENABLED_NAME			0x1000

IOResult FabricModifier::SaveImpData(ISave* isave)
{
	// Save additional values for derived values
	isave->BeginChunk(BASE_MESH_PORT_NAME);
	isave->WriteCString(m_inMeshPort.c_str());
	isave->EndChunk();

	ULONG dontcare;
	bool isenabled = Modifier::IsEnabled();
	isave->BeginChunk( IS_ENABLED_NAME );
	isave->Write( &isenabled, sizeof(isenabled), &dontcare );
	isave->EndChunk();

	return IO_OK;

}

IOResult FabricModifier::LoadImpData(ILoad* iload)
{
	IOResult result = IO_OK;
	while (IO_OK == (result = iload->OpenChunk()))
	{
		switch (iload->CurChunkID())
		{
		case BASE_MESH_PORT_NAME:
		{
			char *buff;
			if (iload->ReadCStringChunk(&buff) == IO_OK) {
				m_inMeshPort = buff;
			}
			break;
		}
		case IS_ENABLED_NAME:
		{
			bool isEnabled;
			ULONG dontcare;
			if (iload->Read( &isEnabled, sizeof(isEnabled), &dontcare ) == IO_OK) {
				if (isEnabled)
					EnableMod();
				else
					DisableMod();
			}
			break;
		}
		}
		iload->CloseChunk();
	}
	return result;
}
