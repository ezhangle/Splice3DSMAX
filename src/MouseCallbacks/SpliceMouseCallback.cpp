
// We need to access private variables on the SelectionCallback
#include "StdAfx.h"
#include <list>
#include <maxapi.h>
#include <maxscript/maxscript.h>
#include "SpliceMouseCallback.h"
#include <objmode.h>
#include "FabricCore.h"
#include "FabricSplice.h"
#include "..\SpliceEvents.h"
#include "..\Splice3dsmax.h"
#include "..\SpliceRestoreObjects.h"

// Include Event constants
#include "KbdTranslation.h"

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
bool SendKLEvent(FabricCore::RTVal& klevent, ViewExp& pView, int eventType) ;

FabricCore::RTVal SpliceMouseCallback::mEventDispatcher;

SpliceMouseCallback::SpliceMouseCallback(void)
	: m_LMouseDown(false)
	, m_MMouseDown(false)
	, m_RMouseDown(false)
	, m_kbHookHandle(0)
{
}


SpliceMouseCallback::~SpliceMouseCallback(void)
{
}

int SpliceMouseCallback::override( int mode )
{
	return CLICK_DRAG_CLICK;
}

BOOL SpliceMouseCallback::SupportTransformGizmo()
{
	return FALSE;
}

void SpliceMouseCallback::DeactivateTransformGizmo()
{
}

BOOL SpliceMouseCallback::SupportAutoGrid()
{
	// TODO: How to integrate AutoGrid into Splice?
	return TRUE;
}

BOOL SpliceMouseCallback::TolerateOrthoMode()
{
	return TRUE;
}

FabricCore::RTVal SetupViewport(ViewExp* pView)
{
	GraphicsWindow* gw = pView->getGW();

	ViewExp13* vp13 = NULL;
	if (pView->IsAlive())
		vp13 = reinterpret_cast<ViewExp13*>(pView->Execute(ViewExp::kEXECUTE_GET_VIEWEXP_13));

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
	return inlineViewport;
}

int SpliceMouseCallback::proc( HWND hwnd, int msg, int point, int flags, IPoint2 m )
{
	if (!mEventDispatcher.isValid())
		return FALSE;  // Return FALSE to exit this command mode

	if (msg == MOUSE_IDLE)
		return TRUE;

	MAXSPLICE_CATCH_BEGIN()

	// Hide the cursor (we assume here for the point of Siggraph demo's that we are in painting mode)
	SetCursor(NULL);

	FabricCore::RTVal klevent = FabricSplice::constructObjectRTVal("MouseEvent");

	int eventType;

	if(msg == MOUSE_KEYBOARD){
		// the key event is handled by the KeyboardHookProc
		return TRUE;
	}
	else if(msg == MOUSE_FREEMOVE){
		eventType = Event_MouseMove;

		// Generate mouse up events
		if (m_LMouseDown && (flags&MOUSE_LBUTTON) == 0)
		{
			eventType = Event_MouseButtonRelease;
			m_LMouseDown = false;
		}

		if (m_RMouseDown && (flags&MOUSE_RBUTTON) == 0)
		{
			eventType = Event_MouseButtonRelease;
			m_RMouseDown = false;
		}

		if (m_MMouseDown && (flags&MOUSE_MBUTTON) == 0)
		{
			eventType = Event_MouseButtonRelease;
			m_MMouseDown = false;
		}
	}
	else if(msg == MOUSE_POINT){
		eventType = Event_MouseButtonPress;
		// Generate mouse down events
		if (!m_LMouseDown && flags&MOUSE_LBUTTON)
			m_LMouseDown = true;

		if (!m_RMouseDown && flags&MOUSE_RBUTTON)
			m_RMouseDown = true;

		if (!m_MMouseDown && flags&MOUSE_MBUTTON)
			m_MMouseDown = true;
	}
	else if(msg == MOUSE_DBLCLICK){
		eventType = Event_MouseButtonDblClick;

		if (!m_LMouseDown && flags&MOUSE_LBUTTON)
		{
			eventType = Event_MouseButtonPress;
			m_LMouseDown = true;
		}

		if (!m_RMouseDown && flags&MOUSE_RBUTTON)
		{
			eventType = Event_MouseButtonPress;
			m_RMouseDown = true;
		}

		if (!m_MMouseDown && flags&MOUSE_MBUTTON)
		{
			eventType = Event_MouseButtonPress;
			m_MMouseDown = true;
		}
		
	}
	else if(msg == MOUSE_MOVE){
		eventType = Event_MouseMove;
	}
	else{
		eventType = Event_MouseMove;
	}
	
	long buttons = 0;
	if(m_LMouseDown)
		buttons += MouseButton_LeftButton;
	if(m_RMouseDown)
		buttons += MouseButton_RightButton;
	if(m_MMouseDown)
		buttons += MouseButton_MiddleButton;

	FabricCore::RTVal klpos = FabricSplice::constructRTVal("Vec2");
	klpos.setMember("x", FabricSplice::constructFloat32RTVal((float)m.x));
	klpos.setMember("y", FabricSplice::constructFloat32RTVal((float)m.y));
	klevent.setMember("pos", klpos);


	// In Max, we may have different behaviour, so figure out what we can do here
	klevent.setMember("button", FabricSplice::constructUInt32RTVal(buttons));
	klevent.setMember("buttons", FabricSplice::constructUInt32RTVal(buttons));

	SendKLEvent(klevent, GetCOREInterface7()->GetViewExp(hwnd), eventType);

	MAXSPLICE_CATCH_RETURN(FALSE);
	return 0;
}


