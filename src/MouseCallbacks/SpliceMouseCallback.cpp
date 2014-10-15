
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

class CustomKLUndoRedoCommandObject : public RestoreObj
{
	FabricCore::RTVal m_rtval_commands;

public:
	CustomKLUndoRedoCommandObject(FabricCore::RTVal& commands) 
	{
		m_rtval_commands = commands;
	}
	~CustomKLUndoRedoCommandObject() 
	{
		m_rtval_commands.invalidate();
	};
	
	virtual void Restore( int isUndo )
	{
		if(m_rtval_commands.isValid())
		{
			for(int i=0; i< m_rtval_commands.getArraySize(); i++)
			{
				m_rtval_commands.getArrayElement(i).callMethod("", "undoAction", 0, 0);
			}
		}
	}

	virtual void Redo()
	{
		if(m_rtval_commands.isValid())
		{
			for(int i = 0; i < m_rtval_commands.getArraySize(); i++)
			{
				m_rtval_commands.getArrayElement(i).callMethod("", "doAction", 0, 0);
			}
		}
	}

};
// The following IDs are taken from ManipulationConstants.kl in the 'Manipulation' extension.
// The IDs cover all of the event types propagated by Qt.
// A given DCC will not support all event types, but the IDs must match between
// all DCC, (and standalone tools built using Qt, PyQt, PySide or toher frameworks).

typedef unsigned int EventType;

