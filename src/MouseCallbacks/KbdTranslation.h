#pragma once

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

const int Key_Escape=0x01000000;
const int Key_Tab=0x01000001;
const int Key_Backtab=0x01000002;
const int Key_Backspace=0x01000003;
const int Key_Return=0x01000004;
const int Key_Enter=0x01000005;
const int Key_Insert=0x01000006;
const int Key_Delete=0x01000007;
const int Key_Pause=0x01000008;
const int Key_Print=0x01000009;
const int Key_SysReq=0x0100000a;
const int Key_Clear=0x0100000b;
const int Key_Home=0x01000010;   
const int Key_End=0x01000011;   
const int Key_Left=0x01000012;   
const int Key_Up=0x01000013;   
const int Key_Right=0x01000014;   
const int Key_Down=0x01000015;   
const int Key_PageUp=0x01000016;   
const int Key_PageDown=0x01000017;   
const int Key_Shift=0x01000020;   
const int Key_Control=0x01000021; // Corresponds to the Command key on Mac OS X.
const int Key_Meta=0x01000022; // Corresponds to the Control keys on on Mac OS X. Maps to the Windows key on Windows keyboards.
const int Key_Alt=0x01000023;   
const int Key_AltGr=0x01001103; // When the KeyDown event for this key is sent on Windows, the Ctrl+Alt modifiers are also set.
const int Key_CapsLock=0x01000024;   
const int Key_NumLock=0x01000025;   
const int Key_ScrollLock=0x01000026;   
const int Key_F1=0x01000030;   
const int Key_F2=0x01000031;   
const int Key_F3=0x01000032;   
const int Key_F4=0x01000033;   
const int Key_F5=0x01000034;   
const int Key_F6=0x01000035;   
const int Key_F7=0x01000036;   
const int Key_F8=0x01000037;   
const int Key_F9=0x01000038;   
const int Key_F10=0x01000039;   
const int Key_F11=0x0100003a;   
const int Key_F12=0x0100003b;   
const int Key_F13=0x0100003c;   
const int Key_F14=0x0100003d;   
const int Key_F15=0x0100003e;   
const int Key_F16=0x0100003f;   
const int Key_F17=0x01000040;   
const int Key_F18=0x01000041;   
const int Key_F19=0x01000042;   
const int Key_F20=0x01000043;   
const int Key_F21=0x01000044;   
const int Key_F22=0x01000045;   
const int Key_F23=0x01000046;   
const int Key_F24=0x01000047;   
const int Key_F25=0x01000048;   
const int Key_F26=0x01000049;   
const int Key_F27=0x0100004a;   
const int Key_F28=0x0100004b;   
const int Key_F29=0x0100004c;   
const int Key_F30=0x0100004d;   
const int Key_F31=0x0100004e;   
const int Key_F32=0x0100004f;   
const int Key_F33=0x01000050;   
const int Key_F34=0x01000051;   
const int Key_F35=0x01000052;   
const int Key_Super_L=0x01000053;   
const int Key_Super_R=0x01000054;   
const int Key_Menu=0x01000055;   
const int Key_Hyper_L=0x01000056;   
const int Key_Hyper_R=0x01000057;   
const int Key_Help=0x01000058;   
const int Key_Direction_L=0x01000059;   
const int Key_Direction_R=0x01000060;   
const int Key_Space=0x20;
const int Key_Exclam=0x21;
const int Key_QuoteDbl=0x22;
const int Key_NumberSign=0x23;
const int Key_Dollar=0x24;
const int Key_Percent=0x25;
const int Key_Ampersand=0x26;
const int Key_Apostrophe=0x27;
const int Key_ParenLeft=0x28;
const int Key_ParenRight=0x29;
const int Key_Asterisk=0x2a;
const int Key_Plus=0x2b;
const int Key_Comma=0x2c;
const int Key_Minus=0x2d;
const int Key_Period=0x2e;
const int Key_Slash=0x2f;
const int Key_Key_0=0x30;
const int Key_Key_1=0x31;
const int Key_Key_2=0x32;
const int Key_Key_3=0x33;
const int Key_Key_4=0x34;
const int Key_Key_5=0x35;
const int Key_Key_6=0x36;
const int Key_Key_7=0x37;
const int Key_Key_8=0x38;
const int Key_Key_9=0x39;
const int Key_Colon=0x3a;
const int Key_Semicolon=0x3b;
const int Key_Less=0x3c;
const int Key_Equal=0x3d;
const int Key_Greater=0x3e;
const int Key_Question=0x3f;
const int Key_At=0x40;
const int Key_A=0x41;
const int Key_B=0x42;
const int Key_C=0x43;
const int Key_D=0x44;
const int Key_E=0x45;
const int Key_F=0x46;
const int Key_G=0x47;
const int Key_H=0x48;
const int Key_I=0x49;
const int Key_J=0x4a;
const int Key_K=0x4b;
const int Key_L=0x4c;
const int Key_M=0x4d;
const int Key_N=0x4e;
const int Key_O=0x4f;
const int Key_P=0x50;
const int Key_Q=0x51;
const int Key_R=0x52;
const int Key_S=0x53;
const int Key_T=0x54;
const int Key_U=0x55;
const int Key_V=0x56;
const int Key_W=0x57;
const int Key_X=0x58;
const int Key_Y=0x59;
const int Key_Z=0x5a;
const int Key_BracketLeft=0x5b;
const int Key_Backslash=0x5c;
const int Key_BracketRight=0x5d;
const int Key_AsciiCircum=0x5e;
const int Key_Underscore=0x5f;
const int Key_QuoteLeft=0x60;
const int Key_BraceLeft=0x7b;
const int Key_Bar=0x7c;
const int Key_BraceRight=0x7d;
const int Key_AsciiTilde=0x7e;
const int Key_nobreakspace=0x0a0;  
const int Key_exclamdown=0x0a1;  
const int Key_cent=0x0a2;  
const int Key_sterling=0x0a3;  
const int Key_currency=0x0a4;  
const int Key_yen=0x0a5;  
const int Key_brokenbar=0x0a6;  
const int Key_section=0x0a7;  
const int Key_diaeresis=0x0a8;  
const int Key_copyright=0x0a9;  
const int Key_ordfeminine=0x0aa;  
const int Key_guillemotleft=0x0ab;  
const int Key_notsign=0x0ac;  
const int Key_hyphen=0x0ad;  
const int Key_registered=0x0ae;  
const int Key_macron=0x0af;  
const int Key_degree=0x0b0;  
const int Key_plusminus=0x0b1;  
const int Key_twosuperior=0x0b2;  
const int Key_threesuperior=0x0b3;  
const int Key_acute=0x0b4;  
const int Key_mu=0x0b5;  
const int Key_paragraph=0x0b6;  
const int Key_periodcentered=0x0b7;  
const int Key_cedilla=0x0b8;  
const int Key_onesuperior=0x0b9;  
const int Key_masculine=0x0ba;  
const int Key_guillemotright=0x0bb;  
const int Key_onequarter=0x0bc;  
const int Key_onehalf=0x0bd;  
const int Key_threequarters=0x0be;  
const int Key_questiondown=0x0bf;  
const int Key_Agrave=0x0c0;  
const int Key_Aacute=0x0c1;  
const int Key_Acircumflex=0x0c2;  
const int Key_Atilde=0x0c3;  
const int Key_Adiaeresis=0x0c4;  
const int Key_Aring=0x0c5;  
const int Key_AE=0x0c6;  
const int Key_Ccedilla=0x0c7;  
const int Key_Egrave=0x0c8;  
const int Key_Eacute=0x0c9;  
const int Key_Ecircumflex=0x0ca;  
const int Key_Ediaeresis=0x0cb;  
const int Key_Igrave=0x0cc;  
const int Key_Iacute=0x0cd;  
const int Key_Icircumflex=0x0ce;  
const int Key_Idiaeresis=0x0cf;  
const int Key_ETH=0x0d0;  
const int Key_Ntilde=0x0d1;  
const int Key_Ograve=0x0d2;  
const int Key_Oacute=0x0d3;  
const int Key_Ocircumflex=0x0d4;  
const int Key_Otilde=0x0d5;  
const int Key_Odiaeresis=0x0d6;  
const int Key_multiply=0x0d7;  
const int Key_Ooblique=0x0d8;  
const int Key_Ugrave=0x0d9;  
const int Key_Uacute=0x0da;  
const int Key_Ucircumflex=0x0db;  
const int Key_Udiaeresis=0x0dc;  
const int Key_Yacute=0x0dd;  
const int Key_THORN=0x0de;  
const int Key_ssharp=0x0df;  
const int Key_division=0x0f7;  
const int Key_ydiaeresis=0x0ff;  
const int Key_Multi_key=0x01001120;
const int Key_Codeinput=0x01001137;
const int Key_SingleCandidate=0x0100113c;
const int Key_MultipleCandidate=0x0100113d;
const int Key_PreviousCandidate=0x0100113e;
const int Key_Mode_switch=0x0100117e;
const int Key_Kanji=0x01001121;
const int Key_Muhenkan=0x01001122;
const int Key_Henkan=0x01001123;
const int Key_Romaji=0x01001124;
const int Key_Hiragana=0x01001125;
const int Key_Katakana=0x01001126;
const int Key_Hiragana_Katakana=0x01001127;
const int Key_Zenkaku=0x01001128;
const int Key_Hankaku=0x01001129;
const int Key_Zenkaku_Hankaku=0x0100112a;
const int Key_Touroku=0x0100112b;
const int Key_Massyo=0x0100112c;
const int Key_Kana_Lock=0x0100112d;
const int Key_Kana_Shift=0x0100112e;
const int Key_Eisu_Shift=0x0100112f;
const int Key_Eisu_toggle=0x01001130;
const int Key_Hangul=0x01001131;
const int Key_Hangul_Start=0x01001132;
const int Key_Hangul_End=0x01001133;
const int Key_Hangul_Hanja=0x01001134;
const int Key_Hangul_Jamo=0x01001135;
const int Key_Hangul_Romaja=0x01001136;
const int Key_Hangul_Jeonja=0x01001138;
const int Key_Hangul_Banja=0x01001139;
const int Key_Hangul_PreHanja=0x0100113a;
const int Key_Hangul_PostHanja=0x0100113b;
const int Key_Hangul_Special=0x0100113f;
const int Key_Dead_Grave=0x01001250;
const int Key_Dead_Acute=0x01001251;
const int Key_Dead_Circumflex=0x01001252;
const int Key_Dead_Tilde=0x01001253;
const int Key_Dead_Macron=0x01001254;
const int Key_Dead_Breve=0x01001255;
const int Key_Dead_Abovedot=0x01001256;
const int Key_Dead_Diaeresis=0x01001257;
const int Key_Dead_Abovering=0x01001258;
const int Key_Dead_Doubleacute=0x01001259;
const int Key_Dead_Caron=0x0100125a;
const int Key_Dead_Cedilla=0x0100125b;
const int Key_Dead_Ogonek=0x0100125c;
const int Key_Dead_Iota=0x0100125d;
const int Key_Dead_Voiced_Sound=0x0100125e;
const int Key_Dead_Semivoiced_Sound=0x0100125f;
const int Key_Dead_Belowdot=0x01001260;
const int Key_Dead_Hook=0x01001261;
const int Key_Dead_Horn=0x01001262;
const int Key_Back=0x01000061;
const int Key_Forward=0x01000062;
const int Key_Stop=0x01000063;
const int Key_Refresh=0x01000064;
const int Key_VolumeDown=0x01000070;
const int Key_VolumeMute=0x01000071;
const int Key_VolumeUp=0x01000072;
const int Key_BassBoost=0x01000073;
const int Key_BassUp=0x01000074;
const int Key_BassDown=0x01000075;
const int Key_TrebleUp=0x01000076;
const int Key_TrebleDown=0x01000077;
const int Key_MediaPlay=0x01000080;
const int Key_MediaStop=0x01000081;
const int Key_MediaPrevious=0x01000082;
const int Key_MediaNext=0x01000083;
const int Key_MediaRecord=0x01000084;
const int Key_MediaPause=0x1000085;
const int Key_MediaTogglePlayPause=0x1000086;
const int Key_HomePage=0x01000090;
const int Key_Favorites=0x01000091;
const int Key_Search=0x01000092;
const int Key_Standby=0x01000093;
const int Key_OpenUrl=0x01000094;
const int Key_LaunchMail=0x010000a0;
const int Key_LaunchMedia=0x010000a1;
const int Key_Launch0=0x010000a2; // On X11 this key is mapped to "My Computer" (XF86XK_MyComputer) key for legacy reasons.
const int Key_Launch1=0x010000a3; // On X11 this key is mapped to "Calculator" (XF86XK_Calculator) key for legacy reasons.
const int Key_Launch2=0x010000a4; // On X11 this key is mapped to XF86XK_Launch0 key for legacy reasons.
const int Key_Launch3=0x010000a5; // On X11 this key is mapped to XF86XK_Launch1 key for legacy reasons.
const int Key_Launch4=0x010000a6; // On X11 this key is mapped to XF86XK_Launch2 key for legacy reasons.
const int Key_Launch5=0x010000a7; // On X11 this key is mapped to XF86XK_Launch3 key for legacy reasons.
const int Key_Launch6=0x010000a8; // On X11 this key is mapped to XF86XK_Launch4 key for legacy reasons.
const int Key_Launch7=0x010000a9; // On X11 this key is mapped to XF86XK_Launch5 key for legacy reasons.
const int Key_Launch8=0x010000aa; // On X11 this key is mapped to XF86XK_Launch6 key for legacy reasons.
const int Key_Launch9=0x010000ab; // On X11 this key is mapped to XF86XK_Launch7 key for legacy reasons.
const int Key_LaunchA=0x010000ac; // On X11 this key is mapped to XF86XK_Launch8 key for legacy reasons.
const int Key_LaunchB=0x010000ad; // On X11 this key is mapped to XF86XK_Launch9 key for legacy reasons.
const int Key_LaunchC=0x010000ae; // On X11 this key is mapped to XF86XK_LaunchA key for legacy reasons.
const int Key_LaunchD=0x010000af; // On X11 this key is mapped to XF86XK_LaunchB key for legacy reasons.
const int Key_LaunchE=0x010000b0; // On X11 this key is mapped to XF86XK_LaunchC key for legacy reasons.
const int Key_LaunchF=0x010000b1; // On X11 this key is mapped to XF86XK_LaunchD key for legacy reasons.
const int Key_LaunchG=0x0100010e; // On X11 this key is mapped to XF86XK_LaunchE key for legacy reasons.
const int Key_LaunchH=0x0100010f; // On X11 this key is mapped to XF86XK_LaunchF key for legacy reasons.
const int Key_MonBrightnessUp=0x010000b2;
const int Key_MonBrightnessDown=0x010000b3;
const int Key_KeyboardLightOnOff=0x010000b4;
const int Key_KeyboardBrightnessUp=0x010000b5;
const int Key_KeyboardBrightnessDown=0x010000b6;
const int Key_PowerOff=0x010000b7;
const int Key_WakeUp=0x010000b8;
const int Key_Eject=0x010000b9;
const int Key_ScreenSaver=0x010000ba;
const int Key_WWW=0x010000bb;
const int Key_Memo=0x010000bc;
const int Key_LightBulb=0x010000bd;
const int Key_Shop=0x010000be;
const int Key_History=0x010000bf;
const int Key_AddFavorite=0x010000c0;
const int Key_HotLinks=0x010000c1;
const int Key_BrightnessAdjust=0x010000c2;
const int Key_Finance=0x010000c3;
const int Key_Community=0x010000c4;
const int Key_AudioRewind=0x010000c5;
const int Key_BackForward=0x010000c6;
const int Key_ApplicationLeft=0x010000c7;
const int Key_ApplicationRight=0x010000c8;
const int Key_Book=0x010000c9;
const int Key_CD=0x010000ca;
const int Key_ToDoList=0x010000cc;   
const int Key_ClearGrab=0x010000cd;   
const int Key_Close=0x010000ce;   
const int Key_Copy=0x010000cf;   
const int Key_Cut=0x010000d0;   
const int Key_Display=0x010000d1;   
const int Key_DOS=0x010000d2;   
const int Key_Documents=0x010000d3;   
const int Key_Excel=0x010000d4;   
const int Key_Explorer=0x010000d5;   
const int Key_Game=0x010000d6;   
const int Key_Go=0x010000d7;   
const int Key_iTouch=0x010000d8;   
const int Key_LogOff=0x010000d9;   
const int Key_Market=0x010000da;   
const int Key_Meeting=0x010000db;   
const int Key_MenuKB=0x010000dc;   
const int Key_MenuPB=0x010000dd;   
const int Key_MySites=0x010000de;   
const int Key_News=0x010000df;   
const int Key_OfficeHome=0x010000e0;   
const int Key_Option=0x010000e1;   
const int Key_Paste=0x010000e2;   
const int Key_Phone=0x010000e3;   
const int Key_Calendar=0x010000e4;   
const int Key_Reply=0x010000e5;   
const int Key_Reload=0x010000e6;   
const int Key_RotateWindows=0x010000e7;   
const int Key_RotationPB=0x010000e8;   
const int Key_RotationKB=0x010000e9;   
const int Key_Save=0x010000ea;   
const int Key_Send=0x010000eb;   
const int Key_Spell=0x010000ec;   
const int Key_SplitScreen=0x010000ed;   
const int Key_Support=0x010000ee;   
const int Key_TaskPane=0x010000ef;   
const int Key_Terminal=0x010000f0;   
const int Key_Tools=0x010000f1;   
const int Key_Travel=0x010000f2;   
const int Key_Video=0x010000f3;   
const int Key_Word=0x010000f4;   
const int Key_Xfer=0x010000f5;   
const int Key_ZoomIn=0x010000f6;   
const int Key_ZoomOut=0x010000f7;   
const int Key_Away=0x010000f8;   
const int Key_Messenger=0x010000f9;   
const int Key_WebCam=0x010000fa;   
const int Key_MailForward=0x010000fb;   
const int Key_Pictures=0x010000fc;   
const int Key_Music=0x010000fd;   
const int Key_Battery=0x010000fe;   
const int Key_Bluetooth=0x010000ff;   
const int Key_WLAN=0x01000100;   
const int Key_UWB=0x01000101;   
const int Key_AudioForward=0x01000102;   
const int Key_AudioRepeat=0x01000103;   
const int Key_AudioRandomPlay=0x01000104;   
const int Key_Subtitle=0x01000105;   
const int Key_AudioCycleTrack=0x01000106;   
const int Key_Time=0x01000107;   
const int Key_Hibernate=0x01000108;   
const int Key_View=0x01000109;   
const int Key_TopMenu=0x0100010a;   
const int Key_PowerDown=0x0100010b;   
const int Key_Suspend=0x0100010c;   
const int Key_ContrastAdjust=0x0100010d;   
const int Key_MediaLast=0x0100ffff;   
const int Key_unknown=0x01ffffff;   
const int Key_Camera=0x01100020; // A key to activate the camera shutter
const int Key_CameraFocus=0x01100021; // A key to focus the camera
const int Key_Context1=0x01100000;   
const int Key_Context2=0x01100001;   
const int Key_Context3=0x01100002;   
const int Key_Context4=0x01100003;   
const int Key_Flip=0x01100006;   
const int Key_No=0x01010002;   
const int Key_Select=0x01010000;   
const int Key_Yes=0x01010001;   
const int Key_Execute=0x01020003;   
const int Key_Printer=0x01020002;   
const int Key_Play=0x01020005;   
const int Key_Sleep=0x01020004;   
const int Key_Zoom=0x01020006;   
const int Key_Cancel=0x01020001;   

