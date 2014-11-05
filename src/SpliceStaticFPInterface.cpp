#include "StdAfx.h"
#include "SpliceStaticFPInterface.h"
#include "MaxScript/MaxScript.h"
#include "SpliceEvents.h"
#include "FabricSplice.h"
#include "FabricCore.h"

#if MAX_VERSION_MAJOR < 15
#define p_end end
#endif

SpliceStaticFPInterface* SpliceStaticFPInterface::GetInstance()
{
	// The single instance of this class
	// Initialize our instance.  This part is tells Max about the
	// functions exposed in our function map.
	static SpliceStaticFPInterface _theInstance(
		// Describe our interface
		ISPLICE_STATIC_INTERFACE , _T("Splice"), 0, NULL, FP_CORE,
		// Describe our function(s)
			SpliceStaticFPInterface::fn_showSceneGraphEditor, _T("ShowSceneGraphEditor"), 0, TYPE_BOOL, 0, 0, 
			SpliceStaticFPInterface::fn_importSpliceFile, _T("ImportSpliceFile"), 0, TYPE_BOOL, 0, 1,
				_M("file"),	0,	TYPE_TSTR_BV,
			SpliceStaticFPInterface::fn_exportSpliceFile, _T("ExportSpliceFile"), 0, TYPE_BOOL, 0, 2,
				_M("file"),	0,	TYPE_TSTR_BV,
				_M("spliceEntity"), 0, TYPE_REFTARG,
			
			SpliceStaticFPInterface::fn_getGlobalOperatorCount, _T("GetGlobalOperatorCount"), 0, TYPE_INT, 0, 0,
			SpliceStaticFPInterface::fn_getGlobalOperatorName, _T("GetGlobalOperatorName"), 0, TYPE_TSTR_BV, 0, 1,
				_M("index"),	0,	TYPE_INDEX,
			SpliceStaticFPInterface::fn_reloadFabricExtension, _T("LoadExtension"), 0, TYPE_BOOL, 0, 3,
				_M("extension"),	0,	TYPE_TSTR_BV,
				_M("version"),	0,	TYPE_TSTR_BV,
				_M("reload"),	0,	TYPE_bool,

		properties,
//			SpliceStaticFPInterface::prop_getPythonFile,SpliceStaticFPInterface::prop_setPythonFile, _T("PythonFile"), 0,TYPE_TSTR,
			SpliceStaticFPInterface::prop_getSpliceRendering,SpliceStaticFPInterface::prop_setSpliceRendering, _T("Rendering"), 0,TYPE_bool,
			SpliceStaticFPInterface::prop_getSpliceManip,SpliceStaticFPInterface::prop_setSpliceManip, _T("Manipulation"), 0,TYPE_bool,
//			SpliceStaticFPInterface::prop_getMaxImageSize,SpliceStaticFPInterface::prop_setMaxImageSize, _T("MaxImageSize"), 0,TYPE_INT,
//			SpliceStaticFPInterface::prop_getMaxTextureRenderThreads,SpliceStaticFPInterface::prop_setMaxTextureRenderThreads, _T("MaxTextureRenderThreads"), 0,TYPE_INT,
		p_end
		);
	return &_theInstance;
}

// Implement the functions exposed above
BOOL SpliceStaticFPInterface::ShowSceneGraphEditor()
{
	//if (CPApplication::m_applicationState == CPApplication::kReady)
	//{
	//	SGCAPI::Wrapper* pWrapper = CPApplication::GetInstance()->GetWrapper();
	//	if (pWrapper == NULL)
	//		return FALSE;

	//	SGCAPI::Widget widget = pWrapper->getApplication()->getWidget("Xfo Options");
	//	HWND mainWindowId = GetCOREInterface()->GetMAXHWnd();
	//	widget.show((long)mainWindowId);
	//	return TRUE;
	//}
	return FALSE;
}

ClassDesc2* GetClassDesc(int paramType)
{
	// TODO: Only the TYPE_MESH makes sense in a traditional
	// import scenario.  
	switch(paramType)
	{
	case TYPE_FLOAT:
	case TYPE_INT:
	case TYPE_BOOL:
		return SpliceTranslationLayer<Control, float>::GetClassDesc();
	case TYPE_POINT3:
		return SpliceTranslationLayer<Control, Point3>::GetClassDesc();
	//case TYPE_FRGBA:
	//case TYPE_POINT4:
	//	return SpliceTranslationLayer<Control, Point4>::GetClassDesc();
	case TYPE_QUAT:
		return SpliceTranslationLayer<Control, Quat>::GetClassDesc();
	case TYPE_MATRIX3:
		return SpliceTranslationLayer<Control, Matrix3>::GetClassDesc();
	case TYPE_MESH:
		return SpliceTranslationLayer<GeomObject, Mesh>::GetClassDesc();

	}
	return NULL;
}

