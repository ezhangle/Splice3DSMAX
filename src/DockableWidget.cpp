#include "StdAfx.h"
#include "DockableWidget.h"
#include <QtGui/qwidget.h>

DockableWindow::DockableWindow(HWND hwndCuiFrame) : h(hwndCuiFrame), w(NULL), frame(NULL)
{
	frame = ::GetICUIFrame(h);
	frame->InstallMsgHandler(this);
}

DockableWindow::~DockableWindow()
{
	::ReleaseICUIFrame(frame);
	delete w;
}


void DockableWindow::ResizeFrameToContent()
{
	RECT r = { 0 };
	if (w == NULL) return;
	GetWindowRect(h, &r);
	MoveWindow(h, r.left, r.top, w->width(), w->height(), TRUE);
}

void DockableWindow::ResizeContentToFrame()
{
	if (w == NULL) return;
	RECT r = { 0 };
	GetWindowRect(h, &r);
	int width = r.right - r.left;
	int height = r.bottom - r.top;
	w->resize(width, height);
}

int DockableWindow::GetWidth(int sizeType, int orient)
{
	switch (sizeType) {
	case CUI_MIN_SIZE: return 50;
	case CUI_MAX_SIZE: return 10000;
	}
	return w->width();
}

int DockableWindow::GetHeight(int sizeType, int orient)
{
	switch (sizeType) {
	case CUI_MIN_SIZE: return 50;
	case CUI_MAX_SIZE: return 10000;
	}
	return w->height();
}

DockableWindow* DockableWindow::Create(MCHAR* name, DockFlags pos /*= All*/, DWORD initialPos /*= 0*/, bool isDockedInitially /*= false*/, bool resizable /*= true*/, bool visible /*= true*/)
{
	HWND h = ::CreateCUIFrameWindow(
		::GetCOREInterface()->GetMAXHWnd(), name, 0, 0, 0, 0);
	if (!h) return NULL;
	DockableWindow* r = new DockableWindow(h);
	ICUIFrame* f = r->GetICUIFrame();
	DWORD flags = pos | CUI_FLOATABLE;
	if (resizable) flags |= CUI_SM_HANDLES;
	f->SetPosType(flags);
	if (isDockedInitially) r->Dock(initialPos);
	f->Hide(visible ? FALSE : TRUE);
	return r;
}

int DockableWindow::ProcessMessage(UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case CUI_POSDATA_MSG: {
		CUIPosData **cpd = (CUIPosData **)lParam;
		cpd[0] = this;
		return TRUE;
	}
	case WM_SIZING:
		ResizeContentToFrame();
		return FALSE;
	}
	return FALSE;
}

bool DockableWindow::HasWidget()
{
	return GetWidget() != NULL;
}

QWidget* DockableWindow::GetWidget()
{
	return w;
}

void DockableWindow::SetWidget(QWidget* w)
{
	delete(this->w);
	this->w = w;
	if (w == NULL) return;
	w->move(0, 0);
	ResizeFrameToContent();
	w->show();
}

void DockableWindow::Dock(DWORD location)
{
	if (location & Top) GetCUIFrameMgr()->DockCUIWindow(h, Top);
	else if (location & Bottom) GetCUIFrameMgr()->DockCUIWindow(h, Bottom);
	else if (location & Left) GetCUIFrameMgr()->DockCUIWindow(h, Left);
	else if (location & Right) GetCUIFrameMgr()->DockCUIWindow(h, Right);
}

void DockableWindow::Float()
{
	GetCUIFrameMgr()->FloatCUIWindow(h);
}

HWND DockableWindow::GetHWND()
{
	return h;
}

ICUIFrame* DockableWindow::GetICUIFrame()
{
	return frame;
}