void SpliceMouseCallback::EnterMode()
{
	MAXSPLICE_CATCH_BEGIN();

	const FabricCore::Client * client = NULL;
	FECS_DGGraph_getClient(&client);

	if(!client){
		logMessage("Fabric Client not constructed yet. A Splice Node must be created before the manipulation tool can be activated.");
		return;
	}

	// We should not re-enter this mode while already active
	//DbgAssert(!mEventDispatcher.isValid());

    FabricCore::RTVal eventDispatcherHandle = FabricSplice::constructObjectRTVal("EventDispatcherHandle");
    mEventDispatcher = eventDispatcherHandle.callMethod("EventDispatcher", "getEventDispatcher", 0, 0);
	if (mEventDispatcher.isValid())
	{
		mEventDispatcher.callMethod("", "activateManipulation", 0, 0);
	}

	// Directly hook the keyboard proc, as Max doesn't seem to reliably pass on modifier keys.
	m_kbHookHandle = SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)KeyboardHookProc, NULL, GetCurrentThreadId());

	MAXSPLICE_CATCH_END()
}

void SpliceMouseCallback::ExitMode()
{
	MAXSPLICE_CATCH_BEGIN()

	DbgAssert(!theHold.Holding());
	if (theHold.Holding())
		theHold.Accept(_T("ERROR - Splice Actions"));

	if (mEventDispatcher.isValid())
	{
		// By deactivating the manipulation, we enable the manipulators to perform
		// cleanup, such as hiding paint brushes/gizmos. 
		mEventDispatcher.callMethod("", "deactivateManipulation", 0, 0);
		mEventDispatcher.invalidate();
	}

	// Release our windows hook, we don't need it anymore.
	UnhookWindowsHookEx(m_kbHookHandle);
	m_kbHookHandle = NULL;

	MAXSPLICE_CATCH_END()
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) {
		FabricCore::RTVal klevent;
		
		int eventType = 0;
		if (HIWORD(lParam) & KF_UP) {
			eventType = Event_KeyRelease;
			klevent = FabricSplice::constructObjectRTVal("KeyEvent");
			klevent.setMember("key", FabricSplice::constructUInt32RTVal(KeyTbl[wParam&0xFF]));
			klevent.setMember("count", FabricSplice::constructUInt32RTVal(0xFFFF & lParam));
		}
		else {
			eventType = Event_KeyPress;
			klevent = FabricSplice::constructObjectRTVal("KeyEvent");
			klevent.setMember("key", FabricSplice::constructUInt32RTVal(KeyTbl[wParam&0xFF]));
			klevent.setMember("count", FabricSplice::constructUInt32RTVal(0xFFFF & lParam));
			klevent.setMember("isAutoRepeat", FabricSplice::constructBooleanRTVal(HIWORD(lParam) & KF_REPEAT));
		}
		
		if (klevent.isValid()) {
			ViewExp& activeViewport = GetCOREInterface()->GetActiveViewExp();
			bool shouldStop = SendKLEvent(klevent, activeViewport, eventType);
			if (shouldStop)
				return 1;
		}
	}
	return CallNextHookEx(NULL, nCode, wParam, lParam);
}


