#pragma once
#include "MouseCallbacks\FabricCommandMode.h"

class FabricViewportDrawCallback;


#define MAX_VPTS	4			// max 3D viewports
class FabricEvents 
{
private:
	FabricEvents();
	~FabricEvents();

	FabricViewportDrawCallback* m_VptCallback;
	static FabricEvents* s_Instance;

	FabricCommandMode m_MouseCommandMode;

public:

	// This trio of functions to pipe mouse/kb events down to the splice layer
	void HookMouseEvents();
	void UnHookMouseEvents();
	bool MouseEventsHooked();

	// Call this to allow Fabric to render to the (OGL only) viewport
	void HookViewportRender();
	void UnHookViewportRender();
	bool ViewportRenderHooked();
	void TriggerRedraw();

	static FabricEvents* GetInstance();
	static void ReleaseInstance();
	
	//static FabricCore::RTVal s_DrawContexts[MAX_VPTS];
	static FabricCore::RTVal s_DrawContext;
	static FabricCore::RTVal s_InlineViewports[MAX_VPTS];

};
