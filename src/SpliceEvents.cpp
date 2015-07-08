#include "StdAfx.h"
#include "SpliceEvents.h"
#include <maxapi.h>
#include <GraphicsWindow.h>
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

//FabricCore::RTVal SpliceEvents::s_DrawContexts[MAX_VPTS];
FabricCore::RTVal SpliceEvents::s_DrawContext;
FabricCore::RTVal SpliceEvents::s_InlineViewports[MAX_VPTS];
FabricCore::RTVal s_viewportIds[MAX_VPTS];

void doSpliceDrawing(void *data)
{
	// Invoke the Splice Drawing now that we have the OpenGL context bound.

	MAXSPLICE_CATCH_BEGIN();

		if (SpliceEvents::s_DrawContext.isValid())
		{
			ViewExp *vpt = reinterpret_cast<ViewExp*>(data);
			int id = vpt->GetViewID();
			if (SpliceEvents::s_InlineViewports[id].isValid())
			{
				FabricCore::RTVal args[2];
				args[0] = s_viewportIds[id];
				args[1] = SpliceEvents::s_DrawContext;
				FabricCore::RTVal success = GetDrawing().callMethod("Boolean", "drawViewport", 2, args);
				if (!success.getBoolean())
				{
					myLogFunc(NULL, "Drawing Failed", -1);
				}
			}
		}

	MAXSPLICE_CATCH_END
}