bool SendKLEvent(FabricCore::RTVal& klevent, ViewExp& pView, int eventType) 
{

	// Trigger the event on Splice
	FabricCore::RTVal inlineViewport = SetupViewport(&pView);

	//////////////////////////
	// Setup the Host
	// We cannot set an interface value via RTVals.
	FabricCore::RTVal host = FabricSplice::constructObjectRTVal("Host");
	host.setMember("hostName", FabricSplice::constructStringRTVal("3dsMax"));

	long modifiers = 0;
	if(GetKeyState(VK_CONTROL) & (1<<16))
		modifiers |= ModiferKey_Ctrl;

	if(GetKeyState(VK_MENU) & (1<<16))
		modifiers |= ModiferKey_Alt;

	if(GetKeyState(VK_SHIFT) & (1<<16))
		modifiers |= ModiferKey_Shift;

	//////////////////////////
	// Configure the event...
	std::vector<FabricCore::RTVal> args(4);
	args[0] = host;
	args[1] = inlineViewport;
	args[2] = FabricSplice::constructUInt32RTVal(eventType);
	args[3] = FabricSplice::constructUInt32RTVal(modifiers);
	klevent.callMethod("", "init", 4, &args[0]);
	//klevent.setMember("modifiers", FabricSplice::constructUInt32RTVal(modifiers));

	SpliceMouseCallback::GetEventDispatcher().callMethod("Boolean", "onEvent", 1, &klevent);

	bool result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();

	// The manipulation system has requested that a custom command be invoked. 
	// Invoke the custom command passing the speficied args. 
	std::string customCommand(host.maybeGetMember("customCommand").getStringCString());
	if(customCommand != std::string("")){
		BOOL cmdSuccess;
		BOOL quietErrors = FALSE;
		FabricCore::RTVal customCommandArgs = host.maybeGetMember("customCommandArgs");
		bool found = customCommand.find(std::string("=")) !=std::string::npos;
		if (found){
			// Setting a value in Max.
			// e.g. 
			// $Sphere01.radius = 3.0
			std::string args;
			for(uint32_t i=0; i<customCommandArgs.getArraySize(); i++){
				args += std::string(customCommandArgs.getArrayElement(i).getStringCString());
			}
			cmdSuccess = ExecuteMAXScriptScript(TSTR::FromCStr((customCommand + args).data()), quietErrors);
		}
		else{
			// Calling a functions in MaxScript.
			if(customCommandArgs.getArraySize() == 0){
				// No args
				// e.g. 
				// theHold.Begin()
				bool found = customCommand.find(std::string("(")) !=std::wstring::npos;
				if(found)
					cmdSuccess = ExecuteMAXScriptScript(TSTR::FromCStr(customCommand.data()), quietErrors);
				else
					cmdSuccess = ExecuteMAXScriptScript(TSTR::FromCStr((customCommand + std::string("()")).data()), quietErrors);
			}
			else{
				// With args
				// e.g. 
				// theHold.Accept "MyChanges"
				std::string args;
				for(uint32_t i=0; i<customCommandArgs.getArraySize(); i++){
					args += std::string(" ");
					args += std::string(customCommandArgs.getArrayElement(i).getStringCString());
				}
				cmdSuccess = ExecuteMAXScriptScript(TSTR::FromCStr((customCommand + args).data()), quietErrors);
			}
		}
	}

	if(host.maybeGetMember("redrawRequested").getBoolean())	{
		Interface7* pCore = GetCOREInterface7();
		ViewExp10* vp10 = NULL;
		if (pView.IsAlive()){
			vp10 = reinterpret_cast<ViewExp10*>(pView.Execute(ViewExp::kEXECUTE_GET_VIEWEXP_10));
			vp10->Invalidate(true);
			pCore->RedrawViewportsLater(pCore->GetTime());
		}
	}

	if(host.callMethod("Boolean", "undoRedoCommandsAdded", 0, 0).getBoolean()){
		bool bCloseHold = false;
		if (!theHold.Holding()){
			theHold.Begin();
			bCloseHold = true;
		}
		FabricCore::RTVal fabricUndoVal = host.callMethod("UndoRedoCommand[]", "getUndoRedoCommands", 0, 0);
		CustomKLUndoRedoCommandObject* pNewUndo = new CustomKLUndoRedoCommandObject(fabricUndoVal);
		theHold.Put(pNewUndo);

		// if we are in mouse-up, then close our hold
		if (bCloseHold){
			if (fabricUndoVal.getArraySize() == 1){
				std::string desc(fabricUndoVal.getArrayElement(0).callMethod("String", "getDesc", 0, 0).getStringCString());
#ifdef _UNICODE
				theHold.Accept(MSTR::FromACP(desc.data()));
#else
				theHold.Accept((desc + std::string("\n")).data());
#endif
			}
			else	
				theHold.Accept(_T("Splice Manipulation"));
		}
	}


	klevent.invalidate();
	return result;
}