const EventType Event_None =  0;  //  Not an event.
const EventType Event_ActionAdded = 114;  //  A new action has been added (QActionEvent).
const EventType Event_ActionChanged = 113;  //  An action has been changed (QActionEvent).
const EventType Event_ActionRemoved = 115;  //  An action has been removed (QActionEvent).
const EventType Event_ActivationChange =  99; //  A widget's top-level window activation state has changed.
const EventType Event_ApplicationActivate = 121;  //  This enum has been deprecated. Use ApplicationStateChange instead.
const EventType Event_ApplicationDeactivate = 122;  //  This enum has been deprecated. Use ApplicationStateChange instead.
const EventType Event_ApplicationFontChange = 36; //  The default application font has changed.
const EventType Event_ApplicationLayoutDirectionChange =  37; //  The default application layout direction has changed.
const EventType Event_ApplicationPaletteChange =  38; //  The default application palette has changed.
const EventType Event_ApplicationStateChange =  214;  //  The state of the application has changed.
const EventType Event_ApplicationWindowIconChange = 35; //  The application's icon has changed.
const EventType Event_ChildAdded =  68; //  An object gets a child (QChildEvent).
const EventType Event_ChildPolished = 69; //  A widget child gets polished (QChildEvent).
const EventType Event_ChildRemoved =  71; //  An object loses a child (QChildEvent).
const EventType Event_Clipboard = 40; //  The clipboard contents have changed (QClipboardEvent).
const EventType Event_Close = 19; //  Widget was closed (QCloseEvent).
const EventType Event_CloseSoftwareInputPanel = 200;  //  A widget wants to close the software input panel (SIP).
const EventType Event_ContentsRectChange =  178;  //  The margins of the widget's content rect changed.
const EventType Event_ContextMenu = 82; //  Context popup menu (QContextMenuEvent).
const EventType Event_CursorChange =  183;  //  The widget's cursor has changed.
const EventType Event_DeferredDelete =  52; //  The object will be deleted after it has cleaned up (QDeferredDeleteEvent).
const EventType Event_DragEnter = 60; //  The cursor enters a widget during a drag and drop operation (QDragEnterEvent).
const EventType Event_DragLeave = 62; //  The cursor leaves a widget during a drag and drop operation (QDragLeaveEvent).
const EventType Event_DragMove =  61; //  A drag and drop operation is in progress (QDragMoveEvent).
const EventType Event_Drop =  63; //  A drag and drop operation is completed (QDropEvent).
const EventType Event_DynamicPropertyChange = 170;  //  A dynamic property was added, changed, or removed from the object.
const EventType Event_EnabledChange = 98; //  Widget's enabled state has changed.
const EventType Event_Enter = 10; //  Mouse enters widget's boundaries (QEnterEvent).
const EventType Event_EnterEditFocus =  150;  //  An editor widget gains focus for editing. QT_KEYPAD_NAVIGATION must be defined.
const EventType Event_EnterWhatsThisMode =  124;  //  Send to toplevel widgets when the application enters "What's This?" mode.
const EventType Event_Expose =  206;  //  Sent to a window when its on-screen contents are invalidated and need to be flushed from the backing store.
const EventType Event_FileOpen =  116;  //  File open request (QFileOpenEvent).
const EventType Event_FocusIn = 8;  //  Widget or Window gains keyboard focus (QFocusEvent).
const EventType Event_FocusOut =  9;  //  Widget or Window loses keyboard focus (QFocusEvent).
const EventType Event_FocusAboutToChange =  23; //  Widget or Window focus is about to change (QFocusEvent)
const EventType Event_FontChange =  97; //  Widget's font has changed.
const EventType Event_Gesture = 198;  //  A gesture was triggered (QGestureEvent).
const EventType Event_GestureOverride = 202;  //  A gesture override was triggered (QGestureEvent).
const EventType Event_GrabKeyboard =  188;  //  Item gains keyboard grab (QGraphicsItem only).
const EventType Event_GrabMouse = 186;  //  Item gains mouse grab (QGraphicsItem only).
const EventType Event_GraphicsSceneContextMenu =  159;  //  Context popup menu over a graphics scene (QGraphicsSceneContextMenuEvent).
const EventType Event_GraphicsSceneDragEnter =  164;  //  The cursor enters a graphics scene during a drag and drop operation (QGraphicsSceneDragDropEvent).
const EventType Event_GraphicsSceneDragLeave =  166;  //  The cursor leaves a graphics scene during a drag and drop operation (QGraphicsSceneDragDropEvent).
const EventType Event_GraphicsSceneDragMove = 165;  //  A drag and drop operation is in progress over a scene (QGraphicsSceneDragDropEvent).
const EventType Event_GraphicsSceneDrop = 167;  //  A drag and drop operation is completed over a scene (QGraphicsSceneDragDropEvent).
const EventType Event_GraphicsSceneHelp = 163;  //  The user requests help for a graphics scene (QHelpEvent).
const EventType Event_GraphicsSceneHoverEnter = 160;  //  The mouse cursor enters a hover item in a graphics scene (QGraphicsSceneHoverEvent).
const EventType Event_GraphicsSceneHoverLeave = 162;  //  The mouse cursor leaves a hover item in a graphics scene (QGraphicsSceneHoverEvent).
const EventType Event_GraphicsSceneHoverMove =  161;  //  The mouse cursor moves inside a hover item in a graphics scene (QGraphicsSceneHoverEvent).
const EventType Event_GraphicsSceneMouseDoubleClick = 158;  //  Mouse press again (double click) in a graphics scene (QGraphicsSceneMouseEvent).
const EventType Event_GraphicsSceneMouseMove =  155;  //  Move mouse in a graphics scene (QGraphicsSceneMouseEvent).
const EventType Event_GraphicsSceneMousePress = 156;  //  Mouse press in a graphics scene (QGraphicsSceneMouseEvent).
const EventType Event_GraphicsSceneMouseRelease = 157;  //  Mouse release in a graphics scene (QGraphicsSceneMouseEvent).
const EventType Event_GraphicsSceneMove = 182;  //  Widget was moved (QGraphicsSceneMoveEvent).
const EventType Event_GraphicsSceneResize = 181;  //  Widget was resized (QGraphicsSceneResizeEvent).
const EventType Event_GraphicsSceneWheel =  168;  //  Mouse wheel rolled in a graphics scene (QGraphicsSceneWheelEvent).
const EventType Event_Hide =  18; //  Widget was hidden (QHideEvent).
const EventType Event_HideToParent =  27; //  A child widget has been hidden.
const EventType Event_HoverEnter =  127;  //  The mouse cursor enters a hover widget (QHoverEvent).
const EventType Event_HoverLeave =  128;  //  The mouse cursor leaves a hover widget (QHoverEvent).
const EventType Event_HoverMove = 129;  //  The mouse cursor moves inside a hover widget (QHoverEvent).
const EventType Event_IconDrag =  96; //  The main icon of a window has been dragged away (QIconDragEvent).
const EventType Event_IconTextChange =  101;  //  Widget's icon text has been changed.
const EventType Event_InputMethod = 83; //  An input method is being used (QInputMethodEvent).
const EventType Event_InputMethodQuery =  207;  //  A input method query event (QInputMethodQueryEvent)
const EventType Event_KeyboardLayoutChange =  169;  //  The keyboard layout has changed.
const EventType Event_KeyPress =  6;  //  Key press (QKeyEvent).
const EventType Event_KeyRelease =  7;  //  Key release (QKeyEvent).
const EventType Event_LanguageChange =  89; //  The application translation changed.
const EventType Event_LayoutDirectionChange = 90; //  The direction of layouts changed.
const EventType Event_LayoutRequest = 76; //  Widget layout needs to be redone.
const EventType Event_Leave = 11; //  Mouse leaves widget's boundaries.
const EventType Event_LeaveEditFocus =  151;  //  An editor widget loses focus for editing. QT_KEYPAD_NAVIGATION must be defined.
const EventType Event_LeaveWhatsThisMode =  125;  //  Send to toplevel widgets when the application leaves "What's This?" mode.
const EventType Event_LocaleChange =  88; //  The system locale has changed.
const EventType Event_NonClientAreaMouseButtonDblClick =  176;  //  A mouse double click occurred outside the client area.
const EventType Event_NonClientAreaMouseButtonPress = 174;  //  A mouse button press occurred outside the client area.
const EventType Event_NonClientAreaMouseButtonRelease = 175;  //  A mouse button release occurred outside the client area.
const EventType Event_NonClientAreaMouseMove =  173;  //  A mouse move occurred outside the client area.
const EventType Event_MacSizeChange = 177;  //  The user changed his widget sizes (Mac OS X only).
const EventType Event_MetaCall =  43; //  An asynchronous method invocation via QMetaObject::invokeMethod().
const EventType Event_ModifiedChange =  102;  //  Widgets modification state has been changed.
const EventType Event_MouseButtonDblClick = 4;  //  Mouse press again (QMouseEvent).
const EventType Event_MouseButtonPress =  2;  //  Mouse press (QMouseEvent).
const EventType Event_MouseButtonRelease =  3;  //  Mouse release (QMouseEvent).
const EventType Event_MouseMove = 5;  //  Mouse move (QMouseEvent).
const EventType Event_MouseTrackingChange = 109;  //  The mouse tracking state has changed.
const EventType Event_Move =  13; //  Widget's position changed (QMoveEvent).
const EventType Event_OrientationChange = 208;  //  The screens orientation has changes (QScreenOrientationChangeEvent)
const EventType Event_Paint = 12; //  Screen update necessary (QPaintEvent).
const EventType Event_PaletteChange = 39; //  Palette of the widget changed.
const EventType Event_ParentAboutToChange = 131;  //  The widget parent is about to change.
const EventType Event_ParentChange =  21; //  The widget parent has changed.
const EventType Event_PlatformPanel = 212;  //  A platform specific panel has been requested.
const EventType Event_Polish =  75; //  The widget is polished.
const EventType Event_PolishRequest = 74; //  The widget should be polished.
const EventType Event_QueryWhatsThis =  123;  //  The widget should accept the event if it has "What's This?" help.
const EventType Event_RequestSoftwareInputPanel = 199;  //  A widget wants to open a software input panel (SIP).
const EventType Event_Resize =  14; //  Widget's size changed (QResizeEvent).
const EventType Event_ScrollPrepare = 204;  //  The object needs to fill in its geometry information (QScrollPrepareEvent).
const EventType Event_Scroll =  205;  //  The object needs to scroll to the supplied position (QScrollEvent).
const EventType Event_Shortcut =  117;  //  Key press in child for shortcut key handling (QShortcutEvent).
const EventType Event_ShortcutOverride =  51; //  Key press in child, for overriding shortcut key handling (QKeyEvent).
const EventType Event_Show =  17; //  Widget was shown on screen (QShowEvent).
const EventType Event_ShowToParent =  26; //  A child widget has been shown.
const EventType Event_SockAct = 50; //  Socket activated, used to implement QSocketNotifier.
const EventType Event_StateMachineSignal =  192;  //  A signal delivered to a state machine (QStateMachine::SignalEvent).
const EventType Event_StateMachineWrapped = 193;  //  The event is a wrapper for, i.e., contains, another event (QStateMachine::WrappedEvent).
const EventType Event_StatusTip = 112;  //  A status tip is requested (QStatusTipEvent).
const EventType Event_StyleChange = 100;  //  Widget's style has been changed.
const EventType Event_TabletMove =  87; //  Wacom tablet move (QTabletEvent).
const EventType Event_TabletPress = 92; //  Wacom tablet press (QTabletEvent).
const EventType Event_TabletRelease = 93; //  Wacom tablet release (QTabletEvent).
const EventType Event_OkRequest = 94; //  Ok button in decoration pressed. Supported only for Windows CE.
const EventType Event_TabletEnterProximity =  171;  //  Wacom tablet enter proximity event (QTabletEvent), sent to QApplication.
const EventType Event_TabletLeaveProximity =  172;  //  Wacom tablet leave proximity event (QTabletEvent), sent to QApplication.
const EventType Event_ThreadChange =  22; //  The object is moved to another thread. This is the last event sent to this object in the previous thread. See QObject::moveToThread().
const EventType Event_Timer = 1;  //  Regular timer events (QTimerEvent).
const EventType Event_ToolBarChange = 120;  //  The toolbar button is toggled on Mac OS X.
const EventType Event_ToolTip = 110;  //  A tooltip was requested (QHelpEvent).
const EventType Event_ToolTipChange = 184;  //  The widget's tooltip has changed.
const EventType Event_TouchBegin =  194;  //  Beginning of a sequence of touch-screen or track-pad events (QTouchEvent).
const EventType Event_TouchCancel = 209;  //  Cancellation of touch-event sequence (QTouchEvent).
const EventType Event_TouchEnd =  196;  //  End of touch-event sequence (QTouchEvent).
const EventType Event_TouchUpdate = 195;  //  Touch-screen event (QTouchEvent).
const EventType Event_UngrabKeyboard =  189;  //  Item loses keyboard grab (QGraphicsItem only).
const EventType Event_UngrabMouse = 187;  //  Item loses mouse grab (QGraphicsItem only).
const EventType Event_UpdateLater = 78; //  The widget should be queued to be repainted at a later time.
const EventType Event_UpdateRequest = 77; //  The widget should be repainted.
const EventType Event_WhatsThis = 111;  //  The widget should reveal "What's This?" help (QHelpEvent).
const EventType Event_WhatsThisClicked =  118;  //  A link in a widget's "What's This?" help was clicked.
const EventType Event_Wheel = 31; //  Mouse wheel rolled (QWheelEvent).
const EventType Event_WinEventAct = 132;  //  A Windows-specific activation event has occurred.
const EventType Event_WindowActivate =  24; //  Window was activated.
const EventType Event_WindowBlocked = 103;  //  The window is blocked by a modal dialog.
const EventType Event_WindowDeactivate =  25; //  Window was deactivated.
const EventType Event_WindowIconChange =  34; //  The window's icon has changed.
const EventType Event_WindowStateChange = 105;  //  The window's state (minimized, maximized or full-screen) has changed (QWindowStateChangeEvent).
const EventType Event_WindowTitleChange = 33; //  The window title has changed.
const EventType Event_WindowUnblocked = 104;  //  The window is unblocked after a modal dialog exited.
const EventType Event_WinIdChange = 203;  //  The window system identifer for this native widget has changed.
const EventType Event_ZOrderChange =  126;  //  The widget's z-order has changed. This event is never sent to top level windows.


