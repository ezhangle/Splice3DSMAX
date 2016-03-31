//
// DockableWindow allows hosting QT content in a Win32 frame
// that can be docked within the Max docking system
//

class QWidget;

class DockableWindow
	: public CUIPosData, public CUIFrameMsgHandler
{
	HWND h;
	ICUIFrame* frame;
	QWidget* w;
	CUIPosData posData;
	FabricTranslationFPInterface* m_owner;

	DockableWindow(HWND hwndCuiFrame, FabricTranslationFPInterface* owner);

	void ResizeFrameToContent();

	void ResizeContentToFrame();

	virtual int GetWidth(int sizeType, int orient);

	virtual int GetHeight(int sizeType, int orient);

public:

	enum DockFlags {
		None = 0,
		Top = 1 << 0,
		Bottom = 1 << 1,
		Left = 1 << 2,
		Right = 1 << 3,
		Horizontal = Left | Right,
		Vertical = Top | Bottom,
		All = Horizontal | Vertical,
	};

	virtual ~DockableWindow();

	static DockableWindow* Create(MCHAR* name, 
								   FabricTranslationFPInterface* owner, 
								   DockFlags pos = None,
								   DWORD initialPos = 0,
								   bool isDockedInitially = false, 
								   bool resizable = true,
								   bool visible = true);

	virtual int ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam);

	bool HasWidget();

	QWidget* GetWidget();

	void SetWidget(QWidget* w);

	void Dock(DWORD location);

	void Float();

	HWND GetHWND();

	ICUIFrame* GetICUIFrame();
};

extern void AcquireQt();
extern void ReleaseQt();