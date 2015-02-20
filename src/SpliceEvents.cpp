#include "StdAfx.h"
#include "SpliceEvents.h"
#include <maxapi.h>
#include <GraphicsWindow.h>
#include "FabricCore.h"
#include "FabricSplice.h"
#include "SpliceLogging.h"
#include <map>
#include "Graphics\IDisplayManager.h"

SpliceEvents* SpliceEvents::s_Instance = nullptr;

SpliceEvents::SpliceEvents()
	: m_VptCallback(nullptr)
{
}

SpliceEvents::~SpliceEvents()
{
	UnHookMouseEvents();
	UnHookViewportRender();
}

//////////////////////////////////////////////////////////////////////////
#pragma region Mouse Events

void SpliceEvents::HookMouseEvents()
{
	if (!MouseEventsHooked())
		GetCOREInterface()->PushCommandMode(&m_MouseCommandMode);
}

void SpliceEvents::UnHookMouseEvents()
{
	Interface* pCore = GetCOREInterface();
	pCore->DeleteMode(&m_MouseCommandMode);
}

bool SpliceEvents::MouseEventsHooked()
{
	Interface* pCore = GetCOREInterface();
	return (pCore->GetCommandMode() == &m_MouseCommandMode);
}

#pragma endregion Mouse Events

//////////////////////////////////////////////////////////////////////////
#pragma region Viewport Rendering event

void doSpliceDrawing(void *data)
{
	// Invoke the Splice Drawing now that we have the OpenGL context bound.

	MAXSPLICE_CATCH_BEGIN();

		ViewExp *vpt = reinterpret_cast<ViewExp*>(data);
		int id = vpt->GetViewID();
		if(SpliceEvents::s_DrawContexts[id].isValid())
		{
			FabricSplice::SceneManagement::drawOpenGL(SpliceEvents::s_DrawContexts[id]);
		}

	MAXSPLICE_CATCH_END
}

FabricCore::RTVal SpliceEvents::s_DrawContexts[MAX_VPTS];

class SpliceViewportDrawCallback : public ViewportDisplayCallback
{
	//typedef std::map<int, FabricCore::RTVal*> ContextMap;
	//ContextMap m_DrawContexts;
	//FabricCore::RTVal* m_DrawContext[MAX_VPTS];
public:
	SpliceViewportDrawCallback()
	{
	}
	~SpliceViewportDrawCallback() 
	{
		ReleaseAllDrawContexts();
	}

	virtual void Display( TimeValue t, ViewExp *vpt, int flags )
	{
		MAXSPLICE_CATCH_BEGIN();

		if(!FabricSplice::SceneManagement::hasRenderableContent())
			return;
		
		if(ConfigureDrawContext(t, vpt)){
			// Trigger the callback which allows us to continue processing on the render thread.
			DWORD_PTR id = reinterpret_cast<DWORD_PTR>(vpt);
			int xyzCt = 0; 
			Point3 *xyz = NULL;
			int norCt = 0; 
			Point3 *nor = NULL;
			int texNum = 0; 
			int uvwCt = 0; 
			Point3 *uvw = NULL;
			int mtlCt = 0; 
			Material *mtl = NULL;

			GraphicsWindow *gw = vpt->getGW();
			// We call this function explicitly to load this ptr into the Id parameter.
			// If the id parameter is not set, our next call may fail
			gw->loadMeshData(id, xyzCt, xyz, norCt, nor, texNum, uvwCt, uvw, mtlCt, mtl);

			int stripCt = 0;
			// Because we can't control when this will be accessed on the Render thread, 
			// and it is passed by reference, we can't use a stack variable 
			// (it might get deallocated before the render thread renders)
			static StripTab stab; 
			gw->processStrips(id, stripCt, &stab, doSpliceDrawing);
		}

		MAXSPLICE_CATCH_END;
	}

	virtual void GetViewportRect( TimeValue t, ViewExp *vpt, Rect *rect )
	{
		rect->top = -INT_MAX;
		rect->left = -INT_MAX;
		rect->right = INT_MAX;
		rect->bottom = INT_MAX;
	}

	virtual BOOL Foreground() { return TRUE; }