void ComputeFOVProjection(float result[4][4], float fov, float aspect, float nearDist, float farDist, bool leftHanded = true )
{
	//
	// General form of the Projection Matrix
	//
	// uh = Cot( fov/2 ) == 1/Tan(fov/2)
	// uw / uh = 1/aspect
	// 
	//   uw         0       0       0
	//    0        uh       0       0
	//    0         0      f/(f-n)  1
	//    0         0    -fn/(f-n)  0
	//
	// Make result to be identity first

	// check for bad parameters to avoid divide by zero:
	// if found, assert and return an identity matrix.
	if (fov <= 0 || aspect == 0)
	{
		DbgAssert(fov > 0 && aspect != 0);
		return;
	}

	float frustumDepth = farDist - nearDist;
	float oneOverDepth = 1 / frustumDepth;

	result[1][1] = 1 / tan(0.5f * fov);
	result[0][0] = (leftHanded ? 1 : -1) * result[1][1] / aspect;
	result[2][2] = farDist * oneOverDepth;
	result[3][2] = (-farDist * nearDist) * oneOverDepth;
	result[2][3] = 1;
	result[3][3] = 0;
}

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

		if (!AnyInstances())
			return;

		// Force the whole window to update.
		Rect rect;
		GraphicsWindow* gw = vpt->getGW();
		rect.top = 0; rect.bottom = gw->getWinSizeY(); rect.left = 0; rect.right = gw->getWinSizeX(); 
		vpt->InvalidateRect(rect);
		
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
		if (!drawContext.isValid())
			return false;

		// update the camera
		FabricCore::Client client = GetClient();
		float tSec = TicksToSec(t);
		drawContext.setMember("time", FabricCore::RTVal::ConstructFloat32(client, tSec));

		int width = gw->getWinSizeX();
		int height = gw->getWinSizeY();
		
		//////////////////////////
		// Setup the viewport
		int id = pView->GetViewID();
		FabricCore::RTVal& inlineViewport = GetSpliceViewport(pView);
		FabricCore::RTVal args[3];
		args[0] = drawContext;
		args[1] = FabricCore::RTVal::ConstructFloat64(client, width);
		args[2] = FabricCore::RTVal::ConstructFloat64(client, height);
		inlineViewport.callMethod("", "resize", 3, &args[0]);

		{
			FabricCore::RTVal inlineCamera = inlineViewport.callMethod("InlineCamera", "getCamera", 0, 0);

			//float viewProjTm[4][4];
			//Matrix3 invViewTm;
			//int persp;
			//float hither, yon;
			//gw->getCameraMatrix(viewProjTm, &invViewTm, &persp, &hither, &yon);
			float hither = vp13->GetHither();
			float yon = vp13->GetYon();

			float viewProjTm2[4][4];
			memset(viewProjTm2, 0, sizeof(float) * 16);
			ComputeFOVProjection(viewProjTm2, pView->GetFOV(), float(width) / height, hither, yon, false);

			//Camera maxCam;
			//maxCam.setClip(hither, yon);
			//maxCam.setPersp(pView->GetFOV(), float(width) / height);
			//maxCam.getProj(viewProjTm);

			FabricCore::RTVal projectionMatrixExtArray = FabricCore::RTVal::ConstructExternalArray(client, "Float32", 16, viewProjTm2);
			FabricCore::RTVal projectionVal = inlineCamera.maybeGetMemberRef("projection");
			projectionVal.callMethod("", "set", 1, &projectionMatrixExtArray);

			bool isOrthographic = gw->isPerspectiveView() == FALSE;
			FabricCore::RTVal param = FabricCore::RTVal::ConstructBoolean(client, isOrthographic);
			inlineCamera.callMethod("", "setOrthographic", 1, &param);

			Matrix3 tmView;
			pView->GetAffineTM(tmView);
			tmView.Invert();

			if(isOrthographic){
				//Don't know how the ortho scaling gets computed (FOV doesn't make sense), so extracting info from the matrix.
				Point3 ptInFrontOfCamera = tmView.GetTrans();
				float vptWidth = pView->GetVPWorldWidth(ptInFrontOfCamera);
				FabricCore::RTVal rtVptWidth = FabricCore::RTVal::ConstructFloat64(client, vptWidth);
				inlineCamera.callMethod("", "setOrthographicFrustumHeight", 1, &rtVptWidth);
			}
			else{
				float fovX = pView->GetFOV();
				//convert to vertical fov as the RTR camera is always using this mode:
				double aspect = double(width) / double(height);
				double fovY = (2.0 * atan(1.0 / aspect * tan(fovX / 2.0)));
				FabricCore::RTVal rtFovY = FabricCore::RTVal::ConstructFloat64(client, fovY);
				inlineCamera.callMethod("", "setFovY", 1, &rtFovY);
			}
			
			FabricCore::RTVal rtHither = FabricCore::RTVal::ConstructFloat64(client, hither);
			inlineCamera.callMethod("", "setNearDistance", 1, &rtHither);
			FabricCore::RTVal rtYon = FabricCore::RTVal::ConstructFloat64(client, yon);
			inlineCamera.callMethod("", "setFarDistance", 1, &rtYon);


			FabricCore::RTVal cameraMat = FabricCore::RTVal::Construct(client, "Mat44", 0, nullptr);
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
		}
		//drawContext.setMember("viewport", inlineViewport);
		return true;
	}

	FabricCore::RTVal& GetSpliceViewport(ViewExp* pView)
	{
		int id = pView->GetViewID();
		FabricCore::RTVal& rtViewport = SpliceEvents::s_InlineViewports[id];
		DbgAssert(id < MAX_VPTS);
		if (rtViewport.isValid())
			return rtViewport;

		// Create viewport ID
		char buff[10];
		sprintf_s(buff, 10, "vpt%d", id);
		s_viewportIds[id] = FabricCore::RTVal::ConstructString(GetClient(), buff);

		// Create viewport
		rtViewport = FabricCore::RTVal::Create(GetClient(), "InlineViewport", 0, nullptr);
		if (!rtViewport.isValid() || rtViewport.isNullObject())
			throw FabricCore::Exception("Cannot create Fabric Viewport (Extension loaded?)");

		rtViewport.callMethod("", "setName", 1, &s_viewportIds[id]);
		rtViewport.callMethod("", "setup", 1, &GetSpliceDrawContext(pView));

		// Register viewport with drawing system
		FabricCore::RTVal args[2];
		args[0] = s_viewportIds[id];
		args[1] = rtViewport;
		GetDrawing().callMethod("", "registerViewport", 2, args);

		return rtViewport;
	}

	FabricCore::RTVal GetSpliceDrawContext(ViewExp* pView)
	{
		if (!SpliceEvents::s_DrawContext.isValid() || SpliceEvents::s_DrawContext.isNullObject())
		{
			try {
				SpliceEvents::s_DrawContext = FabricCore::RTVal::Construct(GetClient(), "DrawContext", 0, nullptr);
			}
			catch (FabricCore::Exception e) {
				// Empty catch.  This will happen if the user has not loaded InlineDrawing yet
				return FabricCore::RTVal();
			}

			// We may get an invalid RTValue here if InlineDrawing hasn't been loaded yet.
			if (SpliceEvents::s_DrawContext.isValid())
			{
				SpliceEvents::s_DrawContext = SpliceEvents::s_DrawContext.callMethod("DrawContext", "getInstance", 0, 0);
			}
		}
		return SpliceEvents::s_DrawContext;
	}

	void ReleaseAllDrawContexts() 
	{
		SpliceEvents::s_DrawContext = FabricCore::RTVal();
		for (int i = 0; i < MAX_VPTS; i++) {
			// We release our draw contexts by assigning blank RTVals over them
			SpliceEvents::s_InlineViewports[i] = FabricCore::RTVal();
			s_viewportIds[i] = FabricCore::RTVal();
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

