
// We need to access private variables on the SelectionCallback
#include "StdAfx.h"
#include <list>
#include <maxapi.h>
#include <maxscript/maxscript.h>
#include "FabricMouseCallback.h"
#include <objmode.h>
#include "FabricCore.h"
#include "..\FabricEvents.h"
#include "..\Fabric3dsmax.h"
#include "..\FabricRestoreObjects.h"

// Include Event constants
#include "KbdTranslation.h"

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam);
bool SendKLEvent(FabricCore::RTVal& klevent, ViewExp& pView, int eventType) ;

FabricCore::RTVal FabricMouseCallback::mEventDispatcher;

FabricMouseCallback::FabricMouseCallback(void)
	: m_LMouseDown(false)
	, m_MMouseDown(false)
	, m_RMouseDown(false)
	, m_kbHookHandle(0)
{
}


FabricMouseCallback::~FabricMouseCallback(void)
{
}

int FabricMouseCallback::override( int mode )
{
	return CLICK_DRAG_CLICK;
}

BOOL FabricMouseCallback::SupportTransformGizmo()
{
	return FALSE;
}

void FabricMouseCallback::DeactivateTransformGizmo()
{
}

BOOL FabricMouseCallback::SupportAutoGrid()
{
	// TODO: How to integrate AutoGrid into Fabric?
	return TRUE;
}

BOOL FabricMouseCallback::TolerateOrthoMode()
{
	return TRUE;
}

FabricCore::RTVal SetupViewport(ViewExp* pView)
{
	GraphicsWindow* gw = pView->getGW();
	FabricCore::Client& client = GetClient();

	ViewExp13* vp13 = NULL;
	if (pView->IsAlive())
		vp13 = reinterpret_cast<ViewExp13*>(pView->Execute(ViewExp::kEXECUTE_GET_VIEWEXP_13));

	int width = gw->getWinSizeX();
	int height = gw->getWinSizeY();

	//////////////////////////
	// Setup the viewport
	FabricCore::RTVal inlineViewport = FabricCore::RTVal::Create(client, "InlineViewport", 0, nullptr);
	FabricCore::RTVal viewportDim = FabricCore::RTVal::Construct(client, "Vec2", 0, nullptr);
	viewportDim.setMember("x", FabricCore::RTVal::ConstructFloat64(client, width));
	viewportDim.setMember("y", FabricCore::RTVal::ConstructFloat64(client, height));
	inlineViewport.setMember("dimensions", viewportDim);

	{
		FabricCore::RTVal inlineCamera = FabricCore::RTVal::Create(client, "InlineCamera", 0, nullptr);

		bool isOrthographic = gw->isPerspectiveView() == FALSE;
		inlineCamera.setMember("isOrthographic", FabricCore::RTVal::ConstructBoolean(client, isOrthographic));

		Matrix3 tmView;
		pView->GetAffineTM(tmView);
		tmView.Invert();

		if(isOrthographic){
			//Don't know how the ortho scaling gets computed (FOV doesn't make sense), so extracting info from the matrix.
			Point3 ptInFrontOfCamera = tmView.GetTrans();
			float vptWidth = pView->GetVPWorldWidth(ptInFrontOfCamera);
			inlineCamera.setMember("orthographicFrustumH", FabricCore::RTVal::ConstructFloat64(client, vptWidth));
		}
		else{
			float fovX = pView->GetFOV();
			//convert to vertical fov as the RTR camera is always using this mode:
			double aspect = double(width) / double(height);
			double fovY = (2.0 * atan(1.0 / aspect * tan(fovX / 2.0)));
			inlineCamera.setMember("fovY", FabricCore::RTVal::ConstructFloat64(client, fovY));
		}

		float hither = vp13->GetHither();
		float yon = vp13->GetYon();
		inlineCamera.setMember("nearDistance", FabricCore::RTVal::ConstructFloat64(client, hither));
		inlineCamera.setMember("farDistance", FabricCore::RTVal::ConstructFloat64(client, yon));


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
		inlineViewport.setMember("camera", inlineCamera);
	}
	return inlineViewport;
}

int FabricMouseCallback::proc( HWND hwnd, int msg, int point, int flags, IPoint2 m )
{
	if (!mEventDispatcher.isValid())
		return FALSE;  // Return FALSE to exit this command mode

	if (msg == MOUSE_IDLE)
		return TRUE;

	MAXSPLICE_CATCH_BEGIN

	FabricCore::Client& client = GetClient();
	FabricCore::RTVal klevent = FabricCore::RTVal::Create(client, "MouseEvent", 0, nullptr);

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

	FabricCore::RTVal klpos = FabricCore::RTVal::Construct(client, "Vec2", 0, nullptr);
	klpos.setMember("x", FabricCore::RTVal::ConstructFloat32(client, (float)m.x));
	klpos.setMember("y", FabricCore::RTVal::ConstructFloat32(client, (float)m.y));
	klevent.setMember("pos", klpos);


	// In Max, we may have different behaviour, so figure out what we can do here
	klevent.setMember("button", FabricCore::RTVal::ConstructUInt32(client, buttons));
	klevent.setMember("buttons", FabricCore::RTVal::ConstructUInt32(client, buttons));

	SendKLEvent(klevent, GetCOREInterface7()->GetViewExp(hwnd), eventType);

	MAXSPLICE_CATCH_RETURN(FALSE);
	return 0;
}


