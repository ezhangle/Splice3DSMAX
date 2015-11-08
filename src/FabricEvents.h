#pragma once
#include "MouseCallbacks\SpliceCommandMode.h"

class SpliceViewportDrawCallback;


#define MAX_VPTS	4			// max 3D viewports
class SpliceEvents 
{
private:
	SpliceEvents();
	~SpliceEvents();

	SpliceViewportDrawCallback* m_VptCallback;
	static SpliceEvents* s_Instance;

	SpliceCommandMode m_MouseCommandMode;

public:

	// This trio of functions to pipe mouse/kb events down to the splice layer
	void HookMouseEvents();
	void UnHookMouseEvents();
	bool MouseEventsHooked();

	// Call this to allow Splice to render to the (OGL only) viewport
	void HookViewportRender();
	void UnHookViewportRender();
	bool ViewportRenderHooked();
	void TriggerRedraw();

	static SpliceEvents* GetInstance();
	static void ReleaseInstance();
	
	//static FabricCore::RTVal s_DrawContexts[MAX_VPTS];
	static FabricCore::RTVal s_DrawContext;
	static FabricCore::RTVal s_InlineViewports[MAX_VPTS];

};