	bool ConfigureDrawContext(TimeValue t, ViewExp* pView)
	{
		GraphicsWindow* gw = pView->getGW();
		
        ViewExp13* vp13 = NULL;
        if (pView->IsAlive())
            vp13 = reinterpret_cast<ViewExp13*>(pView->Execute(ViewExp::kEXECUTE_GET_VIEWEXP_13));
		else
			return false;

		FabricCore::RTVal drawContext = GetSpliceDrawContext(pView);
		// update the camera

		int width = gw->getWinSizeX();
		int height = gw->getWinSizeY();
		
		//////////////////////////
		// Setup the viewport
		FabricCore::RTVal inlineViewport = FabricSplice::constructObjectRTVal("InlineViewport");
		FabricCore::RTVal viewportDim = FabricSplice::constructRTVal("Vec2");
		viewportDim.setMember("x", FabricSplice::constructFloat64RTVal(width));
		viewportDim.setMember("y", FabricSplice::constructFloat64RTVal(height));
		inlineViewport.setMember("dimensions", viewportDim);

		{
			FabricCore::RTVal inlineCamera = FabricSplice::constructObjectRTVal("InlineCamera");

			bool isOrthographic = gw->isPerspectiveView() == FALSE;
			inlineCamera.setMember("isOrthographic", FabricSplice::constructBooleanRTVal(isOrthographic));

			Matrix3 tmView;
			pView->GetAffineTM(tmView);
			tmView.Invert();

			if(isOrthographic){
				//Don't know how the ortho scaling gets computed (FOV doesn't make sense), so extracting info from the matrix.
				Point3 ptInFrontOfCamera = tmView.GetTrans();
				float vptWidth = pView->GetVPWorldWidth(ptInFrontOfCamera);
				inlineCamera.setMember("orthographicFrustumH", FabricSplice::constructFloat64RTVal(vptWidth));
			}
			else{
				float fovX = pView->GetFOV();
				//convert to vertical fov as the RTR camera is always using this mode:
				double aspect = double(width) / double(height);
				double fovY = (2.0 * atan(1.0 / aspect * tan(fovX / 2.0)));
				inlineCamera.setMember("fovY", FabricSplice::constructFloat64RTVal(fovY));
			}
			
			float hither = vp13->GetHither();
			float yon = vp13->GetYon();
			inlineCamera.setMember("nearDistance", FabricSplice::constructFloat64RTVal(hither));
			inlineCamera.setMember("farDistance", FabricSplice::constructFloat64RTVal(yon));


          	FabricCore::RTVal cameraMat = FabricSplice::constructRTVal("Mat44");
			FabricCore::RTVal cameraMatData = cameraMat.callMethod("Data", "data", 0, 0);

			float * cameraMatFloats = (float*)cameraMatData.getData();
			if(cameraMat) {
				cameraMatFloats[0] = tmView.GetRow(0)[0];
				cameraMatFloats[1] = tmView.GetRow(1)[0];
				cameraMatFloats[2] = tmView.GetRow(2)[0];
				cameraMatFloats[3] = tmView.GetTrans()[0];
				cameraMatFloats[4] = tmView.GetRow(0)[1];
				cameraMatFloats[5] = tmView.GetRow(1)[1];
				cameraMatFloats[6] = tmView.GetRow(2)[1];
				cameraMatFloats[7] = tmView.GetTrans()[1];
				cameraMatFloats[8] =  tmView.GetRow(0)[2];
				cameraMatFloats[9] =  tmView.GetRow(1)[2];
				cameraMatFloats[10] = tmView.GetRow(2)[2];
				cameraMatFloats[11] = tmView.GetTrans()[2];
				cameraMatFloats[12] = 0.0f;
				cameraMatFloats[13] = 0.0f;
				cameraMatFloats[14] = 0.0f;
				cameraMatFloats[15] = 1.0f;

				inlineCamera.callMethod("", "setFromMat44", 1, &cameraMat);
			}
			inlineViewport.setMember("camera", inlineCamera);
		}
		drawContext.setMember("viewport", inlineViewport);
		return true;
	}

	FabricCore::RTVal GetSpliceDrawContext(ViewExp* pView)
	{
		int id = pView->GetViewID();
		if(!SpliceEvents::s_DrawContexts[id].isValid())
			SpliceEvents::s_DrawContexts[id] = FabricSplice::constructObjectRTVal("DrawContext");
		else if(SpliceEvents::s_DrawContexts[id].isNullObject())
			SpliceEvents::s_DrawContexts[id] = FabricSplice::constructObjectRTVal("DrawContext");
		return SpliceEvents::s_DrawContexts[id];
	}

	void ReleaseAllDrawContexts() 
	{
		for (int i = 0; i < MAX_VPTS; i++) {
			// We release our draw contexts by assigning blank RTVals over them
			SpliceEvents::s_DrawContexts[i] = FabricCore::RTVal();
		}
	}
};

void SpliceEvents::HookViewportRender()
{
	// Ideally, we'd like to detect if we are in OGL mode.
	// Unfortunately, its curiously difficult to figure this out (no API I can find)
	// Failing that, we can detect if we are in the most-likely alternative
	//if (getCfgMgr().keyExists(_T("GFXType"))) {
	//	getCfgMgr().getString(_T("GFXType"), buf, GW_MAX_FILE_LEN);

    if (MaxSDK::Graphics::IsRetainedModeEnabled())
	{
		static DWORD dontShowMsg(0);
		if (!dontShowMsg && !GetCOREInterface()->GetQuietMode())
		{
			MaxMsgBox(NULL, _T("For Fabric Engine rendering, please restart max in OGL graphics mode"), _T("Incompatible Render Mode"), MB_OK, MAX_MB_DONTSHOWAGAIN, &dontShowMsg);
		}
	}
	else 
	{
		if (m_VptCallback == nullptr) 
		{
			m_VptCallback = new SpliceViewportDrawCallback();
			GetCOREInterface()->RegisterViewportDisplayCallback(FALSE, m_VptCallback) ;
		}
	}
}

void SpliceEvents::UnHookViewportRender()
{
	if (m_VptCallback != nullptr)
	{
		GetCOREInterface()->UnRegisterViewportDisplayCallback(FALSE, m_VptCallback) ;
		SAFE_DELETE(m_VptCallback);
	}
}

bool SpliceEvents::ViewportRenderHooked()
{
	return m_VptCallback != nullptr;
}

void SpliceEvents::TriggerRedraw()
{
	if (m_VptCallback != nullptr)
	{
		GetCOREInterface()->NotifyViewportDisplayCallbackChanged(FALSE, m_VptCallback);
	}
}
#pragma endregion

//////////////////////////////////////////////////////////////////////////
SpliceEvents* SpliceEvents::GetInstance()
{
	if (s_Instance == nullptr)
	{
		s_Instance = new SpliceEvents();
	}
	return s_Instance;
}

void SpliceEvents::ReleaseInstance()
{
	SAFE_DELETE(s_Instance);
}