void FabricMouseCallback::EnterMode()
{
	MAXSPLICE_CATCH_BEGIN;

	if (!AnyInstances()) {
		logMessage("Fabric Client not constructed yet. A Fabric Node must be created before the manipulation tool can be activated.");
		return;
	}

	// We should not re-enter this mode while already active
	//DbgAssert(!mEventDispatcher.isValid());

	FabricCore::Client& client = GetClient();
	client.loadExtension("Manipulation", "", false);

	FabricCore::RTVal eventDispatcherHandle = FabricCore::RTVal::Construct(client, "EventDispatcherHandle", 0, nullptr);
	mEventDispatcher = eventDispatcherHandle.callMethod("EventDispatcher", "getEventDispatcher", 0, 0);
	if (mEventDispatcher.isValid())
	{
		mEventDispatcher.callMethod("", "activateManipulation", 0, 0);
	}

	// Directly hook the keyboard proc, as Max doesn't seem to reliably pass on modifier keys.
	m_kbHookHandle = SetWindowsHookEx(WH_KEYBOARD,(HOOKPROC)KeyboardHookProc, NULL, GetCurrentThreadId());

	MAXSPLICE_CATCH_END
}

void FabricMouseCallback::ExitMode()
{
	MAXSPLICE_CATCH_BEGIN

	DbgAssert(!theHold.Holding());
	if (theHold.Holding())
		theHold.Accept(_T("ERROR - Fabric Actions"));

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

	MAXSPLICE_CATCH_END
}

LRESULT CALLBACK KeyboardHookProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	if (nCode == HC_ACTION) {
		FabricCore::RTVal klevent;
		
		int eventType = 0;
		if (HIWORD(lParam) & KF_UP) {
			FabricCore::Client& client = GetClient();

			eventType = Event_KeyRelease;
			klevent = FabricCore::RTVal::Create(client, "KeyEvent", 0, nullptr);
			klevent.setMember("key", FabricCore::RTVal::ConstructUInt32(client, KeyTbl[wParam&0xFF]));
			klevent.setMember("count", FabricCore::RTVal::ConstructUInt32(client, 0xFFFF & lParam));
		}
		else {
			FabricCore::Client& client = GetClient();

			eventType = Event_KeyPress;
			klevent = FabricCore::RTVal::Create(client, "KeyEvent", 0, nullptr);
			klevent.setMember("key", FabricCore::RTVal::ConstructUInt32(client, KeyTbl[wParam & 0xFF]));
			klevent.setMember("count", FabricCore::RTVal::ConstructUInt32(client, 0xFFFF & lParam));
			klevent.setMember("isAutoRepeat", FabricCore::RTVal::ConstructBoolean(client, HIWORD(lParam) & KF_REPEAT));
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

	// Trigger the event on Fabric
	FabricCore::RTVal inlineViewport = SetupViewport(&pView);
	FabricCore::Client& client = GetClient();

	//////////////////////////
	// Setup the Host
	// We cannot set an interface value via RTVals.
	FabricCore::RTVal host = FabricCore::RTVal::Create(client, "Host", 0, nullptr);
	host.setMember("hostName", FabricCore::RTVal::ConstructString(client, "3dsMax"));

	long modifiers = 0;
	if(GetKeyState(VK_CONTROL) & (1<<16))
		modifiers |= ModiferKey_Ctrl;

	if(GetKeyState(VK_MENU) & (1<<16))
		modifiers |= ModiferKey_Alt;

	if(GetKeyState(VK_SHIFT) & (1<<16))
		modifiers |= ModiferKey_Shift;

	//////////////////////////
	// Configure the event...
	{
		FabricCore::RTVal args[4];
		args[0] = host;
		args[1] = inlineViewport;
		args[2] = FabricCore::RTVal::ConstructUInt32(client, eventType);
		args[3] = FabricCore::RTVal::ConstructUInt32(client, modifiers);
		klevent.callMethod("", "init", 4, args);
	}
	//klevent.setMember("modifiers", FabricCore::RTVal::ConstructUInt32(client, modifiers));

	FabricMouseCallback::GetEventDispatcher().callMethod("Boolean", "onEvent", 1, &klevent);

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
				found = customCommand.find(std::string("(")) !=std::wstring::npos;
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
		//CustomKLUndoRedoCommandObject* pNewUndo = new CustomKLUndoRedoCommandObject(fabricUndoVal);
		//theHold.Put(pNewUndo);

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
				theHold.Accept(_T("Fabric Manipulation"));
		}
	}


	klevent.invalidate();
	return result;
}