// Key translation ---------------------------------------------------------------------[ start ] --
// Meaning of values:
//             0 = Character output key, needs keyboard driver mapping
//   Key_unknown = Unknown Virtual Key, no translation possible, ignore
static const unsigned int KeyTbl[] = { // Keyboard mapping table
	// Dec |  Hex | Windows Virtual key
	Key_unknown,    //   0   0x00
	Key_unknown,    //   1   0x01   VK_LBUTTON          | Left mouse button
	Key_unknown,    //   2   0x02   VK_RBUTTON          | Right mouse button
	Key_Cancel,     //   3   0x03   VK_CANCEL           | Control-Break processing
	Key_unknown,    //   4   0x04   VK_MBUTTON          | Middle mouse button
	Key_unknown,    //   5   0x05   VK_XBUTTON1         | X1 mouse button
	Key_unknown,    //   6   0x06   VK_XBUTTON2         | X2 mouse button
	Key_unknown,    //   7   0x07   -- unassigned --
	Key_Backspace,  //   8   0x08   VK_BACK             | BackSpace key
	Key_Tab,        //   9   0x09   VK_TAB              | Tab key
	Key_unknown,    //  10   0x0A   -- reserved --
	Key_unknown,    //  11   0x0B   -- reserved --
	Key_Clear,      //  12   0x0C   VK_CLEAR            | Clear key
	Key_Return,     //  13   0x0D   VK_RETURN           | Enter key
	Key_unknown,    //  14   0x0E   -- unassigned --
	Key_unknown,    //  15   0x0F   -- unassigned --
	Key_Shift,      //  16   0x10   VK_SHIFT            | Shift key
	Key_Control,    //  17   0x11   VK_CONTROL          | Ctrl key
	Key_Alt,        //  18   0x12   VK_MENU             | Alt key
	Key_Pause,      //  19   0x13   VK_PAUSE            | Pause key
	Key_CapsLock,   //  20   0x14   VK_CAPITAL          | Caps-Lock
	Key_unknown,    //  21   0x15   VK_KANA / VK_HANGUL | IME Kana or Hangul mode
	Key_unknown,    //  22   0x16   -- unassigned --
	Key_unknown,    //  23   0x17   VK_JUNJA            | IME Junja mode
	Key_unknown,    //  24   0x18   VK_FINAL            | IME final mode
	Key_unknown,    //  25   0x19   VK_HANJA / VK_KANJI | IME Hanja or Kanji mode
	Key_unknown,    //  26   0x1A   -- unassigned --
	Key_Escape,     //  27   0x1B   VK_ESCAPE           | Esc key
	Key_unknown,    //  28   0x1C   VK_CONVERT          | IME convert
	Key_unknown,    //  29   0x1D   VK_NONCONVERT       | IME non-convert
	Key_unknown,    //  30   0x1E   VK_ACCEPT           | IME accept
	Key_Mode_switch,//  31   0x1F   VK_MODECHANGE       | IME mode change request
	Key_Space,      //  32   0x20   VK_SPACE            | Spacebar
	Key_PageUp,     //  33   0x21   VK_PRIOR            | Page Up key
	Key_PageDown,   //  34   0x22   VK_NEXT             | Page Down key
	Key_End,        //  35   0x23   VK_END              | End key
	Key_Home,       //  36   0x24   VK_HOME             | Home key
	Key_Left,       //  37   0x25   VK_LEFT             | Left arrow key
	Key_Up,         //  38   0x26   VK_UP               | Up arrow key
	Key_Right,      //  39   0x27   VK_RIGHT            | Right arrow key
	Key_Down,       //  40   0x28   VK_DOWN             | Down arrow key
	Key_Select,     //  41   0x29   VK_SELECT           | Select key
	Key_Printer,    //  42   0x2A   VK_PRINT            | Print key
	Key_Execute,    //  43   0x2B   VK_EXECUTE          | Execute key
	Key_Print,      //  44   0x2C   VK_SNAPSHOT         | Print Screen key
	Key_Insert,     //  45   0x2D   VK_INSERT           | Ins key
	Key_Delete,     //  46   0x2E   VK_DELETE           | Del key
	Key_Help,       //  47   0x2F   VK_HELP             | Help key
	Key_Key_0,                  //  48   0x30   (VK_0)              | 0 key
	Key_Key_1,                  //  49   0x31   (VK_1)              | 1 key
	Key_Key_2,                  //  50   0x32   (VK_2)              | 2 key
	Key_Key_3,                  //  51   0x33   (VK_3)              | 3 key
	Key_Key_4,                  //  52   0x34   (VK_4)              | 4 key
	Key_Key_5,                  //  53   0x35   (VK_5)              | 5 key
	Key_Key_6,                  //  54   0x36   (VK_6)              | 6 key
	Key_Key_7,                  //  55   0x37   (VK_7)              | 7 key
	Key_Key_8,                  //  56   0x38   (VK_8)              | 8 key
	Key_Key_9,                  //  57   0x39   (VK_9)              | 9 key
	Key_unknown,    //  58   0x3A   -- unassigned --
	Key_unknown,    //  59   0x3B   -- unassigned --
	Key_unknown,    //  60   0x3C   -- unassigned --
	Key_unknown,    //  61   0x3D   -- unassigned --
	Key_unknown,    //  62   0x3E   -- unassigned --
	Key_unknown,    //  63   0x3F   -- unassigned --
	Key_unknown,    //  64   0x40   -- unassigned --
	Key_A,                  //  65   0x41   (VK_A)              | A key
	Key_B,                  //  66   0x42   (VK_B)              | B key
	Key_C,                  //  67   0x43   (VK_C)              | C key
	Key_D,                  //  68   0x44   (VK_D)              | D key
	Key_E,                  //  69   0x45   (VK_E)              | E key
	Key_F,                  //  70   0x46   (VK_F)              | F key
	Key_G,                  //  71   0x47   (VK_G)              | G key
	Key_H,                  //  72   0x48   (VK_H)              | H key
	Key_I,                  //  73   0x49   (VK_I)              | I key
	Key_J,                  //  74   0x4A   (VK_J)              | J key
	Key_K,                  //  75   0x4B   (VK_K)              | K key
	Key_L,                  //  76   0x4C   (VK_L)              | L key
	Key_M,                  //  77   0x4D   (VK_M)              | M key
	Key_N,                  //  78   0x4E   (VK_N)              | N key
	Key_O,                  //  79   0x4F   (VK_O)              | O key
	Key_P,                  //  80   0x50   (VK_P)              | P key
	Key_Q,                  //  81   0x51   (VK_Q)              | Q key
	Key_R,                  //  82   0x52   (VK_R)              | R key
	Key_S,                  //  83   0x53   (VK_S)              | S key
	Key_T,                  //  84   0x54   (VK_T)              | T key
	Key_U,                  //  85   0x55   (VK_U)              | U key
	Key_V,                  //  86   0x56   (VK_V)              | V key
	Key_W,                  //  87   0x57   (VK_W)              | W key
	Key_X,                  //  88   0x58   (VK_X)              | X key
	Key_Y,                  //  89   0x59   (VK_Y)              | Y key
	Key_Z,                  //  90   0x5A   (VK_Z)              | Z key
	Key_Meta,       //  91   0x5B   VK_LWIN             | Left Windows  - MS Natural kbd
	Key_Meta,       //  92   0x5C   VK_RWIN             | Right Windows - MS Natural kbd
	Key_Menu,       //  93   0x5D   VK_APPS             | Application key-MS Natural kbd
	Key_unknown,    //  94   0x5E   -- reserved --
	Key_Sleep,      //  95   0x5F   VK_SLEEP
	Key_Key_0,          //  96   0x60   VK_NUMPAD0          | Numeric keypad 0 key
	Key_Key_1,          //  97   0x61   VK_NUMPAD1          | Numeric keypad 1 key
	Key_Key_2,          //  98   0x62   VK_NUMPAD2          | Numeric keypad 2 key
	Key_Key_3,          //  99   0x63   VK_NUMPAD3          | Numeric keypad 3 key
	Key_Key_4,          // 100   0x64   VK_NUMPAD4          | Numeric keypad 4 key
	Key_Key_5,          // 101   0x65   VK_NUMPAD5          | Numeric keypad 5 key
	Key_Key_6,          // 102   0x66   VK_NUMPAD6          | Numeric keypad 6 key
	Key_Key_7,          // 103   0x67   VK_NUMPAD7          | Numeric keypad 7 key
	Key_Key_8,          // 104   0x68   VK_NUMPAD8          | Numeric keypad 8 key
	Key_Key_9,          // 105   0x69   VK_NUMPAD9          | Numeric keypad 9 key
	Key_Asterisk,   // 106   0x6A   VK_MULTIPLY         | Multiply key
	Key_Plus,       // 107   0x6B   VK_ADD              | Add key
	Key_Comma,      // 108   0x6C   VK_SEPARATOR        | Separator key
	Key_Minus,      // 109   0x6D   VK_SUBTRACT         | Subtract key
	Key_Period,     // 110   0x6E   VK_DECIMAL          | Decimal key
	Key_Slash,      // 111   0x6F   VK_DIVIDE           | Divide key
	Key_F1,         // 112   0x70   VK_F1               | F1 key
	Key_F2,         // 113   0x71   VK_F2               | F2 key
	Key_F3,         // 114   0x72   VK_F3               | F3 key
	Key_F4,         // 115   0x73   VK_F4               | F4 key
	Key_F5,         // 116   0x74   VK_F5               | F5 key
	Key_F6,         // 117   0x75   VK_F6               | F6 key
	Key_F7,         // 118   0x76   VK_F7               | F7 key
	Key_F8,         // 119   0x77   VK_F8               | F8 key
	Key_F9,         // 120   0x78   VK_F9               | F9 key
	Key_F10,        // 121   0x79   VK_F10              | F10 key
	Key_F11,        // 122   0x7A   VK_F11              | F11 key
	Key_F12,        // 123   0x7B   VK_F12              | F12 key
	Key_F13,        // 124   0x7C   VK_F13              | F13 key
	Key_F14,        // 125   0x7D   VK_F14              | F14 key
	Key_F15,        // 126   0x7E   VK_F15              | F15 key
	Key_F16,        // 127   0x7F   VK_F16              | F16 key
	Key_F17,        // 128   0x80   VK_F17              | F17 key
	Key_F18,        // 129   0x81   VK_F18              | F18 key
	Key_F19,        // 130   0x82   VK_F19              | F19 key
	Key_F20,        // 131   0x83   VK_F20              | F20 key
	Key_F21,        // 132   0x84   VK_F21              | F21 key
	Key_F22,        // 133   0x85   VK_F22              | F22 key
	Key_F23,        // 134   0x86   VK_F23              | F23 key
	Key_F24,        // 135   0x87   VK_F24              | F24 key
	Key_unknown,    // 136   0x88   -- unassigned --
	Key_unknown,    // 137   0x89   -- unassigned --
	Key_unknown,    // 138   0x8A   -- unassigned --
	Key_unknown,    // 139   0x8B   -- unassigned --
	Key_unknown,    // 140   0x8C   -- unassigned --
	Key_unknown,    // 141   0x8D   -- unassigned --
	Key_unknown,    // 142   0x8E   -- unassigned --
	Key_unknown,    // 143   0x8F   -- unassigned --
	Key_NumLock,    // 144   0x90   VK_NUMLOCK          | Num Lock key
	Key_ScrollLock, // 145   0x91   VK_SCROLL           | Scroll Lock key
	// Fujitsu/OASYS kbd --------------------
	0, //Qt::Key_Jisho, // 146   0x92   VK_OEM_FJ_JISHO     | 'Dictionary' key /
	//              VK_OEM_NEC_EQUAL  = key on numpad on NEC PC-9800 kbd
	Key_Massyo,     // 147   0x93   VK_OEM_FJ_MASSHOU   | 'Unregister word' key
	Key_Touroku,    // 148   0x94   VK_OEM_FJ_TOUROKU   | 'Register word' key
	0, //Qt::Key_Oyayubi_Left,//149   0x95  VK_OEM_FJ_LOYA  | 'Left OYAYUBI' key
	0, //Qt::Key_Oyayubi_Right,//150  0x96  VK_OEM_FJ_ROYA  | 'Right OYAYUBI' key
	Key_unknown,    // 151   0x97   -- unassigned --
	Key_unknown,    // 152   0x98   -- unassigned --
	Key_unknown,    // 153   0x99   -- unassigned --
	Key_unknown,    // 154   0x9A   -- unassigned --
	Key_unknown,    // 155   0x9B   -- unassigned --
	Key_unknown,    // 156   0x9C   -- unassigned --
	Key_unknown,    // 157   0x9D   -- unassigned --
	Key_unknown,    // 158   0x9E   -- unassigned --
	Key_unknown,    // 159   0x9F   -- unassigned --
	Key_Shift,      // 160   0xA0   VK_LSHIFT           | Left Shift key
	Key_Shift,      // 161   0xA1   VK_RSHIFT           | Right Shift key
	Key_Control,    // 162   0xA2   VK_LCONTROL         | Left Ctrl key
	Key_Control,    // 163   0xA3   VK_RCONTROL         | Right Ctrl key
	Key_Alt,        // 164   0xA4   VK_LMENU            | Left Menu key
	Key_Alt,        // 165   0xA5   VK_RMENU            | Right Menu key
	Key_Back,       // 166   0xA6   VK_BROWSER_BACK     | Browser Back key
	Key_Forward,    // 167   0xA7   VK_BROWSER_FORWARD  | Browser Forward key
	Key_Refresh,    // 168   0xA8   VK_BROWSER_REFRESH  | Browser Refresh key
	Key_Stop,       // 169   0xA9   VK_BROWSER_STOP     | Browser Stop key
	Key_Search,     // 170   0xAA   VK_BROWSER_SEARCH   | Browser Search key
	Key_Favorites,  // 171   0xAB   VK_BROWSER_FAVORITES| Browser Favorites key
	Key_HomePage,   // 172   0xAC   VK_BROWSER_HOME     | Browser Start and Home key
	Key_VolumeMute, // 173   0xAD   VK_VOLUME_MUTE      | Volume Mute key
	Key_VolumeDown, // 174   0xAE   VK_VOLUME_DOWN      | Volume Down key
	Key_VolumeUp,   // 175   0xAF   VK_VOLUME_UP        | Volume Up key
	Key_MediaNext,  // 176   0xB0   VK_MEDIA_NEXT_TRACK | Next Track key
	Key_MediaPrevious, //177 0xB1   VK_MEDIA_PREV_TRACK | Previous Track key
	Key_MediaStop,  // 178   0xB2   VK_MEDIA_STOP       | Stop Media key
	Key_MediaPlay,  // 179   0xB3   VK_MEDIA_PLAY_PAUSE | Play/Pause Media key
	Key_LaunchMail, // 180   0xB4   VK_LAUNCH_MAIL      | Start Mail key
	Key_LaunchMedia,// 181   0xB5   VK_LAUNCH_MEDIA_SELECT Select Media key
	Key_Launch0,    // 182   0xB6   VK_LAUNCH_APP1      | Start Application 1 key
	Key_Launch1,    // 183   0xB7   VK_LAUNCH_APP2      | Start Application 2 key
	Key_unknown,    // 184   0xB8   -- reserved --
	Key_unknown,    // 185   0xB9   -- reserved --
	0,                  // 186   0xBA   VK_OEM_1            | ';:' for US
	0,                  // 187   0xBB   VK_OEM_PLUS         | '+' any country
	0,                  // 188   0xBC   VK_OEM_COMMA        | ',' any country
	0,                  // 189   0xBD   VK_OEM_MINUS        | '-' any country
	0,                  // 190   0xBE   VK_OEM_PERIOD       | '.' any country
	0,                  // 191   0xBF   VK_OEM_2            | '/?' for US
	0,                  // 192   0xC0   VK_OEM_3            | '`~' for US
	Key_unknown,    // 193   0xC1   -- reserved --
	Key_unknown,    // 194   0xC2   -- reserved --
	Key_unknown,    // 195   0xC3   -- reserved --
	Key_unknown,    // 196   0xC4   -- reserved --
	Key_unknown,    // 197   0xC5   -- reserved --
	Key_unknown,    // 198   0xC6   -- reserved --
	Key_unknown,    // 199   0xC7   -- reserved --
	Key_unknown,    // 200   0xC8   -- reserved --
	Key_unknown,    // 201   0xC9   -- reserved --
	Key_unknown,    // 202   0xCA   -- reserved --
	Key_unknown,    // 203   0xCB   -- reserved --
	Key_unknown,    // 204   0xCC   -- reserved --
	Key_unknown,    // 205   0xCD   -- reserved --
	Key_unknown,    // 206   0xCE   -- reserved --
	Key_unknown,    // 207   0xCF   -- reserved --
	Key_unknown,    // 208   0xD0   -- reserved --
	Key_unknown,    // 209   0xD1   -- reserved --
	Key_unknown,    // 210   0xD2   -- reserved --
	Key_unknown,    // 211   0xD3   -- reserved --
	Key_unknown,    // 212   0xD4   -- reserved --
	Key_unknown,    // 213   0xD5   -- reserved --
	Key_unknown,    // 214   0xD6   -- reserved --
	Key_unknown,    // 215   0xD7   -- reserved --
	Key_unknown,    // 216   0xD8   -- unassigned --
	Key_unknown,    // 217   0xD9   -- unassigned --
	Key_unknown,    // 218   0xDA   -- unassigned --
	0,                  // 219   0xDB   VK_OEM_4            | '[{' for US
	0,                  // 220   0xDC   VK_OEM_5            | '\|' for US
	0,                  // 221   0xDD   VK_OEM_6            | ']}' for US
	0,                  // 222   0xDE   VK_OEM_7            | ''"' for US
	0,                  // 223   0xDF   VK_OEM_8
	Key_unknown,    // 224   0xE0   -- reserved --
	Key_unknown,    // 225   0xE1   VK_OEM_AX           | 'AX' key on Japanese AX kbd
	Key_unknown,    // 226   0xE2   VK_OEM_102          | "<>" or "\|" on RT 102-key kbd
	Key_unknown,    // 227   0xE3   VK_ICO_HELP         | Help key on ICO
	Key_unknown,    // 228   0xE4   VK_ICO_00           | 00 key on ICO
	Key_unknown,    // 229   0xE5   VK_PROCESSKEY       | IME Process key
	Key_unknown,    // 230   0xE6   VK_ICO_CLEAR        |
	Key_unknown,    // 231   0xE7   VK_PACKET           | Unicode char as keystrokes
	Key_unknown,    // 232   0xE8   -- unassigned --
	// Nokia/Ericsson definitions ---------------
	Key_unknown,    // 233   0xE9   VK_OEM_RESET
	Key_unknown,    // 234   0xEA   VK_OEM_JUMP
	Key_unknown,    // 235   0xEB   VK_OEM_PA1
	Key_unknown,    // 236   0xEC   VK_OEM_PA2
	Key_unknown,    // 237   0xED   VK_OEM_PA3
	Key_unknown,    // 238   0xEE   VK_OEM_WSCTRL
	Key_unknown,    // 239   0xEF   VK_OEM_CUSEL
	Key_unknown,    // 240   0xF0   VK_OEM_ATTN
	Key_unknown,    // 241   0xF1   VK_OEM_FINISH
	Key_unknown,    // 242   0xF2   VK_OEM_COPY
	Key_unknown,    // 243   0xF3   VK_OEM_AUTO
	Key_unknown,    // 244   0xF4   VK_OEM_ENLW
	Key_unknown,    // 245   0xF5   VK_OEM_BACKTAB
	Key_unknown,    // 246   0xF6   VK_ATTN             | Attn key
	Key_unknown,    // 247   0xF7   VK_CRSEL            | CrSel key
	Key_unknown,    // 248   0xF8   VK_EXSEL            | ExSel key
	Key_unknown,    // 249   0xF9   VK_EREOF            | Erase EOF key
	Key_Play,       // 250   0xFA   VK_PLAY             | Play key
	Key_Zoom,       // 251   0xFB   VK_ZOOM             | Zoom key
	Key_unknown,    // 252   0xFC   VK_NONAME           | Reserved
	Key_unknown,    // 253   0xFD   VK_PA1              | PA1 key
	Key_Clear,      // 254   0xFE   VK_OEM_CLEAR        | Clear key
	0
};