typedef unsigned int MouseButton;
const MouseButton MouseButton_NoButton = 0;
const MouseButton MouseButton_LeftButton = 1;
const MouseButton MouseButton_RightButton = 2;
const MouseButton MouseButton_MiddleButton = 4;

typedef unsigned int ModiferKey;
const ModiferKey ModiferKey_NoModifier = 0x00000000; // No modifier key is pressed.
const ModiferKey ModiferKey_Shift = 0x02000000; // A Shift key on the keyboard is pressed.
const ModiferKey ModiferKey_Ctrl = 0x04000000; // A Ctrl key on the keyboard is pressed.
const ModiferKey ModiferKey_Alt = 0x08000000; // An Alt key on the keyboard is pressed.
const ModiferKey ModiferKey_Meta = 0x10000000; // A Meta key on the keyboard is pressed.
const ModiferKey ModiferKey_Keypad = 0x20000000; // A keypad button is pressed.
const ModiferKey ModiferKey_GroupSwitch = 0x40000000; // X11 only. A Mode_switch key on the keyboard is pressed.

SpliceMouseCallback::SpliceMouseCallback(void)
	: m_LMouseDown(false)
	, m_MMouseDown(false)
	, m_RMouseDown(false)
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


		FabricCore::RTVal cameraMat = inlineCamera.maybeGetMember("mat44");
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

			inlineCamera.setMember("mat44", cameraMat);
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

	int res = FALSE;
	
	// Hide the cursor (we assume here for the point of Siggraph demo's that we are in painting mode)
	SetCursor(NULL);

	FabricCore::RTVal klevent = FabricSplice::constructObjectRTVal("MouseEvent");

	int eventType = Event_MouseMove;

	// Generate mouse down/up events
	bool bCloseHold = false;
	if (!m_LMouseDown && flags&MOUSE_LBUTTON)
	{
		eventType = Event_MouseButtonPress;
		m_LMouseDown = true;
		theHold.Begin();
	}
	else if (m_LMouseDown && (flags&MOUSE_LBUTTON) == 0)
	{
		eventType = Event_MouseButtonRelease;
		m_LMouseDown = false;
		bCloseHold = true;
	}

	if (!m_RMouseDown && flags&MOUSE_RBUTTON)
	{
		eventType = Event_MouseButtonPress;
		theHold.Begin();
		m_RMouseDown = true;
	}
	else if (m_RMouseDown && (flags&MOUSE_RBUTTON) == 0)
	{
		eventType = Event_MouseButtonRelease;
		bCloseHold = true;
		m_RMouseDown = false;
	}

	if (!m_MMouseDown && flags&MOUSE_MBUTTON)
	{
		eventType = Event_MouseButtonPress;
		theHold.Begin();
		m_MMouseDown = true;
	}
	else if (m_MMouseDown && (flags&MOUSE_MBUTTON) == 0)
	{
		eventType = Event_MouseButtonRelease;
		bCloseHold = true;
		m_MMouseDown = false;
	}

	//Interface* pCore = GetCOREInterface();
	//ViewExp* pView = GetViewport(pCore, hwnd);
	//LONG cursorX, cursorY;
	//in_ctxt.GetMousePosition( cursorX, cursorY );

	//LONG width, height;
	//in_ctxt.GetViewSize(width, height);
	//ReleaseViewport(pCore, hwnd);

	FabricCore::RTVal klpos = FabricSplice::constructRTVal("Vec2");
	klpos.setMember("x", FabricSplice::constructFloat32RTVal((float)m.x));
	klpos.setMember("y", FabricSplice::constructFloat32RTVal((float)m.y));
	klevent.setMember("pos", klpos);

	long buttons = 0;
	if(flags&MOUSE_LBUTTON)
		buttons += MouseButton_LeftButton;
	if(flags&MOUSE_RBUTTON)
		buttons += MouseButton_RightButton;
	if(flags&MOUSE_MBUTTON)
		buttons += MouseButton_MiddleButton;

	// Note: In Softimage, we don't get independent events from Mousebutton down.
	// TODO: In Max, we may have different behaviour, so figure out what we can do here
	klevent.setMember("button", FabricSplice::constructUInt32RTVal(buttons));
	klevent.setMember("buttons", FabricSplice::constructUInt32RTVal(buttons));

	long modifiers = 0;
	//if(msg&MOUSE_SHIFT)
	//	modifiers += ModiferKey_Shift;
	//if(msg&MOUSE_CTRL)
	//	modifiers += ModiferKey_Ctrl;
	//if(msg&MOUSE_ALT)
	//	modifiers += ModiferKey_Alt;

	// Fetch tab key state.
	SHORT keyState = GetAsyncKeyState( VK_CONTROL );
	if( ( 1 << 16 ) & keyState )
		modifiers += ModiferKey_Ctrl;

	keyState  = GetAsyncKeyState( VK_MENU );
	if( ( 1 << 16 ) & keyState )
		modifiers += ModiferKey_Alt;

	keyState = GetAsyncKeyState( VK_SHIFT );
	if( ( 1 << 16 ) & keyState )
		modifiers += ModiferKey_Shift;

	klevent.setMember("modifiers", FabricSplice::constructUInt32RTVal(modifiers));

	// Trigger the event on Splice

	ViewExp& pView = GetCOREInterface()->GetViewExp(hwnd);
	FabricCore::RTVal inlineViewport = SetupViewport(&pView);

	//////////////////////////
	// Setup the Host
	// We cannot set an interface value via RTVals.
	FabricCore::RTVal host = FabricSplice::constructObjectRTVal("Host");
	host.setMember("hostName", FabricSplice::constructStringRTVal("3dsMax"));

	//////////////////////////
	// Configure the event...
	std::vector<FabricCore::RTVal> args(4);
	args[0] = host;
	args[1] = inlineViewport;
	args[2] = FabricSplice::constructUInt32RTVal(eventType);
	args[3] = FabricSplice::constructUInt32RTVal(modifiers);
	klevent.callMethod("", "init", 4, &args[0]);

	try{
		mEventDispatcher.callMethod("Boolean", "onEvent", 1, &klevent);
	}
	catch(FabricCore::Exception e)    {
		logMessage(e.getDesc_cstr());
		res = 0;
	}
	catch(FabricSplice::Exception e){
		logMessage(e.what());
		res = 0;
	}

	bool result = klevent.callMethod("Boolean", "isAccepted", 0, 0).getBoolean();
	
    // The manipulation system has requested that a custom command be invoked. 
    // Invoke the custom command passing the speficied args. 
	std::string customCommand(host.maybeGetMember("customCommand").getStringCString());
	if(customCommand != std::string("")){
		BOOL result;
		BOOL quietErrors = FALSE;
		FabricCore::RTVal customCommandArgs = host.maybeGetMember("customCommandArgs");
		bool found = customCommand.find(std::string("=")) !=std::string::npos;
		if (found){
			// Setting a value in Max.
			// e.g. 
			// $Sphere01.radius = 3.0
			std::string args;
			for(int i=0; i<customCommandArgs.getArraySize(); i++){
				args += std::string(customCommandArgs.getArrayElement(i).getStringCString());
			}
			result = ExecuteMAXScriptScript(TSTR::FromCStr((customCommand + args).data()), quietErrors);
		}
		else{
			// Calling a functions in MaxScript.
			if(customCommandArgs.getArraySize() == 0){
				// No args
				// e.g. 
				// theHold.Begin()
				bool found = customCommand.find(std::string("(")) !=std::wstring::npos;
				if(found)
					result = ExecuteMAXScriptScript(TSTR::FromCStr(customCommand.data()), quietErrors);
				else
					result = ExecuteMAXScriptScript(TSTR::FromCStr((customCommand + std::string("()")).data()), quietErrors);
			}
			else{
				// With args
				// e.g. 
				// theHold.Accept "MyChanges"
				std::string args;
				for(int i=0; i<customCommandArgs.getArraySize(); i++){
					args += std::string(" ");
					args += std::string(customCommandArgs.getArrayElement(i).getStringCString());
				}
				result = ExecuteMAXScriptScript(TSTR::FromCStr((customCommand + args).data()), quietErrors);
			}
		}
	}

	if(host.maybeGetMember("redrawRequested").getBoolean())	{
		Interface7* pCore = GetCOREInterface7();
		ViewExp& vp = pCore->GetViewExp(hwnd);
		ViewExp10* vp10 = NULL;
		if (vp.IsAlive()){
			vp10 = reinterpret_cast<ViewExp10*>(vp.Execute(ViewExp::kEXECUTE_GET_VIEWEXP_10));
			vp10->Invalidate(true);
			//pCore->RedrawViews(pCore->GetTime());
			//GetCOREInterface7()->RedrawViews(GetCOREInterface()->GetTime());
			pCore->RedrawViewportsLater(pCore->GetTime());
			//pCore->ForceCompleteRedraw();
		}
		//SpliceEvents::GetInstance()->TriggerRedraw();
	}

	if(host.callMethod("Boolean", "undoRedoCommandsAdded", 0, 0).getBoolean()){
		if (theHold.Holding()){
			FabricCore::RTVal fabricUndoVal = host.callMethod("UndoRedoCommand[]", "getUndoRedoCommands", 0, 0);
			CustomKLUndoRedoCommandObject* pNewUndo = new CustomKLUndoRedoCommandObject(fabricUndoVal);
			theHold.Put(pNewUndo);
		}
	}

	// if we are in mouse-up, then close our hold
	if (bCloseHold)
		theHold.Accept(_T("Splice Actions"));

	klevent.invalidate();
	return res;
}

void SpliceMouseCallback::EnterMode()
{
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
		//in_ctxt.Redraw(true);
	}
}

void SpliceMouseCallback::ExitMode()
{
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
}