BOOL SpliceStaticFPInterface::ImportSpliceFile(const TSTR& file)
{
	FabricSplice::DGGraph graph;
	// create a graph to hold our dependency graph nodes.
	graph = FabricSplice::DGGraph("myGraph");
	graph.constructDGNode();

	CStr cFile = file.ToCStr();
	bool res = graph.loadFromFile(cFile);
	if (!res)
		return false;

	// Find output values, and construct new classes for them
	int nPorts = graph.getDGPortCount();
	for (int i = 0; i < nPorts; i++)
	{
		std::string portName = graph.getDGPortName(i);
		FabricSplice::DGPort port = graph.getDGPort(i);
		if(!port.isValid())
			continue;

		const char* sDataType = port.getDataType();
		bool isArray = port.isArray();
		int paramType = SpliceTypeToMaxType(sDataType);
		FabricSplice::Port_Mode portMode = port.getMode();

		// For each IO or OUT port, create a Max translation class
		if (portMode != FabricSplice::Port_Mode_IN)
		{
			int nToCreate = 1;
			if (isArray)
				nToCreate = port.getSliceCount();

			for (int j = 0; j < nToCreate; j++)
			{
				ClassDesc2* pCD = GetClassDesc(paramType);
				if (pCD != NULL)
				{
					ReferenceTarget* pRef = reinterpret_cast<ReferenceTarget*>(pCD->Create(TRUE));

					SpliceTranslationFPInterface* pSpliceInterface = GetSpliceInterface(pRef);
					if (pSpliceInterface != NULL)
					{
						pSpliceInterface->SetSpliceGraph(graph, nullptr);
						pSpliceInterface->SetOutPort(port);
					}
				}
			}
		}
	}
	return FALSE;
}

BOOL SpliceStaticFPInterface::ExportSpliceFile(const MSTR& file, ReferenceTarget* spliceEntity) 
{
	SpliceTranslationFPInterface* pSpliceInterface = GetSpliceInterface(spliceEntity);
	if (pSpliceInterface == nullptr)
		return FALSE;

	CStr cFile = file.ToCStr();
	FabricSplice::DGGraph* pGraph = const_cast<FabricSplice::DGGraph*>(&pSpliceInterface->GetSpliceGraph());
	return pGraph->saveToFile(cFile.data());
}

int SpliceStaticFPInterface::GetGlobalKLOperatorCount()
{
	return FabricSplice::DGGraph::getGlobalKLOperatorCount();
}

MSTR SpliceStaticFPInterface::GetGlobalKLOperatorName(int index)
{
	const char* opName = FabricSplice::DGGraph::getGlobalKLOperatorName(index);
	MSTR rval;
	rval.FromACP(opName);
	return rval;
}


BOOL SpliceStaticFPInterface::LoadExtension( const MSTR& extension, const MSTR& version, bool reload)
{
	const FabricCore::Client* pClient = NULL;
	FECS_DGGraph_getClient(&pClient);

	// No client, nothing to do (the extension will be loaded when requested
	if(pClient == nullptr)
		return FALSE;

	CStr cExtension = extension.ToCStr();
	CStr cVersion = version.ToCStr();
	FEC_ClientLoadExtension(pClient->getFECClientRef(), cExtension.data(), cVersion.data(), reload);
	return TRUE;
}

bool SpliceStaticFPInterface::GetSpliceRendering()
{
	return SpliceEvents::GetInstance()->ViewportRenderHooked();
}

void SpliceStaticFPInterface::SetSpliceRendering(bool isRendering)
{
	if (isRendering)
	{
		SpliceEvents::GetInstance()->HookViewportRender();
	}
	else
		SpliceEvents::GetInstance()->UnHookViewportRender();
}


bool SpliceStaticFPInterface::GetSpliceManip()
{
	return SpliceEvents::GetInstance()->MouseEventsHooked();
}

void SpliceStaticFPInterface::SetSpliceManip(bool isManipulating)
{
	if (isManipulating)
	{
		SpliceEvents::GetInstance()->HookMouseEvents();
	}
	else
		SpliceEvents::GetInstance()->UnHookMouseEvents();
}
